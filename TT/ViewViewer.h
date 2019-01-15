
#pragma once

class CViewViewer : public CView
{
	DECLARE_DYNCREATE(CViewViewer)

protected: // create from serialization only
	CViewViewer();
	
public:
	virtual ~CViewViewer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CDocViewer* GetDocument() const;
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	BOOL ChangeRibbonCategory(UINT nID);
	BOOL AdjustLayoutEx();

protected:
	BOOL Draw(CDC* pDC);
	BOOL SelectInvalidateRgn(CDC* pDC, BOOL bInvalidate);
	BOOL AdjustLayout();

protected:
	int m_nSubFrameViewerIndex;
	UINT m_nSelectedRibbonCategoryID;
};

#ifndef _DEBUG  // debug version in XRLinkClinicViewerView.cpp
inline CDocViewer* CViewViewer::GetDocument() const
   { return reinterpret_cast<CDocViewer*>(m_pDocument); }
#endif

