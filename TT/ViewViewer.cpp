#include "stdafx.h"
#include "QXLinkClinicViewerApp.h"
#include "DocViewer.h"
#include "ViewViewer.h"
#include "MyVisualManagerWindows7Ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace QXLINK_CLINIC_TABLE::VIEWER;
using namespace QXLINK_CLINIC_TABLE::VIEWER_CATEGORY;
using namespace QXLINK_CLINIC_TABLE::VIEWER_CATEGORY::CATEGORY_VIEWER;
using namespace QXLINK_CLINIC_TABLE::VIEWER_CATEGORY::CATEGORY_PRINT;
using namespace QXLINK_CLINIC_TABLE::VIEWER_CATEGORY::CATEGORY_STITCH;


IMPLEMENT_DYNCREATE(CViewViewer, CView)
	
CViewViewer::CViewViewer()
{
	m_nSubFrameViewerIndex = -1;
	m_nSelectedRibbonCategoryID = ID_CATEGORY_VIEWER;
}

CViewViewer::~CViewViewer()
{
}

#ifdef _DEBUG
void CViewViewer::AssertValid() const
{
	CView::AssertValid();
}

void CViewViewer::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDocViewer* CViewViewer::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocViewer)));
	return (CDocViewer*)m_pDocument;
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CViewViewer, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CViewViewer::OnDraw(CDC* /*pDC*/)
{
	CDocViewer* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}

void CViewViewer::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CDocViewer* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_nSubFrameViewerIndex = pDoc->m_nSubFrameViewerIndex;

	if (!theApp.m_ViewerManager.AddUIStudyViewerManager(m_nSubFrameViewerIndex, this))
	{
		AfxDebugBreak();
	}

	if (!theApp.m_ViewerManager.AddUIStudyPrintManager(m_nSubFrameViewerIndex, this))
	{
		AfxDebugBreak();
	}

	if (!theApp.m_ViewerManager.AddUIStudyStitchManager(m_nSubFrameViewerIndex, this))
	{
		AfxDebugBreak();
	}
}

int CViewViewer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

 	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	//
//	m_hBitBk = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_NEW_BK));

	return 0;
}

void CViewViewer::OnDestroy()
{
	theApp.m_ViewerManager.RemoveAt(m_nSubFrameViewerIndex);

	CView::OnDestroy();
}

BOOL CViewViewer::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CViewViewer::OnPaint()
{
	CPaintDC dc(this);
	SelectInvalidateRgn(&dc, TRUE);

	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();
	Draw(pDC);

	return;
}

void CViewViewer::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	//
	AdjustLayout();

}

BOOL CViewViewer::AdjustLayoutEx()
{
	return AdjustLayout();
}

BOOL CViewViewer::AdjustLayout()
{
	if (m_nSubFrameViewerIndex == -1)
	{
		return FALSE;
	}

	if (GetSafeHwnd() == nullptr)
	{
		return FALSE;
	}

	CRect rcClient;
	GetClientRect(rcClient);

	if (rcClient.IsRectEmpty())
	{
		return TRUE;
	}
// 	if (!rcClient.Width() || !rcClient.Height())
// 	{
// 		return TRUE;
// 	}

	switch (m_nSelectedRibbonCategoryID)
	{
	case ID_CATEGORY_VIEWER:		// viewer
		{
			CUIStudyViewerManager* pUIStudyViewerManager = theApp.m_ViewerManager.GetUIStudyViewerManager(m_nSubFrameViewerIndex);
			if (pUIStudyViewerManager && pUIStudyViewerManager->GetSafeHwnd())
			{
				pUIStudyViewerManager->SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
			}

			CUIStudyPrintManager* pUIStudyPrintManager = theApp.m_ViewerManager.GetUIStudyPrintManager(m_nSubFrameViewerIndex);
			if (pUIStudyPrintManager && pUIStudyPrintManager->GetSafeHwnd())
			{
				pUIStudyPrintManager->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_HIDEWINDOW);
			}

			CUIStudyStitchManager* pUIStudyStitchManager = theApp.m_ViewerManager.GetUIStudyStitchManager(m_nSubFrameViewerIndex);
			if (pUIStudyStitchManager && pUIStudyStitchManager->GetSafeHwnd())
			{
				pUIStudyStitchManager->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_HIDEWINDOW);
			}
		}
		break;
	case ID_CATEGORY_PRINT:		// print
		{	
			CUIStudyPrintManager* pUIStudyPrintManager = theApp.m_ViewerManager.GetUIStudyPrintManager(m_nSubFrameViewerIndex);
			if (pUIStudyPrintManager && pUIStudyPrintManager->GetSafeHwnd())
			{
				pUIStudyPrintManager->SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
			}

			CUIStudyViewerManager* pUIStudyViewerManager = theApp.m_ViewerManager.GetUIStudyViewerManager(m_nSubFrameViewerIndex);
			if (pUIStudyViewerManager && pUIStudyViewerManager->GetSafeHwnd())
			{
				pUIStudyViewerManager->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_HIDEWINDOW);
			}

			CUIStudyStitchManager* pUIStudyStitchManager = theApp.m_ViewerManager.GetUIStudyStitchManager(m_nSubFrameViewerIndex);
			if (pUIStudyStitchManager && pUIStudyStitchManager->GetSafeHwnd())
			{
				pUIStudyStitchManager->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_HIDEWINDOW);
			}
		}
		break;
	case ID_CATEGORY_STITCH:		// stitch
		{	
			CUIStudyStitchManager* pUIStudyStitchManager = theApp.m_ViewerManager.GetUIStudyStitchManager(m_nSubFrameViewerIndex);
			if (pUIStudyStitchManager && pUIStudyStitchManager->GetSafeHwnd())
			{
				pUIStudyStitchManager->SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
			}

			CUIStudyViewerManager* pUIStudyViewerManager = theApp.m_ViewerManager.GetUIStudyViewerManager(m_nSubFrameViewerIndex);
			if (pUIStudyViewerManager && pUIStudyViewerManager->GetSafeHwnd())
			{
				pUIStudyViewerManager->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_HIDEWINDOW);
			}

			CUIStudyPrintManager* pUIStudyPrintManager = theApp.m_ViewerManager.GetUIStudyPrintManager(m_nSubFrameViewerIndex);
			if (pUIStudyPrintManager && pUIStudyPrintManager->GetSafeHwnd())
			{
				pUIStudyPrintManager->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOZORDER|SWP_HIDEWINDOW);
			}
		}
		break;
	}

	return TRUE;
}

BOOL CViewViewer::Draw(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	CRect rcClient;
	GetClientRect(rcClient);

	CMyVisualManagerWindows7Ex* pMyVisualManagerWindows7Ex = dynamic_cast<CMyVisualManagerWindows7Ex*>(CMFCVisualManager::GetInstance());
	if (pMyVisualManagerWindows7Ex)
	{
		//pDC->FillSolidRect(&rcClient, RGB(255,0,0));
		pDC->FillSolidRect(&rcClient, pMyVisualManagerWindows7Ex->GetRibbonBarGradientDarkColor());
	}
	else
	{
		pDC->FillSolidRect(&rcClient, RGB(0,0,0));
	}

	return TRUE;
}

BOOL CViewViewer::SelectInvalidateRgn(CDC* pDC, BOOL bInvalidate)
{
	if (!pDC)
	{
		return FALSE;
	}

	pDC->SelectClipRgn(NULL);


	CUIStudyBaseManager* pCurUIStudyBaseManager = nullptr;

	switch (m_nSelectedRibbonCategoryID)
	{
	case ID_CATEGORY_VIEWER:
	case ID_CATEGORY_VIEWER_ANNOTATION:
	case ID_CATEGORY_VIEWER_MEASUREMENT:
	case ID_CATEGORY_VIEWER_VETERINARY_MEASUREMENT:
		{
			pCurUIStudyBaseManager = dynamic_cast<CUIStudyBaseManager*>(theApp.m_ViewerManager.GetUIStudyViewerManager(m_nSubFrameViewerIndex));
		}
		break;
	case ID_CATEGORY_PRINT:
		{
			pCurUIStudyBaseManager = dynamic_cast<CUIStudyBaseManager*>(theApp.m_ViewerManager.GetUIStudyPrintManager(m_nSubFrameViewerIndex));
		}
		break;
	case ID_CATEGORY_STITCH:
		{
			pCurUIStudyBaseManager = dynamic_cast<CUIStudyBaseManager*>(theApp.m_ViewerManager.GetUIStudyStitchManager(m_nSubFrameViewerIndex));
		}
		break;
	}

	if (pCurUIStudyBaseManager && pCurUIStudyBaseManager->GetSafeHwnd())
	{
		CRect rectTemp;
		CRgn rgnTemp;
		if (bInvalidate)
		{
			pCurUIStudyBaseManager->Invalidate(FALSE);
		}
		else
		{
			pCurUIStudyBaseManager->UpdateWindow();
		}
		pCurUIStudyBaseManager->GetWindowRect(&rectTemp);
		ScreenToClient(&rectTemp);
		rgnTemp.CreateRectRgn(rectTemp.left, rectTemp.top, rectTemp.right, rectTemp.bottom);
		pDC->SelectClipRgn(&rgnTemp, RGN_DIFF);
		rgnTemp.DeleteObject();
	}

	return TRUE;
}

BOOL CViewViewer::ChangeRibbonCategory(UINT nID)
{
	if (m_nSelectedRibbonCategoryID == nID)
		return TRUE;

	m_nSelectedRibbonCategoryID = nID;
	AdjustLayout();
	return TRUE;
}
