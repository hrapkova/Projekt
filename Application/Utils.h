#pragma once

#include <utility>
#include <vector>
#include <functional>

#include "ApplicationDlg.h"

namespace Utils
{
	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile);
	void CalcHistogram(void* Scan0, int Stride, int start, int end, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool()> fnCancel);
	void funkcia(int num, void* Scan0, int Stride, int Height, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool()> fnCancel);
	void Solarization(bool m_effect, int threshold, int num, Gdiplus::Bitmap* &m_pBitmap, Gdiplus::Bitmap* &m_pBitmap_effect, std::function<bool()> fnCancel);
	void ThreadEffectCalc(Gdiplus::BitmapData *bmpData, Gdiplus::BitmapData *bmpDataEffect, int threshold, int Start, int End, int Width, std::function<bool()> fnCancel);
}