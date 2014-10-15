#ifndef CVS_RENDER
#define CVS_RENDER

#include "CVS_Precompiled.h"
#include "CVS_SubSystem.h"
#include "CVS_2DTools.h"

struct CVS_RenderSystem;
struct CVS_Window;
struct CVS_Buffer;

struct CVS_VertexObject{
	GLuint VAO;
	CVS_VertexObject();
	void bindVAO();
	void unBind();
	void bindArrayBuffer( CVS_Buffer* buffer, unsigned int location, int size, CVS_Enum type = CVS_FLOAT, bool normalize = false, unsigned int stride = 0, unsigned int offset = 0);
	void bindElementBuffer( unsigned int dataSize, void* data, CVS_Enum usage);
	void drawElements(CVS_Enum type, unsigned int vertCount,unsigned int offset = 0);
	void drawArrays(CVS_Enum type, unsigned int vertCount, unsigned int offset = 0);
};

struct CVS_Buffer{
	GLuint buffer;
	GLenum target;
	CVS_Buffer(CVS_Enum target);
	void bindBuffer();
	void BufferData(unsigned int size, void* data);
	~CVS_Buffer();
};

struct CVS_Renderer{
public:
	SDL_GLContext m_glContext;
	CVS_Window* window;
	CVS_2DTools* tools;
	CVS_RenderSystem* renderSystem;
	CVS_Renderer(CVS_Window* window);
	void Clear();
	void SwapFrameBuffer();
	//CVS_Renderer* createChildRenderer();
};

struct CVS_RenderSystem:public CVS_SubSystem{
public:
	SDL_GLContext m_glContext;
	std::vector<CVS_Renderer*> renderers;
	std::map<std::string, CVS_RenderProgram*> programs;
	std::vector<CVS_VertexObject*> vertexArrays;
	CVS_RenderSystem();
	bool Initialize();
	bool End();
	CVS_Buffer* generateNewBuffer();
	CVS_Buffer* generateElementBuffer();
	CVS_Renderer* createNewRenderer(CVS_Window* window);
	CVS_RenderProgram* createNewShader(std::string name, char* vertpath, char* fragpath);
	CVS_VertexObject* createNewVertexObject();
};


#endif