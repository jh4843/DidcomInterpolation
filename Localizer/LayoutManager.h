#pragma once
#include "afxwin.h"
#include "StudyViewer.h"
#include "Study.h"

class CLayoutManager :	public CWnd
{
	friend class CStudyViewer;

public:
	CLayoutManager();
	~CLayoutManager();

protected:
	INT_PTR m_nSelectedViewerIndex;
	CArray<CStudyViewer*, CStudyViewer*> m_aryStudyViewer;
	CWnd* m_pParent;

private:
	INT_PTR m_nLayoutMargin;
	INT_PTR m_nViewerCount;
	CArray<CStudy*, CStudy*> m_aryLoadedStudy;
	
	BOOL m_bSyncPosAndOrientation;

public:
	void Init();
	BOOL AddStudyViewer(int nLayoutIndex, CWnd* pParent);
	BOOL AddLoadedStudy(CStudy* pStudy);
	BOOL UpdateStudyToViewer(CStudy* pStudy);
	INT_PTR GetStudyViewerCount();
	CStudyViewer* GetCurStudyViewer();
	
	void SetSelectedStudyViewer(INT_PTR nSelectedViewerIndex);
	BOOL SetStudyToEmptyStudyViewer(CStudy* pStudy);		// True : Success to set, Fail : Not empty study viewer
	BOOL SetStudyToSelectedStudyViewer(CStudy* pStudy, INT_PTR nViewIndex, INT_PTR nCurSeriesIndex = 0, INT_PTR nCurInstanceIndex = 0);		// True : Success to set, Fail : Not empty study viewer
	void SetViewerBySeriesUnit();
	void SetSyncPosAndOrientation(BOOL bSync);

	void RedrawWnd();
	BOOL RedrawStudyViewer(INT_PTR nViewerIndex);

protected:
	void CalcLayout();

private:
	INT_PTR GetNeedToAddViewerCount(INT_PTR nMinAddCount);

protected:
	virtual BOOL Draw(CDC* pDC);
	//virtual BOOL SelectInvalidateRgn(CDC* pDC);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

