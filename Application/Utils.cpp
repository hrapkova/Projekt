#include "stdafx.h"
#include "Utils.h"
#include <thread>

namespace Utils
{
	//	parse file names from file name string in OPENFILENAME struct
	//	returns pair of directory and vector of filenames
	//
	std::pair< CString, std::vector<CString> > ParseFiles(LPCTSTR lpstrFile)
	{
		CString cs = lpstrFile;

		// skip directory name
		while (*lpstrFile) ++lpstrFile;

		if (*(++lpstrFile))
		{
			CString csDirectory;
			std::vector<CString> names;

			csDirectory = cs + _T("\\");
			// iterate filenames
			for ( ; *lpstrFile; ++lpstrFile)
			{
				names.push_back(lpstrFile);

				while (*lpstrFile) ++lpstrFile;
			}

			return std::make_pair(csDirectory, names);
		}
		else
		{	// only one filename
			CString csName, csExt;
			_tsplitpath_s(cs, nullptr, 0, nullptr, 0, csName.GetBuffer(_MAX_FNAME), _MAX_FNAME, csExt.GetBuffer(_MAX_EXT), _MAX_EXT);
			csName.ReleaseBuffer();
			csExt.ReleaseBuffer();

			return std::make_pair(cs.Left(cs.GetLength() - csName.GetLength() - csExt.GetLength()), std::vector<CString>({ csName + csExt }));
		}

	}

	void CalcHistogram(void* Scan0, int Stride, int start, int end, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool ()> fnCancel)
	{

		uint32_t *pLine = (uint32_t*)Scan0;
		for (int y = start; y < end; y++)
		{
			if (fnCancel()) { return; }
			pLine = (uint32_t*)((uint8_t*)Scan0 + Stride*y);
			for (int x = 0; x < Width; x++)
			{
				red[((*pLine) >> 16) & 0xff]++;
				green[((*pLine) >> 8) & 0xff]++;
				blue[(*pLine) & 0xff]++;
				jas[0.2126 * (((*pLine) >> 16) & 0xff) + 0.7152 * (((*pLine) >> 8) & 0xff) + 0.0722 * ((*pLine) & 0xff)]++;
				pLine++;
			}
		}

		return;
	}

	void funkcia(int num, void* Scan0, int Stride, int Height, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool()> fnCancel)
	{
		std::vector<std::thread> threads;
		std::vector<std::vector<int> > reds(num, std::vector<int>(256));
		std::vector<std::vector<int> > greens(num, std::vector<int>(256));
		std::vector<std::vector<int> > blues(num, std::vector<int>(256));
		std::vector<std::vector<int> > jass(num, std::vector<int>(256));
		for (int i = 0; i < num; i++)
		{
			reds[i].clear();
			greens[i].clear();
			blues[i].clear();
			jass[i].clear();

			reds[i].assign(256, 0);
			greens[i].assign(256, 0);
			blues[i].assign(256, 0);
			jass[i].assign(256, 0);
		}
			for (int i = 0; i < num; i++)
			{
				if (i != num - 1)
				{
					threads.push_back(std::move(std::thread(&Utils::CalcHistogram, std::ref(Scan0), Stride, i*(Height / num), (i + 1) *(Height / num), Width, std::ref(reds[i]), std::ref(greens[i]), std::ref(blues[i]), std::ref(jass[i]), fnCancel)));
				}
				else
				{
					threads.push_back(std::move(std::thread(&Utils::CalcHistogram, std::ref(Scan0), Stride, i*(Height / num), Height, Width, std::ref(reds[i]), std::ref(greens[i]), std::ref(blues[i]), std::ref(jass[i]), fnCancel)));
				}
			}
			for (int i = 0; i < num;i++)
			{
				threads[i].join();
			}

		for (int i = 0; i <= 255; i++)
		{
			for (int j = 0; j < num; j++)
			{
				red[i] = red[i] + reds[j][i];
				green[i] = green[i] + greens[j][i];
				blue[i] = blue[i] + blues[j][i];
				jas[i] = jas[i] + jass[j][i];
			}
		}

		return;
	}
}
