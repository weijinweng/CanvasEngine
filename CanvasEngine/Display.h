#pragma once
#include "CanvasCore.h"

namespace Canvas{
	//DisplayContext for drawing.
	struct DisplayContext{
	#ifdef CVS_WIN32 
		HWND hWnd;
		HDC hdc;
		DisplayContext(HWND);
		void SetHandle(HWND);
		void SetPaintHandle(HWND);
	#endif
	#ifdef CVS_MAC
	#endif
	#ifdef CVS_LINUX
	#endif
		bool Init3D();
		void Draw();
		void UpdateBuffer();
		~DisplayContext();
	};
};
