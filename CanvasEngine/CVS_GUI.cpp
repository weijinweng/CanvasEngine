#include "Canvas.h"

CVS_Button::CVS_Button(std::string data, CVS_GUI_CONTAINER* parent, int x, int y, int w, int h):text(data),hover(false),mousedown(false),textbool(true),iconbool(false)
{
	container = parent;
	hWnd = CreateWindow(WC_BUTTON, data.c_str(), WS_CHILD | WS_VISIBLE , x, y, w, h, parent->getHWND(), NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, CVS_Button::ButtonProc, CVS_BUTTONCTRL, (DWORD_PTR) this);

	m_Rect.left = x;
	m_Rect.right = x+w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;


}

CVS_Button::CVS_Button(UINT bitmapID, CVS_GUI_CONTAINER* parent, int x, int y, int w, int h):m_Bitmap(NULL),textbool(false),iconbool(true), hover(false), mousedown(false)
{
	m_Bitmap = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(bitmapID));

	container = parent;
	hWnd = CreateWindow(WC_BUTTON, "", WS_CHILD | WS_VISIBLE , x, y, w, h, parent->getHWND(), NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, CVS_Button::ButtonProc, CVS_BUTTONCTRL, (DWORD_PTR) this);

	m_Rect.left = x;
	m_Rect.right = x+w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;

}

int CVS_Button::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{

	case WM_LBUTTONUP:
		{
			mousedown = false;
			
			RedrawWindow(hWnd, NULL, NULL,  RDW_INVALIDATE);
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
			if(hover)
			{
				
				hover = false;
				RedrawWindow(hWnd, NULL, NULL,  RDW_INVALIDATE);
			}
		}
		return 0;
	case WM_MOUSEMOVE:
		{
			if(!hover)
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

				if(mousedown)
				{

					MouseDownDraw();
				} else if(hover)
				{
					OnHoverDraw();
				} else {
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

	RoundRect(hdc, rect.left,rect.top,rect.right,rect.bottom,3,3);

	BeginPath(hdc);

	RoundRect(hdc, rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1,2,2);

	EndPath(hdc);

	SelectClipPath(hdc, RGN_COPY);

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = 0;
	vert [0] .y      = 0;
	vert [0] .Red    = 117 << 8;
	vert [0] .Green  = 117 << 8;
	vert [0] .Blue   = 117 << 8;
	vert [0] .Alpha  = 255;

	vert [1] .x      = rect.right;
	vert [1] .y      = rect.bottom; 
	vert [1] .Red    = 98 << 8;
	vert [1] .Green  = 98 << 8;
	vert [1] .Blue   = 98 << 8;
	vert [1] .Alpha  = 255;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

	Gdiplus::Graphics graphics(hdc);

	Gdiplus::Pen pen(Gdiplus::Color(145,145,145));

	graphics.DrawLine(&pen, 0, rect.top + 1, rect.right, rect.top+1);

	RestoreDC(hdc, oldDC);

	if(textbool)
	{
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, CVS_WHITE);

		DrawText(hdc, text.c_str(), text.length(), &rect,  DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	} else if(iconbool)
	{
		BITMAP bitmap;

		HDC memDC = CreateCompatibleDC(hdc);

		HGDIOBJ oldMap = SelectObject(memDC, m_Bitmap);

		GetObject(m_Bitmap, sizeof(bitmap), &bitmap);



		SetBkMode(hdc, TRANSPARENT);


		SetStretchBltMode(memDC, HALFTONE);

		TransparentBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0,202,203));


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

	if(textbool)
	{
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, CVS_WHITE);

		DrawText(hdc, text.c_str(), text.length(), &rect,  DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	} else if(iconbool)
	{
		BITMAP bitmap;

		HDC memDC = CreateCompatibleDC(hdc);

		HGDIOBJ oldMap = SelectObject(memDC, m_Bitmap);

		GetObject(m_Bitmap, sizeof(bitmap), &bitmap);



		SetBkMode(hdc, TRANSPARENT);


		SetStretchBltMode(memDC, HALFTONE);

		TransparentBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0,202,203));


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

	RoundRect(hdc, rect.left,rect.top,rect.right,rect.bottom,3,3);

	BeginPath(hdc);

	RoundRect(hdc, rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1,2,2);

	EndPath(hdc);

	SelectClipPath(hdc, RGN_COPY);

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = 0;
	vert [0] .y      = 0;
	vert [0] .Red    = 63 << 8;
	vert [0] .Green  = 63 << 8;
	vert [0] .Blue   = 63 << 8;
	vert [0] .Alpha  = 255;

	vert [1] .x      = rect.right;
	vert [1] .y      = rect.bottom; 
	vert [1] .Red    = 55 << 8;
	vert [1] .Green  = 55 << 8;
	vert [1] .Blue   = 55 << 8;
	vert [1] .Alpha  = 255;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

	Gdiplus::Graphics graphics(hdc);

	Gdiplus::Pen pen(Gdiplus::Color(51,51,51));

	graphics.DrawLine(&pen, 0, rect.top + 1, rect.right, rect.top+1);


	if(textbool)
	{
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, CVS_WHITE);

		DrawText(hdc, text.c_str(), text.length(), &rect,  DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	} else if(iconbool)
	{
		BITMAP bitmap;

		HDC memDC = CreateCompatibleDC(hdc);

		HGDIOBJ oldMap = SelectObject(memDC, m_Bitmap);

		GetObject(m_Bitmap, sizeof(bitmap), &bitmap);



		SetBkMode(hdc, TRANSPARENT);


		SetStretchBltMode(memDC, HALFTONE);

		TransparentBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, RGB(0,202,203));


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
	if(result != -1)
	{
		return result;
	}

	return DefSubclassProc(hWnd, msg,wParam, lParam);
}

void CVS_Button::UpdatePosition()
{
	if(!MoveWindow(hWnd, m_Rect.left, m_Rect.top, m_Rect.right - m_Rect.left, m_Rect.bottom- m_Rect.top, FALSE))
	{
		printf("Error\n");
	}
}

CVS_TabContent::CVS_TabContent(CVS_Tab* parent):parent(parent)
{
	hWnd = CreateWindow(WC_STATIC, "Content", WS_CHILD | WS_VISIBLE | WS_BORDER, 0, parent->m_TabMargins.bottom + 2, parent->m_Rect.right - parent->m_Rect.left, parent->m_Rect.bottom - parent->m_Rect.top - 30, parent->hWnd, NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, &CVS_TabContent::TabContProc, CVS_TABCONT, (DWORD_PTR)this);

	m_Rect.left = 0;
	m_Rect.right = parent->m_Rect.right - parent->m_Rect.left;
	m_Rect.top = 0;
	m_Rect.bottom = parent->m_Rect.bottom - parent->m_Rect.top - 30;
	

	if(hWnd == NULL)
	{
		printf("Error\n");
	}

}

int CVS_TabContent::ParseMsg(HWND Hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
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

			if(hdc == NULL) 
			{
				printf("error\n");
				return -1;
			}

			HBRUSH mainBrush = CreateSolidBrush(CVS_GREY);
			HPEN mainPen = CreatePen(PS_SOLID, 1, CVS_GREY);
			
			HGDIOBJ oldBrush = SelectObject(hdc, mainBrush);
			HGDIOBJ oldPen = SelectObject(hdc, mainPen);
	

			FillRect(hdc, &rect, mainBrush);
			
			SelectObject(hdc,oldBrush);
			SelectObject(hdc,oldPen);
			
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
	SetWindowPos(hWnd, HWND_TOP, 0, parent->m_TabMargins.bottom + 2,  parent->m_Rect.right - parent->m_Rect.left, parent->m_Rect.bottom - parent->m_Rect.top - 30,SWP_SHOWWINDOW);
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
	int result = ((CVS_TabContent*) pointer)->ParseMsg (hWnd, msg, wParam, lParam);
	if(result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd,msg, wParam,lParam);
}

CVS_Button* CVS_TabContent::AddButton(UINT bitmapId, int x, int y, int w, int h)
{
	return NULL;
}

CVS_TabSlot::CVS_TabSlot(std::string name, CVS_Tab* tabparent):data(name), parent(tabparent), m_Selected(false)
{
	hWnd = CreateWindow(WC_BUTTON, name.c_str(), WS_CHILD | WS_VISIBLE , 0, 0, 60, 25, tabparent->hWnd, NULL, CVS_AppInstance, NULL);
	content = new CVS_TabContent(tabparent);

	SetWindowSubclass(hWnd, CVS_TabSlot::TabSlotProc, CVS_TABSLOT, (DWORD_PTR)this);
}

int CVS_TabSlot::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if(!m_Selected)
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
			if(hdc == NULL)
			{
				MessageBox(NULL,"Error painting", "Error", MB_OK);
			}



			HBRUSH mainBrush;
			HPEN borderPen = CreatePen(PS_SOLID, 1, CVS_DARK_BLACK);
			

			if(m_Selected)
			{
				mainBrush  = CreateSolidBrush(CVS_GREY);
			}else {
				mainBrush = CreateSolidBrush(CVS_DARK_GREY);
			}

			FillRect(hdc, &rect, mainBrush);

			Gdiplus::Pen pen(Gdiplus::Color(40,40,40));

			Gdiplus::Graphics graphics(hdc);

			graphics.DrawLine(&pen, rect.right-1, rect.top, rect.right-1, rect.bottom);

			if(m_Selected)
			{
				SetTextColor(hdc, CVS_WHITE);
				pen.SetColor(Gdiplus::Color(106,106,106));
				graphics.DrawLine(&pen, rect.left, rect.top, rect.right-2, rect.top );
			} else {
				SetTextColor(hdc, CVS_LIGHT_GREY);
				pen.SetColor(Gdiplus::Color(76,76,76));
				graphics.DrawLine(&pen, rect.left , rect.top, rect.right-2, rect.top );
			}

			SetBkMode(hdc, TRANSPARENT);

			HGDIOBJ oldfont = SelectObject(hdc, CVS_Tab::font);



			DrawText(hdc, data.c_str(), data.length(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER );

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
	if(result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void CVS_TabSlot::UpdatePos()
{

	if(!MoveWindow(hWnd, m_Rect.left, m_Rect.top, m_Rect.right-m_Rect.left, m_Rect.bottom - m_Rect.top, TRUE))
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

CVS_SceneView::CVS_SceneView(CVS_Gui* gui, int x, int y, int w, int h):Scene(NULL), Cam(NULL)
{
	Cam = new CVS_Camera();

	hWnd = CreateWindow(GLOBALSTATEMACHINE.m_WindowSub.className, "OpenGL", WS_CHILD | WS_VISIBLE | CS_OWNDC, x,y,w,h, gui->window->hWnd, NULL, CVS_AppInstance,NULL);

	printf("Creating gl context\n");



	SetWindowSubclass(hWnd, CVS_SceneView::SceneViewCallback, 10, (DWORD_PTR) this);
	
	if(!( glHdc = GetDC(hWnd)))
	{
		printf("Error getting dc\n");
	}

	this->m_Renderer = GLOBALSTATEMACHINE.m_RenderSub.createNewRenderer(glHdc);

	MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);

	GLOBALSTATEMACHINE.m_App->AddToUpdate(this);



	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
}

int CVS_SceneView::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			printf("Drawing\n");

			PAINTSTRUCT ps;

			BeginPaint(hWnd, &ps);

			Render();


			EndPaint(hWnd, &ps);
		}
		return 0;
	}
	return -1;
}

void CVS_SceneView::Render()
{
	CVS_View View = Cam->getView();
	m_Renderer->Render(Scene, View);
}

LRESULT CALLBACK CVS_SceneView::SceneViewCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	int result = ((CVS_SceneView*)pointer)->ParseMsg(msg, wParam, lParam);
	if(result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void CVS_SceneView::SetSize(int x, int y, int w, int h)
{
	MoveWindow(hWnd, x, y, w, h, TRUE);
	glViewport(0,0,w,h);
	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
	Cam->aspectRatio = (float)w/(float)h;
	Cam->UpdateView();
}

HFONT CVS_Tab::font = NULL;



CVS_Tab::CVS_Tab(CVS_Window* parent, int x, int y, int w, int h)
{
	if(font == NULL)
	{
		font = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "Arial");
	}

	this->parent = parent;
	RECT rcClient;

	INITCOMMONCONTROLSEX icex;

	TCITEM tie;

	int i;

	TCHAR achTemp[256];

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);


	hWnd = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, x,y,w,h, parent->hWnd, NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, &CVS_GuiManager::GuiCallback, CVS_TABCTRL, (DWORD_PTR)this);

	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;

	m_TabMargins.left = 0;
	m_TabMargins.right = w;
	m_TabMargins.top = 0;
	m_TabMargins.bottom = 25;



	if(hWnd == NULL)
	{
		MessageBox(parent->hWnd, "Error making tab!", "Error", MB_OK);
	}

}

int CVS_Tab::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
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

			Gdiplus::Pen pen(Gdiplus::Color(40,40,40));

			for(int i = 0, e = this->m_Slots.size(); i < e; ++i)
			{
				rect.left = m_Slots[i]->m_Rect.left;
				rect.right =m_Slots[i]->m_Rect.right;
				rect.top = m_Slots[i]->m_Rect.top;
				rect.bottom = m_Slots[i]->m_Rect.bottom;
				if(m_Slots[i]->m_Selected)
				{

					pen.SetColor(Gdiplus::Color(106,106,106));
					graphics.DrawLine(&pen, rect.left, rect.bottom + 1, 0, rect.bottom + 1);
					graphics.DrawLine(&pen, rect.right-1, rect.bottom + 1, m_Rect.right - rect.left, rect.bottom + 1);

					pen.SetColor(Gdiplus::Color(40,40,40));
					graphics.DrawLine(&pen, rect.left, rect.bottom, 0, rect.bottom);
					graphics.DrawLine(&pen, rect.right, rect.bottom, m_Rect.right - m_Rect.left, rect.bottom);
					pen.SetColor(Gdiplus::Color(83,83,83));
					graphics.DrawLine(&pen, rect.left, rect.bottom + 1, rect.right-2, rect.bottom + 1);
					graphics.DrawLine(&pen, rect.left, rect.bottom , rect.right-2, rect.bottom );
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
	for(int i = 0, e = m_Slots.size(); i < e; ++i)
	{
		if(m_Slots[i] != slot)
		{
			m_Slots[i]->HideContent();
			m_Slots[i]->m_Selected = false;
			m_Slots[i]->Repaint();
		} 
	}
}

bool CVS_Tab::CalibrateLocation()
{
	int offX = 0;
	int y = 0;
	for(int i = 0; i < m_Slots.size(); ++i)
	{
		m_Slots[i]->m_Rect.left = offX;
		m_Slots[i]->m_Rect.right = offX + 60;
		m_Slots[i]->m_Rect.top = y;
		m_Slots[i]->m_Rect.bottom = y +25;
		offX += 60;
		m_Slots[i]->UpdatePos();
	}
	return true;
}

void CVS_Tab::addTab(std::string name, int pos)
{
	CVS_TabSlot* slot = new CVS_TabSlot(name, this);
	if(m_Slots.size() == 0)
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
	for(int i = 0, e = this->m_Slots.size(); i < e; ++i)
	{
		m_Slots[i]->content->OnResize();
	}
	for(int i = 0, e = m_Slots.size(); i < e; ++i)
	{
		if(m_Slots[i]->m_Selected)
		{
			this->HideAllContent(m_Slots[i]);
			break;
		}
	}
}

CVS_ToolBar::CVS_ToolBar(CVS_Gui* gui, int x, int y, int w, int h)
{
	this->parent = gui->window;
	hWnd = CreateWindow(GLOBALSTATEMACHINE.m_WindowSub.className, "LOL", WS_CHILD |  WS_VISIBLE , 0, y, gui->window->width, h, gui->window->hWnd, NULL, CVS_AppInstance, NULL);
	
	SetWindowSubclass(hWnd, CVS_ToolBar::ToolBarCallBack, CVS_TOOLBAR, (DWORD_PTR)this);

	m_Rect.left = 0;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
	m_Rect.right = gui->window->width;


	if(hWnd == NULL)
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
	switch(msg)
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

			Gdiplus::Pen pen(Gdiplus::Color(106,106,106));

			Gdiplus::Graphics graphics(hdc);

			RECT rect;

			rect.top = 0;
			rect.left = 0;
			rect.right = m_Rect.right - m_Rect.left;
			rect.bottom = m_Rect.bottom - m_Rect.top;

			Rectangle(hdc, 0,0, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top);

			graphics.DrawLine(&pen, 1, 1,m_Rect.right-m_Rect.left - 1, 1);

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
	CVS_Button* newButton = new CVS_Button(bitmapID,this, x, 0, w, h);
	buttons.push_back(newButton);
	CalibrateLocation();
	return newButton;
}

bool CVS_ToolBar::CalibrateLocation()
{
	int offX = 0;
	int border = 5;
	int offY = 0;
	for(int i = 0, e = buttons.size(); i < e; ++i)
	{
		int left = buttons[i]->m_Rect.left;
		int top = buttons[i]->m_Rect.top;
		buttons[i]->m_Rect.left = offX + border;

		offY = (m_Rect.bottom - m_Rect.top - buttons[i]->m_Rect.bottom + buttons[i]->m_Rect.top)/2;

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
	SetWindowPos(hWnd, HWND_TOP, x,y,w,h, SWP_SHOWWINDOW );
	m_Rect.left = x;
	m_Rect.right = w + x;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
	CalibrateLocation();
	RedrawWindow(hWnd, NULL, NULL,  RDW_INVALIDATE);
}

LRESULT CALLBACK CVS_ToolBar::ToolBarCallBack(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{

	int result = ((CVS_ToolBar*) pointer) -> ParseMsg(msg, wParam, lParam);
	if(result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
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

void CVS_Layout::onResize()
{

}

CVS_EditorLayout::CVS_EditorLayout(CVS_Gui* gui): toolbarCell(this), rightbarCell(this), leftbarCell(this), bottomCell(this), renderCell(this)
{
	window = gui->window;
	CVS_ToolBar* ToolBar = new CVS_ToolBar(gui, 0,0,0,0);
	CVS_Tab* Tab = new CVS_Tab(gui->window, 0, 0, 0, 0);
	CVS_Tab* Tabbot = new CVS_Tab(gui->window,0,0,0,0);
	CVS_SceneView* view = new CVS_SceneView(gui, 0,0,0,0);


	Tab->addTab("Scene",0);
	Tab->addTab("Assets",0);

	Tabbot->addTab("Debug", 0);

	ToolBar->AddButton(CVS_ARROW, 0, 0, 25,25);
	ToolBar->AddButton(CVS_MOVE,0,0,25,25);
	ToolBar->AddButton(CVS_ROTATE,0,0,25,25);

	toolbarCell.SetObj(ToolBar);
	leftbarCell.SetObj(Tab);
	bottomCell.SetObj(Tabbot);
	renderCell.SetObj(view);


	toolbarCell.SetSize(0,0, gui->window->getClientWidth(), 30);
	leftbarCell.SetSize(window->getClientWidth() - 300, 30, 300, window->getClientHeight() - 200);
	bottomCell.SetSize(0, window->getClientHeight() - 170, window->getClientWidth(), 170);
	renderCell.SetSize(0, 30, window->getClientWidth() - 300, window->getClientHeight() - 200);
}


void CVS_EditorLayout::onResize()
{
	toolbarCell.SetSize(0,0,window->getClientWidth(), 30);
	leftbarCell.SetSize(window->getClientWidth() - 300, 30, 300, window->getClientHeight() - 200);
	bottomCell.SetSize(0, window->getClientHeight() - 170, window->getClientWidth(), 170);
	renderCell.SetSize(0, 30, window->getClientWidth() - 300, window->getClientHeight() - 200);
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