#ifndef TEXT_H
#define TEXT_H

#include "gl.h"
#include "glm.h"
#include "shader.h"

#define CHAR_SIZE (0.05f)

class Text
{
	public:
		Text(const char* string, glm::vec2 position, float size = CHAR_SIZE, int _buffer_length = 0);
		~Text();

		void update(const char* string);
		void setSize(float _size);

		void draw();

	private:
		ShaderProgram& shader;
		GLuint vao, vbo;
		char* data;
		int string_length, buffer_length;
		glm::vec2 position;
		float size;
};

void textInit();
void textExit();

#endif
