#include "OSImage.h"

using namespace Canvas;

Bitmap::Bitmap()
{

}

Bitmap::Bitmap(std::string filename)
{
#ifdef CVS_WIN32
	this->m_bitmap = (HBITMAP)LoadImage(NULL, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
#endif
}

void Bitmap::Load(std::string filename)
{
#ifdef CVS_WIN32
	this->m_bitmap = (HBITMAP)LoadImage(NULL, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
#endif
}

void Bitmap::Free()
{
#ifdef CVS_WIN32
	DeleteObject(m_bitmap);
#endif
}

