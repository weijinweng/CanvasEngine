#pragma once
#include "CVS_Resource.h"
#include "CVS_RenderComponents.h"

class CVS_Skeleton;

// For every material, record the offsets in every VBO and triangle counts
struct CVS_SubMesh
{
	CVS_SubMesh() : m_indexOffset(0), m_triangleCount(0) {}

	int m_indexOffset;
	int m_triangleCount;
};

struct CVS_Mesh : public CVS_Renderable, public CVS_Resource
{

	std::vector<CVS_SubMesh*> m_subMeshes;

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	CVS_Skeleton* m_pSkeleton;
	std::vector<VertexBoneData> m_vertexBones;

	cmat4 m_offset;
	GLuint m_VAO;

	void Init();
	int initFromFbxNode(FbxNode* _pNode);
	void initializeFromAiMesh(const aiMesh* mesh);
	virtual void Draw(EDrawMode _drawMode);
	void DrawSkeleton(CVS_Skeleton* _pSkeleton);
	void CalculateGlobalTransform(CVS_Bone* _pBone, cmat4 _parentTransform = cmat4());
	void UpdateSkeleton(CVS_RenderNode* _pRenderNode);
	CVS_Mesh() : CVS_Resource("Mesh", CVS_Resource::EType::Mesh), m_pSkeleton(), m_VAO() {}
};