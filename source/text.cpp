#include <stdlib.h>
#include "text.h"
#include "shader.h"
#include "texture.h"
#include "settings.h"

static Texture* font;

void textInit()
{
	font = new Texture("font.png");
	font->setFilter(GL_NEAREST);
}

void textExit()
{
	delete font;
}

Text::Text(const char* string, glm::vec2 position, float size, int _buffer_length):
	shader(ShaderProgram::loadFromFile("shaders/text.vsh", "shaders/text.fsh", "shaders/text.gsh", "text")),
	data(NULL),
	position(position),
	size(size)
{
	string_length = strlen(string);
	if(_buffer_length == 0) buffer_length = string_length;

	data = (char*)malloc(sizeof(char) * buffer_length);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);

	update(string);

	shader.setBuffers(vao, vbo, -1);
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "out_color");
	shader.setAttribute("character", 1, GL_FALSE, 1, 0, GL_UNSIGNED_BYTE);
}

Text::~Text()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vao);
}

void Text::update(const char* string)
{
	if(!string) return;

	strncpy(data, string, buffer_length);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer_length * sizeof(char), data, GL_DYNAMIC_DRAW);

	string_length = strlen(string);
	if(buffer_length < string_length) string_length = buffer_length;
}

void Text::setSize(float _size)
{
	size = _size;
}

void Text::draw()
{
	shader.use();

	shader.setBuffers(vao, vbo, -1);

	shader.setUniform("position", position);
	shader.setUniform("CHAR_SIZE", size);
	shader.setUniform("aspect", settings.getAspectRatio());

	glActiveTexture(GL_TEXTURE0);
	if(font) font->bind(GL_TEXTURE_2D);

	glDrawArrays(GL_POINTS, 0, string_length);
}
