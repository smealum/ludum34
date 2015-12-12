#ifndef PATH_H
#define PATH_H

#include "gl.h"
#include "camera.h"
#include "lighting.h"
#include "level.h"
#include "shader.h"

#define PATH_MAXLENGTH (32)

typedef struct
{
	glm::vec3 position;
	glm::vec4 color;
}slab_s;

class Path
{
	public:
		Path(unsigned int length);

		void generate(Level& level, glm::vec3 start);
		void updateGeometry();

		void update(float delta);
		void draw(Camera& camera, Lighting& lighting);

		void initAnimation();

	private:
		ShaderProgram& shader;
		glm::mat4 model;
		GLuint vao, vbo;
		int length, cur_length;
		slab_s data[PATH_MAXLENGTH];
		float animation[PATH_MAXLENGTH];
		float fade;
};

#endif
