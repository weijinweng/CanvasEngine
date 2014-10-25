#ifndef CVS_2DTOOLS
#define CVS_2DTOOLS
#include "CVS_RenderProgram.h"
#include "CVS_GUI.h"

struct CVS_RenderProgram;
struct CVS_Window;
struct CVS_VertexObject;

struct CVS_2DTools{
	CVS_RenderProgram* drawingProgram;
	CVS_RenderProgram* textProgram;
	CVS_RenderProgram* bitProgram;
	CVS_VertexObject* VAO;
	CVS_Window* window;
	TTF_Font* font;
	CVS_2DTools(CVS_Window* window);
	int rectLoc;
	int colorLoc;
	int bitColorLoc;
	int bitTextureLoc;
	int textureLoc;
	//Convert a IRECT specified in pixels to OpenGL screen coords
	CVS_FRECT ConvertToScreenCoords(CVS_IRECT rectangle);
	void drawRect(CVS_ColorRGBA color, float x, float y, float w, float h);
	void drawTextureRect(CVS_Texture2D* texture, float x, float y, float w, float h);
	void drawText(std::string text, float x, float y, float w, float h);
	void drawBit(CVS_Texture2D* texture, CVS_ColorRGBA color, float x, float y, float w, float h);
};

#endif