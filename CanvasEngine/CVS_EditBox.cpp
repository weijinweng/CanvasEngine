#include "Canvas.h"

CVS_EditBox::CVS_EditBox(CVS_GUI_CONTAINER* _Parent, UINT _Type, void* _dataPtr, int x, int y, int w, int h) :dataPtr(NULL)
{
	dataType = _Type;

	hWnd = CreateWindow(WC_EDIT, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE , x, y, w, h, _Parent->getHWND(), NULL, CVS_AppInstance, NULL);
	
	SetWindowSubclass(hWnd, EditBoxCallback, 0, (DWORD_PTR)this);
	
	HDC hdc = GetDC(hWnd);

	SetBkColor(hdc, CVS_BLUE);

	ReleaseDC(hWnd, hdc);

	m_Rect.left = x;
	m_Rect.right = x + w;
	m_Rect.top = y;
	m_Rect.bottom = y + h;
	FinalizeInput();
}

int CVS_EditBox::ProcessChar(WPARAM wParam)
{
	char input = wParam; 
	printf("input\n");
	if (wParam == VK_RETURN)
	{
		FinalizeInput();
		return -1;
	}

	if (wParam == VK_RETURN || wParam == VK_BACK)
	{
		return -1;
	}

	switch (this->dataType)
	{
	case CVS_INT:
		if (input >= '0' && input <= '9')
		{
			printf("the shit is happening\n", input);
			return -1;
		}
		break;
	case CVS_FLOAT:
		if ((input >= '0' && input <= '9') || input == '.')
		{

			if (input == '.')
			{
				char buffer[1024];
				GetWindowText(hWnd, buffer, 1024);
				data = std::string(buffer);
				printf("wtf bitch\n", data.c_str());
				if (count(data.begin(), data.end(), '.') > 0)
				{
					return 0;
				}
			}
			return -1;
		}
		break;
	}
	printf("Carh\n");
	return 0;
}

void CVS_EditBox::FinalizeInput()
{
	char buffer[1024];

	GetWindowText(hWnd, buffer, 1024);

	data = std::string(buffer);

	switch (dataType)
	{
	case CVS_INT:
		if (data == "")
		{
			printf("The fuck\n");
			data = "0";
			if (data.length() == 0)
				printf("Okay?\n");
		}
	
		break;
	case CVS_FLOAT:
		if (data == "")
		{
	
			data = "0.0";
		}
		printf("wtf bitch\n", data.c_str());
		if (count(data.begin(), data.end(), '.') > 1)
		{
			data = "0.0";
		}

	}
	if (dataPtr != NULL)
	{
		switch (dataType)
		{
		case CVS_INT:
			if (dataPtr != NULL)
				*(int*)dataPtr = std::stoi(data);
			break;
		case CVS_FLOAT:
			*(float*)dataPtr = std::stof(data);
		}
	}

	printf("Finalize\n");
	SetWindowText(hWnd, data.c_str());
}

int CVS_EditBox::ParseMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_NCACTIVATE:
		printf("lol\n");
		break;
	case WM_KILLFOCUS:
		FinalizeInput();
		break;
	case WM_ACTIVATE:
		if (WA_INACTIVE == wParam)
		{
			FinalizeInput();
		}
		break;
	case WM_CHAR:
		return ProcessChar(wParam);	
	}
	return -1;
}

void CVS_EditBox::UpdatePosition()
{

}

LRESULT CALLBACK CVS_EditBox::EditBoxCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR pointer)
{
	int result = ((CVS_EditBox*)pointer)->ParseMsg(msg, wParam, lParam);
	if (result != -1)
		return result;
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}