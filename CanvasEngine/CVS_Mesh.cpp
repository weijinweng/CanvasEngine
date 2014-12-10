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

	cvec3 max = cvec3(0, 0, 0);

	cvec3 caller_p, cen;

	double dx, dy, dz;
	double rad_sq, xspan, yspan, zspan, maxspan, rad;
	double old_to_p, old_to_p_sq, old_to_new;
	cvec3 xmin, xmax, ymin, ymax, zmin, zmax, dia1, dia2;

	xmin.x = ymin.y = zmin.z = FLT_MAX;
	xmax.x = ymax.y = zmax.z = -FLT_MAX;

	for (int i = 0, e = mesh->mNumVertices; i < e; i++)
	{
		glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec2 uv = glm::vec2(mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : nullUV);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		this->vertices.push_back(Vertex(pos, uv, normal));

		caller_p = pos;

		if (caller_p.x<xmin.x)
			xmin = caller_p; 
		if (caller_p.x>xmax.x)
			xmax = caller_p;
		if (caller_p.y<ymin.y)
			ymin = caller_p;
		if (caller_p.y>ymax.y)
			ymax = caller_p;
		if (caller_p.z<zmin.z)
			zmin = caller_p;
		if (caller_p.z>zmax.z)
			zmax = caller_p;
	}


	dx = xmax.x - xmin.x;
	dy = xmax.y - xmin.y;
	dz = xmax.z - xmin.z;
	xspan = dx * dx + dy * dy + dz * dz;

	dx = ymax.x - ymin.x;
	dy = ymax.y - ymin.y;
	dz = ymax.z - ymin.z;
	yspan = dx * dx + dy * dy + dz * dz;

	dx = zmax.x - zmin.x;
	dy = zmax.y - zmin.y;
	dz = zmax.z - zmin.z;
	zspan = dx * dx + dy * dy + dz * dz;

	dia1 = xmin; dia2 = xmax; 
	maxspan = xspan;

	if (yspan > maxspan)
	{
		maxspan = yspan;
		dia1 = ymin; dia2 = ymax;
	}
	if (zspan > maxspan)
	{
		dia1 = zmin; dia2 = zmax;
	}

	cen.x = (dia1.x + dia2.x) / 2.0;
	cen.y = (dia1.y + dia2.y) / 2.0;
	cen.z = (dia1.z + dia2.z) / 2.0;

	dx = dia2.x - cen.x;
	dy = dia2.y - cen.y;
	dz = dia2.z - cen.z;

	rad_sq = dx * dx + dy * dy + dz * dz;
	rad = sqrt(rad_sq);

	for (int i = 0, e = vertices.size(); i < e; ++i)
	{
		caller_p = vertices[i].position;

		dx = caller_p.x - cen.x;
		dy = caller_p.y - cen.y;
		dz = caller_p.z - cen.z;
		old_to_p_sq = dx * dx + dy * dy + dz * dz;
		if (old_to_p_sq > rad_sq)
		{
			old_to_p = sqrt(old_to_p_sq);

			rad = (rad + old_to_p) / 2.0;
			rad_sq = rad*rad;
			old_to_new = old_to_p - rad;

			cen.x = (rad*cen.x + old_to_new*caller_p.x) / old_to_p;
			cen.y = (rad*cen.y + old_to_new*caller_p.y) / old_to_p;
			cen.z = (rad*cen.z + old_to_new*caller_p.z) / old_to_p;


		}
	}
	//printf("\n New sphere: cen,rad = %f %f %f   %f",
	//	cen.x, cen.y, cen.z, rad);

	bSphere.radius = rad;
	bSphere.center = cen;



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
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_FlipUVs);
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