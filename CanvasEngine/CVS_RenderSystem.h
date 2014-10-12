#ifndef CVS_RENDER
#define CVS_RENDER

#include "CVS_Precompiled.h"
#include "CVS_SubSystem.h"
#include "CVS_2DTools.h"

struct CVS_Renderer{
public:
	SDL_GLContext m_glContext;
	CVS_Window* window;
	CVS_2DTools* tools;
	CVS_Renderer(CVS_Window* window);
};

struct CVS_RenderSystem:public CVS_SubSystem{
public:
	SDL_GLContext m_glContext;
	bool Initialize();
	bool End();
	
};


#endif