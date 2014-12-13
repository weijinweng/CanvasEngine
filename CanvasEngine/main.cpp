
#include "CanvasSDK.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	Canvas::Initialize(0, hInstance, hPrevInstance, nCmdShow);

	CVS_Main(0,0);
}