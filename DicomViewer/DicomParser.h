#pragma once

#include "MyDicomLanguage.h"
#include "DicomImage.h"
#include "LLDicomDS.h"
#include <math.h>

class CLLDicomDS;
class CDicomParser : public LDicomDS
{
	friend CLLDicomDS;

public:
	CDicomParser();
	~CDicomParser();

public:
	CLLDicomDS::DICOM_HEADER_INFO_ m_dcmHeaderInfo;
	CArray<CDicomImage, CDicomImage> m_aryDicomImage;	// for multi frame
	CLLDicomDS m_LLDicomDS;

	// to Control
	BOOL m_bInitialized;
	pDICOMELEMENT m_pParentElement;
	pDICOMELEMENT m_pCurrentElement;

private:
	static CMyDicomLanguage m_dcmLanguage;
	int m_nCurNLS;

public:
	UINT Initialize();

	//
	L_UINT16 LoadDS(L_TCHAR *pszName, L_UINT16 nFlags);

	// GET
	CString GetValue(UINT uTag = 0, int nOffset = 1);
	CString GetValue(pDICOMELEMENT pElement, int nOffset = 1);
	CString GetValue(UINT uTag, int nOffset, pDICOMELEMENT pCurElement);

	CString GetStudyID();
	CString GetSeriesID();
	CString GetInstanceID();

	//SET
	void SetDcmHeaderInfo(CLLDicomDS::DICOM_HEADER_INFO_ dsHeaderInfo);
	void AddDcmImageInfo(CDicomImage dsImageInfo);

	//
	BOOL ParseDicomHeader();
	BOOL ParseImageInfo();
	BOOL ParseLLDicomDS();
	BOOL ParseOverlayInfo(pBITMAPHANDLE pOverlayBitmapHandle, CDicomImage* pOverlayImgInfo);

	L_UINT GetOverlayImageFromOrigin(pBITMAPHANDLE pBitmapHandle, pBITMAPHANDLE pOverlayBitmapHandle);

	L_UINT16 DeleteAllOverlays();
	UINT MoveRootElement();
	VOID SetFileName(CString strFileName);
	VOID SetCurrentElement(pDICOMELEMENT pCurrentElement);
	L_VOID ResetDS();

	int ParseNLS();
	int& GetCurNLS();
	CLLDicomDS GetLLDicomDS(INT_PTR nFrameIndex = 0);

	CDicomParser& operator=(const CDicomParser& obj);

private:
	FLOAT GetOrientationOrthogonalX(double dRowY, double dRowZ, double dColY, double dColZ, INT_PTR nFrameIndex = 0);
	FLOAT GetOrientationOrthogonalY(double dRowX, double dRowZ, double dColX, double dColZ, INT_PTR nFrameIndex = 0);
	FLOAT GetOrientationOrthogonalZ(double dRowX, double dRowY, double dColX, double dColY, INT_PTR nFrameIndex = 0);
};

