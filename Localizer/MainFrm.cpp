// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DicomViewer.h"

#include "DicomFileDialog.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_FILE_OPENFOLDER, &CMainFrame::OnFileOpenfolder)
	ON_COMMAND(ID_INTERPOLATION_BILINEAR, &CMainFrame::OnInterpolationBilinear)
	ON_COMMAND(ID_INTERPOLATION_BICUBIC_POLYNOMIAL, &CMainFrame::OnInterpolationBicubicPolynomial)
	ON_COMMAND(ID_INTERPOLATION_BICUBIC_SPLINE, &CMainFrame::OnInterpolationBicubicSpline)
	ON_UPDATE_COMMAND_UI(ID_INTERPOLATION_BILINEAR, &CMainFrame::OnUpdateInterpolationBilinear)
	ON_UPDATE_COMMAND_UI(ID_INTERPOLATION_BICUBIC_POLYNOMIAL, &CMainFrame::OnUpdateInterpolationBicubicPolynomial)
	ON_UPDATE_COMMAND_UI(ID_INTERPOLATION_BICUBIC_SPLINE, &CMainFrame::OnUpdateInterpolationBicubicSpline)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPENFOLDER);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_VIEWBYSERIESUNIT);
	lstBasicCommands.AddTail(ID_COMMAND_CLOSEALL);
	lstBasicCommands.AddTail(ID_SYNC_POSANDORIENT);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	//CMFCToolBar::SetBasicCommands(lstBasicCommands);

	m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, 200);
	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_STRETCH, 300);

	SetActiveWindow();

	return 0;
}

void CMainFrame::SetStatusBarText(INT_PTR nIndex, CString strText)
{
	m_wndStatusBar.SetPaneText(nIndex, strText);
}

INTERPOLATION_TYPE CMainFrame::GetInterpolationType()
{
	return m_eInterpolationType;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::OnFileOpenfolder()
{
	CStringArray aryFilePath;

	TCHAR szPath[_MAX_PATH];

	TCHAR szInitDir[MAX_PATH] = { 0, };

	TCHAR szTitle[MAX_PATH] = { 0, };
	_tcscpy_s(szTitle, MAX_PATH, (_T("Choose the folder which DICOMDIR file exists.")));

	LPITEMIDLIST pidl;
	BROWSEINFO bi;
	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = nullptr;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = szTitle;
	bi.ulFlags = BIF_NONEWFOLDERBUTTON | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE;
	bi.lpfn = nullptr;
	bi.lParam = (LPARAM)szInitDir;

	pidl = SHBrowseForFolder(&bi);
	if (!SHGetPathFromIDList(pidl, szPath))
	{
		return;
	}

	CString strFindPath;
	strFindPath.Format(_T("%s\\*.*"), szPath);

	FindFileInDirectory(strFindPath, aryFilePath);

	ParseDicomFile(&aryFilePath);
	AddStudyToLayoutManager();

	// TODO: Add your command handler code here
}


void CMainFrame::OnFileOpen()
{
//	CDicomFileDialog dlgDcm(TRUE);

	CString fileName;
	CFileDialog dlgDcm(TRUE);
	wchar_t* p = fileName.GetBuffer(32767);

	OPENFILENAME& ofn = dlgDcm.GetOFN();
	ofn.hwndOwner = GetSafeHwnd();
	ofn.Flags |= OFN_ALLOWMULTISELECT;
	ofn.lpstrFile = p;
	ofn.nMaxFile = 32767;

// 	dlgDcm.GetOFN().hwndOwner = GetSafeHwnd();
// 	dlgDcm.GetOFN().lpstrFilter = _T("DICOM Files (*.dcm;*.dic)\0*.dcm;*.dic\0All Files (*.*)\0*.*\0\0");
// 	dlgDcm.GetOFN().lpstrDefExt = _T("*.dcm");
// 
// 	TCHAR szFileName[32767];
// 	szFileName[0] = '\0';
// 
// 	dlgDcm.GetOFN().lpstrFile = szFileName;
// 	dlgDcm.GetOFN().nMaxFile = _countof(szFileName);
	CStringArray aryFilePath;
	
	if (dlgDcm.DoModal() == IDOK)
	{
		theApp.OnCloseAll();

		POSITION pos = dlgDcm.GetStartPosition();
		while (pos)
		{
			CString strFileName = dlgDcm.GetNextPathName(pos);
			if (!strFileName.IsEmpty())
			{
				aryFilePath.Add(strFileName);
			}
		}
	}

	ParseDicomFile(&aryFilePath);
	AddStudyToLayoutManager();

	fileName.ReleaseBuffer();

	return;
}

void CMainFrame::Init()
{
	for (INT_PTR nIndex = 0; nIndex < m_aryStudy.GetCount(); nIndex++)
	{
		CStudy* pStudy = m_aryStudy.GetAt(nIndex);
		if (pStudy)
		{
			delete pStudy;
			pStudy = nullptr;
		}
	}

	m_eInterpolationType = eBilinear;

	m_aryStudy.RemoveAll();
	m_aryLLDicomds.RemoveAll();
}

CStudy* CMainFrame::GetStudy(CString strStudyID)
{
	for (INT_PTR iStudy = 0; iStudy < m_aryStudy.GetCount(); iStudy++)
	{
		CStudy* pStudy = m_aryStudy.GetAt(iStudy);

		if (pStudy->IsSameStudyID(strStudyID) == TRUE)
		{
			return pStudy;
		}
	}

	return nullptr;
}

void CMainFrame::ParseDicomFile(CStringArray* aryDicomFilePath)
{
	CString strFilePath;
	CDicomParser dicomParser;

	for (INT_PTR i = 0; i < aryDicomFilePath->GetCount(); i++)
	{
		strFilePath = aryDicomFilePath->GetAt(i);

		if (dicomParser.LoadDS((LPTSTR)(LPCTSTR)strFilePath, 0) == DICOM_SUCCESS)
		{
			dicomParser.ParseDicomHeader();
			dicomParser.ParseImageInfo();

			CLLDicomDS dsLLDicomds = dicomParser.GetLLDicomDS();

			AddDicomDS(dsLLDicomds);

			dicomParser.m_aryDicomImage.RemoveAll();
		}
	}

	dicomParser.ResetDS();
}

BOOL CMainFrame::AddStudyToLayoutManager()
{
	BOOL bRes = FALSE;
	for (INT_PTR iStudy = 0; iStudy < m_aryStudy.GetCount(); iStudy++)
	{
		CStudy* pStudy = m_aryStudy.GetAt(iStudy);
		if (pStudy->m_arrIndexDisplayStudyViewer.GetCount() <= 0)
		{
			theApp.m_pLayoutManager->AddLoadedStudy(pStudy);
		}
		else
		{
			theApp.m_pLayoutManager->UpdateStudyToViewer(pStudy);
		}
	}
	return TRUE;
}

BOOL CMainFrame::AddDicomDS(CLLDicomDS dsLLDicomds)
{
	TRY
	{
		if (m_aryLLDicomds.GetCount() <= 0)
		{
			m_aryLLDicomds.Add(dsLLDicomds);
			AddStudy(dsLLDicomds);
			return TRUE;
		}

		BOOL bFindStudy = FALSE;
		CLLDicomDS dsStoredLLDicomDS;
		for (INT_PTR iParser = 0; iParser < m_aryLLDicomds.GetCount(); iParser++)
		{
			dsStoredLLDicomDS = m_aryLLDicomds.GetAt(iParser);
			if (dsStoredLLDicomDS.GetStudyID().CompareNoCase(dsLLDicomds.GetStudyID()) == 0)
			{
				bFindStudy = TRUE;
				break;
			}
		}

		if (bFindStudy == FALSE)
		{
			m_aryLLDicomds.Add(dsLLDicomds);
			AddStudy(dsLLDicomds);
			return TRUE;
		}

		BOOL bFindSeries = FALSE;
		for (INT_PTR iParser = 0; iParser < m_aryLLDicomds.GetCount(); iParser++)
		{
			dsStoredLLDicomDS = m_aryLLDicomds.GetAt(iParser);
			if (dsStoredLLDicomDS.GetSeriesID().CompareNoCase(dsLLDicomds.GetSeriesID()) == 0)
			{
				bFindSeries = TRUE;
				break;
			}
		}

		if (bFindSeries == FALSE)
		{
			m_aryLLDicomds.Add(dsLLDicomds);
			AddSeries(dsLLDicomds);
			return TRUE;
		}

		BOOL bFindInstance = FALSE;
		for (INT_PTR iParser = 0; iParser < m_aryLLDicomds.GetCount(); iParser++)
		{
			if (dsStoredLLDicomDS.GetInstanceID().CompareNoCase(dsLLDicomds.GetInstanceID()) == 0)
			{
				bFindInstance = TRUE;
				break;
			}
		}

		if (bFindInstance == FALSE)
		{
			m_aryLLDicomds.Add(dsLLDicomds);
			AddInstance(dsLLDicomds);
			return TRUE;
		}
	}
	CATCH_ALL(e)
	{
		if (e->IsKindOf(RUNTIME_CLASS(CMemoryException)))
		{
			AfxMessageBox(_T("MEMORY EXCEPTION"));
		}
		else
		{
			AfxMessageBox(_T("Fail"));
		}
	}
	END_CATCH_ALL
	

	return FALSE;
}

void CMainFrame::AddStudy(CLLDicomDS dsLLdicomDS)
{
	CStudy* pStudy = new CStudy(dsLLdicomDS);
	pStudy->AddSeries(dsLLdicomDS);
	m_aryStudy.Add(pStudy);
}

void CMainFrame::AddSeries(CLLDicomDS dsLLdicomDS)
{
	BOOL bExist = FALSE;

	CStudy* pStudyInfo = nullptr;

	for (INT_PTR iArray = 0; iArray < m_aryStudy.GetCount(); iArray++)
	{
		pStudyInfo = m_aryStudy.GetAt(iArray);
		if (pStudyInfo->IsSameStudyID(dsLLdicomDS.GetStudyID()) == TRUE)
		{
			bExist = TRUE;
			break;
		}
	}

	if (bExist == FALSE)
	{
		AfxMessageBox(_T("No Study to add series"));
		return;
	}

	pStudyInfo->AddSeries(dsLLdicomDS);
}

void CMainFrame::AddInstance(CLLDicomDS dsLLdicomDS)
{
	BOOL bExist = FALSE;

	CStudy* pStudyInfo = nullptr;

	for (INT_PTR iArray = 0; iArray < m_aryStudy.GetCount(); iArray++)
	{
		pStudyInfo = m_aryStudy.GetAt(iArray);
		if (pStudyInfo->IsSameStudyID(dsLLdicomDS.GetStudyID()) == TRUE)
		{
			bExist = TRUE;
			break;
		}
	}

	if (bExist == FALSE)
	{
		AfxMessageBox(_T("No Study to add series"));
		return;
	}

	bExist = FALSE;
	CSeries* pSeriesInfo = nullptr;

	for (INT_PTR iArray = 0; iArray < pStudyInfo->GetSeriesArray()->GetCount(); iArray++)
	{
		pSeriesInfo = pStudyInfo->GetSeriesArray()->GetAt(iArray);
		if (pSeriesInfo->IsSameSeriesID(dsLLdicomDS.GetSeriesID()) == TRUE)
		{
			bExist = TRUE;
			break;
		}
	}

	if (bExist == FALSE)
	{
		AfxMessageBox(_T("No Series to add series"));
		return;
	}

	pSeriesInfo->AddInstance(dsLLdicomDS);
}

void CMainFrame::FindFileInDirectory(CString strPath, CStringArray& aryPath)
{
	CFileFind fileFinder;
	CString strFileName;

	if (fileFinder.FindFile(strPath) == FALSE)
	{
		return;
	}

	while (fileFinder.FindNextFileW())
	{
		if (fileFinder.IsDots())
			continue;

		if (fileFinder.IsDirectory())
		{
			FindFileInDirectory(fileFinder.GetFilePath() + _T("\\*.*"), aryPath);
			continue;
		}

		strFileName = fileFinder.GetFileName();
		if (strFileName.Right(3) != _T("dcm") &&
			strFileName.Find('.') != -1)
		{
			continue;
		}

		aryPath.Add(fileFinder.GetFilePath());
	}

	fileFinder.Close();
}


void CMainFrame::OnInterpolationBilinear()
{
	m_eInterpolationType = eBilinear;
}


void CMainFrame::OnInterpolationBicubicPolynomial()
{
	m_eInterpolationType = eBicubicPolynomial;
}


void CMainFrame::OnInterpolationBicubicSpline()
{
	m_eInterpolationType = eBicubicSpline;
}


void CMainFrame::OnUpdateInterpolationBilinear(CCmdUI *pCmdUI)
{
	if (m_eInterpolationType == eBilinear)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
	
	// TODO: Add your command update UI handler code here
}


void CMainFrame::OnUpdateInterpolationBicubicPolynomial(CCmdUI *pCmdUI)
{
	CMenu *hMenu = GetMenu();

	if (m_eInterpolationType == eBicubicPolynomial)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnUpdateInterpolationBicubicSpline(CCmdUI *pCmdUI)
{
	CMenu *hMenu = GetMenu();

	if (m_eInterpolationType == eBicubicSpline)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}
