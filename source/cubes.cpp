#include "cubes.h"

Cubes::Cubes(int n, int _current_n):
	model(glm::mat4(1.0f)),
	n(n),
	data(new cube_s[n]),
	shader(ShaderProgram::loadFromFile("shaders/cube.vsh", "shaders/simple.fsh", "shaders/cube.gsh", "cube"))
	// shader(ShaderProgram::loadFromFile("shaders/cube.vsh", "shaders/simple.fsh", "shaders/cubes_wireframe.gsh", "cube"))
{
	if(_current_n < 0) current_n = n;
	else current_n = _current_n;

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

Cubes::~Cubes()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vao);
}

void Cubes::clear()
{
	current_n = 0;
}

int Cubes::addCube(glm::vec3 p, glm::vec3 c, bool update)
{
	if(current_n >= n) return -1;

	int id = current_n;

	current_n++;

	setPosition(id, p);
	setColor(id, c);

	if(update) this->update();

	return id;
}

void Cubes::removeCube(int id, bool update)
{
	if(id >= current_n) return;

	setPosition(id, data[current_n - 1].position);
	setColor(id, data[current_n - 1].color);

	current_n--;

	if(update) this->update();
}

void Cubes::removeDepth(float depth)
{
	for(int i = 0; i < current_n;)
	{
		// only increment in the case when we don't delete
		// this way we can go over the replacement cube as well
		if(fabs(data[i].position.z - depth) < 0.001f)
		{
			removeCube(i);
		}else{
			i++;
		}
	}
}

void Cubes::update()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, current_n * sizeof(cube_s), data, GL_STATIC_DRAW);
}

void Cubes::setPosition(int i, glm::vec3 p, bool update)
{
	if(i >= current_n) return;

	data[i].position = p;

	if(update) this->update();
}

void Cubes::setColor(int i, glm::vec3 c, bool update)
{
	if(i >= current_n) return;

	data[i].color = c;

	if(update) this->update();
}

void Cubes::draw(Camera& camera, Lighting& lighting)
{
	shader.use();

	shader.setBuffers(vao, vbo, -1);

	camera.updateCamera(shader);
	lighting.update(shader);

	shader.setUniform("model", model);
	shader.setUniform("bTexture", false);

	// printf("%f %f %f\n", data[0].position.x, data[0].position.y, data[0].position.z);

	glDrawArrays(GL_POINTS, 0, current_n);
}
