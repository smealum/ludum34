#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "gl.h"
#include "window.h"
#include "settings.h"
#include "dbg.h"

#include "shader.h"
#include "camera.h"
#include "cubes.h"
#include "input.h"
#include "audio.h"
#include "obj.h"

Settings settings(800, 600);

int main(void)
{
	windowInit();
	glewInit();
	audioInit();

	Camera camera(0.001f, 100.0f);

	glViewport(0, 0, settings.width, settings.height);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float angle = 0.0f;
	float angle2 = 0.0f;

	Cubes cubes(2);
	Cubes cubes2(2);

	cubes.setPosition(0, glm::vec3(-1.0f, 0.0f, 0.0f));
	cubes.setPosition(1, glm::vec3(1.0f, 0.0f, 0.0f));
	cubes.update();

	cubes2.setPosition(0, glm::vec3(0.0f, -1.0f, 0.0f));
	cubes2.setPosition(1, glm::vec3(0.0f, 1.0f, 0.0f));
	cubes2.update();

	ObjModel obj("test.obj");

	double lastFrame = glfwGetTime();

	// Sound sound("drumloop.wav");

	// sound.play();

	while(windowUpdate())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if(Input::isKeyHold(GLFW_KEY_W)) camera.movePositionDouble(glm::dvec3(0.0, 0.0, -2.0) * deltaTime);
		if(Input::isKeyHold(GLFW_KEY_S)) camera.movePositionDouble(glm::dvec3(0.0, 0.0, 2.0) * deltaTime);
		if(Input::isKeyHold(GLFW_KEY_D)) camera.movePositionDouble(glm::dvec3(2.0, 0.0, 0.0) * deltaTime);
		if(Input::isKeyHold(GLFW_KEY_A)) camera.movePositionDouble(glm::dvec3(-2.0, 0.0, 0.0) * deltaTime);

		cubes.model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
		cubes.draw(camera);
		cubes2.draw(camera);

		obj.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(glm::mat3(0.1f)), angle2, glm::vec3(1.0f, 0.0f, 0.0f));
		obj.draw(camera);

		if(Input::isKeyHold(GLFW_KEY_E)) angle += 2.0f * deltaTime;
		if(Input::isKeyHold(GLFW_KEY_R)) angle2 += 2.0f * deltaTime;
	}

	audioExit();
	windowExit();

	exit(EXIT_SUCCESS);
}
