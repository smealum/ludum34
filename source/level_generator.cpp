#include <iostream>
#include <algorithm>
#include <time.h>
#include "level_generator.h"
#include "level.h"

static slice_s first_slice =
	(slice_s)
	{
		{ 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0},  
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 1, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}
		}	
	};


//////////////////////////
// LEVELGENERATORRANDOM //
//////////////////////////

class PathStepTypeForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			out = position + glm::ivec3(0, 0, 1);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 1), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 1), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "Forward";
			return name;
		}
};

class PathStepTypeLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(1, 0, 0);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(1, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY) | CUBEPROPERTY_NOT_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(1, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "Left";
			return name;
		}
};

class PathStepTypeRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x <= 0) return false;
			out = position + glm::ivec3(-1, 0, 0);


			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(-1, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY) | CUBEPROPERTY_NOT_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(-1, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "Right";
			return name;
		}
};

class PathStepTypeAboveLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x >= LEVEL_WIDTH - 1) return false;
			// TODO : random height
			if(position.y >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(1, 1, 0);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 1, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(1, 0, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD) | CUBEPROPERTY_CLIMBABLE(1), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(1, 1, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "AboveLeft";
			return name;
		}
};

class PathStepTypeAboveRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x <= 0) return false;
			// TODO : random height
			if(position.y >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(-1, 1, 0);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 1, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(-1, 0, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD) | CUBEPROPERTY_CLIMBABLE(1), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(-1, 1, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "AboveRight";
			return name;
		}
};

class PathStepTypeAboveForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			// TODO : random height
			if(position.y >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(0, 1, 1);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 1, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 1), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD) | CUBEPROPERTY_CLIMBABLE(1), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 1, 1), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "AboveForward";
			return name;
		}
};

class PathStepTypeBelowLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x >= LEVEL_WIDTH - 1) return false;
			// TODO : random height
			if(position.y <= 1) return false;
			out = position + glm::ivec3(1, -1, 0);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(1, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(1, -1, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(1, -2, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "BelowLeft";
			return name;
		}
};

class PathStepTypeBelowRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x <= 0) return false;
			// TODO : random height
			if(position.y <= 1) return false;
			out = position + glm::ivec3(-1, -1, 0);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(-1, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(-1, -1, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(-1, -2, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "BelowRight";
			return name;
		}
};

class PathStepTypeBelowForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			// TODO : random height
			if(position.y <= 1) return false;
			out = position + glm::ivec3(0, -1, 1);

			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 1), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 1), 0, true});
			if(ret) {constraints.clearInstancedConstraints(); return false;}
			ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -2, 1), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
			if(ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "BelowForward";
			return name;
		}
};


bool getStepMultipleHeight(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints, glm::ivec3 direction, int max_height, cubePropertyDirection_t propDirection)
{
	bool ret;
	if(max_height > 2) max_height = 2;
	int height = rand() % (max_height) + 1;

	out = position + glm::ivec3(0, height, 0) + direction;

	ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION((cubeProperties_t)propDirection), false});
	if(ret) {constraints.clearInstancedConstraints(); return false;}
	ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
	if(ret) {constraints.clearInstancedConstraints(); return false;}

	for(int i = 0; i < height; i++)
	{
		ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 1 + i, 0), 0, true});
		if(ret) {constraints.clearInstancedConstraints(); return false;}
		cubeProperties_t value = (i == height - 1) ? CUBEPROPERTY_DIRECTION_FORWARD : CUBEPROPERTY_DIRECTION_ANY;
		ret = constraints.instanceConstraint((pathConstraint_s){position + direction + glm::ivec3(0, i, 0), CUBEPROPERTY_DIRECTION(value) | CUBEPROPERTY_CLIMBABLE(1), false});
		if(ret) {constraints.clearInstancedConstraints(); return false;}
	}

	ret = constraints.instanceConstraint((pathConstraint_s){position + direction + glm::ivec3(0, height, 0), 0, true});
	if(ret) {constraints.clearInstancedConstraints(); return false;}

	return true;
}

class PathStepTypeMultipleAboveLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x >= LEVEL_WIDTH - 1) return false;
			if(position.y >= LEVEL_WIDTH - 1) return false;

			int max_height = (LEVEL_WIDTH - 1) - position.y;

			ret = getStepMultipleHeight(position, out, constraints, glm::ivec3(1, 0, 0), max_height, CUBEPROPERTY_DIRECTION_LEFT);
			if(!ret) return false;

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "MultipleAboveLeft";
			return name;
		}
};

class PathStepTypeMultipleAboveRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x <= 0) return false;
			if(position.y >= LEVEL_WIDTH - 1) return false;

			int max_height = (LEVEL_WIDTH - 1) - position.y;

			ret = getStepMultipleHeight(position, out, constraints, glm::ivec3(-1, 0, 0), max_height, CUBEPROPERTY_DIRECTION_RIGHT);
			if(!ret) return false;

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "MultipleAboveRight";
			return name;
		}
};

class PathStepTypeMultipleAboveForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.y >= LEVEL_WIDTH - 1) return false;
			int max_height = (LEVEL_WIDTH - 1) - position.y;

			ret = getStepMultipleHeight(position, out, constraints, glm::ivec3(0, 0, 1), max_height, CUBEPROPERTY_DIRECTION_FORWARD);
			if(!ret) return false;

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "MultipleAboveForward";
			return name;
		}
};

bool getStepMultipleHeightBelow(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints, glm::ivec3 direction, int max_height, cubePropertyDirection_t propDirection)
{
	bool ret;
	if(max_height > 2) max_height = 2;
	int height = rand() % (max_height) + 1;

	out = position + glm::ivec3(0, -height, 0) + direction;

	ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION((cubeProperties_t)propDirection), false});
	if(ret) {constraints.clearInstancedConstraints(); return false;}
	ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
	if(ret) {constraints.clearInstancedConstraints(); return false;}

	for(int i = 0; i <= height; i++)
	{
		ret = constraints.instanceConstraint((pathConstraint_s){position + glm::ivec3(0, -i, 0), 0, true});
		if(ret) {constraints.clearInstancedConstraints(); return false;}
	}

	ret = constraints.instanceConstraint((pathConstraint_s){position + direction + glm::ivec3(0, - height - 1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
	if(ret) {constraints.clearInstancedConstraints(); return false;}


	return true;
}

class PathStepTypeMultipleBelowLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x >= LEVEL_WIDTH - 1) return false;
			if(position.y <= 2) return false;

			int max_height = position.y - 2;

			ret = getStepMultipleHeightBelow(position, out, constraints, glm::ivec3(1, 0, 0), max_height, CUBEPROPERTY_DIRECTION_LEFT);
			if(!ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "MultipleBelowLeft";
			return name;
		}
};

class PathStepTypeMultipleBelowRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.x <= 0) return false;
			if(position.y <= 2) return false;

			int max_height = position.y - 2;

			ret = getStepMultipleHeightBelow(position, out, constraints, glm::ivec3(-1, 0, 0), max_height, CUBEPROPERTY_DIRECTION_RIGHT);
			if(!ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "MultipleBelowRight";
			return name;
		}
};

class PathStepTypeMultipleBelowForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints)
		{
			bool ret;
			if(position.y <= 2) return false;

			int max_height = position.y - 2;

			ret = getStepMultipleHeightBelow(position, out, constraints, glm::ivec3(0, 0, 1), max_height, CUBEPROPERTY_DIRECTION_FORWARD);
			if(!ret) {constraints.clearInstancedConstraints(); return false;}

			constraints.flushInstancedConstraints();

			return true;
		}

		virtual const char* getName()
		{
			static const char* name = "MultipleBelowForward";
			return name;
		}
};

static pathStepRandom_s markov_steps[] =
{
	(pathStepRandom_s) {1.0f, new PathStepTypeForward()},
	(pathStepRandom_s) {1.0f, new PathStepTypeLeft()},
	(pathStepRandom_s) {1.0f, new PathStepTypeRight()},
	(pathStepRandom_s) {1.0f, new PathStepTypeAboveLeft()},
	(pathStepRandom_s) {1.0f, new PathStepTypeAboveRight()},
	(pathStepRandom_s) {1.0f, new PathStepTypeAboveForward()},
	(pathStepRandom_s) {1.0f, new PathStepTypeBelowLeft()},
	(pathStepRandom_s) {1.0f, new PathStepTypeBelowRight()},
	(pathStepRandom_s) {1.0f, new PathStepTypeBelowForward()},
	(pathStepRandom_s) {0.1f, new PathStepTypeMultipleAboveLeft()},
	(pathStepRandom_s) {0.1f, new PathStepTypeMultipleAboveRight()},
	(pathStepRandom_s) {0.1f, new PathStepTypeMultipleAboveForward()},
	(pathStepRandom_s) {0.1f, new PathStepTypeMultipleBelowLeft()},
	(pathStepRandom_s) {0.1f, new PathStepTypeMultipleBelowRight()},
	(pathStepRandom_s) {0.1f, new PathStepTypeMultipleBelowForward()},
};

static const int markov_length = sizeof(markov_steps) / sizeof(pathStepRandom_s);
static std::random_device rd;

difficultyLevel_s difficultyLevels[] =
{
	// RANDOM_DIFFICULTY_STRAIGHT_LINE
	(difficultyLevel_s)
	{
		// length
		16,
		// markovWeights
		{
			1.0f, // Forward
			0.0f, // Left
			0.0f, // Right
			0.0f, // AboveLeft
			0.0f, // AboveRight
			0.0f, // AboveForward
			0.0f, // BelowLeft
			0.0f, // BelowRight
			0.0f,  // BelowForward
			0.0f, // MultipleAboveLeft
			0.0f, // MultipleAboveRight
			0.0f, // MultipleAboveForward
			0.0f, // MultipleBelowLeft
			0.0f, // MultipleBelowRight
			0.0f  // MultipleBelowForward
		}, 
	},
	// RANDOM_DIFFICULTY_VERY_EASY
	(difficultyLevel_s)
	{
		// length
		16,
		// markovWeights
		{
			6.0f, // Forward
			1.0f, // Left
			1.0f, // Right
			0.0f, // AboveLeft
			0.0f, // AboveRight
			0.0f, // AboveForward
			0.0f, // BelowLeft
			0.0f, // BelowRight
			0.0f,  // BelowForward
			0.0f, // MultipleAboveLeft
			0.0f, // MultipleAboveRight
			0.0f, // MultipleAboveForward
			0.0f, // MultipleBelowLeft
			0.0f, // MultipleBelowRight
			0.0f  // MultipleBelowForward
		}, 
	},
	// RANDOM_DIFFICULTY_EASY
	(difficultyLevel_s)
	{
		// length
		16,
		// markovWeights
		{
			3.0f, // Forward
			1.0f, // Left
			1.0f, // Right
			0.0f, // AboveLeft
			0.0f, // AboveRight
			0.0f, // AboveForward
			0.0f, // BelowLeft
			0.0f, // BelowRight
			0.0f,  // BelowForward
			0.0f, // MultipleAboveLeft
			0.0f, // MultipleAboveRight
			0.0f, // MultipleAboveForward
			0.0f, // MultipleBelowLeft
			0.0f, // MultipleBelowRight
			0.0f  // MultipleBelowForward
		}, 
	},
	// RANDOM_DIFFICULTY_MEDIUM
	(difficultyLevel_s)
	{
		// length
		24,
		// markovWeights
		{
			1.0f, // Forward
			1.0f, // Left
			1.0f, // Right
			0.0f, // AboveLeft
			0.0f, // AboveRight
			0.0f, // AboveForward
			1.0f, // BelowLeft
			1.0f, // BelowRight
			1.0f,  // BelowForward
			0.0f, // MultipleAboveLeft
			0.0f, // MultipleAboveRight
			0.0f, // MultipleAboveForward
			0.0f, // MultipleBelowLeft
			0.0f, // MultipleBelowRight
			0.0f  // MultipleBelowForward
		}, 
	},
	// RANDOM_DIFFICULTY_HARD
	(difficultyLevel_s)
	{
		// length
		32,
		// markovWeights
		{
			1.0f, // Forward
			1.0f, // Left
			1.0f, // Right
			1.0f, // AboveLeft
			1.0f, // AboveRight
			1.0f, // AboveForward
			1.0f, // BelowLeft
			1.0f, // BelowRight
			1.0f,  // BelowForward
			0.1f, // MultipleAboveLeft
			0.1f, // MultipleAboveRight
			0.1f, // MultipleAboveForward
			0.1f, // MultipleBelowLeft
			0.1f, // MultipleBelowRight
			0.1f  // MultipleBelowForward
		},
	},
};

static const int num_difficulty_levels = sizeof(difficultyLevels) / sizeof(difficultyLevel_s);

Markov::Markov(pathStepRandom_s* steps, int length):
	steps(steps),
	length(length),
	total_weights(0),
	mt(rd())
{
	mt.seed(time(NULL));

	updateWeights(NULL, 0);

	dist = std::uniform_real_distribution<float>(0.0, total_weights);
}

void Markov::updateWeights(float* weights, int n)
{
	if(!weights && n) return;

	if(n > length) n = length;

	for(int i = 0; i < n; i++)
	{
		steps[i].weight = weights[i];
	}

	for(int i = 0; i < length; i++)
	{
		total_weights += steps[i].weight;
	}
}

bool Markov::getStep(glm::ivec3 position, ConstraintManager& constraints, glm::ivec3& out)
{
	bool rejected[length];
	int num_rejections = 0;

	float _total_weights = total_weights;

	for(int i = 0; i < length; i++) rejected[i] = false;
	
	while(num_rejections < length)
	{
		float t = 0, v = dist(mt) * _total_weights / total_weights;

		for(int i = 0; i < length; i++)
		{
			if(!rejected[i])
			{
				t += steps[i].weight;
				if(v < t)
				{
					bool ret = steps[i].stepper->getStep(position, out, constraints);

					// if(ret) printf("%s\n", steps[i].stepper->getName());
					
					if(ret) return true;

					_total_weights -= steps[i].weight;
					
					rejected[i] = true;
					num_rejections++;
					break;
				}
			}
		}
	}

	return false;
}

LevelGeneratorRandom::LevelGeneratorRandom(difficultyLevel_t difficulty):
	length(difficultyLevels[difficulty].length),
	markov(markov_steps, markov_length),
	depthMap(new int[length + 1]),
	difficulty(difficulty)
{
	markov.updateWeights(difficultyLevels[difficulty].markovWeights, NUM_MARKOVSTEPS);

	clear();

	generatePath();
}

void LevelGeneratorRandom::reset()
{
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		n[i] = 0;
	}
}

void LevelGeneratorRandom::clear()
{
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		n[i] = 0;
	}

	for(int i = 0; i < length + 1; i++)
	{
		depthMap[i] = 0;
	}

	path.clear();
	constraints.reset();
}

void LevelGeneratorRandom::generatePath()
{
	int tries = 1;
	do
	{
		clear();
		path.push_back(glm::ivec3(LEVEL_WIDTH / 2, LEVEL_WIDTH / 2, 0));

		int stuckCounter = 0;
		int cur_depth = 0, cur_depth_cnt = 0;
		while(path.back().z < length && stuckCounter < 5)
		{
			unsigned int s = path.size();
			generatePathStep();
			if(cur_depth != path.back().z)
			{
				cur_depth = path.back().z;
				cur_depth_cnt = 0;
			}else cur_depth_cnt++;
			if(path.size() == s || cur_depth_cnt > LEVEL_WIDTH) stuckCounter++;
			// printf("length %d\n", (int)path.size());
		}
		printf("%d tries\n", tries);
		tries++;
	}while(path.back().z < length);

	constraints.sort();

	// for(int i = 0; i < (int)path.size(); i++)
	// {
	// 	std::cout << glm::to_string(path[i]) << std::endl;
	// }

	for(int i = 0; i < (int)constraints.size(); i++)
	{
		// std::cout << glm::to_string(constraints[i].position) << " " << constraints[i].properties << " " << constraints[i].empty << std::endl;
		pathConstraint_s c;
		constraints.getConstraint(i, c);
		int depth = c.position.z;
		if(depth > 0 && depthMap[depth] == 0)
		{
			depthMap[depth] = i;
		}
	}
}

void LevelGeneratorRandom::generatePathStep()
{
	glm::ivec3 out;
	if(markov.getStep(path.back(), constraints, out))
	{
		// std::cout << glm::to_string(out) << std::endl;
		path.push_back(out);
	}
}

bool LevelGeneratorRandom::getSlice(int layer, slice_s& out)
{
	int depth = n[layer];

	if(depth >= length) return false;

	int index = depthMap[depth];

	slice_s ret;
	memset(&ret, 0, sizeof(slice_s));

	pathConstraint_s c;
	constraints.getConstraint(index, c);
	
	while(index < (int)constraints.size() && c.position.z == depth)
	{
		glm::ivec3 p = c.position;
		unsigned char value = constraints.getConstrainedCubeLayer(layer, c);

		if(value != 0)
		{
			ret.data[LEVEL_WIDTH - 1 - p.y][p.x] = value;
		}

		index++;

		constraints.getConstraint(index, c);
	}

	rotateSlice(&out, &ret, constraints.getLayerSliceOrientation(layer, depth));

	n[layer]++;

	return true;
}

ConstraintManager::ConstraintManager()
{

}

void ConstraintManager::reset()
{
	instanced_data.clear();
	instanced_data_slice.clear();
	data.clear();
	dataMap.clear();
	for(int i = 0; i < LEVEL_NUMLAYERS; i++) orientations[i].clear();
}

int ConstraintManager::getLayerSliceOrientation(int layer, int slice)
{
	if(layer < 0 || layer >= LEVEL_NUMLAYERS) return 0;
	if(slice <= 0 || slice >= (int)orientations[layer].size() + 1) return 0; // on purpose

	// if(layer == 0) printf("orientation %d\n", orientations[layer][slice - 1]);

	return orientations[layer][slice - 1];
}

unsigned char ConstraintManager::getConstrainedCubeLayer(int layer, glm::ivec3 p)
{
	pathConstraint_s c;
	if(!getConstraint(p, c)) return 0;

	return getConstrainedCubeLayer(layer, c);
}

unsigned char ConstraintManager::getConstrainedCubeLayer(int layer, pathConstraint_s c)
{
	if(!c.empty)
	{
		int v = 0;
		cubePropertyDirection_t dir = CUBEPROPERTY_GET_DIRECTION(c.properties);
		bool climbable = CUBEPROPERTY_GET_CLIMBABLE(c.properties);

		if(!climbable)
		{
			if(dir == CUBEPROPERTY_DIRECTION_LEFT) v |= (2 << 4);
			else if(dir == CUBEPROPERTY_DIRECTION_RIGHT) v |= (3 << 4);
		}else{
			if(layer == 1) v |= (2 << 4);
			else if(layer == 2) v |= (3 << 4);
		}

		if((v == 0 && layer == 0)
			|| (v == (2 << 4) && layer == 1)
			|| (v == (3 << 4) && layer == 2))
		{
			return (v | 1);
		}
	}

	return 0;
}

bool ConstraintManager::isConstrainedCubeInLayer(int layer, pathConstraint_s c)
{
	return getConstrainedCubeLayer(layer, c) != 0;
}

bool ConstraintManager::isConstrainedCubeInLayer(int layer, glm::ivec3 p)
{
	return getConstrainedCubeLayer(layer, p) != 0;
}

bool ConstraintManager::doesConstraintConflict(pathConstraint_s constraint)
{
	if(dataMap.find(constraint.position) == dataMap.end()) return false;

	pathConstraint_s _constraint = dataMap[constraint.position];
	if(constraint.empty != _constraint.empty) return true;
	if(constraint.empty) return false;

	cubePropertyDirection_t _dir = CUBEPROPERTY_GET_DIRECTION(_constraint.properties);
	cubePropertyDirection_t dir = CUBEPROPERTY_GET_DIRECTION(constraint.properties); 

	return !(   (_dir == CUBEPROPERTY_DIRECTION_ANY || dir == CUBEPROPERTY_DIRECTION_ANY || _dir == dir)
				&& !(dir != CUBEPROPERTY_DIRECTION_ANY && (_constraint.properties & CUBEPROPERTY_NOT_DIRECTION(dir)))
				&& !(_dir != CUBEPROPERTY_DIRECTION_ANY && (constraint.properties & CUBEPROPERTY_NOT_DIRECTION(_dir))));
}

pathConstraint_s ConstraintManager::mergeConstraints(pathConstraint_s constraint)
{
	pathConstraint_s _constraint = dataMap[constraint.position];

	cubePropertyDirection_t _dir = CUBEPROPERTY_GET_DIRECTION(_constraint.properties);
	cubePropertyDirection_t dir = CUBEPROPERTY_GET_DIRECTION(constraint.properties);

	if(_dir == CUBEPROPERTY_DIRECTION_ANY)
	{
		_constraint.properties = (_constraint.properties & ~ CUBEPROPERTY_DIRECTION(0xFF))
								| CUBEPROPERTY_DIRECTION(dir)
								| CUBEPROPERTY_CLIMBABLE(CUBEPROPERTY_GET_CLIMBABLE(constraint.properties))
								| (constraint.properties & CUBEPROPERTY_NOT_DIRECTION(0xFF));
	}

	return _constraint;
}

bool ConstraintManager::instanceConstraint(pathConstraint_s constraint)
{
	bool ret = doesConstraintConflict(constraint);

	if(ret) return true;

	instanced_data.push_back(constraint);

	return false;
}

void ConstraintManager::clearInstancedConstraints()
{
	instanced_data.clear();
}

void ConstraintManager::flushInstancedConstraints()
{
	int cur_depth = orientations[0].size();
	bool new_slice = false;
	for(int i = 0; i < (int)instanced_data.size(); i++)
	{
		addConstraint(instanced_data[i]);
		if(instanced_data[i].position.z > cur_depth + 1) new_slice = true;
	}

	if(new_slice && instanced_data_slice.size() > 0)
	{
		for(int l = 0; l < LEVEL_NUMLAYERS; l++)
		{
			bool independent = true;
			for(int j = 0; independent && j < (int)instanced_data_slice.size(); j++)
			{
				int cur_depth = -1;

				for(int i = 0; independent && i < (int)instanced_data_slice[j].size(); i++)
				{
					if(isConstrainedCubeInLayer(l, instanced_data_slice[j][i].position))
					{
						if(cur_depth < 0)
						{
							cur_depth = instanced_data_slice[j][i].position.z;
						}
						else if(cur_depth != instanced_data_slice[j][i].position.z)
						{
							independent = false;
						}
					}
				}
			}

			if(independent) orientations[l].push_back(rand() % 4);
			else orientations[l].push_back((orientations[l].size() > 0) ? orientations[l].back() : 0);
		}

		instanced_data_slice.clear();
	}

	instanced_data_slice.push_back(instanced_data);

	clearInstancedConstraints();
}

void ConstraintManager::setConstraint(pathConstraint_s constraint)
{
	if(dataMap.find(constraint.position) == dataMap.end()) return;

	*&dataMap[constraint.position] = constraint;
}

void ConstraintManager::addConstraint(pathConstraint_s constraint)
{
	if(dataMap.find(constraint.position) != dataMap.end()) setConstraint(mergeConstraints(constraint));
	else{
		dataMap[constraint.position] = constraint;
		data.push_back(&dataMap[constraint.position]);
	}
}

bool ConstraintManager::getConstraint(int id, pathConstraint_s& out)
{
	if(id < 0 || id >= (int)data.size()) return false;

	out = *data[id];

	return true;
}

bool ConstraintManager::getConstraint(glm::ivec3 p, pathConstraint_s& out)
{
	if(dataMap.find(p) == dataMap.end()) return false;

	out = dataMap[p];

	return true;
}

bool constraintPointerCompare (pathConstraint_s* i, pathConstraint_s* j) { return (i->position.z < j->position.z); }

void ConstraintManager::sort()
{
	std::sort(data.begin(), data.end(), constraintPointerCompare);
}

int ConstraintManager::size()
{
	return (int)data.size();
}
