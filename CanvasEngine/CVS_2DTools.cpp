#include "CVS_2Dtools.h"
#include "CVS_WindowSystem.h"
#include "Canvas.h"

extern CVS_StateMachine GLOBALSTATEMACHINE;

CVS_2DTools::CVS_2DTools(CVS_Window* window):window(window)
{
	this->drawingProgram = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("2D_Color_Shader", "./shaders/2D.vert", "./shaders/2Drect.frag");

	CVS_Buffer vertexBuffer;

}

CVS_FRECT CVS_2DTools::ConvertToScreenCoords(CVS_IRECT rect)
{
	CVS_FRECT rectangle = {0,0,0,0};
	float screenWidth = ((float)window->width);
	float screenHeight = ((float)window->width);
	rectangle.x = (2.0f * ( (float)rect.x/screenWidth)) - 0.5f;
	rectangle.y = (2.0f * ( (float)rect.y/screenHeight)) - 0.5f;
	rectangle.w /= ((float)window->width) * (2.0f * ( (float)rect.x/screenWidth));
	rectangle.h /= ((float)window->height) * (2.0f * ( (float)rect.y/screenHeight));
	return rectangle;
}

void CVS_2DTools::drawRect(CVS_ColorRGBA color, float x, float y, float w, float h)
{
	
}