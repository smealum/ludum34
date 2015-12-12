#ifndef CUBES_H
#define CUBES_H

#include "shader.h"
#include "camera.h"
#include "lighting.h"
#include "glm.h"

typedef struct
{
	glm::vec3 position;
	glm::vec3 color;
}cube_s;

class Cubes
{
	public:
		Cubes(int n);
		~Cubes();

		void draw(Camera& camera, Lighting& lighting);
		void update();

		void setPosition(int i, glm::vec3 p, bool update = false);
		void setColor(int i, glm::vec3 c, bool update = false);

		glm::mat4 model;

	private:
		int n;
		cube_s* data;
		ShaderProgram& shader;
		GLuint vao, vbo;
};

#endif
