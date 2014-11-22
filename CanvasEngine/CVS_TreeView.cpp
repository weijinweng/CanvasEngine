#include "Canvas.h"
#include "resource.h"

CVS_TreeViewContent::CVS_TreeViewContent(CVS_TreeView* _Parent, int x, int y, int w, int h)
{
	InitCommonControls();

	TVhWnd = CreateWindowEx(0, WC_TREEVIEW, "TREEEESSSSS", WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_SINGLEEXPAND, 10, 10, w - 20, h - 20, _Parent->hWnd, NULL, CVS_AppInstance, NULL);

	if (!(hImageList = ImageList_Create(10, 10, ILC_COLOR24, 2, 10)))
		printf("Error ! Unable to init imagelist\n");

	HBITMAP CollapseBit = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(CVS_TV_COLLAPSE));
	
	mCloseList = ImageList_Add(hImageList, CollapseBit, (HBITMAP)NULL);
	DeleteObject(CollapseBit);

	HBITMAP OpenBit = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(CVS_TV_EXPAND));
	mOpenList = ImageList_Add(hImageList, OpenBit, (HBITMAP)NULL);
	DeleteObject(OpenBit);

	HBITMAP StuffBit = LoadBitmap(CVS_AppInstance, MAKEINTRESOURCE(CVS_TV_STUFF));
	mStuffList = ImageList_Add(hImageList, StuffBit, (HBITMAP)NULL);
	DeleteObject(StuffBit);

	if (ImageList_GetImageCount(hImageList) < 3)
	{
		printf("Error, image list not added\n");
	}

	TreeView_SetImageList(TVhWnd, hImageList, TVSIL_NORMAL);
	SetMsg();
}

void CVS_TreeViewContent::SetMsg(UINT msg)
{
	this->msg = msg;
}

LPCVS_TREENODEHANDLE CVS_TreeViewContent::CreateNewNode(LPCVS_TREENODEHANDLE p, 
	LPCVS_TREENODEHANDLE n1,
	std::string name, 
	void* data)
{
	TVITEM tvi;

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvi.iImage = mStuffList;
	tvi.iSelectedImage = mStuffList;
	tvi.pszText = const_cast<char*>( name.c_str());
	tvi.cchTextMax = name.length();
	tvi.lParam = (LPARAM) data;
	
	TVINSERTSTRUCT tvis;

	tvis.item = tvi;
	tvis.hInsertAfter = n1 != NULL ? n1->item : TVI_SORT;
	tvis.hParent = p != NULL ? p->item : TVI_ROOT;
	
	LPCVS_TREENODEHANDLE handle = new CVS_TREENODEHANDLE();

	handle->item = TreeView_InsertItem(TVhWnd, &tvis);
	handle->data = data;

	if (p != NULL)
	{
		tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvi.hItem = p->item;
		tvi.iImage = mOpenList;
		tvi.iSelectedImage = mCloseList;
		TreeView_SetItem(TVhWnd, &tvi);
	}

	SetWindowSubclass(TVhWnd, CVS_TreeViewContent::TreeViewProc, 8, (DWORD_PTR)this);

	return handle;
}

int CVS_TreeViewContent::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case NM_CLICK:

		return 0;
	}
	return -1;
}

LRESULT CALLBACK CVS_TreeViewContent::TreeViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	int result = ((CVS_TreeViewContent*)pointer)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
		return result;
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}


CVS_TreeView::CVS_TreeView(CVS_GUI_CONTAINER* container, int x, int y, int w, int h) 
{
	hWnd = CreateWindow(WC_STATIC, "", WS_CHILD | WS_VISIBLE, x, y, w, h, container->getHWND(), NULL, CVS_AppInstance, NULL);

	SetWindowSubclass(hWnd, CVS_TreeView::TreeViewCallBack, 7, (DWORD_PTR) this);

	HDC hdc = GetDC(hWnd);

	SetBkColor(hdc, CVS_DARK_GREY);

	ReleaseDC(hWnd, hdc);

	RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);

	mContent = new CVS_TreeViewContent(this, x, y, w, h);
}

void InitTreeViewScene(CVS_TreeViewContent* content, LPCVS_TREENODEHANDLE parentHandle, CVS_GameObject* obj)
{
	LPCVS_TREENODEHANDLE handle = content->CreateNewNode(parentHandle, NULL, obj->name, obj->getComponent("RenderComponent"));
	for (int i = 0; i < obj->children.size(); ++i)
	{
		InitTreeViewScene(content,  handle, obj->children[i]);
	}
}

int CVS_TreeView::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case SCENE_LOAD:
	{
					   CVS_Scene* scene = (CVS_Scene*)lParam;
					   for (int i = 0; i < scene->objects.size(); ++i)
					   {
						   InitTreeViewScene(this->mContent, NULL, scene->objects[i]);
					   }
					   break;
	}
	case WM_NOTIFY:
	{
					  LPNMTREEVIEW lptv = (LPNMTREEVIEW)lParam;
					  switch (((LPNMHDR)lParam)->code)
					  {

					  case TVN_ITEMEXPANDING:

						  if (lptv->action == TVE_EXPAND)
						  {
							  TVITEM tvi;
							  lptv->itemNew.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
							  lptv->itemNew.iImage = mContent->mCloseList;
							  lptv->itemNew.iSelectedImage = mContent->mCloseList;
							  TreeView_SetItem(((LPNMHDR)lParam)->hwndFrom, &lptv->itemNew);
						  }
						  else if (lptv->action == TVE_COLLAPSE)
						  {
							  TVITEM tvi;
							  lptv->itemNew.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
							  lptv->itemNew.iImage = mContent->mOpenList;
							  lptv->itemNew.iSelectedImage = mContent->mOpenList;
							  TreeView_SetItem(((LPNMHDR)lParam)->hwndFrom, &lptv->itemNew);
						  }
						  break;
					  case TVN_SELCHANGED:
						  printf("Stuff changed %d\n", (int)lptv->itemNew.lParam);
						  ((Editor*)GLOBALSTATEMACHINE.m_App)->Message(RENDER_SELECTION, 0, (LONG_PTR)lptv->itemNew.lParam);
						  break;
					  }
	}
		return -1;
	case WM_PAINT:
		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		return 0;

	}
	return -1;
}

LRESULT CALLBACK CVS_TreeView::TreeViewCallBack(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	int result = ((CVS_TreeView*)pointer)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
		return result;
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}