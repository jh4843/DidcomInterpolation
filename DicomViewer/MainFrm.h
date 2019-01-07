
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "DicomParser.h"
#include "LLDicomDS.h"
#include "Study.h"
#include "LLDicomDS.h"

class CMainFrame : public CFrameWndEx
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	void SetStatusBarText(INT_PTR nIndex, CString strText);
	INTERPOLATION_TYPE GetInterpolationType();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CArray<CLLDicomDS, CLLDicomDS> m_aryLLDicomds;

protected:
	CArray<CStudy*, CStudy*> m_aryStudy;
	

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

	INTERPOLATION_TYPE m_eInterpolationType;

public:
	void Init();
	CStudy* GetStudy(CString strStudyID);

protected:
	void ParseDicomFile(CStringArray* aryDicomFilePath);
	BOOL AddStudyToLayoutManager();
	BOOL AddDicomDS(CLLDicomDS dsLLDicomds);
	void AddStudy(CLLDicomDS dsLLdicomDS);
	void AddSeries(CLLDicomDS dsLLdicomDS);
	void AddInstance(CLLDicomDS dsLLdicomDS);

	void FindFileInDirectory(CString strPath, CStringArray& aryPath);

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenfolder();
	afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnInterpolationBilinear();
	afx_msg void OnInterpolationBicubicPolynomial050();
	afx_msg void OnInterpolationBicubicPolynomial075();
	afx_msg void OnInterpolationBicubicPolynomial100();
	afx_msg void OnInterpolationBicubicPolynomial000();
	afx_msg void OnInterpolationBicubicPolynomial300();
	afx_msg void OnInterpolationBicubicBSpline();
	afx_msg void OnInterpolationLanczos();
	afx_msg void OnInterpolationMitchell();
	afx_msg void OnUpdateInterpolationBilinear(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationBicubicPolynomial(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationBicubicSpline(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationBicubicPolynomial075(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationBicubicPolynomial100(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationBicubicPolynomial000(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationBicubicPolynomial300(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationLanczos(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInterpolationMitchell(CCmdUI *pCmdUI);
};


