#ifndef LEVEL_H
#define LEVEL_H

#include <deque>
#include "cubes.h"
#include "slices.h"
#include "level_generator.h"

#define LEVEL_NUMLAYERS (3)
#define LEVEL_NUMSLICES (32)
#define SC_NUMCUBES (1024)

#define getCubeFullId(v) ((v) & 0xf)
#define getCubeWireframeId(v) (((v) >> 4) & 0xf)

typedef enum
{
	LAYER_IDLE,
	LAYER_ROTATING,
}layerState_t;

typedef struct
{
	glm::vec3 color;
}cubeType_s;

extern cubeType_s cubeTypes[16];

class SliceCollection
{
	public:
		SliceCollection();

		void addSlice(slice_s s);
		void popSlice();

		void draw(Camera& camera, Lighting& lighting, bool wireframe = false);

		void setAngle(float angle);
		void setOrientation(int orientation);
		void clear();

		void incrementBaseDepth(int v = 1);
		
		int getBaseDepth();
		int getNumSlices();

		void mergeLayers(SliceCollection** sc, int n);

		unsigned char getCubeInfo(glm::ivec3 p);

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

		void addSlice(slice_s s);
		void popSlice();

	private:
		SliceCollection slices;
		layerState_t state;
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

		unsigned char getCubeInfo(glm::vec3 p);

	private:
		LevelGenerator& generator;
		SliceCollection slices;
		Layer layers[LEVEL_NUMLAYERS];
};

#endif
