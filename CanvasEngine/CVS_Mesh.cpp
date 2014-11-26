#include "Canvas.h"

Vertex::Vertex(cvec3 pos, cvec2 uv, cvec3 normal) :position(pos), uv(uv), normal(normal)
{

}

void CVS_Mesh::initialize()
{
	GLuint vertexBuffer, indiceBuffer;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

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

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec3)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);


	glBindVertexArray(0);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indiceBuffer);

}

void CVS_Mesh::initializeFromAiMesh(const aiMesh* mesh)
{
	glm::vec2 nullUV(0, 0);
	this->name = std::string(mesh->mName.data);

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
			return;
		}
		this->indices.push_back(mesh->mFaces[i].mIndices[0]);
		this->indices.push_back(mesh->mFaces[i].mIndices[1]);
		this->indices.push_back(mesh->mFaces[i].mIndices[2]);
	}


	if (mesh->HasTangentsAndBitangents())
	{

		for (int i = 0, e = mesh->mNumVertices; i < e; i++)
		{
			vertices[i].tangent.x = mesh->mTangents[i].x;
			vertices[i].tangent.y = mesh->mTangents[i].y;
			vertices[i].tangent.z = mesh->mTangents[i].z;
		}
	}
	else {
		calculateTangents();
	}

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i].tangent = glm::normalize(vertices[i].tangent);
	}

	this->initialize();



	return;
}

void CVS_Mesh::calculateTangents()
{
	for (unsigned int i = 0 ; i < indices.size() ; i += 3) {
		Vertex & v0 = vertices[indices[i]];
		Vertex & v1 = vertices[indices[i + 1]];
		Vertex & v2 = vertices[indices[i + 2]];

		glm::vec3 Edge1 = v1.position - v0.position;
		glm::vec3 Edge2 = v2.position - v0.position;

		float DeltaU1 = v1.uv.x - v0.uv.x;
		float DeltaV1 = v1.uv.y - v0.uv.y;
		float DeltaU2 = v2.uv.x - v0.uv.x;
		float DeltaV2 = v2.uv.y - v0.uv.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		vertices[indices[i]].tangent += Tangent;
		vertices[indices[i + 1]].tangent += Tangent;
		vertices[indices[i + 2]].tangent += Tangent;

	}

	for (unsigned int i = 0 ; i < vertices.size() ; i++) {
		vertices[i].tangent = glm::normalize(vertices[i].tangent);
	}
}



void CVS_Mesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool CVS_Mesh::LoadFromFile(char* filepath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (scene)
	{
		if (scene->mNumMeshes)
		{
			this->initializeFromAiMesh(scene->mMeshes[0]);
			return true;
		}
	}
	return false;
}