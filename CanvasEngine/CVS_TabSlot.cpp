#include "Canvas.h"


CVS_TabContent::CVS_TabContent(CVS_Tab* parent) :parent(parent)
{
	hWnd = CreateWindow(WC_STATIC, "Content", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPCHILDREN, 0, parent->m_TabMargins.bottom + 2, parent->m_Rect.right - parent->m_Rect.left, parent->m_Rect.bottom - parent->m_Rect.top - 30, parent->hWnd, NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, &CVS_TabContent::TabContProc, CVS_TABCONT, (DWORD_PTR)this);

	m_Rect.left = 0;
	m_Rect.right = parent->m_Rect.right - parent->m_Rect.left;
	m_Rect.top = 0;
	m_Rect.bottom = parent->m_Rect.bottom - parent->m_Rect.top - 30;


	if (hWnd == NULL)
	{
		printf("Error\n");
	}

}

int CVS_TabContent::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case SCENE_LOAD:
	{
					   for (int i = 0, e = this->buttons.size(); i < e; ++i)
					   {
						   this->buttons[i]->ParseMsg(msg, wParam, lParam);
					   }
	}
	case WM_SHOWWINDOW:
	{
	}
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
	{
					 PAINTSTRUCT ps;

					 RECT rect;
					 rect.left = m_Rect.left;
					 rect.right = m_Rect.right;
					 rect.top = m_Rect.top;
					 rect.bottom = m_Rect.bottom;

					 HDC hdc = BeginPaint(hWnd, &ps);

					 if (hdc == NULL)
					 {
						 printf("error\n");
						 return -1;
					 }

					 HBRUSH mainBrush = CreateSolidBrush(CVS_GREY);
					 HPEN mainPen = CreatePen(PS_SOLID, 1, CVS_GREY);

					 HGDIOBJ oldBrush = SelectObject(hdc, mainBrush);
					 HGDIOBJ oldPen = SelectObject(hdc, mainPen);


					 FillRect(hdc, &rect, mainBrush);

					 SelectObject(hdc, oldBrush);
					 SelectObject(hdc, oldPen);

					 DeleteObject(mainBrush);
					 DeleteObject(mainPen);


					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}
	return -1;
}

HWND CVS_TabContent::getHWND()
{
	return hWnd;
}

void CVS_TabContent::OnResize()
{
	SetWindowPos(hWnd, HWND_TOP, 0, parent->m_TabMargins.bottom + 2, parent->m_Rect.right - parent->m_Rect.left, parent->m_Rect.bottom - parent->m_Rect.top - 30, SWP_SHOWWINDOW);
	m_Rect.left = 0;
	m_Rect.right = parent->m_Rect.right - parent->m_Rect.left;
	m_Rect.top = 0;
	m_Rect.bottom = parent->m_Rect.bottom - parent->m_Rect.top - 30;
}

CVS_Window* CVS_TabContent::getWindow()
{
	return parent->parent;
}

CVS_Button* CVS_TabContent::AddButton(std::string data, int x, int y, int w, int h)
{
	CVS_Button* newButton;
	newButton = new CVS_Button(data, this, x, y, w, h);
	this->buttons.push_back(newButton);
	return newButton;
}

LRESULT CALLBACK CVS_TabContent::TabContProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	int result = ((CVS_TabContent*)pointer)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

CVS_Button* CVS_TabContent::AddButton(UINT bitmapId, int x, int y, int w, int h)
{
	return NULL;
}

CVS_EditBox* CVS_TabContent::AddEditBox(UINT type, void* dataPtr, int x, int y, int w, int h)
{
	CVS_EditBox* newEditBox = new CVS_EditBox(this, type, dataPtr, x, y, w, h);
	this->buttons.push_back(newEditBox);
	return newEditBox;
}

CVS_TabSlot::CVS_TabSlot(std::string name, CVS_Tab* tabparent) :data(name), parent(tabparent), m_Selected(false)
{
	hWnd = CreateWindow(WC_BUTTON, name.c_str(), WS_CHILD | WS_VISIBLE, 0, 0, 60, 25, tabparent->hWnd, NULL, CVS_AppInstance, NULL);
	content = new CVS_TabContent(tabparent);

	SetWindowSubclass(hWnd, CVS_TabSlot::TabSlotProc, CVS_TABSLOT, (DWORD_PTR)this);
}

int CVS_TabSlot::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if (!m_Selected)
		{
			printf("Clicked\n");
			parent->HideAllContent(this);
			m_Selected = true;
			InvalidateRect(parent->hWnd, NULL, TRUE);
		}
		return 0;
	case WM_PAINT:
	{
					 PAINTSTRUCT ps;

					 RECT rect;
					 rect.left = 0;
					 rect.right = m_Rect.right - m_Rect.left;
					 rect.top = 0;
					 rect.bottom = m_Rect.bottom - m_Rect.top;


					 HDC hdc = BeginPaint(hWnd, &ps);
					 if (hdc == NULL)
					 {
						 MessageBox(NULL, "Error painting", "Error", MB_OK);
					 }



					 HBRUSH mainBrush;
					 HPEN borderPen = CreatePen(PS_SOLID, 1, CVS_DARK_BLACK);


					 if (m_Selected)
					 {
						 mainBrush = CreateSolidBrush(CVS_GREY);
					 }
					 else {
						 mainBrush = CreateSolidBrush(CVS_DARK_GREY);
					 }

					 FillRect(hdc, &rect, mainBrush);

					 Gdiplus::Pen pen(Gdiplus::Color(40, 40, 40));

					 Gdiplus::Graphics graphics(hdc);

					 graphics.DrawLine(&pen, rect.right - 1, rect.top, rect.right - 1, rect.bottom);

					 if (m_Selected)
					 {
						 SetTextColor(hdc, CVS_WHITE);
						 pen.SetColor(Gdiplus::Color(106, 106, 106));
						 graphics.DrawLine(&pen, rect.left, rect.top, rect.right - 2, rect.top);
					 }
					 else {
						 SetTextColor(hdc, CVS_LIGHT_GREY);
						 pen.SetColor(Gdiplus::Color(76, 76, 76));
						 graphics.DrawLine(&pen, rect.left, rect.top, rect.right - 2, rect.top);
					 }

					 SetBkMode(hdc, TRANSPARENT);

					 HGDIOBJ oldfont = SelectObject(hdc, CVS_Tab::font);



					 DrawText(hdc, data.c_str(), data.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

					 SelectObject(hdc, oldfont);

					 DeleteObject(mainBrush);
					 DeleteObject(borderPen);

					 EndPaint(hWnd, &ps);
	}
		return 0;
	}
	return -1;
}

void CVS_TabSlot::ShowContent()
{
	ShowWindow(content->hWnd, SW_SHOWNORMAL);
}

LRESULT CALLBACK CVS_TabSlot::TabSlotProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	int result = ((CVS_TabSlot*)pointer)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void CVS_TabSlot::UpdatePos()
{

	if (!MoveWindow(hWnd, m_Rect.left, m_Rect.top, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, TRUE))
	{
		printf("Error Updating pos\n");
	}
}

void CVS_TabSlot::Repaint()
{
	InvalidateRect(hWnd, NULL, TRUE);
}

void CVS_TabSlot::HideContent()
{
	ShowWindow(content->hWnd, SW_HIDE);
}
