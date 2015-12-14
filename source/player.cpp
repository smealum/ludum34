#include <iostream>
#include "player.h"
#include "_math.h"
#include "input.h"

static const glm::vec3 playerColor = glm::vec3(255.0f, 174.0f, 68.0f) * (1.4f / 255);

Player::Player():
	sfx_cubemove("cubemove.wav", false),
	sfx_endpulse("endpulse.wav", false),
	cube(1),
	cube_outline(1, 1, true),
	path(PLAYER_PATHLENGTH)
{
	reset();

	cube.setColor(0, playerColor, true);

	// setup shadow lighting
	shadow_lighting.setObjectColor(false);
	shadow_lighting.setLightEnabled(0, true);
	shadow_lighting.setLightADSS(0, 0.6f, 0.0f, 0.0f, 0.0f);
	shadow_lighting.setLightFresnel(0, 0.0f, 0.0f, 0.0f, 0.0f);

	// setup outline lighting
	outline_lighting.setObjectColor(false);
	outline_lighting.setLightEnabled(0, true);
	outline_lighting.setLightADSS(0, 1.0f, 0.0f, 0.0f, 0.0f);
	outline_lighting.setLightFresnel(0, 0.0f, 0.0f, 0.0f, 0.0f);
}

void Player::reset()
{
	missedPulses = 0;

	position = glm::vec3(0.0f, 0.0f, 0.0f);
	last_position = position;
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
	rotation = false;
	progress = 0.0f;
	target_progress = 1.0f;
	state = PLAYER_INTRO;

	done_intro = false;

	progress = 0.0f;
	speed = 0.0f;
	target_progress = 2 * M_PI;

	autopilot = false;
	timer = -500.0f;
	outline_timer = 0.0f;

	gameover = false;

	type = 0;
	moves.clear();
}

void Player::finishIntro()
{
	done_intro = true;
	// if(state == PLAYER_INTRO)
	// {
	// 	state = PLAYER_NEWLYIDLE;
	// 	rotation = false;
	// 	progress = 0.0f;
	// }
}

void Player::startOutro()
{
	if(state != PLAYER_OUTRO)
	{
		state = PLAYER_OUTRO;
		progress = 0.0f;
		speed = 0.0f;
		target_progress = 2 * M_PI;
	}
}

void Player::setNextMove(glm::vec3 _direction)
{
	if(glm::length(_direction) < 0.001f) return;
	moves.push_front(_direction);
}

void Player::setType(unsigned char type)
{
	if(type >= 16) return;

	this->type = type;
}

glm::vec3 Player::getPosition()
{
	return position;
}

void Player::updateCamera(Camera& camera)
{
	if(state == PLAYER_INTRO || state == PLAYER_OUTRO) camera.setPosition(position + glm::vec3(-5.5f, 10.0f, -2.0f) * 1.2f);
	else camera.setPosition(position + direction * float(progress / target_progress) + glm::vec3(-5.5f, 10.0f, -2.0f) * 1.2f);
}

void Player::startNextMove()
{
	if(moves.empty() || state == PLAYER_MOVING) return;

	direction = moves.back();

	target_progress = M_PI / 2;

	speed = 3.0f;

	if(direction.y > 0.001f)
	{
		rotation_center = - direction * 0.5f;
		rotation_axis = glm::vec3(direction.z, 0.0, -direction.x);
		direction = glm::vec3(0.0f, direction.y, 0.0f);
		rotation = true;
	}else if(direction.y < -0.001f)
	{
		direction = glm::vec3(0.0f, direction.y, 0.0f);
		speed = 10.0f;
		rotation = false;
	}else{
		rotation_center = glm::vec3(0.0f, 0.5f, 0.0f) - direction * 0.5f;
		rotation_axis = glm::vec3(direction.z, 0.0, -direction.x);
		rotation = true;
	}

	moves.pop_back();
	state = PLAYER_MOVING;
}

bool Player::getAutopilot()
{
	return autopilot;
}

void Player::setAutopilot(bool v)
{
	autopilot = v;
	if(autopilot)
	{
		sfx_endpulse.play();
	}
}

bool Player::doStep(Level& level, bool nonlethal)
{
	if(state != PLAYER_IDLE || !moves.empty()) return false;

	glm::vec3 next;

	level.getNextLocation(position, next);

	if(glm::distance(position, next) < 0.001f)
	{
		// TODO : cause malus
		autopilot = false;
		return false;
	}

	// is next on same horizontal plane ?
	if(fabs(position.y - next.y) < 0.001f)
	{
		if((next.x < -LEVEL_WIDTH / 2 || next.x > LEVEL_WIDTH / 2) && nonlethal)
		{
			autopilot = false;
			return false;
		}
		setNextMove(next - position);
		return true;
	}

	// is next above us ?
	if(position.y < next.y)
	{
		int height_differential = int(next.y - position.y);
		glm::vec3 _direction = next - position;
		_direction.y = 1.0f;
		for(int i = 0; i < height_differential; i++)
		{
			setNextMove(_direction);
		}
		_direction.y = 0.0f;
		setNextMove(_direction);
		return true;
	}

	// is next below us ?
	if(position.y > next.y)
	{
		if(next.y <= -LEVEL_WIDTH / 2 && nonlethal)
		{
			autopilot = false;
			return false;
		}
		int height_differential = int(position.y - next.y);
		glm::vec3 _direction = next - position;
		_direction.y = 0.0f;
		setNextMove(_direction);
		for(int i = 0; i < height_differential; i++)
		{
			setNextMove(glm::vec3(0.0f, -1.0f, 0.0f));
		}
		return true;
	}

	return false;
}

void Player::updatePath(Level& level)
{
	path.generate(level, getPosition());
}

void Player::update(Level& level, float delta)
{
	playerState_t old_state;

	// loop is to avoid missing frames between moves
	do
	{
		old_state = state;

		switch(state)
		{
			case PLAYER_INTRO:
			case PLAYER_OUTRO:
				{
					if(progress < target_progress)
					{
						speed += sin(progress) * 10.0f * delta;
						progress += 2.0f * delta;
					}else{
						if(fabs(fmod(speed, M_PI / 2)) < 0.2f)
						{
							progress = target_progress;

							if(state == PLAYER_INTRO && done_intro)
							{
								state = PLAYER_NEWLYIDLE;
								rotation = false;
								progress = 0.0f;
							}
						}else
						{
							speed += sin(progress) * 20.0f * delta;
						}
					}
				}
				break;
			case PLAYER_IDLE:
				{
					if(!moves.empty())
					{
						startNextMove();
					}
				}
				break;
			case PLAYER_NEWLYIDLE:
				{
					unsigned char cube_type = level.getCubeInfo(position);

					// printf("%d\n", cube_type);

					if(cube_type > 0)
					{
						setType(getCubeWireframeId(cube_type));
					}

					updatePath(level);

					state = PLAYER_IDLE;
				}
				break;
			case PLAYER_MOVING:
				progress += speed * delta;
				if(progress >= target_progress)
				{
					progress = 0.0f;

					level.killCube(position + glm::vec3(0.0f, -1.0f, 0.0f));

					last_position = position;

					position += direction;
					position = glm::vec3(glm::ivec3(position));

					sfx_cubemove.play();

					state = PLAYER_NEWLYIDLE;

					while(int(position.z - level.getOffset()) > 8)
					{
						level.popSlice();
					}
				}
				break;
			default:
				break;
		}
	}while(old_state != state && state != PLAYER_MOVING);

	if(outline_timer <= 0.0f)
	{
		outline_timer = 0.0f;
	}else outline_timer -= 1.0f * delta;

	path.update(delta);

	if(autopilot)
	{
		doStep(level, true);
		
		if(autopilot)
		{
			missedPulses = 0;
		}else{
			missedPulses++;

			if(missedPulses > PLAYER_MAXMISSED)
			{
				gameover = true;
			}
		}
	}

	// if(Input::isKeyPressed(GLFW_KEY_Z))
	// {
	// 	setNextMove(glm::vec3(1.0f, 1.0f, 0.0f));
	// 	setNextMove(glm::vec3(1.0f, 0.0f, 0.0f));
	// }

	// if(Input::isKeyPressed(GLFW_KEY_X))
	// {
	// 	setNextMove(glm::vec3(0.0f, 1.0f, 1.0f));
	// 	setNextMove(glm::vec3(0.0f, 0.0f, 1.0f));
	// }

	// if(Input::isKeyPressed(GLFW_KEY_A))
	// {
	// 	setNextMove(glm::vec3(1.0f, 0.0f, 0.0f));
	// }

	// if(Input::isKeyPressed(GLFW_KEY_D))
	// {
	// 	setNextMove(glm::vec3(-1.0f, 0.0f, 0.0f));
	// }

	// if(Input::isKeyPressed(GLFW_KEY_W))
	// {
	// 	setNextMove(glm::vec3(0.0f, 0.0f, 1.0f));
	// }

	// if(Input::isKeyPressed(GLFW_KEY_S))
	// {
	// 	setNextMove(glm::vec3(0.0f, 0.0f, -1.0f));
	// }

	cube_outline.setColor(0, cubeTypes[type].color, true);
}

bool Player::canRotateLayer(Level& level, int layer)
{
	if(layer < 0 || layer >= LEVEL_NUMLAYERS) return false;

	return !level.isCubeLayer(layer, position + glm::vec3(0.0f, -1.0f, 0.0f));
}

bool Player::isGameover()
{
	return gameover || position.y <= - LEVEL_WIDTH / 2;
}

void Player::draw(Camera& camera, Lighting& lighting, bool shadow)
{
	if(shadow)
	{
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);
	}

	if(state == PLAYER_OUTRO || state == PLAYER_INTRO)
	{
		cube.model = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), speed, glm::vec3(0, 1, 0));
	}else{
		if(rotation)
		{
			cube.model = glm::rotate(glm::mat4(1.0f), progress, rotation_axis) * glm::translate(glm::mat4(1.0f), rotation_center);
			cube.model = glm::translate(glm::mat4(1.0f), position - rotation_center) * cube.model;
		}else{
			cube.model = glm::translate(glm::mat4(1.0f), position + direction * progress / target_progress);
		}
	}

	float f = (sin(timer * 5.0f) + 1.0f) * 0.5f;
	float blending = timer / PLAYER_PERIOD;
	glm::vec3 color2 = glm::vec3(68.0f, 255.0f, 74.0f) * (1.0f / 255.0f);
	glm::vec3 color3 = glm::vec3(255.0f, 38.0f, 0.0f) * (1.0f / 255.0f);

	float blending2 = ((missedPulses - 1) * 1.0f) / (PLAYER_MAXMISSED - 1);
	glm::vec3 color_final = color2 * (1 - blending2) + blending2 * color3;
	if(missedPulses == 1) color_final = glm::vec3(255.0f, 142.0f, 0.0f) * (1.0f / 255.0f);

	if(timer > -50.0f)
	{
		cube.setColor(0, playerColor * blending + (1.0f - blending) * color_final, true);
		if(timer <= 0.1f)
		{
			outline_timer = 1.0f;
		}
	}else{
		cube.setColor(0, playerColor, true);
	}

	cube.draw(camera, shadow ? shadow_lighting : lighting);
	if(state != PLAYER_INTRO && state != PLAYER_OUTRO) path.draw(camera, shadow ? shadow_lighting : lighting);
	
	// if(type > 1)
	if(outline_timer >= 0.0f)
	{
		cube_outline.setColor(0, glm::vec4(color_final, outline_timer * outline_timer * outline_timer), true);
		cube_outline.model = cube.model;
		cube_outline.draw(camera, outline_lighting);
	}

	if(shadow)
	{
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
	}
}
