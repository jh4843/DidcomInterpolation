#include "stdafx.h"
#include "OleDataSourceEx.h"

CObject* COleDataSourceEx::m_pDragSourceObject = nullptr;

COleDataSourceEx::COleDataSourceEx()
{
}


COleDataSourceEx::~COleDataSourceEx()
{
}

void COleDataSourceEx::SetDragSourceObject(CObject* pObject, CRect rtEffect)
{
	ASSERT(pObject);

	m_pDragSourceObject = pObject;

	if (pObject->IsKindOf(RUNTIME_CLASS(CStudyViewer)))
	{
		CStudyViewer* pStudyViewer = dynamic_cast<CStudyViewer*>(pObject);

		CString strBuffer;
		strBuffer.Format(_T("STUDYVIEW:%s,%d,%d"), pStudyViewer->GetStudy()->GetStudyId(), pStudyViewer->GetCurrentSeriesIndex(), pStudyViewer->GetCurrentInstanceIndex());

		CacheString(CF_TEXT, strBuffer);

		DROPEFFECT DropEffect = DoDragDrop(DROPEFFECT_MOVE, (LPCRECT)&rtEffect);
	}
}

BOOL COleDataSourceEx::CacheString(CLIPFORMAT cfFormat, LPCTSTR lpszText)
{
	HGLOBAL hGlobal = NULL;
	size_t nSize = (_tcslen(lpszText) + 1) * sizeof(TCHAR);
	if (nSize > sizeof(TCHAR))                       // don't cache empty strings
	{
		hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, nSize);
		if (NULL != hGlobal)
		{
			LPVOID lpBuf = ::GlobalLock(hGlobal);
			::CopyMemory(lpBuf, lpszText, nSize);
			::GlobalUnlock(hGlobal);
			CacheGlobalData(cfFormat, hGlobal);
		}
	}
	return NULL != hGlobal;
}

