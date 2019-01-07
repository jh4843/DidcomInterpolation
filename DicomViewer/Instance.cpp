#include "stdafx.h"
#include "Instance.h"


CInstance::CInstance(CLLDicomDS dsLLDicomds)
{
	m_dsLLDicomds = dsLLDicomds;
}

CInstance::~CInstance()
{

}

BOOL CInstance::IsSameInstanceID(CString strInstanceId)
{
	if (m_dsLLDicomds.m_dcmHeaderInfo.m_strStudyID.IsEmpty())
	{
		return FALSE;
	}
	else if (!strInstanceId.CompareNoCase(m_dsLLDicomds.m_dcmHeaderInfo.m_strSOPInstanceUID))
	{
		return TRUE;
	}

	return FALSE;
}

void CInstance::UpdateDicomDs(CLLDicomDS dsLLDicom)
{
	m_dsLLDicomds = dsLLDicom;
}


CDicomImage* CInstance::GetDicomImage(INT_PTR nFrameIndex)
{
	return &m_dsLLDicomds.m_aryDicomImage.GetAt(nFrameIndex);
}

CLLDicomDS* CInstance::GetDicomDS()
{
	return &m_dsLLDicomds;
}

CInstance& CInstance::operator=(const CInstance& obj)
{
	m_dsLLDicomds = obj.m_dsLLDicomds;
	m_pImage = obj.m_pImage;

	return *this;
}