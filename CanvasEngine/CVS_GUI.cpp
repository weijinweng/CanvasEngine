#include "Canvas.h"

void KeyParse::AddKey(char key)
{
	keys[key] = false;
}

int KeyParse::ParseKeyDown(char key)
{
	if (keys.find(key) != keys.end())
	{
		keys[key] = true; 
		return 0;
	}
	return - 1;
}

int KeyParse::ParseKeyUp(char key)
{
	if (keys.find(key) != keys.end())
	{
		keys[key] = false;
		return 0;
	}
	return -1;
}

bool KeyParse::GetKey(char key)
{
	if (keys.find(key) != keys.end())
	{
		return keys[key];
	}
	return false;
}

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

CVS_Button* CVS_GUI_CONTAINER::AddButton(std::string data, int x, int y, int w, int h)
{
	return NULL;
}

CVS_Button* CVS_GUI_CONTAINER::AddButton(UINT type, int x, int y, int w, int h)
{
	return NULL;
}

CVS_EditBox* CVS_GUI_CONTAINER::AddEditBox(UINT type, void* data, int x, int y, int w, int h)
{
	return NULL;
}

CVS_TreeView* CVS_GUI_CONTAINER::AddTreeView(int x, int y, int w, int h, UINT type)
{
	//FOR the environment
	CVS_TreeView* newtree = new CVS_TreeView(this, x, y, w, h);
	this->buttons.push_back(newtree);
	return newtree;
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

int CVS_GUI_CELL::ParseMsg(UINT msg, UINT_PTR sParam, LONG_PTR lParam)
{
	switch (msg)
	{
		case SCENE_LOAD:
			{
					   this->obj->GetMsg(msg, sParam, lParam);
			}
	}
	return -1;
}


/*Main gui controller*/
CVS_Gui::CVS_Gui(CVS_Window* parent):window(parent),Layout(NULL)
{


}

LONG_PTR CVS_Gui::Message(UINT msg, UINT_PTR sParam, LONG_PTR lParam)
{
	Layout->ParseMsg(msg, sParam, lParam);
	return 0;
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