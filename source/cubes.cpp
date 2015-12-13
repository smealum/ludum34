#include <iostream>
#include "cubes.h"

static Lighting wireframe_lighting;

Cubes::Cubes(int n, int _current_n, bool wireframe):
	model(glm::mat4(1.0f)),
	n(n),
	data(new cube_s[n]),
	shader(ShaderProgram::loadFromFile("shaders/cube.vsh", "shaders/simple.fsh", wireframe ? "shaders/cubes_wireframe.gsh" : "shaders/cube.gsh", wireframe ? "cube_wireframe" : "cube")),
	floatiness(1.0f),
	wireframe(wireframe),
	max_depth(0.0f)
{
	if(_current_n < 0) current_n = n;
	else current_n = _current_n;

	for(int i = 0; i < n; i++)
	{
		data[i].position = glm::vec3(0.0);
		data[i].color = glm::vec3(1.0);
		data[i].fall_time = 0.0;
		data[i].layer = 0;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(cube_s), data, GL_DYNAMIC_DRAW);

	shader.setBuffers(vao, vbo, -1);
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "out_color");
	shader.setAttribute("position", 3, GL_FALSE, 8, 0);
	shader.setAttribute("color", 3, GL_FALSE, 8, 3);
	shader.setAttribute("fall_time", 1, GL_FALSE, 8, 6);
	shader.setAttribute("layer", 1, GL_FALSE, 8, 7);

	// setup wireframe lighting
	wireframe_lighting.setObjectColor(true);
	wireframe_lighting.setLightEnabled(0, true);
	wireframe_lighting.setLightADSS(0, 1.0f, 0.0f, 0.0f, 0.0f);
	wireframe_lighting.setLightFresnel(0, 0.0f, 0.0f, 0.0f, 0.0f);
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

int Cubes::addCube(glm::vec3 p, glm::vec3 c, int layer, bool update)
{
	if(current_n >= n) return -1;

	int id = current_n;

	current_n++;

	// only works if we keep growing
	if(p.z > max_depth) max_depth = p.z;

	setPosition(id, p);
	setColor(id, c);
	data[id].fall_time = 0.0;
	data[id].layer = layer;

	if(update) this->update();

	return id;
}

void Cubes::removeCube(int id, bool update)
{
	if(id >= current_n) return;

	data[id] = data[current_n - 1];

	current_n--;

	if(update) this->update();
}

void Cubes::removeCube(glm::vec3 p, bool update)
{
	for(int i = 0; i < current_n;)
	{
		// only increment in the case when we don't delete
		// this way we can go over the replacement cube as well
		if(glm::distance(data[i].position, p) < 0.001f)
		{
			removeCube(i, update);
		}else{
			i++;
		}
	}
}

void Cubes::removeDepth(float depth, bool update)
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

	if(update) this->update();
}

void Cubes::update()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, current_n * sizeof(cube_s), data, GL_DYNAMIC_DRAW);
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

void Cubes::setFalling(int i, bool update)
{
	if(i >= current_n) return;

	data[i].fall_time = glfwGetTime();

	if(update) this->update();
}

void Cubes::setOutro(int i, bool update)
{
	if(i >= current_n) return;

	data[i].fall_time = glfwGetTime() + (max_depth - data[i].position.z) * 0.1f;

	if(update) this->update();
}

void Cubes::setIntro(int i, bool update)
{
	if(i >= current_n) return;

	data[i].fall_time = -glfwGetTime();

	if(update) this->update();
}

void Cubes::clearIntro(int i, bool update)
{
	if(i >= current_n) return;

	data[i].fall_time = 0.0f;

	if(update) this->update();
}

void Cubes::setFloatiness(float floatiness)
{
	this->floatiness = floatiness;
}

void Cubes::draw(Camera& camera, Lighting& lighting, int selected_layer)
{
	shader.use();

	shader.setBuffers(vao, vbo, -1);

	camera.updateCamera(shader);

	if(wireframe)
	{
		wireframe_lighting.update(shader);
		glDepthFunc(GL_LESS);
	}else{
		lighting.update(shader);
	}

	shader.setUniform("t", (float)glfwGetTime());
	shader.setUniform("floatiness", floatiness);
	shader.setUniform("model", model);
	shader.setUniform("bTexture", false);
	shader.setUniform("selected_layer", selected_layer);

	// printf("%f %f %f\n", data[0].position.x, data[0].position.y, data[0].position.z);

	glDrawArrays(GL_POINTS, 0, current_n);

	if(wireframe)
	{
		glDepthFunc(GL_LESS);
	}
}
