#ifndef LEVEL_H
#define LEVEL_H

#include <deque>
#include "cubes.h"

#define LEVEL_WIDTH (9)
#define LEVEL_NUMLAYERS (3)
#define SC_NUMCUBES (1024)

typedef struct
{
	unsigned char data[LEVEL_WIDTH][LEVEL_WIDTH];
}slice_s;

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

		void mergeLayers(SliceCollection** sc, int n);

	private:
		std::deque<slice_s> data;
		Cubes cubes;
		int depth, orientation;
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

	private:
		SliceCollection slices;
		layerState_t state;
		float angle;
		int orientation;
};

class Level
{
	public:
		Level();
		
		void draw(Camera& camera, Lighting& lighting);

		void rotateLayer(int l);

		void update(float delta);
		void updateGeometry();
		void addSliceLayer(int l, slice_s s, bool update = true);

	private:
		SliceCollection slices;
		Layer layers[LEVEL_NUMLAYERS];
};

#endif
