#pragma once
#include "Display.h"

namespace Canvas{
	//Window Properties
	struct WindowProperties{
		//Cross plateform properteis
#ifdef CVS_WIN32
		HWND m_handle;
#endif
#ifdef CVS_MAC
#endif
#ifdef CVS_LINUX
#endif
		ObjectRect m_rect;
		DisplayContext getDisplayContext();
		//Set size
		void setWindowSize(int x, int y, int w, int h);
	};
};