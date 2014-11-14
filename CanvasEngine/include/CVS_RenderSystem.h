#ifndef CVS_RENDER
#define CVS_RENDER

#include CVS_PRECOMPILED_H
#include "CVS_SubSystem.h"
#include "CVS_2DTools.h"
#include "CVS_GUI.h"
#include "CVS_RenderComponents.h"

struct CVS_RenderSystem;
struct CVS_Window;
struct CVS_Buffer;

struct CVS_RenderPackage{
	//Set up the render package for rendering.
	virtual void setUp();
	//Last set after all renderings are down. This step does not do anything in\
	//forward shading, only useful for deferred tactices.
	virtual void Render(CVS_Camera* cam, CVS_RenderScene* scenes);
};

struct CVS_DeferredRenderPackage:public CVS_RenderPackage{
	//Unlike most rendering aspects, render packages need to be implementation specific
	GLuint colorText;
	GLuint normalText;
	GLuint uvText;
	GLuint posText;
	GLuint depthText;
	GLuint frameBuffer;
	CVS_RenderSystem* system;
	CVS_DeferredRenderPackage(CVS_RenderSystem* system);
	void setUp();
	void Render(CVS_Camera* cam, CVS_RenderScene* scene);
	void GeometryPass(CVS_Camera* cam, CVS_RenderScene* scene);
	void LightingPass(CVS_Camera* cam, CVS_RenderScene* scene);
};

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
	void setScene(CVS_RenderScene* scene);
	//CVS_Renderer* createChildRenderer();
};

struct CVS_Font{
	TTF_Font* face;
	std::string fontName;
};

struct CVS_RenderSystem:public CVS_SubSystem{
public:
	int renderWidth, renderHeight;
	SDL_GLContext m_glContext;
	FT_Library lib;
	std::vector<CVS_Renderer*> renderers;
	std::map<std::string, CVS_RenderProgram*> programs;
	std::vector<CVS_VertexObject*> vertexArrays;
	std::vector<CVS_Font> fonts;

	//storage for 3D related data types
	std::vector<CVS_RenderScene*> scenes;
	std::vector<CVS_Mesh*> meshes;
	std::vector<CVS_Texture2D*> textures;

	CVS_RenderPackage* renderMode;
	CVS_RenderSystem();
	bool Initialize();
	bool End();

	//
	CVS_RenderProgram* getRenderProgram(std::string name);

	CVS_Renderer* createNewRenderer(CVS_Window* window);
	CVS_RenderProgram* createNewShader(std::string name, char* vertpath, char* fragpath);
	CVS_VertexObject* createNewVertexObject();
	CVS_Texture2D* generateNewTexture();
	CVS_RenderScene* createNewScene();
	std::vector<CVS_Mesh*> addMeshesFromaiScene(const aiScene* scene);
	CVS_IVEC2 getGlyphSize(char character, unsigned int font);
	bool loadFont(std::string name, char* fontPath);
};


#endif