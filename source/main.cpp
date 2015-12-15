#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "gl.h"
#include "window.h"
#include "settings.h"
#include "dbg.h"

#include "game.h"
#include "audio.h"
#include "text.h"

Settings settings(800, 600, 3.0f);

int main(void)
{
	windowInit();
	glewInit();
	audioInit();
	textInit();
	
	glLineWidth(settings.line_width);

	glViewport(0, 0, settings.width, settings.height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Game game;
	// LevelGeneratorRandom levelGenerator_random(RANDOM_DIFFICULTY_VERY_EASY);
	// LevelGeneratorRandom levelGenerator_random(RANDOM_DIFFICULTY_STRAIGHT_LINE);
	LevelGeneratorRandom levelGenerator_random(RANDOM_DIFFICULTY_HARD);

	game.loadLevel(levelGenerator_random);

	double lastFrame = glfwGetTime();

	while(windowUpdate())
	{
		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		game.update(deltaTime);
		game.draw();
	}

	textExit();
	audioExit();
	windowExit();

	exit(EXIT_SUCCESS);
}
