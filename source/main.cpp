#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "gl.h"
#include "window.h"
#include "settings.h"
#include "dbg.h"

#include "shader.h"
#include "camera.h"

Settings settings(800, 600);

int main(void)
{
	windowInit();
	glewInit();

	// ShaderProgram& simpleShader = ShaderProgram::loadFromFile("shaders/simple.vsh", "shaders/simple.fsh", "simple");
	ShaderProgram& simpleShader = ShaderProgram::loadFromFile("shaders/cube.vsh", "shaders/simple.fsh", "shaders/cube.gsh", "simple");

	Camera camera(0.001f, 100.0f);

	float points[] = {
	    0.0f,  2.0f, -0.0f, 1.0f, 0.0f, 0.0f,
	    1.0f, -1.0f, -0.0f, 1.0f, 0.0f, 0.0f,
	   -1.0f, -1.0f, -0.0f, 1.0f, 0.0f, 0.0f,
	};

	glViewport(0, 0, settings.width, settings.height);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	GLuint vbo = 0;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (points), points, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);

	glBindFragDataLocation(simpleShader.getHandle(), 0, "frag_colour");
	simpleShader.setAttribute("position", 3, GL_FALSE, 6, 0);
	simpleShader.setAttribute("color", 3, GL_FALSE, 6, 3);

	glm::mat4 model(1.0f);
	float angle = 0.0f;

	while(windowUpdate())
	{
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		simpleShader.use();
		simpleShader.setUniform("model", glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f)));
		camera.updateCamera(simpleShader);

		simpleShader.setBuffers(vao, vbo, -1);
		// glDrawArrays (GL_TRIANGLES, 0, 3);
		glDrawArrays (GL_POINTS, 0, 3);

		angle = 2.0f * glfwGetTime();
	}

	windowExit();

	exit(EXIT_SUCCESS);
}
