#include <cmath>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#define radToDeg(X) (float)((360.0*(X))/(2.0*M_PI))
#define degToRad(X) (float)((2.0*M_PI*(X))/360.0)
