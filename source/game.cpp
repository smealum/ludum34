#include "game.h"
#include "gl.h"
#include "input.h"
#include "_math.h"

Game::Game():
	camera(0.001f, 100.0f),
	lighting(),
	player(),
	hud(),
	level(NULL),
	layer(0)
{
	// CAMERA
	camera.setPosition(glm::vec3(-5.0f, 10.0f, -10.0f));
	// camera.view3 = glm::mat3(glm::rotate(glm::mat4(1.0f), float(M_PI / 4), glm::vec3(1.0f, 0.0f, 0.0f))) * glm::mat3(glm::rotate(glm::mat4(1.0f), float(3 * M_PI / 4), glm::vec3(0.0f, 1.0f, 0.0f)));
	camera.view3 = glm::mat3(glm::rotate(glm::mat4(1.0f), float(M_PI / 4) * 0.95f, glm::vec3(1.0f, 0.0f, 0.0f))) * glm::mat3(glm::rotate(glm::mat4(1.0f), float(3 * M_PI / 4) * 1.05f, glm::vec3(0.0f, 1.0f, 0.0f)));

	// LIGHTING
	lighting.setLightPosition(0, glm::vec3(0.575778, 0.324158, -0.750601));
	lighting.setLightEnabled(0, true);
	lighting.setLightDirectional(0, true);
	lighting.setLightFresnel(0, 0.0f);
	lighting.setLightShininess(0, 3.0f);

	glClearColor(19.0f / 255.0f, 31.0f / 255.0f, 46.0f / 255.0f, 1.0f);
}

void Game::update(float delta)
{
	if(!level) return;

	if(Input::isKeyPressed(GLFW_KEY_T))
	{
		bool canRotate = player.canRotateLayer(*level, layer);

		for(int i = 0; !canRotate && i < LEVEL_NUMLAYERS; i++)
		{
			if(i != layer && !player.canRotateLayer(*level, i))
			{
				canRotate = true;
			}
		}

		if(canRotate) level->rotateLayer(layer);
	}

	if(Input::isKeyPressed(GLFW_KEY_R))
	{
		layer++;
		layer %= LEVEL_NUMLAYERS;
		hud.updateSelectedLayer(layer);
	}

	if(Input::isKeyPressed(GLFW_KEY_O))
	{
		resetLevel();
	}

	level->update(delta);
	player.update(*level, delta);
	player.updateCamera(camera);
}

void Game::draw()
{
	if(!level) return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	level->draw(camera, lighting, false);

	player.draw(camera, lighting, true);
	player.draw(camera, lighting);
	
	level->draw(camera, lighting, true);

	hud.draw();
}

void Game::loadLevel(LevelGenerator& lg)
{
	level = new Level(lg);
}

void Game::resetLevel()
{
	if(!level) return;

	level->reset();
	player.reset();
}
