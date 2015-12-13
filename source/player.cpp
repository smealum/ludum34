#include <iostream>
#include "player.h"
#include "_math.h"
#include "input.h"

Player::Player():
	cube(1),
	cube_outline(1, 1, true),
	path(PLAYER_PATHLENGTH)
{
	reset();

	cube.setColor(0, glm::vec3(255.0f, 174.0f, 68.0f) * (1.4f / 255), true);

	last_position = position;

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
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
	rotation = false;
	progress = 0.0f;
	target_progress = 1.0f;
	state = PLAYER_NEWLYIDLE;
	type = 0;
	moves.clear();
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
	camera.setPosition(position + direction * float(progress / target_progress) + glm::vec3(-5.5f, 10.0f, -2.0f) * 1.2f);
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

void Player::doStep(Level& level)
{
	if(state != PLAYER_IDLE || !moves.empty()) return;

	glm::vec3 next;

	level.getNextLocation(position, next);

	if(glm::distance(position, next) < 0.001f)
	{
		// TODO : cause malus
		return;
	}

	// is next on same horizontal plane ?
	if(fabs(position.y - next.y) < 0.001f)
	{
		setNextMove(next - position);
		return;
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
		return;
	}

	// is next below us ?
	if(position.y > next.y)
	{
		int height_differential = int(position.y - next.y);
		glm::vec3 _direction = next - position;
		_direction.y = 0.0f;
		setNextMove(_direction);
		for(int i = 0; i < height_differential; i++)
		{
			setNextMove(glm::vec3(0.0f, -1.0f, 0.0f));
		}
		return;
	}
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

					path.generate(level, getPosition());

					state = PLAYER_IDLE;
				}
				break;
			case PLAYER_MOVING:
				progress += speed * delta;
				if(progress >= target_progress)
				{
					progress = 0.0f;

					level.killCube(last_position + glm::vec3(0.0f, -1.0f, 0.0f));

					last_position = position;

					position += direction;
					position = glm::vec3(glm::ivec3(position));

					state = PLAYER_NEWLYIDLE;

					while(int(position.z - level.getOffset()) > 8)
					{
						level.popSlice();
					}
				}
				break;
		}
	}while(old_state != state && state != PLAYER_MOVING);

	path.update(delta);

	// TEMP
	if(Input::isKeyHold(GLFW_KEY_SPACE))
	{
		doStep(level);
	}

	if(Input::isKeyPressed(GLFW_KEY_Z))
	{
		setNextMove(glm::vec3(1.0f, 1.0f, 0.0f));
		setNextMove(glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if(Input::isKeyPressed(GLFW_KEY_X))
	{
		setNextMove(glm::vec3(0.0f, 1.0f, 1.0f));
		setNextMove(glm::vec3(0.0f, 0.0f, 1.0f));
	}

	if(Input::isKeyPressed(GLFW_KEY_A))
	{
		setNextMove(glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if(Input::isKeyPressed(GLFW_KEY_D))
	{
		setNextMove(glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	if(Input::isKeyPressed(GLFW_KEY_W))
	{
		setNextMove(glm::vec3(0.0f, 0.0f, 1.0f));
	}

	if(Input::isKeyPressed(GLFW_KEY_S))
	{
		setNextMove(glm::vec3(0.0f, 0.0f, -1.0f));
	}

	cube_outline.setColor(0, cubeTypes[type].color, true);
}

bool Player::canRotateLayer(Level& level, int layer)
{
	if(layer < 0 || layer >= LEVEL_NUMLAYERS) return false;

	return !level.isCubeLayer(layer, position + glm::vec3(0.0f, -1.0f, 0.0f));
}

void Player::draw(Camera& camera, Lighting& lighting, bool shadow)
{
	if(shadow)
	{
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);
	}

	if(rotation)
	{
		cube.model = glm::rotate(glm::mat4(1.0f), progress, rotation_axis) * glm::translate(glm::mat4(1.0f), rotation_center);
		cube.model = glm::translate(glm::mat4(1.0f), position - rotation_center) * cube.model;
	}else{
		cube.model = glm::translate(glm::mat4(1.0f), position + direction * progress / target_progress);
	}
	
	cube.draw(camera, shadow ? shadow_lighting : lighting);
	path.draw(camera, shadow ? shadow_lighting : lighting);
	
	if(type > 1)
	{
		cube_outline.model = cube.model;
		cube_outline.draw(camera, outline_lighting);
	}

	if(shadow)
	{
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
	}
}
