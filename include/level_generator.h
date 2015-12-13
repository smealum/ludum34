#ifndef LEVELGENERATOR_H
#define LEVELGENERATOR_H

#include <random>
#include <vector>
#include <unordered_map>
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

struct KeyFuncs
{
	size_t operator()(const glm::ivec3& k)const
	{
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y) ^ std::hash<int>()(k.z);
	}

	bool operator()(const glm::ivec3& a, const glm::ivec3& b)const
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
};

class ConstraintManager
{
	public:
		ConstraintManager();

		void reset();

		pathConstraint_s mergeConstraints(pathConstraint_s constraint);
		bool doesConstraintConflict(pathConstraint_s constraint);
		bool instanceConstraint(pathConstraint_s constraint);
		void clearInstancedConstraints();
		void flushInstancedConstraints();
		void sort();
		int size();

		// don't use this directly
		void addConstraint(pathConstraint_s constraint);
		void setConstraint(pathConstraint_s constraint);
		bool getConstraint(int id, pathConstraint_s& out);

	private:
		std::vector<pathConstraint_s> instanced_data;
		std::vector<pathConstraint_s*> data;
		std::unordered_map<glm::ivec3, pathConstraint_s, KeyFuncs, KeyFuncs> dataMap;
};

class PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints) = 0;

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

		bool getStep(glm::ivec3 position, ConstraintManager& constraints, glm::ivec3& out);

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

		void reset();

	private:
		int n[LEVEL_NUMLAYERS];
		int length;
		std::vector<glm::ivec3> path;
		ConstraintManager constraints;
		Markov markov;
		int* depthMap;
};

#endif
