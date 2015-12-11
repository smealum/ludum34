#include "texture.h"
#include "lodepng.h"
#include "dbg.h"

Texture::Texture(const char* filename)
{
	id = load(filename);
}

Texture::~Texture()
{
	glDeleteTextures(1, (GLuint*)&id);
}

int Texture::load(const char* filename)
{
	if(!filename) return -1;

	log_info("loading texture : %s", filename);

	std::vector<unsigned char> image;
	unsigned int width, height;

	unsigned int error = lodepng::decode(image, width, height, filename);

	if(error)
	{
		log_err("decoder error %d : %s", error, lodepng_error_text(error));
		return -1;
	}

	GLuint texture = 0;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	return texture;
}

void Texture::bind(GLenum target)
{
	glBindTexture(GL_TEXTURE_2D, id);
}
