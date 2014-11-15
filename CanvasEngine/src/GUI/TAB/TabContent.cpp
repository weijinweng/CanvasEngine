#include "Canvas.h"
#include "CVS_GUI.h"

CVS_TabContent::CVS_TabContent(CVS_Tab* parent) :parent(parent)
{
	hWnd = CreateWindow(WC_STATIC, "Content", WS_CHILD | WS_VISIBLE | WS_BORDER, 0, parent->m_TabMargins.bottom + 2, parent->m_Rect.right - parent->m_Rect.left, parent->m_Rect.bottom - parent->m_Rect.top - 30, parent->hWnd, NULL, CVS_AppInstance, NULL);

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

int CVS_TabContent::ParseMsg(HWND Hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
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

					 HDC hdc = BeginPaint(Hwnd, &ps);

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


					 EndPaint(Hwnd, &ps);
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
	int result = ((CVS_TabContent*)pointer)->ParseMsg(hWnd, msg, wParam, lParam);
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