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
	float fall_time;
}cube_s;

class Cubes
{
	public:
		Cubes(int n, int current_n = -1, bool wireframe = false);
		~Cubes();

		void draw(Camera& camera, Lighting& lighting);
		void update();

		void setPosition(int i, glm::vec3 p, bool update = false);
		void setColor(int i, glm::vec3 c, bool update = false);
		void setIntro(int i, bool update = false);
		void setFalling(int i, bool update = false);
		void setFloatiness(float floatiness);
		void clearIntro(int i, bool update = false);

		void clear();

		int addCube(glm::vec3 p, glm::vec3 c, bool update = false);
		void removeCube(int id, bool update = false);
		void removeCube(glm::vec3 p, bool update = false);
		void removeDepth(float depth);

		glm::mat4 model;

	private:
		int current_n, n;
		cube_s* data;
		ShaderProgram& shader;
		GLuint vao, vbo;
		float floatiness;
		bool wireframe;
};

#endif
