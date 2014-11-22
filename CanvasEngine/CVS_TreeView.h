#ifndef CVS_TREEVIEW_H
#define CVS_TREEVIEW_H

#include "CVS_GUISystem.h"

#define CVS_TV_NODEDBCLK 700
typedef struct CVS_TreeNodeHandle
{
	HTREEITEM item;
	void* data;
} CVS_TREENODEHANDLE ,* LPCVS_TREENODEHANDLE;

struct CVS_TreeView;

struct CVS_TreeViewContent{
	HWND TVhWnd;

	HIMAGELIST hImageList;
	int mOpenList;
	int mCloseList;
	int mStuffList;

	CVS_TreeView* mParent;
	UINT msg;

	int ParseMsg(UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK TreeViewProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

	std::vector<CVS_TREENODEHANDLE> mNodes;
	CVS_TreeViewContent(CVS_TreeView* parent, int, int, int, int);
	void SetMsg(UINT msg = RENDER_SELECTION);
	LPCVS_TREENODEHANDLE CreateNewNode(LPCVS_TREENODEHANDLE, 
		LPCVS_TREENODEHANDLE,
		std::string name,
		void* data);
};

struct CVS_TreeView:public CVS_GUI_OBJ{
	CVS_TreeViewContent* mContent;
	CVS_TreeView(CVS_GUI_CONTAINER* container, int, int, int, int);

	int ParseMsg(UINT msg, WPARAM, LPARAM);
	static LRESULT CALLBACK TreeViewCallBack(HWND,UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
		
	CVS_TREENODEHANDLE CreateNewNode(CVS_TREENODEHANDLE, CVS_TREENODEHANDLE,CVS_TREENODEHANDLE);
	
};

#endif