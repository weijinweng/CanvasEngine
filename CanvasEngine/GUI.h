#pragma once

#include "Window.h"

namespace Canvas{

	struct GUI_Entity:public Window{
		GUI_Entity(int x, int y, int w, int h, Window* Parent);
		virtual void setRight(uint32, uint32 = 504);
		virtual void setLeft(uint32, uint32 = 504);
		virtual void setTop(uint32, uint32 = 504);
		virtual void SetBot(uint32, uint32 = 504);

	};

	struct GUIManager{
		std::vector<GUI_Entity*> guiObj;
	};
};