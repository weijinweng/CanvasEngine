#ifndef CVS_2DTOOLS
#define CVS_2DTOOLS
#include "CVS_RenderProgram.h"
#include "CVS_GUI.h"

struct CVS_RenderProgram;
struct CVS_Window;

struct CVS_2DTools{
	CVS_RenderProgram* drawingProgram;
	GLuint VAO;
	CVS_Window* window;
	CVS_2DTools(CVS_Window* window);
	CVS_FRECT ConvertToScreenCoords(CVS_IRECT rectangle);
	void drawRect(CVS_ColorRGBA color, float x, float y, float w, float h);
	void drawTextureRect(CVS_Texture2D* texture, float x, float y, float w, float h);
};

#endif