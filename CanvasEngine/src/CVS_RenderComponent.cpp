#include "CVS_RenderComponents.h"
#include "Canvas.h"

struct CVS_Camera;

Vertex::Vertex(cvec3 position, cvec2 uv, cvec3 normal):position(position), uv(uv), normal(normal)
{
}

void CVS_Mesh::initialize()
{
	GLuint vertexBuffer, indiceBuffer;
	glGenVertexArrays(1, &TestVAO);
	glBindVertexArray(TestVAO);
	printf("mesh initialized!\n");

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

void CVS_Mesh::draw()
{
	glBindVertexArray(TestVAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

CVS_Camera::CVS_Camera(float FOV, float aspectRatio, float nearZ, float farZ):FOV(FOV),aspectRatio(aspectRatio),NearZ(nearZ), FarZ(farZ)
{
	transform.translation = cvec3(3,4,3);
}

CAM_CTRL::CAM_CTRL(){
	Up = false;
	Down = false;
	Left = false;
	Right  = false;
	accelerate = false;
	ShiftUp = false;
	ShiftDown = false;

	horAngle = 0;
	vertAngle = 3.14;
}

void CVS_Camera::parseInputs(SDL_Event e)
{
	if(e.type == SDL_KEYDOWN)
	{
		printf("Got a keyDown\n");
		switch(e.key.keysym.sym)
		{
			case SDLK_w:
				Up = true;
				break;
			case SDLK_s:
				Down = true;
				break;
			case SDLK_a:
				Left = true;
				break;
			case SDLK_d:
				Right = true;
				break;
			case SDLK_x:
				ShiftUp = true;
				break;
			case SDLK_z:
				ShiftDown = true;
				break;
			case SDLK_LSHIFT:
				accelerate = true;
				break;
		}
	}
	if(e.type == SDL_KEYUP)
	{
		switch(e.key.keysym.sym)
		{
		case SDLK_w:
			Up = false;
			break;
		case SDLK_s:
			Down = false;
			break;
		case SDLK_a:
			Left = false;
			break;
		case SDLK_d:
			Right = false;
			break;
		case SDLK_x:
			ShiftUp = false;
			break;
		case SDLK_z:
			ShiftDown = false;
			break;
		case SDLK_LSHIFT:
			accelerate = false;
		}
	}
}

void CVS_Camera::Update()
{
	cmat4 View = cmat4(1.0f);
	static int lastTime = 0;
	int deltaTime = SDL_GetTicks() - lastTime;
	lastTime = SDL_GetTicks();
	float multiplier = 1.0f;

	int x,y;
	SDL_GetMouseState(&x,&y);
	float deltaVert = (y - 900/2)*(0.01);
	float deltaHor = (x - 1600/2) * (0.01);
	vertAngle += deltaVert;
	horAngle -= deltaHor;
	
	printf("Horangle %f vertangle %f\n", horAngle, vertAngle);

	vertAngle = fClamp(vertAngle, -3.14/2 + 3.14, 3.14/2 + 3.14);

	glm::vec3 direction(std::cos(vertAngle)*std::sin(horAngle),
						std::sin(vertAngle),
						std::cos(vertAngle)*std::cos(horAngle));

	glm::vec3 directionRight(std::sin(horAngle-3.14f/2.0f),
							 0,
							 std::cos(horAngle-3.14f/2.0f));
	float timeSeconds = (float)deltaTime/1000.0f;
	if(accelerate)
		multiplier = 3.0f;
	if(Up)
	{
		position += direction*timeSeconds*multiplier;
	}
	if(Down)
	{
		position -= direction*timeSeconds*multiplier;
	}
	if (Left)
	{
		position += directionRight*timeSeconds*multiplier;
	}
	if (Right)
	{
		position -= directionRight*timeSeconds*multiplier;
	}
	if (ShiftUp)
	{
		position.y += 2*timeSeconds*multiplier;
	}
	if (ShiftDown)
	{
		position.y -= 2*timeSeconds*multiplier;
	}
}

cmat4 CVS_Camera::getPerspective()
{
	return glm::perspective(FOV, aspectRatio, NearZ, FarZ);
}

cmat4 CVS_Camera::getView()
{
	glm::vec3 direction(std::cos(vertAngle)*std::sin(horAngle),
						std::sin(vertAngle),
						std::cos(vertAngle)*std::cos(horAngle));

	glm::vec3 directionRight(std::sin(horAngle-3.14f/2.0f),
							 0,
							 std::cos(horAngle-3.14f/2.0f));

	glm::vec3 up = glm::cross(direction, directionRight);

	glm::mat4 View = glm::lookAt(position,direction+position, up);

	return View;
}

CVS_RenderProgramInstance::CVS_RenderProgramInstance(CVS_RenderProgram* program)
{
	this->program = program;
	
	ViewLoc = program->getUniformHash("V");
	ModelLoc = program->getUniformHash("M");
	MVPLoc = program->getUniformHash("MVP");
}

void CVS_RenderProgramInstance::remove(CVS_RenderNode* node)
{
	for(auto it = children.begin(), et = children.end(); it != et; it++)
	{
		if((*it) == node)
		{
			children.erase(it);
		}
	}
}

void CVS_RenderProgramInstance::Render(CVS_Camera* cam)
{
	program->setAsCurrentProgram();

	cmat4 P = cam->getPerspective();
	cmat4 V = cam->getView();
	program->bindMat4v(ViewLoc, glm::value_ptr(V));

	for(int i = 0, e = children.size(); i < e; ++i)
	{
		
		cmat4 M = children[i]->modelMatrix;
		cmat4 MVP = P * V * M;

		program->bindMat4v(ModelLoc, glm::value_ptr(M));
		
		program->bindMat4v(MVPLoc, glm::value_ptr(MVP));
		
		children[i]->bindTextures();

		children[i]->mesh->draw();
		
	}
}

void CVS_RenderProgramInstance::addChild(CVS_RenderNode* node)
{
	if(node->parent)
	{
		node->parent->remove(node);
	}
	node->parent = this;
	node->getTextures(program);
	children.push_back(node);
}

void CVS_TextureReference::bind()
{
	texture->bindToLocation(uniformLoc);
}

CVS_RenderNode::CVS_RenderNode(CVS_RenderProgramInstance* program):parent(NULL)
{
	program->addChild(this);
}

void CVS_RenderNode::getTextures(CVS_RenderProgram* program)
{
	for(int i = 0, e = program->uniforms.size(); i < e; ++i)
	{
		if(program->uniforms[i].type == GL_SAMPLER_2D)
		{
			printf("Added texture %s\n", program->uniforms[i].name.c_str());
			CVS_TextureReference reference = {GLOBALSTATEMACHINE.m_RenderSub.textures[0], program->uniforms[i].name, program->uniforms[i].location};
			textures.push_back(reference);
		}
	}
}

void CVS_RenderNode::setMesh(CVS_Mesh* mesh)
{
	this->mesh = mesh;
	printf("Added mesh %s\n", mesh->name.c_str());
}

void CVS_RenderNode::bindTextures()
{
	for(int i = 0, e = textures.size(); i < e; ++i)
	{
		textures[i].texture->setActive(i);
		textures[i].bind();
	}
}

void CVS_RenderNode::Render(CVS_Camera* cam)
{
}

CVS_RenderScene::CVS_RenderScene()
{
	loadRenderProgram("Default");
	cameras.push_back(CVS_Camera());
}

CVS_RenderNode* CVS_RenderScene::createNewNode()
{
	printf("Creating new CVS_RENDERNODE\n");
	nodes.push_back(new CVS_RenderNode(&programs[0]));
	return nodes.back();
}

void CVS_RenderScene::loadRenderProgram(std::string programName)
{
	CVS_RenderProgram* program = GLOBALSTATEMACHINE.m_RenderSub.getRenderProgram(programName);

	if(program)
	{
		printf("PROGRAM ADDED\n");
		this->programs.push_back(CVS_RenderProgramInstance(program));
	} else {
		printf("PROGRAM ERROR\n");
	}
}

void CVS_RenderScene::Render()
{
	GLOBALSTATEMACHINE.m_RenderSub.renderMode->Render(&cameras[0], this);
}

void CVS_RenderScene::Render(int index)
{
	GLOBALSTATEMACHINE.m_RenderSub.renderMode->Render(&cameras[1],this);
}

