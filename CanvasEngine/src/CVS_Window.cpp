#include "CVS_WindowSystem.h"
#include "Canvas.h"

extern CVS_StateMachine GLOBALSTATEMACHINE;

CVS_Window::CVS_Window(std::string header, int x, int y, int w, int h, CVS_WindowSystem* system):gui(NULL)
{
	window = SDL_CreateWindow(header.c_str(), x, y, w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_BORDERLESS );
	this->system = system;
	width = w;
	height = h;
	this->x = x;
	this-> y = y;
	gui = new CVS_Gui(this);
	windowID = SDL_GetWindowID(window);
}

void CVS_Window::getEvent(SDL_Event e)
{

	gui->ParseInputs(e);
}

void CVS_Window::UpdateScreen()
{
	renderer->Clear();
	if(gui!=NULL)
		gui->Update();
	renderer->SwapFrameBuffer();
}

bool CVS_WindowSystem::Initialize()
{
	return true;
}

CVS_Window* CVS_WindowSystem::createNewWindow(std::string name, int x, int y, int w, int h)
{
	CVS_Window* newWindow = new CVS_Window(name, x, y, w, h, this);
	windows.push_back(newWindow);
	newWindow->renderer = GLOBALSTATEMACHINE.m_RenderSub.createNewRenderer(newWindow);
	return newWindow;
}

bool CVS_WindowSystem::Update()
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{

		for(int i = 0; i < windows.size(); ++i)
		{
			windows[i]->getEvent(e);
		}
	}

	for(int i = 0; i < windows.size(); ++i)
	{
		windows[i]->UpdateScreen();
	}

	return true;
}

bool CVS_WindowSystem::End()
{
	for(int i = 0 ; i < windows.size(); ++i)
	{
		delete windows[i];
	}
	return true;
}