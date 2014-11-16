#include "Canvas.h"

int CVS_GUI_OBJ::getWidth()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return rect.right - rect.left;
}

int CVS_GUI_OBJ::getHeight()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return rect.bottom - rect.top;
}

void CVS_GUI_OBJ::UpdatePosition()
{

}

CVS_GUI_OBJ::~CVS_GUI_OBJ()
{
	DestroyWindow(hWnd);
}

CVS_GUI_CELL::CVS_GUI_CELL(CVS_Layout* parent):m_Weight(1.0f)
{
	
}

void CVS_GUI_CELL::SetSize(int x, int y, int w, int h)
{
	obj->SetSize(x,y,w,h);
	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
}

void CVS_GUI_CELL::SetPosition(UINT type, CVS_GUI_CELL* pos , int value)
{

}


void CVS_GUI_CELL::SetObj(CVS_LAYOUT_OBJ* obj)
{
	this->obj = obj;
	obj->SetSize(m_Rect.left, m_Rect.top, m_Rect.right- m_Rect.left, m_Rect.bottom - m_Rect.top);
	
}



/*Main gui controller*/
CVS_Gui::CVS_Gui(CVS_Window* parent):window(parent),Layout(NULL)
{


}



bool CVS_GuiManager::Initialize()
{
	return true;
}

LRESULT CALLBACK CVS_GuiManager::GuiCallback(HWND hWnd,UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR swRefData )
{
	int response;
	response = ((CVS_GUI_OBJ*)swRefData)->ParseMsg(msg, wParam, lParam) ;
	if( response >= 0)
	{
		return response;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

bool CVS_GuiManager::Update()
{
	return true;
}

bool CVS_GuiManager::End()
{
	return true;
}