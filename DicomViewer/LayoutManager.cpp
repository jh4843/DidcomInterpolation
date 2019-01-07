#include "stdafx.h"
#include "LayoutManager.h"

CLayoutManager::CLayoutManager()
{
	m_nViewerCount = 0;
	m_nSelectedViewerIndex = -1;
	m_nLayoutMargin = 5;

	m_bSyncPosAndOrientation = FALSE;
}


CLayoutManager::~CLayoutManager()
{
}

void CLayoutManager::Init()
{
	for (INT_PTR iStudyViewer = 0; iStudyViewer < m_aryStudyViewer.GetCount(); iStudyViewer++)
	{
		CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);
		pStudyViewer->Init();
		delete pStudyViewer;
	}
	m_aryStudyViewer.RemoveAll();
	m_nViewerCount = 0;

	for (INT_PTR iLayout = 0; iLayout < DEFAULT_LAYOUT_COUNT; iLayout++)
	{
		AddStudyViewer(iLayout, this);
	}

	for (INT_PTR iStudyViewer = 0; iStudyViewer < m_aryStudyViewer.GetCount(); iStudyViewer++)
	{
		CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);
		pStudyViewer->Init();
		pStudyViewer->SetViewAllSeries();
	}

	m_nSelectedViewerIndex = -1;
	m_aryLoadedStudy.RemoveAll();
}

BOOL CLayoutManager::AddStudyViewer(int nLayoutIndex, CWnd* pParent)
{
	BOOL res = FALSE;

	CStudyViewer* pStudyViewer = new CStudyViewer(nLayoutIndex);
	
	pStudyViewer->CreateCtrl(pParent);
	m_aryStudyViewer.Add(pStudyViewer);
	
	m_nViewerCount++;

	return res;
}

BOOL CLayoutManager::AddLoadedStudy(CStudy *pStudy)
{
	m_aryLoadedStudy.Add(pStudy);

	BOOL bResSet = SetStudyToEmptyStudyViewer(pStudy);

	return bResSet;
}

BOOL CLayoutManager::UpdateStudyToViewer(CStudy *pStudy)
{
	BOOL bFindStudy = FALSE;

	for (INT_PTR iStudy = 0; iStudy < m_aryLoadedStudy.GetCount(); iStudy++)
	{
		CStudy* pLoadedStudy = m_aryLoadedStudy.GetAt(iStudy);
		if (pLoadedStudy->IsSameStudyID(pStudy->GetStudyId()))
		{
			pLoadedStudy = pStudy;

			for (INT_PTR iViewer = 0; iViewer < pLoadedStudy->m_arrIndexDisplayStudyViewer.GetCount(); iViewer++)
			{
				INT_PTR nViewerIndex = pLoadedStudy->m_arrIndexDisplayStudyViewer.GetAt(iViewer);
				CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(nViewerIndex);
				pStudyViewer->SetStudy(pStudy);
			}
			
			bFindStudy = TRUE;
		}
	}

	return bFindStudy;
}

INT_PTR CLayoutManager::GetStudyViewerCount()
{
	return m_nViewerCount;
}

CStudyViewer * CLayoutManager::GetCurStudyViewer()
{
	if (m_nSelectedViewerIndex < 0 || m_aryStudyViewer.GetCount() <= 0)
		return nullptr;

	return m_aryStudyViewer.GetAt(m_nSelectedViewerIndex);
}

void CLayoutManager::SetSelectedStudyViewer(INT_PTR nSelectedViewerIndex)
{
	m_nSelectedViewerIndex = nSelectedViewerIndex;

	if (m_bSyncPosAndOrientation)
	{
		CStudyViewer* pSrcStudyViewer = m_aryStudyViewer.GetAt(m_nSelectedViewerIndex);
		CStudy* pSrcStudy = pSrcStudyViewer->GetStudy();

		CLLDicomDS* pSrcDicomDS = pSrcStudyViewer->GetDisplayingDicomDS();

		for (INT_PTR i = 0; i < pSrcStudy->m_arrIndexDisplayStudyViewer.GetCount(); i++)
		{
			CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(i);
			if (pStudyViewer->GetStudy()->IsSameStudyID(pSrcStudy->GetStudyId()) == FALSE)
				continue;

			if (pStudyViewer->m_nCurSeriesIndex == pSrcStudyViewer->m_nCurSeriesIndex)
				continue;

			CArray<CSeries*, CSeries*>* pArySeries = pStudyViewer->GetStudy()->GetSeriesArray();
			CSeries* pSeries = pArySeries->GetAt(pStudyViewer->m_nCurSeriesIndex);
			
			for (INT_PTR iInstance = 0; iInstance < pSeries->GetInstanceArray()->GetCount(); iInstance++)
			{
				CInstance* pInstance = pSeries->GetInstanceArray()->GetAt(iInstance);
				if (pInstance->GetDicomDS()->IsSamePosAndOrient(*pSrcDicomDS))
				{
					pStudyViewer->SetCurrentInstanceIndex(iInstance);
				}
			}
		}
	}

	for (INT_PTR iStudyViewer = 0; iStudyViewer < m_aryStudyViewer.GetCount(); iStudyViewer++)
	{
		CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);
		pStudyViewer->RedrawWnd();
	}
}

BOOL CLayoutManager::SetStudyToEmptyStudyViewer(CStudy* pStudy)
{
	if (m_aryStudyViewer.GetCount() <= 0)
		return FALSE;

	for (INT_PTR iStudyViewer = 0; iStudyViewer < m_aryStudyViewer.GetCount(); iStudyViewer++)
	{
		CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);
		if (pStudyViewer->GetStudy() == nullptr)
		{
			pStudyViewer->Init();
			pStudyViewer->SetStudy(pStudy);
			pStudy->AddStudyViewerIndex(iStudyViewer);
			RedrawStudyViewer(iStudyViewer);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CLayoutManager::SetStudyToSelectedStudyViewer(CStudy* pStudy, INT_PTR nViewIndex, INT_PTR nCurSeriesIndex, INT_PTR nCurInstanceIndex)
{
	if (m_aryStudyViewer.GetCount() <= 0)
		return FALSE;

	CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(nViewIndex);
	pStudyViewer->Init(nCurSeriesIndex, nCurInstanceIndex);
	pStudyViewer->SetStudy(pStudy);
	pStudyViewer->RedrawWnd();

	pStudy->AddStudyViewerIndex(nViewIndex);

	return TRUE;
}

void CLayoutManager::SetViewerBySeriesUnit()
{
	if (m_nSelectedViewerIndex < 0)
		return;

	CStudyViewer* pSelectedStudyViewer = m_aryStudyViewer.GetAt(m_nSelectedViewerIndex);
	CStudy* pStudy = pSelectedStudyViewer->GetStudy();

	INT_PTR nSeriesCount = pStudy->GetSeriesArray()->GetCount();

	if (nSeriesCount > MAX_LAYOUT_COUNT)
	{
		CString strMsg;
		strMsg.Format(_T("Max Viewer Count : %d, Cant display the series count(%d)"), MAX_LAYOUT_COUNT, nSeriesCount);
		nSeriesCount = MAX_LAYOUT_COUNT;
	}

	// Need to Add viewer
	if (nSeriesCount > m_nViewerCount)
	{
		INT_PTR nAddViewerCount = GetNeedToAddViewerCount(nSeriesCount - m_nViewerCount);
		for (INT_PTR iAddViewer = 0; iAddViewer < nAddViewerCount; iAddViewer++)
		{
			AddStudyViewer(m_nViewerCount, this);
		}
	}

	for (INT_PTR iStudyViewer = 0; iStudyViewer < nSeriesCount; iStudyViewer++)
	{
		INT_PTR nSeriesIndex = iStudyViewer;
		SetStudyToSelectedStudyViewer(pStudy, iStudyViewer, nSeriesIndex, 0);

		CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);
		pStudyViewer->SetViewOnlySameSeries();
	}

	RedrawWnd();
}

void CLayoutManager::SetSyncPosAndOrientation(BOOL bSync)
{
	m_bSyncPosAndOrientation = bSync;

	RedrawWnd();
}

BOOL CLayoutManager::Draw(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	CMemDC memDC(*pDC, this);
	pDC = &memDC.GetDC();

	CRect rtClient;
	GetClientRect(&rtClient);
	
	pDC->FillSolidRect(&rtClient, RGB(250, 180, 180));

	return TRUE;
}

void CLayoutManager::RedrawWnd()
{
	Invalidate(FALSE);
	UpdateWindow();
}

BOOL CLayoutManager::RedrawStudyViewer(INT_PTR nViewerIndex)
{
	CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(nViewerIndex);

	pStudyViewer->RedrawWnd();

	return TRUE;
}

void CLayoutManager::CalcLayout()
{
	CRect rtClient;
	GetClientRect(&rtClient);

	if (m_nViewerCount <= 0)
		return;

	INT_PTR nTotalRowCount, nTotalColCount;
	nTotalRowCount = nTotalColCount = sqrt(m_nViewerCount);

	INT_PTR nDividWidth = rtClient.Width() / nTotalColCount;
	INT_PTR nDividHeight = rtClient.Height() / nTotalRowCount;

	INT_PTR nViewerWidth = nDividWidth - m_nLayoutMargin;
	INT_PTR nViewerHeight = nDividHeight - m_nLayoutMargin;

	CStudyViewer* pStudyViewer = nullptr;
	INT_PTR nRow = 0;
	INT_PTR nCol = 0;

	for (INT_PTR iStudyViewer = 0; iStudyViewer < m_aryStudyViewer.GetCount(); iStudyViewer++)
	{
		pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);

		pStudyViewer->SetWindowPos(NULL,
			rtClient.left + (nDividWidth + m_nLayoutMargin) * nCol,
			rtClient.top + (nDividHeight + m_nLayoutMargin) * nRow,
			nViewerWidth,
			nViewerHeight,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);

		nCol++;
		if (nCol >= nTotalColCount)
		{
			nCol = 0;
			nRow++;
		}
	}
}

INT_PTR CLayoutManager::GetNeedToAddViewerCount(INT_PTR nMinAddCount)
{
	INT_PTR nResCount = DEFAULT_LAYOUT_COUNT;
	INT_PTR nTotalMinViewCount = nMinAddCount + m_nViewerCount;

	if (nTotalMinViewCount > 4 && nTotalMinViewCount <= 9)
	{
		nResCount = 9;
	}
	else if (nTotalMinViewCount > 9 && nTotalMinViewCount <= 16)
	{
		nResCount = 16;
	}
	else if (nTotalMinViewCount > 16)
	{
		nResCount = MAX_LAYOUT_COUNT;
	}

	nResCount -= m_nViewerCount;

	return nResCount;
}

BEGIN_MESSAGE_MAP(CLayoutManager, CWnd)
ON_WM_SIZE()
ON_WM_PAINT()
ON_WM_CREATE()
END_MESSAGE_MAP()


void CLayoutManager::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CalcLayout();
	// TODO: Add your message handler code here
}


void CLayoutManager::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CWnd::OnPaint() for painting messages

	CalcLayout();
	Draw(&dc);
}


int CLayoutManager::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();

	// TODO:  Add your specialized creation code here
	return 0;
}
