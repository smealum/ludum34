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
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(slab_s), data, GL_STATIC_DRAW);

	shader.setBuffers(vao, vbo, -1);
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "out_color");
	shader.setAttribute("position", 3, GL_FALSE, 6, 0);
	shader.setAttribute("color", 3, GL_FALSE, 6, 3);
}

void Path::updateGeometry()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, cur_length * sizeof(slab_s), data, GL_STATIC_DRAW);
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
		data[cur_length].color = glm::vec3(0.0f);
		cur_length++;
	}

	updateGeometry();
}

void Path::draw(Camera& camera, Lighting& lighting, bool shadow)
{
	shader.use();

	shader.setBuffers(vao, vbo, -1);

	camera.updateCamera(shader);
	lighting.update(shader);

	shader.setUniform("model", model);
	shader.setUniform("bTexture", false);

	glDrawArrays(GL_POINTS, 0, cur_length);
}
