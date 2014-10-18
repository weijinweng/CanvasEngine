#include "CVS_RenderComponents.h"
#include "Canvas.h"

struct CVS_Camera;

Vertex::Vertex(cvec3 position, cvec2 uv, cvec3 normal):position(position), uv(uv), normal(normal)
{
}

void CVS_Mesh::initialize()
{
	vertexArray = GLOBALSTATEMACHINE.m_RenderSub.createNewVertexObject();
	vertexArray->bindVAO();

	CVS_Buffer buffer(CVS_ARRAY_BUFFER);

	buffer.BufferData(vertices.size() * sizeof(CVS_VertexObject), &vertices[0]);
	
	vertexArray->bindArrayBuffer(&buffer, 0, 3, CVS_FLOAT, false, sizeof(Vertex), 0);
	vertexArray->bindArrayBuffer(&buffer, 1, 2, CVS_FLOAT, false, sizeof(Vertex), sizeof(cvec3));
	vertexArray->bindArrayBuffer(&buffer, 2, 3, CVS_FLOAT, false, sizeof(Vertex), sizeof(cvec3)+ sizeof(cvec2));

	vertexArray->bindElementBuffer(indices.size()*sizeof(unsigned int), &indices[0], CVS_STATIC_DRAW);

	vertexArray->unBind();
}

void CVS_Mesh::initializeFromAiMesh(const aiMesh* mesh)
{
	glm::vec2 nullUV(0,0);
	this->name = std::string(mesh->mName.data);
	printf("Init mesh %s\n", name.c_str());
	for (int i = 0, e = mesh->mNumVertices; i < e; i++)
	{
		glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec2 uv = glm::vec2(mesh->HasTextureCoords(0)? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y):nullUV);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		
		this->vertices.push_back(Vertex(pos,uv,normal));
	}

	for (int i = 0, e = mesh->mNumFaces; i < e; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			printf("Triangle is no longer a triangle!! it may have more than 3 vertices!");
			return;
		}
		this->indices.push_back(mesh->mFaces[i].mIndices[0]);
		this->indices.push_back(mesh->mFaces[i].mIndices[1]);
		this->indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	this->initialize();
	return;
}

void CVS_RenderProgramInstance::Render(CVS_Camera* cam)
{
}

void CVS_RenderProgramInstance::addChild(CVS_RenderNode* node)
{
	node->parent = this;
}

CVS_RenderNode::CVS_RenderNode(CVS_RenderProgramInstance* program)
{
	program->addChild(this);
}

void CVS_RenderNode::Render(CVS_Camera* cam)
{
}

CVS_RenderScene::CVS_RenderScene()
{
	GLOBALSTATEMACHINE.m_RenderSub.getRenderProgram("Default");
}

CVS_RenderNode* CVS_RenderScene::createNewNode()
{
	return new CVS_RenderNode(&programs[0]);
}