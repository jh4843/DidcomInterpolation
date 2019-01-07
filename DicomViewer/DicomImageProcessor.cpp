#include "stdafx.h"
#include "DicomImageProcessor.h"
#include "template_image_process.h"

CDicomImageProcessor::CDicomImageProcessor()
{
}


CDicomImageProcessor::~CDicomImageProcessor()
{
}

 BOOL CDicomImageProcessor::ProcessWindowLevel(BYTE* pOutImage, CDicomImage* pOutImageInfo, CDicomImage* pInputDicomImage, INT_PTR nW1, INT_PTR nW2)
{
	BOOL bRes = FALSE;

	if (!pInputDicomImage->m_stImageInfo.IsValid())
	{
		return FALSE;
	}

	if (pOutImageInfo->m_stImageInfo.m_nWidth != pInputDicomImage->m_stImageInfo.m_nWidth)
	{
		return FALSE;
	}

	if (pOutImageInfo->m_stImageInfo.m_nHeight != pInputDicomImage->m_stImageInfo.m_nHeight)
	{
		return FALSE;
	}

	if (nW1 >= nW2)
	{
		nW1 = nW2 - 1;
	}

	INT_PTR a = sizeof(pInputDicomImage->m_pImageData);

	switch (pOutImageInfo->m_stImageInfo.m_nSamplesPerPixel)
	{
	case 1:
	{
		if (pInputDicomImage->m_stImageInfo.m_nBytesPerPixel == 1)
		{
			if (pOutImageInfo->m_stImageInfo.m_nBytesPerPixel == 1)			// BYTE to BYTE
			{
				bRes = window_level_image<BYTE, BYTE>((BYTE*)pInputDicomImage->m_pImageData,
					(BYTE*)pOutImage,
					(UINT)pInputDicomImage->m_stImageInfo.m_nBitsPerPixel,
					(UINT)pOutImageInfo->m_stImageInfo.m_nBitsPerPixel,
					(UINT)pInputDicomImage->m_stImageInfo.m_nBytesPerLine,
					(UINT)pOutImageInfo->m_stImageInfo.m_nBytesPerLine,
					(UINT)pInputDicomImage->m_stImageInfo.m_nWidth,
					(UINT)pInputDicomImage->m_stImageInfo.m_nHeight,
					(int)nW1,
					(int)nW2);
			}
		}
		else if (pInputDicomImage->m_stImageInfo.m_nBytesPerPixel == 2)
		{
			if (pOutImageInfo->m_stImageInfo.m_nBytesPerPixel == 1)			// WORD to BYTE
			{
				bRes = window_level_image<WORD, BYTE>((WORD*)pInputDicomImage->m_pImageData,
					(BYTE*)pOutImage,
					(UINT)pInputDicomImage->m_stImageInfo.m_nBitsPerPixel,
					(UINT)pOutImageInfo->m_stImageInfo.m_nBitsPerPixel,
					(UINT)pInputDicomImage->m_stImageInfo.m_nBytesPerLine,
					(UINT)pOutImageInfo->m_stImageInfo.m_nBytesPerLine,
					(UINT)pInputDicomImage->m_stImageInfo.m_nWidth,
					(UINT)pInputDicomImage->m_stImageInfo.m_nHeight,
					(int)nW1,
					(int)nW2);
			}
			else if (pOutImageInfo->m_stImageInfo.m_nBytesPerPixel == 2)		// WORD to WORD
			{
				bRes = window_level_image<WORD, WORD>((WORD*)pInputDicomImage->m_pImageData,
					(WORD*)pOutImage,
					(UINT)pInputDicomImage->m_stImageInfo.m_nBitsPerPixel,
					(UINT)pOutImageInfo->m_stImageInfo.m_nBitsPerPixel,
					(UINT)pInputDicomImage->m_stImageInfo.m_nBytesPerLine,
					(UINT)pOutImageInfo->m_stImageInfo.m_nBytesPerLine,
					(UINT)pInputDicomImage->m_stImageInfo.m_nWidth,
					(UINT)pInputDicomImage->m_stImageInfo.m_nHeight,
					(int)nW1,
					(int)nW2);
			}
		}
	}
	break;
	case 3:
	{
		if (pOutImageInfo->m_stImageInfo.m_nSamplesPerPixel == 3)
		{
			if (pInputDicomImage->m_stImageInfo.m_nBytesPerPixel == 1)
			{
				if (pOutImageInfo->m_stImageInfo.m_nBytesPerPixel == 1)		// RGB to RGB
				{
					bRes = window_level_image<BYTE, BYTE>((BYTE*)pInputDicomImage->m_pImageData,
						(BYTE*)pOutImage,
						(UINT)pInputDicomImage->m_stImageInfo.m_nBitsPerPixel,
						(UINT)pOutImageInfo->m_stImageInfo.m_nBitsPerPixel,
						(UINT)pInputDicomImage->m_stImageInfo.m_nBytesPerLine,
						(UINT)pOutImageInfo->m_stImageInfo.m_nBytesPerLine,
						(UINT)(pInputDicomImage->m_stImageInfo.m_nWidth * 3),
						(UINT)pInputDicomImage->m_stImageInfo.m_nHeight,
						(int)nW1,
						(int)nW2);
				}
			}
		}
	}
	break;
	}

	return bRes;
}

BOOL CDicomImageProcessor::AdjustDICOMLut(void* pImage, RGBQUAD* pLUT, CDicomImage* pImageinfoDicom)
{
	if (!pLUT)
	{
		return FALSE;
	}

	if (!pImage)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	switch (pImageinfoDicom->m_stImageInfo.m_nTotalAllocatedBytes)
	{
	case 1:
	{
		if (pImageinfoDicom->m_stImageInfo.m_nSamplesPerPixel == 1)
		{
			if (adjust_dicom_lut<BYTE>((BYTE*)pImage, (RGBQUAD*)pLUT, (UINT)pImageinfoDicom->m_stImageInfo.m_nWidth, (UINT)pImageinfoDicom->m_stImageInfo.m_nHeight, (UINT)pImageinfoDicom->m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	case 2:
	{
		if (pImageinfoDicom->m_stImageInfo.m_nSamplesPerPixel == 1)
		{
			if (adjust_dicom_lut<WORD>((WORD*)pImage, (RGBQUAD*)pLUT, (UINT)pImageinfoDicom->m_stImageInfo.m_nWidth, (UINT)pImageinfoDicom->m_stImageInfo.m_nHeight, (UINT)pImageinfoDicom->m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	case 3:
	{
		if (pImageinfoDicom->m_stImageInfo.m_nSamplesPerPixel == 3)
		{
			if (adjust_dicom_lut<RGBTRIPLE*>((RGBTRIPLE**)&pImage, (RGBQUAD*)pLUT, (UINT)pImageinfoDicom->m_stImageInfo.m_nWidth, (UINT)pImageinfoDicom->m_stImageInfo.m_nHeight, (UINT)pImageinfoDicom->m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	case 4:
	{
		if (pImageinfoDicom->m_stImageInfo.m_nSamplesPerPixel == 1)
		{
			if (adjust_dicom_lut<DWORD>((DWORD*)pImage, (RGBQUAD*)pLUT, (UINT)pImageinfoDicom->m_stImageInfo.m_nWidth, (UINT)pImageinfoDicom->m_stImageInfo.m_nHeight, (UINT)pImageinfoDicom->m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
		else if (pImageinfoDicom->m_stImageInfo.m_nSamplesPerPixel == 4)
		{
			if (adjust_dicom_lut<RGBQUAD*>((RGBQUAD**)&pImage, (RGBQUAD*)pLUT, (UINT)pImageinfoDicom->m_stImageInfo.m_nWidth, (UINT)pImageinfoDicom->m_stImageInfo.m_nHeight, (UINT)pImageinfoDicom->m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	}

	return bRet;
}

BOOL CDicomImageProcessor::BurnOverlayOnImage(BYTE* pIn, BYTE* pOut, UINT nOutAllocByte, UINT nInBitsPerPixel, UINT nInBytePerLine, UINT nOutHeight, UINT nOutWidth, UINT nOutMaxValue)
{
	BOOL bRes = FALSE;

	switch (nOutAllocByte)
	{
	case 2:
		bRes = burn_overlay_on_image<BYTE, WORD>((BYTE*)pIn,
			(WORD*)pOut,
			(UINT)nInBitsPerPixel,
			(UINT)nInBytePerLine,
			(UINT)nOutHeight,
			(UINT)nOutWidth,
			(UINT)nOutMaxValue);
		break;
	case 1:
	default:
		bRes = burn_overlay_on_image<BYTE, BYTE>((BYTE*)pIn,
			(BYTE*)pOut,
			(UINT)nInBitsPerPixel,
			(UINT)nInBytePerLine,
			(UINT)nOutHeight,
			(UINT)nOutWidth,
			(UINT)nOutMaxValue);
		break;
	}

	return bRes;
}