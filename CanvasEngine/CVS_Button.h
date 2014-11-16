#ifndef CVS_BUTTON_H
#define CVS_BUTTON_H
#include "CVS_GUISystem.h"

struct CVS_Button : public CVS_GUI_OBJ{
	HBITMAP m_Bitmap;
	std::string text;
	bool textbool;
	bool iconbool;
	bool hover;
	bool mousedown;
	CVS_Button(std::string data, CVS_GUI_CONTAINER* parent, int, int, int, int);
	CVS_Button(UINT, CVS_GUI_CONTAINER*, int, int, int, int);
	~CVS_Button();
	int ParseMsg(UINT, WPARAM, LPARAM);
	void OnHoverDraw();
	void MouseDownDraw();
	void OffHoverDraw();
	void UpdatePosition();
	static LRESULT CALLBACK ButtonProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
};

#endif