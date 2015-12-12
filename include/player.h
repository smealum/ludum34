#ifndef PLAYER_H
#define PLAYER_H

#include "glm.h"
#include "cubes.h"
#include "level.h"
#include "path.h"

#include <queue>

#define PLAYER_PATHLENGTH (8)

typedef enum
{
	PLAYER_IDLE,
	PLAYER_NEWLYIDLE,
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
		
		glm::vec3 getPosition();
		
	private:
		glm::vec3 position;
		glm::vec3 direction, next_direction;
		Lighting shadow_lighting, outline_lighting;
		float angle;
		playerState_t state;
		Cubes cube, cube_outline;
		Path path;

		unsigned char type;

		std::queue<glm::vec3> moves;
};

#endif
