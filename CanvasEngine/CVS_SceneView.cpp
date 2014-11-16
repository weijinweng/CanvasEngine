#include "Canvas.h"
CVS_Selection::CVS_Selection(CVS_SceneView* view) :parent(view)
{
	this->mRenderBuffer = GLOBALSTATEMACHINE.m_RenderSub.createNewFramebuffer();
	this->mSelectionTexture = GLOBALSTATEMACHINE.m_RenderSub.createNewTexture(GL_TEXTURE_2D);

	int width = view->getWidth(), height = view->getHeight();

	mRenderBuffer->Bind(GL_FRAMEBUFFER);

	mSelectionTexture->loadData(GL_RGB, GL_RGB32F, 0, width ? width : 100, height ? height : 100, 0, NULL);
	mRenderBuffer->BindColorAttachment(mSelectionTexture, 0);

	mDepthTexture = GLOBALSTATEMACHINE.m_RenderSub.createNewTexture(GL_TEXTURE_2D);
	mDepthTexture->loadData(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, 0, width ? width : 100, height ? height : 100, 0, NULL);

	mRenderBuffer->BindDepthAttachment(mDepthTexture);

	mRenderBuffer->setDrawBuffer(GL_COLOR_ATTACHMENT0);
	mRenderBuffer->setReadBuffer(GL_NONE);

	if (!mRenderBuffer->GetBufferStatus())
	{
		MessageBox(NULL, "Error Generating buffer Status", "Error!", MB_OK | MB_ICONEXCLAMATION);
	}
	mRenderBuffer->unBind();

	mRenderProgram = GLOBALSTATEMACHINE.m_RenderSub.getRenderProgram("Selection");

	MVPLoc = mRenderProgram->getUniformHash("MVP");
	DrawIndexLoc = mRenderProgram->getUniformHash("DrawIndex");
	ObjectIndexLoc = mRenderProgram->getUniformHash("ObjectIndex");
}

void CVS_Selection::UpdateSize()
{
	int width = parent->getWidth(), height = parent->getHeight();

	mRenderBuffer->Bind(GL_FRAMEBUFFER);

	mSelectionTexture->loadData(GL_RGB, GL_RGB32F, 0, width ? width : 100, height ? height : 100, 0, NULL);
	mRenderBuffer->BindColorAttachment(mSelectionTexture, 0);

	mDepthTexture = GLOBALSTATEMACHINE.m_RenderSub.createNewTexture(GL_TEXTURE_2D);
	mDepthTexture->loadData(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, 0, width ? width : 100, height ? height : 100, 0, NULL);

	mRenderBuffer->BindDepthAttachment(mDepthTexture);

	mRenderBuffer->setDrawBuffer(GL_COLOR_ATTACHMENT0);
	mRenderBuffer->setReadBuffer(GL_NONE);

	if (!mRenderBuffer->GetBufferStatus())
	{
		MessageBox(NULL, "Error Generating buffer Status", "Error!", MB_OK | MB_ICONEXCLAMATION);
	}
	mRenderBuffer->unBind();
}

void CVS_Selection::Render(CVS_RenderScene* scene, CVS_View* view)
{
	mRenderBuffer->Bind(GL_DRAW_FRAMEBUFFER);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mRenderProgram->setAsCurrentProgram();

	for (int i = 0, e = scene->nodes.size(); i < e; ++i)
	{
		glUniform1i(this->ObjectIndexLoc, (GLint)scene->nodes[i]->msgData);
		glUniform1i(this->DrawIndexLoc, i);
		cmat4 mat = view->Pers * view->View * scene->nodes[i]->modelMatrix;

		glUniformMatrix4fv(this->MVPLoc, 1, GL_FALSE, glm::value_ptr(mat));

		scene->nodes[i]->mesh->Draw();
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClearColor(0.7, 0.7, 0.7, 1.0);
}

SelectData CVS_Selection::getPrimitiveID(int x, int y)
{
	SelectData data;

	RECT rect;

	GetClientRect(this->parent->hWnd, &rect);

	y = rect.bottom - rect.top - y - 1;

	mRenderBuffer->Bind(GL_READ_FRAMEBUFFER);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &data);
	printf("Data %p\n", (void*)((GLint)data.ObjectID));
	return data;
}

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
	case WM_RBUTTONDOWN:
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
	case WM_MBUTTONDOWN:
	{
						   mMiddleMouse = true;
						   mouseX = GET_X_LPARAM(lParam);
						   mouseY = GET_Y_LPARAM(lParam);
	}
		return 0;
	case WM_MBUTTONUP:
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

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

void CVS_SceneView::Render()
{


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

