#ifndef CVS_RENDER
#define CVS_RENDER

#include "CVS_Precompiled.h"
#include "CVS_SubSystem.h"
#include "CVS_2DTools.h"
#include "CVS_GUI.h"
#include "CVS_RenderComponents.h"

struct CVS_RenderSystem;
struct CVS_Window;
struct CVS_Buffer;

struct CVS_RenderPipeline{
	void SetUp();
	void Render(CVS_RenderScene* scene, CVS_View view);
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
	CVS_2DTools* tools;
	HGLRC m_glContext;
	HDC m_ParentHDC;
	CVS_RenderSystem* renderSystem;
	CVS_Renderer(HDC);
	void Clear();
	void SwapFrameBuffer();
	void setScene(CVS_RenderScene* scene);
	void Render(CVS_RenderScene* scene, CVS_View view);
	//CVS_Renderer* createChildRenderer();
};

struct CVS_Font{
	FT_Face face;
	std::string fontName;
};

struct CVS_RenderSystem:public CVS_SubSystem{
public:
	int renderWidth, renderHeight;
	int m_GridNum;
	GLint m_GridMVP;

	FT_Library lib;
	CVS_RenderProgram* m_GridDraw;
	GLuint gridVAO;
	CVS_RenderProgram* m_DefaultProgram;
	CVS_Texture2D* m_DefaultTexture;
	std::vector<CVS_Renderer*> renderers;
	std::map<std::string, CVS_RenderProgram*> programs;
	std::vector<CVS_VertexObject*> vertexArrays;
	std::vector<CVS_Font> fonts;

	HGLRC m_glContext;

	CVS_RenderPipeline* pipeline;

	//storage for 3D related data types
	std::vector<CVS_RenderScene*> scenes;
	std::vector<CVS_Mesh*> meshes;
	std::vector<CVS_Texture2D> textures;

	CVS_RenderSystem();
	bool Initialize();
	bool End();

	//
	CVS_RenderProgram* getRenderProgram(std::string name);


	CVS_Renderer* createNewRenderer(HDC view);
	CVS_RenderProgram* createNewShader(std::string name, char* vertpath, char* fragpath);
	CVS_VertexObject* createNewVertexObject();
	CVS_RenderScene* createNewScene();
	std::vector<CVS_Mesh*> addMeshesFromaiScene(const aiScene* scene);
	CVS_IVEC2 getGlyphSize(char character, unsigned int font);
	bool loadFont(std::string name, char* fontPath);
};


#endif