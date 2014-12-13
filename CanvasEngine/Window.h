#pragma once

#include "WindowProperties.h"

#define CVS_SIZE WM_USER + 1

//Reserve 100 to 300 for Window.

namespace Canvas{
	
	enum WindowStyles{
		CVS_WS_SCROLL_H = 1 << 0,
		CVS_WS_SCROLL_W = 1 << 2,
		CVS_WS_BORDER = 1 << 3,
		CVS_WS_CAPTION = 1 << 4,
		CVS_WS_CHILD = 1 << 5,
		CVS_WS_CLIP_CHILDREN = 1 << 6,
		CVS_WS_CLIP_SIBLINGS = 1 << 7,
		CVS_WS_OVERLAPPED = 1 << 8,
		CVS_WS_VISIBLE = 1 << 9
	};
	//Window Handler
	class Window:public Entity{
		WindowProperties m_properties;
		Window* m_parent;
		std::vector<Window*> m_children;
	public:
		static std::string defclass;
		bool active;
		//Default constructor creates no windows
		//Used for layout manager.
		Window();
		Window(std::string caption, int x, int y, int w, int h, uint32 flags, Window* parent);

		void setFocus();

		void setWindowSize(G_Input, G_Input, G_Input, G_Input);
		void setWindowSize(B_Rect);
		void setWindowSize(I_Rect);
		void setWindowSize(G_Rect);

		int getWidth();
		int getHeight();

		void redraw(int = 0, int = 0, int = 0, int = 0, int = 0);

		DisplayContext getDisplayContext();
		DisplayContext getPaintContext();
		//Hierarchy
		void setParent(Window*);
		void removeChild(Window*);
		void addChild(Window*);
		Response Message(unsigned int msg, void* pointer, long data);
		void onResize();
		//Get Client Rectangle
		I_Rect getIRectPx();
		B_Rect getBRectPx();
		G_Rect getGRect();
#ifdef CVS_WIN32
		virtual int parseMsg(UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK WNDCallBack(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK SubclassCallback(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
		HWND getHWND();
#endif
	};

	//Window system
	class WindowSystem:public SubSystem{
		std::queue<Window*> destroyed;
		std::list <Window*> m_windows;
	public:
		WindowSystem();
		bool init(uint32);
		void update();
		bool end();
		Response Message(unsigned int, void*, long);
		Window* CreateNewWindow(std::string caption, int x, int y, int w, int h, uint32 flags, Window* parent);
	};

	int CVS_WS_ToWinApi(int flag);
	Window* CreateNewWindow(std::string caption, int x, int y, int w, int h, uint32 flags, Window* parent);
};

extern Canvas::WindowSystem* current_windowsys;