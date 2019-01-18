#include "stdafx.h"
#include "MyImageInterpolationDrawUtils.h"
#include "MyInterpolation.h"

CMyImageInterpolationDrawUtils::CMyImageInterpolationDrawUtils(
	CDicomImage::IMAGE_INFO* pSrcImageInfo, 
	BYTE* pSrcImage, 
	CRect rtCanvas, 
	CPoint ptPanDelta, 
	double dCanvasPerImageRatio,
	double dZoomValue, 
	INTERPOLATION_TYPE interpolationType)
{
	ASSERT(pSrcImageInfo);
	ASSERT(pSrcImage);

	m_pSrcImageInfo = pSrcImageInfo;
	m_pSrcImage = pSrcImage;
	//
	m_pInterpolatedImage = nullptr;
	//m_dOldZoomValue = 0.0;
	m_rtCanvas = rtCanvas;
	m_dCanvasPerImageRatio = dCanvasPerImageRatio;
	m_dZoomValue = dZoomValue;
	m_ptPanDelta = ptPanDelta;
	//
	m_rtImageOnCanvas = Gdiplus::RectF(0.0f, 0.0f, 0.0f, 0.0f);
	m_rtDrawRectOnCanvas.SetRectEmpty();
	m_rtDisplayedRoiOnImage.SetRectEmpty();
	m_rtDisplayedRoiOnCanvas.SetRectEmpty();
	//
	m_interpolationType = interpolationType;
}


CMyImageInterpolationDrawUtils::~CMyImageInterpolationDrawUtils()
{
	//
	FreeInterpolatedImage();
}

BOOL CMyImageInterpolationDrawUtils::UpdateDisplayRoiImage()
{
	BOOL bRet = TRUE;

	if (!m_pSrcImageInfo)
		return FALSE;

	if (!m_pSrcImage)
		return FALSE;

	if (!m_rtCanvas)
		return FALSE;

	INT_PTR nOrgWidth = BytesPerLine((UINT)m_pSrcImageInfo->m_nWidth, 8);
	INT_PTR nOrgHeight =BytesPerLine((UINT)m_pSrcImageInfo->m_nHeight, 8);
	INT_PTR nSamplePerPixel = m_pSrcImageInfo->m_nSamplesPerPixel;

	CDicomImage::IMAGE_INFO imageInfo = *m_pSrcImageInfo;

	if (!CalcImageOnCanvasRect())
	{
		return FALSE;
	}

	if (!CalcDisplayedRoiOnImageRect())
	{
		return FALSE;
	}

	//
	CalcDisplayedRoiOnCanvasRect();
	
	//
	CalcDrawRectOnCanvasRect();

	if (m_rtDisplayedRoiOnImage.Width() <= 0 || m_rtDisplayedRoiOnImage.Height() <= 0)
	{
		m_pInterpolatedImage = nullptr;
	}

	INT_PTR nRoiSize = m_rtDisplayedRoiOnImage.Width() * m_rtDisplayedRoiOnImage.Height() * nSamplePerPixel;

	if (nRoiSize <= 0)
		return FALSE;

	BYTE* pRoiImage = nullptr;
	//
	TRY
	{
		pRoiImage = new BYTE[nRoiSize];

		bRet = CopyROIImage(m_pSrcImage,
			pRoiImage,
			nOrgWidth,
			nOrgHeight,
			nSamplePerPixel,
			m_rtDisplayedRoiOnImage);

		if (!bRet)
		{
			AfxThrowUserException();
		}

		//
		if (!pRoiImage)
		{
			delete[] pRoiImage;
			pRoiImage = nullptr;
			return FALSE;
		}

		FreeInterpolatedImage();
		AllocInterpolatedImage();

		CMyInterpolation interAlgo;
		interAlgo.SetUseParallelCalc(TRUE);

		switch (m_interpolationType)
		{
		case InterpolationTypeBilinear:
			interAlgo.DoBilinearInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel);
			break;
		case InterpolationTypeBicubicPolynomial_100:
			interAlgo.DoBiCubicInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				FALSE,
				-1.0);
			break;
		case InterpolationTypeBicubicPolynomial_050:
			interAlgo.DoBiCubicInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				FALSE,
				-0.5);
			break;
		case InterpolationTypeBicubicPolynomial_075:
			interAlgo.DoBiCubicInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				FALSE,
				-0.75);
			break;

		case InterpolationTypeBicubicPolynomial_000:
			interAlgo.DoBiCubicInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				FALSE,
				0.0);
			break;
		case InterpolationTypeBicubicPolynomial_300:
			interAlgo.DoBiCubicInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				FALSE,
				-3.0);
			break;
		case InterpolationTypeBicubicBSpline:
			interAlgo.DoBiCubicInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				TRUE);

			break;
		case InterpolationTypeBicubicLanczos:
			interAlgo.DoLanczosInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				2.0);

			break;
		case InterpolationTypeBicubicMichell:
			interAlgo.DoMitchellInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel,
				0.33333,
				0.33333);

			break;
		case InterpolationTypeBicubicCatmullRom:
			interAlgo.DoCatmullRomSplineInterpolation(pRoiImage,
				m_pInterpolatedImage,
				m_rtDisplayedRoiOnImage.Width(),
				m_rtDisplayedRoiOnImage.Height(),
				m_rtDisplayedRoiOnCanvas.Width(),
				m_rtDisplayedRoiOnCanvas.Height(),
				nSamplePerPixel);
			break;
		}



// 		delete[] pRoiImage;
// 		pRoiImage = nullptr;
	}
	CATCH_ALL(e)
	{
		bRet = FALSE;
		//
		if (e->IsKindOf(RUNTIME_CLASS(CMemoryException)))
		{
			AfxThrowMemoryException();
		}
	}
	END_CATCH_ALL

	//
	if (pRoiImage)
	{
		delete[] pRoiImage;
		pRoiImage = nullptr;
	}

	return bRet;
}

DWORD CMyImageInterpolationDrawUtils::CalcScreenImageSize()
{
	ASSERT(m_pSrcImageInfo);
	INT_PTR nSceenImageSize = m_rtDisplayedRoiOnCanvas.Width() * 
		m_pSrcImageInfo->m_nSamplesPerPixel *
		m_rtDisplayedRoiOnCanvas.Height();
	//
	return nSceenImageSize;
}

BYTE* CMyImageInterpolationDrawUtils::GetScreenImage()
{
	return m_pInterpolatedImage;
}

CRect CMyImageInterpolationDrawUtils::GetDrawRectOnCanvas()
{
	return m_rtDrawRectOnCanvas;
}

CRect CMyImageInterpolationDrawUtils::GetDisplayedRoiRectOnCanvas()
{
	return m_rtDisplayedRoiOnCanvas;
}

void CMyImageInterpolationDrawUtils::SetRectCanvas(CRect rtCanvas)
{
	m_rtCanvas = rtCanvas;
}

void CMyImageInterpolationDrawUtils::SetInterpolationType(INTERPOLATION_TYPE interpolationType)
{
	m_interpolationType = interpolationType;
}

BOOL CMyImageInterpolationDrawUtils::CalcImageOnCanvasRect()
{
	if (m_pSrcImageInfo->m_nWidth < 0)
	{
		return FALSE;
	}

	if (m_pSrcImageInfo->m_nHeight < 0)
	{
		return FALSE;
	}

	double dImgWidth = 0., dImgHeight = 0.;
	double dImgWidthOnCanvas = 0., dImgHeightOnCanvas = 0.;
	double dCanvasWidth = 0., dCanvasHeight = 0.;
	double dRatio = 0.;
	double dRatioBasedOnHeight = 0.;
	double dRatioBasedOnWidth = 0.;

	//
	dImgWidth = (double)m_pSrcImageInfo->m_nWidth;
	dImgHeight = (double)m_pSrcImageInfo->m_nHeight;
	dCanvasWidth = (double)m_rtCanvas.Width();
	dCanvasHeight = (double)m_rtCanvas.Height();

	dRatioBasedOnHeight = dCanvasHeight / dImgHeight;
	dRatioBasedOnWidth = dCanvasWidth / dImgWidth;

	if (dRatioBasedOnHeight < dRatioBasedOnWidth)
	{
		dRatio = dRatioBasedOnHeight;
	}
	else
	{
		dRatio = dRatioBasedOnWidth;
	}

	dImgWidthOnCanvas = dImgWidth * dRatio * m_dZoomValue;
	dImgHeightOnCanvas = dImgHeight * dRatio * m_dZoomValue;

	m_rtImageOnCanvas.X = (dCanvasWidth * 0.5f) - (dImgWidthOnCanvas * 0.5);
	m_rtImageOnCanvas.Width = dImgWidthOnCanvas;
	m_rtImageOnCanvas.Y = (dCanvasHeight * 0.5f) - (dImgHeightOnCanvas * 0.5);
	m_rtImageOnCanvas.Height = dImgHeightOnCanvas;

	m_rtImageOnCanvas.X += (double)m_ptPanDelta.x * m_dCanvasPerImageRatio / m_dZoomValue - 0.5;
	m_rtImageOnCanvas.Y += (double)m_ptPanDelta.y * m_dCanvasPerImageRatio / m_dZoomValue - 0.5;

	return TRUE;
}

BOOL CMyImageInterpolationDrawUtils::CalcDisplayedRoiOnImageRect()
{
	INT_PTR nImageWidth = (INT_PTR)m_pSrcImageInfo->m_nWidth;
	INT_PTR nImageHeight = (INT_PTR)m_pSrcImageInfo->m_nHeight;

	double nCanvasHeight = (double)m_rtCanvas.Height();
	double nCanvasWidth = (double)m_rtCanvas.Width();

	double dWidthRatio = (double)nImageWidth / (double)m_rtImageOnCanvas.Width;
	double dHeightRatio = (double)nImageHeight / (double)m_rtImageOnCanvas.Height;

	CRect rtOldRoiImage = m_rtDisplayedRoiOnImage;

	if (m_rtImageOnCanvas.GetLeft() > nCanvasWidth || 
		m_rtImageOnCanvas.GetTop() > nCanvasHeight ||
		m_rtImageOnCanvas.GetRight() < 0 || 
		m_rtImageOnCanvas.GetBottom() < 0)
	{
		m_rtDisplayedRoiOnImage.SetRectEmpty();

		return TRUE;
	}

	if (m_rtImageOnCanvas.GetLeft() > 0) // 좌측이 다 보이는경우,
	{
		m_rtDisplayedRoiOnImage.left = 0;
	}
	else // 좌측이 다 보이지 않는 경우
	{
		m_rtDisplayedRoiOnImage.left = m_rtImageOnCanvas.X * dWidthRatio * (-1);
		//rtDisplayedROIOnImage.left = m_rtImage.left;
	}

	if (m_rtImageOnCanvas.GetRight() > nCanvasWidth) // 우측이 다 보이는 경우
	{
		m_rtDisplayedRoiOnImage.right = nImageWidth - ((m_rtImageOnCanvas.GetRight() - nCanvasWidth) * dWidthRatio);
	}
	else // 우측이 다 보이지 않는 경우
	{
		m_rtDisplayedRoiOnImage.right = nImageWidth;
	}

	if (m_rtImageOnCanvas.GetTop() > 0) // 상단이 다 보이는 경우,
	{
		m_rtDisplayedRoiOnImage.top = 0;
	}
	else // 상단이 다 보이지 않는 경우
	{
		m_rtDisplayedRoiOnImage.top = m_rtImageOnCanvas.GetTop() * dHeightRatio * (-1);
	}

	if (m_rtImageOnCanvas.GetBottom() < nCanvasHeight) // 하단이 다 보이는 경우,
	{
		m_rtDisplayedRoiOnImage.bottom = nImageHeight;
	}

	else // 하단이 다 보이지 않는 경우,
	{
		m_rtDisplayedRoiOnImage.bottom = nImageHeight - ((m_rtImageOnCanvas.GetBottom() - nCanvasHeight) * dHeightRatio);
	}

	INT_PTR nRoiImageWidth = m_rtDisplayedRoiOnImage.Width();
	INT_PTR nRoiImageHeight = m_rtDisplayedRoiOnImage.Height();

	m_rtDisplayedRoiOnImage.right = m_rtDisplayedRoiOnImage.left + BytesPerLine(nRoiImageWidth, 8);
	m_rtDisplayedRoiOnImage.bottom = m_rtDisplayedRoiOnImage.top + BytesPerLine(nRoiImageHeight, 8);

	if (m_rtDisplayedRoiOnImage.right > nImageWidth)
	{
		m_rtDisplayedRoiOnImage.right = nImageWidth;
	}

	if (m_rtDisplayedRoiOnImage.bottom > nImageHeight)
	{
		m_rtDisplayedRoiOnImage.bottom = nImageHeight;
	}

	//if (m_dOldZoomValue != m_pSrcUIInstanceBase->GetDispStatus()->dZoomValue &&
	if (m_rtDisplayedRoiOnImage.Width() != nImageWidth &&	// 영상이 전부 보일 때는 고려하지 않는다.
		m_rtDisplayedRoiOnImage.Height() != nImageHeight)
	{
		if (m_rtDisplayedRoiOnImage.left != rtOldRoiImage.left &&
			m_rtDisplayedRoiOnImage.Width() == rtOldRoiImage.Width())
		{
			double dRatio = (double)rtOldRoiImage.Height() / (double)rtOldRoiImage.Width();
			INT_PTR nDiff = m_rtDisplayedRoiOnImage.left - rtOldRoiImage.left;
			m_rtDisplayedRoiOnImage.right = rtOldRoiImage.right - nDiff;

			double dHeight = (double)m_rtDisplayedRoiOnImage.Width() * dRatio;
			m_rtDisplayedRoiOnImage.top = ((double)m_rtDisplayedRoiOnImage.top + (double)m_rtDisplayedRoiOnImage.bottom - dHeight) / 2;
			m_rtDisplayedRoiOnImage.bottom = (double)m_rtDisplayedRoiOnImage.top + dHeight;

			//			m_pSrcUIInstanceBase->GetDispStatus()->rtDisplayedROIOnImage = rtOldRoiImage;
			return FALSE;
		}

		if (m_rtDisplayedRoiOnImage.top != rtOldRoiImage.top &&
			(m_rtDisplayedRoiOnImage.Height() == rtOldRoiImage.Height()))
		{
			double dRatio = (double)rtOldRoiImage.Width() / (double)rtOldRoiImage.Height();
			INT_PTR nDiff = m_rtDisplayedRoiOnImage.top - rtOldRoiImage.top;
			m_rtDisplayedRoiOnImage.bottom = rtOldRoiImage.bottom - nDiff;

			double dWidth = (double)m_rtDisplayedRoiOnImage.Height() * dRatio;
			m_rtDisplayedRoiOnImage.left = ((double)m_rtDisplayedRoiOnImage.left + (double)m_rtDisplayedRoiOnImage.right - dWidth) / 2;
			m_rtDisplayedRoiOnImage.right = (double)m_rtDisplayedRoiOnImage.left + dWidth;

			//			m_rtDisplayedROIOnImage = rtOldRoiImage;
			return FALSE;
		}
	}

	//m_dOldZoomValue = m_pSrcUIInstanceBase->GetDispStatus()->dZoomValue;
	return TRUE;
}

void CMyImageInterpolationDrawUtils::CalcDisplayedRoiOnCanvasRect()
{
	if (m_rtDisplayedRoiOnImage.Width() == 0 || m_rtDisplayedRoiOnImage.Height() == 0)
	{
		m_rtDisplayedRoiOnImage.SetRectEmpty();
		return;
	}

	INT_PTR nImageWidth = (INT_PTR)m_pSrcImageInfo->m_nWidth;
	INT_PTR nImageHeight = (INT_PTR)m_pSrcImageInfo->m_nHeight;

	double dCanvasHeight = (double)m_rtCanvas.Height();
	double dCanvasWidth = (double)m_rtCanvas.Width();

	////////////////////////////////////////////////////////////////////////

	m_rtDisplayedRoiOnCanvas.left = m_rtImageOnCanvas.X;
	m_rtDisplayedRoiOnCanvas.right = m_rtImageOnCanvas.Width + m_rtImageOnCanvas.X;
	m_rtDisplayedRoiOnCanvas.top = m_rtImageOnCanvas.Y;
	m_rtDisplayedRoiOnCanvas.bottom = m_rtImageOnCanvas.Height + m_rtImageOnCanvas.Y;

	if (m_rtDisplayedRoiOnCanvas.left < 0)
	{
		m_rtDisplayedRoiOnCanvas.left = 0;
	}

	if (m_rtDisplayedRoiOnCanvas.right > dCanvasWidth)
	{
		m_rtDisplayedRoiOnCanvas.right = dCanvasWidth;
	}

	if (m_rtDisplayedRoiOnCanvas.top < 0)
	{
		m_rtDisplayedRoiOnCanvas.top = 0;
	}

	if (m_rtDisplayedRoiOnCanvas.bottom > dCanvasHeight)
	{
		m_rtDisplayedRoiOnCanvas.bottom = dCanvasHeight;
	}

	INT_PTR nRoiCanvasWidth = m_rtDisplayedRoiOnCanvas.Width();
	INT_PTR nRoiCanvasHeight = m_rtDisplayedRoiOnCanvas.Height();

	m_rtDisplayedRoiOnCanvas.right = m_rtDisplayedRoiOnCanvas.left + BytesPerLine(nRoiCanvasWidth, 8);
	m_rtDisplayedRoiOnCanvas.bottom = m_rtDisplayedRoiOnCanvas.top + BytesPerLine(nRoiCanvasHeight, 8);
}

void CMyImageInterpolationDrawUtils::CalcDrawRectOnCanvasRect()
{
	if (m_rtDisplayedRoiOnImage.Width() == 0 || m_rtDisplayedRoiOnImage.Height() == 0)
	{
		m_rtDisplayedRoiOnImage.SetRectEmpty();
		return;
	}

	double nCanvasHeight = (double)m_rtCanvas.Height();
	double nCanvasWidth = (double)m_rtCanvas.Width();

	if (m_rtImageOnCanvas.GetLeft() < 0)
	{
		m_rtDrawRectOnCanvas.left = 0;
	}
	else
	{
		m_rtDrawRectOnCanvas.left = (-1) * m_rtDisplayedRoiOnCanvas.left;
	}

	m_rtDrawRectOnCanvas.right = nCanvasWidth + m_rtDrawRectOnCanvas.left;

	INT_PTR nTopSpare = m_rtDisplayedRoiOnCanvas.top;
	INT_PTR nBottomSpare = nCanvasHeight - m_rtDisplayedRoiOnCanvas.Height();

	m_rtDrawRectOnCanvas.top = m_rtDisplayedRoiOnCanvas.top - nBottomSpare;
	m_rtDrawRectOnCanvas.bottom = m_rtDisplayedRoiOnCanvas.Height() + nTopSpare;

	double dDrawCanvasWidth = BytesPerLine((UINT)(m_rtDrawRectOnCanvas.Width()), 8);
	double dDrawCanvasHeight = BytesPerLine((UINT)(m_rtDrawRectOnCanvas.Height()), 8);

	m_rtDrawRectOnCanvas.right = m_rtDrawRectOnCanvas.left + dDrawCanvasWidth;
	m_rtDrawRectOnCanvas.bottom = m_rtDrawRectOnCanvas.top + dDrawCanvasHeight;
}

BOOL CMyImageInterpolationDrawUtils::CopyROIImage(BYTE* pSrc, BYTE* pDest, INT_PTR nSrcWidth, INT_PTR nSrcHeight, INT_PTR nSamplePerPixel, CRect rtRoi)
{
	if (rtRoi.IsRectEmpty())
		return FALSE;

	UINT nLine = 0, nCopyHeight = 0;
	UINT nDestLine, nSrcLine;

	nCopyHeight = rtRoi.Height();
	nDestLine = rtRoi.Width() * nSamplePerPixel;
	nSrcLine = (UINT)nSrcWidth * nSamplePerPixel;

	pSrc += (rtRoi.top * nSrcLine);
	pSrc += (rtRoi.left * nSamplePerPixel);

	for (nLine = 0; nLine < nCopyHeight; nLine++, pDest += nDestLine, pSrc += nSrcLine)
	{
		memcpy_s(pDest, nDestLine, pSrc, rtRoi.Width()*nSamplePerPixel);
	}

	//
	return TRUE;
}

BOOL CMyImageInterpolationDrawUtils::AllocInterpolatedImage()
{
	BOOL bRet = TRUE;

	INT_PTR nSceenSize = CalcScreenImageSize();
	if (nSceenSize <= 0)
		return FALSE;

	m_pInterpolatedImage = new BYTE[nSceenSize];

	if (!m_pInterpolatedImage)
	{
		return FALSE;
	}

	memset(m_pInterpolatedImage, 0, nSceenSize);

	return bRet;
}

void CMyImageInterpolationDrawUtils::FreeInterpolatedImage()
{
	if (m_pInterpolatedImage)
	{
		delete[] m_pInterpolatedImage;
		m_pInterpolatedImage = nullptr;
	}
}

UINT CMyImageInterpolationDrawUtils::BytesPerLine(UINT nPixelCount, UINT nBitPerPixel)
{
	return ((nPixelCount * nBitPerPixel + 31) & (~31)) / 8;
}
