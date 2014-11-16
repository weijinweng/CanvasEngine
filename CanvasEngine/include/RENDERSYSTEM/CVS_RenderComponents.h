#ifndef CVS_RENDERSCENE_H
#define CVS_RENDERSCENE_H

#include CVS_PRECOMPILED_H
#include "CVS_Transform.h"
#include "CVS_RenderProgram.h"
#include "CVS_Light.h"

struct CVS_VertexObject;
struct CVS_RenderNode;
struct CVS_RenderPackage;
struct CVS_RenderSystem;
struct CVS_View;

struct Vertex{
	cvec3 position;
	cvec3 normal;
	cvec2 uv;
	Vertex();
	Vertex(cvec3 position, cvec2 uv, cvec3 normal);
};



struct CVS_Mesh{
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint VAO;
	void initialize();
	static CVS_Mesh* initFromFbxNode(FbxNode* inNode);
	void initializeFromAiMesh(const aiMesh* mesh);
	void Draw();

	// For every material, record the offsets in every VBO and triangle counts
	struct CVSSubMesh
	{
		CVSSubMesh() : m_indexOffset(0), m_triangleCount(0) {}

		int m_indexOffset;
		int m_triangleCount;
	};

	std::vector<CVSSubMesh*> m_subMeshes;
};

struct CVS_View{
	cmat4 Pers;
	cmat4 View;
};


struct CVS_Camera{
	CVS_Transform transform;

	cvec3 target;

	float aspectRatio;
	float FOV;
	float NearZ;
	float FarZ;

	cmat4 Persp;
	cmat4 View; 

	CVS_Camera(cvec3 pos = cvec3(4,3,4), cvec3 target = cvec3(0,0,0), float FOV = 45.0f, float AspectRatio = 16.0f/9.0f, float NearZ = 0.1f, float FarZ = 100.0f);

	void UpdateView();
	void UpdatePerspective();

	void RotateAroundTarget(float,float,float);
	void pointRotate(float, float, float);
	void shiftLocalPos(float x, float y, float z);

	CVS_View getView();
};


struct CVS_RenderProgramInstance{
	CVS_RenderProgram* program;
	GLuint	viewLoc, mvpLoc, modelLoc;
	std::vector<CVS_RenderNode*> children;
	CVS_RenderProgramInstance(CVS_RenderProgram* program);
	void addChild(CVS_RenderNode* node);
	void Render(CVS_View* cam);
};

struct CVS_TextureReference{
	CVS_Texture* texture;
	std::string name;
	int uniformLoc;
	CVS_TextureReference(CVS_Texture* texture, GLint uniformLoc);
};

struct CVS_RenderNode{
	void* msgData;
	int type;
	CVS_RenderProgramInstance* parent;
	CVS_Mesh* mesh;
	std::vector<CVS_TextureReference> textures;
	cmat4 modelMatrix;
	CVS_RenderNode(CVS_RenderProgramInstance* parent);
	int voidGetTextureHash(std::string texture);
	void BindTexture(CVS_Texture* texture, int location);
	void Render(CVS_View* view);
	void setMesh(CVS_Mesh* mesh);
};

struct CVS_RenderScene{
	std::vector<CVS_RenderNode*> nodes;
	std::vector<CVS_RenderProgramInstance*> programs;
	std::vector<CVS_Light> lights;
	CVS_RenderScene();
	CVS_RenderNode* createNewNode();
	void addProgram(std::string name);
	void Draw(CVS_View* view);
};

#endif