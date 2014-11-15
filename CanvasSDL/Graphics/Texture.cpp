#include "Texture.h"

namespace Canvas
{
	Texture::Texture()
	{}

	Texture::Texture(int width, int height)
	{}

	Texture::Texture(std::string name) :name(name)
	{}


	void Texture::loadFile(char* filePath)
	{
		SDL_Surface* img = IMG_Load(filePath);

		if (img == NULL)
		{
			printf("Error loading image %s\n", filePath);
			return;
		}

		glGenTextures(1, &this->textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		int Mode = GL_RGB;

		printf("pixel format = %d\n", img->format->BitsPerPixel);

		if (img->format->BytesPerPixel == 4)
		{
			Mode = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, Mode, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		this->textureWidth = img->w;
		this->textureHeight = img->h;
		this->filePath = filePath;
		this->textureType = Mode == GL_RGBA ? CVS_TXT_RGBA : CVS_TXT_RGB;
	}

	Texture::~Texture()
	{}

}