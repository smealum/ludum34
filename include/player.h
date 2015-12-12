#ifndef PLAYER_H
#define PLAYER_H

#include "glm.h"
#include "cubes.h"

#include <queue>

typedef enum
{
	PLAYER_IDLE,
	PLAYER_MOVING
}playerState_t;

class Player
{
	public:
		Player();

		void update(float delta);
		void draw(Camera& camera, Lighting& lighting);
		
		void setNextMove(glm::vec3 direction);
		void startMoving(void);

	private:
		glm::vec3 position;
		glm::vec3 direction, next_direction;
		float angle;
		playerState_t state;
		Cubes cube;

		std::queue<glm::vec3> moves;
};

#endif
