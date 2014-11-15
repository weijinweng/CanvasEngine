#ifndef CVS_RENDERPROGRAM
#define CVS_RENDERPROGRAM
#include "CVS_Precompiled.h"

struct CVS_Texture{
	UINT target;
	GLuint textureID;
	int height;
	int width;
	std::string filepath;
	UINT type;
	CVS_Texture(UINT target);
	bool loadData(UINT flags, UINT format, UINT type, int width, int height, int mipmap, void* data);
	bool loadFile(char* filepath);
	void Bind();
};

struct CVS_UniformLocation{
public:
	std::string name;
	GLenum type;
	GLuint location;
};

struct CVS_RenderProgram{
	GLuint programID;
	std::string vertPath;
	std::string fragPath;
	std::vector<CVS_UniformLocation> uniforms;
	CVS_RenderProgram();
	int getUniformHash(std::string name);
	bool loadFile(char* vertpath, char* fragpath);
	void setAsCurrentProgram();
};


#endif