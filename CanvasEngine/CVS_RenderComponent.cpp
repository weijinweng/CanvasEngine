#include "CVS_RenderComponents.h"

Vertex::Vertex(cvec3 pos, cvec2 uv, cvec3 normal):position(pos), uv(uv),normal(normal)
{

}

void CVS_Mesh::initialize()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint vertexBuffer, indiceBuffer;

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indiceBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);

	glBindVertexArray(0);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indiceBuffer);
}

void CVS_Mesh::initializeFromAiMesh(const aiMesh* mesh)
{
	glm::vec2 nullUV(0,0);
	this->name = std::string(mesh->mName.data);

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

	printf("Init mesh %s\n", mesh->mName.data);

	return;
}

void CVS_Mesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::quat CreateQuatFromTwoVectors(cvec3 U, cvec3 V)
{
	cvec3 w = glm::cross(U,V);
	glm::quat q = glm::quat(glm::dot(U,V), w.x, w.y, w.z);
	q.w += sqrt(q.x*q.x + q.w*q.w + q.y*q.y + q.z*q.z);
	return glm::normalize(q);
}

CVS_Camera::CVS_Camera(cvec3 pos,cvec3 target, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	this->transform.translation = pos;
	this->transform.orientation = CreateQuatFromTwoVectors(cvec3(0,0,1), target - pos);
	this->transform.scale = glm::vec3(1.0f);

	this->FOV = FOV;
	this->aspectRatio = AspectRatio;
	this->NearZ = NearZ;
	this->FarZ = FarZ;

	UpdateView();
}



void CVS_Camera::UpdateView()
{
	cvec4 direction = cvec4(0,0,1.0f,1.0f);

	cvec4 targetDir =  glm::mat4_cast(transform.orientation) *  direction;

	cvec3 target = glm::normalize(transform.translation) + cvec3(targetDir.x, targetDir.y, targetDir.z);

	printf(" %f %f %f\n", target.x, target.y, target.z);


	View  = glm::lookAt(transform.translation, target, glm::vec3(0, 1, 0));

	Persp = glm::perspective(FOV, aspectRatio, NearZ, FarZ);
}

CVS_View CVS_Camera::getView()
{
	CVS_View view;
	view.Pers = Persp;
	view.View = View;
	return view;
}

CVS_RenderProgramInstance::CVS_RenderProgramInstance(CVS_RenderProgram* program)
{
	this->program = program;
}

void CVS_RenderProgramInstance::Render(CVS_View* view)
{
	program->setAsCurrentProgram();

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view->Pers));

	for(int i = 0, e = this->children.size(); i < e; ++i)
	{
		cmat4 Model = children[i]->modelMatrix;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model));
		cmat4 MVP = view->Pers * view->View * Model;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		children[i]->mesh->Draw();
	}
}

CVS_RenderScene::CVS_RenderScene()
{

}

void CVS_RenderScene::Draw(CVS_View* view)
{
	for(int i = 0, e = programs.size(); i < e; ++i)
	{
		programs[i].Render(view);
	}
}

