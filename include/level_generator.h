#ifndef LEVELGENERATOR_H
#define LEVELGENERATOR_H

#include <random>
#include <vector>
#include "slices.h"
#include "glm.h"

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
		int n[LEVEL_NUMLAYERS];
};

typedef struct
{
	glm::ivec3 position;
	cubeProperties_t properties;
	bool empty;
}pathConstraint_s;


class PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, std::vector<pathConstraint_s>& constraints) = 0;

	private:

};

typedef struct
{
	float weight;
	PathStepType* stepper;
}pathStepRandom_s;

class Markov
{
	public:
		Markov(pathStepRandom_s* steps, int length);

		glm::ivec3 getStep(glm::ivec3 position, std::vector<pathConstraint_s>& constraints);

	private:
		pathStepRandom_s* steps;
		int length;
		float total_weights;
		std::mt19937 mt;
		std::uniform_real_distribution<float> dist;
};

class LevelGeneratorRandom : public LevelGenerator
{
	public:
		LevelGeneratorRandom(int length);

		virtual slice_s getSlice(int layer);
		void generatePath();
		void generatePathStep();

	private:
		int n[LEVEL_NUMLAYERS];
		int length;
		std::vector<glm::ivec3> path;
		std::vector<pathConstraint_s> constraints;
		Markov markov;
		int* depthMap;
};

#endif
