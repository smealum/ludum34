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
	layer(0),
	state(GAME_TITLE_SCREEN),
	timeStart(glfwGetTime()),
	music("music.wav", true),
	gameOverText("Game Over", glm::vec2(-0.725f, 0.0f), 0.16f),
	successText( " Success ", glm::vec2(-0.725f, 0.0f), 0.16f),
	titleText1(  "  qBen   ", glm::vec2(-0.725f + 0.16f / 2, 0.0f), 0.16f),
	titleText2(  "Press space to start the tutorial", glm::vec2(-0.725f + 0.16f / 2, -0.5f), 0.03f),
	titleText3(  "  Press enter to play the game   ", glm::vec2(-0.725f + 0.16f / 2, -0.5f), 0.03f)
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

	music.play();
}

void Game::startIntro()
{
	if(!level) return;

	level->reset();
	player.reset();

	state = GAME_INTRO;
	timeStart = glfwGetTime();
}

void Game::startOutro(gameTransitionTarget_t tt)
{
	state = GAME_OUTRO;
	transitionTarget = tt;
	player.startOutro();
	level->startOutro();
	timeStart = glfwGetTime();
}

void Game::update(float delta)
{
	if(!level) return;

	if(state == GAME_TITLE_SCREEN)
	{

		return;
	}

	switch(state)
	{
		case GAME_INTRO:
			if(glfwGetTime() - timeStart > 5.0)
			{
				state = GAME_PLAYING;
				player.finishIntro();
				level->finishIntro();
			}
			break;
		case GAME_PLAYING:
			if(level->isEndPosition(player.getPosition()))
			{
				startOutro(GAME_NEXTLEVEL);
			}

			if(player.isGameover())
			{
				startOutro(GAME_GAMEOVER);
			}

			if(!player.getAutopilot())
			{
				if(player.timer <= -50.0f)
				{
					// reset player.timer !
					player.timer = PLAYER_PERIOD;
				}else if(player.timer <= 0.0f)
				{
					player.timer = -500.0f;
					player.setAutopilot(true);
				}else{
					player.timer -= delta;
				}
			}else{
				// player is currently autopiloting
				// do nothing
			}

			if(Input::isKeyPressed(GLFW_KEY_SPACE))
			{
				startOutro(GAME_GAMEOVER);
			}

			if(Input::isKeyPressed(GLFW_KEY_ENTER))
			{
				startOutro(GAME_NEXTLEVEL);
			}

			if(Input::isKeyPressed(GLFW_KEY_R))
			{
				layer++;
				layer %= LEVEL_NUMLAYERS;
				hud.updateSelectedLayer(layer);
			}

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

			break;
		case GAME_OUTRO:
			if(glfwGetTime() - timeStart > 5.0)
			{
				if(transitionTarget == GAME_NEXTLEVEL)
				{
					// mem leak !
					loadLevel(*(new LevelGeneratorRandom()));
				}

				startIntro();
			}
			break;
		default:
			break;
	}

	if(level->update(delta))
	{
		// level geometry got updated !
		player.updatePath(*level);
	}
	player.update(*level, delta);
	player.updateCamera(camera);
}

void Game::draw()
{
	if(!level) return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	background.draw();

	if(state == GAME_TITLE_SCREEN)
	{
		titleText1.draw();
		titleText2.draw();
		titleText3.draw();
		return;
	}

	level->draw(camera, lighting, false, layer + 1);

	player.draw(camera, lighting, true);
	player.draw(camera, lighting);
	
	level->draw(camera, lighting, true, layer + 1);

	if(state == GAME_OUTRO)
	{
		if(transitionTarget == GAME_GAMEOVER)
		{
			gameOverText.draw();
		}else if(transitionTarget == GAME_NEXTLEVEL){
			successText.draw();
		}
	}

	hud.draw();
}

void Game::loadLevel(LevelGenerator& lg)
{
	level = new Level(lg);
}
