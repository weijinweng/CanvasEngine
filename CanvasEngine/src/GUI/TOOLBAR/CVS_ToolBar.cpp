#include "Canvas.h"
#include "CVS_GUI.h"

CVS_ToolBar::CVS_ToolBar(CVS_Gui* gui, int x, int y, int w, int h)
{
	this->parent = gui->window;
	hWnd = CreateWindow(GLOBALSTATEMACHINE.m_WindowSub.className, "LOL", WS_CHILD | WS_VISIBLE, 0, y, gui->window->width, h, gui->window->hWnd, NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, CVS_ToolBar::ToolBarCallBack, CVS_TOOLBAR, (DWORD_PTR)this);

	m_Rect.left = 0;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
	m_Rect.right = gui->window->width;


	if (hWnd == NULL)
	{
		printf("Toolbar error\n");
	}
}

HWND CVS_ToolBar::getHWND()
{
	return hWnd;
}

CVS_Window* CVS_ToolBar::getWindow()
{
	return parent;
}

int CVS_ToolBar::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		return 0;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
	{

					 PAINTSTRUCT ps;

					 RECT rectIv;

					 GetUpdateRect(hWnd, &rectIv, FALSE);

					 ps.rcPaint = rectIv;

					 HDC hdc = BeginPaint(hWnd, &ps);



					 HBRUSH mainBrush = CreateSolidBrush(CVS_GREY);
					 HPEN mainPen = CreatePen(PS_SOLID, 1, CVS_DARK_BLACK);



					 HGDIOBJ oldBrush = SelectObject(hdc, mainBrush);
					 HGDIOBJ oldPen = SelectObject(hdc, mainPen);

					 Gdiplus::Pen pen(Gdiplus::Color(106, 106, 106));

					 Gdiplus::Graphics graphics(hdc);

					 RECT rect;

					 rect.top = 0;
					 rect.left = 0;
					 rect.right = m_Rect.right - m_Rect.left;
					 rect.bottom = m_Rect.bottom - m_Rect.top;

					 Rectangle(hdc, 0, 0, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top);

					 graphics.DrawLine(&pen, 1, 1, m_Rect.right - m_Rect.left - 1, 1);

					 SelectObject(hdc, oldBrush);
					 SelectObject(hdc, oldPen);

					 DeleteObject(mainBrush);
					 DeleteObject(mainPen);

					 EndPaint(hWnd, &ps);
	}
		return 0;
	}
	return -1;
}

CVS_Button* CVS_ToolBar::AddButton(std::string name, int x, int y, int w, int h)
{
	CVS_Button* newButton = new CVS_Button(name, this, x, 0, w, h);
	buttons.push_back(newButton);
	CalibrateLocation();
	return newButton;
}

CVS_Button* CVS_ToolBar::AddButton(UINT bitmapID, int x, int y, int w, int h)
{
	int height = m_Rect.bottom - m_Rect.top;
	CVS_Button* newButton = new CVS_Button(bitmapID, this, x, 0, w, h);
	buttons.push_back(newButton);
	CalibrateLocation();
	return newButton;
}

bool CVS_ToolBar::CalibrateLocation()
{
	int offX = 0;
	int border = 5;
	int offY = 0;
	for (int i = 0, e = buttons.size(); i < e; ++i)
	{
		int left = buttons[i]->m_Rect.left;
		int top = buttons[i]->m_Rect.top;
		buttons[i]->m_Rect.left = offX + border;

		offY = (m_Rect.bottom - m_Rect.top - buttons[i]->m_Rect.bottom + buttons[i]->m_Rect.top) / 2;

		buttons[i]->m_Rect.top = offY;
		buttons[i]->m_Rect.right = buttons[i]->m_Rect.left - left + buttons[i]->m_Rect.right;
		buttons[i]->m_Rect.bottom = buttons[i]->m_Rect.top + buttons[i]->m_Rect.bottom - top;
		buttons[i]->UpdatePosition();
		offX += buttons[i]->m_Rect.right - buttons[i]->m_Rect.left + border;
	}
	return true;
}

void CVS_ToolBar::SetSize(int x, int y, int w, int h)
{
	SetWindowPos(hWnd, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
	m_Rect.left = x;
	m_Rect.right = w + x;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
	CalibrateLocation();
	RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
}

LRESULT CALLBACK CVS_ToolBar::ToolBarCallBack(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{

	int result = ((CVS_ToolBar*)pointer)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}
