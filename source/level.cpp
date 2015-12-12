#include <iostream>
#include "level.h"
#include "_math.h"
#include "input.h"

#define orientationToAngle(o) ((o) * M_PI / 2)

cubeType_s cubeTypes[16] =
{
	(cubeType_s){glm::vec3(0.0f)},
	(cubeType_s){glm::vec3(1.0f, 1.0f, 1.0f)},
	(cubeType_s){glm::vec3(66.0f, 130.0f, 255.0f) * (1.0f / 255)},
	(cubeType_s){glm::vec3(255.0f, 59.0f, 24.0f) * (1.0f / 255)},
	(cubeType_s){glm::vec3(147.0f, 37.0f, 255.0f) * (1.0f / 255)},
};

SliceCollection::SliceCollection():
	cubes(SC_NUMCUBES, 0),
	cubes_wireframe(SC_NUMCUBES, 0, true),
	base_depth(0),
	depth(0),
	orientation(0),
	angle(0.0f)
{

}

int SliceCollection::getNumSlices()
{
	return data.size();
}

int SliceCollection::getBaseDepth()
{
	return base_depth;
}

void SliceCollection::incrementBaseDepth(int v)
{
	base_depth += v;
}

float getTiling(glm::vec3 position)
{
	int i = int(position.x);
	int j = int(position.y);
	int k = int(position.z);

	return ((i + j + k) % 2) ? 1.0f : 0.985f;
}


void SliceCollection::addSlice(slice_s s)
{
	for(int i = 0; i < LEVEL_WIDTH; i++)
	{
		for(int j = 0; j < LEVEL_WIDTH; j++)
		{
			glm::vec3 p = glm::vec3(j * 1.0f, (LEVEL_WIDTH - 1 - i) * 1.0f, (base_depth + depth) * 1.0f);
			unsigned char v = getCubeFullId(s.data[i][j]);

			if(v)
			{
				cubes.addCube(p, cubeTypes[v].color * getTiling(p));
			}

			v = getCubeWireframeId(s.data[i][j]);

			if(v)
			{
				cubes_wireframe.addCube(p, cubeTypes[v].color);
			}
		}
	}

	depth++;
	data.push_back(s);
	cubes.update();
	cubes_wireframe.update();
}

void SliceCollection::popSlice()
{
	int slice_depth = depth - data.size();

	cubes.removeDepth(float(slice_depth));
	cubes_wireframe.removeDepth(float(slice_depth));

	data.pop_front();
}

unsigned char mergeFilledCubes(unsigned char c1, unsigned char c2)
{
	return c1 ? c1 : c2;
}

unsigned char mergeWireframeCubes(unsigned char c1, unsigned char c3)
{
	unsigned char c2 = c3;
	if(c1 > c2)
	{
		c2 = c1;
		c1 = c3;
	}

	if(!c1) return c2;
	if(c1 == c2) return c1;

	if(c1 == 2 && c2 == 3) return 4;

	return c1;
}

unsigned char mergeCubes(unsigned char c1, unsigned char c2)
{
	return (mergeWireframeCubes(getCubeWireframeId(c1), getCubeWireframeId(c2)) << 4) | mergeFilledCubes(getCubeFullId(c1), getCubeFullId(c2));
}

void initSlice(slice_s* s)
{
	if(!s) return;

	memset(s, 0, sizeof(slice_s));
}

void rotateSlices(slice_s* dst, slice_s* src, int orientation)
{
	orientation %= 4;
	if(!dst || !src) return;

	switch(orientation)
	{
		case 0:
			*dst = *src;
			break;
		case 1:
			for(int i = 0; i < LEVEL_WIDTH; i++)
			{
				for(int j = 0; j < LEVEL_WIDTH; j++)
				{
					dst->data[LEVEL_WIDTH - 1 - j][i] = src->data[i][j];
				}
			}
			break;
		case 2:
			for(int i = 0; i < LEVEL_WIDTH; i++)
			{
				for(int j = 0; j < LEVEL_WIDTH; j++)
				{
					dst->data[LEVEL_WIDTH - 1 - i][LEVEL_WIDTH - 1 - j] = src->data[i][j];
				}
			}
			break;
		case 3:
			for(int i = 0; i < LEVEL_WIDTH; i++)
			{
				for(int j = 0; j < LEVEL_WIDTH; j++)
				{
					dst->data[j][LEVEL_WIDTH - 1 - i] = src->data[i][j];
				}
			}
			break;
	}
}

void mergeSlices(slice_s* s1, slice_s* s2, int orientation)
{
	if(!s1 || !s2) return;

	slice_s s3;
	rotateSlices(&s3, s2, orientation);

	for(int i = 0; i < LEVEL_WIDTH; i++)
	{
		for(int j = 0; j < LEVEL_WIDTH; j++)
		{
			s1->data[i][j] = mergeCubes(s1->data[i][j], s3.data[i][j]);
		}
	}
}

void SliceCollection::clear()
{
	data.clear();
	cubes.clear();
	cubes_wireframe.clear();
	depth = 0;
}

void SliceCollection::mergeLayers(SliceCollection** sc, int n)
{
	if(!sc) return;

	clear();

	std::deque<slice_s>::iterator its[n];

	for(int i = 0; i < n; i++)
	{
		its[i] = sc[i]->data.begin();
	}

	while(true)
	{
		slice_s dst_slice;

		initSlice(&dst_slice);

		bool done = true;
		for(int i = 0; i < n; i++)
		{
			if(its[i] != sc[i]->data.end())
			{
				mergeSlices(&dst_slice, &*its[i], sc[i]->orientation);
				++its[i];
				done = false;
			}
		}
		if(done) break;

		addSlice(dst_slice);
	}
}

unsigned char SliceCollection::getCubeInfo(glm::ivec3 p)
{
	if(p.z < 0 || p.z >= (int)data.size()) return 0;
	if(p.x < 0 || p.x >= LEVEL_WIDTH) return 0;
	if(p.y < 0 || p.y >= LEVEL_WIDTH) return 0;

	return data[p.z].data[LEVEL_WIDTH - 1 - p.y][p.x];
}

void SliceCollection::draw(Camera& camera, Lighting& lighting, bool wireframe)
{
	if(!wireframe)
	{
		cubes.model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-LEVEL_WIDTH / 2, -LEVEL_WIDTH / 2, 0.0f));
		cubes.draw(camera, lighting);
	}else{
		cubes_wireframe.model = cubes.model;
		cubes_wireframe.draw(camera, lighting);
	}
}

void SliceCollection::setAngle(float angle)
{
	this->angle = angle;
}

void SliceCollection::setOrientation(int orientation)
{
	this->orientation = orientation;
}

Layer::Layer():
	state(LAYER_IDLE),
	angle(0.0f),
	orientation(0.0f)
{

}

void Layer::rotate()
{
	state = LAYER_ROTATING;
}

bool Layer::update(float delta)
{
	bool ret = false;

	switch(state)
	{
		case LAYER_IDLE:
			break;
		case LAYER_ROTATING:
			{
				int next_orientation = orientation + 1;
				float target = orientationToAngle(next_orientation);

				if(angle >= target)
				{
					orientation++;

					orientation %= 4;
					angle = orientationToAngle(orientation);

					state = LAYER_IDLE;

					ret = true;
				}else{
					if(next_orientation > orientation) angle += 2.0f * delta;
					else angle -= 2.0f * delta;
				}
			}
			break;
	}

	slices.setOrientation(orientation);

	return ret;
}

void Layer::draw(Camera& camera, Lighting& lighting, bool wireframe)
{
	slices.setAngle(angle);
	slices.draw(camera, lighting, wireframe);
}

void Layer::addSlice(slice_s s)
{
	slices.addSlice(s);
}

void Layer::popSlice()
{
	slices.popSlice();
}

Level::Level(LevelGenerator& lg):
	generator(lg)
{

}

void Level::draw(Camera& camera, Lighting& lighting, bool wireframe)
{
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		if(layers[i].state == LAYER_ROTATING)
		{
			layers[i].draw(camera, lighting, wireframe);
		}
	}

	slices.draw(camera, lighting, wireframe);
}

void Level::update(float delta)
{
	bool needUpdate = false;
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		needUpdate = layers[i].update(delta) || needUpdate;
		while(layers[i].slices.getNumSlices() < LEVEL_NUMSLICES)
		{
			addSliceLayer(i, generator.getSlice(i), false);
			needUpdate = true;
		}
	}

	// TEMP
	if(Input::isKeyPressed(GLFW_KEY_P))
	{
		popSlice(false);
		needUpdate = true;
	}

	if(needUpdate)
	{
		updateGeometry();
	}
}

void Level::rotateLayer(int l)
{
	if(l < 0 || l >= LEVEL_NUMLAYERS) return;

	layers[l].rotate();

	updateGeometry();
}

void Level::updateGeometry()
{
	SliceCollection* sc[LEVEL_NUMLAYERS];

	int num = 0;

	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		if(layers[i].state != LAYER_ROTATING)
		{
			sc[num] = &layers[i].slices;
			num++;
		}
	}

	slices.mergeLayers(sc, num);
}

void Level::addSliceLayer(int l, slice_s s, bool update)
{
	if(l < 0 || l >= LEVEL_NUMLAYERS) return;

	layers[l].addSlice(s);

	if(update) this->updateGeometry();
}

void Level::popSlice(bool update)
{
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		layers[i].popSlice();
	}

	slices.incrementBaseDepth();

	if(update) this->updateGeometry();
}

int Level::getOffset()
{
	return slices.getBaseDepth();
}

unsigned char Level::getCubeInfo(glm::vec3 p)
{
	glm::ivec3 _p(p);

	_p.x += LEVEL_WIDTH / 2;
	_p.y += LEVEL_WIDTH / 2;
	_p.z -= getOffset();

	// std::cout << glm::to_string(_p) << std::endl;

	return slices.getCubeInfo(_p);
}
