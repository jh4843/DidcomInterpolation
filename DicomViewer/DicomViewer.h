
// DicomViewer.h : main header file for the Localizer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "LayoutManager.h"
#include "XRLink.Clinic.MemoryDump.h"


// CLocalizerApp:
// See Localizer.cpp for the implementation of this class
//

class CLayoutManager;
class CDicomViewerApp : public CWinAppEx
{
public:
	CDicomViewerApp();

protected:
	CXRLinkClinicMemoryDump m_MiniDump;

private:
	ULONG_PTR m_nGdiPlusToken;
	BOOL m_bSyncPosAndOrient;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

public:
	CLayoutManager* m_pLayoutManager;

	afx_msg void OnAppAbout();
	afx_msg void OnCloseAll();
	afx_msg void OnViewBySeriesUnit();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSyncPosandorient();
	afx_msg void OnUpdateSyncPosandorient(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CDicomViewerApp theApp;
