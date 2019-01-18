#pragma once

class CMyInterpolation
{
public:
	CMyInterpolation(BOOL bUseParallelCalc = TRUE);
	~CMyInterpolation();

public:
	BOOL DoBilinearInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, UINT nSamplePerPixel);
	BOOL DoBiCubicInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, UINT nSamplePerPixel, BOOL bBSpline = FALSE, double a = -0.5);
	BOOL DoLanczosInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, UINT nSamplePerPixel, double a = 3);
	BOOL DoMitchellInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, UINT nSamplePerPixel, double B = 0.333, double C = 0.333);
	BOOL DoCatmullRomSplineInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, UINT nSamplePerPixel);
	//
	void SetUseParallelCalc(BOOL bUse);
	void SetThreadCount(INT_PTR nCount);

private:
	unsigned char DoLinear(unsigned char fx1, unsigned char fx2, double dWeight);
	unsigned char DoCubicKernel(unsigned char ucImg[4], double x, double a);
	unsigned char DoBSplineKernel(unsigned char ucImg[4], double x);
	unsigned char DoLanczos(unsigned char ucImg[4], double x, double a);
	unsigned char DoMitchell(unsigned char ucImg[4], double x, double B, double C);
	//
	unsigned char DoCubicPolynomial(unsigned char ucImg[4], double x);
	unsigned char DoNaturalCubicSpline(unsigned char ucImg[4], double x);
	
	void clip(double max, double min, double& num);
	unsigned char GetPixelClamped(unsigned char* pImage, INT_PTR x, INT_PTR y, INT_PTR nWidth, INT_PTR nHeight, INT_PTR nSample, INT_PTR nSamplePerPixel);

private:
	BOOL m_bUseParallelCalc;
	static INT_PTR m_nOmpProcCount;
};

