#ifndef CVS_GUI
#define CVS_GUI

#include "CVS_Precompiled.h"

struct CVS_Window;
struct CVS_2DTools;
struct CVS_Gui;
struct CVS_Tab;
struct CVS_GUI_OBJECT;

typedef unsigned int CVSButtonHandle;
typedef unsigned short CVSFontHandle;

//GUI rectangle
struct CVS_IRECT{
	int x, y, w, h;
};

struct CVS_ARECT{
	int left, right, top, bottom;
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

/*//Animation data, objects that can be animated inherits from this.
struct CVS_GUI_OBJ{
	CVS_IRECT rect;
	CVS_ColorRGBA color;
	CVSFontHandle font;
};

struct CVS_TextNode{
	std::string text;
	CVSFontHandle font;
	CVS_IVEC2 size;
	CVS_TextNode(CVSFontHandle font);
	bool addChar(char letter);
	bool setText(std::string text);
	void renderText(int x, int y);
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
*/

struct CVS_Button;

struct CVS_LAYOUT_OBJ{
	virtual void SetSize(int,int,int,int) = 0 ;
};


struct CVS_GUI_CONTAINER{
	std::vector<CVS_Button* > buttons;
	int type;
	virtual HWND getHWND() = 0;
	virtual CVS_Window* getWindow() = 0;
	virtual CVS_Button* AddButton(std::string data, int x, int y, int w, int h) = 0;
	virtual CVS_Button* AddButton(UINT, int x, int y, int w, int h) = 0;
};


struct CVS_GUI_OBJ{
	CVS_ARECT m_Rect;
	HWND hWnd;
	CVS_GUI_CONTAINER* container;
	CVS_Window* parent;
	virtual int ParseMsg(UINT msg, WPARAM, LPARAM) = 0;
	virtual int getWidth();
	virtual int getHeight();
};

struct CVS_SceneView;
//Render forward declaration
struct CVS_Renderer;
struct CVS_RenderScene;
struct CVS_Camera;
struct CVS_Scene;
struct CVS_Texture;
struct CVS_FrameBuffer;
struct CVS_View;

struct SelectData{
	float ObjectID;
	float DrawID;
	float PrimID;
};
//Selection object for 3D picking
struct CVS_Selection{
	CVS_SceneView* parent;

	CVS_FrameBuffer* mRenderBuffer;
	CVS_Texture* mSelectionTexture;
	CVS_Texture* mDepthTexture;

	int MVPLoc;
	int DrawIndexLoc;
	int ObjectIndexLoc;
	
	CVS_RenderProgram* mRenderProgram;
	
	CVS_Selection(CVS_SceneView* view);
	bool Initializ();
	SelectData getPrimitiveID(int, int);
	void Render(CVS_RenderScene*, CVS_View*);
	void UpdateSize();
};

struct CVS_SceneView:public CVS_GUI_OBJ, CVS_LAYOUT_OBJ{
	bool mMouseDown;
	bool mMiddleMouse;
	bool mHover;
	int mouseX;
	int mouseY;

	CVS_Timer mTimer;
	CVS_RenderScene* Scene;
	CVS_Camera* Cam;
	static GLuint m_GridProgram;
	HGLRC m_glContext;
	HDC glHdc;
	CVS_Renderer* m_Renderer;
	CVS_Selection* mSelection;
	CVS_SceneView(CVS_Gui* gui, int x, int y, int w, int h);
	//void SetScene(CVS_Scene* scene);
	int ParseMsg(UINT msg, WPARAM, LPARAM);
	static LRESULT CALLBACK SceneViewCallback(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	void SetScene(CVS_Scene* scene);
	void Render();
	void SetSize(int,int,int,int);
};


struct CVS_Button: public CVS_GUI_OBJ{
	HBITMAP m_Bitmap;
	std::string text;
	bool textbool;
	bool iconbool;
	bool hover;
	bool mousedown;
	CVS_Button(std::string data, CVS_GUI_CONTAINER* parent, int,int,int,int);
	CVS_Button(UINT, CVS_GUI_CONTAINER*, int,int,int,int);
	int ParseMsg(UINT, WPARAM, LPARAM);
	void OnHoverDraw();
	void MouseDownDraw();
	void OffHoverDraw();
	void UpdatePosition();
	static LRESULT CALLBACK ButtonProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
};

struct CVS_TabContent:public CVS_GUI_CONTAINER{
	CVS_ARECT m_Rect;
	HWND hWnd;
	CVS_Tab* parent;
	int ParseMsg(HWND,UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK TabContProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	CVS_TabContent(CVS_Tab* parent);
	void OnResize();
	HWND getHWND();
	CVS_Button* AddButton(std::string data, int x, int y, int w, int h);
	CVS_Button* AddButton(UINT,int,int,int,int);
	CVS_Window* getWindow();
};

struct CVS_TabSlot{
	HWND hWnd;
	CVS_ARECT m_Rect;
	bool m_Selected;
	std::string data;
	CVS_TabContent* content;
	CVS_Tab* parent;
	static LRESULT CALLBACK TabSlotProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	CVS_TabSlot(std::string name, CVS_Tab* tab);
	int ParseMsg(UINT,WPARAM,LPARAM);
	void HideContent();
	void ShowContent();
	void UpdatePos();
	void Repaint();
};

struct CVS_Tab:public CVS_GUI_OBJ, public CVS_LAYOUT_OBJ{
	static HFONT font;
	RECT m_TabMargins;
	std::vector<CVS_TabSlot*> m_Slots;
	CVS_Tab(CVS_Window* window, int x, int  y, int w, int h);
	void getMouseDown(int x, int y);
	void getHover(int x, int y);
	void Render(CVS_2DTools* tools);
	int ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	bool CalibrateLocation();

	void SetSize(int,int,int,int);
	void HideAllContent(CVS_TabSlot* slot);

	static LRESULT CALLBACK CVS_TABProc(HWND, UINT msg, WPARAM, LPARAM);
	//Need to add more stuff
	void addTab(std::string tab, int index);
};

struct CVS_ToolBar:public CVS_GUI_OBJ, public CVS_GUI_CONTAINER, public CVS_LAYOUT_OBJ{
	std::vector<CVS_Tab> tabs;
	CVS_ToolBar(CVS_Gui* parent, int x, int y, int w, int h);
	int ParseMsg(UINT, WPARAM, LPARAM);
	HWND getHWND();
	static LRESULT CALLBACK ToolBarCallBack(HWND,UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	bool CalibrateLocation();
	CVS_Button* AddButton(std::string data, int x, int y, int w, int h);
	CVS_Button* AddButton(UINT bitmapID, int x, int y, int w, int h);
	CVS_Window* getWindow();
	void SetSize(int x, int y, int w, int h);
	//Creates a new line of tab
	void AddLine();
};

struct CVS_Layout;

struct CVS_SRECT{
	int x, y, w, h;
};



struct CVS_Layout{

	CVS_Window* window;
	virtual void onResize();
};

#define CVS_WLEFT 4
#define CVS_WRIGHT 5
#define CVS_WTOP 6
#define CVS_WBOTTOM 7
#define CVS_LLEFT 0
#define CVS_LRIGHT 1
#define CVS_LTOP 2
#define CVS_LBOTTOM 3
#define CVS_LEFT 8
#define CVS_RIGHT 9
#define CVS_TOP 10
#define CVS_BOTTOM 11

struct CVS_GUI_CELL{



	bool m_Divided;
	RECT m_Rect;
	RECT m_MaxRect;
	RECT m_MinRect;
	float m_Weight;
	CVS_LAYOUT_OBJ* obj;
	CVS_GUI_CELL(CVS_Layout* layout);
	void SetSize(int,int,int,int);
	void onResize();
	RECT GetCellRect();
	void SetPosition(UINT type, CVS_GUI_CELL* pos = NULL, int value = 0);
	void SetObj(CVS_LAYOUT_OBJ* obj);
};

struct CVS_EditorLayout:public CVS_Layout{
	enum CVS_ED_LAYOUT{
		LEFT,
		RIGHT,
		TOP,
		MIDDLE
	};
	CVS_SceneView* view;
	CVS_GUI_CELL toolbarCell;
	CVS_GUI_CELL rightbarCell;
	CVS_GUI_CELL leftbarCell;
	CVS_GUI_CELL bottomCell;
	CVS_GUI_CELL renderCell;
	//Creates an editor gui;
	//All commands are sent to he main window
	CVS_EditorLayout(CVS_Gui* parent);
	CVS_Button* AddButton(std::string,int,int,int,int,UINT ID);
	CVS_Tab* GetTab();
	CVS_ToolBar* getToolBar();
	void onResize();
	void setScene(CVS_Scene* scene);
};

struct CVS_Menu{
	int type;
	HMENU hMenu;
};

/*Cvs gui parent*/
struct CVS_Gui{
	std::vector<CVS_Tab*> objects;
	//CVS_Frame frame;
	CVS_Window* window;
	CVS_Layout* Layout;
	CVS_Gui(CVS_Window* window);
	void DrawTab(WPARAM, LPARAM);
	CVSButtonHandle addButton(int x, int y, int w, int h, void (function)(void* bundle) = NULL, void* bundle = NULL);
	void Update();
};

struct CVS_GuiManager:public CVS_SubSystem{
	std::vector<CVS_Tab*> tabctrls;
	std::vector<CVS_TabContent*> tabconts;
	static LRESULT CALLBACK GuiCallback(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	bool Initialize();
	bool Update();
	bool End();
};

#endif