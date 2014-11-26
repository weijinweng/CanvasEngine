#ifndef CVS_WINDOWSYSTEM
#define CVS_WINDOWSYSTEM

#include "CVS_SubSystem.h"
#include "CVS_RenderSystem.h"
#include "CVS_GUI.h"

struct CVS_WindowSystem;

/*Generic CVS_Window*/
struct CVS_Window{
#ifdef CVS_WIN32
	HWND g_MDIClient;
	HWND hWnd;
	bool ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	bool ParseMsgMDI(UINT msg, WPARAM wParam, LPARAM lParam);
	std::vector<UINT> BarID;
#endif
	CVS_Window* parent;
	std::vector<CVS_Window*> children;
	int width, height, x, y;
	bool m_Active;
	CVS_Renderer* renderer;
	CVS_Gui* gui;
	CVS_WindowSystem* system;
	CVS_Window(std::string header, int x, int y, int width, int height, int Flags, CVS_Window* parent);
	CVS_Window(std::string header, int x, int y, int width, int height, int FLAGS);
	//TODO:~ Allow parsing string to create menu. 
	//TODO:~ Allow dynamic event processing
	void CreateMenuMain();
	bool DrawMenu(WPARAM, LPARAM);
	bool MeasureMenu(WPARAM, LPARAM);
	void UpdateScreen();
	int getWidth();
	int getHeight();
	int getClientWidth();
	int getClientHeight();
	bool Close();
	bool OpenFile();
	CVS_Window* CreateSubWindow(std::string header, int x, int y, int width, int height, int FLAGS);
	CVS_Tab* CreateNewTab(std::string header, int x, int y, int width, int height);
};

/*Window manager and creator*/
struct CVS_WindowSystem:public CVS_SubSystem{
public:
	std::vector<CVS_Window*> windows;
	//Creates a new window with the specified attributes
	CVS_Window* createNewWindow(std::string header, int x, int y, int width, int height, int FLAGS);
	void deleteWindow(CVS_Window* window);
#ifdef CVS_WIN32
	HWND temp;
	const char* className;
	const char* className_MDI;
	const char* className_MDIChild;
	//callback manager.
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc_MDI(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK WndProc_MDIChild(HWND, UINT, WPARAM, LPARAM);
#endif
	CVS_WindowSystem();
	bool Initialize();
	bool Update();
	bool End();
};

#endif