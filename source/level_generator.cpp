#include "level_generator.h"

static slice_s test_slice = 
{
	{ 
	  {1, 0, 0, 0, 0, 0, 0, 0, 0},  
	  {1, 0, 0, 0, 0, 0, 0, 0, 0}, 
	  {1, 0, 0, 0, 0, 0, 0, 0, 0}, 
	  {1, 0, 0, 0, 0, 0, 0, 0, 0}, 
	  {1, 1, 0, 0, 0, 0, 0, 0, 0}, 
	  {1, 0, 0, 0, 1, 0, 0, 0, 0}, 
	  {1, 0, 0, 0, 0, 0, 0, 0, 0}, 
	  {1, 0, 0, 0, 0, 0, 0, 0, 0}, 
	  {1, 1, 1, 1, 1, 1, 1, 1, 1}
	}	
};

LevelGeneratorStatic::LevelGeneratorStatic()
{
	
}

slice_s LevelGeneratorStatic::getSlice(int layer)
{
	return test_slice;
}
