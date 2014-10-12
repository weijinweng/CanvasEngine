#include "Canvas.h"

bool CVS_Initialized = false;
CVS_StateMachine GLOBALSTATEMACHINE;

bool CVS_StateMachine::initialize()
{
	if(!m_RenderSub.Initialize())
	{
		return false;
	}
	return m_WindowSub.Initialize();
}

bool CVS_Initialize()
{
	printf("Initialize CVS\n");
	if(CVS_Initialized)
		return true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return false;
	}
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);	

	int imgFlags = IMG_INIT_PNG;

	if( !(IMG_Init( imgFlags ) & imgFlags))
	{
		return false;
	}
	if(!GLOBALSTATEMACHINE.initialize())
	{
		return false;
	}

	CVS_Initialized = true;
	printf("Initialized CVS\n");
	return true;
}