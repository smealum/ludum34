#include "cubes.h"

Cubes::Cubes(int n):
	model(glm::mat4(1.0f)),
	n(n),
	data(new cube_s[n]),
	shader(ShaderProgram::loadFromFile("shaders/cube.vsh", "shaders/simple.fsh", "shaders/cube.gsh", "cube"))
{
	for(int i = 0; i < n; i++)
	{
		data[i].position = glm::vec3(0.0);
		data[i].color = glm::vec3(1.0);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(cube_s), data, GL_STATIC_DRAW);

	shader.setBuffers(vao, vbo, -1);
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "out_color");
	shader.setAttribute("position", 3, GL_FALSE, 6, 0);
	shader.setAttribute("color", 3, GL_FALSE, 6, 3);
}

void Cubes::update()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(cube_s), data, GL_STATIC_DRAW);
}

void Cubes::setPosition(int i, glm::vec3 p, bool update)
{
	if(i >= n) return;

	data[i].position = p;

	if(update) this->update();
}

void Cubes::setColor(int i, glm::vec3 c, bool update)
{
	if(i >= n) return;

	data[i].color = c;

	if(update) this->update();
}

void Cubes::draw(Camera& camera)
{
    shader.use();

    shader.setBuffers(vao, vbo, -1);

	camera.updateCamera(shader);

	shader.setUniform("model", model);

	glDrawArrays(GL_POINTS, 0, n);
}
