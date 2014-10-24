#include "CVS_2Dtools.h"
#include "CVS_WindowSystem.h"
#include "Canvas.h"


CVS_2DTools::CVS_2DTools(CVS_Window* window):window(window)
{
	
	this->textProgram = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("2D_Texture", "./shaders/2DDrawTools/2D.vert", "./shaders/2DDrawTools/2Dtexture.frag");
	this->drawingProgram = GLOBALSTATEMACHINE.m_RenderSub.createNewShader("2D_Color_Shader", "./shaders/2DDrawTools/2D.vert", "./shaders/2DDrawTools/2D.frag");

	rectLoc = drawingProgram->getUniformHash("rect");
	colorLoc = drawingProgram->getUniformHash("color");
	textureLoc = textProgram->getUniformHash("myTexture");

	//Rectangle vertex data
	float vertexData[] = {
		0.0f, 0.0f,
		0.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 0.0f
	};

	/*float vertexData[] = {
		-1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};*/

	CVS_Buffer vertexBuffer(CVS_ARRAY_BUFFER);

	vertexBuffer.BufferData(8*sizeof(float), vertexData);

	//Prepare our VAOS for rectangle drawing.
	VAO = GLOBALSTATEMACHINE.m_RenderSub.createNewVertexObject();

	VAO->bindVAO();

	VAO->bindArrayBuffer(&vertexBuffer, 0, 2);

	CVS_Buffer uvBuffer(CVS_ARRAY_BUFFER);

	float uvData[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	unsigned int indiceData[] = {
		0,1,2,0,2,3
	};

	uvBuffer.BufferData(8*sizeof(float), uvData);

	VAO->bindArrayBuffer(&uvBuffer, 1, 2);

	VAO->bindElementBuffer(sizeof(unsigned int)*6, indiceData, CVS_STATIC_DRAW);

	VAO->unBind();
}

CVS_FRECT CVS_2DTools::ConvertToScreenCoords(CVS_IRECT rect)
{
	CVS_FRECT rectangle = {0,0,0,0};
	float screenWidth = ((float)window->width);
	float screenHeight = ((float)window->height);
	rectangle.x = (2.0f * ( (float)rect.x/screenWidth)) - 1.0f;
	rectangle.y = (2.0f * ( (float)rect.y/screenHeight)) - 1.0f;
	rectangle.w = 2.0f*((float)rect.w/(float)window->width);
	rectangle.h = 2.0f*((float)rect.h/(float)window->height);
	return rectangle;
}

void CVS_2DTools::drawRect(CVS_ColorRGBA color, float x, float y, float w, float h)
{
	drawingProgram->setAsCurrentProgram();
	//Instantiate as glm vec to garantee memory contiguous
	cvec4 rect(x,y,w,h);
	//Bind data
	drawingProgram->bindVec4v(rectLoc, glm::value_ptr(rect));
	cvec4 vcolor(color.r,color.g, color.b, color.a);
	drawingProgram->bindVec4v(colorLoc, glm::value_ptr(vcolor));
	VAO->drawElements(CVS_TRIANGLES, 6);
}

void CVS_2DTools::drawTextureRect(CVS_Texture2D* texture, float x, float y, float w, float h)
{
	textProgram->setAsCurrentProgram();

	texture->setActive(0);
	texture->bindToLocation(textureLoc);
}