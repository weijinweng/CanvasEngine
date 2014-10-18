#include "Canvas.h"
#include "CVS_GUI.h"
#include "CVS_2DTools.h"
#include "CVS_WindowSystem.h"

extern CVS_StateMachine GLOBALSTATEMACHINE;

bool getMouseOver(CVS_IRECT rect, int x, int y)
{
	if(rect.x > x
		||rect.x + rect.w < x
		||rect.y > y
		||rect.y + rect.h < y)
		return false;
	return true;
}

CVS_TextNode::CVS_TextNode(CVSFontHandle handle):font(handle)
{
	size.x = 0;
	size.y = 0;
}

bool CVS_TextNode::addChar(char letter)
{
	if(letter != ' ')
	{
		text += letter;


		
		
	}
	return true;
}

CVS_Button::CVS_Button(int x, int y, int w, int h):callBack(NULL),bundle(NULL),mouseDown(false)
{
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	font = 0;
}

void CVS_Button::setText(std::string text)
{
	this->text.clear();
	for(const char* p = text.c_str(); *p;)
	{
		CVS_TextNode node(this->font);
		while(node.addChar(*p))
		{
			this->text.push_back(node);
			p++;
		}
	}
}

bool CVS_Button::getMouseDown(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		mouseDown = true;
		color.r = 1.0f;
		return true;
	}
	return false;
}

bool CVS_Button::getMouseUp(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		if(mouseDown && callBack != NULL)
		{
			(*callBack)(bundle);
		}

	}
	mouseDown = false;
	color.r = 0.5f;
	return mouseDown;
}

void CVS_Button::onHover(int x, int y)
{
	if(getMouseOver(rect, x, y))
	{
		color.r = 0.5f;
	} else offHover();
}

void CVS_Button::offHover()
{
	color.r = 0.0f;
	printf("offhover\n");
}

void CVS_Button::setOnClickFunction( void function(void* bundle), void* data)
{
	callBack = function;
	this->bundle = data;
}

void CVS_Button::Render(CVS_2DTools* tools)
{
	CVS_FRECT rect = {0,0,0,0};
	rect = tools->ConvertToScreenCoords(this->rect);
	tools->drawRect(color, rect.x, rect.y, rect.w, rect.h);
}

CVS_Gui::CVS_Gui(CVS_Window* window):window(window)
{
}

CVSButtonHandle CVS_Gui::addButton(int x, int y, int w, int h, void (function)(void* bundle), void* bundle)
{
	this->buttons.push_back(CVS_Button(x, y, w, h));
	buttons.back().setOnClickFunction(function, bundle);
	return buttons.size()-1;
}

void CVS_Gui::Update()
{
	for(int i = 0; i < buttons.size(); ++i)
	{
		buttons[i].Render(window->renderer->tools);
	}
}

void CVS_Gui::ParseInputs(SDL_Event e)
{
	int x, y;
	SDL_GetMouseState(&x, &y);

	if(e.type == SDL_MOUSEBUTTONDOWN)
	{
		for(int i = 0; i < buttons.size(); ++i)
		{
			buttons[i].getMouseDown(x,y);
		}
	}
	else if(e.type == SDL_MOUSEBUTTONUP)
	{
		for(int i = 0; i < buttons.size(); ++i)
		{
			buttons[i].getMouseUp(x,y);
		}
	}
	else if(e.type == SDL_MOUSEMOTION)
	{
		for(int i = 0; i < buttons.size(); ++i)
		{
			buttons[i].onHover(x,y);
		}
	}

}