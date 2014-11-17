#ifndef CVS_SCENEVIEW_H
#define CVS_SCENEVIEW_H
#include "CVS_GUISystem.h"

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

struct CVS_SceneView :public CVS_GUI_OBJ, CVS_LAYOUT_OBJ{
	bool mMouseDown;
	bool mMiddleMouse;
	bool mHover;
	int mouseX;
	int mouseY;
	bool mFpsMode;

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
	void SetSize(int, int, int, int);
};


#endif