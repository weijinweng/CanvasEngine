#include "CVS_RenderSystem.h"
#include "CVS_WindowSystem.h"

//For checking for current vao to reduce calls to VAO bindings
GLuint currentVAO = 0;

CVS_Buffer::CVS_Buffer(CVS_Enum target)
{
	this->target = convertToGLEnum(target);
	glGenBuffers(1, &buffer);
	glBindBuffer(this->target, buffer);
}

void CVS_Buffer::bindBuffer()
{
	glBindBuffer(this->target, buffer);
}

void CVS_Buffer::BufferData(unsigned int size, void* data)
{
	bindBuffer();
	glBufferData(target, size, data, GL_STATIC_DRAW);
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
	buffer->bindBuffer();

	glEnableVertexAttribArray(location);
	glVertexAttribPointer(0,size, convertToGLEnum(type), normalize ? GL_TRUE:GL_FALSE, stride, (void*)offset);
	
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
	}
	CVS_Renderer* newRenderer =  new CVS_Renderer(window);
	newRenderer->m_glContext = m_glContext;
	renderers.push_back(newRenderer);
	return newRenderer;
}