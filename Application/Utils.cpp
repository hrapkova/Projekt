#include "stdafx.h"
#include "Utils.h"

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

	void CalcHistogram(void* Scan0, int Stride, int start, int end, int Width, std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas)
	{

		uint32_t *pLine = (uint32_t*)Scan0;
		for (int y = start; y < end; y++)
		{
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
}
