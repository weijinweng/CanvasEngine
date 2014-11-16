#ifndef CVS_GROUPBOX_H
#define CVS_GROUPBOX_H
#include "CVS_GUISystem.h"

struct CVS_GroupBox :public CVS_GUI_CONTAINER, public CVS_GUI_OBJ{
	std::string header;
	HBITMAP icon;
	int getWidth();
	int getHeight();
	CVS_Button* AddButton(std::string data, int, int, int, int);
	CVS_Button* AddButton(UINT, int, int, int, int);
	CVS_EditBox* AddEditBox(UINT, void*, int, int, int, int);
	void UpdatePosition();
};


#endif