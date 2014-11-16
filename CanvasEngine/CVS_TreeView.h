#ifndef CVS_TREEVIEW_H

#include "CVS_GUISystem.h"

typedef UINT CVS_TREENODEHANDLE;

struct CVS_TreeViewContent{


};

struct CVS_TreeView:public CVS_GUI_OBJ{
	CVS_TreeViewContent mContent;
	CVS_TreeView(CVS_GUI_CONTAINER* container, int, int, int, int);
	int ParseMsg(UINT msg, WPARAM, LPARAM);
	static LRESULT CALLBACK TreeViewCallBack(HWND, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
		
	CVS_TREENODEHANDLE CreateNewNode(CVS_TREENODEHANDLE, CVS_TREENODEHANDLE);
	
};

#endif