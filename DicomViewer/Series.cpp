#include "stdafx.h"
#include "Series.h"


CSeries::CSeries(CLLDicomDS dsDicomDS)
{
	m_dsDicomDS = dsDicomDS;
}


CSeries::~CSeries()
{
	for (INT_PTR iArr = 0; iArr < m_arrInstance.GetCount(); iArr++)
	{
		CInstance* pSeries = m_arrInstance.GetAt(iArr);
		if (pSeries)
		{
			delete pSeries;
			pSeries = NULL;
		}
	}
	m_arrInstance.RemoveAll();
}

BOOL CSeries::IsSameSeriesID(CString strSeriesId)
{
	if (m_dsDicomDS.m_dcmHeaderInfo.m_strStudyID.IsEmpty())
	{
		return FALSE;
	}
	else if (!strSeriesId.CompareNoCase(m_dsDicomDS.m_dcmHeaderInfo.m_strSeriesInstanceUID))
	{
		return TRUE;
	}

	return FALSE;
}

void CSeries::AddInstance(CLLDicomDS& dsLLDicomDS)
{
	CInstance* pInstance = new CInstance(dsLLDicomDS);
	INT_PTR iInsertIndex = -1;
	CInstance* pOldInstanceInfo;
	long lCurInstanceNumber = -1;
	long lOldInstanceNumber = -1;

	
	for (INT_PTR iArray = 0; iArray < m_arrInstance.GetCount(); iArray++)
	{
		CInstance* pInstanceInfo = m_arrInstance.GetAt(iArray);
		if (pInstanceInfo->IsSameInstanceID(m_dsDicomDS.m_dcmHeaderInfo.m_strSeriesInstanceUID) == TRUE)
		{
			return;
		}

		lCurInstanceNumber = pInstanceInfo->GetDicomDS()->m_dcmHeaderInfo.m_lInstanceNumber;

		if (dsLLDicomDS.m_dcmHeaderInfo.m_lInstanceNumber < lCurInstanceNumber)
		{
			if (lOldInstanceNumber == -1 ||
				lOldInstanceNumber > lCurInstanceNumber)
			{
				iInsertIndex = iArray;
				pOldInstanceInfo = pInstanceInfo;
				lOldInstanceNumber = pOldInstanceInfo->GetDicomDS()->m_dcmHeaderInfo.m_lInstanceNumber;
			}
		}
	}

	if (iInsertIndex < 0)
	{
		m_arrInstance.Add(pInstance);
	}
	else
	{
		m_arrInstance.InsertAt(iInsertIndex, pInstance);
	}

	return;
}

void CSeries::UpdateDicomDs(CLLDicomDS dsLLDicom)
{
	m_dsDicomDS = dsLLDicom;
}

CArray<CInstance*, CInstance*>* CSeries::GetInstanceArray()
{
	return &m_arrInstance;
}

CLLDicomDS* CSeries::GetDicomDS()
{
	return &m_dsDicomDS;
}

CSeries& CSeries::operator=(const CSeries& obj)
{
	m_dsDicomDS = obj.m_dsDicomDS;
	m_arrInstance.Copy(obj.m_arrInstance);

	return *this;
}