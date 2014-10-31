#include "Canvas.h"

CVS_Tab::CVS_Tab(CVS_Window* parent, int x, int y, int w, int h)
{
	this->parent = parent;
	RECT rcClient;

	INITCOMMONCONTROLSEX icex;

	TCITEM tie;

	int i;

	TCHAR achTemp[256];

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);

	hWnd = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, x,y,w,h, parent->hWnd, NULL, CVS_AppInstance, NULL);

	if(hWnd == NULL)
	{
		MessageBox(parent->hWnd, "Error making tab!", "Error", MB_OK);
	}

}

void CVS_Tab::addTab(std::string name, int pos)
{
	TCITEM tcItem;


    tcItem.mask = TCIF_TEXT;
    tcItem.iImage = -1;

	tcItem.pszText = (LPSTR) name.c_str();


	int index = TabCtrl_InsertItem(hWnd, pos, &tcItem);

	if(index == -1)
	{
		printf("Error adding tab\n");
	}
}

/*Main gui controller*/
CVS_Gui::CVS_Gui(CVS_Window* parent):window(parent)
{


}

