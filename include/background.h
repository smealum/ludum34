#ifndef BACKGROUND_H
#define BACKGROUND_H

#define BG_NUMSTARS (512)

#include "gl.h"
#include "glm.h"
#include "shader.h"
#include "camera.h"

typedef struct
{
	glm::vec2 position;
	glm::vec4 color;
	float period, phase;
}star_s;

class Background
{
	public:
		Background();

		void draw();

	private:
		ShaderProgram& shader;
		GLuint vao, vbo;
		star_s data[BG_NUMSTARS];
};

#endif
