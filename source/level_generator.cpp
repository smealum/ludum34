#include <iostream>
#include <algorithm>
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

static slice_s test_slice[] = 
{
	(slice_s)
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
	},
	(slice_s)
	{
		{ 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0},  
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 2 << 4, 0, 0, 2 << 4, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 2 << 4, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}
		}	
	},
	(slice_s)
	{
		{ 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0},  
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 3 << 4, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 3 << 4, 3, (3 << 4) | 3, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
		  {0, 0, 0, 0, 0, 0, 0, 0, 0}
		}	
	},
};


LevelGeneratorStatic::LevelGeneratorStatic()
{
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		n[i] = 0;
	}
}

slice_s LevelGeneratorStatic::getSlice(int layer)
{
	n[layer]++;
	if(n[layer] == 1) return first_slice;
	return test_slice[layer];
}

//////////////////////////
// LEVELGENERATORRANDOM //
//////////////////////////

class PathStepTypeForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			out = position + glm::ivec3(0, 0, 1);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 1), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 1), 0, true});

			return true;
		}
};

class PathStepTypeLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			if(position.x >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(1, 0, 0);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(1, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(1, 0, 0), 0, true});

			return true;
		}
};

class PathStepTypeRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			if(position.x <= 0) return false;
			out = position + glm::ivec3(-1, 0, 0);


			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(-1, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(-1, 0, 0), 0, true});

			return true;
		}
};

class PathStepTypeAboveLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			if(position.x >= LEVEL_WIDTH - 1) return false;
			// TODO : random height
			if(position.y >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(1, 1, 0);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 1, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(1, 0, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY) | CUBEPROPERTY_CLIMBABLE(1), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(1, 1, 0), 0, true});

			return true;
		}
};

class PathStepTypeAboveRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			if(position.x <= 0) return false;
			// TODO : random height
			if(position.y >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(-1, 1, 0);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 1, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(-1, 0, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY) | CUBEPROPERTY_CLIMBABLE(1), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(-1, 1, 0), 0, true});

			return true;
		}
};

class PathStepTypeAboveForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			// TODO : random height
			if(position.y >= LEVEL_WIDTH - 1) return false;
			out = position + glm::ivec3(0, 1, 1);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 1, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 1), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY) | CUBEPROPERTY_CLIMBABLE(1), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 1, 1), 0, true});

			return true;
		}
};

class PathStepTypeBelowLeft : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			if(position.x >= LEVEL_WIDTH - 1) return false;
			// TODO : random height
			if(position.y <= 0) return false;
			out = position + glm::ivec3(1, -1, 0);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_LEFT), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(1, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(1, -1, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(1, -2, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});

			return true;
		}
};

class PathStepTypeBelowRight : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			if(position.x <= 0) return false;
			// TODO : random height
			if(position.y <= 0) return false;
			out = position + glm::ivec3(-1, -1, 0);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_RIGHT), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(-1, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(-1, -1, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(-1, -2, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});

			return true;
		}
};

class PathStepTypeBelowForward : public PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints)
		{
			// TODO : random height
			if(position.y <= 0) return false;
			out = position + glm::ivec3(0, -1, 1);

			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 0), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_FORWARD), false});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 0), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, 0, 1), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -1, 1), 0, true});
			constraints.push_back((pathConstraint_s){position + glm::ivec3(0, -2, 1), CUBEPROPERTY_DIRECTION(CUBEPROPERTY_DIRECTION_ANY), false});

			return true;
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
};

static const int markov_length = sizeof(markov_steps) / sizeof(pathStepRandom_s);
static std::random_device rd;

Markov::Markov(pathStepRandom_s* steps, int length):
	steps(steps),
	length(length),
	total_weights(0),
	mt(rd())
{
	for(int i = 0; i < length; i++)
	{
		total_weights += steps[i].weight;
	}

	dist = std::uniform_real_distribution<float>(0.0, total_weights);
}

glm::ivec3 Markov::getStep(glm::ivec3 position, std::vector<pathConstraint_s>& constraints)
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
					glm::ivec3 out;
					bool ret = steps[i].stepper->getStep(position, out, constraints);
					
					if(ret) return out;

					_total_weights -= steps[i].weight;
					
					rejected[i] = true;
					num_rejections++;
					break;
				}
			}
		}
	}

	return position;
}

LevelGeneratorRandom::LevelGeneratorRandom(int length):
	length(length),
	markov(markov_steps, markov_length),
	depthMap(new int[length + 1])
{
	for(int i = 0; i < LEVEL_NUMLAYERS; i++)
	{
		n[i] = 0;
	}

	for(int i = 0; i < length + 1; i++)
	{
		depthMap[i] = 0;
	}

	generatePath();
}

bool constraintCompare (pathConstraint_s i, pathConstraint_s j) { return (i.position.z < j.position.z); }

void LevelGeneratorRandom::generatePath()
{
	path.push_back(glm::ivec3(LEVEL_WIDTH / 2, LEVEL_WIDTH / 2, 0));

	while(path.back().z < length)
	{
		generatePathStep();
	}

	std::sort(constraints.begin(), constraints.end(), constraintCompare);

	// for(int i = 0; i < (int)path.size(); i++)
	// {
	// 	std::cout << glm::to_string(path[i]) << std::endl;
	// }

	for(int i = 0; i < (int)constraints.size(); i++)
	{
		// std::cout << glm::to_string(constraints[i].position) << " " << constraints[i].properties << " " << constraints[i].empty << std::endl;
		int depth = constraints[i].position.z;
		if(depth > 0 && depthMap[depth] == 0)
		{
			depthMap[depth] = i;
		}
	}
}

void LevelGeneratorRandom::generatePathStep()
{
	path.push_back(markov.getStep(path.back(), constraints));
}

slice_s LevelGeneratorRandom::getSlice(int layer)
{
	int depth = n[layer];

	if(depth >= length) return first_slice;

	int index = depthMap[depth];

	slice_s ret;
	memset(&ret, 0, sizeof(slice_s));

	while(index < (int)constraints.size() && constraints[index].position.z == depth)
	{
		glm::ivec3 p = constraints[index].position;
		
		if(!constraints[index].empty)
		{
			int v = 0;
			cubePropertyDirection_t dir = CUBEPROPERTY_GET_DIRECTION(constraints[index].properties);
			bool climbable = CUBEPROPERTY_GET_CLIMBABLE(constraints[index].properties);
			
			std::cout << glm::to_string(p) << " " << constraints[index].empty << " " << constraints[index].properties << std::endl;

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
				ret.data[LEVEL_WIDTH - 1 - p.y][p.x] = v | 1;
			}

		}

		index++;
	}

	n[layer]++;

	return ret;
}
