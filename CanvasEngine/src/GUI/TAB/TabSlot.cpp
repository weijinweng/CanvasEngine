#include "Canvas.h"
#include "CVS_GUI.h"

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
