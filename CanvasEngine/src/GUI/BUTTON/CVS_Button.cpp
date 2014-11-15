#include "CVS_GUI.h"
#include "Canvas.h"

CVS_Button::CVS_Button(std::string data, CVS_GUI_CONTAINER* parent, int x, int y, int w, int h) :text(data), hover(false), mousedown(false), textbool(true), iconbool(false)
{
	container = parent;
	hWnd = CreateWindow(WC_BUTTON, data.c_str(), WS_CHILD | WS_VISIBLE, x, y, w, h, parent->getHWND(), NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, CVS_Button::ButtonProc, CVS_BUTTONCTRL, (DWORD_PTR) this);

	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;


}

CVS_Button::CVS_Button(UINT bitmapID, CVS_GUI_CONTAINER* parent, int x, int y, int w, int h) :m_Bitmap(NULL), textbool(false), iconbool(true), hover(false), mousedown(false)
{
	m_Bitmap = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(bitmapID));

	container = parent;
	hWnd = CreateWindow(WC_BUTTON, "", WS_CHILD | WS_VISIBLE, x, y, w, h, parent->getHWND(), NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, CVS_Button::ButtonProc, CVS_BUTTONCTRL, (DWORD_PTR) this);

	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;

}

int CVS_Button::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_LBUTTONUP:
	{
						 mousedown = false;

						 RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
	}
		return 0;
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	{
						   mousedown = true;

						   RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
	}
		return 0;
	case WM_MOUSELEAVE:
	{
						  if (hover)
						  {

							  hover = false;
							  RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
						  }
	}
		return 0;
	case WM_MOUSEMOVE:
	{
						 if (!hover)
						 {
							 TRACKMOUSEEVENT lpEventTrack;

							 lpEventTrack.cbSize = sizeof(lpEventTrack);
							 lpEventTrack.dwFlags = TME_LEAVE;
							 lpEventTrack.hwndTrack = hWnd;

							 TrackMouseEvent(&lpEventTrack);
							 hover = true;

							 RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
						 }
						 return 0;
	}
		break;
	case WM_PAINT:
	{

					 if (mousedown)
					 {

						 MouseDownDraw();
					 }
					 else if (hover)
					 {
						 OnHoverDraw();
					 }
					 else {
						 OffHoverDraw();
					 }
	}
		return 0;

	}
	return -1;
}

void CVS_Button::OnHoverDraw()
{
	PAINTSTRUCT ps;
	RECT rect;
	rect.left = 0;
	rect.right = m_Rect.right - m_Rect.left;
	rect.top = 0;
	rect.bottom = m_Rect.bottom - m_Rect.top;

	HDC hdc = BeginPaint(hWnd, &ps);


	HBRUSH mainBrush = CreateSolidBrush(CVS_GREY);



	int oldDC = SaveDC(hdc);

	HGDIOBJ oldBrush = SelectObject(hdc, mainBrush);

	RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 3, 3);

	BeginPath(hdc);

	RoundRect(hdc, rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, 2, 2);

	EndPath(hdc);

	SelectClipPath(hdc, RGN_COPY);

	TRIVERTEX        vert[2];
	GRADIENT_RECT    gRect;
	vert[0].x = 0;
	vert[0].y = 0;
	vert[0].Red = 117 << 8;
	vert[0].Green = 117 << 8;
	vert[0].Blue = 117 << 8;
	vert[0].Alpha = 255;

	vert[1].x = rect.right;
	vert[1].y = rect.bottom;
	vert[1].Red = 98 << 8;
	vert[1].Green = 98 << 8;
	vert[1].Blue = 98 << 8;
	vert[1].Alpha = 255;

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;
	GradientFill(hdc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

	Gdiplus::Graphics graphics(hdc);

	Gdiplus::Pen pen(Gdiplus::Color(145, 145, 145));

	graphics.DrawLine(&pen, 0, rect.top + 1, rect.right, rect.top + 1);

	RestoreDC(hdc, oldDC);

	if (textbool)
	{
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, CVS_WHITE);

		DrawText(hdc, text.c_str(), text.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	else if (iconbool)
	{
		BITMAP bitmap;

		HDC memDC = CreateCompatibleDC(hdc);

		HGDIOBJ oldMap = SelectObject(memDC, m_Bitmap);

		GetObject(m_Bitmap, sizeof(bitmap), &bitmap);



		SetBkMode(hdc, TRANSPARENT);


		SetStretchBltMode(memDC, HALFTONE);

		TransparentBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 202, 203));


		SelectObject(memDC, oldMap);
		DeleteDC(memDC);


	}





	SelectObject(hdc, oldBrush);

	DeleteObject(mainBrush);



	EndPaint(hWnd, &ps);
}

void CVS_Button::OffHoverDraw()
{
	PAINTSTRUCT ps;

	RECT rect;
	rect.left = 0;
	rect.right = m_Rect.right - m_Rect.left;
	rect.top = 0;
	rect.bottom = m_Rect.bottom - m_Rect.top;

	HDC hdc = BeginPaint(hWnd, &ps);


	HBRUSH mainBrush = CreateSolidBrush(CVS_GREY);

	FillRect(hdc, &rect, mainBrush);

	if (textbool)
	{
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, CVS_WHITE);

		DrawText(hdc, text.c_str(), text.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	else if (iconbool)
	{
		BITMAP bitmap;

		HDC memDC = CreateCompatibleDC(hdc);

		HGDIOBJ oldMap = SelectObject(memDC, m_Bitmap);

		GetObject(m_Bitmap, sizeof(bitmap), &bitmap);



		SetBkMode(hdc, TRANSPARENT);


		SetStretchBltMode(memDC, HALFTONE);

		TransparentBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 202, 203));


		SelectObject(memDC, oldMap);
		DeleteDC(memDC);

	}

	DeleteObject(mainBrush);

	EndPaint(hWnd, &ps);

}

void CVS_Button::MouseDownDraw()
{
	PAINTSTRUCT ps;

	RECT rect;
	rect.left = 0;
	rect.right = m_Rect.right - m_Rect.left;
	rect.top = 0;
	rect.bottom = m_Rect.bottom - m_Rect.top;

	HDC hdc = BeginPaint(hWnd, &ps);


	HBRUSH mainBrush = CreateSolidBrush(CVS_GREY);



	int oldDC = SaveDC(hdc);

	HGDIOBJ oldBrush = SelectObject(hdc, mainBrush);

	RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 3, 3);

	BeginPath(hdc);

	RoundRect(hdc, rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, 2, 2);

	EndPath(hdc);

	SelectClipPath(hdc, RGN_COPY);

	TRIVERTEX        vert[2];
	GRADIENT_RECT    gRect;
	vert[0].x = 0;
	vert[0].y = 0;
	vert[0].Red = 63 << 8;
	vert[0].Green = 63 << 8;
	vert[0].Blue = 63 << 8;
	vert[0].Alpha = 255;

	vert[1].x = rect.right;
	vert[1].y = rect.bottom;
	vert[1].Red = 55 << 8;
	vert[1].Green = 55 << 8;
	vert[1].Blue = 55 << 8;
	vert[1].Alpha = 255;

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;
	GradientFill(hdc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

	Gdiplus::Graphics graphics(hdc);

	Gdiplus::Pen pen(Gdiplus::Color(51, 51, 51));

	graphics.DrawLine(&pen, 0, rect.top + 1, rect.right, rect.top + 1);


	if (textbool)
	{
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, CVS_WHITE);

		DrawText(hdc, text.c_str(), text.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	else if (iconbool)
	{
		BITMAP bitmap;

		HDC memDC = CreateCompatibleDC(hdc);

		HGDIOBJ oldMap = SelectObject(memDC, m_Bitmap);

		GetObject(m_Bitmap, sizeof(bitmap), &bitmap);



		SetBkMode(hdc, TRANSPARENT);


		SetStretchBltMode(memDC, HALFTONE);

		TransparentBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0, 202, 203));


		SelectObject(memDC, oldMap);
		DeleteDC(memDC);

	}

	RestoreDC(hdc, oldDC);


	SelectObject(hdc, oldBrush);

	DeleteObject(mainBrush);

	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK CVS_Button::ButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR dword)
{
	int result = ((CVS_Button*)dword)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
	{
		return result;
	}

	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void CVS_Button::UpdatePosition()
{
	if (!MoveWindow(hWnd, m_Rect.left, m_Rect.top, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, FALSE))
	{
		printf("Error\n");
	}
}