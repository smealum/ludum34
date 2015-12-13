#ifndef LEVELGENERATOR_H
#define LEVELGENERATOR_H

#include <random>
#include <vector>
#include <unordered_map>
#include "slices.h"
#include "glm.h"

#define NUM_MARKOVSTEPS (15)

class LevelGenerator
{
	public:
		virtual bool getSlice(int layer, slice_s& out) = 0;
		virtual void reset() = 0;

	private:

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

		bool getConstraint(int id, pathConstraint_s& out);
		bool getConstraint(glm::ivec3 p, pathConstraint_s& out);

		// don't use this directly
		void addConstraint(pathConstraint_s constraint);
		void setConstraint(pathConstraint_s constraint);

		unsigned char getConstrainedCubeLayer(int layer, glm::ivec3 p);
		unsigned char getConstrainedCubeLayer(int layer, pathConstraint_s c);
		bool isConstrainedCubeInLayer(int layer, pathConstraint_s c);
		bool isConstrainedCubeInLayer(int layer, glm::ivec3 p);

		int getLayerSliceOrientation(int layer, int slice);

	private:
		std::vector<pathConstraint_s> instanced_data;
		std::vector< std::vector<pathConstraint_s> > instanced_data_slice;
		std::vector< std::vector<pathConstraint_s> > instanced_data_slice_next;
		std::vector<pathConstraint_s*> data;
		std::unordered_map<glm::ivec3, pathConstraint_s, KeyFuncs, KeyFuncs> dataMap;
		std::vector<int> orientations[LEVEL_NUMLAYERS];
};

class PathStepType
{
	public:
		virtual bool getStep(glm::ivec3 position, glm::ivec3& out, ConstraintManager& constraints) = 0;
		virtual const char* getName() = 0;

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

		
		void updateWeights(float* weights, int n);

		bool getStep(glm::ivec3 position, ConstraintManager& constraints, glm::ivec3& out);

	private:
		pathStepRandom_s* steps;
		int length;
		float total_weights;
		std::mt19937 mt;
		std::uniform_real_distribution<float> dist;
};

typedef struct
{
	int length;
	float markovWeights[NUM_MARKOVSTEPS];
}difficultyLevel_s;

typedef enum
{
	RANDOM_DIFFICULTY_STRAIGHT_LINE,
	RANDOM_DIFFICULTY_VERY_EASY,
	RANDOM_DIFFICULTY_EASY,
	RANDOM_DIFFICULTY_MEDIUM,
	RANDOM_DIFFICULTY_HARD,
}difficultyLevel_t;

class LevelGeneratorRandom : public LevelGenerator
{
	public:
		LevelGeneratorRandom(difficultyLevel_t difficulty = RANDOM_DIFFICULTY_HARD);

		virtual bool getSlice(int layer, slice_s& out);
		void generatePath();
		void generatePathStep();

		void clear();
		virtual void reset();

	private:
		int n[LEVEL_NUMLAYERS];
		slice_s previous_slice[LEVEL_NUMLAYERS];
		int length;
		std::vector<glm::ivec3> path;
		ConstraintManager constraints;
		Markov markov;
		int* depthMap;
		difficultyLevel_t difficulty;
};

#endif
