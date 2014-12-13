#pragma once

#include "Window.h"

namespace Canvas{
	
	extern Color_RGB GUI_Primary;
	extern Color_RGB GUI_Secondary;
	extern Color_RGB GUI_Tertiary;
	extern Font_OS GUI_DefaultFont;
	
	struct GUI_Properties{
		enum GUI_Enum{
			LEFT_ORIENT = 1 << 0,
			RIGHT_ORIENT = 1 << 1,
			TOP_ORIENT = 1 << 2,
			BOT_ORIENT = 1 << 3,
		};
		GUI_Properties();
		int m_style;
	};

	class GUI_Entity:public Window{
		GUI_Properties m_guiproperties;
	public:
		GUI_Entity();
		GUI_Entity(int x, int y, int w, int h, uint32 style, Window* Parent);
		virtual int parseMsg(UINT, WPARAM, LPARAM) = 0;
		virtual void setRight(G_Input);
		virtual void setLeft(G_Input);
		virtual void setTop(G_Input);
		virtual void setBot(G_Input);
		virtual void setSize(G_Input, G_Input, G_Input, G_Input);
	};

};