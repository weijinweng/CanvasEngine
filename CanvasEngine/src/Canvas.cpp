#include "Canvas.h"

bool CVS_Initialized = false;
CVS_StateMachine GLOBALSTATEMACHINE;

HINSTANCE CVS_AppInstance;
HINSTANCE CVS_AppPrevInstance;
int CVS_CmdShow;
LPSTR CVS_CmdLine;

CVS_Timer::CVS_Timer()
{
	lastTime = 0;
	LARGE_INTEGER newTime;
	QueryPerformanceCounter(&newTime);
	lastTime = newTime.QuadPart;
}

int CVS_Timer::setFrame(UINT precision)
{
	int deltaTime = 0;

	LARGE_INTEGER newTime;
	QueryPerformanceCounter(&newTime);

	switch (precision)
	{
	case 0:
		deltaTime = newTime.QuadPart / (1000);
		return deltaTime;
	case 1:
		deltaTime = newTime.QuadPart;
		return deltaTime;
	default:
		return 0;
	}
	lastTime = newTime.QuadPart;
}

void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to)
{
	to[0][0] = (GLfloat)from->a1; to[1][0] = (GLfloat)from->a2;
	to[2][0] = (GLfloat)from->a3; to[3][0] = (GLfloat)from->a4;
	to[0][1] = (GLfloat)from->b1; to[1][1] = (GLfloat)from->b2;
	to[2][1] = (GLfloat)from->b3; to[3][1] = (GLfloat)from->b4;
	to[0][2] = (GLfloat)from->c1; to[1][2] = (GLfloat)from->c2;
	to[2][2] = (GLfloat)from->c3; to[3][2] = (GLfloat)from->c4;
	to[0][3] = (GLfloat)from->d1; to[1][3] = (GLfloat)from->d2;
	to[2][3] = (GLfloat)from->d3; to[3][3] = (GLfloat)from->d4;
}

void FbxAMatrixToMat4(const FbxAMatrix* _in, glm::mat4& _out)
{
	auto m = _in->Double44();
	_out = cmat4(
		cvec4(m[0][0], m[0][1], m[0][2], m[0][3]),
		cvec4(m[1][0], m[1][1], m[1][2], m[1][3]),
		cvec4(m[2][0], m[2][1], m[2][2], m[2][3]),
		cvec4(m[3][0], m[3][1], m[3][2], m[3][3]));

}


int iClamp(int value, int min, int max)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

GLenum convertToGLEnum(CVS_Enum enumerator)
{
	switch (enumerator)
	{
	case CVS_FLOAT:
		return GL_FLOAT;
	case CVS_UINT:
		return GL_UNSIGNED_INT;
	case CVS_INT:
		return GL_INT;
	case CVS_STATIC_DRAW:
		return GL_STATIC_DRAW;
	case CVS_ARRAY_BUFFER:
		return GL_ARRAY_BUFFER;
	case CVS_TRIANGLES:
		return GL_TRIANGLES;
	case CVS_ELEMENT_BUFFER:
		return GL_ELEMENT_ARRAY_BUFFER;
	default:
		return GL_FALSE;
	}
}

bool CVS_StateMachine::initialize()
{
	auto hr = true;
	hr &= m_RenderSub.Initialize();
	hr &= m_AnimationSub.Initialize();
	hr &= m_ResourceSub.Initialize();
	hr &= m_WindowSub.Initialize();

	return hr;
}

void testButtonFunction(void* lol)
{
	printf("Pressed!\n");
}

bool CVS_Initialize()
{
	return true;

}

LRESULT CALLBACK CVS_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GLOBALSTATEMACHINE.m_WindowSub.WndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK CVS_WndProcMDI(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GLOBALSTATEMACHINE.m_WindowSub.WndProc_MDI(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK CVS_WndProcMDIChild(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GLOBALSTATEMACHINE.m_WindowSub.WndProc_MDIChild(hWnd, msg, wParam, lParam);
}

ULONG_PTR gdiToken;

bool CVS_Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR mCmdLine, int mCmdNum)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL error\n", SDL_GetError());
		return false;
	}

	int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;

	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("Error\n");
		return false;
	}
	//Register Default window class
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = CVS_WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GLOBALSTATEMACHINE.m_WindowSub.className;
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(CVS_ICON));

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//Register MDI window class
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GLOBALSTATEMACHINE.m_WindowSub.className_MDI;
	wc.lpfnWndProc = CVS_WndProcMDI;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window MDI Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	wc.lpfnWndProc = CVS_WndProcMDIChild;
	wc.lpszClassName = GLOBALSTATEMACHINE.m_WindowSub.className_MDIChild;
	wc.lpszMenuName = (LPCTSTR)NULL;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window MDI Child Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	wc.lpfnWndProc = CVS_WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpszClassName = "OpenGL";

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Windows OPENGL Child Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}


	Gdiplus::GdiplusStartupInput gdiInput;

	Gdiplus::GdiplusStartupOutput gdiOutput;

	GdiplusStartup(&gdiToken, &gdiInput, &gdiOutput);



	//Register MDI child class
	CVS_AppInstance = hInstance;
	CVS_AppPrevInstance = hPrevInstance;
	CVS_CmdLine = mCmdLine;
	CVS_CmdShow = mCmdNum;


	if (!GLOBALSTATEMACHINE.initialize())
	{
		return false;
	}

	CVS_Initialized = true;
	return true;
}

// For constructing game object. remove asap
#include "CVS_Mesh.h"

bool Editor::Initialize()
{
	if (!CVS_Initialized)
	{
		return false;
	}
	GLOBALSTATEMACHINE.m_App = this;

	m_MainWindow = GLOBALSTATEMACHINE.m_WindowSub.createNewWindow("Canvas Editor", 0, 0, 1600, 900, CVS_NULL);
	m_MainWindow->gui->Layout = new CVS_EditorLayout(m_MainWindow->gui);
	m_MainWindow->CreateMenuMain();

	CVS_Scene* testScene = GLOBALSTATEMACHINE.m_WorldSub.createNewScene();
	// Load FBX to Resource System
	GLOBALSTATEMACHINE.m_ResourceSub.import("mannequin/mannequin.fbx");
	GLOBALSTATEMACHINE.m_ResourceSub.import("mannequin/Walk.fbx");
	
	// Get our mesh
	auto pMesh = static_cast<CVS_Mesh*>(GLOBALSTATEMACHINE.m_ResourceSub.get("HeroTPP", CVS_Resource::EType::Mesh));
	if (!pMesh)
	{
		assert(0);
	}

	// Get our animation
	auto pAnim = static_cast<CVS_Animation*>(GLOBALSTATEMACHINE.m_ResourceSub.get("Walk-Take0", CVS_Resource::EType::Animation));
	if (!pAnim)
	{
		assert(0);
	}

	// Create new GameObject, set mesh
	auto pGameObject = new CVS_GameObject("Mannequin");
	testScene->m_objects.push_back(pGameObject);
	pGameObject->transformNode.transform.scale = cvec3(0.02f, 0.02f, 0.02f);

	auto pRenderComp = new CVS_RenderComponent(pGameObject, testScene->m_pScene);
	pRenderComp->m_pNode->setMesh(pMesh);
	pGameObject->addComponent(pRenderComp);

	auto pAnimationComp = new CVS_AnimationComponent(pGameObject, pRenderComp);
	pAnimationComp->m_pAnimation = pAnim;
	pGameObject->addComponent(pAnimationComp);
	pAnimationComp->PlayAnim(pAnim, true);
	
	((CVS_EditorLayout*)m_MainWindow->gui->Layout)->setScene(testScene);
	mMainScene = testScene;
	/*
	CVS_Tab* tab = m_MainWindow->CreateNewTab("Lol", 1400,30,200,900);
	CVS_Tab* tab2 = m_MainWindow->CreateNewTab("FUCK", 0, 30, 200, 900);

	tab->addTab("hello", 0);
	tab->addTab("Second", 1);

	tab->m_Slots[0]->content->AddButton("Hello", 10,10,100,100);


	CVS_ToolBar* hello = new CVS_ToolBar(m_MainWindow->gui, 0, 0, 100, 30);

	hello->AddButton(CVS_ARROW, 0, 0, 25, 25);
	*/
	return true;
}

bool Editor::Run()
{
	if (!CVS_Initialized)
		return false;
	MSG Msg;
	bool quit = false;
	while (m_MainWindow->m_Active)
	{
		//Non blocking message transfer
		while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		for (int i = 0, e = updatables.size(); i < e; ++i)
		{
			GLOBALSTATEMACHINE.m_WorldSub.Update();
			GLOBALSTATEMACHINE.m_AnimationSub.Update();
			updatables[i]->Render();
		}

	}
	return true;
}

void Editor::AddToUpdate(CVS_SceneView* view)
{
	updatables.push_back(view);
}

int Editor::FileOpen(void* data)
{
	MessageBox(NULL, (char*)data, "File opened", MB_OK);
	return 1;
}

LONG_PTR Editor::Message(UINT msg, UINT sParam, LONG_PTR lParam)
{
	switch (msg)
	{
	case RENDER_SELECTION:
		if (lParam != 0)
		{
			CVS_RenderComponent* component = (CVS_RenderComponent*)(void*)((GLint)lParam);
			mSelected = component->object;
			printf("yay %s\n", mSelected->name.c_str());
		}
		return 0;
		break;
	}
}

bool Editor::End()
{
	return true;
}