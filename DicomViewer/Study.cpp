#include "stdafx.h"
#include "LLDicomDS.h"
#include "Study.h"

CStudy::CStudy(CLLDicomDS dsDicomDS)
{
	m_dsDicomDS = dsDicomDS;
}

CStudy::CStudy()
{
}

CStudy::~CStudy()
{
	for(INT_PTR iArr = 0; iArr < m_arrSeries.GetCount(); iArr++)
	{
		CSeries* pSeries = m_arrSeries.GetAt(iArr);
		if (pSeries)
		{
			delete pSeries;
			pSeries = NULL;
		}
	}
	m_arrSeries.RemoveAll();
}

BOOL CStudy::IsSameStudyID(CString strStudyId)
{
	if (m_dsDicomDS.m_dcmHeaderInfo.m_strStudyID.IsEmpty())
	{
		return FALSE;
	}
	else if (!strStudyId.CompareNoCase(m_dsDicomDS.m_dcmHeaderInfo.m_strStudyInstanceUID))
	{
		return TRUE;
	}

	return FALSE;
}

void CStudy::AddSeries(CLLDicomDS& dsLLDicomDS)
{
	CSeries* pSeries = new CSeries(dsLLDicomDS);
	BOOL bFindSeries = FALSE;

	INT_PTR iInsertIndex = -1;
	CSeries* pOldSeriesInfo;
	long lCurSeriesNumber = -1;
	long lOldSeriesNumber = -1;

	for (INT_PTR iArray = 0; iArray < m_arrSeries.GetCount(); iArray++)
	{
		CSeries* pSeriesInfo = m_arrSeries.GetAt(iArray);
		if (pSeriesInfo->IsSameSeriesID(dsLLDicomDS.m_dcmHeaderInfo.m_strSeriesInstanceUID) == TRUE)
		{
			bFindSeries = TRUE;
			pSeries = pSeriesInfo;
			break;
		}

		lCurSeriesNumber = pSeriesInfo->GetDicomDS()->m_dcmHeaderInfo.m_lSeriesNumber;

		if (dsLLDicomDS.m_dcmHeaderInfo.m_lSeriesNumber < lCurSeriesNumber)
		{
			if (lOldSeriesNumber == -1 ||
				lOldSeriesNumber > lCurSeriesNumber)
			{
				iInsertIndex = iArray;
				pOldSeriesInfo = pSeriesInfo;
				lOldSeriesNumber = pOldSeriesInfo->GetDicomDS()->m_dcmHeaderInfo.m_lSeriesNumber;
			}
		}
	}

	pSeries->AddInstance(dsLLDicomDS);

	if (bFindSeries == FALSE)
	{
		if (iInsertIndex < 0)
		{
			m_arrSeries.Add(pSeries);
		}
		else
		{
			m_arrSeries.InsertAt(iInsertIndex, pSeries);
		}
	}

	return;
}

void CStudy::AddStudyViewerIndex(INT_PTR nStudyViewerIndex)
{
	for (INT_PTR i = 0; i < m_arrIndexDisplayStudyViewer.GetCount(); i++)
	{
		if (nStudyViewerIndex == m_arrIndexDisplayStudyViewer.GetAt(i))
		{
			return;
		}
	}
	
	m_arrIndexDisplayStudyViewer.Add(nStudyViewerIndex);
	return;
}

void CStudy::UpdateDicomDs(CLLDicomDS dicomParser)
{
	m_dsDicomDS = dicomParser;
}

CLLDicomDS CStudy::GetLLDicomDS()
{
	return m_dsDicomDS;
}

CString CStudy::GetStudyId()
{
	return m_dsDicomDS.m_dcmHeaderInfo.m_strStudyInstanceUID;
}

CArray<CSeries*, CSeries*>* CStudy::GetSeriesArray()
{
	return &m_arrSeries;
}

CStudy& CStudy::operator=(const CStudy& obj)
{
	m_dsDicomDS = obj.m_dsDicomDS;
	m_arrSeries.Copy(obj.m_arrSeries);

	return *this;
}