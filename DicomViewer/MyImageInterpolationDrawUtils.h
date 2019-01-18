#pragma once

#include "DicomImage.h"

class CMyImageInterpolationDrawUtils
{
public:
	CMyImageInterpolationDrawUtils(
		CDicomImage::IMAGE_INFO* pSrcImageInfo,
		BYTE* pSrcImage,
		CRect rtCanvas,
		CPoint ptPanDelta,
		double dCanvasPerImageRatio,
		double dZoomValue = 1.0, 
		INTERPOLATION_TYPE interpolationType = InterpolationTypeBilinear);

	~CMyImageInterpolationDrawUtils();

public:
	BOOL UpdateDisplayRoiImage();
	DWORD CalcScreenImageSize();
	BYTE* GetScreenImage();
	CRect GetDrawRectOnCanvas();
	CRect GetDisplayedRoiRectOnCanvas();
	void SetRectCanvas(CRect rtCanvas);
	void SetInterpolationType(INTERPOLATION_TYPE interpolationType);

protected:
	BOOL CalcImageOnCanvasRect();
	BOOL CalcDisplayedRoiOnImageRect();
	void CalcDisplayedRoiOnCanvasRect();
	void CalcDrawRectOnCanvasRect();
	BOOL CopyROIImage(BYTE* pSrc, BYTE* pDest, INT_PTR nSrcWidth, INT_PTR nSrcHeight, INT_PTR nSamplePerPixel, CRect rtRoi);
	BOOL AllocInterpolatedImage();
	void FreeInterpolatedImage();
	UINT BytesPerLine(UINT nPixelCount, UINT nBitPerPixel);
	

protected:
	CDicomImage::IMAGE_INFO* m_pSrcImageInfo;
	BYTE* m_pSrcImage;
	BYTE* m_pInterpolatedImage;
	//
	CRect m_rtCanvas;
	double m_dZoomValue;
	double m_dCanvasPerImageRatio;
	CPoint m_ptPanDelta;
	//
	Gdiplus::RectF m_rtImageOnCanvas;
	CRect m_rtDrawRectOnCanvas;
	CRect m_rtDisplayedRoiOnImage;
	CRect m_rtDisplayedRoiOnCanvas;
	//
	INTERPOLATION_TYPE m_interpolationType;
};

