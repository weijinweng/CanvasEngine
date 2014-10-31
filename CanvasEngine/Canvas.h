#ifndef CANVAS_H
#define CANVAS_H

#include "resource.h"
#include "CVS_Precompiled.h"
#include "CVS_WindowSystem.h"
#include "CVS_World.h"

//Forward declaration
struct CVS_App;
struct Editor;

/***********************************
*Canvas singleton state machine, used
*primarily for subsystem management 
*global resource requirements and in 
*the future, memory management and plugin
*management
************************************/
struct CVS_StateMachine{
public:
	CVS_App* m_App;
	CVS_RenderSystem m_RenderSub;
	CVS_WindowSystem m_WindowSub;
	//CVS_GameSystem m_GameSub;
	bool initialize();
	void update();
};

//Windows resources
#if defined (CVS_WIN32)
typedef char* FILEFILTER;
extern HINSTANCE CVS_AppInstance;
extern HINSTANCE CVS_AppPrevInstance;
extern LPSTR CVS_CmdLine;
extern int CVS_CmdShow;
extern FILEFILTER Filter;

#define CVS_MDI_INITIAL 50000
#define CVS_MAIN_MDI 101

#endif

class CVS_App{
public:
	virtual bool Initialize() = 0;
	virtual bool Run() = 0;
	virtual bool End() = 0;
	virtual int FileOpen(void* data) = 0;
};

class Editor:public CVS_App{
public:
	CVS_Window* m_MainWindow;
	bool Initialize();
	bool Run();
	bool End();
	int FileOpen(void* data);
};

bool CVS_Initialize();
#ifdef CVS_WIN32

bool CVS_Initialize(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR mCmdLine, int mCmdNum);
LRESULT CALLBACK CVS_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
extern CVS_StateMachine GLOBALSTATEMACHINE;

#endif