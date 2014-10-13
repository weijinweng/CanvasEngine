#ifndef CVS_GUI
#define CVS_GUI

#include "CVS_Precompiled.h"

struct CVS_Window;
struct CVS_2DTools;

typedef unsigned int CVSButtonHandle;

//GUI rectangle
struct CVS_IRECT{
	int x, y, w, h;
};

struct CVS_FRECT{
	float x,y,w,h;
};

//Vector2 data
struct CVS_IVEC2{
	int x, y;
};

struct CVS_ColorRGBA{
	float r,g,b,a;
};

//Animation data, objects that can be animated inherits from this.
struct CVS_GUI_OBJ{
	CVS_IRECT rect;
	CVS_IVEC2 location;
	CVS_ColorRGBA color;
};

struct CVS_Button:public CVS_GUI_OBJ{
	CVS_Button(int x, int y, int w, int h);
	//GUI callback bundle
	void (*callBack)(void* databundle);
	void* bundle;
	bool mouseDown;
	bool getMouseDown(int x, int y);
	bool getMouseUp(int x, int y);
	void offHover();
	void onHover(int x, int y);
	void setOnClickFunction( void(*function)(void* bundle), void* data);
	void Render(CVS_2DTools* tools);
};

struct CVS_Frame{
	CVS_IRECT area;
	CVS_Button quit;
	CVS_Button maximize;
	CVS_Button minimize;
	CVS_Window* window;
	CVS_Frame(CVS_Window* window);
	void getMouseDown(int x, int y);
	void getMouseUp(int x, int y);
	void offHover();
	void onHover(int x, int y);
	void Render(CVS_2DTools* tools);

};

struct CVS_TabContent{
	std::vector<CVS_Button> buttons;
	void addButton(CVS_Button);
};

struct CVS_Tab{
	CVS_TabContent content;
	void getMouseDown(int x, int y);
	void getHover(int x, int y);
	void Render(CVS_2DTools* tools);
};

struct CVS_ToolBar{
	std::vector<CVS_Tab> tabs;
	void getMouseDown(int x, int y);
	void getHover(int x, int y);
	void Render(CVS_2DTools* tools);
};

struct CVS_Layout{
	CVS_ToolBar toolbar;
	CVS_ToolBar rightBar;
	CVS_ToolBar bottomBar;
	CVS_ToolBar leftBar;
	void getClicks(int x, int y);
	void getHover(int x, int y);
	void Render(CVS_2DTools* tools);
};

/*Cvs gui parent*/
struct CVS_Gui{
	std::vector<CVS_Button> buttons;
	//CVS_Frame frame;
	CVS_Window* window;
	CVS_Gui(CVS_Window* window);
	CVSButtonHandle addButton(int x, int y, int w, int h, void (function)(void* bundle) = NULL, void* bundle = NULL);
	void ParseInputs(SDL_Event e);
	void Update();
};

#endif