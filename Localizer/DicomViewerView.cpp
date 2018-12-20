
// LocalizerView.cpp : implementation of the CLocalizerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DicomViewer.h"
#endif

#include "DicomViewerDoc.h"
#include "DicomViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLocalizerView

IMPLEMENT_DYNCREATE(CDicomViewerView, CView)

BEGIN_MESSAGE_MAP(CDicomViewerView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CLocalizerView construction/destruction

CDicomViewerView::CDicomViewerView()
{
	// TODO: add construction code here
	
}

CDicomViewerView::~CDicomViewerView()
{
}

BOOL CDicomViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLocalizerView drawing

void CDicomViewerView::OnDraw(CDC* /*pDC*/)
{
	CDicomViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CDicomViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDicomViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CLocalizerView diagnostics

#ifdef _DEBUG
void CDicomViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CDicomViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDicomViewerDoc* CDicomViewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDicomViewerDoc)));
	return (CDicomViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CLocalizerView message handlers


void CDicomViewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	theApp.m_pLayoutManager = new CLayoutManager();
	theApp.m_pLayoutManager->Create(NULL, _T("UIStudyViewer"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_LAYOUT_MANAGER);

	

	// TODO: Add your specialized code here and/or call the base class
}


void CDicomViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rtClient;
	GetClientRect(&rtClient);

	CLayoutManager* pLayoutManager = theApp.m_pLayoutManager;

	if (pLayoutManager == nullptr)
		return;

	pLayoutManager->SetWindowPos(NULL, rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);

	// TODO: Add your message handler code here
}


void CDicomViewerView::OnDestroy()
{
	CView::OnDestroy();

	delete theApp.m_pLayoutManager;

	// TODO: Add your message handler code here
}
