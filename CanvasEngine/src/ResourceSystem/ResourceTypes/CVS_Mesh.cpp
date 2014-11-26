#include "Canvas.h"
#include "CVS_Mesh.h"

//TODO: remove this include
#include "CVS_Bone.h"

Vertex::Vertex() : position(glm::vec3()), uv(glm::vec2()), normal(glm::vec3())
{

}

Vertex::Vertex(cvec3 pos, cvec2 uv, cvec3 normal) : position(pos), uv(uv), normal(normal)
{

}

void CVS_Mesh::Init()
{
	GLuint vertexBuffer, boneBuffer, indexBuffer;
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	printf("mesh initializing\n");

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &boneBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(AttribIndex::POSITION);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(AttribIndex::UV);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(AttribIndex::NORMAL);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindBuffer(GL_ARRAY_BUFFER, boneBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBones.size() * sizeof(VertexBoneData), &m_vertexBones[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(AttribIndex::BONE_ID);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), 0);
	glEnableVertexAttribArray(AttribIndex::BONE_ID_2);
	glVertexAttribIPointer(4, MAX_BONES_PER_VERTEX - 4, GL_INT, sizeof(VertexBoneData), (void*)(sizeof(uint32)* 4));

	glEnableVertexAttribArray(AttribIndex::BONE_WEIGHT);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(sizeof(uint32)* 6));
	glEnableVertexAttribArray(AttribIndex::BONE_WEIGHT_2);
	glVertexAttribPointer(6, MAX_BONES_PER_VERTEX - 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(sizeof(uint32)* 10));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBindVertexArray(0);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &boneBuffer);
	glDeleteBuffers(1, &indexBuffer);
}

void CVS_Mesh::initializeFromAiMesh(const aiMesh* mesh)
{
	glm::vec2 nullUV(0, 0);
	this->m_name = std::string(mesh->mName.data);

	for (int i = 0, e = mesh->mNumVertices; i < e; i++)
	{
		glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec2 uv = glm::vec2(mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : nullUV);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		this->m_vertices.push_back(Vertex(pos, uv, normal));
	}

	for (int i = 0, e = mesh->mNumFaces; i < e; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			printf("Triangle is no longer a triangle!! it may have more than 3 vertices!");
			return;
		}
		this->m_indices.push_back(mesh->mFaces[i].mIndices[0]);
		this->m_indices.push_back(mesh->mFaces[i].mIndices[1]);
		this->m_indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	this->Init();

	printf("Init mesh %s\n", mesh->mName.C_Str());

	return;
}

int CVS_Mesh::initFromFbxNode(FbxNode* _pNode)
{
	auto pInMesh = _pNode->GetMesh();
	if (!pInMesh)
	{
		printf("Mesh::initFromFbxNode: Not a valid mesh node. %s\n", _pNode->GetName());
		return 1;
	}
	else
	{
		printf("Mesh::initFromFbxNode: Mesh processed: %s\n", _pNode->GetName());
	}

	this->m_name = _pNode->GetName();

	// Count the polygon count of each material
	auto lPolyCount = pInMesh->GetPolygonCount();

#pragma region
	FbxLayerElementArrayTemplate<int>* lMaterialIndices = NULL;
	auto lMaterialMappingMode = FbxGeometryElement::eNone;
	if (pInMesh->GetElementMaterial())
	{
		lMaterialIndices = &pInMesh->GetElementMaterial()->GetIndexArray();
		lMaterialMappingMode = pInMesh->GetElementMaterial()->GetMappingMode();
		if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			FBX_ASSERT(lMaterialIndices->GetCount() == lPolyCount);
			if (lMaterialIndices->GetCount() == lPolyCount)
			{
				// Count the faces of each material
				for (int i = 0; i < lPolyCount; ++i)
				{
					const int lMaterialIndex = lMaterialIndices->GetAt(i);
					if (this->m_subMeshes.size() < lMaterialIndex + 1)
					{
						this->m_subMeshes.resize(lMaterialIndex + 1);
					}
					if (this->m_subMeshes[lMaterialIndex] == NULL)
					{
						this->m_subMeshes[lMaterialIndex] = new CVS_SubMesh;
					}
					this->m_subMeshes[lMaterialIndex]->m_triangleCount += 1;
				}

				// Make sure we have no "holes" (NULL) in the pDstMesh->m_subMeshes table. This can happen
				// if, in the loop above, we resized the pDstMesh->m_subMeshes by more than one slot.
				for (int i = 0; i < this->m_subMeshes.size(); i++)
				{
					if (this->m_subMeshes[i] == NULL)
						this->m_subMeshes[i] = new CVS_SubMesh;
				}

				// Record the offset (how many vertex)
				const int lMaterialCount = this->m_subMeshes.size();
				int lOffset = 0;
				for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
				{
					this->m_subMeshes[lIndex]->m_indexOffset = lOffset;
					lOffset += this->m_subMeshes[lIndex]->m_triangleCount * 3;
					// This will be used as counter in the following procedures, reset to zero
					this->m_subMeshes[lIndex]->m_triangleCount = 0;
				}
				FBX_ASSERT(lOffset == lPolyCount * 3);
			}
		}
	}

	// All faces will use the same material.
	if (this->m_subMeshes.size() == 0)
	{
		this->m_subMeshes.resize(1);
		this->m_subMeshes[0] = new CVS_SubMesh;
	}
#pragma endregion Create SubMeshes Based on Materials

	// Fill all attributes here
	/*
	double* pDouble = _pNode->PreRotation.Get().mData;
	auto rot1 = cvec3(pDouble[0], pDouble[1], pDouble[2]);
	auto PreRotation = cquat(cvec3(pDouble[0] * RAD_CONV, pDouble[1] * RAD_CONV, pDouble[2] * RAD_CONV));
	// Note: y and z are flipped here because we want Y-up translation
	pDouble = _pNode->LclTranslation.Get().mData;
	auto LclTranslation = cvec3(pDouble[0], pDouble[2], pDouble[1]);
	pDouble = _pNode->LclRotation.Get().mData;
	auto rot = cvec3(pDouble[0], pDouble[1], pDouble[2]);
	auto LclRotation = cquat(cvec3(pDouble[0] * RAD_CONV, pDouble[1] * RAD_CONV, pDouble[2] * RAD_CONV));
	pDouble = _pNode->LclScaling.Get().mData;
	auto LclScaling = cvec3(pDouble[0], pDouble[1], pDouble[2]);
	// 	FbxAMatrix lGeometryOffset = FbxAMatrix(lT, lR, lS);
	// 	const FbxAMatrix* pOffset = &lGeometryOffset;
	// 	this->m_position = LclTranslation;
	// 	this->m_orientation = LclRotation;
	// 	FbxAMatrixToMat4(pOffset, tempMatrix);
	cmat4 matTranslation, matRotation, matScaling, matSRT;
	matTranslation = glm::translate(matTranslation, LclTranslation);
	matScaling = glm::scale(matScaling, LclScaling);
	matRotation = glm::mat4_cast(LclRotation) * matRotation;// LclRotation;
	matSRT = matTranslation * matRotation * matScaling;
	*/
#pragma region
	// Congregate all the data of a mesh to be cached in VBOs.
	// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
	bool hasNormal = pInMesh->GetElementNormalCount() > 0;
	bool hasUV = pInMesh->GetElementUVCount() > 0;
	bool bAllByControlPoint = true;
	auto lNormalMappingMode = FbxGeometryElement::eNone;
	auto lUVMappingMode = FbxGeometryElement::eNone;
	if (hasNormal)
	{
		lNormalMappingMode = pInMesh->GetElementNormal(0)->GetMappingMode();
		if (lNormalMappingMode == FbxGeometryElement::eNone)
		{
			hasNormal = false;
		}
		// TODO: Really should be hasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint
		// Here, but the API tell use the geometry layout the other way around.
		// If it contains split normals, it tells you it's by control point, and vice versa
		// DON'T TRUST ANYONE
		if (hasNormal && lNormalMappingMode == FbxGeometryElement::eByControlPoint)
		{
			bAllByControlPoint = false;
		}
	}
	if (hasUV)
	{
		lUVMappingMode = pInMesh->GetElementUV(0)->GetMappingMode();
		if (lUVMappingMode == FbxGeometryElement::eNone)
		{
			hasUV = false;
		}
		if (hasUV && lUVMappingMode == FbxGeometryElement::eByControlPoint)
		{
			bAllByControlPoint = false;
		}
	}

	// TODO: Fix Temporary hack
#define TRIANGLE_VERTEX_COUNT 3
	// Four floats for every position.
#define VERTEX_STRIDE 4
	// Three floats for every normal.
#define NORMAL_STRIDE 3
	// Two floats for every UV.
#define UV_STRIDE 2

	// Allocate the array memory, by control point or by polygon vertex.
	int lPolygonVertexCount = pInMesh->GetControlPointsCount();
	if (!bAllByControlPoint)
	{
		lPolygonVertexCount = lPolyCount * TRIANGLE_VERTEX_COUNT;
	}

	this->m_vertices.resize(lPolygonVertexCount);
	this->m_indices.resize(lPolyCount * TRIANGLE_VERTEX_COUNT);
	this->m_vertexBones.resize(lPolygonVertexCount);

	FbxStringList lUVNames;
	pInMesh->GetUVSetNames(lUVNames);
	const char * lUVName = NULL;
	if (hasUV && lUVNames.GetCount())
	{
		lUVName = lUVNames[0];
	}

	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = pInMesh->GetControlPoints();
	glm::vec3 lCurrentVertex;
	glm::vec3 lCurrentNormal;
	glm::vec2 lCurrentUV;
	if (bAllByControlPoint)
	{
		const FbxGeometryElementNormal * lNormalElement = NULL;
		const FbxGeometryElementUV * lUVElement = NULL;
		if (hasNormal)
		{
			lNormalElement = pInMesh->GetElementNormal(0);
		}
		if (hasUV)
		{
			lUVElement = pInMesh->GetElementUV(0);
		}
		for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
		{
			// Save the vertex position.
			lCurrentVertex = glm::vec3(
				lControlPoints[lIndex][0],
				lControlPoints[lIndex][1],
				lControlPoints[lIndex][2]);

			this->m_vertices[lIndex].position = lCurrentVertex;

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
				this->m_vertices[lIndex].normal = lCurrentNormal;
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
				this->m_vertices[lIndex].uv = lCurrentUV;

			}
		}

	}
	int lVertexCount = 0;
	for (int polyIndex = 0; polyIndex < lPolyCount; ++polyIndex)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndices->GetAt(polyIndex);
		}

		// Where should I save the vertex attribute index, according to the material
		const int lIndexOffset = this->m_subMeshes[lMaterialIndex]->m_indexOffset +
			this->m_subMeshes[lMaterialIndex]->m_triangleCount * 3;
		for (int j = 0; j < TRIANGLE_VERTEX_COUNT; ++j)
		{
			const int lControlPointIndex = pInMesh->GetPolygonVertex(polyIndex, j);

			if (bAllByControlPoint)
			{
				this->m_indices[lIndexOffset + j] = static_cast<unsigned int>(lControlPointIndex);
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				this->m_indices[lIndexOffset + j] = static_cast<unsigned int>(lVertexCount);

				lCurrentVertex = glm::vec3(
					lControlPoints[lControlPointIndex].mData[0],
					lControlPoints[lControlPointIndex].mData[1],
					lControlPoints[lControlPointIndex].mData[2]);
				this->m_vertices[lVertexCount].position = lCurrentVertex;

				if (hasNormal)
				{
					FbxVector4 lNormal;
					pInMesh->GetPolygonVertexNormal(polyIndex, j, lNormal);
					lCurrentNormal = glm::vec3(
						lNormal[0],
						lNormal[1],
						lNormal[2]);
					this->m_vertices[lVertexCount].normal = lCurrentNormal;
				}

				if (hasUV)
				{
					bool lUnmappedUV;
					FbxVector2 lUV;
					pInMesh->GetPolygonVertexUV(polyIndex, j, lUVName, lUV, lUnmappedUV);
					lCurrentUV = glm::vec2(
						lUV[0],
						lUV[1]);
					this->m_vertices[lVertexCount].uv = lCurrentUV;
				}
			}
			++lVertexCount;
		}
		this->m_subMeshes[lMaterialIndex]->m_triangleCount += 1;
	}

#pragma endregion Inject Data: vertices, normal, UV
	return true;
}

void _UpdateBoneRecursive(CVS_Bone* _pBone, CVS_RenderNode* _pRenderNode, cmat4 _parentTransform = cmat4())
{
	auto M = _parentTransform;
	auto t = _pBone->m_translation;
	auto r = _pBone->m_rotation;
	auto s = _pBone->m_scaling;
	auto o = _pBone->m_offset;
	/*
	auto T = cvec3();
	auto R = cquat();
	auto S = cvec3(1.0f, 1.0f, 1.0f);


	auto rVec3 = glm::degrees(glm::eulerAngles(r));
	auto RVec3 = cvec3();
	auto R_SVec3 = cvec3();
	auto RQ = cquat();
	auto R_SQ = cquat();

	auto SVec3 = cvec3();

	if (_pBone->m_pParent)
	{
		T = _pBone->m_pParent->m_translation;
		R = _pBone->m_pParent->m_rotation;
		S = _pBone->m_pParent->m_scaling;

		RVec3 = glm::degrees(glm::eulerAngles(R));

		SVec3 = _pBone->m_pParent->m_scaling;
		SVec3 = cvec3(1 / SVec3.x, 1 / SVec3.y, 1 / SVec3.z);

		R_SQ = glm::normalize(glm::quat_cast(M * glm::scale(cmat4(), SVec3)));
		R_SVec3 = glm::degrees(glm::eulerAngles(R_SQ));

	}
	*/
	// Translate

	// Rotation

	// Scaling
// 	auto TS = T * S;
// 	auto Rt = cvec3(R * cvec4(t, 1));
// 
// 
// 	auto TRt = T + Rt;
// 	auto Rr = glm::normalize(R * r);
// 	auto Ss = S * s;
// 	auto TtSs = TRt * Ss;

	//_pBone->m_translation = TRt;
	//_pBone->m_rotation = Rr;
	//_pBone->m_scaling = Ss;

	//*
	_pBone->m_transform =
	M *
	glm::translate(cmat4(), t) *
	glm::mat4_cast(r) *
	glm::scale(cmat4(), s);
	//*/

	for (int i = 0, e = _pBone->m_children.size(); i < e; ++i)
	{
		_UpdateBoneRecursive(_pBone->m_children[i], _pRenderNode, _pBone->m_transform);
	}

	// Local offset comes in after updating children
	// With shared portion of global transform
	_pBone->m_transform *= o;
	_pRenderNode->m_boneMats[_pBone->m_index] = _pBone->m_transform;
}

void CVS_Mesh::UpdateSkeleton(CVS_RenderNode* _pRenderNode)
{
	if (m_pSkeleton)
	{
		_UpdateBoneRecursive(m_pSkeleton->m_pRootBone, _pRenderNode);
	}
}

void CVS_Mesh::Draw(EDrawMode _drawMode)
{
	switch (_drawMode)
	{
	case CVS_Renderable::EDrawMode::Default:
	{
											   glBindVertexArray(m_VAO);
											   glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
											   glBindVertexArray(0);
	}
		break;
	case CVS_Renderable::EDrawMode::WireFrame:
	{

	}
		break;
	case CVS_Renderable::EDrawMode::Skeleton:
	{
												DrawSkeleton(m_pSkeleton);
	}
		break;
	default:
		break;
	}

}

void CVS_Mesh::DrawSkeleton(CVS_Skeleton* _pSkeleton)
{
	/*
	if (_pSkeleton)
	{
		if (_pSkeleton->m_pParent)
		{
			// 		glColor3f(1.0, 1.0, 1.0);
			// 		glLineWidth(2.0);
			// 
			// 		glLoadIdentity();//load identity matrix
			// 
			// 		glPointSize(10.0f);//set point size to 10 pixels
			// 
			// 
			// 		auto a = cvec3(0, 0, 0);
			// 		auto b = cvec3(50, 50, 50);
			// 
			// 		glBegin(GL_LINES);
			// 		glVertex3f(a.x, a.y, a.z);
			// 		glVertex3f(b.x, b.y, b.z);
			// 		glEnd();
		}

		for (auto i : _pSkeleton->m_children)
		{
			_DrawSkeletonRecursive(i);
		}
	}
	//*/
}