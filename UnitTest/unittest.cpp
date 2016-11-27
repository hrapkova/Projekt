#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Library/Library.h"

#include <vector>
#include <thread>

namespace UnitTest
{
	/*TEST_CLASS(ParseFilesUnitTest)
	{
	public:

		TEST_METHOD(ParseFiles_TestOneFile)
		{
			auto result = ParseFiles(L"C:\\directory\\subdirectory\\file.name.ext\0");
			Assert::AreEqual(result.first, L"C:\\directory\\subdirectory\\", L"directory");
			Assert::IsTrue(result.second.size() == 1, L"count of files");
			Assert::AreEqual(result.second[0], L"file.name.ext", "filename");
		}

		TEST_METHOD(ParseFiles_TestMultipleFiles)
		{
			auto result = ParseFiles(L"C:\\directory\\subdirectory\0file1.name.ext\0file2.name.ext\0");
			Assert::AreEqual(result.first, L"C:\\directory\\subdirectory\\", L"directory");
			Assert::IsTrue(result.second.size() == 2, L"count of files");
			Assert::AreEqual(result.second[0], L"file1.name.ext", "filename 1");
			Assert::AreEqual(result.second[1], L"file2.name.ext", "filename 2");
		}
	};*/

	TEST_CLASS(HistogramUnitTest)
	{
	public:

		TEST_METHOD(TestHistogram)
		{
			std::vector<int> red, green, blue, jas;
			std::vector<int> red1, green1, blue1, jas1;
			std::vector<int> red2, green2, blue2, jas2;
			red.assign(256, 0);
			green.assign(256, 0);
			blue.assign(256, 0);
			jas.assign(256, 0);
			red1.assign(256, 0);
			green1.assign(256, 0);
			blue1.assign(256, 0);
			jas1.assign(256, 0);
			red2.assign(256, 0);
			green2.assign(256, 0);
			blue2.assign(256, 0);
			jas2.assign(256, 0);

			//CalcHistogram(nullptr, 0, 0, 0, red, green, blue, jas);

			UINT32 pBitmap[256][256];
			memset(pBitmap, 0, sizeof(UINT32) * (256 * 256));

			for (int i = 0; i < 256; i++)
			{
				for (int j = 0; j < 256; j++)
				{
					pBitmap[i][j] = 0xffffffff;
				}
			}
			auto t = std::this_thread::get_id();
			CalcHistogram(pBitmap, 256 * 4, 0, 256 / 2, 256, red1, green1, blue1, jas1, [this, t]() {return false; });
			CalcHistogram(pBitmap, 256 * 4, 256 / 2, 256, 256, red2, green2, blue2, jas2, [this, t]() {return false; });
			for (int i = 0; i <= 255; i++)
			{
				red[i] = red1[i] + red2[i];
				green[i] = green1[i] + green2[i];
				blue[i] = blue1[i] + blue2[i];
				jas[i] = jas1[i] + jas2[i];
			}
			//biela
			for (int i = 0; i <= 255; i++)
			{
				if (i == 255)
				{
					Assert::AreEqual(red[i], 256*256, L"cerveny255");
					Assert::AreEqual(green[i], 256*256, L"zeleny255");
					Assert::AreEqual(blue[i], 256*256, L"modry255");
				}
				else
				{
					Assert::AreEqual(red[i], 0, L"cerveny0");
					Assert::AreEqual(green[i], 0, L"zeleny0");
					Assert::AreEqual(blue[i], 0, L"modry0");
				}
				   
			}
			//*****************************************************************
			int num[6] = { 1,2,4,8,12,16 };
			for (int j = 0; j < 6; j++)
			{
				std::vector<int> redfun, greenfun, bluefun, jasfun;
				redfun.assign(256, 0);
				greenfun.assign(256, 0);
				bluefun.assign(256, 0);
				jasfun.assign(256, 0);

				auto t = std::this_thread::get_id();
				funkcia(num[j], pBitmap, 256 * 4, 256, 256, redfun, greenfun, bluefun, jasfun, [this, t]() {return false; });

				for (int i = 0; i <= 255; i++)
				{
					if (i == 255)
					{
						Assert::AreEqual(redfun[i], 256 * 256, L"cerveny255fun");
						Assert::AreEqual(greenfun[i], 256 * 256, L"zeleny255fun");
						Assert::AreEqual(bluefun[i], 256 * 256, L"modry255fun");
					}
					else
					{
						Assert::AreEqual(redfun[i], 0, L"cerveny0fun");
						Assert::AreEqual(greenfun[i], 0, L"zeleny0fun");
						Assert::AreEqual(bluefun[i], 0, L"modry0fun");
					}

				}
			}
		}

	};
}
