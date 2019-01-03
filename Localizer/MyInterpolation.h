#pragma once
class CMyInterpolation
{
public:
	CMyInterpolation();
	~CMyInterpolation();

public:
	BOOL DoBilinearInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight);
	BOOL DoHighOrderInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, BOOL bSpline = TRUE);

private:
	double DoLinear(unsigned char fx1, unsigned char fx2, double dWeight);
	double DoCubicPolynomial(unsigned char ucImg[4], double x);
	double DoNaturalCubicSpline(unsigned char ucImg[4], double x);
	
	void clip(double max, double min, double& num);
	BYTE GetPixelClamped(BYTE* pImage, INT_PTR x, INT_PTR y, INT_PTR nWidth, INT_PTR nHeight);
};

