#include "stdafx.h"
#include "OleDropTargetEx.h"
#include "MainFrm.h"


CString COleDropTargetEx::m_strStudyViewInfoPrefix = _T("STUDYVIEW:");
extern CDicomViewerApp theApp;

COleDropTargetEx::COleDropTargetEx(CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
}


COleDropTargetEx::~COleDropTargetEx()
{
}


CStudy* COleDropTargetEx::ParseStudyViewerInfo(CString strData, INT_PTR& nCurSeriesIndex, INT_PTR& nCurInstanceIndex)
{
	CStudy* pStudy;
	if (strData.Find(m_strStudyViewInfoPrefix) != -1)
	{
		strData.Replace(m_strStudyViewInfoPrefix, _T(""));

		CStringArray datas;
		CString Sep;
		Sep.Format(_T(","));
		CString data = strData;
		int Position = 0;
		CString Token;

		Token = data.Tokenize(Sep, Position);
		if (Token != L"") datas.Add(Token);

		while (!Token.IsEmpty())
		{
			// Get next token.
			Token = data.Tokenize(Sep, Position);
			if (Token != L"")
			{
				datas.Add(Token);
			}
		}
		
		int nPos = strData.Find(_T(",,"));
		CString strStudyID = datas.GetAt(0);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pStudy = pFrame->GetStudy(strStudyID);

		nCurSeriesIndex = _ttoi((LPCTSTR)datas.GetAt(1));
		nCurInstanceIndex = _ttoi((LPCTSTR)datas.GetAt(2));
	}

	return pStudy;
}

DROPEFFECT COleDropTargetEx::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return OnDragOver(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT COleDropTargetEx::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dropEffect = DROPEFFECT_NONE;
	//
	INT_PTR nViewerIndex = -1;
	INT_PTR nViewerPos = -1;

	if (!pDataObject->IsDataAvailable(CF_TEXT))
	{
		return dropEffect;
	}

	if (m_pParentWnd->IsKindOf(RUNTIME_CLASS(CStudyViewer)))
	{
		dropEffect = DROPEFFECT_MOVE;
	}

	//return dropEffect;

	//

	HGLOBAL hGlobal = pDataObject->GetGlobalData(CF_TEXT);
	CHAR* pText = (CHAR*)::GlobalLock(hGlobal);

	if (!AfxIsValidString(pText))
	{
		return dropEffect;
	}

	return dropEffect;
}

BOOL COleDropTargetEx::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	BOOL bSuccess = FALSE;

	INT_PTR nViewerIndex = -1;
	INT_PTR nViewerPos = -1;

	if (!pDataObject->IsDataAvailable(CF_TEXT))
	{
		return bSuccess;
	}

	HGLOBAL hGlobal = pDataObject->GetGlobalData(CF_TEXT);
	CString strTemp = static_cast<LPCTSTR>(::GlobalLock(hGlobal));

	if (!AfxIsValidString(strTemp))
	{
		return bSuccess;
	}

	//
	::GlobalUnlock(hGlobal);
	::GlobalFree(hGlobal);

	if (strTemp.Find(m_strStudyViewInfoPrefix) != -1)
	{
		INT_PTR nSrcInstancePos = -1;
		//
		CStudy* pStudy;
		INT_PTR nCurSeriesIndex;
		INT_PTR nCurInstanceIndex;

		pStudy = ParseStudyViewerInfo(strTemp, nCurSeriesIndex, nCurInstanceIndex);
		if (pStudy == nullptr)
			return bSuccess;

		if (m_pParentWnd->IsKindOf(RUNTIME_CLASS(CStudyViewer)))
		{
			CStudyViewer* pStudyViewer = dynamic_cast<CStudyViewer*>(m_pParentWnd);
			INT_PTR nLayoutIndex = pStudyViewer->GetLayoutIndex();

			theApp.m_pLayoutManager->SetStudyToSelectedStudyViewer(pStudy, nLayoutIndex, nCurSeriesIndex, nCurInstanceIndex);
			bSuccess = TRUE;
		}
	}

	return bSuccess;
}

void COleDropTargetEx::OnDragLeave(CWnd* pWnd)
{
	if (pWnd->IsKindOf(RUNTIME_CLASS(CStudyViewer)))
	{
		CStudyViewer* pStudyViewer = dynamic_cast<CStudyViewer*>(pWnd);
		pStudyViewer->Invalidate(FALSE);
	}
}
