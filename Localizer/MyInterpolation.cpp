#include "stdafx.h"
#include "MyInterpolation.h"
#include "opencv2\opencv.hpp"

#define CLAMP(v, min, max) if (v < min) { v = min; } else if (v > max) { v = max; } 

CMyInterpolation::CMyInterpolation()
{
}


CMyInterpolation::~CMyInterpolation()
{
}

BOOL CMyInterpolation::DoBilinearInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight)
{
	UINT nrow, ncol, nIndex, nAllow1, nAllow2;
	double dEWweight, dNSweight, dEWtop, dEWbottom;
	INT_PTR nSrcCol, nSrcRow;
	double dDestCol, dDestRow;
	double dWidthScale, dHeightScale;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
	UCHAR NW, NE, SW, SE;
	ULONG nAddress, nSrcImageLength;
	BYTE* pLineBuffer = NULL;

	TRY
	{
		pLineBuffer = new BYTE[nDestWidth];	memset(pLineBuffer, 0, nDestWidth);

		dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
		dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;

		dWidthScale = dOutWidth / dInWidth;
		dHeightScale = dOutHeight / dInHeight;

		nAllow1 = (dWidthScale > 1) ? (UINT)dWidthScale + 1 : (2);
		nAllow2 = (dHeightScale > 1) ? (UINT)dHeightScale + 1 : (2);

		nSrcImageLength = nDestWidth * nDestHeight;

		for (nrow = 0; nrow < nDestHeight; nrow++)
		{
			nIndex = 0;
			for (ncol = 0; ncol < nDestWidth; ncol++)
			{
				dDestCol = (double)(ncol / dWidthScale);
				dDestRow = (double)(nrow / dHeightScale);

				nSrcCol = (int)dDestCol;
				nSrcRow = (int)dDestRow;

				dEWweight = dDestCol - nSrcCol;
				dNSweight = dDestRow - nSrcRow;

				NW = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow    , nSrcWidth, nSrcHeight);  
				NE = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow    , nSrcWidth, nSrcHeight);
				SW = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow + 1, nSrcWidth, nSrcHeight);
				SE = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);

				dEWtop = DoLinear(NW, NE, dEWweight);
				dEWbottom = DoLinear(SW, SE, dEWweight);

				*(pLineBuffer + nIndex) = (BYTE)(DoLinear(dEWtop, dEWbottom, dNSweight));
				nIndex++;
			}
			memcpy(pDestImage + nrow * nDestWidth, pLineBuffer, nDestWidth);
		}

		delete[] pLineBuffer;
	}
		CATCH_ALL(e)
	{
		if (e->IsKindOf(RUNTIME_CLASS(CUserException)))
		{
			return 0L;
		}
		else
		{
			return (LRESULT)e;
		}
	}
	END_CATCH_ALL

		return TRUE;
}

BOOL CMyInterpolation::DoHighOrderInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, BOOL bSpline)
{
	SYSTEM_INFO systemInfo;
	DWORD dwCpuCount;
	GetSystemInfo(&systemInfo);
	dwCpuCount = systemInfo.dwNumberOfProcessors;
	
	if (dwCpuCount < 1)
		dwCpuCount = 1;

	int nrow, ncol, nIndex, nAllow1, nAllow2;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
	INT_PTR nSrcCol, nSrcRow;
	double dDestCol, dDestRow;
	double dWidthScale, dHeightScale;
	ULONG nAddress, nSrcImageLength;

	BYTE* pLineBuffer = new BYTE[nDestWidth];	memset(pLineBuffer, 0, nDestWidth);
	unsigned char image[4][4];	memset(image, 0, sizeof(image));

	dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
	dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;

	dWidthScale = dOutWidth / dInWidth;
	dHeightScale = dOutHeight / dInHeight;

	nAllow1 = (dWidthScale > 1) ? (UINT)dWidthScale + 3 : (3);
	nAllow2 = (dHeightScale > 1) ? (UINT)dHeightScale + 3 : (3);

	nSrcImageLength = nSrcWidth * nSrcHeight;

//#pragma omp parallel for private(ncol) num_threads(dwCpuCount)
	for (nrow = 0; nrow < nDestHeight; nrow++)
	{
		nIndex = 0;
		for (ncol = 0; ncol < nDestWidth; ncol++)
		{
			dDestCol = (double)(ncol / dWidthScale);
			dDestRow = (double)(nrow / dHeightScale);

			nSrcCol = (int)dDestCol;
			nSrcRow = (int)dDestRow;

			image[0][0] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][1] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow - 1, nSrcWidth, nSrcHeight); 
			image[0][2] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow - 1, nSrcWidth, nSrcHeight); 
			image[0][3] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow - 1, nSrcWidth, nSrcHeight);
			//
			image[1][0] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow    , nSrcWidth, nSrcHeight);
			image[1][1] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow    , nSrcWidth, nSrcHeight);
			image[1][2] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow    , nSrcWidth, nSrcHeight);
			image[1][3] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow    , nSrcWidth, nSrcHeight);
			//
			image[2][0] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][1] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][2] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][3] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 1, nSrcWidth, nSrcHeight);
			//
			image[3][0] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][1] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][2] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][3] = (unsigned char)GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 2, nSrcWidth, nSrcHeight);

			double dXdiff = dDestCol - nSrcCol;
			double dYdiff = dDestRow - nSrcRow;

			unsigned char column[4];
			double a0, a1, a2, a3;
			double x_plus_1, y_plus_1;
			double one_minus_x, one_minus_y;
			double two_minus_x, two_minus_y;
			int i;
			double pixel;

			if ((dXdiff == 0.0) && (dYdiff == 0.0))
			{
				pixel = image[1][1];
			}

			if (dXdiff == 0.0)
			{
				for (i = 0; i < 4; i++)
					column[i] = image[i][1];
			}
			else
			{
				if (bSpline)
				{
					column[0] = DoNaturalCubicSpline(image[0], dXdiff);
					column[1] = DoNaturalCubicSpline(image[1], dXdiff);
					column[2] = DoNaturalCubicSpline(image[2], dXdiff);
					column[3] = DoNaturalCubicSpline(image[3], dXdiff);

					pixel = DoNaturalCubicSpline(column, dYdiff);
				}
				else
				{
					column[0] = DoCubicPolynomial(image[0], dXdiff);
					column[1] = DoCubicPolynomial(image[1], dXdiff);
					column[2] = DoCubicPolynomial(image[2], dXdiff);
					column[3] = DoCubicPolynomial(image[3], dXdiff);

					pixel = DoCubicPolynomial(column, dYdiff);
				}
			}

			pLineBuffer[nIndex++] = pixel;

		}
		memcpy(pDestImage + nrow * nDestWidth, pLineBuffer, nDestWidth);
	}

	delete[] pLineBuffer;

	return TRUE;
}

double CMyInterpolation::DoLinear(unsigned char fx1, unsigned char fx2, double dWeight)
{
	return fx1 + dWeight * (fx2 - fx1);
}

// https://blog.naver.com/mykepzzang/220546694965
double CMyInterpolation::DoCubicPolynomial(unsigned char ucImg[4], double x)
{
	double dInvX[4][4] = {
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ -0.333f, -0.5f, 1.0f, -0.167f},
		{ 0.5f, -1.0f, 0.5f, 0.0f},
		{ -0.167f, 0.5f, -0.5f, 0.167f},
	};

	double a1 = dInvX[0][0] * (double)ucImg[0] + dInvX[0][1] * (double)ucImg[1] + dInvX[0][2] * (double)ucImg[2] + dInvX[0][3] *(double)ucImg[3];
	double a2 = dInvX[1][0] * (double)ucImg[0] + dInvX[1][1] * (double)ucImg[1] + dInvX[1][2] * (double)ucImg[2] + dInvX[1][3] * (double)ucImg[3];
	double a3 = dInvX[2][0] * (double)ucImg[0] + dInvX[2][1] * (double)ucImg[1] + dInvX[2][2] * (double)ucImg[2] + dInvX[2][3] * (double)ucImg[3];
	double a4 = dInvX[3][0] * (double)ucImg[0] + dInvX[3][1] * (double)ucImg[1] + dInvX[3][2] * (double)ucImg[2] + dInvX[3][3] * (double)ucImg[3];

	return a1 + x* a2 + x*x*a3 + x*x*x*a4;
}

// S(x) = aj + bj(x-xj) + cj(x-xj)^2 + dj(x-xj)^3 (xj <= x <= xj+1)
// https://fac.ksu.edu.sa/sites/default/files/numerical_analysis_9th.pdf#page=167
double CMyInterpolation::DoNaturalCubicSpline(unsigned char ucImg[4], double x)
{
	// Input
	const INT_PTR nCount = 3;						// n
	double dX[nCount + 1] = { -1, 0, 1, 2 };		// x
	double dY[nCount + 1] = { 
		(double)ucImg[0], 
		(double)ucImg[1], 
		(double)ucImg[2], 
		(double)ucImg[3] };							// f(x)

	// Output
	double dA[nCount + 1] = { dY[0], dY[1], dY[2], dY[3] };	 // a
	double dB[nCount + 1] = { 0, };  // b (x)
	double dC[nCount + 1] = { 0, };  // c (x*x)
	double dD[nCount + 1] = { 0, };  // d (x*x*x)

	// Variable
	double dH = 1.0;				// h = x[i+1] - x[i];	/** Step 1 */ so, Remove dH from all functions
	double dAlpha[nCount] = { 0, };	// A
	double dL[nCount + 1] = { 0, };	// l
	double dU[nCount + 1] = { 0, };	// u
	double dZ[nCount + 1] = { 0, };	// z

	/** Step 2 */
	for (INT_PTR i = 1; i < nCount; ++i)
	{
		dAlpha[i] = 3 * (dA[i + 1] - dA[i]) - 3 * (dA[i] - dA[i - 1]);
	}
	// Origin
// 	for (INT_PTR i = 1; i < nCount; ++i)
// 	{
// 		dAlpha[i] = 3 * (dA[i + 1] - dA[i]) / dH - 3 * (dA[i] - dA[i - 1]) / dH;
// 	}

	/** Step 3 */
	dL[0] = 1;
	dU[0] = 0;
	dZ[0] = 0;
		
	/** Step 4 */
	for (INT_PTR i = 1; i < nCount; ++i) {
		dL[i] = 2 * (dX[i + 1] - dX[i - 1]) - dU[i - 1];		// 2(h0 + h1) - l(i,i-1) * u(i-1,i)
		dU[i] = 1 / dL[i];
		dZ[i] = (dAlpha[i] - dZ[i - 1]) / dL[i];
	}

	// Origin
// 	for (INT_PTR i = 1; i < nCount; ++i) {
// 		dL[i] = 2 * (dX[i + 1] - dX[i - 1]) - dH * dU[i - 1];		// 2(h0 + h1) - l(i,i-1) * u(i-1,i)
// 		dU[i] = dH / dL[i];
// 		dZ[i] = (dAlpha[i] - dH * dZ[i - 1]) / dL[i];
// 	}

	/** Step 5 */
	dL[nCount] = 1;
	dZ[nCount] = 0;
	dC[nCount] = 0;

	/** Step 6 */
	for (INT_PTR j = nCount - 1; j >= 0; --j) {
		dC[j] = dZ[j] - dU[j] * dC[j + 1];
		dB[j] = (dA[j + 1] - dA[j]) - (dC[j + 1] + 2 * dC[j]) / 3;
		dD[j] = (dC[j + 1] - dC[j]) / (3);
	}

	// Origin
// 	for (INT_PTR j = nCount - 1; j >= 0; --j) {
// 		dC[j] = dZ[j] - dU[j] * dC[j + 1];
// 		dB[j] = (dA[j + 1] - dA[j]) / dH - dH * (dC[j + 1] + 2 * dC[j]) / 3;
// 		dD[j] = (dC[j + 1] - dC[j]) / (3 * dH);
// 	}

	// Result
	return dA[1] + dB[1]*x + dC[1]*x*x + dD[1]*x*x*x;
}

void CMyInterpolation::clip(double max, double min, double& num)
{
	if (num > max)
		num = max;
	if (num < min)
		num = min;
}

BYTE CMyInterpolation::GetPixelClamped(BYTE* pImage, INT_PTR x, INT_PTR y, INT_PTR nWidth, INT_PTR nHeight)
{
	CLAMP(x, 0, nWidth - 1);
	CLAMP(y, 0, nHeight - 1);
	return pImage[(y * nWidth) + x];
}