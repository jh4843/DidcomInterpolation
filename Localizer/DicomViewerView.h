
// CDicomViewerView.h : interface of the CLocalizerView class
//

#pragma once

class CDicomViewerView : public CView
{
protected: // create from serialization only
	CDicomViewerView();
	DECLARE_DYNCREATE(CDicomViewerView)

// Attributes
public:
	CDicomViewerDoc* GetDocument() const;

// Operations


// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	

// Implementation
public:
	virtual ~CDicomViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};

#ifndef _DEBUG  // debug version in LocalizerView.cpp
inline CDicomViewerDoc* CDicomViewerView::GetDocument() const
   { return reinterpret_cast<CDicomViewerDoc*>(m_pDocument); }
#endif

