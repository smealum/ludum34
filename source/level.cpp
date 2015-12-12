#include "level.h"
#include "_math.h"

#define orientationToAngle(o) ((o) * M_PI / 2)

SliceCollection::SliceCollection():
	cubes(SC_NUMCUBES, 0),
	depth(0),
	orientation(0),
	angle(0.0f)
{

}

void SliceCollection::addSlice(slice_s s)
{
	for(int i = 0; i < LEVEL_WIDTH; i++)
	{
		for(int j = 0; j < LEVEL_WIDTH; j++)
		{
			if(s.data[i][j])
			{
				cubes.addCube(glm::vec3(i * 1.0f, j * 1.0f, depth * 1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
			}
		}
	}

	depth++;
	data.push_back(s);
	cubes.update();
}

void SliceCollection::popSlice()
{
	int slice_depth = depth - data.size();

	cubes.removeDepth(float(slice_depth));

	data.pop_front();
}

unsigned char mergeCubes(unsigned char c1, unsigned char c2)
{
	return c1 ? c1 : c2;
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

void SliceCollection::draw(Camera& camera, Lighting& lighting)
{
	cubes.model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-LEVEL_WIDTH / 2, -LEVEL_WIDTH / 2, 0.0f));
	cubes.draw(camera, lighting);
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

void Layer::draw(Camera& camera, Lighting& lighting)
{
	slices.setAngle(angle);
	slices.draw(camera, lighting);
}

void Layer::addSlice(slice_s s)
{
	slices.addSlice(s);
}

Level::Level()
{

}

void Level::draw(Camera& camera, Lighting& lighting)
{
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		if(layers[i].state == LAYER_ROTATING)
		{
			layers[i].draw(camera, lighting);
		}
	}

	slices.draw(camera, lighting);
}

void Level::update(float delta)
{
	bool needUpdate = false;
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		needUpdate = layers[i].update(delta) || needUpdate; 
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
}

void Level::updateGeometry()
{
	SliceCollection* sc[LEVEL_NUMLAYERS];

	for(int i = 0; i < LEVEL_NUMLAYERS; i++) sc[i] = &layers[i].slices;

	slices.mergeLayers(sc, LEVEL_NUMLAYERS);
}

void Level::addSliceLayer(int l, slice_s s, bool update)
{
	if(l < 0 || l >= LEVEL_NUMLAYERS) return;

	layers[l].addSlice(s);

	if(update) this->updateGeometry();
}
