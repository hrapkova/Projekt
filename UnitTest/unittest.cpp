#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../Library/Library.h"

#include <vector>
#include <thread>

namespace UnitTest
{
	TEST_CLASS(HistogramUnitTest)
	{
	public:
		TEST_METHOD(TestHistogramBlue)
		{
			std::vector<int> redfun, greenfun, bluefun, jasfun;
			UINT32 pBitmap[256][256];
			memset(pBitmap, 0, sizeof(UINT32) * (256 * 256));
			for (int i = 0; i <= 255; i++)
			{
				for (int j = 0; j <= 255; j++)
				{
						pBitmap[i][j] = ((0 << 16) & 0xff0000) | ((0 << 8) & 0xff00) | (255 & 0xff);
				}
			}
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
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
						Assert::AreEqual(redfun[i], 0, L"cerveny255fun");
						Assert::AreEqual(greenfun[i], 0, L"zeleny255fun");
						Assert::AreEqual(bluefun[i], 256 * 256, L"modry255fun");
					}
					if (i== 0)
					{
						Assert::AreEqual(redfun[i], 256*256, L"cerveny0fun");
						Assert::AreEqual(greenfun[i], 256*256, L"zeleny0fun");
						Assert::AreEqual(bluefun[i], 0, L"modry0fun");
					}
					if (i != 0 && i != 255)
					{
						Assert::AreEqual(redfun[i], 0, L"cerveny");
						Assert::AreEqual(greenfun[i], 0, L"zeleny");
						Assert::AreEqual(bluefun[i], 0, L"modry");
					}
				}
			}
			//*************************************************************************************
			std::vector<int> red, green, blue, jas;
			for (int j = 0; j < 6; j++)
			{
				std::vector<std::vector<int> > reds(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > greens(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > blues(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > jass(num[j], std::vector<int>(256));
				red.assign(256, 0);
				green.assign(256, 0);
				blue.assign(256, 0);
				jas.assign(256, 0);
				redfun.assign(256, 0);
				greenfun.assign(256, 0);
				bluefun.assign(256, 0);
				jasfun.assign(256, 0);
				for (int i = 0; i < num[j]; i++)
				{
					reds[i].assign(256, 0);
					greens[i].assign(256, 0);
					blues[i].assign(256, 0);
					jass[i].assign(256, 0);
				}

				auto t = std::this_thread::get_id();
				funkcia(num[j], pBitmap, 256 * 4, 256, 256, redfun, greenfun, bluefun, jasfun, [this, t]() {return false; });
				t = std::this_thread::get_id();
				for (int i = 0; i < num[j]; i++)
				{
					if (i != num[j] - 1)
					{
						CalcHistogram(pBitmap, 256*4, i*(256 / num[j]), (i + 1) *(256 / num[j]), 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
					}
					else
					{
						CalcHistogram(pBitmap, 256*4, i*(256 / num[j]), 256, 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
					}
				}
				for (int i = 0; i <= 255; i++)
				{
					for (int l = 0; l < num[j]; l++)
					{
						red[i] = red[i] + reds[l][i];
						green[i] = green[i] + greens[l][i];
						blue[i] = blue[i] + blues[l][i];
						jas[i] = jas[i] + jass[l][i];
					}
				}
				for (int i = 0; i <= 255; i++)
				{
						Assert::AreEqual(redfun[i], red[i], L"cerveny255fun");
						Assert::AreEqual(greenfun[i], green[i], L"zeleny255fun");
						Assert::AreEqual(bluefun[i], blue[i], L"modry255fun");
				}
			}
		}

		TEST_METHOD(TestHistogramGreen)
		{
			std::vector<int> redfun, greenfun, bluefun, jasfun;
			UINT32 pBitmap[256][256];
			memset(pBitmap, 0, sizeof(UINT32) * (256 * 256));
			for (int i = 0; i <= 255; i++)
			{
				for (int j = 0; j <= 255; j++)
				{
					pBitmap[i][j] = ((0 << 16) & 0xff0000) | ((255 << 8) & 0xff00) | (0 & 0xff);
				}
			}
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
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
						Assert::AreEqual(redfun[i], 0, L"cerveny255fun");
						Assert::AreEqual(greenfun[i], 256 * 256, L"zeleny255fun");
						Assert::AreEqual(bluefun[i], 0, L"modry255fun");
					}
					if (i == 0)
					{
						Assert::AreEqual(redfun[i], 256 * 256, L"cerveny0fun");
						Assert::AreEqual(greenfun[i], 0, L"zeleny0fun");
						Assert::AreEqual(bluefun[i], 256 * 256, L"modry0fun");
					}
					if (i != 0 && i != 255)
					{
						Assert::AreEqual(redfun[i], 0, L"cerveny");
						Assert::AreEqual(greenfun[i], 0, L"zeleny");
						Assert::AreEqual(bluefun[i], 0, L"modry");
					}
				}
			}
			//*************************************************************************************
			std::vector<int> red, green, blue, jas;
			for (int j = 0; j < 6; j++)
			{
				std::vector<std::vector<int> > reds(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > greens(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > blues(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > jass(num[j], std::vector<int>(256));
				red.assign(256, 0);
				green.assign(256, 0);
				blue.assign(256, 0);
				jas.assign(256, 0);
				redfun.assign(256, 0);
				greenfun.assign(256, 0);
				bluefun.assign(256, 0);
				jasfun.assign(256, 0);
				for (int i = 0; i < num[j]; i++)
				{
					reds[i].assign(256, 0);
					greens[i].assign(256, 0);
					blues[i].assign(256, 0);
					jass[i].assign(256, 0);
				}

				auto t = std::this_thread::get_id();
				funkcia(num[j], pBitmap, 256 * 4, 256, 256, redfun, greenfun, bluefun, jasfun, [this, t]() {return false; });
				t = std::this_thread::get_id();
				for (int i = 0; i < num[j]; i++)
				{
					if (i != num[j] - 1)
					{
						CalcHistogram(pBitmap, 256 * 4, i*(256 / num[j]), (i + 1) *(256 / num[j]), 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
					}
					else
					{
						CalcHistogram(pBitmap, 256 * 4, i*(256 / num[j]), 256, 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
					}
				}
				for (int i = 0; i <= 255; i++)
				{
					for (int l = 0; l < num[j]; l++)
					{
						red[i] = red[i] + reds[l][i];
						green[i] = green[i] + greens[l][i];
						blue[i] = blue[i] + blues[l][i];
						jas[i] = jas[i] + jass[l][i];
					}
				}
				for (int i = 0; i <= 255; i++)
				{
					Assert::AreEqual(redfun[i], red[i], L"cerveny255fun");
					Assert::AreEqual(greenfun[i], green[i], L"zeleny255fun");
					Assert::AreEqual(bluefun[i], blue[i], L"modry255fun");
				}
			}
		}

		TEST_METHOD(TestHistogramRed)
		{
			std::vector<int> redfun, greenfun, bluefun, jasfun;
			UINT32 pBitmap[256][256];
			memset(pBitmap, 0, sizeof(UINT32) * (256 * 256));
			for (int i = 0; i <= 255; i++)
			{
				for (int j = 0; j <= 255; j++)
				{
					pBitmap[i][j] = ((255 << 16) & 0xff0000) | ((0 << 8) & 0xff00) | (0 & 0xff);
				}
			}
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
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
						Assert::AreEqual(greenfun[i], 0, L"zeleny255fun");
						Assert::AreEqual(bluefun[i], 0, L"modry255fun");
					}
					if (i == 0)
					{
						Assert::AreEqual(redfun[i], 0, L"cerveny0fun");
						Assert::AreEqual(greenfun[i], 256 * 256, L"zeleny0fun");
						Assert::AreEqual(bluefun[i], 256 * 256, L"modry0fun");
					}
					if (i != 0 && i != 255)
					{
						Assert::AreEqual(redfun[i], 0, L"cerveny");
						Assert::AreEqual(greenfun[i], 0, L"zeleny");
						Assert::AreEqual(bluefun[i], 0, L"modry");
					}
				}
			}
			//*************************************************************************************
			std::vector<int> red, green, blue, jas;
			for (int j = 0; j < 6; j++)
			{
				std::vector<std::vector<int> > reds(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > greens(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > blues(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > jass(num[j], std::vector<int>(256));
				red.assign(256, 0);
				green.assign(256, 0);
				blue.assign(256, 0);
				jas.assign(256, 0);
				redfun.assign(256, 0);
				greenfun.assign(256, 0);
				bluefun.assign(256, 0);
				jasfun.assign(256, 0);
				for (int i = 0; i < num[j]; i++)
				{
					reds[i].assign(256, 0);
					greens[i].assign(256, 0);
					blues[i].assign(256, 0);
					jass[i].assign(256, 0);
				}

				auto t = std::this_thread::get_id();
				funkcia(num[j], pBitmap, 256 * 4, 256, 256, redfun, greenfun, bluefun, jasfun, [this, t]() {return false; });
				t = std::this_thread::get_id();
				for (int i = 0; i < num[j]; i++)
				{
					if (i != num[j] - 1)
					{
						CalcHistogram(pBitmap, 256 * 4, i*(256 / num[j]), (i + 1) *(256 / num[j]), 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
					}
					else
					{
						CalcHistogram(pBitmap, 256 * 4, i*(256 / num[j]), 256, 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
					}
				}
				for (int i = 0; i <= 255; i++)
				{
					for (int l = 0; l < num[j]; l++)
					{
						red[i] = red[i] + reds[l][i];
						green[i] = green[i] + greens[l][i];
						blue[i] = blue[i] + blues[l][i];
						jas[i] = jas[i] + jass[l][i];
					}
				}
				for (int i = 0; i <= 255; i++)
				{
					Assert::AreEqual(redfun[i], red[i], L"cerveny255fun");
					Assert::AreEqual(greenfun[i], green[i], L"zeleny255fun");
					Assert::AreEqual(bluefun[i], blue[i], L"modry255fun");
				}
			}
		}
	};

	TEST_CLASS(SolarizationTest)
	{
	public:
		TEST_METHOD(EffectTestWhite)
		{
			std::vector<int> thresholds = { 0,50,100,150,200,255 };
			std::vector<int> red, green, blue, jas;
			UINT32 pBitmap[256][256];
			UINT32 pBitmapEffect[256][256];
			//biela 
			memset(pBitmap, 0xffffffff, sizeof(UINT32) * (256 * 256));
			memset(pBitmapEffect, 0xffffffff, sizeof(UINT32) * (256 * 256));
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
				red.assign(256, 0);
				green.assign(256, 0);
				blue.assign(256, 0);
				jas.assign(256, 0);

				auto t = std::this_thread::get_id();
				for (int k = 0; k < thresholds.size(); k++)
				{
					Solarization(true, thresholds[k], num[j], pBitmap, 256 * 4, pBitmapEffect, 256 * 4, 256, 256, red, green, blue, jas, [this, t]() {return false; });
					for (int i = 0; i <= 255; i++)
					{
						if (thresholds[k] != 255)
						{
							if (i == 0)
							{
								Assert::AreEqual(red[i], 256 * 256, L"cerveny00");
								Assert::AreEqual(green[i], 256 * 256, L"zeleny00");
								Assert::AreEqual(blue[i], 256 * 256, L"modry00");
							}
							else
							{
								Assert::AreEqual(red[i], 0, L"cerveny0");
								Assert::AreEqual(green[i], 0, L"zeleny0");
								Assert::AreEqual(blue[i], 0, L"modry0");
							}
						}
						else
						{
							if (i == 255)
							{
								Assert::AreEqual(red[i], 256 * 256, L"cerveny255");
								Assert::AreEqual(green[i], 256 * 256, L"zeleny255");
								Assert::AreEqual(blue[i], 256 * 256, L"modry255");
							}
							else
							{
								Assert::AreEqual(red[i], 0, L"cerveny2550");
								Assert::AreEqual(green[i], 0, L"zeleny2550");
								Assert::AreEqual(blue[i], 0, L"modry2550");
							}
						}
					}
				}
			}
		}

		TEST_METHOD(EffectTestBlack)
		{
			std::vector<int> thresholds = { 0,50,100,150,200,255 };
			std::vector<int> red, green, blue, jas;
			UINT32 pBitmap[256][256];
			UINT32 pBitmapEffect[256][256];
			//cierna
			memset(pBitmap, 0, sizeof(UINT32) * (256 * 256));
			memset(pBitmapEffect, 0, sizeof(UINT32) * (256 * 256));
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
				red.assign(256, 0);
				green.assign(256, 0);
				blue.assign(256, 0);
				jas.assign(256, 0);
				auto t = std::this_thread::get_id();
				for (int k = 0; k < thresholds.size(); k++)
				{
					Solarization(true, thresholds[k], num[j], pBitmap, 256 * 4, pBitmapEffect, 256 * 4, 256, 256, red, green, blue, jas, [this, t]() {return false; });
					for (int i = 0; i <= 255; i++)
					{
						if (thresholds[k] != 0)
						{
							if (i == 255)
							{
								Assert::AreEqual(red[i], 256 * 256, L"c00");
								Assert::AreEqual(green[i], 256 * 256, L"z00");
								Assert::AreEqual(blue[i], 256 * 256, L"m00");
							}
							else
							{
								Assert::AreEqual(red[i], 0, L"c0");
								Assert::AreEqual(green[i], 0, L"z0");
								Assert::AreEqual(blue[i], 0, L"m0");
							}
						}
						else
						{
							if (i == 0)
							{
								Assert::AreEqual(red[i], 256 * 256, L"c255");
								Assert::AreEqual(green[i], 256 * 256, L"z255");
								Assert::AreEqual(blue[i], 256 * 256, L"m255");
							}
							else
							{
								Assert::AreEqual(red[i], 0, L"c2550");
								Assert::AreEqual(green[i], 0, L"z2550");
								Assert::AreEqual(blue[i], 0, L"m2550");
							}
						}
					}
				}
			}
		}

		TEST_METHOD(EffectTestBlackAndWhite)
		{
			std::vector<int> thresholds = { 0,50,100,150,200,255 };
			std::vector<int> red, green, blue, jas;
			UINT32 pBitmap[256][256];
			UINT32 pBitmapEffect[256][256];
			//cierna a biela
			memset(pBitmap, 0, sizeof(UINT32) * (256 * 256));
			memset(pBitmapEffect, 0, sizeof(UINT32) * (256 * 256));
			bool white = true;
			for (int i = 0; i <= 255; i++)
			{
				for (int j = 0; j <= 255; j++)
				{
					if (white)
					{
						pBitmap[i][j] = ((255 << 16) & 0xff0000) | ((255 << 8) & 0xff00) | (255 & 0xff);
					}
					else
					{
						pBitmap[i][j] = ((0 << 16) & 0xff0000) | ((0 << 8) & 0xff00) | (0 & 0xff);
					}
					white = !white;
				}
			}
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
				red.assign(256, 0);
				green.assign(256, 0);
				blue.assign(256, 0);
				jas.assign(256, 0);
				auto t = std::this_thread::get_id();
				for (int k = 0; k < thresholds.size(); k++)
				{
					Solarization(true, thresholds[k], num[j], pBitmap, 256 * 4, pBitmapEffect, 256 * 4, 256, 256, red, green, blue, jas, [this, t]() {return false; });
					for (int i = 0; i <= 255; i++)
					{
						if (thresholds[k] == 0 || thresholds[k] == 255)
						{
							if (i == thresholds[k])
							{
								Assert::AreEqual(red[i], 256 * 256, L"c00");
								Assert::AreEqual(green[i], 256 * 256, L"z00");
								Assert::AreEqual(blue[i], 256 * 256, L"m00");
							}
							else
							{
								Assert::AreEqual(red[i], 0, L"c0");
								Assert::AreEqual(green[i], 0, L"z0");
								Assert::AreEqual(blue[i], 0, L"m0");
							}
						}
						else
						{
							if (i == 255 || i == 0)
							{
								Assert::AreEqual(red[i], 128 * 256, L"c00");
								Assert::AreEqual(green[i], 128 * 256, L"z00");
								Assert::AreEqual(blue[i], 128 * 256, L"m00");
							}
							else
							{
								Assert::AreEqual(red[i], 0, L"c0");
								Assert::AreEqual(green[i], 0, L"z0");
								Assert::AreEqual(blue[i], 0, L"m0");
							}
						}
					}
				}
			}
		}
	};

	TEST_CLASS(ThreadEffectFunTest)
	{
	public:
		TEST_METHOD(ThreadEffectCalcTestWhite)
		{
			std::vector<int> red, green, blue, jas;
			std::vector<int> redFinal, greenFinal, blueFinal, jasFinal;
			std::vector<int> thresholds = { 0,50,100,150,200,255 };
			UINT32 pBitmap[256][256];
			UINT32 pBitmapEffect[256][256];
			memset(pBitmap, 0xffffffff, sizeof(UINT32) * (256 * 256));
			memset(pBitmapEffect, 0xffffffff, sizeof(UINT32) * (256 * 256));
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
				std::vector<std::vector<int> > reds(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > greens(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > blues(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > jass(num[j], std::vector<int>(256));
				auto t = std::this_thread::get_id();
				for (int k = 0; k < thresholds.size(); k++)
				{
					red.assign(256, 0);
					green.assign(256, 0);
					blue.assign(256, 0);
					jas.assign(256, 0);
					redFinal.assign(256, 0);
					greenFinal.assign(256, 0);
					blueFinal.assign(256, 0);
					jasFinal.assign(256, 0);
					for (int i = 0; i < num[j]; i++)
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
					Solarization(true, thresholds[k], num[j], pBitmap, 256 * 4, pBitmapEffect, 256 * 4, 256, 256, red, green, blue, jas, [this, t]() {return false; });

					for (int i = 0; i < num[j]; i++)
					{
						if (i != num[j] - 1)
						{
							ThreadEffectCalc(pBitmap, 256 * 4, pBitmapEffect, 256 * 4, thresholds[k], i*(256 / num[j]), (i + 1) *(256 / num[j]), 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
						}
						else
						{
							ThreadEffectCalc(pBitmap, 256 * 4, pBitmapEffect, 256 * 4, thresholds[k], i*(256 / num[j]), 256, 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
						}
					}
					for (int i = 0; i <= 255; i++)
					{
						for (int l = 0; l < num[j]; l++)
						{
							redFinal[i] = redFinal[i] + reds[l][i];
							greenFinal[i] = greenFinal[i] + greens[l][i];
							blueFinal[i] = blueFinal[i] + blues[l][i];
							jasFinal[i] = jasFinal[i] + jass[l][i];
						}
					}

					for (int i = 0; i <= 255; i++)
					{
						Assert::AreEqual(red[i], redFinal[i], L"cerveny");
						Assert::AreEqual(green[i], greenFinal[i], L"zeleny");
						Assert::AreEqual(blue[i], blueFinal[i], L"modry");

					}
				}
			}
		}

		TEST_METHOD(ThreadEffectCalcTestBlack)
		{
			std::vector<int> red, green, blue, jas;
			std::vector<int> redFinal, greenFinal, blueFinal, jasFinal;
			std::vector<int> thresholds = { 0,50,100,150,200,255 };
			UINT32 pBitmap[256][256];
			UINT32 pBitmapEffect[256][256];
			memset(pBitmap, 0, sizeof(UINT32) * (256 * 256));
			memset(pBitmapEffect, 0xff, sizeof(UINT32) * (256 * 256));
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
				std::vector<std::vector<int> > reds(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > greens(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > blues(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > jass(num[j], std::vector<int>(256));
				auto t = std::this_thread::get_id();
				for (int k = 0; k < thresholds.size(); k++)
				{
					red.assign(256, 0);
					green.assign(256, 0);
					blue.assign(256, 0);
					jas.assign(256, 0);
					redFinal.assign(256, 0);
					greenFinal.assign(256, 0);
					blueFinal.assign(256, 0);
					jasFinal.assign(256, 0);
					for (int i = 0; i < num[j]; i++)
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
					Solarization(true, thresholds[k], num[j], pBitmap, 256 * 4, pBitmapEffect, 256 * 4, 256, 256, red, green, blue, jas, [this, t]() {return false; });

					for (int i = 0; i < num[j]; i++)
					{
						if (i != num[j] - 1)
						{
							ThreadEffectCalc(pBitmap, 256 * 4, pBitmapEffect, 256 * 4, thresholds[k], i*(256 / num[j]), (i + 1) *(256 / num[j]), 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
						}
						else
						{
							ThreadEffectCalc(pBitmap, 256 * 4, pBitmapEffect, 256 * 4, thresholds[k], i*(256 / num[j]), 256, 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
						}
					}
					for (int i = 0; i <= 255; i++)
					{
						for (int l = 0; l < num[j]; l++)
						{
							redFinal[i] = redFinal[i] + reds[l][i];
							greenFinal[i] = greenFinal[i] + greens[l][i];
							blueFinal[i] = blueFinal[i] + blues[l][i];
							jasFinal[i] = jasFinal[i] + jass[l][i];
						}
					}

					for (int i = 0; i <= 255; i++)
					{
						Assert::AreEqual(red[i], redFinal[i], L"cerveny");
						Assert::AreEqual(green[i], greenFinal[i], L"zeleny");
						Assert::AreEqual(blue[i], blueFinal[i], L"modry");

					}
				}
			}
		}

		TEST_METHOD(ThreadEffectCalcTestBlackAndWhite)
		{
			std::vector<int> red, green, blue, jas;
			std::vector<int> redFinal, greenFinal, blueFinal, jasFinal;
			std::vector<int> thresholds = { 0,50,100,150,200,255 };
			UINT32 pBitmap[256][256];
			UINT32 pBitmapEffect[256][256];
			memset(pBitmap, 0xff, sizeof(UINT32) * (256 * 256));
			memset(pBitmapEffect, 0xff, sizeof(UINT32) * (256 * 256));
			bool white = true;
			for (int i = 0; i <= 255; i++)
			{
				for (int j = 0; j <= 255; j++)
				{
					if (white)
					{
						pBitmap[i][j] = ((255 << 16) & 0xff0000) | ((255 << 8) & 0xff00) | (255 & 0xff);
					}
					else
					{
						pBitmap[i][j] = ((0 << 16) & 0xff0000) | ((0 << 8) & 0xff00) | (0 & 0xff);
					}
					white = !white;
				}
			}
			int num[6] = { 1,2,4,8,12,16 };

			for (int j = 0; j < 6; j++)
			{
				std::vector<std::vector<int> > reds(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > greens(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > blues(num[j], std::vector<int>(256));
				std::vector<std::vector<int> > jass(num[j], std::vector<int>(256));
				auto t = std::this_thread::get_id();
				for (int k = 0; k < thresholds.size(); k++)
				{
					red.assign(256, 0);
					green.assign(256, 0);
					blue.assign(256, 0);
					jas.assign(256, 0);
					redFinal.assign(256, 0);
					greenFinal.assign(256, 0);
					blueFinal.assign(256, 0);
					jasFinal.assign(256, 0);
					for (int i = 0; i < num[j]; i++)
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
					Solarization(true, thresholds[k], num[j], pBitmap, 256 * 4, pBitmapEffect, 256 * 4, 256, 256, red, green, blue, jas, [this, t]() {return false; });

					for (int i = 0; i < num[j]; i++)
					{
						if (i != num[j] - 1)
						{
							ThreadEffectCalc(pBitmap, 256 * 4, pBitmapEffect, 256 * 4, thresholds[k], i*(256 / num[j]), (i + 1) *(256 / num[j]), 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
						}
						else
						{
							ThreadEffectCalc(pBitmap, 256 * 4, pBitmapEffect, 256 * 4, thresholds[k], i*(256 / num[j]), 256, 256, reds[i], greens[i], blues[i], jass[i], [this, t]() {return false; });
						}
					}
					for (int i = 0; i <= 255; i++)
					{
						for (int l = 0; l < num[j]; l++)
						{
							redFinal[i] = redFinal[i] + reds[l][i];
							greenFinal[i] = greenFinal[i] + greens[l][i];
							blueFinal[i] = blueFinal[i] + blues[l][i];
							jasFinal[i] = jasFinal[i] + jass[l][i];
						}
					}

					for (int i = 0; i <= 255; i++)
					{
						Assert::AreEqual(red[i], redFinal[i], L"cerveny");
						Assert::AreEqual(green[i], greenFinal[i], L"zeleny");
						Assert::AreEqual(blue[i], blueFinal[i], L"modry");

					}
				}
			}
		}
	};
}
