#ifndef SLICES_H
#define SLICES_H

#define LEVEL_WIDTH (9)
#define LEVEL_NUMLAYERS (3)

typedef unsigned int cubeProperties_t;

typedef struct
{
	unsigned char data[LEVEL_WIDTH][LEVEL_WIDTH];
}slice_s;

#endif
