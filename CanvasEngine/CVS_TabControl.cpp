#include "Canvas.h"

HFONT CVS_Tab::font = NULL;

CVS_Tab::CVS_Tab(CVS_Window* parent, int x, int y, int w, int h)
{
	if (font == NULL)
	{
		font = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "Arial");
	}

	this->parent = parent;
	RECT rcClient;

	INITCOMMONCONTROLSEX icex;

	TCITEM tie;


	TCHAR achTemp[256];

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);


	hWnd = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, x, y, w, h, parent->hWnd, NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, &CVS_GuiManager::GuiCallback, CVS_TABCTRL, (DWORD_PTR)this);

	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;

	m_TabMargins.left = 0;
	m_TabMargins.right = w;
	m_TabMargins.top = 0;
	m_TabMargins.bottom = 25;



	if (hWnd == NULL)
	{
		MessageBox(parent->hWnd, "Error making tab!", "Error", MB_OK);
	}

}

int CVS_Tab::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	{
						   HDC hdc = GetDC(hWnd);

	}
		break;
	case WM_MOUSEMOVE:
	{

	}
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
	{
					 PAINTSTRUCT ps;

					 HDC hdc = BeginPaint(hWnd, &ps);

					 Gdiplus::Graphics graphics(hdc);

					 RECT rect;
					 rect.left = 0;
					 rect.right = m_Rect.right - m_Rect.left;
					 rect.top = 0;
					 rect.bottom = m_Rect.bottom - m_Rect.top;

					 HBRUSH mainBrush = CreateSolidBrush(CVS_GREY);
					 HPEN mainPen = CreatePen(PS_SOLID, 1, CVS_DARK_BLACK);
					 HBRUSH secondBrush = CreateSolidBrush(CVS_DARK_GREY);

					 HGDIOBJ oldBrush = SelectObject(hdc, secondBrush);
					 HGDIOBJ oldPen = SelectObject(hdc, mainPen);

					 Rectangle(hdc, 0, 0, rect.right, rect.bottom);

					 Gdiplus::Pen pen(Gdiplus::Color(40, 40, 40));

					 for (int i = 0, e = this->m_Slots.size(); i < e; ++i)
					 {
						 rect.left = m_Slots[i]->m_Rect.left;
						 rect.right = m_Slots[i]->m_Rect.right;
						 rect.top = m_Slots[i]->m_Rect.top;
						 rect.bottom = m_Slots[i]->m_Rect.bottom;
						 if (m_Slots[i]->m_Selected)
						 {

							 pen.SetColor(Gdiplus::Color(106, 106, 106));
							 graphics.DrawLine(&pen, rect.left, rect.bottom + 1, 0, rect.bottom + 1);
							 graphics.DrawLine(&pen, rect.right - 1, rect.bottom + 1, m_Rect.right - rect.left, rect.bottom + 1);

							 pen.SetColor(Gdiplus::Color(40, 40, 40));
							 graphics.DrawLine(&pen, rect.left, rect.bottom, 0, rect.bottom);
							 graphics.DrawLine(&pen, rect.right, rect.bottom, m_Rect.right - m_Rect.left, rect.bottom);
							 pen.SetColor(Gdiplus::Color(83, 83, 83));
							 graphics.DrawLine(&pen, rect.left, rect.bottom + 1, rect.right - 2, rect.bottom + 1);
							 graphics.DrawLine(&pen, rect.left, rect.bottom, rect.right - 2, rect.bottom);
							 break;
						 }
					 }
					 SelectObject(hdc, oldBrush);
					 SelectObject(hdc, oldPen);

					 DeleteObject(mainBrush);
					 DeleteObject(mainPen);
					 DeleteObject(secondBrush);

					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}

	return -1;
}

void CVS_Tab::HideAllContent(CVS_TabSlot* slot)
{
	slot->ShowContent();
	for (int i = 0, e = m_Slots.size(); i < e; ++i)
	{
		if (m_Slots[i] != slot)
		{
			m_Slots[i]->HideContent();
			m_Slots[i]->m_Selected = false;
			m_Slots[i]->Repaint();
		}
	}
}

void CVS_Tab::GetMsg(UINT msg, UINT_PTR sParam, LONG_PTR lParam)
{
	switch (msg)
	{
	case SCENE_LOAD:
	{
					   for (int i = 0, e = m_Slots.size(); i < e; ++i)
					   {
						   m_Slots[i]->content->ParseMsg(msg, sParam, lParam);
					   }
	}
	}
}

bool CVS_Tab::CalibrateLocation()
{
	int offX = 0;
	int y = 0;
	for (int i = 0; i < m_Slots.size(); ++i)
	{
		m_Slots[i]->m_Rect.left = offX;
		m_Slots[i]->m_Rect.right = offX + 60;
		m_Slots[i]->m_Rect.top = y;
		m_Slots[i]->m_Rect.bottom = y + 25;
		offX += 60;
		m_Slots[i]->UpdatePos();
	}
	return true;
}

void CVS_Tab::addTab(std::string name, int pos)
{
	CVS_TabSlot* slot = new CVS_TabSlot(name, this);
	if (m_Slots.size() == 0)
		slot->m_Selected = true;
	else
	{
		slot->HideContent();
	}

	m_Slots.push_back(slot);
	CalibrateLocation();
}

void CVS_Tab::SetSize(int x, int y, int w, int h)
{
	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
	MoveWindow(hWnd, x, y, w, h, true);
	for (int i = 0, e = this->m_Slots.size(); i < e; ++i)
	{
		m_Slots[i]->content->OnResize();
	}
	for (int i = 0, e = m_Slots.size(); i < e; ++i)
	{
		if (m_Slots[i]->m_Selected)
		{
			this->HideAllContent(m_Slots[i]);
			break;
		}
	}
}
