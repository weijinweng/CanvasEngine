#include "Canvas.h"

bool CVS_Initialized = false;
CVS_StateMachine GLOBALSTATEMACHINE;

GLenum convertToGLEnum(CVS_Enum enumerator){
	switch(enumerator)
	{
	case CVS_FLOAT:
		return GL_FLOAT;
	case CVS_UINT:
		return GL_UNSIGNED_INT;
	case CVS_INT:
		return GL_INT;
	case CVS_STATIC_DRAW:
		return GL_STATIC_DRAW;
	case CVS_ARRAY_BUFFER:
		return GL_ARRAY_BUFFER;
	default :
		return GL_FALSE;
	}
}

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