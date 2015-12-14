#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "level_generator.h"
#include "player.h"
#include "camera.h"
#include "lighting.h"
#include "background.h"
#include "audio.h"
#include "hud.h"
#include "text.h"

typedef enum
{
	GAME_TITLE_SCREEN,
	GAME_INTRO,
	GAME_PLAYING,
	GAME_OUTRO,
}gameState_t;

typedef enum
{
	GAME_GAMEOVER,
	GAME_NEXTLEVEL,
}gameTransitionTarget_t;

class Game
{
	public:
		Game();
		
		void update(float delta);
		void draw();

		void loadLevel(LevelGenerator& lg);

		void startIntro();
		void startOutro(gameTransitionTarget_t tt);

	private:
		Camera camera;
		Background background;
		Lighting lighting;
		Player player;
		Hud hud;

		Level* level;

		int layer;

		gameState_t state;
		gameTransitionTarget_t transitionTarget;
		double timeStart;

		Sound music;

		Text gameOverText;
		Text successText;
		Text titleText1;
		Text titleText2;
		Text titleText3;
};

#endif
