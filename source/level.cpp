#include <iostream>
#include "level.h"
#include "_math.h"
#include "input.h"

#define orientationToAngle(o) ((o) * M_PI / 2)

cubeType_s cubeTypes[16] =
{
	// EMPTY
	(cubeType_s){glm::vec3(0.9f, 0.9f, 0.9f),},
	// DEFAULT
	(cubeType_s){
		glm::vec3(1.0f, 1.0f, 1.0f),
		CUBEPROPERTY_DISCRIMINATE(0) | CUBEPROPERTY_CLIMBABLE(0) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD),
		CUBEPROPERTY_CLIMBABLE(0) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD),
	},
	// BLUE
	(cubeType_s){
		glm::vec3(66.0f, 130.0f, 255.0f) * (1.0f / 255),
		CUBEPROPERTY_DISCRIMINATE(1) | CUBEPROPERTY_CLIMBABLE(0) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD),
		CUBEPROPERTY_CLIMBABLE(0) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT),
	},
	// RED
	(cubeType_s){
		glm::vec3(255.0f, 59.0f, 24.0f) * (1.0f / 255),
		CUBEPROPERTY_DISCRIMINATE(1) | CUBEPROPERTY_CLIMBABLE(0) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD),
		CUBEPROPERTY_CLIMBABLE(0) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT),
	},
	// PURPLE
	(cubeType_s){
		glm::vec3(147.0f, 37.0f, 255.0f) * (1.0f / 255),
		CUBEPROPERTY_DISCRIMINATE(1) | CUBEPROPERTY_CLIMBABLE(1) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD),
		CUBEPROPERTY_CLIMBABLE(1) | CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD),
	},
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

	return ((i + j + k) % 2) ? 1.0f : 0.97f;
}

void addCube(Cubes& _cubes, Cubes& _cubes_wireframe, glm::vec3 p, unsigned char info, bool falling = false)
{
	unsigned char v1 = getCubeFullId(info);

	if(v1)
	{
		int id = _cubes.addCube(p, cubeTypes[v1].color * getTiling(p));
		if(falling) _cubes.setFalling(id);
	}

	unsigned char v2 = getCubeWireframeId(info);

	if(v2 || v1)
	{
		int id = _cubes_wireframe.addCube(p, cubeTypes[v2].color);
		if(falling) _cubes_wireframe.setFalling(id);
	}
}

void SliceCollection::addSlice(slice_s s)
{
	for(int i = 0; i < LEVEL_WIDTH; i++)
	{
		for(int j = 0; j < LEVEL_WIDTH; j++)
		{
			glm::vec3 p = glm::vec3(j * 1.0f, (LEVEL_WIDTH - 1 - i) * 1.0f, (base_depth + depth) * 1.0f);

			addCube(cubes, cubes_wireframe, p, s.data[i][j]);
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

	cubes.removeDepth(float(slice_depth), true);
	cubes_wireframe.removeDepth(float(slice_depth), true);

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

static inline glm::ivec2 rotateVector(glm::ivec2 v, int orientation)
{
	orientation %= 4;

	switch(orientation)
	{
		case 1:
			return glm::ivec2(LEVEL_WIDTH - 1 - v.y, v.x);
		case 2:
			return glm::ivec2(LEVEL_WIDTH - 1 - v.x, LEVEL_WIDTH - 1 - v.y);
		case 3:
			return glm::ivec2(v.y, LEVEL_WIDTH - 1 - v.x);
		default:
			return v;
	}
}

void rotateSlice(slice_s* dst, slice_s* src, int orientation)
{
	orientation %= 4;
	if(!dst || !src) return;

	if(orientation == 0) *dst = *src;

	for(int i = 0; i < LEVEL_WIDTH; i++)
	{
		for(int j = 0; j < LEVEL_WIDTH; j++)
		{
			glm::ivec2 r = rotateVector(glm::ivec2(i, j), orientation);
			dst->data[r.x][r.y] = src->data[i][j];
		}
	}
}

void mergeSlices(slice_s* s1, slice_s* s2, int orientation)
{
	if(!s1 || !s2) return;

	slice_s s3;
	rotateSlice(&s3, s2, orientation);

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

void SliceCollection::reset()
{
	clear();
	base_depth = 0;
	orientation = 0;
	angle = 0.0f;
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

unsigned char SliceCollection::getCubeInfo(glm::ivec3 p, bool* out_of_bounds)
{
	if(p.z < 0 || p.z >= (int)data.size()) {if(out_of_bounds) *out_of_bounds = true; return 0;}
	if(p.x < 0 || p.x >= LEVEL_WIDTH) {if(out_of_bounds) *out_of_bounds = true; return 0;}
	if(p.y < 0 || p.y >= LEVEL_WIDTH) {if(out_of_bounds) *out_of_bounds = true; return 0;}

	if(out_of_bounds) *out_of_bounds = false;

	if(orientation == 0) return data[p.z].data[LEVEL_WIDTH - 1 - p.y][p.x];
	else{
		slice_s rotated;

		rotateSlice(&rotated, &data[p.z], orientation);

		return rotated.data[LEVEL_WIDTH - 1 - p.y][p.x];
	}
}

void SliceCollection::removeCube(glm::ivec3 p)
{
	if(p.z < 0 || p.z >= (int)data.size()) return;
	if(p.x < 0 || p.x >= LEVEL_WIDTH) return;
	if(p.y < 0 || p.y >= LEVEL_WIDTH) return;

	if(orientation == 0)
	{
		data[p.z].data[LEVEL_WIDTH - 1 - p.y][p.x] = 0;

		cubes.removeCube(glm::vec3(p), true);
		cubes_wireframe.removeCube(glm::vec3(p), true);
	}
	else{
		glm::ivec2 r = rotateVector(glm::ivec2(p.x, p.y), 4 - orientation);

		data[p.z].data[LEVEL_WIDTH - 1 - r.y][r.x] = 0;

		cubes.removeCube(glm::vec3(r.x, r.y, p.z), true);
		cubes_wireframe.removeCube(glm::vec3(r.x, r.y, p.z), true);
	}
}

cubeProperties_t _getCubeProperties(unsigned char cube_info)
{
	unsigned char cubeFullId = getCubeFullId(cube_info);
	unsigned char cubeWireframeId = getCubeWireframeId(cube_info);

	return (CUBEPROPERTY_COLOR(cube_info) | cubeTypes[cubeFullId].full_properties | cubeTypes[cubeWireframeId].wireframe_properties) & ~(CUBEPROPERTY_CLIMBABLE(cubeFullId == 0));
}

cubeProperties_t SliceCollection::getCubeProperties(glm::ivec3 p)
{
	bool out_of_bounds = false;
	unsigned char cube_info = getCubeInfo(p, &out_of_bounds);
	return _getCubeProperties(cube_info) | CUBEPROPERTY_OUTOFBOUNDS(out_of_bounds);
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
	queued_rotation(false),
	angle(0.0f),
	orientation(0.0f)
{

}

void Layer::clear()
{
	slices.reset();
	state = LAYER_IDLE;
	queued_rotation = false;
	angle = 0.0f;
	orientation = 0;
}

bool Layer::isCube(glm::vec3 p)
{
	return getCubeFullId(slices.getCubeInfo(p)) != 0;
}

void Layer::rotate()
{
	if(state == LAYER_ROTATING) queued_rotation = true;
	else state = LAYER_ROTATING;
}

bool Layer::update(float delta)
{
	bool ret = false;

	switch(state)
	{
		case LAYER_IDLE:
			if(queued_rotation)
			{
				queued_rotation = false;
				rotate();
				ret = true;
			}
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

void Layer::removeCube(glm::ivec3 p)
{
	slices.removeCube(p);
}

Level::Level(LevelGenerator& lg):
	state(LEVEL_INTRO_START),
	deadcubes(SC_NUMCUBES, 0),
	deadcubes_wireframe(SC_NUMCUBES, 0, true),
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

	// printf("dead %d\n", deadcubes.current_n);

	if(!wireframe)
	{
		// deadcubes.model = glm::translate(glm::mat4(1.0f), glm::vec3(-LEVEL_WIDTH / 2, -LEVEL_WIDTH / 2, 0.0f));
		deadcubes.model = glm::translate(glm::mat4(1.0f), glm::vec3(-LEVEL_WIDTH / 2, -LEVEL_WIDTH / 2, 0.0f));
		deadcubes.draw(camera, lighting);
	}else{
		deadcubes_wireframe.model = deadcubes.model;
		deadcubes_wireframe.draw(camera, lighting);
	}
}

bool Level::update(float delta)
{
	bool needUpdate = false;
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		needUpdate = layers[i].update(delta) || needUpdate;
		while(layers[i].slices.getNumSlices() < LEVEL_NUMSLICES)
		{
			slice_s out;
			bool ret = generator.getSlice(i, out);
			if(!ret) break;
			addSliceLayer(i, out, false);
			needUpdate = true;
		}
	}

	if(needUpdate)
	{
		updateGeometry();
	}

	switch(state)
	{
		case LEVEL_INTRO_START:
			setIntro();
			state = LEVEL_INTRO;
			break;
		case LEVEL_INTRO:
			break;
		default:
			break;
	}

	return needUpdate;
}

void Level::finishIntro()
{
	if(state == LEVEL_INTRO)
	{
		state = LEVEL_PLAYING;
		clearIntro();
	}
}

void Level::startOutro()
{
	if(state != LEVEL_OUTRO)
	{
		state = LEVEL_OUTRO;
		setOutro();
	}
}

void Level::setOutro()
{
	for(int i = 0; i < SC_NUMCUBES; i++)
	{
		slices.cubes.setOutro(i, true);
		slices.cubes_wireframe.setOutro(i, true);
	}
}


void Level::killCube(glm::vec3 p)
{
	unsigned char cube_info = getCubeInfo(p);

	if(cube_info != 0)
	{
		glm::ivec3 _p(p);

		_p.x += LEVEL_WIDTH / 2;
		_p.y += LEVEL_WIDTH / 2;

		addCube(deadcubes, deadcubes_wireframe, _p, cube_info, true);
		
		_p.z -= getOffset();

		deadcubes.update();
		deadcubes_wireframe.update();

		slices.removeCube(_p);

		for(int i = 0; i < LEVEL_NUMLAYERS; i++)
		{
			layers[i].removeCube(_p);
		}
	}
}

void Level::rotateLayer(int l)
{
	if(l < 0 || l >= LEVEL_NUMLAYERS || state != LEVEL_PLAYING) return;

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

	int slice_depth = getOffset();

	deadcubes.removeDepth(float(slice_depth), true);
	deadcubes_wireframe.removeDepth(float(slice_depth), true);

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

cubeProperties_t Level::getCubeProperties(glm::vec3 p)
{
	glm::ivec3 _p(p);

	_p.x += LEVEL_WIDTH / 2;
	_p.y += LEVEL_WIDTH / 2;
	_p.z -= getOffset();

	// std::cout << glm::to_string(_p) << std::endl;

	return slices.getCubeProperties(_p);
}

glm::vec3 cubeDirections[] =
{
	glm::vec3(0.0f, 0.0f, 1.0f), // CUBEPROPERTY_DIRECTION_FORWARD = 0,
	glm::vec3(0.0f, 0.0f, -1.0f), // CUBEPROPERTY_DIRECTION_BACKWARD = 1,
	glm::vec3(1.0f, 0.0f, 0.0f), // CUBEPROPERTY_DIRECTION_LEFT = 2,
	glm::vec3(-1.0f, 0.0f, 0.0f), // CUBEPROPERTY_DIRECTION_RIGHT = 3,
};

bool Level::getNextLocation(glm::vec3 p, glm::vec3& out)
{
	cubeProperties_t bottom = getCubeProperties(p + glm::vec3(0.0f, -1.0f, 0.0f));

	// std::cout << glm::to_string(p) << " " << (int)bottom << std::endl;

	glm::vec3 ret = p;

	if(!CUBEPROPERTY_IS_EMPTY(bottom))
	{
		cubePropertyDirection_t direction = CUBEPROPERTY_GET_DIRECTION(bottom);

		ret += cubeDirections[direction];
	}

	cubeProperties_t next = getCubeProperties(ret);

	if(CUBEPROPERTY_IS_EMPTY(next))
	{
		bottom = getCubeProperties(ret + glm::vec3(0.0f, -1.0f, 0.0f));

		if(CUBEPROPERTY_IS_EMPTY(bottom))
		{
			while(CUBEPROPERTY_IS_EMPTY(bottom) && !CUBEPROPERTY_IS_OUTOFBOUNDS(bottom))
			{
				ret.y -= 1.0f;
				bottom = getCubeProperties(ret + glm::vec3(0.0f, -1.0f, 0.0f));
			}
			out = ret;
			return false;
		}

		out = ret;
		return false;
	}

	while(CUBEPROPERTY_GET_CLIMBABLE(getCubeProperties(ret))) ret += glm::vec3(0.0f, 1.0f, 0.0f);

	if(CUBEPROPERTY_IS_EMPTY(getCubeProperties(ret)))
	{
		out = ret;
		return false;
	}

	out = p;
	return false;
}

bool Level::isCubeLayer(int layer, glm::vec3 p)
{
	if(layer < 0 || layer >= LEVEL_NUMLAYERS) return false;

	glm::ivec3 _p(p);

	_p.x += LEVEL_WIDTH / 2;
	_p.y += LEVEL_WIDTH / 2;
	_p.z -= getOffset();

	return layers[layer].isCube(_p);
}

void Level::reset()
{
	state = LEVEL_INTRO_START;
	generator.reset();
	deadcubes.clear();
	deadcubes_wireframe.clear();
	slices.reset();
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		layers[i].clear();
	}
}

void Level::setIntro()
{
	for(int i = 0; i < SC_NUMCUBES; i++)
	{
		slices.cubes.setIntro(i, true);
		slices.cubes_wireframe.setIntro(i, true);
	}
}

void Level::clearIntro()
{
	for(int i = 0; i < SC_NUMCUBES; i++)
	{
		slices.cubes.clearIntro(i, true);
		slices.cubes_wireframe.clearIntro(i, true);
	}
}

bool Level::isEndPosition(glm::vec3 p)
{
	return p.z >= slices.getNumSlices() + slices.getBaseDepth();
}
