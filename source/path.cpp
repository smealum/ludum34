#include <math.h>
#include "path.h"

Path::Path(unsigned int length):
	shader(ShaderProgram::loadFromFile("shaders/cube.vsh", "shaders/simple.fsh", "shaders/slab.gsh", "path")),
	model(1.0f),
	length((length < PATH_MAXLENGTH) ? length : PATH_MAXLENGTH),
	cur_length(0)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(slab_s), data, GL_DYNAMIC_DRAW);

	shader.setBuffers(vao, vbo, -1);
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "out_color");
	shader.setAttribute("position", 3, GL_FALSE, 7, 0);
	shader.setAttribute("color", 4, GL_FALSE, 7, 3);
}

void Path::initAnimation()
{
	for(int i = 0; i < cur_length; i++)
	{
		animation[i] = 3.0f + 5.0f * float(cur_length - i) / cur_length;
	}
	fade = 0.0f;
}

void Path::updateGeometry()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, cur_length * sizeof(slab_s), data, GL_DYNAMIC_DRAW);
}

void Path::generate(Level& level, glm::vec3 start)
{
	if(!length) return;

	glm::vec3 position = start;

	cur_length = 0;
	for(int i = 0; i < length; i++)
	{
		level.getNextLocation(position, position);
		data[cur_length].position = position;
		data[cur_length].color = glm::vec4(glm::vec3(232.0f, 108.0f, 0.0f) * (1.4f / 255), 1.0f);
		cur_length++;
	}

	updateGeometry();
	initAnimation();
}

void Path::update(float delta)
{
	fade += 1.0f * delta;
	if(fade > 1.0f) fade = 1.0f;

	for(int i = 0; i < cur_length; i++)
	{
		animation[i] += 2.0f * delta;
		data[i].color.w = fade * (sin(animation[i]) + 1.0f) * 0.5f;
	}

	updateGeometry();
}

void Path::draw(Camera& camera, Lighting& lighting)
{
	shader.use();

	shader.setBuffers(vao, vbo, -1);

	camera.updateCamera(shader);
	lighting.update(shader);

	shader.setUniform("model", model);
	shader.setUniform("bTexture", false);

	glDrawArrays(GL_POINTS, 0, cur_length);
}
