#ifndef LEVEL_H
#define LEVEL_H

#include <deque>
#include "cubes.h"
#include "slices.h"
#include "level_generator.h"

#define LEVEL_NUMSLICES (32)
#define SC_NUMCUBES (1024)

#define getCubeFullId(v) ((v) & 0xf)
#define getCubeWireframeId(v) (((v) >> 4) & 0xf)

#define CUBEPROPERTY_COLOR(v) ((v) & 0xFF)
#define CUBEPROPERTY_DIRECTION(v) (((v) & 0x7) << 8)
#define CUBEPROPERTY_CLIMBABLE(v) (((v) & 0x1) << 11)
#define CUBEPROPERTY_DISCRIMINATE(v) (((v) & 0x1) << 12)
#define CUBEPROPERTY_NOT_DIRECTION(v) ((1) << (13 + ((v) & 3)))
#define CUBEPROPERTY_OUTOFBOUNDS(v) (((v) & 0x1) << 31)
#define CUBEPROPERTY_GET_COLOR(v) ((v) & 0xFF)
#define CUBEPROPERTY_GET_DIRECTION(v) (cubePropertyDirection_t)(((v) >> 8) & 0x7)
#define CUBEPROPERTY_GET_CLIMBABLE(v) (bool)(((v) >> 11) & 0x1)
#define CUBEPROPERTY_IS_DISCRIMINATE(v) (bool)(((v) >> 12) & 0x1)
#define CUBEPROPERTY_IS_OUTOFBOUNDS(v) (bool)(((v) >> 31) & 0x1)
#define CUBEPROPERTY_IS_EMPTY(v) (bool)(getCubeFullId(CUBEPROPERTY_GET_COLOR(v)) == 0)

typedef enum
{
	CUBEPROPERTY_DIRECTION_FORWARD = 0,
	CUBEPROPERTY_DIRECTION_BACKWARD = 1,
	CUBEPROPERTY_DIRECTION_LEFT = 2,
	CUBEPROPERTY_DIRECTION_RIGHT = 3,
	CUBEPROPERTY_DIRECTION_ANY = 4,
}cubePropertyDirection_t;

typedef enum
{
	LAYER_IDLE,
	LAYER_ROTATING,
}layerState_t;

typedef struct
{
	glm::vec3 color;
	cubeProperties_t full_properties, wireframe_properties;
}cubeType_s;

extern cubeType_s cubeTypes[16];

class SliceCollection
{
	friend class Level;
	public:
		SliceCollection();

		void addSlice(slice_s s);
		void popSlice();

		void draw(Camera& camera, Lighting& lighting, bool wireframe = false);

		void setAngle(float angle);
		void setOrientation(int orientation);
		void clear();
		void reset();

		void incrementBaseDepth(int v = 1);
		
		int getBaseDepth();
		int getNumSlices();

		void mergeLayers(SliceCollection** sc, int n);

		unsigned char getCubeInfo(glm::ivec3 p, bool* out_of_bounds = NULL);
		cubeProperties_t getCubeProperties(glm::ivec3 p);
		void removeCube(glm::ivec3 p);

	private:
		std::deque<slice_s> data;
		Cubes cubes, cubes_wireframe;
		int base_depth, depth, orientation;
		float angle;
};

class Layer
{
	friend class Level;
	public:
		Layer();

		void draw(Camera& camera, Lighting& lighting, bool wireframe = false);
		bool update(float delta);

		void rotate();

		void clear();

		void addSlice(slice_s s);
		void popSlice();

		bool isCube(glm::vec3 p);
		void removeCube(glm::ivec3 p);

	private:
		SliceCollection slices;
		layerState_t state;
		bool queued_rotation;
		float angle;
		int orientation;
};

class Level
{
	public:
		Level(LevelGenerator& lg);
		
		void draw(Camera& camera, Lighting& lighting, bool wireframe = false);

		void rotateLayer(int l);

		void update(float delta);
		void updateGeometry();

		void addSliceLayer(int l, slice_s s, bool update = true);
		void popSlice(bool update = true);

		int getOffset();

		bool isCubeLayer(int layer, glm::vec3 p);
		unsigned char getCubeInfo(glm::vec3 p);
		cubeProperties_t getCubeProperties(glm::vec3 p);

		bool getNextLocation(glm::vec3 p, glm::vec3& out);

		void killCube(glm::vec3 p);

		void reset();

	private:
		Cubes deadcubes, deadcubes_wireframe;
		LevelGenerator& generator;
		SliceCollection slices;
		Layer layers[LEVEL_NUMLAYERS];
};

#endif
