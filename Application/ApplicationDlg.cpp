// ApplicationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "ApplicationDlg.h"
#include "afxdialogex.h"
#include <utility>
#include <tuple>
#include <vector>
#include "Utils.h"
#include <omp.h>
#include <math.h>
#include <thread>
#include <atomic>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef MIN_SIZE
#define MIN_SIZE 300
#endif

void CStaticImage::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	GetParent()->SendMessage( CApplicationDlg::WM_DRAW_IMAGE, (WPARAM)lpDrawItemStruct);
}

void CStaticHistogram::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	GetParent()->SendMessage( CApplicationDlg::WM_DRAW_HISTOGRAM, (WPARAM)lpDrawItemStruct);
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	void DoDataExchange(CDataExchange* pDX) override    // DDX/DDV support
	{
		CDialogEx::DoDataExchange(pDX);
	}

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


namespace
{
	typedef BOOL(WINAPI *LPFN_GLPI)(
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
		PDWORD);


	// Helper function to count set bits in the processor mask.
	DWORD CountSetBits(ULONG_PTR bitMask)
	{
		DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
		DWORD bitSetCount = 0;
		ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
		DWORD i;

		for (i = 0; i <= LSHIFT; ++i)
		{
			bitSetCount += ((bitMask & bitTest) ? 1 : 0);
			bitTest /= 2;
		}

		return bitSetCount;
	}

	DWORD CountMaxThreads()
	{
		LPFN_GLPI glpi;
		BOOL done = FALSE;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
		DWORD returnLength = 0;
		DWORD logicalProcessorCount = 0;
		DWORD numaNodeCount = 0;
		DWORD processorCoreCount = 0;
		DWORD processorL1CacheCount = 0;
		DWORD processorL2CacheCount = 0;
		DWORD processorL3CacheCount = 0;
		DWORD processorPackageCount = 0;
		DWORD byteOffset = 0;
		PCACHE_DESCRIPTOR Cache;

		glpi = (LPFN_GLPI)GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),
			"GetLogicalProcessorInformation");
		if (NULL == glpi)
		{
			TRACE(TEXT("\nGetLogicalProcessorInformation is not supported.\n"));
			return (1);
		}

		while (!done)
		{
			DWORD rc = glpi(buffer, &returnLength);

			if (FALSE == rc)
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
						free(buffer);

					buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
						returnLength);

					if (NULL == buffer)
					{
						TRACE(TEXT("\nError: Allocation failure\n"));
						return (2);
					}
				}
				else
				{
					TRACE(TEXT("\nError %d\n"), GetLastError());
					return (3);
				}
			}
			else
			{
				done = TRUE;
			}
		}

		ptr = buffer;

		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
		{
			switch (ptr->Relationship)
			{
			case RelationNumaNode:
				// Non-NUMA systems report a single record of this type.
				numaNodeCount++;
				break;

			case RelationProcessorCore:
				processorCoreCount++;

				// A hyperthreaded core supplies more than one logical processor.
				logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
				break;

			case RelationCache:
				// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
				Cache = &ptr->Cache;
				if (Cache->Level == 1)
				{
					processorL1CacheCount++;
				}
				else if (Cache->Level == 2)
				{
					processorL2CacheCount++;
				}
				else if (Cache->Level == 3)
				{
					processorL3CacheCount++;
				}
				break;

			case RelationProcessorPackage:
				// Logical processors share a physical package.
				processorPackageCount++;
				break;

			default:
				TRACE(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
				break;
			}
			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}

		TRACE(TEXT("\nGetLogicalProcessorInformation results:\n"));
		TRACE(TEXT("Number of NUMA nodes: %d\n"), numaNodeCount);
		TRACE(TEXT("Number of physical processor packages: %d\n"), processorPackageCount);
		TRACE(TEXT("Number of processor cores: %d\n"), processorCoreCount);
		TRACE(TEXT("Number of logical processors: %d\n"), logicalProcessorCount);
		TRACE(TEXT("Number of processor L1/L2/L3 caches: %d/%d/%d\n"), processorL1CacheCount, processorL2CacheCount, processorL3CacheCount);

		free(buffer);
		TRACE(_T("OPENMP - %i/%i\n"), omp_get_num_procs(), omp_get_max_threads());
		return logicalProcessorCount;
	}
}

// CApplicationDlg dialog


CApplicationDlg::CApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_APPLICATION_DIALOG, pParent)
	, m_pBitmap(nullptr), m_pBitmap_effect(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nMaxThreads = CountMaxThreads();
}

void CApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_ctrlFileList);
	DDX_Control(pDX, IDC_IMAGE, m_ctrlImage);
	DDX_Control(pDX, IDC_HISTOGRAM, m_ctrlHistogram);
}

BEGIN_MESSAGE_MAP(CApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_MESSAGE(WM_DRAW_IMAGE, OnDrawImage)
	ON_MESSAGE(WM_DRAW_HISTOGRAM, OnDrawHistogram)
	ON_MESSAGE(WM_SET_BITMAP, OnSetBitmap)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnLvnItemchangedFileList)
	ON_COMMAND(ID_LOG_OPEN, OnLogOpen)
	ON_UPDATE_COMMAND_UI(ID_LOG_OPEN, OnUpdateLogOpen)
	ON_COMMAND(ID_LOG_CLEAR, OnLogClear)
	ON_UPDATE_COMMAND_UI(ID_LOG_CLEAR, OnUpdateLogClear)
	ON_WM_DESTROY()
	ON_COMMAND(ID_HISTOGRAM_RED, &CApplicationDlg::OnHistogramRed)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_RED, &CApplicationDlg::OnUpdateHistogramRed)
	ON_COMMAND(ID_HISTOGRAM_GREEN, &CApplicationDlg::OnHistogramGreen)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_GREEN, &CApplicationDlg::OnUpdateHistogramGreen)
	ON_COMMAND(ID_HISTOGRAM_BLUE, &CApplicationDlg::OnHistogramBlue)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_BLUE, &CApplicationDlg::OnUpdateHistogramBlue)
	ON_COMMAND(ID_HISTOGRAM_ALPHA, &CApplicationDlg::OnHistogramAlpha)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM_ALPHA, &CApplicationDlg::OnUpdateHistogramAlpha)
	ON_COMMAND(ID_THREADS_1, &CApplicationDlg::OnThreads<1>)
	ON_COMMAND(ID_THREADS_2, &CApplicationDlg::OnThreads<2>)
	ON_COMMAND(ID_THREADS_4, &CApplicationDlg::OnThreads<4>)
	ON_COMMAND(ID_THREADS_8, &CApplicationDlg::OnThreads<8>)
	ON_COMMAND(ID_THREADS_12, &CApplicationDlg::OnThreads<12>)
	ON_COMMAND(ID_THREADS_16, &CApplicationDlg::OnThreads<16>)
	ON_UPDATE_COMMAND_UI(ID_THREADS_1, &CApplicationDlg::OnUpdateThreads<1>)
	ON_UPDATE_COMMAND_UI(ID_THREADS_2, &CApplicationDlg::OnUpdateThreads<2>)
	ON_UPDATE_COMMAND_UI(ID_THREADS_4, &CApplicationDlg::OnUpdateThreads<4>)
	ON_UPDATE_COMMAND_UI(ID_THREADS_8, &CApplicationDlg::OnUpdateThreads<8>)
	ON_UPDATE_COMMAND_UI(ID_THREADS_12, &CApplicationDlg::OnUpdateThreads<12>)
	ON_UPDATE_COMMAND_UI(ID_THREADS_16, &CApplicationDlg::OnUpdateThreads<16>)
	ON_COMMAND(ID_THREADS_AUTO, &CApplicationDlg::OnThreadsAuto)
	ON_UPDATE_COMMAND_UI(ID_THREADS_AUTO, &CApplicationDlg::OnUpdateThreadsAuto)
	ON_COMMAND(ID_SOLARIZATION_0, &CApplicationDlg::OnSolarization<0>)
	ON_UPDATE_COMMAND_UI(ID_SOLARIZATION_0, &CApplicationDlg::OnUpdateSolarization<0>)
	ON_COMMAND(ID_SOLARIZATION_50, &CApplicationDlg::OnSolarization<50>)
	ON_UPDATE_COMMAND_UI(ID_SOLARIZATION_50, &CApplicationDlg::OnUpdateSolarization<50>)
	ON_COMMAND(ID_SOLARIZATION_100, &CApplicationDlg::OnSolarization<100>)
	ON_UPDATE_COMMAND_UI(ID_SOLARIZATION_100, &CApplicationDlg::OnUpdateSolarization<100>)
	ON_COMMAND(ID_SOLARIZATION_150, &CApplicationDlg::OnSolarization<150>)
	ON_UPDATE_COMMAND_UI(ID_SOLARIZATION_150, &CApplicationDlg::OnUpdateSolarization<150>)
	ON_COMMAND(ID_SOLARIZATION_200, &CApplicationDlg::OnSolarization<200>)
	ON_UPDATE_COMMAND_UI(ID_SOLARIZATION_200, &CApplicationDlg::OnUpdateSolarization<200>)
	ON_COMMAND(ID_SOLARIZATION_255, &CApplicationDlg::OnSolarization<255>)
	ON_UPDATE_COMMAND_UI(ID_SOLARIZATION_255, &CApplicationDlg::OnUpdateSolarization<255>)
END_MESSAGE_MAP()


void CApplicationDlg::OnDestroy()
{
	m_ctrlLog.DestroyWindow();
	Default();
}

	void CApplicationDlg::DrawHist(CDC * pDC, double scaleX,double scaleY, std::vector<int>& vektor, COLORREF farba)
	{
		if (vektor.size() != 0)
		{
			for (int i = 0; i <= 255; i++)
			{
				if (vektor[i] == 0)
				{
					CRect rct(int(i*scaleX + 1), m_ptHistogram.y, int((i + 1)*scaleX + 1), m_ptHistogram.y);
					pDC->FillSolidRect(rct, farba);
				}
				else
				{
					CRect rct(int(i*scaleX + 1),  int(m_ptHistogram.y - log10(vektor[i]) * scaleY), int((i + 1)*scaleX + 1), m_ptHistogram.y);
					pDC->FillSolidRect(rct, farba);
				}
			}

		}
		return;
	}

	void CApplicationDlg::LogAxY(CDC * pDC,CRect* rect,std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas)
	{
		if (red.size() != 0 && green.size() != 0 && blue.size() != 0 && jas.size() != 0)
		{
			int max = 0;
			for (int i = 0; i <= 255; i++)
			{
				if (red[i] > max)
				{
					max = red[i];
				}
				if (green[i] > max)
				{
					max = green[i];
				}
				if (blue[i] > max)
				{
					max = blue[i];
				}
				if (jas[i] > max)
				{
					max = jas[i];
				}
			}
			double sirka = (double)rect->Width() / 256.;
			double vyska = (double)rect->Height() / log10(max);

			if (m_bHistRed)
			{
				DrawHist(pDC, sirka, vyska, red, RGB(255, 0, 0));
			}
			if (m_bHistGreen)
			{
				DrawHist(pDC, sirka, vyska, green, RGB(0, 255, 0));
			}
			if (m_bHistBlue)
			{
				DrawHist(pDC, sirka, vyska, blue, RGB(0, 0, 255));
			}
			if (m_bHistJas)
			{
				DrawHist(pDC, sirka, vyska, jas, RGB(0, 0, 0));
			}

		}
		
		return;
	}

LRESULT CApplicationDlg::OnDrawHistogram(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)wParam;

	CDC * pDC = CDC::FromHandle(lpDI->hDC);

	pDC->FillSolidRect(&(lpDI->rcItem), RGB(255, 255, 255));
	CRect rect(&(lpDI->rcItem));

	if(!m_effect)
	{
		LogAxY(pDC, &rect, m_vHistRed, m_vHistGreen, m_vHistBlue, m_vHistJas);
	}
	else
	{
		LogAxY(pDC, &rect, m_vHistRed_effect, m_vHistGreen_effect, m_vHistBlue_effect, m_vHistJas_effect);
	}


	CBrush brBlack(RGB(0, 0, 0));
	pDC->FrameRect(&(lpDI->rcItem), &brBlack);

	return S_OK;
}

LRESULT CApplicationDlg::OnDrawImage(WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)wParam;

	CDC * pDC = CDC::FromHandle(lpDI->hDC);

	if (m_pBitmap == nullptr)
	{
		pDC->FillSolidRect(&(lpDI->rcItem), RGB(255, 255, 255));
	}
	else
	{
		// Fit bitmap into client area
		double dWtoH = (double)m_pBitmap->GetWidth() / (double)m_pBitmap->GetHeight();

		CRect rct(lpDI->rcItem);
		rct.DeflateRect(1, 1, 1, 1);

		UINT nHeight = rct.Height();
		UINT nWidth = (UINT)(dWtoH * nHeight);

		if (nWidth > (UINT)rct.Width())
		{
			nWidth = rct.Width();
			nHeight = (UINT)(nWidth / dWtoH);
			_ASSERTE(nHeight <= (UINT)rct.Height());
		}

		if (nHeight < (UINT)rct.Height())
		{
			UINT nBanner = (rct.Height() - nHeight) / 2;
			pDC->FillSolidRect(rct.left, rct.top, rct.Width(), nBanner, RGB(255, 255, 255));
			pDC->FillSolidRect(rct.left, rct.bottom - nBanner - 2, rct.Width(), nBanner + 2, RGB(255, 255, 255));
		}

		if (nWidth < (UINT)rct.Width())
		{
			UINT nBanner = (rct.Width() - nWidth) / 2;
			pDC->FillSolidRect(rct.left, rct.top, nBanner, rct.Height(), RGB(255, 255, 255));
			pDC->FillSolidRect(rct.right - nBanner - 2, rct.top, nBanner + 2, rct.Height(), RGB(255, 255, 255));
		}

		Gdiplus::Graphics gr(lpDI->hDC);
		Gdiplus::Rect destRect(rct.left + (rct.Width() - nWidth) / 2, rct.top + (rct.Height() - nHeight) / 2, nWidth, nHeight);
		if (m_effect)
		{
			gr.DrawImage(m_pBitmap_effect, destRect);
		}
		else {
			gr.DrawImage(m_pBitmap, destRect);
		}
	}

	CBrush brBlack(RGB(0, 0, 0));
	pDC->FrameRect(&(lpDI->rcItem), &brBlack);

	return S_OK;
}

void CApplicationDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	if ((lpRect->right - lpRect->left) < MIN_SIZE)
	{
		switch (nSide)
		{
		case WMSZ_LEFT:
		case WMSZ_BOTTOMLEFT:
		case WMSZ_TOPLEFT:
			lpRect->left = lpRect->right - MIN_SIZE;
		default:
			lpRect->right = lpRect->left + MIN_SIZE;
			break;
		}
	}

	if ((lpRect->bottom - lpRect->top) < MIN_SIZE)
	{
		switch (nSide)
		{
		case WMSZ_TOP:
		case WMSZ_TOPRIGHT:
		case WMSZ_TOPLEFT:
			lpRect->top = lpRect->bottom - MIN_SIZE;
		default:
			lpRect->bottom = lpRect->top + MIN_SIZE;
			break;
		}
	}

	__super::OnSizing(nSide, lpRect);
}

void CApplicationDlg::OnSize(UINT nType, int cx, int cy)
{
	Default();

	if (!::IsWindow(m_ctrlFileList.m_hWnd) || !::IsWindow(m_ctrlImage.m_hWnd))
		return;

	m_ctrlFileList.SetWindowPos(nullptr, -1, -1, m_ptFileList.x, cy - m_ptFileList.y, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	m_ctrlFileList.Invalidate();


	m_ctrlImage.SetWindowPos(nullptr, -1, -1, cx - m_ptImage.x, cy - m_ptImage.y, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	m_ctrlImage.Invalidate();

	CRect rct;
	GetClientRect(&rct);

	m_ctrlHistogram.SetWindowPos(nullptr, rct.left + m_ptHistogram.x, rct.bottom - m_ptHistogram.y, -1, -1, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	m_ctrlHistogram.Invalidate();
}

void CApplicationDlg::OnClose()
{
	EndDialog(0);
}

BOOL CApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rct;
	m_ctrlFileList.GetClientRect(&rct);
	m_ctrlFileList.InsertColumn(0, _T("Filename"), 0, rct.Width());

	CRect rctClient;
	GetClientRect(&rctClient);
	m_ctrlFileList.GetWindowRect(&rct);
	m_ptFileList.y = rctClient.Height() - rct.Height();
	m_ptFileList.x = rct.Width();

	m_ctrlImage.GetWindowRect(&rct);
	m_ptImage.x = rctClient.Width() - rct.Width();
	m_ptImage.y = rctClient.Height() - rct.Height();

	m_ctrlHistogram.GetWindowRect(&rct);
	ScreenToClient(&rct);
	m_ptHistogram.x = rct.left - rctClient.left;
	m_ptHistogram.y = rctClient.bottom - rct.top;

	m_ctrlLog.Create(IDD_LOG_DIALOG, this);
	m_ctrlLog.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CApplicationDlg::OnFileOpen()
{
	CFileDialog dlg(true, nullptr, nullptr
		, OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST
		, _T("Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|*.png||")
		, this);
	CString cs;
	const int maxFiles = 100;
	const int buffSize = maxFiles * (MAX_PATH + 1) + 1;

	dlg.GetOFN().lpstrFile = cs.GetBuffer(buffSize);
	dlg.GetOFN().nMaxFile = buffSize;

	if (dlg.DoModal() == IDOK)
	{
		m_ctrlFileList.DeleteAllItems();

		if (m_pBitmap != nullptr)
		{
			delete m_pBitmap;
			m_pBitmap = nullptr;
		}
		if (m_pBitmap_effect != nullptr)
		{
			delete m_pBitmap_effect;
			m_pBitmap_effect = nullptr;
		}
		m_ctrlImage.Invalidate();
		m_ctrlHistogram.Invalidate();

		cs.ReleaseBuffer();

		std::vector<CString> names;

		std::tie( m_csDirectory, names) = Utils::ParseFiles(cs);

		for (int i = 0; i < (int)names.size(); ++i)
		{
			m_ctrlFileList.InsertItem(i, names[i]);
		}
	}
	else
	{
		cs.ReleaseBuffer();
	}

}


void CApplicationDlg::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CApplicationDlg::OnFileClose()
{
	m_ctrlFileList.DeleteAllItems();
}


void CApplicationDlg::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ctrlFileList.GetItemCount() > 0);
}

LRESULT CApplicationDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	// stack for menu
	std::vector<CMenu*> menus{ GetMenu() };    //starting with main menu

	CCmdUI cmdUI;
	while (!menus.empty())
	{
		// pop menu from stack
		CMenu * pMenu = menus.back();
		menus.pop_back();

		cmdUI.m_nIndexMax = pMenu->GetMenuItemCount();

		// "recursively" crawl submenus / items
		for (int i = 0; i < pMenu->GetMenuItemCount(); ++i)
		{
			cmdUI.m_nID = pMenu->GetMenuItemID(i);

			if (cmdUI.m_nID == (UINT)-1)
			{
				CMenu* pSubMenu = pMenu->GetSubMenu(i);
				if (pSubMenu)
					menus.push_back(pSubMenu);       // submenu - push to stack
			}
			else
			{
				// call OnUpdate... handler for menu item
				cmdUI.m_nIndex = i;
				cmdUI.m_pMenu = pMenu;
				cmdUI.DoUpdate(this, FALSE);
			}
		}
	}

	return TRUE;
}

namespace
{	
	void LoadAndCount(int thread_num, Gdiplus::Bitmap* &pBitmap, CString fileName,std::vector<int>& red, std::vector<int>& green, std::vector<int>& blue, std::vector<int>& jas, std::function<bool()> fnCancel)
	{
		pBitmap = Gdiplus::Bitmap::FromFile(fileName);
		//vektory vysledne
		red.clear();
		green.clear(); 
		blue.clear(); 
		jas.clear();
		red.assign(256, 0);
		green.assign(256, 0); 
		blue.assign(256, 0); 
		jas.assign(256, 0);

		Gdiplus::Rect ret(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
		Gdiplus::BitmapData *bmpData = new Gdiplus::BitmapData();
		pBitmap->LockBits(&ret, Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, bmpData);

		Utils::funkcia(thread_num, bmpData->Scan0, bmpData->Stride, ret.Height, ret.Width, red, green, blue, jas, fnCancel);

		pBitmap->UnlockBits(bmpData);
		delete bmpData;

		return;
	}
}

LRESULT CApplicationDlg::OnSetBitmap(WPARAM wParam, LPARAM lParam)
{
	//dopln kontrolu ako vo funkcii odkial sa vola OnSetBitmap
	auto ptuple = (std::tuple<Gdiplus::Bitmap*, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&>*)(wParam);
	    m_pBitmap = std::get<0>(*ptuple);
		m_vHistRed = std::move(std::get<1>(*ptuple));
		m_vHistGreen = std::move(std::get<2>(*ptuple));
		m_vHistBlue = std::move(std::get<3>(*ptuple));
		m_vHistJas = std::move(std::get<4>(*ptuple));

	m_ctrlImage.Invalidate();
	m_ctrlHistogram.Invalidate();

	return TRUE;
}

	void CApplicationDlg::SingleThread(CString csFileName)
	{
			Gdiplus::Bitmap *pB = nullptr;
			pB = Gdiplus::Bitmap::FromFile(csFileName);
			std::vector<int> m_vR;
			std::vector<int> m_vG;
			std::vector<int> m_vB;
			std::vector<int> m_vJ; 
			m_vR.clear();
			m_vG.clear();
			m_vB.clear();
			m_vJ.clear();
			m_vR.assign(256, 0);
			m_vG.assign(256, 0);
			m_vB.assign(256, 0);
			m_vJ.assign(256, 0);

			m_thread_id = std::this_thread::get_id();
			auto t= std::this_thread::get_id();
			//auto t = std::thread::id();
			LoadAndCount(thread_num, pB, csFileName, m_vR, m_vG, m_vB, m_vJ, [this, t]() {return m_thread_id != t; });
			if (m_thread_id == std::this_thread::get_id())
			{
				std::tuple<Gdiplus::Bitmap*, std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&> tuple(pB, m_vR, m_vG, m_vB, m_vJ);
				SendMessage(WM_SET_BITMAP, (WPARAM)&tuple);
				m_thread_id = std::thread::id();
			}
			else
			{
				delete pB;
			}

		return;
	}

void CApplicationDlg::OnLvnItemchangedFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_pBitmap != nullptr)
	{
		delete m_pBitmap;
		m_pBitmap = nullptr;
	}
	if (m_pBitmap_effect != nullptr)
	{
		delete m_pBitmap_effect;
		m_pBitmap_effect = nullptr;
	}
	CString csFileName;
	POSITION pos = m_ctrlFileList.GetFirstSelectedItemPosition();
	if (pos)
		csFileName = m_csDirectory + m_ctrlFileList.GetItemText(m_ctrlFileList.GetNextSelectedItem(pos), 0);

	if (!csFileName.IsEmpty())
	{
		m_effect = false;
		std::thread thread(&CApplicationDlg::SingleThread, this, csFileName);
		thread.detach();
	}

	*pResult = 0;
}


void CApplicationDlg::OnLogOpen()
{
	m_ctrlLog.ShowWindow(SW_SHOW);
}


void CApplicationDlg::OnUpdateLogOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(::IsWindow(m_ctrlLog.m_hWnd) && !m_ctrlLog.IsWindowVisible());
}


void CApplicationDlg::OnLogClear()
{
	m_ctrlLog.SendMessage( CLogDlg::WM_TEXT);
}


void CApplicationDlg::OnUpdateLogClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(::IsWindow(m_ctrlLog.m_hWnd) && m_ctrlLog.IsWindowVisible());
}

void CApplicationDlg::OnHistogramRed()
{
	m_bHistRed = ! m_bHistRed;
	/*m_bHistGreen = false;
	m_bHistBlue = false;
	m_bHistJas = false;*/
	Invalidate();
}


void CApplicationDlg::OnUpdateHistogramRed(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pBitmap != NULL);
	if (m_bHistRed)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}


void CApplicationDlg::OnHistogramGreen()
{
	m_bHistGreen = !m_bHistGreen;
	/*m_bHistRed = false;
	m_bHistBlue = false;
	m_bHistJas = false;*/
	Invalidate();
}


void CApplicationDlg::OnUpdateHistogramGreen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pBitmap != NULL);
	if (m_bHistGreen)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}


void CApplicationDlg::OnHistogramBlue()
{
	m_bHistBlue = !m_bHistBlue;
	/*m_bHistGreen = false;
	m_bHistRed = false;
	m_bHistJas = false;*/
	Invalidate();
}


void CApplicationDlg::OnUpdateHistogramBlue(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pBitmap != NULL);
	if (m_bHistBlue)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}


void CApplicationDlg::OnHistogramAlpha()
{
	m_bHistJas = !m_bHistJas;
	/*m_bHistGreen = false;
	m_bHistBlue = false;
	m_bHistRed = false;*/
	Invalidate();
}


void CApplicationDlg::OnUpdateHistogramAlpha(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pBitmap != NULL);
	if (m_bHistJas)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

template<int threshold> void CApplicationDlg::OnUpdateSolarization(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(m_pBitmap != NULL);
	if (m_threshold == threshold && m_effect)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

template<int threshold> void CApplicationDlg::OnSolarization()
{
	if (m_threshold == threshold && m_effect)
	{
		m_effect = !m_effect;
		if (m_pBitmap_effect != nullptr)
		{
			delete m_pBitmap_effect;
			m_pBitmap_effect = nullptr;
		}
	}
	else
	{
		m_threshold = threshold; 
		m_effect = true;
		if (m_pBitmap_effect != nullptr)
		{
			delete m_pBitmap_effect;
			m_pBitmap_effect = nullptr;
		}
		m_thread_id = std::this_thread::get_id();
		auto t = std::this_thread::get_id();

		Gdiplus::Rect ret(0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
		Gdiplus::BitmapData *bmpData = new Gdiplus::BitmapData();
		Gdiplus::BitmapData *bmpDataEffect = new Gdiplus::BitmapData();
		m_pBitmap_effect = m_pBitmap->Clone(ret, PixelFormat32bppRGB);

		auto yyy = m_pBitmap->LockBits(&ret, Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, bmpData);
		auto xxx = m_pBitmap_effect->LockBits(&ret, Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, bmpDataEffect);
		Utils::Solarization(m_effect, m_threshold, thread_num, bmpData->Scan0,bmpData->Stride,bmpDataEffect->Scan0,bmpDataEffect->Stride, m_pBitmap->GetHeight(), m_pBitmap->GetWidth(),m_vHistRed_effect, m_vHistGreen_effect, m_vHistBlue_effect, m_vHistJas_effect, [this, t]() {return m_thread_id != t; });
		m_pBitmap_effect->UnlockBits(bmpDataEffect);

		m_pBitmap->UnlockBits(bmpData);
		delete bmpData;
		delete bmpDataEffect;
	}

	Invalidate();
}

template<int num> void CApplicationDlg::OnUpdateThreads(CCmdUI * pCmdUI)
{
	if (thread_num == num && thread_auto == false)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

template<int num> void CApplicationDlg::OnThreads()
{
	thread_num = num;
	thread_auto = false;
	Invalidate();
}

void CApplicationDlg::OnThreadsAuto()
{
	thread_num = std::thread::hardware_concurrency();
	thread_auto = true;
	Invalidate();
}


void CApplicationDlg::OnUpdateThreadsAuto(CCmdUI *pCmdUI)
{
	if (thread_auto)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}
