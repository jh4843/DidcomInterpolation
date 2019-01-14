#define _USE_MATH_DEFINES

#include "stdafx.h"
#include "MyInterpolation.h"
#include "opencv2\opencv.hpp"
#include <math.h>


#define PI       3.14159265
#define CLAMP(v, min, max) if (v < min) { v = min; } else if (v > max) { v = max; } 

CMyInterpolation::CMyInterpolation()
{
	m_bUseParallelCalc = TRUE;
}


CMyInterpolation::~CMyInterpolation()
{
}

BOOL CMyInterpolation::DoBilinearInterpolation(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight)
{
	SYSTEM_INFO systemInfo;
	DWORD dwCpuCount;
	GetSystemInfo(&systemInfo);
	dwCpuCount = systemInfo.dwNumberOfProcessors;

	if (dwCpuCount < 1 || m_bUseParallelCalc == FALSE)
	{
		dwCpuCount = 1;
	}
	else
	{
		dwCpuCount /= 2;
	}

	INT_PTR nrow, ncol;
	double dWidthScale, dHeightScale;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
	UCHAR NW, NE, SW, SE;
	ULONG nAddress, nSrcImageLength;

	TRY
	{
		dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
		dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;

		dWidthScale = dOutWidth / dInWidth;
		dHeightScale = dOutHeight / dInHeight;

		nSrcImageLength = nDestWidth * nDestHeight;

#pragma omp parallel for private(nrow, ncol, NW, NE, SW, SE) num_threads(dwCpuCount)
		for (nrow = 0; nrow < nDestHeight; nrow++)
		{
			for (ncol = 0; ncol < nDestWidth; ncol++)
			{
				double dDestCol = (double)(ncol / dWidthScale);
				double dDestRow = (double)(nrow / dHeightScale);

				INT_PTR nSrcCol = (int)dDestCol;
				INT_PTR nSrcRow = (int)dDestRow;

				double dEWweight = dDestCol - nSrcCol;
				double dNSweight = dDestRow - nSrcRow;

				NW = GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow    , nSrcWidth, nSrcHeight);  
				NE = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow    , nSrcWidth, nSrcHeight);
				SW = GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow + 1, nSrcWidth, nSrcHeight);
				SE = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);

				unsigned char dEWtop = DoLinear(NW, NE, dEWweight);
				unsigned char dEWbottom = DoLinear(SW, SE, dEWweight);

				pDestImage[nrow * nDestWidth + ncol] = DoLinear(dEWtop, dEWbottom, dNSweight);
			}
		}
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

// https://en.wikipedia.org/wiki/Bicubic_interpolation --> Bicubic convolution algorithm
BOOL CMyInterpolation::DoBiCubicInterpolation(BYTE * pSrcImage, BYTE * pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, BOOL bBSpline, double a)
{
	SYSTEM_INFO systemInfo;
	DWORD dwCpuCount;
	GetSystemInfo(&systemInfo);
	dwCpuCount = systemInfo.dwNumberOfProcessors;

	if (dwCpuCount < 1 || m_bUseParallelCalc == FALSE)
	{
		dwCpuCount = 1;
	}
	else
	{
		dwCpuCount /= 2;
	}

	int nrow, ncol;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
// 	INT_PTR nSrcCol, nSrcRow;
// 	double dDestCol, dDestRow;
	double dWidthScale, dHeightScale;
	ULONG nAddress, nSrcImageLength;

	dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
	dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;

	dWidthScale = dOutWidth / dInWidth;
	dHeightScale = dOutHeight / dInHeight;

	nSrcImageLength = nSrcWidth * nSrcHeight;

	unsigned char image[4][4];

#pragma omp parallel for private(nrow, ncol, image) num_threads(dwCpuCount)
	for (nrow = 0; nrow < nDestHeight; nrow++)
	{
		for (ncol = 0; ncol < nDestWidth; ncol++)
		{
			memset(image, 0, sizeof(image));

			double dDestCol = (double)(ncol / dWidthScale);
			double dDestRow = (double)(nrow / dHeightScale);

			double nSrcCol = (int)dDestCol;
			double nSrcRow = (int)dDestRow;

			image[0][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow - 1, nSrcWidth, nSrcHeight);
			//
			image[1][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow, nSrcWidth, nSrcHeight);
			//
			image[2][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 1, nSrcWidth, nSrcHeight);
			//
			image[3][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 2, nSrcWidth, nSrcHeight);

			double dXdiff = dDestCol - nSrcCol;
			double dYdiff = dDestRow - nSrcRow;

			unsigned char column[4];
			unsigned char pixel;

			if ((dXdiff == 0.0) && (dYdiff == 0.0))
			{
				pixel = image[1][1];
			}

			if (dXdiff == 0.0)
			{
				for (INT_PTR i = 0; i < 4; i++)
					column[i] = image[i][1];
			}
			else
			{
				if (bBSpline)
				{
					column[0] = DoBSplineKernel(image[0], dXdiff);
					column[1] = DoBSplineKernel(image[1], dXdiff);
					column[2] = DoBSplineKernel(image[2], dXdiff);
					column[3] = DoBSplineKernel(image[3], dXdiff);

					pixel = DoBSplineKernel(column, dYdiff);
				}
				else
				{
					column[0] = DoCubicKernel(image[0], dXdiff, a);
					column[1] = DoCubicKernel(image[1], dXdiff, a);
					column[2] = DoCubicKernel(image[2], dXdiff, a);
					column[3] = DoCubicKernel(image[3], dXdiff, a);

					pixel = DoCubicKernel(column, dYdiff, a);
				}
			}

			pDestImage[nrow * nDestWidth + ncol] = pixel;
		}
	}

	return TRUE;
}

// a는 2보다 커야 한다.
BOOL CMyInterpolation::DoLanczosInterpolation(BYTE * pSrcImage, BYTE * pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, double a)
{
	SYSTEM_INFO systemInfo;
	DWORD dwCpuCount;
	GetSystemInfo(&systemInfo);
	dwCpuCount = systemInfo.dwNumberOfProcessors;

	if (dwCpuCount < 1 || m_bUseParallelCalc == FALSE)
	{
		dwCpuCount = 1;
	}
	else
	{
		dwCpuCount /= 2;
	}

	int nrow, ncol;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
	double dWidthScale, dHeightScale;
	ULONG nAddress, nSrcImageLength;

	unsigned char image[4][4];

	dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
	dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;

	dWidthScale = dOutWidth / dInWidth;
	dHeightScale = dOutHeight / dInHeight;

	nSrcImageLength = nSrcWidth * nSrcHeight;

#pragma omp parallel for private(nrow, ncol, image) num_threads(dwCpuCount)
	for (nrow = 0; nrow < nDestHeight; nrow++)
	{
		for (ncol = 0; ncol < nDestWidth; ncol++)
		{
			memset(image, 0, sizeof(image));

			double dDestCol = (double)(ncol / dWidthScale);
			double dDestRow = (double)(nrow / dHeightScale);

			INT_PTR nSrcCol = (int)dDestCol;
			INT_PTR nSrcRow = (int)dDestRow;

			image[0][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow - 1, nSrcWidth, nSrcHeight);
			//
			image[1][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow, nSrcWidth, nSrcHeight);
			//
			image[2][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 1, nSrcWidth, nSrcHeight);
			//
			image[3][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 2, nSrcWidth, nSrcHeight);

			double dXdiff = dDestCol - nSrcCol;
			double dYdiff = dDestRow - nSrcRow;

			unsigned char column[4];
			double pixel;

			if ((dXdiff == 0.0) && (dYdiff == 0.0))
			{
				pixel = image[1][1];
			}

			if (dXdiff == 0.0)
			{
				for (INT_PTR i = 0; i < 4; i++)
					column[i] = image[i][1];
			}
			else
			{
				column[0] = DoLanczos(image[0], dXdiff, a);
				column[1] = DoLanczos(image[1], dXdiff, a);
				column[2] = DoLanczos(image[2], dXdiff, a);
				column[3] = DoLanczos(image[3], dXdiff, a);

				pixel = DoLanczos(column, dYdiff, a);
			}

			pDestImage[nrow * nDestWidth + ncol] = pixel;
		}
	}

	return TRUE;
}

// normally B = 1/3, C = 1/3
BOOL CMyInterpolation::DoMitchellInterpolation(BYTE * pSrcImage, BYTE * pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight, double B, double C)
{
	SYSTEM_INFO systemInfo;
	DWORD dwCpuCount;
	GetSystemInfo(&systemInfo);
	dwCpuCount = systemInfo.dwNumberOfProcessors;

	if (dwCpuCount < 1 || m_bUseParallelCalc == FALSE)
	{
		dwCpuCount = 1;
	}
	else
	{
		dwCpuCount /= 2;
	}

	int nrow, ncol;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
	double dWidthScale, dHeightScale;
	ULONG nAddress, nSrcImageLength;

	unsigned char image[4][4];	memset(image, 0, sizeof(image));

	dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
	dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;

	dWidthScale = dOutWidth / dInWidth;
	dHeightScale = dOutHeight / dInHeight;

	nSrcImageLength = nSrcWidth * nSrcHeight;

#pragma omp parallel for private(nrow, ncol, image) num_threads(dwCpuCount)
	for (nrow = 0; nrow < nDestHeight; nrow++)
	{
		for (ncol = 0; ncol < nDestWidth; ncol++)
		{
			double dDestCol = (double)(ncol / dWidthScale);
			double dDestRow = (double)(nrow / dHeightScale);

			INT_PTR nSrcCol = (int)dDestCol;
			INT_PTR nSrcRow = (int)dDestRow;

			image[0][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow - 1, nSrcWidth, nSrcHeight);
			//
			image[1][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow, nSrcWidth, nSrcHeight);
			//
			image[2][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 1, nSrcWidth, nSrcHeight);
			//
			image[3][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 2, nSrcWidth, nSrcHeight);

			double dXdiff = dDestCol - nSrcCol;
			double dYdiff = dDestRow - nSrcRow;

			unsigned char column[4];
			double pixel;

			if ((dXdiff == 0.0) && (dYdiff == 0.0))
			{
				pixel = image[1][1];
			}

			if (dXdiff == 0.0)
			{
				for (INT_PTR i = 0; i < 4; i++)
					column[i] = image[i][1];
			}
			else
			{
				column[0] = DoMitchell(image[0], dXdiff, B, C);
				column[1] = DoMitchell(image[1], dXdiff, B, C);
				column[2] = DoMitchell(image[2], dXdiff, B, C);
				column[3] = DoMitchell(image[3], dXdiff, B, C);

				pixel = DoMitchell(column, dYdiff, B, C);
			}

			pDestImage[nrow * nDestWidth + ncol] = pixel;
		}
	}

	return TRUE;
}

// B = 0, C = 0.5
BOOL CMyInterpolation::DoCatmullRomSplineInterpolation(BYTE * pSrcImage, BYTE * pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight)
{
	SYSTEM_INFO systemInfo;
	DWORD dwCpuCount;
	GetSystemInfo(&systemInfo);
	dwCpuCount = systemInfo.dwNumberOfProcessors;

	if (dwCpuCount < 1)
		dwCpuCount = 1;

	int nrow, ncol;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
	double dWidthScale, dHeightScale;
	ULONG nAddress, nSrcImageLength;

	double B = 0;
	double C = 0.5;

	unsigned char image[4][4];	

	dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
	dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;

	dWidthScale = dOutWidth / dInWidth;
	dHeightScale = dOutHeight / dInHeight;

	nSrcImageLength = nSrcWidth * nSrcHeight;

#pragma omp parallel for private(nrow, ncol, image) num_threads(dwCpuCount)
	for (nrow = 0; nrow < nDestHeight; nrow++)
	{
		for (ncol = 0; ncol < nDestWidth; ncol++)
		{
			memset(image, 0, sizeof(image));

			double dDestCol = (double)(ncol / dWidthScale);
			double dDestRow = (double)(nrow / dHeightScale);

			INT_PTR nSrcCol = (int)dDestCol;
			INT_PTR nSrcRow = (int)dDestRow;

			image[0][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow - 1, nSrcWidth, nSrcHeight);
			//
			image[1][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow, nSrcWidth, nSrcHeight);
			image[1][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow, nSrcWidth, nSrcHeight);
			//
			image[2][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 1, nSrcWidth, nSrcHeight);
			//
			image[3][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][1] = GetPixelClamped(pSrcImage, nSrcCol, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 2, nSrcWidth, nSrcHeight);

			double dXdiff = dDestCol - nSrcCol;
			double dYdiff = dDestRow - nSrcRow;

			unsigned char column[4];
			double pixel;

			if ((dXdiff == 0.0) && (dYdiff == 0.0))
			{
				pixel = image[1][1];
			}

			if (dXdiff == 0.0)
			{
				for (INT_PTR i = 0; i < 4; i++)
					column[i] = image[i][1];
			}
			else
			{
				column[0] = DoMitchell(image[0], dXdiff, B, C);
				column[1] = DoMitchell(image[1], dXdiff, B, C);
				column[2] = DoMitchell(image[2], dXdiff, B, C);
				column[3] = DoMitchell(image[3], dXdiff, B, C);

				pixel = DoMitchell(column, dYdiff, B, C);
			}

			pDestImage[nrow * nDestWidth + ncol] = pixel;
		}
	}

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

	int nrow, ncol, nAllow1, nAllow2;
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
		for (ncol = 0; ncol < nDestWidth; ncol++)
		{
			dDestCol = (double)(ncol / dWidthScale);
			dDestRow = (double)(nrow / dHeightScale);

			nSrcCol = (int)dDestCol;
			nSrcRow = (int)dDestRow;

			image[0][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow - 1, nSrcWidth, nSrcHeight);
			image[0][1] = GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow - 1, nSrcWidth, nSrcHeight); 
			image[0][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow - 1, nSrcWidth, nSrcHeight); 
			image[0][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow - 1, nSrcWidth, nSrcHeight);
			//
			image[1][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow    , nSrcWidth, nSrcHeight);
			image[1][1] = GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow    , nSrcWidth, nSrcHeight);
			image[1][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow    , nSrcWidth, nSrcHeight);
			image[1][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow    , nSrcWidth, nSrcHeight);
			//
			image[2][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][1] = GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 1, nSrcWidth, nSrcHeight);
			image[2][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 1, nSrcWidth, nSrcHeight);
			//
			image[3][0] = GetPixelClamped(pSrcImage, nSrcCol - 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][1] = GetPixelClamped(pSrcImage, nSrcCol    , nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][2] = GetPixelClamped(pSrcImage, nSrcCol + 1, nSrcRow + 2, nSrcWidth, nSrcHeight);
			image[3][3] = GetPixelClamped(pSrcImage, nSrcCol + 2, nSrcRow + 2, nSrcWidth, nSrcHeight);

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

			pLineBuffer[ncol] = pixel;
			//pLineBuffer[nIndex++] = pixel;

		}
		memcpy(pDestImage + nrow * nDestWidth, pLineBuffer, nDestWidth);
	}

	delete[] pLineBuffer;

	return TRUE;
}

void CMyInterpolation::SetUseParallelCalc(BOOL bUse)
{
	m_bUseParallelCalc = bUse;
}

unsigned char CMyInterpolation::DoLinear(unsigned char fx1, unsigned char fx2, double dWeight)
{
	return fx1 + dWeight * (fx2 - fx1);
}

// ref : http://teaching.csse.uwa.edu.au/units/CITS4241/Handouts/Lecture04.html
// ref : https://clouard.users.greyc.fr/Pantheon/experiments/rescaling/index-en.html
// a, range [-3, 0], -3에 가까울 수록 sharpening effect, 0에 가까울 수록 blurring effect
// -0.5 = cubic Hermite spline(default), 
// 0 = Hermite algorithm
// ucImg[0] = f(-1), ucImg[1] = f(0), ucImg[2] = f(1), ucImg[3] = f(2)
// x를 기준으로 Convolution 하기 위해, 위 값을 x만큼 이동 시키면
// (-1-x, f(-1)), (0 - x, f(0)), (1 - x, f(1)), (2 - x, f(2))
unsigned char CMyInterpolation::DoCubicKernel(unsigned char ucImg[4], double x, double a)
{
	// x = |0-x|               // 0 < |0  - x| <= 1
	double dXPlusOne = x + 1;  // 1 < |-1 - x| <  2
	double dOneMinusX = 1 - x; // 0 < |1  - x| <= 1
	double dTwoMinusX = 2 - x; // 1 < |2  - x| <  2

	// bicubic convolution algorithm (ref https://en.wikipedia.org/wiki/Bicubic_interpolation )
	double w0 = (a * dXPlusOne * dXPlusOne * dXPlusOne) - (5.0*a * dXPlusOne* dXPlusOne) + (8.0*a * dXPlusOne) - 4.0 * a;
	double w1 = ((a + 2.0) * x * x * x) - ((a + 3.0) * x * x) + 1;
	double w2 = ((a + 2.0) * dOneMinusX * dOneMinusX * dOneMinusX) - ((a + 3.0) * dOneMinusX * dOneMinusX) +1;  
	double w3 = (a * dTwoMinusX * dTwoMinusX * dTwoMinusX) - (5.0*a * dTwoMinusX* dTwoMinusX) + (8.0*a * dTwoMinusX) - 4.0 * a; 
	
	double res = w0 * ucImg[0] + w1 * ucImg[1] + w2 * ucImg[2] + w3 * ucImg[3];
	CLAMP(res, 0, 255);

	// convolution
	return (unsigned char)res;
}

// http://teaching.csse.uwa.edu.au/units/CITS4241/Handouts/Lecture04.html
// Positive smoothing interpolation
// ucImg[0] = f(-1), ucImg[1] = f(0), ucImg[2] = f(1), ucImg[3] = f(2)
// x를 기준으로 Convolution 하기 위해, 위 값을 x만큼 이동 시키면
// (-1-x, f(-1)), (0 - x, f(0)), (1 - x, f(1)), (2 - x, f(2))
unsigned char CMyInterpolation::DoBSplineKernel(unsigned char ucImg[4], double x)
{
	// x = |0-x|               // 0 < |0  - x| <= 1
	double dXPlusOne = x + 1;  // 1 < |-1 - x| <  2
	double dOneMinusX = 1 - x; // 0 < |1  - x| <= 1
	double dTwoMinusX = 2 - x; // 1 < |2  - x| <  2

	// bicubic convolution algorithm (ref https://en.wikipedia.org/wiki/Bicubic_interpolation )
	double w0 = (-0.16666667 * dXPlusOne * dXPlusOne * dXPlusOne) + ((1.0) * dXPlusOne * dXPlusOne) - ((2.0) * dXPlusOne) + 1.33333;
	double w1 = (0.5 * x * x * x) - (1.0 * x * x) + 0.666666667;  
	double w2 = (0.5 * dOneMinusX * dOneMinusX * dOneMinusX) - (1.0 * dOneMinusX * dOneMinusX) + 0.666666667;
	double w3 = (-0.16666667 * dTwoMinusX * dTwoMinusX * dTwoMinusX) + ((1.0) * dTwoMinusX * dTwoMinusX) - ((2.0) * dTwoMinusX) + 1.33333; 

	double res = w0 * ucImg[0] + w1 * ucImg[1] + w2 * ucImg[2] + w3 * ucImg[3];
	CLAMP(res, 0, 255);

	// convolution
	return (unsigned char)res;
}

// https://en.wikipedia.org/wiki/Lanczos_resampling
unsigned char CMyInterpolation::DoLanczos(unsigned char ucImg[4], double x, double a)
{
	// x = |0-x|               // 0 < |0  - x| <= 1
	double dXPlusOne = x + 1;  // 1 < |-1 - x| <  2
	double dOneMinusX = 1 - x; // 0 < |1  - x| <= 1
	double dTwoMinusX = 2 - x; // 1 < |2  - x| <  2

	// Lanczos convolution algorithm (ref https://en.wikipedia.org/wiki/Lanczos_resampling)
	double w0 = (dXPlusOne == 0) ? 1 : (a * sin(PI * dXPlusOne) * sin((PI * dXPlusOne) / a)) / (PI * PI * dXPlusOne * dXPlusOne);
	double w1 = (x == 0) ? 1 : (a * sin(PI * x) * sin((PI * x) / a)) / (PI * PI * x * x);
	double w2 = (dOneMinusX == 0) ? 1 : (a * sin(PI * dOneMinusX) * sin((PI * dOneMinusX) / a)) / (PI * PI * dOneMinusX * dOneMinusX);
	double w3 = (dTwoMinusX == 0) ? 1 : (a * sin(PI * dTwoMinusX) * sin((PI * dTwoMinusX) / a)) / (PI * PI * dTwoMinusX * dTwoMinusX);

	double res = w0 * ucImg[0] + w1 * ucImg[1] + w2 * ucImg[2] + w3 * ucImg[3];
	CLAMP(res, 0, 255);

	// convolution
	return (unsigned char)res;
}

// https://de.wikipedia.org/wiki/Mitchell-Netravali-Filter
unsigned char CMyInterpolation::DoMitchell(unsigned char ucImg[4], double x, double B, double C)
{
	// x = |0-x|               // 0 < |0  - x| <= 1
	double dXPlusOne = x + 1;  // 1 < |-1 - x| <  2
	double dOneMinusX = 1 - x; // 0 < |1  - x| <= 1
	double dTwoMinusX = 2 - x; // 1 < |2  - x| <  2

	// mitchell convolution algorithm (ref https://clouard.users.greyc.fr/Pantheon/experiments/rescaling/index-en.html#mitchell)
	double w0 = ((-B - 6 * C) * dXPlusOne * dXPlusOne * dXPlusOne) + ((6 * B + 30 * C) * dXPlusOne * dXPlusOne) + ((-12 * B - 48 * C) * dXPlusOne) + (8*B + 24*C);
	double w1 = ((12 - 9 * B - 6 * C) * x * x * x) + ((-18 + 12 * B + 6 * C) * x * x) + (6 - 2 * B);
	double w2 = ((12 - 9 * B - 6 * C) * dOneMinusX * dOneMinusX * dOneMinusX) + ((-18 + 12 * B + 6 * C) * dOneMinusX * dOneMinusX) + (6 - 2 * B);
	double w3 = ((-B - 6 * C) * dTwoMinusX * dTwoMinusX * dTwoMinusX) + ((6 * B + 30 * C) * dTwoMinusX * dTwoMinusX) + ((-12 * B - 48 * C) * dTwoMinusX) + (8 * B + 24 * C);

	w0 = w0 * 0.16666667;
	w1 = w1 * 0.16666667;
	w2 = w2 * 0.16666667;
	w3 = w3 * 0.16666667;

	double res = w0 * ucImg[0] + w1 * ucImg[1] + w2 * ucImg[2] + w3 * ucImg[3];
	CLAMP(res, 0, 255);

	// convolution
	return (unsigned char)res;
}

// https://blog.naver.com/mykepzzang/220546694965
unsigned char CMyInterpolation::DoCubicPolynomial(unsigned char ucImg[4], double x)
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

	double res = a1 + x* a2 + x*x*a3 + x*x*x*a4;
	CLAMP(res, 0, 255);

	// convolution
	return (unsigned char)res;
}

// S(x) = aj + bj(x-xj) + cj(x-xj)^2 + dj(x-xj)^3 (xj <= x <= xj+1)
// https://fac.ksu.edu.sa/sites/default/files/numerical_analysis_9th.pdf#page=167
unsigned char CMyInterpolation::DoNaturalCubicSpline(unsigned char ucImg[4], double x)
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

	double res = dA[1] + dB[1] * x + dC[1] * x*x + dD[1] * x*x*x;
	CLAMP(res, 0, 255);

	// convolution
	return (unsigned char)res;
}

void CMyInterpolation::clip(double max, double min, double& num)
{
	if (num > max)
		num = max;
	if (num < min)
		num = min;
}

unsigned char CMyInterpolation::GetPixelClamped(unsigned char* pImage, INT_PTR x, INT_PTR y, INT_PTR nWidth, INT_PTR nHeight)
{
	CLAMP(x, 0, nWidth - 1);
	CLAMP(y, 0, nHeight - 1);
	return pImage[(y * nWidth) + x];
}