#include "Canvas.h"
#include "CVS_Mesh.h"
#include "Vertex.h"

namespace Canvas {
CVS_Mesh::CVS_Mesh()
{

}

CVS_Mesh::CVS_Mesh(std::string name)
{
	this->name = name;
}

void CVS_Mesh::initialize()
{
	GLuint vertexBuffer, indiceBuffer;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	printf("mesh initialized!\n");

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indiceBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);

	glBindVertexArray(0);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indiceBuffer);

}

bool CVS_Mesh::loadMeshFromFile(char* filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	if (scene)
	{
		initMeshFromAiMesh(scene->mMeshes[0]);
	}
	return true;
}

CVS_Mesh* CVS_Mesh::initFromFbxNode(FbxNode* inNode)
{
	const char* srcMeshName;
	auto pSrcMesh = inNode->GetMesh();
	if (!pSrcMesh)
	{
		return nullptr;
	}
	else
	{
		srcMeshName = inNode->GetName();
		printf("Mesh::initFromFbxNode: Mesh processed: %s\n", srcMeshName);
	}
	auto lPolyCount = pSrcMesh->GetPolygonCount();

	auto pDstMesh = new CVS_Mesh();
	pDstMesh->name = srcMeshName;

	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndices = NULL;
	auto lMaterialMappingMode = FbxGeometryElement::eNone;
	if (pSrcMesh->GetElementMaterial())
	{
		lMaterialIndices = &pSrcMesh->GetElementMaterial()->GetIndexArray();
		lMaterialMappingMode = pSrcMesh->GetElementMaterial()->GetMappingMode();
		if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			FBX_ASSERT(lMaterialIndices->GetCount() == lPolyCount);
			if (lMaterialIndices->GetCount() == lPolyCount)
			{
				// Count the faces of each material
				for (int i = 0; i < lPolyCount; ++i)
				{
					const int lMaterialIndex = lMaterialIndices->GetAt(i);
					if (pDstMesh->m_subMeshes.size() < lMaterialIndex + 1)
					{
						pDstMesh->m_subMeshes.resize(lMaterialIndex + 1);
					}
					if (pDstMesh->m_subMeshes[lMaterialIndex] == NULL)
					{
						pDstMesh->m_subMeshes[lMaterialIndex] = new CVSSubMesh;
					}
					pDstMesh->m_subMeshes[lMaterialIndex]->m_triangleCount += 1;
				}

				// Make sure we have no "holes" (NULL) in the pDstMesh->m_subMeshes table. This can happen
				// if, in the loop above, we resized the pDstMesh->m_subMeshes by more than one slot.
				for (int i = 0; i < pDstMesh->m_subMeshes.size(); i++)
				{
					if (pDstMesh->m_subMeshes[i] == NULL)
						pDstMesh->m_subMeshes[i] = new CVSSubMesh;
				}

				// Record the offset (how many vertex)
				const int lMaterialCount = pDstMesh->m_subMeshes.size();
				int lOffset = 0;
				for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
				{
					pDstMesh->m_subMeshes[lIndex]->m_indexOffset = lOffset;
					lOffset += pDstMesh->m_subMeshes[lIndex]->m_triangleCount * 3;
					// This will be used as counter in the following procedures, reset to zero
					pDstMesh->m_subMeshes[lIndex]->m_triangleCount = 0;
				}
				FBX_ASSERT(lOffset == lPolyCount * 3);
			}
		}
	}

	// All faces will use the same material.
	if (pDstMesh->m_subMeshes.size() == 0)
	{
		pDstMesh->m_subMeshes.resize(1);
		pDstMesh->m_subMeshes[0] = new CVSSubMesh;
	}

	// Congregate all the data of a mesh to be cached in VBOs.
	// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
	bool hasNormal = pSrcMesh->GetElementNormalCount() > 0;
	bool hasUV = pSrcMesh->GetElementUVCount() > 0;
	bool bAllByControlPoint = true;
	auto lNormalMappingMode = FbxGeometryElement::eNone;
	auto lUVMappingMode = FbxGeometryElement::eNone;
	if (hasNormal)
	{
		lNormalMappingMode = pSrcMesh->GetElementNormal(0)->GetMappingMode();
		if (lNormalMappingMode == FbxGeometryElement::eNone)
		{
			hasNormal = false;
		}
		if (hasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
		{
			bAllByControlPoint = false;
		}
	}
	if (hasUV)
	{
		lUVMappingMode = pSrcMesh->GetElementUV(0)->GetMappingMode();
		if (lUVMappingMode == FbxGeometryElement::eNone)
		{
			hasUV = false;
		}
		if (hasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
		{
			bAllByControlPoint = false;
		}
	}

	// Temporary hacks
#define TRIANGLE_VERTEX_COUNT 3
	// Four floats for every position.
#define VERTEX_STRIDE 4
	// Three floats for every normal.
#define NORMAL_STRIDE 3
	// Two floats for every UV.
#define UV_STRIDE 2

	// Allocate the array memory, by control point or by polygon vertex.
	int lPolygonVertexCount = pSrcMesh->GetControlPointsCount();
	if (!bAllByControlPoint)
	{
		lPolygonVertexCount = lPolyCount * TRIANGLE_VERTEX_COUNT;
	}

	pDstMesh->vertices.resize(lPolygonVertexCount);
	pDstMesh->indices.resize(lPolygonVertexCount);

	FbxStringList lUVNames;
	pSrcMesh->GetUVSetNames(lUVNames);
	const char * lUVName = NULL;
	if (hasUV && lUVNames.GetCount())
	{
		lUVName = lUVNames[0];
	}

	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = pSrcMesh->GetControlPoints();
	glm::vec3 lCurrentVertex;
	glm::vec3 lCurrentNormal;
	glm::vec2 lCurrentUV;
	if (bAllByControlPoint)
	{
		const FbxGeometryElementNormal * lNormalElement = NULL;
		const FbxGeometryElementUV * lUVElement = NULL;
		if (hasNormal)
		{
			lNormalElement = pSrcMesh->GetElementNormal(0);
		}
		if (hasUV)
		{
			lUVElement = pSrcMesh->GetElementUV(0);
		}
		for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
		{
			// Save the vertex position.
			lCurrentVertex = glm::vec3(
				lControlPoints[lIndex][0],
				lControlPoints[lIndex][1],
				lControlPoints[lIndex][2]);
			pDstMesh->vertices[lIndex].position = lCurrentVertex;

			// Save the normal.
			if (hasNormal)
			{
				int lNormalIndex = lIndex;
				if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentNormal = glm::vec3(
					lNormalElement->GetDirectArray().GetAt(lNormalIndex)[0],
					lNormalElement->GetDirectArray().GetAt(lNormalIndex)[1],
					lNormalElement->GetDirectArray().GetAt(lNormalIndex)[2]);
				pDstMesh->vertices[lIndex].normal = lCurrentNormal;
			}

			// Save the UV.
			if (hasUV)
			{
				int lUVIndex = lIndex;
				if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentUV = glm::vec2(
					lUVElement->GetDirectArray().GetAt(lUVIndex)[0],
					lUVElement->GetDirectArray().GetAt(lUVIndex)[1]);
				pDstMesh->vertices[lIndex].uv = lCurrentUV;

			}
		}

	}

	int lVertexCount = 0;
	for (int i = 0; i < lPolyCount; ++i)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndices->GetAt(i);
		}

		// Where should I save the vertex attribute index, according to the material
		const int lIndexOffset = pDstMesh->m_subMeshes[lMaterialIndex]->m_indexOffset +
			pDstMesh->m_subMeshes[lMaterialIndex]->m_triangleCount * 3;
		for (int j = 0; j < TRIANGLE_VERTEX_COUNT; ++j)
		{
			const int lControlPointIndex = pSrcMesh->GetPolygonVertex(i, j);

			if (bAllByControlPoint)
			{
				pDstMesh->indices[lIndexOffset + j] = static_cast<unsigned int>(lControlPointIndex);
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				pDstMesh->indices[lIndexOffset + j] = static_cast<unsigned int>(lVertexCount);

				lCurrentVertex = glm::vec3(
					lControlPoints[lControlPointIndex].mData[0],
					lControlPoints[lControlPointIndex].mData[1],
					lControlPoints[lControlPointIndex].mData[2]);
				pDstMesh->vertices[lVertexCount].position = lCurrentVertex;

				if (hasNormal)
				{
					FbxVector4 lNormal;
					pSrcMesh->GetPolygonVertexNormal(i, j, lNormal);
					lCurrentNormal = glm::vec3(
						lNormal[0],
						lNormal[1],
						lNormal[2]);
					pDstMesh->vertices[lVertexCount].normal = lCurrentNormal;
				}

				if (hasUV)
				{
					bool lUnmappedUV;
					FbxVector2 lUV;
					pSrcMesh->GetPolygonVertexUV(i, j, lUVName, lUV, lUnmappedUV);
					lCurrentUV = glm::vec2(
						lUV[0],
						lUV[1]);
					pDstMesh->vertices[lVertexCount].uv = lCurrentUV;
				}
			}
			++lVertexCount;
		}
		pDstMesh->m_subMeshes[lMaterialIndex]->m_triangleCount += 1;
	}

	return pDstMesh;
}


bool CVS_Mesh::initMeshFromAiMesh(aiMesh* mesh)
{
	glm::vec2 nullUV(0, 0);
	this->name = std::string(mesh->mName.data);
	printf("Init mesh %s\n", name.c_str());
	for (int i = 0, e = mesh->mNumVertices; i < e; i++)
	{
		glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec2 uv = glm::vec2(mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : nullUV);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		this->vertices.push_back(Vertex(pos, uv, normal));
	}

	for (int i = 0, e = mesh->mNumFaces; i < e; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			printf("Triangle is no longer a triangle!! it may have more than 3 vertices!");
			return false;
		}
		this->indices.push_back(mesh->mFaces[i].mIndices[0]);
		this->indices.push_back(mesh->mFaces[i].mIndices[1]);
		this->indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	this->initialize();
	return true;
}

CVS_Mesh::~CVS_Mesh()
{}
}