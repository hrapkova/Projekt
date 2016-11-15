#pragma once

#include <utility>
#include <vector>

namespace Utils
{
	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile);
	void CalcHistogram(void* Scan0, int Stride, int start, int end, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas);
}