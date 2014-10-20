#ifndef CVS_RENDERPROGRAM
#define CVS_RENDERPROGRAM
#include "CVS_Precompiled.h"

struct CVS_Texture2D{
	GLuint texture;
	CVS_Texture2D();
	bool loadData(int width, int height, int mipmap, void* data);
	bool loadFile(char* filepath);
};

struct CVS_UniformLocation{
public:
	std::string name;
	GLenum type;
	GLuint location;
};

struct CVS_RenderProgram{
	GLuint ProgramName;
	std::string vertPath;
	std::string fragPath;
	std::vector<CVS_UniformLocation> uniforms;
	CVS_RenderProgram();
	int getUniformHash(std::string name);
	void bindVec4v(int uniformHash, float* pointer);
	void bindMat4v(int uniformHash, float* pointer);
	bool loadFile(char* vertpath, char* fragpath);
	void setAsCurrentProgram();
};


#endif