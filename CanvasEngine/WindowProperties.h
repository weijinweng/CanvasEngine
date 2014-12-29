#pragma once
#include "Display.h"

//Window Style Definition reserver 50000 - 50100

//Enable Drop Shadows
#define CVS_CS_SHADOW 50001
//Gets Own Display Context NOTE: Required for 3D renderers
#define CVS_CS_OWNDC 50002
//Redraws Window On Scale
#define CVS_CS_REDRAW_H 50003
#define CVS_CS_REDRAW_W 50004

namespace Canvas{

	struct WindowClass{
		std::string m_classname;
		uint32 m_class_styles;
#ifdef CVS_WIN32
		Icon icon;
		Cursor cursor;
		Brush brush;
		WindowClass(uint32 styles, Icon, Cursor, Brush, std::string);
#endif
		bool RegisterOS();
	};

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
		WindowProperties(int, int, int, int);
		DisplayContext getDisplayContext(bool on_paint = false);
		//Set size
		void setWindowSize(int x, int y, int w, int h);
		void setWindowSize(B_Rect);
		void setWindowSize(I_Rect);
		I_Rect getIRect();
		B_Rect getBRect();
		I_Rect getClientIRect();
	};
};