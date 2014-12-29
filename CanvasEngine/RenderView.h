#pragma once

#include "GUI.h"
#include "RenderSystem.h"

namespace Canvas
{
	class RenderView:public GUI_Entity{
		Camera cam;
	public:
		RenderView(int x,int y,int w,int h, Renderer*);
		void DrawFrame();
		int parseMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	};
};