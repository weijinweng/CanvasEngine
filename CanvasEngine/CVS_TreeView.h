#ifndef CVS_TREEVIEW_H
#define CVS_TREEVIEW_H

#include "CVS_GUISystem.h"

typedef struct CVS_TreeNodeHandle
{
	int msg;
	HTREEITEM item;
	void* data;
} CVS_TREENODEHANDLE;

struct CVS_TreeView;

struct CVS_TreeViewContent{
	HWND TVhWnd;

	HIMAGELIST hImageList;
	int mOpenList;
	int mCloseList;
	int mStuffList;

	CVS_TreeView* mParent;

	std::vector<CVS_TREENODEHANDLE> mNodes;
	CVS_TreeViewContent(CVS_TreeView* parent, int, int, int, int);
	CVS_TREENODEHANDLE CreateNewNode(CVS_TREENODEHANDLE, CVS_TREENODEHANDLE,CVS_TREENODEHANDLE);
};

struct CVS_TreeView:public CVS_GUI_OBJ{
	CVS_TreeViewContent* mContent;
	CVS_TreeView(CVS_GUI_CONTAINER* container, int, int, int, int);

	int ParseMsg(UINT msg, WPARAM, LPARAM);
	static LRESULT CALLBACK TreeViewCallBack(HWND,UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
		
	CVS_TREENODEHANDLE CreateNewNode(CVS_TREENODEHANDLE, CVS_TREENODEHANDLE,CVS_TREENODEHANDLE);
	
};

#endif