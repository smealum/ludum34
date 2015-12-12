#include "player.h"
#include "_math.h"
#include "input.h"

Player::Player():
	position(0.0f, 1.0f, 0.0f),
	direction(1.0f, 0.0f, 0.0f),
	angle(0.0f),
	state(PLAYER_IDLE),
	cube(1),
	cube_outline(1, 1, true),
	type(0)
{
	cube.setColor(0, glm::vec3(255.0f, 174.0f, 68.0f) * (1.4f / 255), true);

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

void Player::setNextMove(glm::vec3 _direction)
{
	moves.push(_direction);
}

void Player::startMoving(void)
{
	if(moves.empty() || state == PLAYER_MOVING) return;

	direction = moves.front();
	moves.pop();
	state = PLAYER_MOVING;
}

void Player::setType(unsigned char type)
{
	if(type >= 16) return;

	this->type = type;
}

void Player::updateCamera(Camera& camera)
{
	camera.setPosition(position + direction * float(angle / (M_PI / 2.0f)) + glm::vec3(-5.0f, 10.0f, -5.0f));
}

void Player::update(Level& level, float delta)
{
	switch(state)
	{
		case PLAYER_IDLE:
			{
				unsigned char cube_type = level.getCubeInfo(position);

				if(cube_type > 0)
				{
					setType(getCubeWireframeId(cube_type));
				}

				if(!moves.empty())
				{
					direction = moves.front();
					moves.pop();
					state = PLAYER_MOVING;
				}
			}
			break;
		case PLAYER_MOVING:
			angle += 3.0f * delta;
			if(angle >= M_PI / 2)
			{
				angle = 0.0f;
				position += direction;

				state = PLAYER_IDLE;

				while(int(position.z - level.getOffset()) > 8)
				{
					level.popSlice();
				}
			}
			break;
	}

	// TEMP
	if(Input::isKeyPressed(GLFW_KEY_A))
	{
		setNextMove(glm::vec3(1.0f, 0.0f, 0.0f));
		startMoving();
	}

	if(Input::isKeyPressed(GLFW_KEY_D))
	{
		setNextMove(glm::vec3(-1.0f, 0.0f, 0.0f));
		startMoving();
	}

	if(Input::isKeyPressed(GLFW_KEY_W))
	{
		setNextMove(glm::vec3(0.0f, 0.0f, 1.0f));
		startMoving();
	}

	if(Input::isKeyPressed(GLFW_KEY_S))
	{
		setNextMove(glm::vec3(0.0f, 0.0f, -1.0f));
		startMoving();
	}

	cube_outline.setColor(0, cubeTypes[type].color, true);
}

void Player::draw(Camera& camera, Lighting& lighting, bool shadow)
{
	if(shadow)
	{
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);
	}

	glm::vec3 v = glm::vec3(0.0f, 0.5f, 0.0f) - direction * 0.5f;
	cube.model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(direction.z, 0.0, -direction.x)) * glm::translate(glm::mat4(1.0f), v);
	cube.model = glm::translate(glm::mat4(1.0f), position - v) * cube.model;
	
	cube.draw(camera, shadow ? shadow_lighting : lighting);
	
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
