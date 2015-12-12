#ifndef LEVEL_H
#define LEVEL_H

#include <deque>
#include "cubes.h"
#include "slices.h"
#include "level_generator.h"

#define LEVEL_NUMLAYERS (3)
#define LEVEL_NUMSLICES (32)
#define SC_NUMCUBES (1024)

typedef enum
{
	LAYER_IDLE,
	LAYER_ROTATING,
}layerState_t;

class SliceCollection
{
	public:
		SliceCollection();

		void addSlice(slice_s s);
		void popSlice();

		void draw(Camera& camera, Lighting& lighting);

		void setAngle(float angle);
		void setOrientation(int orientation);
		void clear();

		void incrementBaseDepth(int v = 1);
		
		int getBaseDepth();
		int getNumSlices();

		void mergeLayers(SliceCollection** sc, int n);

	private:
		std::deque<slice_s> data;
		Cubes cubes;
		int base_depth, depth, orientation;
		float angle;
};

class Layer
{
	friend class Level;
	public:
		Layer();

		void draw(Camera& camera, Lighting& lighting);
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
		
		void draw(Camera& camera, Lighting& lighting);

		void rotateLayer(int l);

		void update(float delta);
		void updateGeometry();

		void addSliceLayer(int l, slice_s s, bool update = true);
		void popSlice(bool update = true);

		int getOffset();

	private:
		LevelGenerator& generator;
		SliceCollection slices;
		Layer layers[LEVEL_NUMLAYERS];
};

#endif
