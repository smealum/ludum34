#include "game.h"
#include "gl.h"
#include "input.h"
#include "_math.h"

#define center_text(t, s) ((-1.0f * strlen((t)) * (s)) / 2)

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
	titleText2(      "Press space to start the tutorial", glm::vec2(-0.725f + 0.16f / 2, -0.5f), 0.04f),
	titleText3(      "  Press enter to play the game   ", glm::vec2(-0.725f + 0.16f / 2, -0.6f), 0.04f),
	
	tutorial0text1(  "Meet Ben.", glm::vec2(center_text("Meet Ben.", 0.07f), 0.6f + 0.04f), 0.07f),
	tutorial0text2(  "     You do not control Ben.     ", glm::vec2(-0.725f + 0.16f / 2, 0.6f - 0.05f * 1), 0.04f),
	tutorial0text3(  "Ben is a strong independent cube ", glm::vec2(-0.725f + 0.16f / 2, 0.6f - 0.05f * 2), 0.04f),
	tutorial0text4(  "    who don't need no player.    ", glm::vec2(-0.725f + 0.16f / 2, 0.6f - 0.05f * 3), 0.04f),
	
	tutorial1text1(  "Ben is not very smart.", glm::vec2(center_text("Ben is not very smart.", 0.06f), 0.6f + 0.04f), 0.06f),
	tutorial1text2(  "He's very superstitious and picks his direction", glm::vec2(center_text("He's very superstitious and picks his direction", 0.04f), 0.6f - 0.05f * 1), 0.04f),
	tutorial1text3(  "based on other cubes' colors.", glm::vec2(center_text("based on other cubes' colors.", 0.04f), 0.6f - 0.05f * 2), 0.04f),
	tutorial1text4(  "Use R to select a cube color and T to move it.", glm::vec2(center_text("Use R to select a cube color and T to move it.", 0.04f), 0.6f - 0.05f * 3), 0.04f),
	tutorial1text5(  "When Ben gets stuck, he'll try 3 times before giving up.", glm::vec2(center_text("When Ben gets stuck, he'll try 3 times before giving up.", 0.03f), 0.6f - 0.05f * 4), 0.03f),
	tutorial1text6(  "Come on, Ben.", glm::vec2(center_text("Come on, Ben.", 0.025f), 0.6f - 0.05f * 5), 0.025f),
	
	tutorial2text1(  "Ben has his own conventions.", glm::vec2(center_text("Ben has his own conventions.", 0.06f), 0.6f + 0.04f), 0.06f),
	tutorial2text2(  "Red means right, and Blue means left.", glm::vec2(center_text("Red means right, and Blue means left.", 0.04f), 0.6f - 0.05f * 1), 0.04f),
	tutorial2text3(  "Don't ask us why, Ben is pretty weird.", glm::vec2(center_text("Don't ask us why, Ben is pretty weird.", 0.04f), 0.6f - 0.05f * 2), 0.04f),
	tutorial2text4(  "", glm::vec2(center_text("", 0.04f), 0.6f - 0.05f * 3), 0.04f),
	
	tutorial3text1(  "Ben is not afraid of heights.", glm::vec2(center_text("Ben is not afraid of heights.", 0.06f), 0.6f + 0.04f), 0.06f),
	tutorial3text2(  "But of course he won't go up without the right color.", glm::vec2(center_text("But of course he won't go up without the right color.", 0.03f), 0.6f - 0.05f * 1), 0.03f),
	tutorial3text3(  "Red and blue make purple, purple obviously means up.", glm::vec2(center_text("Red and blue make purple, purple obviously means up.", 0.03f), 0.6f - 0.05f * 2), 0.03f),
	tutorial3text4(  "Please help Ben get where he wants to go.", glm::vec2(center_text("Please help Ben get where he wants to go.", 0.04f), 0.6f - 0.05f * 3), 0.04f)
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

difficultyLevel_t lastlevel = RANDOM_DIFFICULTY_TUTORIAL0;
int easycnt = 0;
int mediumcnt = 0;
int hardcnt = 0;

difficultyLevel_t getNextLevel()
{
	if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL0)
	{
		lastlevel = RANDOM_DIFFICULTY_TUTORIAL1;
		return lastlevel;
	}
	if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL1)
	{
		lastlevel = RANDOM_DIFFICULTY_TUTORIAL2;
		return lastlevel;
	}
	if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL2)
	{
		lastlevel = RANDOM_DIFFICULTY_TUTORIAL3;
		return lastlevel;
	}
	if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL3)
	{
		lastlevel = RANDOM_DIFFICULTY_EASY;
		return lastlevel;
	}
	if(lastlevel == RANDOM_DIFFICULTY_EASY)
	{
		if(easycnt < 2)
		{
			easycnt++;
			lastlevel = RANDOM_DIFFICULTY_EASY;
			return lastlevel;
		}
		lastlevel = RANDOM_DIFFICULTY_MEDIUM;
		return lastlevel;
	}	
	if(lastlevel == RANDOM_DIFFICULTY_MEDIUM)
	{
		if(mediumcnt < 3)
		{
			mediumcnt++;
			lastlevel = RANDOM_DIFFICULTY_MEDIUM;
			return lastlevel;
		}
		lastlevel = RANDOM_DIFFICULTY_HARD;
		return lastlevel;
	}
	if(lastlevel == RANDOM_DIFFICULTY_HARD)
	{
		if(hardcnt < 3)
		{
			hardcnt++;
			lastlevel = RANDOM_DIFFICULTY_HARD;
			return lastlevel;
		}
		lastlevel = RANDOM_DIFFICULTY_VERYHARD;
		return lastlevel;
	}
	lastlevel = RANDOM_DIFFICULTY_VERYHARD;
	return lastlevel;
}

void Game::update(float delta)
{
	if(!level) return;

	if(state == GAME_TITLE_SCREEN)
	{
		if(Input::isKeyPressed(GLFW_KEY_SPACE))
		{
			// mem leak !
			lastlevel = RANDOM_DIFFICULTY_TUTORIAL0;
			loadLevel(*(new LevelGeneratorRandom(RANDOM_DIFFICULTY_TUTORIAL0)));
			startIntro();
		}

		if(Input::isKeyPressed(GLFW_KEY_ENTER))
		{
			// mem leak !
			lastlevel = RANDOM_DIFFICULTY_MEDIUM;
			loadLevel(*(new LevelGeneratorRandom(RANDOM_DIFFICULTY_MEDIUM)));
			startIntro();
		}

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
					loadLevel(*(new LevelGeneratorRandom(getNextLevel())));
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
	}else if(state == GAME_PLAYING)
	{
		if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL0)
		{
			tutorial0text1.draw();
			tutorial0text2.draw();
			tutorial0text3.draw();
			tutorial0text4.draw();
		}else if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL1)
		{
			tutorial1text1.draw();
			tutorial1text2.draw();
			tutorial1text3.draw();
			tutorial1text4.draw();
			tutorial1text5.draw();
			tutorial1text6.draw();
		}else if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL2)
		{
			tutorial2text1.draw();
			tutorial2text2.draw();
			tutorial2text3.draw();
			tutorial2text4.draw();
		}else if(lastlevel == RANDOM_DIFFICULTY_TUTORIAL3)
		{
			tutorial3text1.draw();
			tutorial3text2.draw();
			tutorial3text3.draw();
			tutorial3text4.draw();
		}
	}

	hud.draw();
}

void Game::loadLevel(LevelGenerator& lg)
{
	level = new Level(lg);
}
