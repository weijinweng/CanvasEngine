#include "RenderSystem.h"
#include "RenderNode.h"
#include "CVS_Mesh.h"
#include "Scene.h"
#include "Camera.h"
#include "RenderProgram.h"


namespace Canvas {

std::vector<CanvasWindow*> CVS_MainWindow;

shaderTextureID::shaderTextureID(std::string name, RenderSys* parent):name(name),parent(parent)
{
	
}

void shaderTextureID::setTexture(std::string name)
{
	texture = parent->getTexture(name);
	if(texture == NULL)
	{
		printf("Error! Texture not found\n");
		texture = parent->getTexture("default");
	}
}

shaderTextureID::~shaderTextureID()
{
}

std::vector<CVS_Mesh*> RenderSys::initMeshesFromFbxNode(FbxNode* _pNode)
{
	std::vector<CVS_Mesh*> meshes;

	auto pMesh = CVS_Mesh::initFromFbxNode(_pNode);
	if (pMesh)
	{
		meshes.push_back(pMesh);
		m_meshes.push_back(*pMesh);
	}
	else
	{
		printf("Error: Failed to init mesh from fbx node: %s.\n", _pNode->GetName());
	}

	// Process children
	const int childCount = _pNode->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		std::vector<CVS_Mesh*> childMeshes = initMeshesFromFbxNode(_pNode->GetChild(i));
		meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
	}

	return meshes;
}

RenderSys::RenderSys()
{

}

bool RenderSys::initialize(SDL_Window* windowHandler)
{
	glContext = SDL_GL_CreateContext(windowHandler);

	if (glContext == NULL)
	{
		printf("OpenGl context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}else
	{
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		}
		if (SDL_GL_SetSwapInterval(0) < 0)
		{
			printf("Error setting swap interval %s\n", SDL_GetError());
		}
	}
	glClearColor( 0.0f, 0.0f, 0.5f, 1.0f);
	m_meshes.reserve(50);
	textures.reserve(50);
	programs.reserve(50);
	Texture* tex = this->createNewTexture("default");
	tex->loadFile("./textures/default.tga");

	this->createNewProgram("Standard", "./shaders/Toon.vert", "./shaders/Toon.frag");
	this->createNewProgram("Shadow", "./shaders/shadow.vert", "./shaders/shadow.frag");
	this->createNewProgram("2D", "./shaders/image.vert","./shaders/image.frag");
	init2D();
	fbxManager = FbxManager::Create();
	return true;
}

bool RenderSys::end()
{
	return true;
}

void RenderSys::draw()
{
}

void RenderSys::draw(int x, int y, int width, int height)
{
}

void RenderSys::loadMesh(char* path)
{
}

void RenderSys::loadScene(char* path)
{
}

void RenderSys::unloadAllMemory()
{
}

Scene* RenderSys::createNewScene()
{
	scenes.push_back(Scene(this));
	scenes.back().renderables.reserve(20);
	printf("creation capacity %d\n", scenes.back().renderables.capacity());
	return &scenes.back();
}

renderProgram* RenderSys::createNewProgram(std::string name, char* vertPath, char* fragPath)
{
	programs.push_back(renderProgram(name, vertPath, fragPath, this));
	return &programs.back();
}

renderProgram* RenderSys::getProgram(std::string name)
{
	for(int i = 0, e = programs.size(); i < e; ++i)
	{
		if(programs[i].name == name)
			return &programs[i];
	}
	printf("Error getting program %s\n", name.c_str());
	return NULL;
}

RenderSys::~RenderSys()
{
}

std::vector<CVS_Mesh*> RenderSys::initMeshesFromAiScene(aiMesh** mesh, int meshcount)
{
	std::vector<CVS_Mesh*> newmeshes;
	for(int i = 0, e = meshcount; i < e; ++i)
	{
		m_meshes.push_back(CVS_Mesh());
		CVS_Mesh* newMesh = &m_meshes.back();
		newMesh->initMeshFromAiMesh(mesh[i]);
		newmeshes.push_back(newMesh);
	}
	return newmeshes;
}


CVS_Mesh* RenderSys::createNewMesh(std::string mesh)
{
	m_meshes.push_back(CVS_Mesh(mesh));
	return &m_meshes.back();
}

void RenderSys::init2D()
{
	GLuint vertex2d, uv,indices;
	glGenVertexArrays(1, &VAO2d);
	glGenBuffers(1, &vertex2d);
	glGenBuffers(1, &uv);
	glGenBuffers(1, &indices);

	glBindVertexArray(VAO2d);

	float vertices[] = {
		-1.0, 1.0,
		-1.0,-1.0,
		1.0,-1.0,
		1.0,1.0
	};

	float uvs[] = {
		0.0,0.0,
		0.0,1.0,
		1.0,1.0,
		1.0,0.0
	};

	unsigned int indice[] = {
		0,1,2,0,2,3
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertex2d);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indice, GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	glDeleteBuffers(1, &vertex2d);
	glDeleteBuffers(1, &uv);
	glDeleteBuffers(1, &indices);
}

void RenderSys::DrawTextureRect(Texture* texture, float x, float y, float width, float height)
{
	renderProgram* prog2D = getProgram("2D");
	if(prog2D == NULL)
	{
		printf("lol\n");
	}
	glUseProgram(prog2D->programID);
	glBindVertexArray(VAO2d);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture->textureID);

	GLuint textureLoc = glGetUniformLocation(prog2D->programID, "myTexture");
	glUniform1i(textureLoc, 0);


	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Texture* RenderSys::createNewTexture(std::string name)
{
	textures.push_back(Texture(name));
	printf("Texture created\n");
	return &textures.back();
}
Texture* RenderSys::getTexture(std::string name)
{
	printf("textures size %d\n", textures.size());
	for (int i = 0, e = textures.size(); i < e; ++i)
	{
		printf("textures name %s\n", textures[i].name.c_str());
		if (textures[i].name == name)
			return &textures[i];
	}
	return NULL;
}

CanvasWindow::CanvasWindow(int x, int y, int width, int height, int flags):x(x), y(y), width(width), height(height)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL_Init error %s\n", SDL_GetError());
		return;
	}
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	int imgFlags = IMG_INIT_PNG;

	if( !(IMG_Init( imgFlags ) & imgFlags))
	{
		printf( "SDL_image could not be initialize! SDL_image Error: %s\n", IMG_GetError() );
		return;
	}

	Uint32 flag = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;
	
	if(flags & CVS_WDW_FULLSCREEN)
		flag = SDL_WINDOW_FULLSCREEN|SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;
	if(flags & CVS_WDW_WINDOW)
		flag = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;

	windowHandler = SDL_CreateWindow("Canvas Engine", x, y, width, height, flag);
	renderer.initialize(windowHandler);
}

void CanvasWindow::Minimize()
{

}

void CanvasWindow::Maximize()
{

}

void CanvasWindow::Move(int x,int y)
{

}

void CanvasWindow::Resize(int width, int height)
{

}

CanvasWindow* CanvasWindow::CVS_CreateWindow(int x, int y, int width, int height, int flags)
{
	CanvasWindow* window = new CanvasWindow(x, y, width, height, flags);
	CVS_MainWindow.push_back(window);
	return window;
}

}