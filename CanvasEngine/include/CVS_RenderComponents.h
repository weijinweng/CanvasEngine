#ifndef CVS_RENDERSCENE_H
#define CVS_RENDERSCENE_H

#include "CVS_Precompiled.h"
#include "CVS_Transform.h"
#include "CVS_RenderProgram.h"
#include "CVS_Light.h"

struct CVS_VertexObject;
struct CVS_RenderNode;
struct CVS_RenderPackage;
struct CVS_RenderSystem;

//Debug uses only
struct CAM_CTRL{
public:
	bool Up , Down, Left, Right, accelerate, ShiftUp, ShiftDown;
	float horAngle, vertAngle;
	CAM_CTRL();
};

struct Vertex{
	cvec3 position;
	cvec2 uv;
	cvec3 normal;
	Vertex(cvec3 position, cvec2 uv, cvec3 normal);
};



struct CVS_Mesh{
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	CVS_VertexObject* vertexArray;
	GLuint TestVAO;
	void initialize();
	void initializeFromAiMesh(const aiMesh* mesh);
	void draw();
};

struct CVS_Camera:public CAM_CTRL{
	CVS_Transform transform;
	float FOV;
	float aspectRatio;
	float NearZ;
	float FarZ;
	cvec3 position;
	CVS_Camera(float FOV = 45.0f, float aspectRatio = 16.0f/9.0f, float nearZ = 0.1f, float farZ = 100.0f);
	cmat4 getPerspective();
	cmat4 getView();
	void parseInputs(SDL_Event e);
	void Update();
};

struct CVS_RenderProgramInstance{
	CVS_RenderProgram* program;
	int ViewLoc;
	int ModelLoc;
	int MVPLoc;
	std::vector<CVS_RenderNode*> children;
	CVS_RenderProgramInstance(CVS_RenderProgram* program);
	void addChild(CVS_RenderNode* node);
	void Render(CVS_Camera* cam);
	void remove(CVS_RenderNode* node);
};

struct CVS_TextureReference{
	CVS_Texture2D* texture;
	std::string name;
	GLuint uniformLoc;
	void bind();
};

struct CVS_RenderNode{
	int type;
	CVS_RenderProgramInstance* parent;
	CVS_Mesh* mesh;
	std::vector<CVS_TextureReference> textures;
	cmat4 modelMatrix;
	CVS_RenderNode(CVS_RenderProgramInstance* parent);
	void getTextures(CVS_RenderProgram* program);
	void bindTextures();
	void Render(CVS_Camera* cam);
	void setMesh(CVS_Mesh* mesh);
};

struct CVS_RenderScene{
	std::vector<CVS_RenderNode*> nodes;
	std::vector<CVS_RenderProgramInstance> programs;
	std::vector<CVS_Light> lights;
	std::vector<CVS_Camera> cameras;
	CVS_RenderScene();
	CVS_RenderNode* createNewNode();
	void loadRenderProgram(std::string name);
	void Render(int index);
	void Render();
};

#endif