#ifndef LEVELGENERATOR_H
#define LEVELGENERATOR_H

#include "slices.h"

class LevelGenerator
{
	public:
		virtual slice_s getSlice(int layer) = 0;

	private:

};

class LevelGeneratorStatic : public LevelGenerator
{
	public:
		LevelGeneratorStatic();

		virtual slice_s getSlice(int layer);

	private:

};

#endif
