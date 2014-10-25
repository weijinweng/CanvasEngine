#include "Canvas.h"

bool CVS_Initialized = false;
CVS_StateMachine GLOBALSTATEMACHINE;

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

GLenum convertToGLEnum(CVS_Enum enumerator){
	switch(enumerator)
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
	default :
		return GL_FALSE;
	}
}

bool CVS_StateMachine::initialize()
{
	if(!m_RenderSub.Initialize())
	{
		return false;
	}
	return m_WindowSub.Initialize();
}

void testButtonFunction(void* lol)
{
	printf("Pressed!\n");
}

bool CVS_Initialize()
{
	printf("Initialize CVS\n");
	if(CVS_Initialized)
		return true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return false;
	}
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);	

	int imgFlags = IMG_INIT_PNG;

	if( !(IMG_Init( imgFlags ) & imgFlags))
	{
		return false;
	}
	if(!GLOBALSTATEMACHINE.initialize())
	{
		return false;
	}
	FT_Library lib;
	if(FT_Init_FreeType(&lib)){
		printf("Error initializing font\n");
		return false;
	}

	if(TTF_Init() == -1)
	{
		printf("Error initializing ttf\n");
		return false;
	}

	GLOBALSTATEMACHINE.m_RenderSub.lib = lib;

	GLOBALSTATEMACHINE.m_RenderSub.loadFont("Default", "ChaletParisNineteenSixty.ttf");

	CVS_Initialized = true;
	printf("Initialized CVS\n");
	return true;
}

void boolToggle(void* data)
{
	*(bool*) data = !*(bool*)data;
}

void Minimize(void* window)
{
	SDL_MinimizeWindow((SDL_Window*) window);
}

bool Editor::Initialize()
{
	quit = false;
	if(!CVS_Initialized)
	{
		return false;
	}
	m_MainWindow = GLOBALSTATEMACHINE.m_WindowSub.createNewWindow("Canvas Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900);

	CVS_Button* closeButton = m_MainWindow->gui->addButton(0,0,50,30,boolToggle, &quit);

	closeButton->offHoverColor = CVS_ColorRGBA(0.9,0.9,0.9,1.0);
	closeButton->onHoverColor = CVS_ColorRGBA(1.0, 1.0, 1.0, 1.0);
	closeButton->mouseDownColor = CVS_ColorRGBA(0.0,0.23137254,0.70588,1.0);
	closeButton->mouseUpColor = CVS_ColorRGBA(1.0,1.0,1.0,1.0);
	closeButton->bitColor = CVS_ColorRGBA(1.0,1.0,1.0,1.0);
	closeButton->addBitmap("./bitmap/close.png");

	CVS_Button* minimizeButton = m_MainWindow->gui->addButton(50,0,50,30,Minimize, m_MainWindow->window);
	minimizeButton->onHoverColor = CVS_ColorRGBA(1.0,1.0,1.0,1.0);
	minimizeButton->mouseDownColor = CVS_ColorRGBA(1.0,0.0,0.0,1.0);
	minimizeButton->mouseUpColor = CVS_ColorRGBA(1.0,1.0,1.0,1.0);
	minimizeButton->offHoverColor = CVS_ColorRGBA(0.9,0.9,0.9,1.0);
	minimizeButton->addBitmap("./bitmap/minimize.png");

	m_MainWindow->gui->mainCell.Divide(true, 30);

	m_MainWindow->gui->mainCell.bar.active = false;

	m_MainWindow->gui->mainCell.cell1->debugColor = CVS_ColorRGBA(0.9,0.9,0.9,1.0f);

	m_MainWindow->gui->mainCell.cell2->Divide(true, 200);

	m_MainWindow->gui->mainCell.cell2->cell1->debugColor = CVS_ColorRGBA(0.85,0.85,0.85,1.0f);

	m_MainWindow->gui->mainCell.cell2->bar.active = false;

	m_MainWindow->gui->mainCell.cell2->cell2->Divide(false,200);

	m_MainWindow->gui->mainCell.cell2->cell2->setHandleBarMin(200);

	m_MainWindow->gui->mainCell.cell2->cell2->setHandleBarMax(300);

	CVS_Gui_SceneRenderer* renderer = new CVS_Gui_SceneRenderer(0,0, 400,400, NULL, m_MainWindow);
	m_MainWindow->gui->buttons.push_back(renderer);

	return true;
}

bool Editor::Run()
{
	while(!quit)
	{
		GLOBALSTATEMACHINE.m_WindowSub.Update();
	}
	return true;
}

bool Editor::End()
{
	return true;
}