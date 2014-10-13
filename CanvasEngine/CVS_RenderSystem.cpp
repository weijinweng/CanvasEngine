#include "CVS_RenderSystem.h"
#include "CVS_WindowSystem.h"

//For checking for current vao to reduce calls to VAO bindings
GLuint currentVAO = 0;

CVS_Renderer::CVS_Renderer(CVS_Window* window):window(window)
{
	
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CVS_Renderer::SwapFrameBuffer()
{
	SDL_GL_SwapWindow(window->window);
}

CVS_RenderProgram* CVS_RenderSystem::createNewShader(std::string name, char* vertpath, char* fragpath)
{
	if(programs.find(name) == programs.end())
	{
		CVS_RenderProgram* newProgram = new CVS_RenderProgram();
		programs[name] = newProgram;
		newProgram->loadFile(vertpath, fragpath);
		return newProgram;
	} else {
		printf("This program already exists! Returning existing program\n");
		return programs[name];
	}
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
		glClearColor( 0.0f, 0.0f, 0.5f, 1.0f);
	}
	CVS_Renderer* newRenderer =  new CVS_Renderer(window);
	newRenderer->m_glContext = m_glContext;
	renderers.push_back(newRenderer);
	return newRenderer;
}