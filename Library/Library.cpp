// Library.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
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

LIBRARY_API void CalcHistogram(void* Scan0, int Stride, int Height, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas)
{
	return Utils::CalcHistogram(Scan0, Stride, Height, Width, red, green, blue, jas);
}
