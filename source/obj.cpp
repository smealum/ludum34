#include "obj.h"
#include <cstdio>
#include <iostream>
#include <string>

ObjModel::ObjModel(const char* filename):
	model(glm::mat4(1.0f)),
	shader(ShaderProgram::loadFromFile("shaders/simple.vsh", "shaders/simple.fsh", "simple_obj"))
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	load(filename);
	
	shader.setBuffers(vao, -1, -1);
	shader.use();
	shader.setAttribute("position", 3, GL_FALSE, 8, 0);
	shader.setAttribute("texcoord", 2, GL_FALSE, 8, 3);
	shader.setAttribute("color", 2, GL_FALSE, 8, 3);
	shader.setAttribute("normal", 3, GL_FALSE, 8, 5);
}

int ObjModel::load(const char* filename)
{
	if (!filename)return -1;

	FILE* f = fopen(filename, "r");
	if (!f)return -1;

	std::vector<objFace_s> faces;

	char line[1024];

	scale = 0.0f;

	static char cur_mtl[128] = "";

	while (fgets(line, 1024, f))
	{
		if (!line[0])continue;
		char* ptr = line;

		//skip leading spaces
		while (*ptr == ' ')ptr++;

		switch (*ptr++)
		{
		case 'm':
			ptr--;
			if (!memcmp(ptr, "mtllib", 6))
			{
				static char mtl[128];
				sscanf(ptr, "mtllib %s", mtl);
				loadmtl(mtl);
			}
			break;
		case 'u':
			ptr--;
			if (!memcmp(ptr, "usemtl", 6))
			{
				generateGeometry(faces, cur_mtl);
				sscanf(ptr, "usemtl %s", cur_mtl);
			}
			break;
		case 'v':
			switch (*ptr++)
			{
			case ' ':
			{
						glm::vec3 v;
						if (sscanf(ptr, "%f %f %f", &v.x, &v.y, &v.z) != 3)continue;
						if (fabs(v.x) > scale) scale = fabs(v.x);
						if (fabs(v.y) > scale) scale = fabs(v.y);
						if (fabs(v.z) > scale) scale = fabs(v.z);
						vertices.push_back(v);
			}
				break;
			case 't':
			{
						glm::vec2 t;
						if (sscanf(ptr, "%f %f", &t.x, &t.y) != 2)continue;
						texcoords.push_back(t);
			}
				break;
			case 'n':
			{
						glm::vec3 n;
						if (sscanf(ptr, "%f %f %f", &n.x, &n.y, &n.z) != 3)continue;
						normals.push_back(n);
			}
				break;
			}
			break;
		case 'f':
		{
					objFace_s f;
					if (sscanf(ptr, "%d/%d/%d %d/%d/%d %d/%d/%d", &f.v[0], &f.t[0], &f.n[0], &f.v[1], &f.t[1], &f.n[1], &f.v[2], &f.t[2], &f.n[2]) != 9)continue;
					f.v[0]--; f.v[1]--; f.v[2]--;
					f.n[0]--; f.n[1]--; f.n[2]--;
					f.t[0]--; f.t[1]--; f.t[2]--;
					faces.push_back(f);
		}
			break;
		}
	}

	generateGeometry(faces, cur_mtl);

	std::cout << vertices.size() << " " << faces.size() << std::endl;

	fclose(f);

	return 0;
}

int ObjModel::loadmtl(const char* filename)
{
	if (!filename)return -1;

	std::cout << "loading " << filename << std::endl;

	FILE* f = fopen(filename, "r");
	if (!f) return -1;

	char line[1024];

	static char lastmtl[128];
	static char lastfile[128];

	while (fgets(line, 1024, f))
	{
		if(!line[0]) continue;
		char* ptr = line;

		//skip leading spaces
		while (*ptr == ' ')ptr++;

		if (!memcmp(ptr, "newmtl", 6))
		{
			sscanf(ptr, "newmtl %s", lastmtl);
		}else if (!memcmp(ptr, "map_Kd", 6))
		{
			sscanf(ptr, "map_Kd %s", lastfile);
			textures[lastmtl] = loadTexture(lastfile);
		}
	}

	fclose(f);

	return 0;
}

int ObjModel::loadTexture(const char* filename)
{
	if (!filename)return -1;

	// //std::cout << "loading " << filename << std::endl;

	// GLuint texture = 0;
	// cv::Mat img = cv::imread(filename);
	// glGenTextures(1, &texture);

	// glBindTexture(GL_TEXTURE_2D, texture);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.ptr());

	// return texture;
	return -1;
}

int ObjModel::generateGeometry(std::vector<objFace_s>& faces, char* mtl)
{
	if (!vertices.size() || !texcoords.size() || !normals.size() || !faces.size())return -1;

	std::vector<vertex_s> interleaved;
	std::vector<unsigned short> indices;
	std::vector<vertperm_s> permutation;

	// not really efficient but it only happens once at load time so it should be ok-ish
	// could be improved by using a better-suited data structure for lookups
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		glm::ivec3 index;
		for (int j = 2; j >= 0; j--)
		{
			int v = faces[i].v[j];
			int t = faces[i].t[j];
			int n = faces[i].n[j];
			unsigned short k;

			for (k = 0; k < permutation.size(); k++)
			{
				if (permutation[k].v == v && permutation[k].n == n && (permutation[k].t == t || (texcoords[permutation[k].t].s == texcoords[t].s && texcoords[permutation[k].t].t == texcoords[t].t)))break;
			}

			if (k == permutation.size())
			{
				const vertperm_s vertperm = { v, t, n };
				const vertex_s vertex = { vertices[v], texcoords[t], normals[n] };

				permutation.push_back(vertperm);
				interleaved.push_back(vertex);
			}

			index[j] = k;
			indices.push_back(k);
		}
	}

	printf("permutation %d vs %d vs %d vs %d (%d)\n", (int)permutation.size(), (int)faces.size() * 3, (int)vertices.size(), (int)texcoords.size(), (int)indices.size());

	//load geometry
	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, interleaved.size()*sizeof(vertex_s), &interleaved[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	subobj_t subobj = { ebo, vbo, mtl, (int)faces.size() };
	subobject.push_back(subobj);

	faces.clear();

	return 0;
}

float ObjModel::getScale(void)
{
	return scale;
}

void ObjModel::draw(Camera& camera)
{
	shader.use();

	shader.setUniform("model", model);
	camera.updateCamera(shader);

	for (std::vector<subobj_t>::iterator it = subobject.begin(); it != subobject.end(); ++it)
	{
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, textures[it->mtl]);

		shader.setBuffers(vao, it->vbo, it->ebo);

		shader.setAttribute("position", 3, GL_FALSE, 8, 0);
		shader.setAttribute("texcoord", 2, GL_FALSE, 8, 3);
		shader.setAttribute("color", 2, GL_FALSE, 8, 3);
		shader.setAttribute("normal", 3, GL_FALSE, 8, 5);

		glDrawElements(GL_TRIANGLES, it->faces * 3, GL_UNSIGNED_SHORT, NULL);
	}
}
