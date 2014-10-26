#include "CVS_RenderSystem.h"
#include "CVS_WindowSystem.h"
#include "Canvas.h"

//For checking for current vao to reduce calls to VAO bindings
GLuint currentVAO = 0;
GLuint currentBuffer = 0;

void CVS_RenderPackage::setUp()
{
	GLOBALSTATEMACHINE.m_RenderSub.createNewShader("Default", "./Shaders/3DForward/First.vert", "./Shaders/3DForward/First.frag");
}

void CVS_RenderPackage::Render(CVS_Camera* cam, CVS_RenderScene* scene)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClearColor(0.0,0.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(int i = 0; i < scene->programs.size();++i)
	{
		scene->programs[i].Render(cam);
	}
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
}

CVS_DeferredRenderPackage::CVS_DeferredRenderPackage(CVS_RenderSystem* renderSystem):system(renderSystem)
{

}

void CVS_DeferredRenderPackage::setUp()
{
	printf("Setting up deferred shading\n");
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &colorText);
	glGenTextures(1, &normalText);
	glGenTextures(1, &uvText);
	glGenTextures(1, &posText);

	glBindTexture(GL_TEXTURE_2D, colorText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1600, 900, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorText, 0);

	glBindTexture(GL_TEXTURE_2D, colorText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1600, 900, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uvText, 0);

	glBindTexture(GL_TEXTURE_2D, colorText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1600, 900, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalText, 0);

	glBindTexture(GL_TEXTURE_2D, colorText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1600, 900, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posText, 0);

	glBindTexture(GL_TEXTURE_2D, depthText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1600, 900, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthText, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, DrawBuffers);
	
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(Status != GL_FRAMEBUFFER_COMPLETE){
		printf("Framebuffer error! Deferred shader unable to initialize\n");
		return;
	}

	//Unbind framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	system->createNewShader("Default", "./Shaders/3DDeferred/Deferred.vert", "./Shaders/3DDeferred/Deferred.frag");
	CVS_Texture2D* texture = system->generateNewTexture();
	texture->loadFile("./Textures/Default.png");
}

void CVS_DeferredRenderPackage::Render(CVS_Camera* cam, CVS_RenderScene* scene)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);



	//Bind framebuffer for drawing.
	glBindFramebuffer(GL_DRAW_BUFFER, frameBuffer);
	glClearColor(0.0,0.0,1.0,1.0);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	GeometryPass(cam, scene);

	glClearColor(1.0,1.0,1.0,1.0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_READ_BUFFER, frameBuffer);

	LightingPass(cam, scene);

}

void CVS_DeferredRenderPackage::GeometryPass(CVS_Camera* cam, CVS_RenderScene* scene)
{
	for(int i = 0, e = scene->programs.size(); i < e; ++i)
	{
		scene->programs[i].Render(cam);
	}
}

//CurrentTestMode
void CVS_DeferredRenderPackage::LightingPass(CVS_Camera* cam, CVS_RenderScene* scene)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + 0);
	glBlitFramebuffer(0,0,1600, 900, 0, 0, 800, 450, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + 1);
	glBlitFramebuffer(0,0,1600, 900, 0, 450, 800, 450, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + 2);
	glBlitFramebuffer(0,0,1600, 900, 800, 450, 800, 450, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + 3);
	glBlitFramebuffer(0,0,1600, 900, 800, 0, 800, 450, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

CVS_Buffer::CVS_Buffer(CVS_Enum target)
{
	this->target = convertToGLEnum(target);
	glGenBuffers(1, &buffer);
	glBindBuffer(this->target, buffer);
}

void CVS_Buffer::bindBuffer()
{
	if(currentBuffer == buffer)
		return;
	glBindBuffer(this->target, buffer);
	currentBuffer == buffer;
}

void CVS_Buffer::BufferData(unsigned int size, void* data)
{
	bindBuffer();
	glBufferData(target, size, data, GL_STATIC_DRAW);
}

CVS_Buffer::~CVS_Buffer()
{
	glDeleteBuffers(1, &buffer);
}

CVS_VertexObject::CVS_VertexObject():VAO(0)
{
	glGenVertexArrays(1, &VAO);
	if(VAO == 0)
	{
		printf("VAO initialization error, invalid VAO generated\n");
	}
}

void CVS_VertexObject::bindVAO()
{
	if(currentVAO != VAO)
		glBindVertexArray(VAO);
	currentVAO = VAO;
}

void CVS_VertexObject::unBind()
{
	glBindVertexArray(0);
	currentVAO = 0;
}

void CVS_VertexObject::bindArrayBuffer( CVS_Buffer* buffer, unsigned int location, int size, CVS_Enum type, bool normalize, unsigned int stride, unsigned int offset) 
{
	if(currentVAO != VAO)
	{
		printf("VAO not bound!\n");
		return;
	}
	if(buffer->target != convertToGLEnum(CVS_ARRAY_BUFFER))
	{
		printf("Invalid Buffer used!\n");
		return ;
	}

	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location,size, convertToGLEnum(type), normalize ? GL_TRUE:GL_FALSE, stride, (void*)offset);
	
}

void CVS_VertexObject::bindElementBuffer(unsigned int size, void* data, CVS_Enum usage)
{
	bindVAO();
	CVS_Buffer buffer(CVS_ELEMENT_BUFFER);
	buffer.BufferData(size, data);
	buffer.bindBuffer();
	unBind();
}

void CVS_VertexObject::drawElements(CVS_Enum type, unsigned int vertCount, unsigned int offset)
{
	bindVAO();
	glDrawElements(convertToGLEnum(type), vertCount, GL_UNSIGNED_INT, (void*) offset);
	unBind();
}

CVS_Renderer::CVS_Renderer(CVS_Window* window):window(window)
{
	Clear();
	SwapFrameBuffer();
	this->tools = new CVS_2DTools(window);
}

CVS_RenderSystem::CVS_RenderSystem():m_glContext(NULL)
{
}

bool CVS_RenderSystem::Initialize()
{

	return true;
}

bool CVS_RenderSystem::End()
{
	return true;
}

void CVS_Renderer::Clear()
{
	//Default clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CVS_Renderer::SwapFrameBuffer()
{
	SDL_GL_SwapWindow(window->window);
}

CVS_RenderProgram* CVS_RenderSystem::createNewShader(std::string name, char* vertpath, char* fragpath)
{
	//if no program with the name exists
	if(programs.find(name) == programs.end())
	{
		CVS_RenderProgram* newProgram = new CVS_RenderProgram();
		programs[name] = newProgram;
		newProgram->loadFile(vertpath, fragpath);
		return newProgram;
	} else {
		//Return existing program
		printf("This program already exists! Returning existing program\n");
		return programs[name];
	}
}

CVS_VertexObject* CVS_RenderSystem::createNewVertexObject()
{
	CVS_VertexObject* newObject = new CVS_VertexObject();
	vertexArrays.push_back(newObject);
	return newObject;
}

CVS_Renderer* CVS_RenderSystem::createNewRenderer(CVS_Window* window)
{
	if(m_glContext == NULL)
	{
		m_glContext = SDL_GL_CreateContext(window->window);

		if (m_glContext == NULL)
		{
			printf("OpenGl context could not be created! SDL Error: %s\n", SDL_GetError());
			return NULL;
		}else
		{
			glewExperimental = GL_TRUE;
			GLenum glewError = glewInit();
			if (glewError != GLEW_OK)
			{
				printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
				return NULL;
			}
			if (SDL_GL_SetSwapInterval(0) < 0)
			{
				printf("Error setting swap interval %s\n", SDL_GetError());
				return NULL;
			}
		}
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f);
		renderMode = new CVS_RenderPackage();
		renderMode->setUp();
	}
	CVS_Renderer* newRenderer =  new CVS_Renderer(window);
	newRenderer->m_glContext = m_glContext;
	renderers.push_back(newRenderer);
	CVS_Texture2D* texture = generateNewTexture();

	texture->loadFile("./Textures/Default.png");

	return newRenderer;
}

bool CVS_RenderSystem::loadFont(std::string name, char* fontPath)
{
	TTF_Font* face = TTF_OpenFont(fontPath, 72);

	if(face == NULL)
	{
		printf("Error could not open font\n");
		return false;
	}
	
	CVS_Font font = {face, name};
	fonts.push_back(font);
	return true;
}

std::vector<CVS_Mesh*> CVS_RenderSystem::addMeshesFromaiScene(const aiScene* scene)
{
	std::vector<CVS_Mesh*> meshlist;
	for(int i = 0, e = scene->mNumMeshes; i<e; ++i)
	{
		CVS_Mesh* newMesh = new CVS_Mesh();
		newMesh->initializeFromAiMesh(scene->mMeshes[i]);
		meshlist.push_back(newMesh);
	}
	return meshlist;
}

CVS_RenderProgram* CVS_RenderSystem::getRenderProgram(std::string name)
{
	if(programs.find(name) == programs.end())
	{
		return NULL;
	} else{
		printf("Got one?");
		return programs[name];
	}
}

CVS_Texture2D* CVS_RenderSystem::generateNewTexture()
{
	textures.push_back(new CVS_Texture2D());
	return textures.back();
}

CVS_RenderScene* CVS_RenderSystem::createNewScene()
{
	scenes.push_back(new CVS_RenderScene());
	return scenes.back();
}