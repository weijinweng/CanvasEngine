#include "Canvas.h"
#include "CVS_GUI.h"


CVS_SceneView::CVS_SceneView(CVS_Gui* gui, int x, int y, int w, int h) :Scene(NULL), Cam(NULL)
{
	mMouseDown = false;
	mMiddleMouse = false;

	Cam = new CVS_Camera();

	hWnd = CreateWindow(GLOBALSTATEMACHINE.m_WindowSub.className, "OpenGL", WS_CHILD | WS_VISIBLE | CS_OWNDC, x, y, w, h, gui->window->hWnd, NULL, CVS_AppInstance, NULL);

	printf("Creating gl context\n");



	SetWindowSubclass(hWnd, CVS_SceneView::SceneViewCallback, 10, (DWORD_PTR) this);

	if (!(glHdc = GetDC(hWnd)))
	{
		printf("Error getting dc\n");
	}

	this->m_Renderer = GLOBALSTATEMACHINE.m_RenderSub.createNewRenderer(glHdc);

	MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);

	GLOBALSTATEMACHINE.m_App->AddToUpdate(this);

	mSelection = new CVS_Selection(this);

	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
}

int CVS_SceneView::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
					   switch (wParam)
					   {
					   case 'W':
						   this->Cam->shiftLocalPos(0, 0, 0.2);
						   break;
					   case 'S':
						   this->Cam->shiftLocalPos(0, 0, -0.2);
						   break;
					   case 'A':
						   this->Cam->shiftLocalPos(-0.2, 0, 0);
						   break;
					   case 'D':
						   this->Cam->shiftLocalPos(0.2, 0, 0);
						   break;
					   case 'Q':
						   this->Cam->shiftLocalPos(0, -0.2, 0);
						   break;
					   case 'E':
						   this->Cam->shiftLocalPos(0, 0.2, 0);
						   break;
					   }
	}
		return 0;
	case WM_MBUTTONDOWN:
/*	case WM_RBUTTONDOWN:*/
	{
						   CVS_View view = Cam->getView();
						   mSelection->Render(Scene, &view);
						   SelectData data = mSelection->getPrimitiveID(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
						   ((Editor*)GLOBALSTATEMACHINE.m_App)->Message(RENDER_SELECTION, 0, (LONG_PTR)data.ObjectID);
	}
		return 0;
	case WM_LBUTTONDOWN:
	{
						   if (!mMouseDown)
						   {
							   mMouseDown = true;
							   mouseX = GET_X_LPARAM(lParam);
							   mouseY = GET_Y_LPARAM(lParam);
						   }
	}
		return 0;
	case WM_RBUTTONDOWN:
/*	case WM_MBUTTONDOWN:*/
	{
						   mMiddleMouse = true;
						   mouseX = GET_X_LPARAM(lParam);
						   mouseY = GET_Y_LPARAM(lParam);
	}
		return 0;
	case WM_RBUTTONUP:
/*	case WM_MBUTTONUP:*/
	{
						 mMiddleMouse = false;
	}
		return 0;
	case WM_LBUTTONUP:
	{
						 mMouseDown = false;
	}
		return 0;
	case WM_MOUSEMOVE:
	{
						 int deltaX = -(GET_X_LPARAM(lParam) - mouseX);
						 int deltaY = GET_Y_LPARAM(lParam) - mouseY;
						 if (mMouseDown)
						 {


							 this->Cam->RotateAroundTarget(-1.0f*(float)deltaY / 600, (float)deltaX / 600, 0);
						 }

						 if (mMouseDown || mMiddleMouse)
						 {
							 mouseX = GET_X_LPARAM(lParam);
							 mouseY = GET_Y_LPARAM(lParam);
						 }

						 if (mMiddleMouse)
						 {
							 this->Cam->shiftLocalPos((float)deltaX / 200, (float)deltaY / 200, 0);
						 }

						 TRACKMOUSEEVENT lpEventTrack;

						 lpEventTrack.cbSize = sizeof(lpEventTrack);
						 lpEventTrack.dwFlags = TME_LEAVE;
						 lpEventTrack.hwndTrack = hWnd;

						 TrackMouseEvent(&lpEventTrack);

						 SetFocus(hWnd);

						 RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
	}
		return 0;
	case WM_MOUSELEAVE:
	{
						  mMouseDown = false;
						  mMiddleMouse = false;
	}
		return 0;
	case WM_MOUSEWHEEL:
	{
						  int wheelScroll = GET_WHEEL_DELTA_WPARAM(wParam);
						  Cam->shiftLocalPos(0, 0, (float)wheelScroll / 100);
	}
		return 0;
	case WM_PAINT:
	{
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
	static int time = 0;
	LARGE_INTEGER newTime;
	QueryPerformanceCounter(&newTime);
	int deltaTime = newTime.QuadPart / 1000 - time;
	//printf("%d\n", deltaTime);
	time = newTime.QuadPart / 1000;

	CVS_View View = Cam->getView();
	m_Renderer->Render(Scene, View);
	mSelection->Render(Scene, &View);
}

LRESULT CALLBACK CVS_SceneView::SceneViewCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	int result = ((CVS_SceneView*)pointer)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
	{
		return result;
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void CVS_SceneView::SetSize(int x, int y, int w, int h)
{
	MoveWindow(hWnd, x, y, w, h, TRUE);
	glViewport(0, 0, w, h);

	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;

	Cam->aspectRatio = ((float)w) / ((float)h);
	Cam->UpdatePerspective();
	mSelection->UpdateSize();
}

