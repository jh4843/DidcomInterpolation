#include "stdafx.h"
#include "DicomParser.h"

CMyDicomLanguage CDicomParser::m_dcmLanguage;

CDicomParser::CDicomParser()
	: m_bInitialized(FALSE)
{
	if (!m_bInitialized)
	{
		Initialize();
	}
	m_dcmHeaderInfo.m_strFileName = _T("");
	m_nCurNLS = -1;
}


CDicomParser::~CDicomParser()
{
	ResetDS();
}


UINT CDicomParser::Initialize()
{
	m_pParentElement = NULL;
	m_pCurrentElement = NULL;

	MoveRootElement();

	UINT ndcmRet = DICOM_SUCCESS;

	m_bInitialized = TRUE;

	m_nCurNLS = -1;

	m_dcmHeaderInfo.Init();

	return ndcmRet;
}

L_UINT16 CDicomParser::LoadDS(L_TCHAR *pszName, L_UINT16 nFlags)
{
	m_dcmHeaderInfo.m_strFileName.Format(_T("%s"), pszName);
	//
	//return LDicomDS::LoadDS(pszName, nFlags);
	L_UINT16 nRet = LDicomDS::LoadDS(pszName, nFlags);

	return nRet;
}

CString CDicomParser::GetValue(pDICOMELEMENT pElement, int nOffset)
{
	CString strValue;
	pDICOMELEMENT pOldCurrentElement = m_pCurrentElement;
	m_pCurrentElement = pElement;
	strValue = GetValue((UINT)0, nOffset);
	m_pCurrentElement = pOldCurrentElement;
	return strValue;
}

CString CDicomParser::GetValue(UINT uTag, int nOffset, pDICOMELEMENT pCurElement)
{
	CString strValue;
	pDICOMELEMENT pOldCurrentElement = m_pCurrentElement;
	m_pCurrentElement = pCurElement;
	strValue = GetValue(uTag, nOffset);
	m_pCurrentElement = pOldCurrentElement;

	return strValue;
}

CString CDicomParser::GetStudyID()
{
	return m_dcmHeaderInfo.m_strStudyInstanceUID;
}

CString CDicomParser::GetSeriesID()
{
	return m_dcmHeaderInfo.m_strSeriesInstanceUID;
}

CString CDicomParser::GetInstanceID()
{
	return m_dcmHeaderInfo.m_strSOPInstanceUID;
}

void CDicomParser::SetDcmHeaderInfo(CLLDicomDS::DICOM_HEADER_INFO_ dsHeaderInfo)
{
	m_dcmHeaderInfo = dsHeaderInfo;
}

void CDicomParser::AddDcmImageInfo(CDicomImage dsImageInfo)
{
	m_aryDicomImage.Add(dsImageInfo);
}

CString CDicomParser::GetValue(UINT uTag, int nOffset)
{
	L_INT32 nIndex = 0;
	L_INT32 nCount = 0;
	pDICOMELEMENT pElement = NULL;

	CString strValue;
	CString	strToken;
	CString strDelimiter;

	strValue.Empty();
	strToken.Empty();
	strDelimiter.Empty();

	if (nOffset < 0)
		strDelimiter.Format(_T("\\"));

	if (!m_pParentElement && !m_pCurrentElement)
		MoveRootElement();

	if (uTag == 0)
		pElement = m_pCurrentElement;
	else
		pElement = LDicomDS::FindFirstElement(m_pCurrentElement, uTag, TRUE);

	if (!pElement)
	{
		pElement = LDicomDS::FindFirstElement(NULL, uTag, FALSE);
	}

	if (pElement == NULL)
		return strValue;

	nCount = LDicomDS::GetCountValue(pElement);
	if (((pElement->nLength == ELEMENT_LENGTH_MAX)) || (pElement->nLength > 1024) || (nCount == 0))
		return strValue;

	switch (pElement->nVR)
	{
	case VR_OB:
	case VR_UN:
	{
		L_UCHAR* pChar = NULL;

		pChar = GetCharValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%02X"), (L_UCHAR)pChar[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%02X"), (L_UCHAR)pChar[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_SS:
	{
		L_INT16* pShort = NULL;
		pShort = GetShortValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%d"), pShort[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%d"), pShort[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_US:
	case VR_OW:
	{
		L_INT16* pShort = NULL;
		pShort = GetShortValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%u"), (L_UINT16)pShort[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%u"), (L_UINT16)pShort[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_SL:
	case VR_IS:
	{
		L_INT32* pLong = NULL;
		pLong = GetLongValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%ld"), pLong[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%ld"), pLong[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_UL:
	{
		L_INT32* pLong = NULL;
		pLong = GetLongValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%lu"), (L_UINT32)pLong[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%lu"), (L_UINT32)pLong[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_AT:
	{
		L_INT32* pLong = NULL;
		pLong = GetLongValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%04X:%04x"), GETGROUP((L_UINT32)pLong[nIndex]), GETELEMENT((L_UINT32)pLong[nIndex]));
					break;
				}
			}
			else
			{
				strToken.Format(_T("%04X:%04x"), GETGROUP((L_UINT32)pLong[nIndex]), GETELEMENT((L_UINT32)pLong[nIndex]));
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_FL:
	{
		L_FLOAT* pFloat = NULL;
		pFloat = GetFloatValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%f"), pFloat[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%f"), pFloat[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_FD:
	case VR_DS:
	{
		L_DOUBLE* pDouble = NULL;
		pDouble = GetDoubleValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%f"), pDouble[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%f"), pDouble[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_CS:
	case VR_SH:
	case VR_LO:
	case VR_AE:
	case VR_LT:
	case VR_ST:
	case VR_UI:
	case VR_UT:
	case VR_PN:
	{
		int nLength = pElement->nLength;
		if (nLength > 0)
		{
			//				strValue = LDicomDS::GetStringValue(pElement, 0, nCount);
			//				USES_CONVERSION;

			CStringA strTempA;
			LDicomDS::GetBinaryValue(pElement, strTempA.GetBuffer(nLength + 1), nLength);
			strTempA.ReleaseBuffer(nLength);

			strValue = m_dcmLanguage.DecodeNLSEx(strTempA, GetCurNLS());
		}
	}
	break;

	case VR_AS:
	{
		pVALUEAGE pAge;
		pAge = LDicomDS::GetAgeValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
					strValue.Format(_T("%u%c"), pAge[nIndex].nNumber, pAge[nIndex].nReference);
			}
			else
			{
				strToken.Format(_T("%u%c"), pAge[nIndex].nNumber, pAge[nIndex].nReference);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_DA:
		if (nCount > 0)
		{
			BOOL bRet = FALSE;
			L_CHAR szValue[MAX_PATH];
			memset(szValue, 0, sizeof(szValue));
			bRet = LDicomDS::GetBinaryValue(pElement, szValue, MAX_PATH);
			strValue = CA2W(szValue);
		}
		break;

	case VR_TM:
		if (nCount > 0)
		{
			BOOL bRet = FALSE;
			L_CHAR szValue[MAX_PATH];
			memset(szValue, 0, sizeof(szValue));
			bRet = LDicomDS::GetBinaryValue(pElement, szValue, MAX_PATH);
			//strValue.Format(_T("%s"), CA2W(szValue));
			strValue = CA2W(szValue);
		}
		break;

	case VR_DT:
		if (nCount > 0)
		{
			BOOL bRet = FALSE;
			L_CHAR szValue[MAX_PATH];
			memset(szValue, 0, sizeof(szValue));
			bRet = LDicomDS::GetBinaryValue(pElement, szValue, MAX_PATH);
			//strValue.Format(_T("%s"), CA2W(szValue));
			strValue = CA2W(szValue);
		}
		break;
	}

	FreeValue(pElement);
	return strValue;
}


BOOL CDicomParser::ParseDicomHeader()
{
	CString strValue;
	pDICOMELEMENT pElement = nullptr;

	// Patient
	m_dcmHeaderInfo.m_strPatientName = GetValue(TAG_PATIENT_NAME);
	m_dcmHeaderInfo.m_strPatientName.Replace(_T("^"), _T(" "));
	m_dcmHeaderInfo.m_strPatientID = GetValue(TAG_PATIENT_ID);
	m_dcmHeaderInfo.m_strBirthDate = GetValue(TAG_PATIENT_BIRTH_DATE);
	m_dcmHeaderInfo.m_strSex = GetValue(TAG_PATIENT_SEX);
	m_dcmHeaderInfo.m_strPatientComments = GetValue(TAG_PATIENT_COMMENTS);

	//Study
	m_dcmHeaderInfo.m_strStudyID = GetValue(TAG_STUDY_ID);
	m_dcmHeaderInfo.m_strStudyInstanceUID = GetValue(TAG_STUDY_INSTANCE_UID);
	m_dcmHeaderInfo.m_strStudyDescription = GetValue(TAG_STUDY_DESCRIPTION);

	// Series
	m_dcmHeaderInfo.m_strModality = GetValue(TAG_MODALITY);
	strValue = GetValue(TAG_SERIES_NUMBER);
	if (!strValue.IsEmpty())
	{
		m_dcmHeaderInfo.m_lSeriesNumber = _ttol(strValue);
	}
	m_dcmHeaderInfo.m_strSeriesInstanceUID = GetValue(TAG_SERIES_INSTANCE_UID);

	// Instance
	strValue = GetValue(TAG_INSTANCE_NUMBER);
	if (!strValue.IsEmpty())
	{
		m_dcmHeaderInfo.m_lInstanceNumber = _ttol(strValue);
	}
	m_dcmHeaderInfo.m_strSOPInstanceUID = GetValue(TAG_SOP_INSTANCE_UID);
	m_dcmHeaderInfo.m_strSOPClassUID = GetValue(TAG_SOP_CLASS_UID);
	m_dcmHeaderInfo.m_strTransferSyntaxUID = GetValue(TAG_REFERENCED_TRANSFER_SYNTAX_UID_IN_FILE);

	pElement = FindLastElement(NULL, TAG_PIXEL_DATA, FALSE);
	m_dcmHeaderInfo.m_nFrameCount = GetCountImage(pElement);
	
	return TRUE;
}

BOOL CDicomParser::ParseImageInfo()
{
	DICOMIMAGE lDicomImage;
	BITMAPHANDLE BitmapHandle;

	CDicomImage dicomImage;
	pDICOMELEMENT pCurPositionElement = NULL;
	pDICOMELEMENT pCurOrientationElement = NULL;
	pDICOMELEMENT pElement;

	// Pixel Information
	dicomImage.m_stImageInfo.m_fPixelXSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_PIXEL_SPACING));
	if (dicomImage.m_stImageInfo.m_fPixelXSpacing < 0.0f)
	{
		dicomImage.m_stImageInfo.m_fPixelXSpacing = 0.0f;
	}

	dicomImage.m_stImageInfo.m_fPixelYSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_PIXEL_SPACING));
	if (dicomImage.m_stImageInfo.m_fPixelYSpacing < 0.0f)
	{
		dicomImage.m_stImageInfo.m_fPixelYSpacing = 0.0f;
	}

	dicomImage.m_stImageInfo.m_fImagerXSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_IMAGER_PIXEL_SPACING));
	if (dicomImage.m_stImageInfo.m_fImagerXSpacing < 0.0f)
	{
		dicomImage.m_stImageInfo.m_fImagerXSpacing = 0.0f;
	}

	dicomImage.m_stImageInfo.m_fImagerYSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_IMAGER_PIXEL_SPACING));
	if (dicomImage.m_stImageInfo.m_fImagerYSpacing < 0.0f)
	{
		dicomImage.m_stImageInfo.m_fImagerYSpacing = 0.0f;
	}

	dicomImage.m_stImageInfo.m_fMagFactor = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_ESTIMATED_RADIOGRAPHIC_MAGNIFICATION_FACTOR));
	//



	double dFrameTime = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_FRAME_TIME));
	if (dFrameTime)
	{
		dicomImage.m_stImageInfo.m_nFramePerSecond = (UINT)(((double)1000 / dFrameTime) + 0.05);
	}

	pElement = FindLastElement(NULL, TAG_PIXEL_DATA, FALSE);
	
	if (GetInfoImage(pElement, &lDicomImage, 0) == DICOM_SUCCESS)
	{
		INT_PTR nWidth = lDicomImage.nColumns;
		INT_PTR nHeight = lDicomImage.nRows;
		INT_PTR nBitsPerPixel = lDicomImage.nHighBit + 1;
		INT_PTR nSamplesPerPixel = lDicomImage.nSamplesPerPixel;

		if (nWidth <= 0)
		{
			AfxDebugBreak();
			return FALSE;
		}

		//
		if (nHeight <= 0)
		{
			AfxDebugBreak();
			return FALSE;
		}

		//
		if (nBitsPerPixel <= 0)
		{
			AfxDebugBreak();
			return FALSE;
		}

		dicomImage.m_stImageInfo.m_nBitsPerPixel = nBitsPerPixel;
		if (dicomImage.m_stImageInfo.m_nBitsPerPixel <= 0)
		{
			if (dicomImage.m_stImageInfo.m_nBitsPerPixel == 24)
			{
				dicomImage.m_stImageInfo.m_nSamplesPerPixel = 3;
			}
			else if (dicomImage.m_stImageInfo.m_nBitsPerPixel > 0 && dicomImage.m_stImageInfo.m_nBitsPerPixel < 24)
			{
				dicomImage.m_stImageInfo.m_nSamplesPerPixel = 1;
			}
		}
		else
		{
			dicomImage.m_stImageInfo.m_nSamplesPerPixel = nSamplesPerPixel;
		}
		
		for (INT_PTR iFrame = 0; iFrame < m_dcmHeaderInfo.m_nFrameCount; iFrame++)
		{
			dicomImage.m_stImageInfo.m_nFrameIndex = iFrame;

			pElement = FindFirstElement(NULL, TAG_IMAGE_POSITION_PATIENT, FALSE);
			if (pElement)
			{
				L_DOUBLE* pDouble_X = NULL;
				L_DOUBLE* pDouble_Y = NULL;
				L_DOUBLE* pDouble_Z = NULL;

				if (iFrame > 0)
				{
					pDICOMELEMENT pTempElement = NULL;
					pTempElement = FindNextElement(pCurPositionElement, FALSE);
					if (pTempElement && pTempElement->nTag == TAG_IMAGE_POSITION_PATIENT)
					{
						pElement = pTempElement;
					}
				}
				pCurPositionElement = pElement;

				pDouble_X = GetDoubleValue(pElement, 0, 1);
				pDouble_Y = GetDoubleValue(pElement, 1, 1);
				pDouble_Z = GetDoubleValue(pElement, 2, 1);

				if (pDouble_X != NULL && pDouble_Y != NULL && pDouble_Z != NULL)
				{
					dicomImage.m_stImageInfo.m_fImagePosition_X = *pDouble_X;
					dicomImage.m_stImageInfo.m_fImagePosition_Y = *pDouble_Y;
					dicomImage.m_stImageInfo.m_fImagePosition_Z = *pDouble_Z;
				}
			}

			pElement = FindFirstElement(NULL, TAG_IMAGE_ORIENTATION_PATIENT, FALSE);
			if (pElement)
			{
				L_DOUBLE* pDouble_RowX = NULL;
				L_DOUBLE* pDouble_RowY = NULL;
				L_DOUBLE* pDouble_RowZ = NULL;
				L_DOUBLE* pDouble_ColX = NULL;
				L_DOUBLE* pDouble_ColY = NULL;
				L_DOUBLE* pDouble_ColZ = NULL;

				if (iFrame > 0)
				{
					pDICOMELEMENT pTempElement = NULL;
					pTempElement = FindNextElement(pCurOrientationElement, FALSE);
					if (pTempElement && pTempElement->nTag == TAG_IMAGE_ORIENTATION_PATIENT)
					{
						pElement = pTempElement;
					}
				}

				pCurOrientationElement = pElement;

				pDouble_RowX = GetDoubleValue(pElement, 0, 1);
				pDouble_RowY = GetDoubleValue(pElement, 1, 1);
				pDouble_RowZ = GetDoubleValue(pElement, 2, 1);
				pDouble_ColX = GetDoubleValue(pElement, 3, 1);
				pDouble_ColY = GetDoubleValue(pElement, 4, 1);
				pDouble_ColZ = GetDoubleValue(pElement, 5, 1);

				if (pDouble_RowX != NULL && pDouble_RowY != NULL && pDouble_RowZ != NULL &&
					pDouble_ColX != NULL && pDouble_ColY != NULL && pDouble_ColZ != NULL)
				{
					dicomImage.m_stImageInfo.m_fImageOrientationRowX = *pDouble_RowX;
					dicomImage.m_stImageInfo.m_fImageOrientationRowY = *pDouble_RowY;
					dicomImage.m_stImageInfo.m_fImageOrientationRowZ = *pDouble_RowZ;
					dicomImage.m_stImageInfo.m_fImageOrientationColX = *pDouble_ColX;
					dicomImage.m_stImageInfo.m_fImageOrientationColY = *pDouble_ColY;
					dicomImage.m_stImageInfo.m_fImageOrientationColZ = *pDouble_ColZ;
					dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalX = GetOrientationOrthogonalX(*pDouble_RowY, *pDouble_RowZ, *pDouble_ColY, *pDouble_ColZ);
					dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalY = GetOrientationOrthogonalY(*pDouble_RowX, *pDouble_RowZ, *pDouble_ColX, *pDouble_ColZ);
					dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalZ = GetOrientationOrthogonalZ(*pDouble_RowX, *pDouble_RowY, *pDouble_ColX, *pDouble_ColY);
				}

				if (dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalX == 0 &&
					dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalY == 0 &&
					dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalZ == 0)
				{
					CString strMsg;
					strMsg.Format(_T("Strange Value"));
					AfxMessageBox(strMsg);
				}
			}

// 			if (dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalX == 0 &&
// 				dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalY == 0 &&
// 				dicomImage.m_stImageInfo.m_fImageOrientationOrthogonalZ == 0)
// 			{
// 				CString strMsg;
// 				strMsg.Format(_T("Strange Value"));
// 				AfxMessageBox(strMsg);
// 			}


			UINT nFlags = DICOM_GETIMAGE_AUTO_APPLY_MODALITY_LUT | DICOM_GETIMAGE_AUTO_APPLY_VOI_LUT;
			
			pElement = FindLastElement(NULL, TAG_PIXEL_DATA, FALSE);
			L_UINT16 nResult = GetImage(pElement,
				&BitmapHandle,
				sizeof(BITMAPHANDLE),
				iFrame,
				0,
				ORDER_RGBORGRAY,
				nFlags,
				NULL,
				NULL);

			if (nResult == DICOM_ERROR_MEMORY)
			{
				AfxThrowMemoryException();
			}
			else if (nResult != DICOM_SUCCESS)
			{
				Sleep(0);
				nResult = GetImage(pElement,
					&BitmapHandle,
					sizeof(BITMAPHANDLE),
					iFrame,
					0,
					ORDER_RGBORGRAY,
					nFlags,
					NULL,
					NULL);

				if (nResult != DICOM_SUCCESS)
				{
					Sleep(0);
					nResult = GetImage(pElement,
						&BitmapHandle,
						sizeof(BITMAPHANDLE),
						iFrame,
						0,
						ORDER_RGBORGRAY,
						nFlags,
						NULL,
						NULL);
				}

				if (nResult != DICOM_SUCCESS)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.Flags.Allocated != 1)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.Flags.Compressed == 1)
				{
					AfxThrowUserException();
				}

				// Not linear image data.
				if (BitmapHandle.Flags.Tiled == 1)
				{
					dicomImage.SetTiledPiexlData(TRUE);
				}

				if (BitmapHandle.Flags.SuperCompressed == 1)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.Flags.UseLUT == 1)
				{
					if (!BitmapHandle.pLUT)
					{
						AfxThrowUserException();
					}

					if (BitmapHandle.LUTLength < 1)
					{
						AfxThrowUserException();
					}
				}

				if (!BitmapHandle.Addr.Windows.pData)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.Width < 2)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.Height < 2)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.BitsPerPixel < 1)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.BytesPerLine < 4)
				{
					AfxThrowUserException();
				}

				dicomImage.FreeDicomImage();
				dicomImage.m_stImageInfo.Init();

				if (BitmapHandle.HighBit < 1)
				{
					AfxThrowUserException();
				}
				else
				{
					dicomImage.m_stImageInfo.m_nBitsPerPixel = (UINT)(BitmapHandle.HighBit + 1);
				}

				// RGB : SamplesPerPixel = 3
				if (BitmapHandle.BitsPerPixel == 24)
				{
					dicomImage.m_stImageInfo.m_nSamplesPerPixel = 3;
				}
				// Gray : SamplesPerPixel = 1 (default)
				else if (BitmapHandle.BitsPerPixel > 0 && BitmapHandle.BitsPerPixel < 24)
				{
					dicomImage.m_stImageInfo.m_nSamplesPerPixel = 1;
				}
				else
				{
					AfxThrowUserException();
				}
			}

			dicomImage.m_stImageInfo.m_nBytesPerPixel = (UINT)Bits2Bytes(dicomImage.m_stImageInfo.m_nBitsPerPixel);
			dicomImage.m_stImageInfo.m_nTotalAllocatedBytes = (UINT)(dicomImage.m_stImageInfo.m_nBytesPerPixel*dicomImage.m_stImageInfo.m_nSamplesPerPixel);
			dicomImage.m_stImageInfo.m_nWidth = nWidth;
			dicomImage.m_stImageInfo.m_nHeight = nHeight;
			int nBytesPerLineX = ((dicomImage.m_stImageInfo.m_nWidth * dicomImage.m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
			int nBytesPerLineY = ((dicomImage.m_stImageInfo.m_nHeight * dicomImage.m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
			dicomImage.m_stImageInfo.m_nWidth = nBytesPerLineX / dicomImage.m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
			dicomImage.m_stImageInfo.m_nHeight = nBytesPerLineY / dicomImage.m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
			dicomImage.m_stImageInfo.m_nBytesPerLine = dicomImage.m_stImageInfo.m_nWidth * dicomImage.m_stImageInfo.m_nTotalAllocatedBytes;

			dicomImage.m_stImageInfo.m_fW1 = 0.0f;
			dicomImage.m_stImageInfo.m_fW2 = Bits2MaxValue(dicomImage.m_stImageInfo.m_nBitsPerPixel);
			//
			// Get W/L
			L_DOUBLE dWindowCenter = 0;
			L_DOUBLE dWindowWidth = 0;
			// Window Center
			pElement = FindFirstElement(NULL, TAG_WINDOW_CENTER, FALSE);
			if (pElement && pElement->nLength)
			{
				dWindowCenter = *GetDoubleValue(pElement, 0, 1);
				dicomImage.m_stImageInfo.m_nW1 = (int)dWindowCenter;
			}
			else
			{
				dicomImage.m_stImageInfo.m_nW1 = (int)dicomImage.m_stImageInfo.m_fW1;
			}

			// Window Width
			pElement = FindFirstElement(NULL, TAG_WINDOW_WIDTH, FALSE);
			if (pElement && pElement->nLength)
			{
				dWindowWidth = *GetDoubleValue(pElement, 0, 1);
				dicomImage.m_stImageInfo.m_nW2 = (int)dWindowWidth;
			}
			else
			{
				dicomImage.m_stImageInfo.m_nW2 = (int)dicomImage.m_stImageInfo.m_fW2;
			}
			//

			//dicomImage.LoadDicomImage(&BitmapHandle);
			m_aryDicomImage.Add(dicomImage);
		}
	}
	else
	{
		L_FreeBitmap(&BitmapHandle);
		return FALSE;
	}
	L_FreeBitmap(&BitmapHandle);
	return TRUE;
}

BOOL CDicomParser::ParseLLDicomDS()
{
	m_LLDicomDS.m_dcmHeaderInfo = m_dcmHeaderInfo;
	m_LLDicomDS.m_aryDicomImage.RemoveAll();
	for (INT_PTR iFrame = 0; iFrame < m_dcmHeaderInfo.m_nFrameCount; iFrame++)
	{
		m_LLDicomDS.m_aryDicomImage.Add(m_aryDicomImage.GetAt(iFrame));
	}

	return TRUE;
}

BOOL CDicomParser::ParseOverlayInfo(pBITMAPHANDLE pOverlayBitmapHandle, CDicomImage* pOverlayImgInfo)
{
	// nWidth, nHeight, m_nBitsPerPixel, nSamplesPerPixel is already set.
	pOverlayImgInfo->m_stImageInfo.m_nBytesPerPixel = (UINT)Bits2Bytes(pOverlayImgInfo->m_stImageInfo.m_nBitsPerPixel);
	pOverlayImgInfo->m_stImageInfo.m_nTotalAllocatedBytes = (UINT)(pOverlayImgInfo->m_stImageInfo.m_nBytesPerPixel*pOverlayImgInfo->m_stImageInfo.m_nSamplesPerPixel);
	int nBytesPerLineX = ((pOverlayImgInfo->m_stImageInfo.m_nWidth * pOverlayImgInfo->m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
	int nBytesPerLineY = ((pOverlayImgInfo->m_stImageInfo.m_nHeight * pOverlayImgInfo->m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
	pOverlayImgInfo->m_stImageInfo.m_nWidth = nBytesPerLineX / pOverlayImgInfo->m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
	pOverlayImgInfo->m_stImageInfo.m_nHeight = nBytesPerLineY / pOverlayImgInfo->m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
	pOverlayImgInfo->m_stImageInfo.m_nBytesPerLine = pOverlayImgInfo->m_stImageInfo.m_nWidth * pOverlayImgInfo->m_stImageInfo.m_nTotalAllocatedBytes;

	pOverlayImgInfo->m_stImageInfo.m_fW1 = 0.0f;
	pOverlayImgInfo->m_stImageInfo.m_fW2 = Bits2MaxValue(pOverlayImgInfo->m_stImageInfo.m_nBitsPerPixel);

	return TRUE;
}


L_UINT CDicomParser::GetOverlayImageFromOrigin(pBITMAPHANDLE pOrgImageBitmapHandle, pBITMAPHANDLE pOverlayBitmapHandle)
{
	OVERLAYATTRIBUTES OverlayAttributes = { 0 };
	L_UINT            uOverlayCount = 0;
	L_UINT            uOverlayIndex;
	L_INT             nLEADRet;
	L_UINT16          uDICOMRet;
	L_INT             GroupNumber = 0;
	L_BOOL            IsOverlayInDataset = FALSE;


	//(1)Sanity Check ! 
	if (NULL == pOrgImageBitmapHandle)
	{
		return 0;
	}
	//(2)Do we have any overlays at all ? 
	uDICOMRet = GetOverlayCount(&uOverlayCount);
	if (DICOM_SUCCESS != uDICOMRet)
	{
		return -1;
	}
	// If no overlays just return 
	if (0 == uOverlayCount)
	{
		return uOverlayCount;
	}
	//(3)Blow away all the overlays in the file 
// 	uDICOMRet = DeleteAllOverlays();
// 	if (DICOM_SUCCESS != uDICOMRet)
// 	{
// 		return -2;
// 	}
	//(4) Loop through the overlays and add them into the DICOM file 
	for (uOverlayIndex = 0; uOverlayIndex < uOverlayCount; uOverlayIndex++)
	{
		memset(&OverlayAttributes, 0, sizeof(OVERLAYATTRIBUTES));
		memset(pOverlayBitmapHandle, 0, sizeof(BITMAPHANDLE));
		// Get overlay attributes 
		uDICOMRet = this->GetOverlayAttributes(uOverlayIndex,
			&OverlayAttributes,
			sizeof(OverlayAttributes),
			&GroupNumber,
			&IsOverlayInDataset,
			//0);
			OVERLAYATTRIBUTES_ORIGIN |
			OVERLAYATTRIBUTES_FLAGS |
			OVERLAYATTRIBUTES_BITINDEX |
			OVERLAYATTRIBUTES_DICOM);
		
		if (DICOM_SUCCESS != uDICOMRet)
		{
			return -3;
		}

		OverlayAttributes.uFlags |= OVERLAY_AUTOPAINT | OVERLAY_AUTOPROCESS;
		OverlayAttributes.crColor = RGB(0xFF, 0xFF, 0xFF);

		// burn overlays which need to be part of the image 
		if (OverlayAttributes.uFlags &OVERLAY_USEBITPLANE)
		{
			// Set overlay attributes inside DICOM 
			uDICOMRet = this->SetOverlayAttributes(uOverlayIndex, &OverlayAttributes, 0);
			if (DICOM_SUCCESS != uDICOMRet)
			{
				return -4;
			}

			nLEADRet = L_UpdateBitmapOverlayBits(pOrgImageBitmapHandle,
				uOverlayIndex,
				SETOVERLAYBITS_FROMOVERLAY);
			if (SUCCESS != nLEADRet)
			{
				return -5;
			}
		}

		// Get the overlay data (if it's not part of the image) 
		nLEADRet = L_GetOverlayBitmap(pOrgImageBitmapHandle,
			uOverlayIndex,
			pOverlayBitmapHandle,
			sizeof(BITMAPHANDLE),
			OVERLAY_NOCOPY);
		if (SUCCESS != nLEADRet)
		{
			return -6;
		}
		// Set overlay data into DICOM 
		uDICOMRet = this->SetOverlayBitmap(uOverlayIndex, pOverlayBitmapHandle, 0);
		if (DICOM_SUCCESS != uDICOMRet)
		{
			return -7;
		}
	}
	return uOverlayCount;

}

L_UINT16 CDicomParser::DeleteAllOverlays()
{
	L_UINT16 nRet;
	L_UINT   uOverlayCount = 0;
	L_UINT16 uOverlayIndex;
	nRet = this->GetOverlayCount(&uOverlayCount);
	if (DICOM_SUCCESS != nRet)
	{
		return nRet;
	}
	for (uOverlayIndex = 0; uOverlayIndex < uOverlayCount; uOverlayIndex++)
	{
		nRet = this->DeleteOverlay(uOverlayIndex, 0);
		if (DICOM_SUCCESS != nRet)
		{
			return nRet;
		}
	}
	return DICOM_SUCCESS;
}

UINT CDicomParser::MoveRootElement()
{
	pDICOMELEMENT pElement = LDicomDS::GetFirstElement(NULL, FALSE, FALSE);
	SetCurrentElement(pElement);
	return DICOM_SUCCESS;
}

VOID CDicomParser::SetFileName(CString strFileName)
{
	if (strFileName.IsEmpty())
	{
		m_dcmHeaderInfo.m_strFileName = _T("");
		return;
	}

	m_dcmHeaderInfo.m_strFileName = strFileName;
	return;
}

VOID CDicomParser::SetCurrentElement(pDICOMELEMENT pCurrentElement)
{
	UINT ndcmRet = DICOM_SUCCESS;
	m_pCurrentElement = pCurrentElement;
	if (m_pCurrentElement)
	{
		m_pParentElement = LDicomDS::GetParentElement(m_pCurrentElement);
	}
}

L_VOID CDicomParser::ResetDS()
{
	
	LDicomDS::ResetDS();
}

int CDicomParser::ParseNLS()
{
	int nNLS = 0;
	pDICOMELEMENT pElement = LDicomDS::FindFirstElement(NULL, TAG_SPECIFIC_CHARACTER_SET, FALSE);
	if (!pElement)
	{
		nNLS = DCM_NLS_DEFAULT;
		return nNLS;
	}

	CString strNLS;
	CStringArray aryNLS;
	L_UINT32 nCount = LDicomDS::GetCountValue(pElement);
	for (L_UINT32 nIndex = 0; nIndex < nCount; nIndex++)
	{
		strNLS += LDicomDS::GetStringValue(pElement, nIndex, 1);
		if (nIndex + 1 < nCount)
		{
			strNLS += _T("\\");
		}

		aryNLS.Add(strNLS);
	}

	for (INT_PTR nIndex = 0; nIndex < aryNLS.GetCount(); nIndex++)
	{
		CString strNLS = aryNLS.GetAt(nIndex);
		strNLS = strNLS.MakeUpper();

		if (strNLS.Find(_T("ISO 2022 IR 100")) != -1 || strNLS.Find(_T("ISO_IR 100")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_100;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 101")) != -1 || strNLS.Find(_T("ISO_IR 101")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_101;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 109")) != -1 || strNLS.Find(_T("ISO_IR 109")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_109;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 110")) != -1 || strNLS.Find(_T("ISO_IR 110")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_110;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 144")) != -1 || strNLS.Find(_T("ISO_IR 144")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_144;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 127")) != -1 || strNLS.Find(_T("ISO_IR 127")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_127;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 126")) != -1 || strNLS.Find(_T("ISO_IR 126")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_126;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 138")) != -1 || strNLS.Find(_T("ISO_IR 138")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_138;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 148")) != -1 || strNLS.Find(_T("ISO_IR 148")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_148;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 13")) != -1 || strNLS.Find(_T("ISO_IR 13")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_13;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 166")) != -1 || strNLS.Find(_T("ISO_IR 166")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_166;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 14")) != -1 || strNLS.Find(_T("ISO_IR 14")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_14;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 87")) != -1 || strNLS.Find(_T("ISO_IR 87")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_87;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 159")) != -1 || strNLS.Find(_T("ISO_IR 159")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_159;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 149")) != -1 || strNLS.Find(_T("ISO_IR 149")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_149;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 192")) != -1 || strNLS.Find(_T("ISO_IR 192")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_192;
		}
		else if (strNLS.Find(_T("GB18030")) != -1)
		{
			nNLS |= DCM_NLS_GB18030;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 58")) != -1 || strNLS.Find(_T("ISO_IR 58")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_58;
		}
	}

	if (!nNLS)
	{
		nNLS = DCM_NLS_DEFAULT;
	}

	return nNLS;
}

int& CDicomParser::GetCurNLS()
{
	if (m_nCurNLS == -1)
	{
		m_nCurNLS = ParseNLS();
	}

	return m_nCurNLS;
}

CLLDicomDS CDicomParser::GetLLDicomDS(INT_PTR nFrameIndex)
{


	return m_LLDicomDS;
}

CDicomParser& CDicomParser::operator=(const CDicomParser& obj)
{
	m_dcmHeaderInfo = obj.m_dcmHeaderInfo;

	m_aryDicomImage.RemoveAll();
	for (INT_PTR iFrame = 0; iFrame < obj.m_aryDicomImage.GetCount(); iFrame++)
	{
		m_aryDicomImage.Add(obj.m_aryDicomImage.GetAt(iFrame));
	}

	return *this;
}

FLOAT CDicomParser::GetOrientationOrthogonalX(double dRowY, double dRowZ, double dColY, double dColZ, INT_PTR nFrameIndex)
{
	FLOAT fRowY = dRowY;
	FLOAT fRowZ = dRowZ;
	FLOAT fColY = dColY;
	FLOAT fColZ = dColZ;

	return (fRowY * fColZ) - (fRowZ * fColY);
}

FLOAT CDicomParser::GetOrientationOrthogonalY(double dRowX, double dRowZ, double dColX, double dColZ, INT_PTR nFrameIndex)
{
	FLOAT fRowX = dRowX;
	FLOAT fRowZ = dRowZ;
	FLOAT fColX = dColX;
	FLOAT fColZ = dColZ;

	return (fRowZ * fColX) - (fRowX * fColZ);
}

FLOAT CDicomParser::GetOrientationOrthogonalZ(double dRowX, double dRowY, double dColX, double dColY, INT_PTR nFrameIndex)
{
	FLOAT fRowX = dRowX;
	FLOAT fRowY = dRowY;
	FLOAT fColX = dColX;
	FLOAT fColY = dColY;

	return (fRowX * fColY) - (fRowY * fColX);
}