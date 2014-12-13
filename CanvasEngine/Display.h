#pragma once
#include "Properties.h"

namespace Canvas{

	extern Color_RGB Black;
	extern Color_RGB White;
	extern Color_RGB Grey;
	extern Color_RGB Blue;
	extern Color_RGB Green;
	extern Color_RGB Red;
	extern Color_RGB CVS_DarkPurple;
	extern Color_RGB CVS_LightPurple;
	extern Color_RGB CVS_Purple;
	extern Color_RGB CVS_DarkBlue;
	extern Color_RGB CVS_DarkGrey;
	extern Color_RGB CVS_Blue;
	extern Color_RGB CVS_LightBlue;
	extern Color_RGB CVS_Red;

	enum DCEnum{
		CVS_DC_BK_TRANSPARENT,
		CVS_DC_DK_OPAQUE
	};

	//DisplayContext for drawing.
	struct DisplayContext{
	private:
		std::vector<Brush*> prevBrushes;
		std::vector<Pen*> prevPens;
		Brush* m_oldbrush;
		Pen* m_oldpen;
		Brush* m_currentbrush;
		Pen* m_currentpen;
		bool initialized;
	public:
#ifdef CVS_WIN32
		PAINTSTRUCT ps;
		bool paint;
		HWND hWnd;
		HDC hdc;

		DisplayContext(HWND);
		DisplayContext(HWND, PAINTSTRUCT);
		
		void SetHandle(HWND);
		void SetPaintHandle(HWND);
#endif
#ifdef CVS_MAC
#endif
#ifdef CVS_LINUX
#endif
		bool Init();
		void End();

		void SetFillColor(int, int, int);
		void SetLineColor(int, int, int);
		void SetFillColor(Color_RGB);
		void SetLineColor(Color_RGB);

		I_Rect getDisplayIRect();
		B_Rect getDisplayBRect();
		B_Rect getTextSize(std::string);
		
		void SetBrush(Brush*);
		void SetPen(Pen*);

		void DrawFillRect(int x, int y, int w, int h);
		void DrawRect(int x, int y, int w, int h);
		void DrawLine(int x, int y, int x2, int y2);
		void DrawUnstyledText(std::string text, int x, int y, int w, int h);
		void DrawStyledText(std::string text, int x, int y, int w, int h, uint32);

		void SetBKMode(uint32);
		void SetBKColor(Color_RGB);
		void SetFontColor(Color_RGB);

		bool Init3D();
		void UpdateBuffer();

		void BeginClipPath();
		void EndClipPath();
		
		void Free();
		void Restore();
		void Save();
		~DisplayContext();
	};
};
