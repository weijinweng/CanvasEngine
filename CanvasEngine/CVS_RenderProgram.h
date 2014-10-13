#ifndef CVS_RENDERPROGRAM
#define CVS_RENDERPROGRAM
#include "CVS_Precompiled.h"

struct CVS_Texture2D{
	GLuint texture;
	CVS_Texture2D();
	bool loadData(int width, int height, int mipmap, void* data);
	bool loadFile(char* filepath);
};

struct CVS_RenderProgram{
	GLuint ProgramName;
	std::string vertPath;
	std::string fragPath;
	CVS_RenderProgram();
	bool loadFile(char* vertpath, char* fragpath);
};


#endif