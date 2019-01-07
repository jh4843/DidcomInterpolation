#pragma once

#include "MyCalc.h"

class CMyInterpolation
{
public:
	CMyInterpolation();
	~CMyInterpolation();

public:
	BOOL DoBilinearInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight);
	BOOL DoBiCubicInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, BOOL bBSpline = FALSE, double a = -0.5);
	BOOL DoLanczosInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, double a = 3);
	BOOL DoMitchellInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, double B = 0.333, double C = 0.333);
	BOOL DoCatmullRomSplineInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight);
	BOOL DoHighOrderInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, BOOL bSpline = TRUE);
	//
	void SetUseParallelCalc(BOOL bUse);

private:
	double DoLinear(unsigned char fx1, unsigned char fx2, double dWeight);
	double DoCubicKernel(unsigned char ucImg[4], double x, double a);
	double DoBSplineKernel(unsigned char ucImg[4], double x);
	double DoLanczos(unsigned char ucImg[4], double x, double a);
	double DoMitchell(unsigned char ucImg[4], double x, double B, double C);
	//
	double DoCubicPolynomial(unsigned char ucImg[4], double x);
	double DoNaturalCubicSpline(unsigned char ucImg[4], double x);
	
	void clip(double max, double min, double& num);
	BYTE GetPixelClamped(BYTE* pImage, INT_PTR x, INT_PTR y, INT_PTR nWidth, INT_PTR nHeight);

private:
	CMyCalc m_CalcUtil;
	BOOL m_bUseParallelCalc;
};

