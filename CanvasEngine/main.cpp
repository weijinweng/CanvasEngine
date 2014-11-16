// CanvasEngineWin32.cpp : Defines the entry point for the application.
//

#include "Canvas.h"

const char g_szClassName[] = "myWindowClass";

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void SetupWindowsDebugConsole(void)
{
#ifdef _DEBUG
	// Create our debug console window
	// redirect the iostreams
	// and enable mouse scrolling
	AllocConsole();
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hConsole, &mode);
	SetConsoleMode(hConsole, mode & ~ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

#endif
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	SetupWindowsDebugConsole();
	CVS_Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	Editor app;
	app.Initialize();
	app.Run();
	app.End();
}