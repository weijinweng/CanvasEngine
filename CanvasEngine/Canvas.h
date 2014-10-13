#ifndef CANVAS_H
#define CANVAS_H

#include "CVS_Precompiled.h"
#include "CVS_WindowSystem.h"

/***********************************
*Canvas singleton state machine, used
*primarily for 
*
*
************************************/
struct CVS_StateMachine{
public:
	CVS_RenderSystem m_RenderSub;
	CVS_WindowSystem m_WindowSub;
	//CVS_GameSystem m_GameSub;
	bool initialize();
	void update();
};



class CVS_App{
public:
	CVS_App();
	bool Initialize();
	bool Run();
	bool End();
};

class Editor:public CVS_App{
public:
	bool Initialize();
	bool Run();
	bool End();
};

bool CVS_Initialize();

#endif