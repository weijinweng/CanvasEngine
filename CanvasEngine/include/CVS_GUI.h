#ifndef CVS_GUI
#define CVS_GUI

#include "CVS_Precompiled.h"

struct CVS_Scene;

struct CVS_RenderScene;
struct CVS_Texture2D;

struct CVS_Window;
struct CVS_2DTools;

struct CVS_Gui_Cell;
struct CVS_Gui;

typedef unsigned int CVSButtonHandle;
typedef unsigned short CVSFontHandle;

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
	CVS_ColorRGBA();
	CVS_ColorRGBA(float r, float g, float b, float a);
	void set(float r = -1, float g = -1, float b = -1, float a = -1);
};


//Predefined colors
const CVS_ColorRGBA RED(1.0,0.0,0.0,1.0f);
const CVS_ColorRGBA BLUE(0.0,0.0,1.0,1.0);
const CVS_ColorRGBA GREEN(0.0,1.0,0.0,1.0);
const CVS_ColorRGBA BLACK(0.0,0.0,0.0,1.0);
const CVS_ColorRGBA WHITE(1.0,1.0,1.0,1.0);



//Animation data, objects that can be animated inherits from this.
struct CVS_GUI_OBJ{
	CVS_IRECT rect;
	CVS_ColorRGBA color;
	CVS_ColorRGBA mouseDownColor;
	CVS_ColorRGBA mouseUpColor;
	CVS_ColorRGBA onHoverColor;
	CVS_ColorRGBA offHoverColor;
	CVSFontHandle font;
	CVS_Enum posType;
	CVS_Texture2D* texture;
	bool bit;
	CVS_ColorRGBA bitColor;
	CVS_GUI_OBJ();
	virtual void Render(CVS_2DTools* tools) = 0;
	virtual bool getMouseDown(int x, int y) = 0;
	virtual bool getMouseUp(int x, int y) = 0;
	virtual void offHover() = 0;
	virtual void onHover(int x, int y) = 0;
	virtual void onResize();
	void addBitmap(char* filePath);
};

struct CVS_TextNode{
	std::string text;
	TTF_Font* font;
	CVS_IVEC2 size;
	CVS_Texture2D* texture;
	bool updated;
	CVS_TextNode(CVSFontHandle font);
	bool addChar(char letter);
	bool setText(std::string text);
	void renderText(int x, int y, CVS_2DTools* tools);
};

struct CVS_Button:public CVS_GUI_OBJ{
	std::vector<CVS_TextNode> text;
	CVS_Button(int x, int y, int w, int h);
	//GUI callback bundle
	void (*callBack)(void* databundle);
	void* bundle;
	bool mouseDown;
	void setText(std::string text);
	bool getMouseDown(int x, int y);
	bool getMouseUp(int x, int y);
	void offHover();
	void onHover(int x, int y);
	void setOnClickFunction( void(*function)(void* bundle), void* data);
	void Render(CVS_2DTools* tools);
};

struct CVS_Slider:public CVS_GUI_OBJ{
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
	std::vector<CVS_GUI_OBJ*> buttons;
};

struct CVS_Tab{
	CVS_TabContent content;
	void getMouseDown(int x, int y);
	void getHover(int x, int y);
	void Render(CVS_2DTools* tools);
	CVSButtonHandle addButton(int x, int y, int w, int h, void (function)(void* bundle) = NULL, void* bundle = NULL);
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

struct handleBar{
	CVS_Gui_Cell* parent;
	CVS_IRECT rect;
	CVS_ColorRGBA color;
	int mouseX, mouseY;
	bool horizontal;
	bool initialized;
	bool active;
	bool mouseDown;
	int value;
	int max;
	int min;
	int* v1, *v2;
	handleBar(CVS_Gui_Cell* parent);
	void initialize(bool horizontal, int location);
	bool getMouseDown(int x, int y);
	bool getMouseUp(int x, int y);
	bool onHover(int x, int y);
	void offHover();
	void sync();
	void Render(CVS_2DTools* tools);
};

struct CVS_Gui_Cell:public CVS_GUI_OBJ{
	CVS_ColorRGBA debugColor;
	//For debuggined scenes;

	std::vector<CVS_GUI_OBJ*> children;
	CVS_Gui* gui;
	CVS_Gui_Cell* parent;
	handleBar bar;
	CVS_Gui_Cell* cell1;
	CVS_Gui_Cell* cell2;	
	CVS_Gui_Cell(CVS_Gui_Cell* parent);
	CVS_Gui_Cell(CVS_Gui* gui);
	virtual void Render(CVS_2DTools* tools);
	virtual bool getMouseDown(int x, int y);
	virtual bool getMouseUp(int x, int y);
	virtual void offHover();
	virtual void onHover(int x, int y);
	void setHandleBarMin(int min);
	void setHandleBarMax(int max);
	void onResize();
	void Divide(bool horizontal, int loc);
	void UpdatePos();
	CVSButtonHandle addButton(int x, int y, int w, int h, void (function)(void* bundle) = NULL, void* bundle = NULL);
};

struct CVS_Gui_SceneRenderer:public CVS_GUI_OBJ{
	CVS_RenderScene* scene;
	CVS_Window* parent;
	CVS_Gui_SceneRenderer(int x, int y, int w, int h, CVS_RenderScene* scene, CVS_Window* parent);
	virtual void Render(CVS_2DTools* tools);
	virtual bool getMouseDown(int x, int y);
	virtual bool getMouseUp(int x, int y);
	virtual void offHover();
	virtual void onHover(int x, int y);
};


/*Cvs gui parent*/
struct CVS_Gui{
	CVS_Scene* Scene;
	std::vector<CVS_GUI_OBJ*> buttons;
	CVS_Gui_Cell mainCell;
	//CVS_Frame frame;
	CVS_Window* window;
	CVS_Gui(CVS_Window* window);
	CVS_Button* addButton(int x, int y, int w, int h, void (function)(void* bundle) = NULL, void* bundle = NULL);
	void ParseInputs(SDL_Event e);
	void Update();
	void setScene(CVS_Scene* scene);
};

#endif