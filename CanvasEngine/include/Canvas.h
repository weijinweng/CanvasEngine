#ifndef CANVAS_H
#define CANVAS_H

#include "resource.h"
#include CVS_PRECOMPILED_H
#include "CVS_WindowSystem.h"
#include "CVS_World.h"

struct CVS_SceneView;

#define CVS_TABCTRL 200
#define CVS_TABCONT 203
#define CVS_TABSLOT 204
#define CVS_TOOLBAR 205
#define CVS_TABCONTCLASS "CVS_TABCONT"
#define CVS_BUTTONCTRL 201
#define CVS_EDITBOX 202


#define CVS_BLACK RGB(0,0,0)
#define CVS_WHITE RGB(255,255,255)
#define CVS_BLUE RGB(0,0,255)
#define CVS_RED RGB(255, 0,0)


#define CVS_LIGHT_GREY RGB(106,106,106)
#define CVS_GREY RGB(83, 83, 83)
#define CVS_LIGHT_DARK_GREY RGB(74,74,74)
#define CVS_DARK_GREY RGB(57,57,57)

#define CVS_LIGHT_BLACK RGB(55, 55, 55)
#define CVS_DARK_BLACK RGB(40, 40, 40)

class CVS_App;
class Editor;

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
	CVS_GuiManager m_GUISub;
	CVS_WorldSystem m_WorldSub;
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
	virtual void AddToUpdate(CVS_SceneView* obj) = 0;
};

class Editor:public CVS_App, public CVS_Messagable{
public:
	std::vector<CVS_SceneView*> updatables;
	CVS_Scene* mMainScene;
	CVS_GameObject* mSelected;
	CVS_Window* m_MainWindow;
	bool Initialize();
	bool Run();
	bool End();
	void AddToUpdate(CVS_SceneView* obj);
	int FileOpen(void* data);
	LONG_PTR Message(UINT, UINT, LONG_PTR);
};

#define PRECISION_MILLIS 0
#define PRECISION_MILLIC 1

bool CVS_Initialize();
#ifdef CVS_WIN32

bool CVS_Initialize(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR mCmdLine, int mCmdNum);
LRESULT CALLBACK CVS_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
extern CVS_StateMachine GLOBALSTATEMACHINE;

#endif