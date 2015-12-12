#ifndef TEXTURE_H
#define TEXTURE_H

#include "gl.h"

class Texture
{
    public:
        Texture(const char* filename);
        ~Texture();

        void setFilter(GLint filter);

        void bind(GLenum target);

    private:
        int load(const char* filename);

        int id;
};

#endif
