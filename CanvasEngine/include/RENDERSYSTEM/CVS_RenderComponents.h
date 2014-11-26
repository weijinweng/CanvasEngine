#ifndef CVS_RENDERSCENE_H
#define CVS_RENDERSCENE_H


#include "CVS_Transform.h"
#include "CVS_RenderProgram.h"
#include "CVS_Light.h"
#include "CVS_Resource.h"

struct CVS_VertexObject;
struct CVS_RenderNode;
struct CVS_RenderPackage;
struct CVS_RenderSystem;
struct CVS_View;

struct CVS_Bone;
struct CVS_Mesh;
struct CVS_Animation;

const uint8 MAX_BONES_PER_VERTEX = 6;
const uint8 MAX_BONES = 100;

#define TRIANGLE_VERTEX_COUNT 3
// Four floats for every position.
#define VERTEX_STRIDE 4
// Three floats for every normal.
#define NORMAL_STRIDE 3
// Two floats for every UV.
#define UV_STRIDE 2

enum AttribIndex
{
	POSITION = 0,
	UV = 1,
	NORMAL = 2,
	BONE_ID = 3,
	BONE_ID_2 = 4,
	BONE_WEIGHT = 5,
	BONE_WEIGHT_2 = 6,

};

struct Vertex
{
	cvec3 position;
	cvec2 uv;
	cvec3 normal;

	Vertex();
	Vertex(cvec3 position, cvec2 uv, cvec3 normal);
};

struct VertexBoneData
{
	uint32 BoneIDs[MAX_BONES_PER_VERTEX];
	float BoneWeights[MAX_BONES_PER_VERTEX];

	void AddBoneData(uint8 BoneID, float Weight)
	{
		for (uint8 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++)
		{
			if (BoneWeights[i] == 0.0f ||
				BoneWeights[i] == 1.0f /*Default 1.0f and Bone# 0*/)
			{
				BoneIDs[i] = BoneID;
				BoneWeights[i] = Weight;
				return;
			}
		}

		// should never get here - more bones than we have space for
		assert(0);
	}
	VertexBoneData() : BoneIDs(), BoneWeights()
	{
		BoneWeights[0] = 1.0f;
	}
};

struct CVS_Renderable
{
	enum class EDrawMode
	{
		Default,
		WireFrame,
		Skeleton
	};
	virtual void Draw(EDrawMode _drawMode) = 0;
};

struct CVS_View
{
	cmat4 Pers;
	cmat4 View;
};


struct CVS_Camera
{
	CVS_Transform transform;

	cvec3 target;

	float aspectRatio;
	float FOV;
	float NearZ;
	float FarZ;

	cmat4 Persp;
	cmat4 View;

	CVS_Camera(cvec3 pos = cvec3(4, 3, 4), cvec3 target = cvec3(0, 0, 0), float FOV = 45.0f, float AspectRatio = 16.0f / 9.0f, float NearZ = 0.1f, float FarZ = 100.0f);

	void UpdateView();
	void UpdatePerspective();

	void RotateAroundTarget(float, float, float);
	void pointRotate(float, float, float);
	void shiftLocalPos(float x, float y, float z);

	CVS_View getView();
};

struct CVS_RenderProgramInstance
{
	CVS_RenderProgram* program;
	GLuint	viewLoc, mvpLoc, modelLoc, m_boneLoc[MAX_BONES];
	std::vector<CVS_RenderNode*> children;
	CVS_RenderProgramInstance(CVS_RenderProgram* program);
	void addChild(CVS_RenderNode* node);
	void Render(CVS_View* cam);
};

struct CVS_TextureReference
{
	CVS_Texture* texture;
	std::string name;
	int uniformLoc;
	CVS_TextureReference(CVS_Texture* texture, GLint uniformLoc);
};

struct CVS_RenderNode
{
	void* msgData;
	int type;
	CVS_RenderProgramInstance* parent;
	CVS_Mesh* mesh;
	std::vector<CVS_TextureReference> textures;
	std::vector<cmat4> m_boneMats;
	cmat4 modelMatrix;
	CVS_RenderNode(CVS_RenderProgramInstance* parent);
	int voidGetTextureHash(std::string texture);
	void BindTexture(CVS_Texture* texture, int location);
	void Render(CVS_View* view);
	void setMesh(CVS_Mesh* mesh);
};

struct CVS_RenderScene
{
	std::vector<CVS_RenderNode*> nodes;
	std::vector<CVS_RenderProgramInstance*> programs;
	std::vector<CVS_Light> lights;
	CVS_RenderScene();
	CVS_RenderNode* createNewNode();
	void addProgram(std::string name);
	void Draw(CVS_View* view);
};

#endif