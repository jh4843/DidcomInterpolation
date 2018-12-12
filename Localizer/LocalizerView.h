
// LocalizerView.h : interface of the CLocalizerView class
//

#pragma once

class CLocalizerView : public CView
{
protected: // create from serialization only
	CLocalizerView();
	DECLARE_DYNCREATE(CLocalizerView)

// Attributes
public:
	CLocalizerDoc* GetDocument() const;

// Operations


// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	

// Implementation
public:
	virtual ~CLocalizerView();
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
inline CLocalizerDoc* CLocalizerView::GetDocument() const
   { return reinterpret_cast<CLocalizerDoc*>(m_pDocument); }
#endif

