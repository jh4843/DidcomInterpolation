#pragma once
#include "DicomImage.h"

class CDicomImage;
class CDicomImageProcessor
{
public:
	CDicomImageProcessor();
	~CDicomImageProcessor();

public:
	BOOL ProcessWindowLevel(BYTE* pOutImage, CDicomImage* pOutImageInfo, CDicomImage* pInputDicomImage, INT_PTR nW1, INT_PTR nW2);
	BOOL AdjustDICOMLut(void* pImage, RGBQUAD* pLUT, CDicomImage* pImageInfo);
	BOOL BurnOverlayOnImage(BYTE* pIn, BYTE* pOut, UINT nOutAllocByte, UINT nInBitsPerPixel, UINT nInBytePerLine, UINT nInHeight, UINT nOutWidth, UINT nOutMaxValue);
};

