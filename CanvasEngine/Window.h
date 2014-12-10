#pragma once

#include "WindowProperties.h"


//Reserve 100 to 300 for Window.
#define CVS_WS_BORDERLESS 100
#define CVS_WS_CHILD 101

namespace Canvas{
	
	
	//Window Handler
	class Window:public Entity{
		WindowProperties m_properties;
		Window* m_parent;
		std::vector<Window*> m_children;
	public:
		Window();
		Window(int x, int y, int w, int h, uint32 flags, Window* parent);
		void setWindowSize(int x, int y, int w, int h);
		void redraw(int = 0, int = 0, int = 0, int = 0, int = 0);
		DisplayContext getDisplayContext();
		void addChild(Window*);
		Response Message(unsigned int msg, void* pointer, long data);
#ifdef CVS_WIN32
		int parseMsg(UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK WNDCallBack(HWND, UINT, WPARAM, LPARAM);
		HWND getHWND();
#endif
	};

	class WindowSystem:public SubSystem{
		std::vector<Window*> m_windows;
		Window* creatNewWindow(int, int, int, int, uint32, Window*);
		bool init(uint32);
		void update();
		bool end();
		Response Message(unsigned int, void*, long);
	};
};

extern Canvas::WindowSystem* m_worldsys;