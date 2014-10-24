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

bool Editor::Initialize()
{
	quit = false;
	if(!CVS_Initialized)
	{
		return false;
	}
	m_MainWindow = GLOBALSTATEMACHINE.m_WindowSub.createNewWindow("Canvas Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900);

	m_MainWindow->gui->addButton(0,700,200,200,boolToggle, &quit);

	m_MainWindow->gui->mainCell.Divide(true, 400);

	m_MainWindow->gui->mainCell.cell1->debugColor = RED;

	m_MainWindow->gui->mainCell.cell2->debugColor = BLUE;

	m_MainWindow->gui->mainCell.cell1->Divide(true, 100);

	m_MainWindow->gui->mainCell.cell1->cell1->debugColor = GREEN;

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