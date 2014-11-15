#pragma once
#include "Canvas.h"

namespace Canvas
{
	//Textures storage unit.
	struct Texture
	{
		//Texture pointer in opengl Memory
		Texture();
		GLuint textureID;
		int textureWidth;
		int textureHeight;
		//Texture type
		CANVASENUM textureType;
		//String properties
		std::string filePath;
		std::string name;
		Texture(int width, int height);
		Texture(std::string name);
		void setName();
		void loadFile(char* filePath);
		//cleanup, does not clean up strings;
		void unloadFromMemory();
		void reloadMemory();
		~Texture();
	};
}