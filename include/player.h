#ifndef PLAYER_H
#define PLAYER_H

#include "glm.h"
#include "cubes.h"
#include "level.h"

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

		void update(Level& level, float delta);
		void updateCamera(Camera& camera);
		void draw(Camera& camera, Lighting& lighting, bool shadow = false);
		
		void setNextMove(glm::vec3 direction);
		void startMoving(void);

		void setType(unsigned char type);

	private:
		glm::vec3 position;
		glm::vec3 direction, next_direction;
		Lighting shadow_lighting, outline_lighting;
		float angle;
		playerState_t state;
		Cubes cube, cube_outline;

		unsigned char type;

		std::queue<glm::vec3> moves;
};

#endif
