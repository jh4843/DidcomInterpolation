
// Localizer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Localizer.h"
#include "MainFrm.h"

#include "LocalizerDoc.h"
#include "LocalizerView.h"
#include "StudyViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLocalizerApp

BEGIN_MESSAGE_MAP(CLocalizerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CLocalizerApp::OnAppAbout)
	ON_COMMAND(ID_COMMAND_CLOSEALL, &CLocalizerApp::OnCloseAll)
	ON_COMMAND(ID_VIEW_VIEWBYSERIESUNIT, &CLocalizerApp::OnViewBySeriesUnit)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_SYNC_POSANDORIENT, &CLocalizerApp::OnSyncPosandorient)
	ON_UPDATE_COMMAND_UI(ID_SYNC_POSANDORIENT, &CLocalizerApp::OnUpdateSyncPosandorient)
END_MESSAGE_MAP()


// CLocalizerApp construction

CLocalizerApp::CLocalizerApp()
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Localizer.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CLocalizerApp object

CLocalizerApp theApp;


// CLocalizerApp initialization

BOOL CLocalizerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Lead LeadTools Settings
	LBase::LoadLibraries(LT_ALL_LEADLIB);
	WRPUNLOCKSUPPORT();
	LSettings::UnlockSupport(L_SUPPORT_MEDICAL, _T("ZhyFRnk3sY"));
	LSettings::UnlockSupport(L_SUPPORT_MEDICAL_NET, _T("b4nBinY7tv"));
	LSettings::UnlockSupport(L_SUPPORT_MEDICAL_SERVER, _T("QbXwuZxA3h"));

	L_SSIZE_T nMaxConvSize = 0;
	// Get only the max conventional size value
	LMemoryFile::GetMemoryThresholds(NULL, &nMaxConvSize, NULL, NULL, NULL);
	// Double the max conventional size value
	L_INT nMemRet = LMemoryFile::SetMemoryThresholds(0, (L_INT)nMaxConvSize * 2, 0, 0, 0, MEM_MAXCONVSIZE);
	if (nMemRet != SUCCESS)
	{
		AfxMessageBox(_T("Faie to set memory"));
	}

	LSettings LeadSettings;
	if (LeadSettings.IsSupportLocked(L_SUPPORT_MEDICAL))
	{
		AfxMessageBox(_T("Check Medical License"));
	}

	if (LeadSettings.IsSupportLocked(L_SUPPORT_MEDICAL_NET))
	{
		AfxMessageBox(_T("Check Medical Net License"));
	}

	if (LeadSettings.IsSupportLocked(L_SUPPORT_MEDICAL_SERVER))
	{
		AfxMessageBox(_T("Check Medical Net Server License"));
	}

	Sleep(1000);

	GdiplusStartupInput gpsi;
	if (::GdiplusStartup(&m_nGdiPlusToken, &gpsi, NULL) != Ok)
	{
		AfxMessageBox(_T("Failed to initialize GDI+."), MB_ICONERROR);
		return FALSE;
	}


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CLocalizerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLocalizerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	m_bSyncPosAndOrient = FALSE;

	return TRUE;
}

int CLocalizerApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	LBase::UnloadLibraries(LT_ALL_LEADLIB);
	AfxOleTerm(FALSE);

	//
	::GdiplusShutdown(m_nGdiPlusToken);

	return CWinAppEx::ExitInstance();
}

// CLocalizerApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CLocalizerApp message handlers

void CLocalizerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CLocalizerApp::LoadCustomState()
{
}

void CLocalizerApp::SaveCustomState()
{
}

void CLocalizerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CLocalizerApp::OnCloseAll()
{
	m_pLayoutManager->Init();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->Init();
}

void CLocalizerApp::OnViewBySeriesUnit()
{
	m_pLayoutManager->SetViewerBySeriesUnit();
}

void CLocalizerApp::OnSyncPosandorient()
{
	CMenu menu;
	menu.LoadMenuW(IDR_MAINFRAME);

	if (m_bSyncPosAndOrient)
	{
		m_bSyncPosAndOrient = FALSE;
		menu.CheckMenuItem(ID_SYNC_POSANDORIENT, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
		m_bSyncPosAndOrient = TRUE;
		menu.CheckMenuItem(ID_SYNC_POSANDORIENT, MF_BYCOMMAND | MF_CHECKED);
	}

	m_pLayoutManager->SetSyncPosAndOrientation(m_bSyncPosAndOrient);

}


void CLocalizerApp::OnUpdateSyncPosandorient(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSyncPosAndOrient);
}
