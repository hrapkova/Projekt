// Library.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <functional>
#include "Library.h"
#include "../Application/Utils.h"


//// This is an example of an exported variable
//LIBRARY_API int nLibrary=0;
//
//// This is an example of an exported function.
//LIBRARY_API int fnLibrary(void)
//{
//    return 42;
//}
//
//// This is the constructor of a class that has been exported.
//// see Library.h for the class definition
//CLibrary::CLibrary()
//{
//    return;
//}

//	parse file names from file name string in OPENFILENAME struct
//	returns pair of directory and vector of filenames
//
LIBRARY_API std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile)
{
	return Utils::ParseFiles(lpstrFile);
}

LIBRARY_API void CalcHistogram(void* Scan0, int Stride, int start, int end, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool()> fnCancel)
{
	return Utils::CalcHistogram(Scan0, Stride, start, end, Width, red, green, blue, jas, fnCancel);
}

LIBRARY_API void funkcia(int num, void* Scan0, int Stride, int Height, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool()> fnCancel)
{
	return Utils::funkcia(num, Scan0, Stride, Height, Width, red, green, blue, jas,fnCancel);
}

LIBRARY_API void Solarization(bool m_effect,int threshold, int num, Gdiplus::Bitmap* &m_pBitmap, Gdiplus::Bitmap* &m_pBitmap_effect, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool()> fnCancel)
{
	return Utils::Solarization(m_effect, threshold, num, m_pBitmap, m_pBitmap_effect, red, green, blue, jas, fnCancel);
}

LIBRARY_API void  ThreadEffectCalc(Gdiplus::BitmapData *bmpData, Gdiplus::BitmapData *bmpDataEffect, int threshold, int Start, int End, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas,std::function<bool()> fnCancel)
{
	return Utils::ThreadEffectCalc(bmpData, bmpDataEffect, threshold, Start, End, Width, red, green, blue, jas, fnCancel);
}
