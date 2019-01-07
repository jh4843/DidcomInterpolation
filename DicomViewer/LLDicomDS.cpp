#include "stdafx.h"
#include "LLDicomDS.h"


CLLDicomDS::CLLDicomDS()
{
	Init();
}

CLLDicomDS::CLLDicomDS(const CLLDicomDS& dsLLDicomds)
{
	Init();

	m_dcmHeaderInfo = dsLLDicomds.m_dcmHeaderInfo;
	
	for (INT_PTR iFrame = 0; iFrame < dsLLDicomds.m_dcmHeaderInfo.m_nFrameCount; iFrame++)
	{
		m_aryDicomImage.Add(dsLLDicomds.m_aryDicomImage.GetAt(iFrame));
	}
	
}


CLLDicomDS::~CLLDicomDS()
{
	m_aryDicomImage.RemoveAll();
}

void CLLDicomDS::Init()
{
	m_dcmHeaderInfo.Init();

	for(INT_PTR i = 0 ; i<m_aryDicomImage.GetCount(); i++)
	{
		m_aryDicomImage.GetAt(i).FreeDicomImage();
	}
	m_aryDicomImage.RemoveAll();
}

CString CLLDicomDS::GetStudyID()
{
	return m_dcmHeaderInfo.m_strStudyInstanceUID;
}

CString CLLDicomDS::GetSeriesID()
{
	return m_dcmHeaderInfo.m_strSeriesInstanceUID;
}

CString CLLDicomDS::GetInstanceID()
{
	return m_dcmHeaderInfo.m_strSOPInstanceUID;
}

long CLLDicomDS::GetSeriesNumber()
{
	return m_dcmHeaderInfo.m_lSeriesNumber;
}

long CLLDicomDS::GetInstanceNumber()
{
	return m_dcmHeaderInfo.m_lInstanceNumber;
}

CString CLLDicomDS::GetPatientOrientationUp(INT_PTR nFrameIndex)
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation(nFrameIndex) == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX > 0)
	{
		strRes += _T("R");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX < 0)
	{
		strRes += _T("L");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY > 0)
	{
		strRes += _T("A");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY < 0)
	{
		strRes += _T("P");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ > 0)
	{
		strRes += _T("F");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ < 0)
	{
		strRes += _T("H");
	}

	strRes += _T("]");
	return strRes;
}

CString CLLDicomDS::GetPatientOrientationDown(INT_PTR nFrameIndex)
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation(nFrameIndex) == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX > 0)
	{
		strRes += _T("L");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX < 0)
	{
		strRes += _T("R");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY > 0)
	{
		strRes += _T("P");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY < 0)
	{
		strRes += _T("A");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ > 0)
	{
		strRes += _T("H");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ < 0)
	{
		strRes += _T("F");
	}

	strRes += _T("]");
	return strRes;
}

CString CLLDicomDS::GetPatientOrientationLeft(INT_PTR nFrameIndex)
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation(nFrameIndex) == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX > 0)
	{
		strRes += _T("R");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX < 0)
	{
		strRes += _T("L");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY > 0)
	{
		strRes += _T("A");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY < 0)
	{
		strRes += _T("P");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ > 0)
	{
		strRes += _T("F");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ < 0)
	{
		strRes += _T("H");
	}

	strRes += _T("]");
	return strRes;
}

CString CLLDicomDS::GetPatientOrientationRight(INT_PTR nFrameIndex)
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation(nFrameIndex) == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX > 0)
	{
		strRes += _T("L");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX < 0)
	{
		strRes += _T("R");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY > 0)
	{
		strRes += _T("P");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY < 0)
	{
		strRes += _T("A");
	}

	//
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ > 0)
	{
		strRes += _T("H");
	}
	else if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ < 0)
	{
		strRes += _T("F");
	}

	strRes += _T("]");
	return strRes;
}

BOOL CLLDicomDS::IsSamePosAndOrient(CLLDicomDS& dicomDS, INT_PTR nFrameIndex)
{
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_X == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_X &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_Y == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_Y &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_Z == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_Z &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ == dicomDS.m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ
		)
	{
		return TRUE;
	}

	return FALSE;
}

CLLDicomDS CLLDicomDS::operator=(const CLLDicomDS obj)
{
	m_dcmHeaderInfo = obj.m_dcmHeaderInfo;

	m_aryDicomImage.RemoveAll();
	for (INT_PTR iFrame = 0; iFrame < obj.m_aryDicomImage.GetCount(); iFrame++)
	{
		m_aryDicomImage.Add(obj.m_aryDicomImage.GetAt(iFrame));
	}

	return *this;
}

BOOL CLLDicomDS::IsValidPatientOrientation(INT_PTR nFrameIndex)
{
	if (m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX == 0.0 &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY == 0.0 &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ == 0.0 &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX == 0.0 &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY == 0.0 &&
		m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ == 0.0)
	{
		return FALSE;
	}

	return TRUE;
}