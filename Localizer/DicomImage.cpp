#include "stdafx.h"
#include "DicomImage.h"
#include "DicomImageProcessor.h"

CDicomImage::CDicomImage()
{
	m_pImageData = nullptr;
	m_bTiledPixelData = FALSE;

	m_stImageInfo.Init();
}

CDicomImage::CDicomImage(const CDicomImage& dicomImage)
{
	*this = dicomImage;
}

CDicomImage::~CDicomImage()
{
	FreeDicomImage();
}

void CDicomImage::Init()
{
	m_stImageInfo.Init();
}

BOOL CDicomImage::LoadDicomImage(pBITMAPHANDLE pBitmapHandle, pBITMAPHANDLE pOverlayBitmapHandle)
{
	TRY
	{
		FreeDicomImage();

		int nPixelDataSize = GetImageSize();
		m_pImageData = (BYTE*)::VirtualAlloc(NULL, nPixelDataSize, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE);
		if (!m_pImageData)
		{
			AfxThrowMemoryException();
			return FALSE;
		}

// 		LBitmap lBitmap;
// 		lBitmap.SetHandle(pBitmapHandle);
// 		L_INT nRet = lBitmap.SetOverlay(0, pOverlayBitmapHandle, OVERLAY_COPY);
// 
// 		pBitmapHandle = lBitmap.GetHandle();

		ZeroMemory(m_pImageData, nPixelDataSize);

		if (GetTiledPiexlData() == TRUE)
		{
			LBitmap lBitmapObj;
			L_INT nRet = lBitmapObj.SetHandle(pBitmapHandle);
			if (nRet != SUCCESS)
			{
				AfxThrowUserException();
			}

			BOOL bRet = CopyLead2UserBuf(&lBitmapObj, (void*)m_pImageData, m_stImageInfo.m_nWidth, m_stImageInfo.m_nHeight, m_stImageInfo.m_nTotalAllocatedBytes);

			if (!bRet)
			{
				AfxThrowUserException();
			}
		}
		else
		{
			UINT nLine = 0, nTotalSize = 0;
			UINT nDestLine, nSrcLine;
			UINT nCopySize;
			BYTE* pDest = nullptr;
			BYTE* pSrc = nullptr;
			nTotalSize = m_stImageInfo.m_nHeight;
			nDestLine = m_stImageInfo.m_nBytesPerLine;
			nSrcLine = (UINT)pBitmapHandle->BytesPerLine;
			nCopySize = m_stImageInfo.m_nBytesPerLine;
			pDest = (BYTE*)m_pImageData;
			pSrc = (BYTE*)pBitmapHandle->Addr.Windows.pData;
			for (nLine = 0; nLine < nTotalSize; nLine++, pDest += nDestLine, pSrc += nSrcLine)
			{
				memcpy_s(pDest, nDestLine, pSrc, nCopySize);
			}
		}

		if (pOverlayBitmapHandle)
		{
			CDicomImageProcessor* pImgProcessor = new CDicomImageProcessor;

			pImgProcessor->BurnOverlayOnImage(pOverlayBitmapHandle->Addr.Windows.pData,
				m_pImageData,
				(UINT)m_stImageInfo.m_nTotalAllocatedBytes,
				(UINT)pOverlayBitmapHandle->BitsPerPixel,
				(UINT)pOverlayBitmapHandle->BytesPerLine,
				(UINT)m_stImageInfo.m_nHeight,
				(UINT)m_stImageInfo.m_nWidth,
				(UINT)Bits2MaxValue(m_stImageInfo.m_nBitsPerPixel));

			delete pImgProcessor;
		}

		if (pBitmapHandle->pLUT)
		{
			CDicomImageProcessor* pDicomImageProcessor = new CDicomImageProcessor;
			if (!pDicomImageProcessor->AdjustDICOMLut(m_pImageData, pBitmapHandle->pLUT, this))
			{
				FreeDicomImage();
				delete pDicomImageProcessor;
				return FALSE;
			}

			delete pDicomImageProcessor;
		}
	}
	CATCH_ALL(e)
	{
		if (e->IsKindOf(RUNTIME_CLASS(CUserException)))
		{
			return FALSE;
		}
		else
		{
			THROW_LAST();
			return FALSE;
		}
	}
	END_CATCH_ALL

	return TRUE;
}

BOOL CDicomImage::FreeDicomImage()
{
	if (m_pImageData)
	{
		int nPixelDataSize = m_stImageInfo.m_nWidth * m_stImageInfo.m_nHeight * m_stImageInfo.m_nTotalAllocatedBytes;
		::VirtualFree(m_pImageData, nPixelDataSize, MEM_DECOMMIT);
		::VirtualFree(m_pImageData, 0, MEM_RELEASE);
		// 		delete[] m_pImageData;
		m_pImageData = nullptr;
	}

	return TRUE;
}



BOOL CDicomImage::CopyLead2UserBuf(LBitmap* pBitmap, void* pImage, int nWidth, int nHeight, int iAllocBytes)
{
	if (!pBitmap)
	{
		return FALSE;
	}

	if (!pImage)
	{
		return FALSE;
	}

	L_INT nSrcHeight = pBitmap->GetHeight();
	L_INT nSrcBytesPerLine = pBitmap->GetBytesPerLine();

	L_INT nDesHeight = nHeight;
	L_INT nDestBytesPerLine = nWidth*iAllocBytes;

	LBuffer lBuffLine;
	L_CHAR* pBuffLine = nullptr;

	pBitmap->Access();
	lBuffLine.Reallocate(nSrcBytesPerLine);
	pBuffLine = (L_CHAR*)lBuffLine.Lock();

	for (int nIndex = 0; nIndex < min(nSrcHeight, nDesHeight); nIndex++)
	{
		pBitmap->GetRow(&lBuffLine, nIndex);
		memcpy((BYTE*)pImage + nDestBytesPerLine*nIndex, pBuffLine, min(nSrcBytesPerLine, nDestBytesPerLine));
	}

	lBuffLine.Unlock();
	pBitmap->Release();

	return TRUE;
}

UINT CDicomImage::BytesPerLine(UINT nPixelCount, UINT nBitPerPixel)
{
	return ((nPixelCount * nBitPerPixel + 31) & (~31)) / 8;
}

UINT CDicomImage::GetImageSize()
{
	UINT nBytesPerPixel = (UINT)Bits2Bytes(m_stImageInfo.m_nBitsPerPixel);
	UINT nTotalAllocatedBytes = (UINT)(nBytesPerPixel * m_stImageInfo.m_nSamplesPerPixel);
	//
	UINT nSize = m_stImageInfo.m_nWidth *  m_stImageInfo.m_nHeight * nTotalAllocatedBytes;
	return nSize;
}

void CDicomImage::GetImageProcessedImage(BYTE* pOutImage, CDicomImage* pOutImageInfo)
{
	if (!m_pImageData)
	{
		return ;
	}

	if (!m_stImageInfo.IsValid())
	{
		return ;
	}

	float fW1 = m_stImageInfo.m_fW1;
	float fW2 = m_stImageInfo.m_fW2;

	CDicomImageProcessor* pImageProcessor = new CDicomImageProcessor;
	CDicomImage* pInputDicomImage = this;

	pImageProcessor->ProcessWindowLevel(pOutImage, pOutImageInfo, pInputDicomImage, fW1, fW2);

	delete pImageProcessor;

	return ;
}

void CDicomImage::SetTiledPiexlData(BOOL bSet)
{
	m_bTiledPixelData = bSet;
}

BOOL CDicomImage::GetTiledPiexlData()
{
	return m_bTiledPixelData;
}

void CDicomImage::operator=(const CDicomImage& obj)
{
	m_stImageInfo = obj.m_stImageInfo;

	FreeDicomImage();

	if (!obj.m_pImageData)
		return;

	int nPixelDataSize = GetImageSize();
	m_pImageData = (BYTE*)::VirtualAlloc(NULL, nPixelDataSize, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE);

	if (!m_pImageData)
	{
		FreeDicomImage();
		return;
	}

	memcpy_s(m_pImageData, nPixelDataSize, obj.m_pImageData, nPixelDataSize);
}