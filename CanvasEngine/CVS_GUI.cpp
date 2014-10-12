#include "CVS_GUI.h"
#include "CVS_2DTools.h"
#include "CVS_WindowSystem.h"

bool getMouseOver(CVS_IRECT rect, int x, int y)
{
	if(rect.x > x
		||rect.x + rect.w < x
		||rect.y > y
		||rect.y + rect.h < y)
		return false;
	return true;
}

CVS_Button::CVS_Button(int x, int y, int w, int h):callBack(NULL),bundle(NULL),mouseDown(false)
{
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
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
		if(mouseDown)
		{
			(*callBack)(bundle);
		}
	}
	mouseDown = false;
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
}

void CVS_Button::setOnClickFunction( void function(void* bundle), void* data)
{
	callBack = function;
	this->bundle = data;
}

void CVS_Button::Render(CVS_2DTools* tools)
{
	CVS_IRECT rect = {0,0,0,0};
	rect = tools->ConvertToScreenCoords(rect);
	tools->drawRect(color, rect.x, rect.y, rect.w, rect.h);
}

CVS_Frame::CVS_Frame(CVS_Window* window)
{
	area.w = window->width;
}