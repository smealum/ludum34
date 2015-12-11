#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <vector>
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "glm.h"

typedef struct
{
	int v[3];
	int t[3];
	int n[3];
}objFace_s;

typedef struct
{
	int v, t, n;
}vertperm_s;

typedef struct
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
}vertex_s;

typedef struct
{
	GLuint ebo, vbo;
	std::string mtl;
	int faces;
}subobj_t;

class ObjModel
{
	public:
		ObjModel(const char* filename);

		int load(const char* filename);
		int loadmtl(const char* filename);

		void draw(Camera& camera);

		float getScale();

		glm::mat4 model;

	private:
		int generateGeometry(std::vector<objFace_s>& faces, char* mtl);
		int loadTexture(const char* filename);

		GLuint vao;
		
		ShaderProgram& shader;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> texcoords;
		std::vector<glm::vec3> normals;
		std::map<std::string, Texture*> textures;
		std::vector<subobj_t> subobject;
		float scale;
};

#endif
