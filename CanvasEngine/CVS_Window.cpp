#include "Canvas.h"


/*Window Declaration*/
CVS_Window::CVS_Window(std::string name, int x, int y, int w, int h, int FLAGS):parent(NULL)
{
	gui = new CVS_Gui(this);
	m_Active = false;
	//Win32 Windows Creation
#ifdef CVS_WIN32
	//REQREFINEMENTS:~ Allow more style changes and window options.
	switch(FLAGS)
	{

	case CVS_WINDOW_MDI:
		{
			hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, GLOBALSTATEMACHINE.m_WindowSub.className_MDI, name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, CVS_AppInstance, NULL);
			if(hWnd == NULL)
			{
				MessageBox(NULL, "CanvasWindow Creation failed", "Error", MB_OK);
				PostQuitMessage(0);
				return;
			}
			m_Active = true;
			g_MDIClient = GLOBALSTATEMACHINE.m_WindowSub.temp;
			ShowWindow(hWnd, CVS_CmdShow);
			UpdateWindow(hWnd);
		}
		break;
	case CVS_WINDOW:
	default:
		{
			hWnd = CreateWindow(GLOBALSTATEMACHINE.m_WindowSub.className, name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, CVS_AppInstance, NULL);
			if(hWnd == NULL)
			{
				MessageBox(NULL, "CanvasWindow Creation failed", "Error", MB_OK);
				PostQuitMessage(0);
				return;
			}
			m_Active = true;

			ShowWindow(hWnd, CVS_CmdShow);
			UpdateWindow(hWnd);
		}
	}
	width = w;
	height = h;

#endif
}

CVS_Window* CVS_Window::CreateSubWindow(std::string name, int x, int y, int w, int h, int flags)
{
	CVS_Window* newWindow = new CVS_Window(name, x, y, w, h, flags, this);
	this->children.push_back(newWindow);
	return newWindow;
}

CVS_Window::CVS_Window(std::string name, int x, int y, int w, int h, int Flags, CVS_Window* parent):parent(parent)
{
	gui = new CVS_Gui(this);
	//TODO:~ add flags response
	m_Active = false;

	MDICREATESTRUCT mcs;

	mcs.szTitle = name.c_str();
	mcs.szClass = GLOBALSTATEMACHINE.m_WindowSub.className_MDIChild;
	mcs.hOwner = CVS_AppInstance;
	mcs.x = 0;
    mcs.y = 0;
	mcs.cx = w;
	mcs.cy = y;
	mcs.style = MDIS_ALLCHILDSTYLES;

	hWnd = (HWND)SendMessage(GLOBALSTATEMACHINE.m_WindowSub.temp, WM_MDICREATE, 0, (LONG) &mcs); 
	if(hWnd == NULL)
	{
		printf("Error!\n");
		return;
	}
	printf("Success!\n");
	width = w;
	height = h;

	DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED; // DWMNCRP_DISABLED to toggle back
	DwmSetWindowAttribute(hWnd, 
                      DWMWA_NCRENDERING_POLICY, 
                      (void*)&policy, 
                      sizeof(DWMNCRENDERINGPOLICY));
}

void CVS_Window::CreateMenuMain()
{
	HMENU hMenu = CreateMenu();

	HMENU FILE_SUB = CreatePopupMenu();
	HMENU FILE_NEW_SUB = CreatePopupMenu();

	HMENU EDIT_SUB = CreatePopupMenu();

	HMENU CREATE_SUB = CreatePopupMenu();

	int CHILD_FLAG = MF_OWNERDRAW | MF_STRING;
	int SUB_FLAG = MF_OWNERDRAW | MF_STRING | MF_POPUP;

	COLORREF color = RGB(64,64,64);

	HBRUSH bgHbr = CreateSolidBrush(color);

	//File Menu
	AppendMenu(FILE_NEW_SUB, CHILD_FLAG, CVS_FILE_NEW_PROJECT, "Project");
	AppendMenu(FILE_NEW_SUB, CHILD_FLAG, CVS_FILE_NEW_SCENE, "Scene");
	AppendMenu(FILE_SUB, SUB_FLAG, (UINT) FILE_NEW_SUB, "New");


	AppendMenu(FILE_SUB, CHILD_FLAG, CVS_FILE_OPEN40030, "Open");
	AppendMenu(FILE_SUB, CHILD_FLAG, CVS_FILE_SAVE40031, "Save");
	AppendMenu(FILE_SUB, CHILD_FLAG, CVS_FILE_SAVEAS40032, "Save As...");
	AppendMenu(FILE_SUB, CHILD_FLAG, CVS_FILE_EXIT, "Exit");
	AppendMenu(hMenu, SUB_FLAG, (UINT) FILE_SUB, "File");
	BarID.push_back((UINT)FILE_SUB);
	
	//Edit Menu
	AppendMenu(EDIT_SUB, CHILD_FLAG, CVS_EDIT_COPY, "Copy");
	AppendMenu(EDIT_SUB, CHILD_FLAG, CVS_EDIT_PASTE, "Paste");
	AppendMenu(EDIT_SUB, CHILD_FLAG, CVS_EDIT_CUT, "Cut");
	AppendMenu(hMenu, SUB_FLAG, (UINT)  EDIT_SUB, "Edit");
	
	AppendMenu(CREATE_SUB, CHILD_FLAG, CVS_CREATE_SHADER, "New Shader...");
	AppendMenu(CREATE_SUB, CHILD_FLAG, CVS_CREATE_MODEL, "New Model...");
	AppendMenu(CREATE_SUB, CHILD_FLAG, CVS_CREATE_SCENE, "New Scene...");
	AppendMenu(hMenu, SUB_FLAG, (UINT) CREATE_SUB, "Create");
	BarID.push_back((UINT)CREATE_SUB);
	BarID.push_back((UINT)EDIT_SUB);

	MENUINFO info;

	info.cbSize = sizeof(MENUINFO);
	info.hbrBack = bgHbr;
	info.fMask = MIM_BACKGROUND | MIM_MAXHEIGHT;
	info.cyMax = 90;
	if(!SetMenuInfo(hMenu, &info))
	{
		printf("Error setting menu style %s\n", GetLastError());
	}

	SetMenu(hWnd, hMenu);
}

bool CVS_Window::MeasureMenu(WPARAM wParam, LPARAM lParam)
{

	HDC hdc = NULL;
	hdc = GetDC(hWnd);

	LPMEASUREITEMSTRUCT mis = (LPMEASUREITEMSTRUCT)lParam;

	char* DATA = (char*) mis->itemData;

	size_t pcch = 0;
		
	if(StringCchLength(DATA, STRSAFE_MAX_CCH, &pcch) != S_OK)
	{
		printf("Error String CchLength\n");
		return false;
	}

	bool bar = false;

	for(int i = 0, e = BarID.size(); i < e; ++i)
	{
		if(BarID[i] == mis->itemID)
		{
			bar = true;
		}
	}

	SIZE size;

	GetTextExtentPoint32(hdc, DATA, pcch, &size);

	if( bar ) 
	{
		mis->itemHeight = size.cy;
		mis->itemWidth = size.cx;
	} else {
		mis->itemHeight = 20;
		mis->itemWidth = iClamp(size.cx, 140, INT_MAX);
	}


	ReleaseDC(hWnd, hdc);
	return true;
}

bool CVS_Window::DrawMenu(WPARAM wParam, LPARAM lParam)
{

	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;

	bool bar = false;

	for(int i = 0, e = BarID.size(); i < e; ++i)
	{
		if(BarID[i] == dis->itemID)
		{
			bar = true;
		}
	}


	char* DATA = (char*) dis->itemData;

	COLORREF WHITE = RGB(190,190,190);
	COLORREF DARK = RGB(64,64,64);
	COLORREF BLUE = RGB(19, 107, 163);
	COLORREF GREY = RGB(100,100,100);

	size_t pcch = 0;

	if(StringCchLength(DATA, STRSAFE_MAX_CCH, &pcch) != S_OK)
	{
		printf("Error\n");
		return false;
	}



	WORD wCheckX = GetSystemMetrics(SM_CXMENUCHECK);


	int nTextX = wCheckX + dis->rcItem.left - 3;
	int nTextY = dis->rcItem.top;
	if(bar)
	{
		nTextY += 1;
	}

	
	if( dis->itemState & ODS_SELECTED)
	{

		SetTextColor(dis->hDC, WHITE);

		if(SetBkColor(dis -> hDC, GREY) == CLR_INVALID)
		{
			printf("Error drawing background %s\n", GetLastError());
			return false;
		}
	
	} else {
		SetTextColor(dis->hDC, WHITE);

		if(SetBkColor(dis -> hDC, DARK) == CLR_INVALID)
		{
			printf("Error drawing background %s\n", GetLastError());
			return false;
		}
	}
	
	
	SetBkMode(dis->hDC, TRANSPARENT);

	ExtTextOut(dis->hDC, nTextX, nTextY, ETO_OPAQUE, &dis->rcItem, DATA, pcch, NULL);

	/*HDC memDC;
	HBRUSH brush;
	HBITMAP hbit;
	BLENDFUNCTION bf;
	int x,y,width, height;

	x = dis->rcItem.left;
	y = dis->rcItem.top;
	width = dis->rcItem.right-dis->rcItem.left;
	height = dis->rcItem.bottom - dis->rcItem.top;

	ZeroMemory(&bf, sizeof(BLENDFUNCTION));

	bf.SourceConstantAlpha = 150;

	memDC = CreateCompatibleDC(dis->hDC);
	hbit = CreateCompatibleBitmap(dis->hDC, width, height);
	brush=CreateSolidBrush(DARK);
	SelectObject(memDC, brush);
	SelectObject(memDC,hbit);
	PatBlt(memDC, 0,0,width,height,PATCOPY);
	AlphaBlend(dis->hDC, x,y,width,height + 50,memDC,0,0,width,height,bf);
	DeleteObject(hbit);
	DeleteObject(brush);
	DeleteDC(memDC);
	
	AlphaBlend(dis->hDC, x, y, width, height, memDC, 0, 0, width, height, bf);*/

	return true;
}

bool CVS_Window::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{

	//Switches
	switch(msg)
	{	
	case WM_NCCALCSIZE:
		{
			if((UINT)wParam == 1)
			{
		   // Calculate new NCCALCSIZE_PARAMS based on custom NCA inset.
			NCCALCSIZE_PARAMS *pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

			pncsp->rgrc[0].left   = pncsp->rgrc[0].left   + 0;
			pncsp->rgrc[0].top    = pncsp->rgrc[0].top    + 0;
			pncsp->rgrc[0].right  = pncsp->rgrc[0].right  - 0;
			pncsp->rgrc[0].bottom = pncsp->rgrc[0].bottom - 0;


				printf("Calc size\n");
				return 0;
			}
		}
		break;
	case WM_NCACTIVATE:
		{

		}
		break;
	case WM_NCPAINT:

		break;
	case WM_INITMENUPOPUP:
		
		break;
	case WM_PAINT:
		{

		}
		break;
	case WM_SIZE:
		{
			if(gui->Layout!= NULL)
			{
				gui->Layout->onResize();
			}
		}
		break;
	//Response to commands from menu
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case CVS_FILE_NEW_SCENE:
			OpenFile();
			break;
		case CVS_FILE_OPEN40030:
			OpenFile();
			break;
		case CVS_FILE_EXIT:
			Close();
			return true;
		}
		return true;
	case WM_MEASUREITEM:
		{
			//Measure menu items.
			if(wParam == 0)
			{
				MeasureMenu(wParam, lParam);
			}

			return true;
		}
	case WM_DRAWITEM:
		{
			if(wParam == 0)
			{
				if(((LPDRAWITEMSTRUCT) lParam) -> CtlType == ODT_MENU)
					DrawMenu(wParam, lParam);
				else if(((LPDRAWITEMSTRUCT) lParam) -> CtlType == ODT_TAB)
				{
					printf("Tab!\n");

				}
				return 0;
			}

		}
	//On window creation
	case WM_CREATE:
		MessageBox(NULL, "Create", "Lol", MB_OK);
		//TODO:~ Add Renderer creation here.
		break;
	//If window is to be destroyed, return false and pass message to the System
	case WM_DESTROY:
		return false;
	//The close button was pressed. TODO:~ Add callback to main CVS_App so they can respond.
	case WM_CLOSE:
		return Close();
	}
	return false;
}

int CVS_Window::getClientWidth()
{
	RECT rect;

	GetClientRect(hWnd, &rect);

	return rect.right - rect.left;
}

int CVS_Window::getClientHeight()
{
	RECT rect;

	GetClientRect(hWnd, &rect);

	return rect.bottom - rect.top;
}

bool CVS_Window::ParseMsgMDI(UINT msg, WPARAM wParam, LPARAM lParam)
{

	//Switches
	switch(msg)
	{

	//Response to commands from menu
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{

			return true;
		}
		return true;
	//On window creation
	case WM_CREATE:
		MessageBox(NULL, "Create", "Lol", MB_OK);
		//TODO:~ Add Renderer creation here.
		break;
	//If window is to be destroyed, return false and pass message to the System
	case WM_DESTROY:
		return false;
	//The close button was pressed. TODO:~ Add callback to main CVS_App so they can respond.
	case WM_CLOSE:
		return Close();
	}
	return false;
}

CVS_Tab* CVS_Window::CreateNewTab(std::string header, int x, int y, int w, int h)
{
	CVS_Tab* newTab = new CVS_Tab(this, x, y, w, h);
	gui->objects.push_back(newTab);
	GLOBALSTATEMACHINE.m_GUISub.tabctrls.push_back(newTab);
	return newTab;
}

bool CVS_Window::OpenFile()
{
	char szFile[100];

	OPENFILENAME ofn;

	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(GetOpenFileName(&ofn))
	{
		GLOBALSTATEMACHINE.m_App->FileOpen(szFile);
		return true;
	}

	return false;
}

bool CVS_Window::Close()
{
	int closeResponse = MessageBox(hWnd, "Are you sure you want to exit Canvas Engine?", "", MB_OKCANCEL);
	switch (closeResponse)
	{
	case IDCANCEL:
		return true;
	case IDOK:
		DestroyWindow(hWnd);
		m_Active = false;
		return false;
	}
	return false;
}

/*WindowSystem Declaration*/

CVS_WindowSystem::CVS_WindowSystem():className("CanvasEngine_Wnd"),className_MDI("CanvasEngine_Wnd_MDI"),className_MDIChild("CanvasEngine_Wnd_MDIChild")
{
}


bool CVS_WindowSystem::Initialize()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin); 
	freopen("CONOUT$", "w", stdout); 
	freopen("CONOUT$", "w", stderr); 
	/*WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = CVS_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
	wc.hInstance     = CVS_AppInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(CVS_MENU);
	wc.lpszClassName = className_MDI;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", 
		
		,
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	wc.lpfnWndProc = CVS_WndProcChild
	return true;*/
	return true;
}

#ifdef CVS_WIN32

typedef struct _ITEM
{
	LPSTR psz;
	HFONT hFont;
} WNDITEM;

CVS_Window* GetWindow(HWND hWnd)
{
	for(int i = 0, e = GLOBALSTATEMACHINE.m_WindowSub.windows.size(); i < e; ++i)
	{
		if(GLOBALSTATEMACHINE.m_WindowSub.windows[i]->hWnd == hWnd)
		{
			return GLOBALSTATEMACHINE.m_WindowSub.windows[i];
		}
	}
	return NULL;
}

#endif
LRESULT CALLBACK CVS_WindowSystem::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch(msg)
	{
	case WM_CREATE:
		{

			//Drawing out custom menu item.

			break;
		}
	case WM_QUIT:
		PostQuitMessage(1);
		break;


	default:
		for(int i = 0, e = windows.size(); i < e; ++i)
		{
			if(hWnd == windows[i]->hWnd)
			{
				if(windows[i]->ParseMsg(msg, wParam, lParam))
					return 0;
			}
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK CVS_WindowSystem::WndProc_MDIChild(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		
		break;
	case WM_MDIACTIVATE:
		return 0;
	default:
		break;
	}
	return DefMDIChildProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK CVS_WindowSystem::WndProc_MDI(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_NCPAINT:
		printf("lol\n");
		break;
	case WM_CREATE:
		{
			CLIENTCREATESTRUCT ccs;

			ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 5);
			ccs.idFirstChild = CVS_MDI_INITIAL;
			
			this->temp = CreateWindowEx(WS_EX_CLIENTEDGE, "MDICLIENT",
				NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
				CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT, 300, hWnd, (HMENU)CVS_MAIN_MDI, CVS_AppInstance, (LPSTR) &ccs);
	
			if(temp == NULL)
			{
				MessageBox(hWnd, "Could not Create MDI Client.", "Error", MB_OK |MB_ICONERROR);
			}

			ShowWindow(this->temp, SW_SHOW);

		}
	case WM_QUIT:
		PostQuitMessage(1);
		break;
	default:
		for(int i = 0, e = windows.size(); i < e; ++i)
		{
			if(windows[i]->ParseMsgMDI(msg, wParam, lParam))
			{
				return 0;
			}
		}
		break;
	}
	return DefFrameProc(hWnd, temp, msg, wParam, lParam);
}



bool CVS_WindowSystem::Update()
{
	return true;
}

CVS_Window* CVS_WindowSystem::createNewWindow(std::string name, int x, int y, int w, int h, int flags)
{
	CVS_Window* newWindow = new CVS_Window(name, x, y, w, h, flags);
	windows.push_back(newWindow);
	return windows.back();
}

bool CVS_WindowSystem::End()
{
	return true;
}