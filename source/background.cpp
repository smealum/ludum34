#include <stdlib.h>
#include "background.h"
#include "settings.h"

Background::Background():
	shader(ShaderProgram::loadFromFile("shaders/stars.vsh", "shaders/stars.fsh", "shaders/stars.gsh", "stars"))
{
	for(int i = 0; i < BG_NUMSTARS; i++)
	{
		data[i].position = glm::vec2((rand() * (2.0f / RAND_MAX)) - 1.0f, (rand() * (2.0f / RAND_MAX)) - 1.0f);
		data[i].color = glm::vec4(1.0);
		data[i].phase = (rand() * (1.0f / RAND_MAX));
		data[i].period = (rand() * (1.0f / RAND_MAX));
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, BG_NUMSTARS * sizeof(star_s), data, GL_DYNAMIC_DRAW);

	shader.setBuffers(vao, vbo, -1);
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "out_color");
	shader.setAttribute("position", 2, GL_FALSE, 8, 0);
	shader.setAttribute("color", 4, GL_FALSE, 8, 2);
	shader.setAttribute("period", 1, GL_FALSE, 8, 6);
	shader.setAttribute("phase", 1, GL_FALSE, 8, 7);
}

void Background::draw()
{
	shader.use();

	shader.setBuffers(vao, vbo, -1);

	// glEnable(GL_POINT_SPRITE);
	// glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	shader.setUniform("t", (float)glfwGetTime());
	shader.setUniform("aspect", settings.getAspectRatio());

	glDrawArrays(GL_POINTS, 0, BG_NUMSTARS);
	
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}
