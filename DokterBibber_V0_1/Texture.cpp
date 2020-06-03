#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string & fileName)
{
    const char* fileNameConverted = fileName.c_str();

    glGenTextures(1, &this->id);
    bind();
    stbi_set_flip_vertically_on_load(true);
    int width, heigth, comp;
    unsigned char* data = stbi_load(fileNameConverted, &width, &heigth, &comp, 4);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, heigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, this->id);
}
