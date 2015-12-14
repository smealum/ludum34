#ifndef PLAYER_H
#define PLAYER_H

#include "glm.h"
#include "cubes.h"
#include "level.h"
#include "path.h"
#include "audio.h"

#include <deque>

#define PLAYER_PATHLENGTH (8)
#define PLAYER_PERIOD (5.0f)

#define PLAYER_MAXMISSED (3)

typedef enum
{
	PLAYER_INTRO,
	PLAYER_OUTRO,
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
		void startNextMove(void);
		bool doStep(Level& level, bool nonlethal = false);

		void setType(unsigned char type);
		void updatePath(Level& level);

		bool canRotateLayer(Level& level, int layer);

		void finishIntro();
		void startOutro();

		bool getAutopilot();
		void setAutopilot(bool v);
		
		glm::vec3 getPosition();
		bool isGameover();

		void reset();
		
		float timer;
		float outline_timer;

	private:
		int missedPulses;
		Sound sfx_cubemove;
		Sound sfx_endpulse;
		glm::vec3 position, last_position;
		glm::vec3 direction;
		glm::vec3 rotation_axis, rotation_center;
		bool rotation;
		Lighting shadow_lighting, outline_lighting;
		float progress, target_progress, speed;
		playerState_t state;
		Cubes cube, cube_outline;
		Path path;

		bool done_intro;
		bool autopilot;
		bool gameover;

		unsigned char type;

		std::deque<glm::vec3> moves;
};

#endif
