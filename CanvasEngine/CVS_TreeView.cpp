#include "Canvas.h"
#include "resource.h"

CVS_TreeViewContent::CVS_TreeViewContent(CVS_TreeView* _Parent, int x, int y, int w, int h)
{
	InitCommonControls();

	TVhWnd = CreateWindowEx(0, WC_TREEVIEW, "TREEEESSSSS", WS_VISIBLE | WS_CHILD | TVS_HASLINES, 10, 10, w - 20, h - 20, _Parent->hWnd, NULL, CVS_AppInstance, NULL);

	if (!(hImageList = ImageList_Create(10, 10, ILC_COLOR24, 2, 10)))
		printf("Error ! Unable to init imagelist\n");

	HBITMAP CollapseBit = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(CVS_TV_COLLAPSE));
	
	mCloseList = ImageList_Add(hImageList, CollapseBit, (HBITMAP)NULL);
	DeleteObject(CollapseBit);

	HBITMAP OpenBit = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(CVS_TV_EXPAND));
	mOpenList = ImageList_Add(hImageList, OpenBit, (HBITMAP)NULL);
	DeleteObject(OpenBit);

	HBITMAP StuffBit = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(CVS_TV_EXPAND));
	mStuffList = ImageList_Add(hImageList, StuffBit, (HBITMAP)NULL);
	DeleteObject(StuffBit);

	if (ImageList_GetImageCount(hImageList) < 2)
	{
		printf("Error, image list not added\n");
	}


}

CVS_TREENODEHANDLE CVS_TreeViewContent::CreateNewNode(CVS_TREENODEHANDLE p, CVS_TREENODEHANDLE n1, CVS_TREENODEHANDLE n2)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST;
	static HTREEITEM hPrevRootItem = NULL;
	static HTREEITEM hPrevLev2Item = NULL;
	HTREEITEM hti;

	

	static int nLevel = 0;
	nLevel++;

	tvi.mask = TVIF_TEXT |TVIF_PARAM;

	tvi.pszText = "Node Lol";

	tvi.lParam = (LPARAM)1;
	tvins.item = tvi;
	tvins.hInsertAfter = hPrev;

	if (nLevel == 1)
		tvins.hParent = TVI_ROOT;
	else if (nLevel == 2)
		tvins.hParent = hPrevRootItem;
	else
		tvins.hParent = hPrevLev2Item;

	hPrev = (HTREEITEM)SendMessage(TVhWnd, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (hPrev == NULL)
		return NULL;
	
	if (nLevel == 1)
		hPrevRootItem = hPrev;
	else if (nLevel == 2)
		hPrevLev2Item = hPrev;



	return NULL;
}

CVS_TreeView::CVS_TreeView(CVS_GUI_CONTAINER* container, int x, int y, int w, int h) 
{
	hWnd = CreateWindow(WC_STATIC, "Hi", WS_CHILD | WS_VISIBLE, x, y, w, h, container->getHWND(), NULL, CVS_AppInstance, NULL);

	mContent = new CVS_TreeViewContent(this, x, y, w, h);
}

int CVS_TreeView::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return -1;
}

LRESULT CALLBACK CVS_TreeView::TreeViewCallBack(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}