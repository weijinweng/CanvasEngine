#ifndef CANVAS_H
#define CANVAS_H

#include "CVS_Precompiled.h"
#include "CVS_WindowSystem.h"
#include "CVS_World.h"

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
	CVS_WorldSystem m_GameSub;
	//CVS_GameSystem m_GameSub;
	bool initialize();
	void update();
};



class CVS_App{
public:
	virtual bool Initialize() = 0;
	virtual bool Run() = 0;
	virtual bool End() = 0;
};

class Editor:public CVS_App{
public:
	CVS_Window* m_MainWindow;
	bool Initialize();
	bool Run();
	bool End();
};

bool CVS_Initialize();

extern CVS_StateMachine GLOBALSTATEMACHINE;

#endif