#include "Canvas.h"

CVS_Texture::CVS_Texture(UINT target) :target(target)
{
	glGenTextures(1, &textureID);
}

bool CVS_Texture::loadData(UINT flags, UINT format, UINT type, int width, int height, int mipmap, void* data)
{
	Bind();
	glTexImage2D(target, mipmap, format, width, height, 0, flags, GL_FLOAT, data);
	return true;
}

bool CVS_Texture::loadFile(char* filePath)
{
	this->filepath = std::string(filePath);

	SDL_Surface* img = NULL;


	img = IMG_Load(filePath);


	if (img == NULL)
	{
		printf("Error loading image %s\n", filePath);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);

	int Mode = GL_RGB;

	if (img->format->BytesPerPixel == 4)
	{
		Mode = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, Mode, img->w, img->h, 0, Mode, GL_UNSIGNED_BYTE, img->pixels);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);



	this->width = img->w;
	this->height = img->h;
	this->filepath = filePath;
	this->type = Mode == Mode;
	SDL_FreeSurface(img);
	return true;
}

void CVS_Texture::Bind()
{
	glBindTexture(target, textureID);
}
