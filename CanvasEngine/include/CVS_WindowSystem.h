#ifndef CVS_WINDOWSYSTEM
#define CVS_WINDOWSYSTEM
#include "CVS_Precompiled.h"
#include "CVS_SubSystem.h"
#include "CVS_RenderSystem.h"
#include "CVS_GUI.h"

struct CVS_WindowSystem;

/*Generic CVS_Window*/
struct CVS_Window{
	SDL_Window* window;
	Uint32 windowID;
	int width, height, x, y;
	CVS_Renderer* renderer;
	CVS_Gui* gui;
	CVS_WindowSystem* system;
	CVS_Window(std::string header, int x, int y, int width, int height, CVS_WindowSystem* system);
	void getEvent(SDL_Event e);
	void UpdateScreen();
};

/*Window manager and creator*/
struct CVS_WindowSystem:public CVS_SubSystem{
public:
	std::vector<CVS_Window*> windows;
	//Creates a new window with the specified attributes
	CVS_Window* createNewWindow(std::string header, int x, int y, int width, int height);
	void deleteWindow(CVS_Window* window);
	bool Initialize();
	bool Update();
	bool End();
};

#endif