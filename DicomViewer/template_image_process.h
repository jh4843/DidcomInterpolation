#ifndef _TEMPLATE_IMAGE_PROCESS_H_
#define _TEMPLATE_IMAGE_PROCESS_H_

// template 부분 특화 시켜 반드시 한 cpp파일에만 include하는 것을 권장함.
// 한 cpp파일에만 include 못시키는 프로그램 구조에서는 LINK 명령줄에 /FORCE:MULTIPLE 속성을 준다.

#include <math.h>
#include <mmintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <omp.h>
#include <math.h>

#define OMP_NUM_THREADS 2

///////////////////////////////////////////////////////////////////
// template function
///////////////////////////////////////////////////////////////////

int GetOmpProcCount();

template <class TYPE> BOOL adjust_usingbits(TYPE* pImage, UINT nWidth, UINT nHeight, UINT nUsingBits);

template <class TYPE> BOOL adjust_dicom_lut(TYPE* pImage, RGBQUAD* pLut, UINT nWidth, UINT nHeight, UINT nUsingBits);
template <RGBTRIPLE*> BOOL adjust_dicom_lut(RGBTRIPLE** pImage, RGBQUAD* pLut, UINT nWidth, UINT nHeight, UINT nUsingBits);
template <RGBQUAD*> BOOL adjust_dicom_lut(RGBQUAD** pImage, RGBQUAD* pLut, UINT nWidth, UINT nHeight, UINT nUsingBits);

template <class TYPE> BOOL scale_down_image(TYPE* pIn, TYPE* pOut, UINT nInWidth, UINT nInHeight, UINT nOutWidth, UINT nOutHeight);
template <RGBTRIPLE*> BOOL scale_down_image(RGBTRIPLE** pIn, RGBTRIPLE** pOut, UINT nInWidth, UINT nInHeight, UINT nOutWidth, UINT nOutHeight);
template <RGBQUAD*> BOOL scale_down_image(RGBQUAD** pIn, RGBQUAD** pOut, UINT nInWidth, UINT nInHeight, UINT nOutWidth, UINT nOutHeight);

template <class TYPE> BOOL crop_roi_image(TYPE* pImage, UINT nWidth, UINT nHeight, int roi_x, int roi_y, int roi_dx, int roi_dy);
template <class TYPE> BOOL rotate_image(TYPE* pImage, UINT nWidth, UINT nHeight, int nMode);
template <class TYPE> BOOL reverse_image(TYPE* pImage, UINT nWidth, UINT nHeight, int nMode);
template <class IN_TYPE, class OUT_TYPE> BOOL convert_image(IN_TYPE* pIn, OUT_TYPE* pOut, UINT nInUsingBits, UINT nOutUsingBits, UINT nWidth, UINT nHeight);

template <class IN_TYPE, class OUT_TYPE> BOOL window_level_image(IN_TYPE* pIn, OUT_TYPE* pOut, UINT nInUsingBits, UINT nOutUsingBits, UINT nInBytesPerLine, UINT nOutBytesPerLine, UINT nWidth, UINT nHeight, int nW1, int nW2);
template <class IN_TYPE, class OUT_TYPE> BOOL window_level_invert_image(IN_TYPE* pIn, OUT_TYPE* pOut, UINT nInUsingBits, UINT nOutUsingBits, UINT nInBytesPerLine, UINT nOutBytesPerLine, UINT nWidth, UINT nHeight, int nW1, int nW2);
template <class IN_TYPE, class OUT_TYPE, class LUT_TYPE> BOOL window_level_lut_image(IN_TYPE* pIn, OUT_TYPE* pOut, LUT_TYPE *pLut, UINT nInUsingBits, UINT nOutUsingBits, UINT nLutUsingBits, UINT nInBytesPerLine, UINT nOutBytesPerLine, UINT nWidth, UINT nHeight, int nW1, int nW2);

template <class IN_TYPE, class OUT_TYPE> BOOL burn_overlay_on_image(IN_TYPE* pOverlayInput, OUT_TYPE* pOut, UINT nOvlBitsPerPixel, UINT nOvlBytePerLine, UINT nOutHeight, UINT nOutWidth, UINT nOutMaxValue);

template <class TYPE> BOOL mask_circle_mask_data(TYPE* pMask, UINT nWidth, UINT nHeight, UINT nCircleMask);

template <RGBTRIPLE*> BOOL make_histogram_data(RGBTRIPLE** pImage, double* pHistogram, WORD* pMask, UINT nWidth, UINT nHeight, UINT nUsingBits, double& max_histogram);
template <RGBQUAD*> BOOL make_histogram_data(RGBQUAD** pImage, double* pHistogram, WORD* pMask, UINT nWidth, UINT nHeight, UINT nUsingBits, double& max_histogram);
template <class IMAGE_TYPE> BOOL make_histogram_data(IMAGE_TYPE* pImage, double* pHistogram, WORD* pMask, UINT nWidth, UINT nHeight, UINT nUsingBits, double& max_histogram);

template <class TYPE> BOOL adjust_circle_mask_data(TYPE* pImage, WORD* pMask, UINT nWidth, UINT nHeight);

template <class TYPE> BOOL burn_dc_object(TYPE* pImage, UINT nWidth, UINT nHeight, UINT nUsingBits, CDC* pDC, CBitmap* pBitmap, COLORREF crTransparent, BOOL bFlip);
template <RGBTRIPLE*> BOOL burn_dc_object(RGBTRIPLE** pImage, UINT nWidth, UINT nHeight, UINT nUsingBits, CDC* pDC, CBitmap* pBitmap, COLORREF crTransparent, BOOL bFlip);
template <RGBQUAD*> BOOL burn_dc_object(RGBQUAD** pImage, UINT nWidth, UINT nHeight, UINT nUsingBits, CDC* pDC, CBitmap* pBitmap, COLORREF crTransparent, BOOL bFlip);

template <class TYPE> BOOL rotate_image_arbitrarily(TYPE* pDestImage, UINT nDestWidth, UINT nDestHeight, UINT nDestBytesPerLine, TYPE* pSrcImage, UINT nSrcWidth, UINT nSrcHeight, UINT nSrcBytesPerLine, float fDestCenterX, float fDestCenterY, float fSrcCenterX, float fSrcCenterY, float fRadian, float fScale);

int GetOmpProcCount()
{
	static int nOmpProcCount = 0;
	if (!nOmpProcCount)
	{
		nOmpProcCount = omp_get_num_procs() / 2;
		if (nOmpProcCount < 1)
			nOmpProcCount = 1;
	}
	//
	return nOmpProcCount;
}

//////////////////////////////////////////////////////////////////////////
//
template <class TYPE> BOOL adjust_usingbits(TYPE* pImage, UINT nWidth, UINT nHeight, UINT nUsingBits)
{
	if(!pImage)
		return FALSE;

	int i;
	int dx, dy;
	int size;
	int nMaxValue;
	
	dx = nWidth;
	dy = nHeight;
	size = nWidth*nHeight;
	nMaxValue = (int)((1 << nUsingBits)-1);

	omp_set_dynamic(1);
#pragma omp parallel for num_threads(GetOmpProcCount())
	for(i=0; i<size; i++)
	{
		pImage[i] = (TYPE)min(pImage[i], nMaxValue);
	}


	return TRUE;
}

template <> BOOL adjust_dicom_lut<RGBQUAD*>(RGBQUAD** pImage, RGBQUAD* pLut, UINT nWidth, UINT nHeight, UINT nUsingBits)
{
	if (!(*pImage))
	{
		return FALSE;
	}

	if (!pLut)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	if (nUsingBits < 1)
	{
		return FALSE;
	}

	int j = 0;
	int nPixelCount = (int)(nWidth*nHeight);
	int nMaxValue = (int)((1 << nUsingBits)-1);

	omp_set_dynamic(1);
	//omp_set_dynamic(1);
#pragma omp parallel for private(j) num_threads(GetOmpProcCount())
	for (int i = 0; i < nPixelCount; i++)
	{
		//
		j = (int)(((RGBQUAD*)((*pImage)+i))->rgbRed);
		if (j < 0)
		{
			j = 0;
		}
		if (j > nMaxValue)
		{
			j = nMaxValue;
		}
		((RGBQUAD*)((*pImage)+i))->rgbRed = pLut[j].rgbRed;

		//
		j = (int)(((RGBQUAD*)((*pImage)+i))->rgbGreen);
		if (j < 0)
		{
			j = 0;
		}
		if (j > nMaxValue)
		{
			j = nMaxValue;
		}
		((RGBQUAD*)((*pImage)+i))->rgbGreen = pLut[j].rgbGreen;

		//
		j = (int)(((RGBQUAD*)((*pImage)+i))->rgbBlue);
		if (j < 0)
		{
			j = 0;
		}
		if (j > nMaxValue)
		{
			j = nMaxValue;
		}
		((RGBQUAD*)((*pImage)+i))->rgbBlue = pLut[j].rgbBlue;
	}

	return TRUE;
}

template <> BOOL adjust_dicom_lut<RGBTRIPLE*>(RGBTRIPLE** pImage, RGBQUAD* pLut, UINT nWidth, UINT nHeight, UINT nUsingBits)
{
	if (!(*pImage))
	{
		return FALSE;
	}

	if (!pLut)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	if (nUsingBits < 1)
	{
		return FALSE;
	}

	int j = 0;
	int nPixelCount = (int)(nWidth*nHeight);
	int nMaxValue = (int)((1 << nUsingBits)-1);

	omp_set_dynamic(1);
#pragma omp parallel for private(j) num_threads(GetOmpProcCount())
	for (int i=0; i<nPixelCount; i++)
	{
		j = (int)(((RGBTRIPLE*)((*pImage)+i))->rgbtRed);
		if (j < 0)
		{
			j = 0;
		}
		if (j > nMaxValue)
		{
			j = nMaxValue;
		}
		((RGBTRIPLE*)((*pImage)+i))->rgbtRed = pLut[j].rgbRed;

		j = (int)(((RGBTRIPLE*)((*pImage)+i))->rgbtGreen);
		if (j < 0)
		{
			j = 0;
		}
		if (j > nMaxValue)
		{
			j = nMaxValue;
		}
		((RGBTRIPLE*)((*pImage)+i))->rgbtGreen = pLut[j].rgbGreen;


		j = (int)(((RGBTRIPLE*)((*pImage)+i))->rgbtBlue);
		if (j < 0)
		{
			j = 0;
		}
		if (j > nMaxValue)
		{
			j = nMaxValue;
		}
		((RGBTRIPLE*)((*pImage)+i))->rgbtBlue = pLut[j].rgbBlue;
	}

	return TRUE;
}

template <class TYPE> BOOL adjust_dicom_lut(TYPE* pImage, RGBQUAD* pLut, UINT nWidth, UINT nHeight, UINT nUsingBits)
{
	if(!pImage)
	{
		return FALSE;
	}

	if(!pLut)
	{
		return FALSE;
	}

	if(nWidth<1)
	{
		return FALSE;
	}

	if(nHeight<1)
	{
		return FALSE;
	}

	if(nUsingBits<1)
	{
		return FALSE;
	}

	int i = 0;
	DWORD j = 0;
	int pixel_count = (int)(nWidth*nHeight);
	DWORD fMaxCount = (DWORD)((1 << nUsingBits));
	float fPixelValue = 0;
	float fConv = (float)((float)(1 << nUsingBits)/(float)(1 << 8));


	omp_set_dynamic(1);
#pragma omp parallel for private(j, fPixelValue) num_threads(GetOmpProcCount())
	for(i=0; i<pixel_count; i++)
	{
		j = (DWORD)(*(pImage+i));
		j = (DWORD)(j%fMaxCount);
		fPixelValue = (float)pLut[j].rgbRed;
		fPixelValue += (float)pLut[j].rgbGreen;
		fPixelValue += (float)pLut[j].rgbBlue;
		fPixelValue /= (float)3;
		fPixelValue *= fConv;
		(*(pImage+i)) = (TYPE)fPixelValue;
	}

	return TRUE;
}

template <> BOOL scale_down_image<RGBTRIPLE*>(RGBTRIPLE** pIn, RGBTRIPLE** pOut, UINT nInWidth, UINT nInHeight, UINT nOutWidth, UINT nOutHeight)
{
	if(!pIn)
	{
		return FALSE;
	}

	if(!(*pIn))
	{
		return FALSE;
	}

	if(!pOut)
	{
		return FALSE;
	}

	if(!(*pOut))
	{
		return FALSE;
	}

	if(nInWidth<1)
	{
		return FALSE;
	}

	if(nInHeight<1)
	{
		return FALSE;
	}

	if(nOutWidth<1)
	{
		return FALSE;
	}

	if(nOutHeight<1)
	{
		return FALSE;
	}

	UINT x, y;
	UINT dx, dy;
	UINT blockSizeX, blockSizeY;
	UINT In_width, in_hieght;
	UINT out_width, out_height;
	float bufRed;
	float bufGreen;
	float bufBlue;
	RGBTRIPLE rgb_triple;

	In_width = (UINT)(nInWidth);
	in_hieght = (UINT)(nInHeight);
	out_width = (UINT)(nOutWidth);
	out_height = (UINT)(nOutHeight);
	blockSizeX = (UINT)(nInWidth/nOutWidth);
	blockSizeY = (UINT)(nInHeight/nOutHeight);

	if(blockSizeX<1)
	{
		return FALSE;
	}

	if(blockSizeY<1)
	{
		return FALSE;
	}

	for (y=0;y<out_height;y++)
	{
		for (x=0;x<out_width;x++)
		{
			bufRed = 0;
			bufGreen = 0;
			bufBlue = 0;
			for (dy=0;dy<blockSizeY;dy++)
			{
				for (dx=0;dx<blockSizeX;dx++)
				{
					rgb_triple = (RGBTRIPLE)( *( (*pIn) + ((y*blockSizeY+dy)*In_width) + ((x*(blockSizeX))+dx) ) );
					bufRed += (float)rgb_triple.rgbtRed; 
					bufGreen += (float)rgb_triple.rgbtGreen; 
					bufBlue += (float)rgb_triple.rgbtBlue; 
				}
			}
			bufRed /= (float)blockSizeX*blockSizeY;
			bufGreen /= (float)blockSizeX*blockSizeY;
			bufBlue /= (float)blockSizeX*blockSizeY;

			((*pOut)+y*nOutWidth+x)->rgbtRed = (BYTE)bufRed;
			((*pOut)+y*nOutWidth+x)->rgbtGreen = (BYTE)bufGreen;
			((*pOut)+y*nOutWidth+x)->rgbtBlue = (BYTE)bufBlue;
		}
	}

	return TRUE;
}

template <> BOOL scale_down_image<RGBQUAD*>(RGBQUAD** pIn, RGBQUAD** pOut, UINT nInWidth, UINT nInHeight, UINT nOutWidth, UINT nOutHeight)
{
	if(!pIn)
	{
		return FALSE;
	}

	if(!(*pIn))
	{
		return FALSE;
	}

	if(!pOut)
	{
		return FALSE;
	}

	if(!(*pOut))
	{
		return FALSE;
	}

	if(nInWidth<1)
	{
		return FALSE;
	}

	if(nInHeight<1)
	{
		return FALSE;
	}

	if(nOutWidth<1)
	{
		return FALSE;
	}

	if(nOutHeight<1)
	{
		return FALSE;
	}

	UINT x, y;
	UINT dx, dy;
	UINT blockSizeX, blockSizeY;
	UINT In_width, in_hieght;
	UINT out_width, out_height;
	float bufRed;
	float bufGreen;
	float bufBlue;
	float bufReserved;
	RGBQUAD rgb_triple;

	In_width = (UINT)(nInWidth);
	in_hieght = (UINT)(nInHeight);
	out_width = (UINT)(nOutWidth);
	out_height = (UINT)(nOutHeight);
	blockSizeX = (UINT)(nInWidth/nOutWidth);
	blockSizeY = (UINT)(nInHeight/nOutHeight);

	if(blockSizeX<1)
	{
		return FALSE;
	}

	if(blockSizeY<1)
	{
		return FALSE;
	}

	for (y=0;y<out_height;y++)
	{
		for (x=0;x<out_width;x++)
		{
			bufRed = 0;
			bufGreen = 0;
			bufBlue = 0;
			bufReserved = 0;
			for (dy=0;dy<blockSizeY;dy++)
			{
				for (dx=0;dx<blockSizeX;dx++)
				{
					rgb_triple = (RGBQUAD)( *( (*pIn) + ((y*blockSizeY+dy)*In_width) + ((x*(blockSizeX))+dx) ) );
					bufRed += (float)rgb_triple.rgbRed; 
					bufGreen += (float)rgb_triple.rgbGreen; 
					bufBlue += (float)rgb_triple.rgbBlue; 
					bufReserved += (float)rgb_triple.rgbReserved; 
				}
			}
			bufRed /= (float)blockSizeX*blockSizeY;
			bufGreen /= (float)blockSizeX*blockSizeY;
			bufBlue /= (float)blockSizeX*blockSizeY;
			bufReserved /= (float)blockSizeX*blockSizeY;

			((*pOut)+y*nOutWidth+x)->rgbRed = (BYTE)bufRed;
			((*pOut)+y*nOutWidth+x)->rgbGreen = (BYTE)bufGreen;
			((*pOut)+y*nOutWidth+x)->rgbBlue = (BYTE)bufBlue;
			((*pOut)+y*nOutWidth+x)->rgbReserved = (BYTE)bufReserved;
		}
	}

	return TRUE;
}

template <class TYPE> BOOL scale_down_image(TYPE* pIn, TYPE* pOut, UINT nInWidth, UINT nInHeight, UINT nOutWidth, UINT nOutHeight)
{
	if(!pIn)
	{
		return FALSE;
	}

	if(!pOut)
	{
		return FALSE;
	}

	if(nInWidth<1)
	{
		return FALSE;
	}

	if(nInHeight<1)
	{
		return FALSE;
	}

	if(nOutWidth<1)
	{
		return FALSE;
	}

	if(nOutHeight<1)
	{
		return FALSE;
	}

	UINT x, y;
	UINT dx, dy;
	UINT blockSizeX, blockSizeY;
	UINT In_width, in_hieght;
	UINT out_width, out_height;
	float buf;

	In_width = (UINT)(nInWidth);
	in_hieght = (UINT)(nInHeight);
	out_width = (UINT)(nOutWidth);
	out_height = (UINT)(nOutHeight);
	blockSizeX = (UINT)(nInWidth/nOutWidth);
	blockSizeY = (UINT)(nInHeight/nOutHeight);

	if(blockSizeX<1)
	{
		return FALSE;
	}

	if(blockSizeY<1)
	{
		return FALSE;
	}

	for (y=0;y<out_height;y++)
	{
		for (x=0;x<out_width;x++)
		{
			buf = 0;
			for (dy=0;dy<blockSizeY;dy++)
			{
				for (dx=0;dx<blockSizeX;dx++)
				{
					buf += (float)( *( pIn + ((y*blockSizeY+dy)*In_width) + ((x*(blockSizeX))+dx) ) );
				}
			}
			buf /= (float)blockSizeX*blockSizeY;
			*(pOut+y*nOutWidth+x) = (TYPE)buf;
		}
	}

	return TRUE;
}

template <class TYPE> BOOL crop_roi_image(TYPE* pImage, UINT nWidth, UINT nHeight, int nRoiX, int nRoiY, int nRoiWidth, int nRoiHeight)
{
	if (!pImage)
	{
		return FALSE;
	}

	if (nWidth*nHeight < 1)
	{
		return FALSE;
	}

	int i, j;
	int nX, nY;
	TYPE** arSrc;
	TYPE** arDst;

	nX = nWidth;
	nY = nHeight;

	if (nRoiX < 0)
	{
		return FALSE;
	}
	
	if (nRoiX >= nX-1)
	{
		return FALSE;
	}
	
	if (nRoiWidth < 1)
	{
		return FALSE;
	}
	
	if (nRoiWidth > nX)
	{
		return FALSE;
	}

	if (nRoiY < 0)
	{
		return FALSE;
	}

	if (nRoiY >= nY-1)
	{
		return FALSE;
	}

	if (nRoiHeight < 1)
	{
		return FALSE;
	}

	if (nRoiHeight > nY)
	{
		return FALSE;
	}

#ifdef USING_GLOBAL_MEMORY
	arSrc = (TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arSrc = (TYPE**)malloc(nY * sizeof(TYPE*));
#endif
	arSrc[0] = (TYPE*)pImage;
	for (i = 1; i < nY; i++)
	{
		arSrc[i] = arSrc[i-1]+nX;
	}

#ifdef USING_GLOBAL_MEMORY
	arDst = (TYPE**)theGlobal.m_GlobalMemoryManager.max_line2_pointer;
	arDst[0] = (TYPE*)theGlobal.m_GlobalMemoryManager.max_img_pointer;
#else
	arDst = (TYPE**)malloc(nRoiHeight * sizeof(TYPE*));
	arDst[0] = (TYPE*)malloc(nX*nY*sizeof(TYPE));
#endif
	for (i = 1; i < nRoiHeight; i++)
	{
		arDst[i] = arDst[i-1] + nRoiWidth;
	}

	omp_set_dynamic(1);
#pragma omp parallel for private(j) num_threads(GetOmpProcCount())
	for (i = 0; i < nRoiHeight; i++)
	{
		for (j = 0; j < nRoiWidth; j++)
		{
			arDst[i][j] = arSrc[i+nRoiY][j+nRoiX];
		}
	}
			

	memset(arSrc[0], 0, nX*nY*sizeof(TYPE));
	memcpy(arSrc[0], arDst[0], nRoiWidth*nRoiHeight*sizeof(TYPE));	

#ifndef USING_GLOBAL_MEMORY
	free(arDst[0]);
	free(arDst);
	free(arSrc);
#endif

	return TRUE;
}

template <class TYPE> BOOL rotate_image(TYPE* pImage, UINT nWidth, UINT nHeight, int nMode)
{
	if (!pImage)
	{
		return FALSE;
	}

	if ((int)(nWidth*nHeight) < 1)
	{
		return FALSE;
	}

	int i, j;
	int dx, dy;
	int dx_minus1, dy_minus1;
	TYPE **arSrs;
	TYPE **arDst;

	dx = nWidth;
	dy = nHeight;
	dx_minus1 = dx-1;
	dy_minus1 = dy-1;

#ifdef USING_GLOBAL_MEMORY
	arSrs = (TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arSrs = (TYPE**)malloc(dy * sizeof(TYPE*));
#endif
	arSrs[0] = (TYPE*)pImage;
	for (i = 1; i < dy; i++)
	{
		arSrs[i] = arSrs[i-1] + dx;
	}

#ifdef USING_GLOBAL_MEMORY
	arDst = (TYPE**)theGlobal.m_GlobalMemoryManager.max_line2_pointer;
	arDst[0] = (TYPE*)theGlobal.m_GlobalMemoryManager.max_img_pointer;
#else
	arDst = (TYPE**)malloc(dx * sizeof(TYPE*));
	arDst[0] = (TYPE*)malloc(dx*dy*sizeof(TYPE));
#endif
	for (i = 1; i < dx; i++)
	{
		arDst[i] = arDst[i-1] + dy;
	}

	if (!nMode)	//ccw
	{
		omp_set_dynamic(1);
#pragma omp parallel for private(j) num_threads(GetOmpProcCount())
		for (i = 0; i < dy; i++)
		{
			for (j = 0; j < dx; j++)
			{
				arDst[dx_minus1-j][i] = arSrs[i][j];
			}
		}
	}
	else //cw
	{
		omp_set_dynamic(1);
#pragma omp parallel for private(j) num_threads(GetOmpProcCount())
		for (i = 0; i < dy; i++)
		{
			for (j = 0; j < dx; j++)
			{
				arDst[j][dy_minus1-i] = arSrs[i][j];
			}
		}

	}

	memcpy(arSrs[0], arDst[0], dx*dy*sizeof(TYPE));	

#ifndef USING_GLOBAL_MEMORY
	free(arDst[0]);
	free(arDst);
	free(arSrs);
#endif
	
	return TRUE;
}

template <class TYPE> BOOL reverse_image(TYPE* pImage, UINT nWidth, UINT nHeight, int nMode)
{
	if (!pImage)
	{
		return FALSE;
	}

	if ((int)(nWidth*nHeight) < 1)
	{
		return FALSE;
	}

	int i, j;
	int nX, nY;
	int dx_minus1, dy_minus1;
	TYPE **arSrs;
	TYPE **arDst;

	nX = nWidth;
	nY = nHeight;
	dx_minus1 = nX-1;
	dy_minus1 = nY-1;

#ifdef USING_GLOBAL_MEMORY
	arSrs = (TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arSrs = (TYPE**)malloc(nY * sizeof(TYPE*));
#endif
	arSrs[0] = (TYPE*)pImage;
	for (i=1; i<nY; i++)
	{
		arSrs[i] = arSrs[i-1]+nX;
	}

#ifdef USING_GLOBAL_MEMORY
	arDst = (TYPE**)theGlobal.m_GlobalMemoryManager.max_line2_pointer;
	arDst[0] = (TYPE*)theGlobal.m_GlobalMemoryManager.max_img_pointer;
#else
	arDst = (TYPE**)malloc(nY * sizeof(TYPE*));
	arDst[0] = (TYPE*)malloc(nX*nY*sizeof(TYPE));
#endif
	for (i = 1; i < nY; i++)
	{
		arDst[i] = arDst[i-1]+nX;
	}

	if (!nMode)	//mirror
	{
		omp_set_dynamic(1);
#pragma omp parallel for private(j) num_threads(GetOmpProcCount())
		for (i = 0; i < nY; i++)
		{
			for (j = 0; j < nX; j++)
			{
				arDst[i][j] = arSrs[i][dx_minus1-j];
			}
		}
	}
	else //flip
	{
		omp_set_dynamic(1);
#pragma omp parallel for num_threads(GetOmpProcCount())
		for (i = 0; i < nY; i++)
		{
			memcpy(arDst[dy_minus1-i], arSrs[i], sizeof(TYPE)*nX);
		}
	}

	memcpy(arSrs[0], arDst[0], nX*nY*sizeof(TYPE));	

#ifndef USING_GLOBAL_MEMORY
	free(arDst[0]);
	free(arDst);
	free(arSrs);
#endif

	return TRUE;
}

template <class IN_TYPE, class OUT_TYPE> BOOL convert_image(IN_TYPE* pIn, OUT_TYPE* pOut, UINT nInUsingBits, UINT nOutUsingBits, UINT nWidth, UINT nHeight)
{
	if (!pIn)
	{
		return FALSE;
	}

	if (!pOut)
	{
		return FALSE;
	}

	if (nInUsingBits < 1)
	{
		return FALSE;
	}

	if (nOutUsingBits < 1)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	float fInTotalCount	= (float)(1 << nInUsingBits);
	float fOutTotalCount = (float)(1 << nOutUsingBits);		
	float fConv = (float)(fOutTotalCount/fInTotalCount);
	float fPixelValue = 0.0f;

	__m128 m128Temp;
	__m128 m128WndConv = _mm_set_ps1((float)fConv);
	__declspec(align(16)) float fTemp[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	int i = 0;
	int j = 0;
	int nPixelCount;
	int nQuadSize = 0;
	int nRemainSize = 0;
	nPixelCount = (int)(nWidth*nHeight);
	nQuadSize = (int)(nPixelCount/4);
	nRemainSize = (int)(nPixelCount%4);

	omp_set_dynamic(1);
#pragma omp parallel for private(j, m128Temp, fTemp) num_threads(GetOmpProcCount())
	for (i = 0; i < nQuadSize; i++)
	{
		j = i << 2;

		//copy in data -> float data;
		m128Temp = _mm_set_ps((float)(pIn[j+0]),				
			(float)(pIn[j+1]),
			(float)(pIn[j+2]),
			(float)(pIn[j+3]));

		//operating sse
		m128Temp = _mm_mul_ps(m128Temp,m128WndConv);		// 윈도우 Coef 로 곱한다..

		//float dat -> out data
		_mm_store_ps(fTemp, m128Temp);
		pOut[j+0] = (OUT_TYPE)(fTemp[3]);
		pOut[j+1] = (OUT_TYPE)(fTemp[2]);
		pOut[j+2] = (OUT_TYPE)(fTemp[1]);
		pOut[j+3] = (OUT_TYPE)(fTemp[0]);
	}

	j = nQuadSize << 2;
//	omp_set_dynamic(1);
#pragma omp parallel for private(fPixelValue) num_threads(GetOmpProcCount())
	for (i = 0; i < nRemainSize; i++)
	{
		fPixelValue =(float)(*(pIn+j+i));
		fPixelValue =(float)(fPixelValue*fConv);
		*(pOut+j+i) = (OUT_TYPE)(fPixelValue);
	}

	return TRUE;
}

template <class IN_TYPE, class OUT_TYPE> BOOL window_level_image(IN_TYPE* pIn, OUT_TYPE* pOut, UINT nInUsingBits, UINT nOutUsingBits, UINT nInBytesPerLine, UINT nOutBytesPerLine, UINT nWidth, UINT nHeight, int nW1, int nW2)
{
	if (!pIn)
	{
		return FALSE;
	}

	if (!pOut)
	{
		return FALSE;
	}

	if (nInUsingBits < 1)
	{
		return FALSE;
	}

	if (nOutUsingBits < 1)
	{
		return FALSE;
	}

	if (nInBytesPerLine < 1)
	{
		return FALSE;
	}

	if (nOutBytesPerLine < 1)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	if (nW1 >= nW2)
	{
		return FALSE;
	}

	IN_TYPE **arImageIn = NULL;
#ifdef USING_GLOBAL_MEMORY
	arImageIn = (IN_TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arImageIn = (IN_TYPE**)malloc(nHeight * sizeof(IN_TYPE*));
#endif
	if (!arImageIn)
	{
		return FALSE;
	}

	OUT_TYPE** arImageOut = NULL;
#ifdef USING_GLOBAL_MEMORY
	arImageOut = (OUT_TYPE**)theGlobal.m_GlobalMemoryManager.max_line2_pointer;
#else
	arImageOut = (OUT_TYPE**)malloc(nHeight * sizeof(OUT_TYPE*));
#endif
	if (!arImageOut)
	{
#ifndef USING_GLOBAL_MEMORY
		if (arImageIn)
		{
			free(arImageOut);
			arImageOut = NULL;
		}
#endif
		return FALSE;
	}

	arImageIn[0] = (IN_TYPE*)pIn;
	for (int i = 1; i < (int)nHeight; i++) 
	{
		BYTE *pBYTE = (BYTE*)arImageIn[i-1];
		pBYTE += nInBytesPerLine;
		arImageIn[i] = (IN_TYPE*)pBYTE;
	}

	arImageOut[0] = (OUT_TYPE*)pOut;
	for (int i = 1; i < (int)nHeight; i++) 
	{
		BYTE *pBYTE = (BYTE*)arImageOut[i-1];
		pBYTE += nOutBytesPerLine;
		arImageOut[i] = (OUT_TYPE*)pBYTE;
	}

	float fPixelValue = 0.0f;
	float fWndSt = (float)nW1;
	float fWndEd = (float)nW2;
	float fMaxPixelValue = (float)(1 << nInUsingBits) - 1;
	float fMinPixelValue = (float)0.0f;
	float fCoef = (float)((fWndEd-fWndSt)/fMaxPixelValue);
	float fInTotalCount	= (float)(1 << nInUsingBits);	
	float fOutTotalCount = (float)(1 << nOutUsingBits);	
	float fConv = (float)(fOutTotalCount/fInTotalCount);

	__m128 m128Temp;
	__m128 m128WndSt = _mm_set_ps1((float)fWndSt);
	__m128 m128WndCoef = _mm_set_ps1((float)fCoef); 
	__m128 m128WndMax = _mm_set_ps1(fMaxPixelValue);
	__m128 m128WndMin = _mm_set_ps1(fMinPixelValue);
	__m128 m128WndConv = _mm_set_ps1((float)fConv);

	__declspec(align(16)) float fTemp[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	int i, j;
	int nRow, nCol;
	int nPixelCount;
	int nQuadSize;
	int nRemainSize;
	nPixelCount	= nWidth*nHeight;
	nQuadSize = (int)(nPixelCount/4);
	nRemainSize = (int)(nPixelCount%4);

	omp_set_dynamic(1);
#pragma omp parallel for private(j, nRow, nCol, m128Temp, fTemp) num_threads(GetOmpProcCount())
	for (i = 0; i < nQuadSize; i++)
	{
		j = i << 2;
		nRow = (int)(j / nWidth);
		nCol = (int)(j % nWidth);

		//copy in data -> float data;
		m128Temp = _mm_set_ps((float)(arImageIn[nRow][nCol+0]),				
			(float)(arImageIn[nRow][nCol+1]),
			(float)(arImageIn[nRow][nCol+2]),
			(float)(arImageIn[nRow][nCol+3]));

		// SSE
		m128Temp = _mm_sub_ps(m128Temp, m128WndSt);		// 윈도우 low level로 Sub 한다.
		m128Temp = _mm_div_ps(m128Temp, m128WndCoef);		// 윈도우 Coef로 나눈다.
		m128Temp = _mm_min_ps(m128Temp, m128WndMax);		// 윈도우 Max 값 보다 크면 Max 값으로
		m128Temp = _mm_max_ps(m128Temp, m128WndMin);		// 윈도우 Min 값 보다 작으면 Min 값으로
		m128Temp = _mm_mul_ps(m128Temp, m128WndConv);		// 윈도우 Conv 로 곱한다..

		// float dat -> out data
		_mm_store_ps(fTemp, m128Temp);
		arImageOut[nRow][nCol+0] = (OUT_TYPE)(fTemp[3]);
		arImageOut[nRow][nCol+1] = (OUT_TYPE)(fTemp[2]);
		arImageOut[nRow][nCol+2] = (OUT_TYPE)(fTemp[1]);
		arImageOut[nRow][nCol+3] = (OUT_TYPE)(fTemp[0]);
	}

	j = nQuadSize << 2;
	nRow = (int)(j / nWidth);
	nCol = (int)(j % nWidth);
//	omp_set_dynamic(1);
#pragma omp parallel for private(fPixelValue) num_threads(GetOmpProcCount())
	for (i = 0; i < nRemainSize; i++)
	{
		fPixelValue =(float)(arImageIn[nRow][nCol+i]);
		fPixelValue =(float)(fPixelValue-fWndSt) / fCoef;
		if (fPixelValue < 0)
		{
			fPixelValue = 0;
		}

		if (fPixelValue > fMaxPixelValue)
		{
			fPixelValue = fMaxPixelValue;
		}
		fPixelValue = (float)(fPixelValue * fConv);
		arImageOut[nRow][nCol+i] = (OUT_TYPE)(fPixelValue);
	}

#ifndef USING_GLOBAL_MEMORY
	if (arImageIn)
	{
		free(arImageIn);
		arImageIn = NULL;
	}

	if (arImageOut)
	{
		free(arImageOut);
		arImageOut = NULL;
	}
#endif

	return TRUE;
}

template <class IN_TYPE, class OUT_TYPE> BOOL window_level_invert_image(IN_TYPE* pIn, OUT_TYPE* pOut, UINT nInUsingBits, UINT nOutUsingBits, UINT nInBytesPerLine, UINT nOutBytesPerLine, UINT nWidth, UINT nHeight, int nW1, int nW2)
{
	if (!pIn)
	{
		return FALSE;
	}

	if (!pOut)
	{
		return FALSE;
	}

	if (nInUsingBits < 1)
	{
		return FALSE;
	}

	if (nOutUsingBits < 1)
	{
		return FALSE;
	}

	if (nInBytesPerLine < 1)
	{
		return FALSE;
	}

	if (nOutBytesPerLine < 1)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	if (nW1 >= nW2)
	{
		return FALSE;
	}

	IN_TYPE **arImageIn = NULL;
#ifdef USING_GLOBAL_MEMORY
	arImageIn = (IN_TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arImageIn = (IN_TYPE**)malloc(nHeight * sizeof(IN_TYPE*));
#endif
	if (!arImageIn)
	{
		return FALSE;
	}

	OUT_TYPE **arImageOut = NULL;
#ifdef USING_GLOBAL_MEMORY
	arImageOut = (OUT_TYPE**)theGlobal.m_GlobalMemoryManager.max_line2_pointer;
#else
	arImageOut = (OUT_TYPE**)malloc(nHeight * sizeof(OUT_TYPE*));
#endif
	if (!arImageOut)
	{
#ifndef USING_GLOBAL_MEMORY
		if (arImageIn)
		{
			free(arImageOut);
			arImageOut = NULL;
		}
#endif
		return FALSE;
	}

	arImageIn[0] = (IN_TYPE*)pIn;
	for (int i = 1; i < (int)nHeight; i++) 
	{
		BYTE *pBYTE = (BYTE*)arImageIn[i-1];
		pBYTE += nInBytesPerLine;
		arImageIn[i] = (IN_TYPE*)pBYTE;
	}

	arImageOut[0] = (OUT_TYPE*)pOut;
	for (int i = 1; i < (int)nHeight; i++) 
	{
		BYTE *pBYTE = (BYTE*)arImageOut[i-1];
		pBYTE += nOutBytesPerLine;
		arImageOut[i] = (OUT_TYPE*)pBYTE;
	}

	float fPixelValue = 0.0f;
	float fWndSt = (float)nW1;
	float fWndEd = (float)nW2;
	float fMaxPixelValue = (float)(1 << nInUsingBits) - 1;		//위에 return 조건으로 절대 0일 될수 없음.
	float fMinPixelValue = (float)0.0f;
	float fCoef = (float)((fWndEd-fWndSt)/fMaxPixelValue);		//위에 return 조건으로 절대 0일 될수 없음.
	float fInTotalCount	= (float)(1 << nInUsingBits);	
	float fOutTotalCount = (float)(1 << nOutUsingBits);	
	float fConv = (float)(fOutTotalCount/fInTotalCount);

	__m128 m128Temp;
	__m128 m128WndSt = _mm_set_ps1((float)fWndSt);
	__m128 m128WndCoef	= _mm_set_ps1((float)fCoef); 
	__m128 m128WndMax = _mm_set_ps1(fMaxPixelValue);
	__m128 m128WndMin = _mm_set_ps1(fMinPixelValue);
	__m128 m128WndConv = _mm_set_ps1((float)fConv);

	__declspec(align(16)) float fTemp[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	int i, j;
	int nRow, nCol;
	int nPixelCount;
	int nQuadSize;
	int nRemainSize;
	nPixelCount	= nWidth*nHeight;
	nQuadSize = (int)(nPixelCount / 4);
	nRemainSize = (int)(nPixelCount % 4);

	omp_set_dynamic(1);
#pragma omp parallel for private(j, nRow, nCol, m128Temp, fTemp) num_threads(GetOmpProcCount())
	for (i = 0; i < nQuadSize; i++)
	{
		j = i << 2;
		nRow = (int)(j / nWidth);
		nCol = (int)(j % nWidth);

		//copy in data -> float data;
		m128Temp = _mm_set_ps((float)(arImageIn[nRow][nCol+0]),				
			(float)(arImageIn[nRow][nCol+1]),
			(float)(arImageIn[nRow][nCol+2]),
			(float)(arImageIn[nRow][nCol+3]));

		//operating sse
		m128Temp = _mm_sub_ps(m128WndMax, m128Temp);		// 윈도우 low level로 Sub 한다.
		m128Temp = _mm_sub_ps(m128Temp, m128WndSt);			// 윈도우 low level로 Sub 한다.
		m128Temp = _mm_div_ps(m128Temp, m128WndCoef);		// 윈도우 Coef로 나눈다.
		m128Temp = _mm_min_ps(m128Temp, m128WndMax);		// 윈도우 Max 값 보다 크면 Max 값으로
		m128Temp = _mm_max_ps(m128Temp, m128WndMin);		// 윈도우 Min 값 보다 작으면 Min 값으로
		m128Temp = _mm_mul_ps(m128Temp, m128WndConv);		// 윈도우 Conv 로 곱한다..

		//float dat -> out data
		_mm_store_ps(fTemp, m128Temp);
		arImageOut[nRow][nCol+0] = (OUT_TYPE)(fTemp[3]);
		arImageOut[nRow][nCol+1] = (OUT_TYPE)(fTemp[2]);
		arImageOut[nRow][nCol+2] = (OUT_TYPE)(fTemp[1]);
		arImageOut[nRow][nCol+3] = (OUT_TYPE)(fTemp[0]);
	}

	j = nQuadSize<<2;
	nRow = (int)(j/nWidth);
	nCol = (int)(j%nWidth);
//	omp_set_dynamic(1);
#pragma omp parallel for private(fPixelValue) num_threads(GetOmpProcCount())
	for (i = 0; i < nRemainSize; i++)
	{
		fPixelValue =(float)(arImageIn[nRow][nCol+i]);
		fPixelValue = (float)(fMaxPixelValue - fPixelValue);
		fPixelValue =(float)(fPixelValue - fWndSt)/fCoef;
		if (fPixelValue < 0)
		{
			fPixelValue = 0;
		}
		if (fPixelValue > fMaxPixelValue)
		{
			fPixelValue = fMaxPixelValue;
		}
		fPixelValue = (float)(fPixelValue*fConv);
		arImageOut[nRow][nCol+i] = (OUT_TYPE)(fPixelValue);
	}

#ifndef USING_GLOBAL_MEMORY
	if (arImageIn)
	{
		free(arImageIn);
		arImageIn = NULL;
	}

	if (arImageOut)
	{
		free(arImageOut);
		arImageOut = NULL;
	}
#endif

	return TRUE;
}

template <class IN_TYPE, class OUT_TYPE, class LUT_TYPE> BOOL window_level_lut_image(IN_TYPE* pIn, OUT_TYPE* pOut, LUT_TYPE *pLut, UINT nInUsingBits, UINT nOutUsingBits, UINT nLutUsingBits, UINT nInBytesPerLine, UINT nOutBytesPerLine, UINT nWidth, UINT nHeight, int nW1, int nW2)
{
	if (!pIn)
	{
		return FALSE;
	}

	if (!pOut)
	{
		return FALSE;
	}

	if (!pLut)
	{
		return FALSE;
	}

	if (nInUsingBits < 1)
	{
		return FALSE;
	}

	if (nOutUsingBits < 1)
	{
		return FALSE;
	}

	if (nLutUsingBits < 1)
	{
		return FALSE;
	}

	if (nInUsingBits > nLutUsingBits)
	{
		return FALSE;
	}

	if (nInBytesPerLine < 1)
	{
		return FALSE;
	}

	if (nOutBytesPerLine < 1)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	if (nW1 >= nW2)
	{
		return FALSE;
	}

	IN_TYPE **arImageIn = NULL;
#ifdef USING_GLOBAL_MEMORY
	arImageIn = (IN_TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arImageIn = (IN_TYPE**)malloc(nHeight * sizeof(IN_TYPE*));
#endif
	if (!arImageIn)
	{
		return FALSE;
	}

	OUT_TYPE **arImageOut = NULL;
#ifdef USING_GLOBAL_MEMORY
	arImageOut = (OUT_TYPE**)theGlobal.m_GlobalMemoryManager.max_line2_pointer;
#else
	arImageOut = (OUT_TYPE**)malloc(nHeight * sizeof(OUT_TYPE*));
#endif
	if (!arImageOut)
	{
#ifndef USING_GLOBAL_MEMORY
		if (arImageIn)
		{
			free(arImageOut);
			arImageOut = NULL;
		}
#endif

		return FALSE;
	}

	arImageIn[0] = (IN_TYPE*)pIn;
	for (int i = 1; i < (int)nHeight; i++) 
	{
		BYTE *pBYTE = (BYTE*)arImageIn[i-1];
		pBYTE += nInBytesPerLine;
		arImageIn[i] = (IN_TYPE*)pBYTE;
	}

	arImageOut[0] = (OUT_TYPE*)pOut;
	for (int i = 1; i < (int)nHeight; i++) 
	{
		BYTE *pBYTE = (BYTE*)arImageOut[i-1];
		pBYTE += nOutBytesPerLine;
		arImageOut[i] = (OUT_TYPE*)pBYTE;
	}

	float fPixelValue = 0.0f;
	float fWndSt = (float)nW1;
	float fWndEd = (float)nW2;
	float fMaxPixelValue = (float)(1 << nInUsingBits) - 1;
	float fMinPixelValue = 0.0f;
	float fCoef = (float)((fWndEd-fWndSt)/fMaxPixelValue);
	float fInTotalCount	= (float)(1 << nInUsingBits);	
	float fOutTotalCount = (float)(1 << nOutUsingBits);	
	float fLutTotalCOunt = (float)(1 << nLutUsingBits);
	float fConv = (float)(fOutTotalCount/fLutTotalCOunt);
	float fLut = (float)(fLutTotalCOunt/fInTotalCount);

	__m128 m128Temp;
	__m128 m128WndSt = _mm_set_ps1((float)fWndSt);
	__m128 m128WndCoef = _mm_set_ps1((float)fCoef); 
	__m128 m128WndMax = _mm_set_ps1(fMaxPixelValue);
	__m128 m128WndMin = _mm_set_ps1(fMinPixelValue);
	__m128 m128WndConv = _mm_set_ps1((float)fConv);
	__m128 m128WndLut = _mm_set_ps1((float)fLut);

	__declspec(align(16)) float fTemp[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	int i,j;
	int nRow, nCol;
	int nPixelCount;
	int nQuadSize;
	int nRemainSize;
	nPixelCount	= nWidth*nHeight;
	nQuadSize = (int)(nPixelCount / 4);
	nRemainSize = (int)(nPixelCount % 4);

	omp_set_dynamic(1);
#pragma omp parallel for private(j, nRow, nCol, m128Temp, fTemp) num_threads(GetOmpProcCount())
	for (i = 0; i < nQuadSize; i++)
	{
		j = i << 2;
		nRow = (int)(j / nWidth);
		nCol = (int)(j % nWidth);

		//copy in data -> float data;
		m128Temp = _mm_set_ps((float)(arImageIn[nRow][nCol+0]),				
			(float)(arImageIn[nRow][nCol+1]),
			(float)(arImageIn[nRow][nCol+2]),
			(float)(arImageIn[nRow][nCol+3]));

		// SSE
		m128Temp = _mm_sub_ps(m128Temp, m128WndSt);			// 윈도우 low level로 Sub 한다.
		m128Temp = _mm_div_ps(m128Temp, m128WndCoef);		// 윈도우 Coef로 나눈다.
		m128Temp = _mm_min_ps(m128Temp, m128WndMax);		// 윈도우 Max 값 보다 크면 Max 값으로
		m128Temp = _mm_max_ps(m128Temp, m128WndMin);		// 윈도우 Min 값 보다 작으면 Min 값으로

		//float dat -> out data
		_mm_store_ps(fTemp, m128Temp);
		arImageOut[nRow][nCol+0] = (OUT_TYPE)(pLut[(int)(fTemp[3]*fLut)]*fConv);
		arImageOut[nRow][nCol+1] = (OUT_TYPE)(pLut[(int)(fTemp[2]*fLut)]*fConv);
		arImageOut[nRow][nCol+2] = (OUT_TYPE)(pLut[(int)(fTemp[1]*fLut)]*fConv);
		arImageOut[nRow][nCol+3] = (OUT_TYPE)(pLut[(int)(fTemp[0]*fLut)]*fConv);
	}

	j = nQuadSize<<2;
	nRow = (int)(j / nWidth);
	nCol = (int)(j % nWidth);
//	omp_set_dynamic(1);
#pragma omp parallel for private(fPixelValue) num_threads(GetOmpProcCount())
	for(i=0; i<nRemainSize; i++)
	{
		fPixelValue =(float)(arImageIn[nRow][nCol+i]);
		fPixelValue =(float)(fPixelValue - fWndSt)/fCoef;
		if (fPixelValue < 0)
		{
			fPixelValue = 0;
		}
		if (fPixelValue > fMaxPixelValue)
		{
			fPixelValue = fMaxPixelValue;
		}
		fPixelValue =(float)pLut[(int)fPixelValue];
		fPixelValue =(float)(fPixelValue*fLut);
		fPixelValue =(float)(fPixelValue*fConv);
		arImageOut[nRow][nCol+i] = (OUT_TYPE)(fPixelValue);
	}

#ifndef USING_GLOBAL_MEMORY
	if (arImageIn)
	{
		free(arImageIn);
		arImageIn = NULL;
	}

	if (arImageOut)
	{
		free(arImageOut);
		arImageOut = NULL;
	}
#endif

	return TRUE;
}

template <class IN_TYPE, class OUT_TYPE> BOOL burn_overlay_on_image(IN_TYPE* pOverlayInput, OUT_TYPE* pOut, UINT nOvlBitsPerPixel, UINT nOvlBytePerLine, UINT nOutHeight, UINT nOutWidth, UINT nOutMaxValue)
{
	BOOL bRes = FALSE;

	if (!pOverlayInput)
	{
		return FALSE;
	}

	if (!pOut)
	{
		return FALSE;
	}

	if (nOvlBitsPerPixel < 0)
	{
		return FALSE;
	}

	if (nOvlBytePerLine < 0)
	{
		return FALSE;
	}

	if (nOutHeight < 0)
	{
		return FALSE;
	}
	
	if (nOvlBitsPerPixel == 1)
	{
		INT_PTR nBit = 0;
		INT_PTR nOverlayTotalByte = nOvlBytePerLine * nOutHeight;
		
		char cMask = 0x01;
		char cInput;
		char cBitValue;

		BOOL bNeedLineChange;
		INT_PTR iOutLineByte = 0;
		INT_PTR iOverlayLineByte = 0;
		// Need to change for fitting image.
		for (INT_PTR iHeight = 0; iHeight < nOutHeight; iHeight++)
		{
			for (INT_PTR iOverlayBit = 7; iOverlayBit >= 0; iOverlayBit--)
			{
				if (iOutLineByte >= nOutWidth)
				{
					continue;
				}

				cInput = *pOverlayInput >> iOverlayBit;
				cBitValue = cInput & cMask;

				if (cBitValue > 0)
				{
					*pOut = nOutMaxValue;
				}
				//
				pOut++;
				iOutLineByte++;
			}

			iOverlayLineByte++;
			pOverlayInput++;

			// sometime, overlay byteperline is over out image's width
			if (iOverlayLineByte < nOvlBytePerLine)
			{
				iHeight--;
			}
			else
			{
				iOverlayLineByte = 0;
				iOutLineByte = 0;
			}
				
		}

		bRes = TRUE;
	}
	
	return bRes;
}

template <class TYPE> BOOL mask_circle_mask_data(TYPE* pMask, UINT nWidth, UINT nHeight, UINT nCircleMask)
{
	if (!pMask)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	int i, j;
	int dx, dy;
	int dx_half, dy_half;
	int section1, section2;
	float dRadius;
	float dDiameter;

	dx = nWidth;
	dy = nHeight;
	dx_half = (int)(dx*0.5+0.5);
	dy_half = (int)(dy*0.5+0.5);

	dRadius = (float)nCircleMask;
	dDiameter = dRadius*2;

	if(dDiameter<=0)
	{
		omp_set_dynamic(1);
#pragma omp parallel for num_threads(GetOmpProcCount())
		for(i=0; i<dy; i++)
		{
			pMask[i] = (TYPE)dx;
		}
	}
	else
	{
		section1 = dy_half - (int)(dRadius+0.5);
		section2 = dy_half + (int)(dRadius+0.5);

		omp_set_dynamic(1);
#pragma omp parallel for private(j) num_threads(GetOmpProcCount())
		for(i=0; i<dy; i++)
		{
			if(i<=section1)
				pMask[i] = (TYPE)dx_half;
			else if(i>=section2)
				pMask[i] = (TYPE)dx_half;
			else
			{
				j = dx_half - (int)(dRadius*sin(acos((dy_half-i)/dRadius))+0.5);
				if(j<0)
					pMask[i] = (TYPE)0;
				else if(j>dx_half)
					pMask[i] = (TYPE)dx_half;
				else
					pMask[i] = (TYPE)j;
			}
		}
	}

	return TRUE;
}

template <> BOOL make_histogram_data<RGBTRIPLE*>(RGBTRIPLE** pImage, double* pHistogram, WORD* pMask, UINT nWidth, UINT nHeight, UINT nUsingBits, double& max_histogram)
{
	if (!pImage)
	{
		return FALSE;
	}

	if (!(*pImage))
	{
		return FALSE;
	}

	if (!pHistogram)
	{
		return FALSE;
	}

	/*
	if(!pMask)
	{
		return FALSE;
	}
	*/

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	if (nUsingBits < 1)
	{
		return FALSE;
	}

	max_histogram = 0;

	UINT i, j;
	UINT dx, dy;
	UINT x_st, x_end;
	UINT maver_width;
	UINT maver_width_half;
	UINT nMaxValue;
	UINT nTotalCount;
	UINT nRefTotalCount;
	double fConv;
	double sum, ratio;
	RGBTRIPLE **arImage = NULL;
	double *pHistogramTemp = NULL;
	double *ip, *jp;

	nMaxValue = (UINT)((1<<nUsingBits)-1);
	nTotalCount = (UINT)(1<<nUsingBits);
	nRefTotalCount = (UINT)(1<<12);
	fConv = (double)((double)nTotalCount/(double)nRefTotalCount);

#ifdef USING_GLOBAL_MEMORY
	pHistogramTemp = theGlobal.m_GlobalMemoryManager.max_histogram_pointer;
#else
	pHistogramTemp = new double[nTotalCount*2];
#endif
	if(!pHistogramTemp)
	{
		return FALSE;
	}
	memset(pHistogram, 0, sizeof(double)*nTotalCount*2);
	memset(pHistogramTemp, 0, sizeof(double)*nTotalCount*2);
	dx = nWidth;
	dy = nHeight;
	maver_width_half = (int)(128*fConv);
	maver_width = (int)maver_width_half*2;

#ifdef USING_GLOBAL_MEMORY
	arImage = (RGBTRIPLE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arImage = (RGBTRIPLE**)malloc(dy * sizeof(RGBTRIPLE*));
#endif
	if(!arImage)
	{
#ifndef USING_GLOBAL_MEMORY
		if(pHistogramTemp)
		{
			delete []pHistogramTemp;
			pHistogramTemp = NULL;
		}
#endif
		return FALSE;
	}
	arImage[0] = (RGBTRIPLE*)*pImage;
	for(i=1; i<dy; i++) 
		arImage[i] = arImage[i-1]+dx;

	double fTemp = 0;
	for (i=0; i<dy; i++)
	{
		if(pMask)
		{
			x_st = pMask[i];
			x_end = dx - pMask[i];
		}
		else
		{
			x_st = 0;
			x_end = dx;
		}
		
		for(j=x_st; j<x_end; j++)
		{
			RGBTRIPLE rgb_quad = (RGBTRIPLE)(arImage[i][j]);
			//pHistogramTemp[rgb_quad.rgbtBlue]++;
			//pHistogramTemp[rgb_quad.rgbtGreen]++;
			//pHistogramTemp[rgb_quad.rgbtRed]++;
			fTemp = (float)((BYTE)rgb_quad.rgbtRed);
			fTemp += (float)((BYTE)rgb_quad.rgbtGreen);
			fTemp += (float)((BYTE)rgb_quad.rgbtBlue);
			fTemp /= (float)3;
			if(fTemp<0)
				fTemp = 0;
			if(fTemp>nMaxValue)
				fTemp = nMaxValue;
			pHistogramTemp[(BYTE)fTemp]++;
		}
	}

	//pHistogramTemp[0] = 0;
	memset(pHistogramTemp, 0, sizeof(double)*(int)((double)maver_width_half/2));

	sum = 0;
	ratio = (double)1/maver_width;
	ip = pHistogramTemp;
	jp = ip;
	for(i=0; i<maver_width_half; i++, jp++)
	{
		sum += (double)*jp;
	}
	pHistogram[0] = (double)(sum/maver_width_half);
	max_histogram = pHistogram[0];
	for(i=1; i<(nTotalCount+maver_width); i++)
	{
		if(i<=maver_width_half)
		{
			sum += (double)*jp;
			pHistogram[i] = (double)(sum/(maver_width_half+i));
			jp++;
		}
		else
		{
			sum -= (double)*ip;
			sum += (double)*jp;
			pHistogram[i] = (double)(sum*ratio);
			ip++;
			jp++;
		}
		if(max_histogram<pHistogram[i])
			max_histogram = pHistogram[i];
	}

	pHistogram[0] = 0;	//mask gray level은 0으로 reset함.

#ifndef USING_GLOBAL_MEMORY
	if(pHistogramTemp)
	{
		delete []pHistogramTemp;
		pHistogramTemp = NULL;
	}

	if(arImage)
	{
		free(arImage);
	}
#endif

	return TRUE;
}

template <> BOOL make_histogram_data<RGBQUAD*>(RGBQUAD** pImage, double* pHistogram, WORD* pMask, UINT nWidth, UINT nHeight, UINT nUsingBits, double& max_histogram)
{
	if (!pImage)
	{
		return FALSE;
	}

	if (!(*pImage))
	{
		return FALSE;
	}

	if (!pHistogram)
	{
		return FALSE;
	}

	/*
	if(!pMask)
	{
		return FALSE;
	}
	*/

	if(nWidth<1)
	{
		return FALSE;
	}

	if(nHeight<1)
	{
		return FALSE;
	}

	if(nUsingBits<1)
	{
		return FALSE;
	}

	max_histogram = 0;

	UINT i, j;
	UINT dx, dy;
	UINT x_st, x_end;
	UINT maver_width;
	UINT maver_width_half;
	UINT nMaxValue;
	UINT nTotalCount;
	UINT nRefTotalCount;
	double fConv;
	double sum, ratio;
	RGBQUAD **arImage = NULL;
	double *pHistogramTemp = NULL;
	double *ip, *jp;

	nMaxValue = (UINT)((1<<nUsingBits)-1);
	nTotalCount = (UINT)(1<<nUsingBits);
	nRefTotalCount = (UINT)(1<<12);
	fConv = (double)((double)nTotalCount/(double)nRefTotalCount);

#ifdef USING_GLOBAL_MEMORY
	pHistogramTemp = theGlobal.m_GlobalMemoryManager.max_histogram_pointer;
#else
	pHistogramTemp = new double[nTotalCount*2];
#endif
	if(!pHistogramTemp)
	{
		return FALSE;
	}
	memset(pHistogram, 0, sizeof(double)*nTotalCount*2);
	memset(pHistogramTemp, 0, sizeof(double)*nTotalCount*2);
	dx = nWidth;
	dy = nHeight;
	maver_width_half = (int)(128*fConv);
	maver_width = (int)maver_width_half*2;

#ifdef USING_GLOBAL_MEMORY
	arImage = (RGBQUAD**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arImage = (RGBQUAD**)malloc(dy * sizeof(RGBQUAD*));
#endif
	if(!arImage)
	{
#ifndef USING_GLOBAL_MEMORY
		if(pHistogramTemp)
		{
			delete []pHistogramTemp;
			pHistogramTemp = NULL;
		}
#endif

		return FALSE;
	}
	arImage[0] = (RGBQUAD*)*pImage;
	for(i=1; i<dy; i++) 
		arImage[i] = arImage[i-1]+dx;

	double fTemp = 0;
	for(i=0; i<dy; i++)
	{
		if(pMask)
		{
			x_st = pMask[i];
			x_end = dx - pMask[i];
		}
		else
		{
			x_st = 0;
			x_end = dx;
		}

		for(j=x_st; j<x_end; j++)
		{
			RGBQUAD rgb_quad = (RGBQUAD)(arImage[i][j]);
			//pHistogramTemp[rgb_quad.rgbBlue]++;
			//pHistogramTemp[rgb_quad.rgbGreen]++;
			//pHistogramTemp[rgb_quad.rgbRed]++;
			//rgb_quad.rgbReserved;
			fTemp = (float)((BYTE)rgb_quad.rgbRed);
			fTemp += (float)((BYTE)rgb_quad.rgbGreen);
			fTemp += (float)((BYTE)rgb_quad.rgbBlue);
			fTemp /= (float)3;
			if(fTemp<0)
				fTemp = 0;
			if(fTemp>nMaxValue)
				fTemp = nMaxValue;
			pHistogramTemp[(BYTE)fTemp]++;

		}
	}

	//pHistogramTemp[0] = 0;
	memset(pHistogramTemp, 0, sizeof(double)*(int)((double)maver_width_half/2));
	sum = 0;
	ratio = (double)1/maver_width;
	ip = pHistogramTemp;
	jp = ip;
	for(i=0; i<maver_width_half; i++, jp++)
	{
		sum += (double)*jp;
	}
	pHistogram[0] = (double)(sum/maver_width_half);
	max_histogram = pHistogram[0];
	for(i=1; i<(nTotalCount+maver_width); i++)
	{
		if(i<=maver_width_half)
		{
			sum += (double)*jp;
			pHistogram[i] = (double)(sum/(maver_width_half+i));
			jp++;
		}
		else
		{
			sum -= (double)*ip;
			sum += (double)*jp;
			pHistogram[i] = (double)(sum*ratio);
			ip++;
			jp++;
		}
		if(max_histogram<pHistogram[i])
			max_histogram = pHistogram[i];
	}

	pHistogram[0] = 0;	//mask gray level은 0으로 reset함.

#ifndef USING_GLOBAL_MEMORY
	if(pHistogramTemp)
	{
		delete []pHistogramTemp;
		pHistogramTemp = NULL;
	}

	if(arImage)
	{
		free(arImage);
	}
#endif

	return TRUE;
}

template <class IMAGE_TYPE> BOOL make_histogram_data(IMAGE_TYPE* pImage, double* pHistogram, WORD* pMask, UINT nWidth, UINT nHeight, UINT nUsingBits, double& max_histogram)
{
	if (!pImage)
	{
		return FALSE;
	}

	if (!pHistogram)
	{
		return FALSE;
	}

	/*
	if(!pMask)
	{
		return FALSE;
	}
	*/

	if (nWidth<1)
	{
		return FALSE;
	}

	if (nHeight<1)
	{
		return FALSE;
	}

	if (nUsingBits<1)
	{
		return FALSE;
	}

	max_histogram = 0.0;

	UINT i, j;
	UINT dx, dy;
	UINT x_st, x_end;
	UINT maver_width;
	UINT maver_width_half;
	UINT nMaxValue;
	UINT nTotalCount;
	UINT nRefTotalCount;
	double fConv;
	double sum, ratio;
	IMAGE_TYPE **arImage = NULL;
	double *pHistogramTemp = NULL;
	double *ip, *jp;

	nMaxValue = (UINT)((1<<nUsingBits)-1);
	nTotalCount = (UINT)(1<<nUsingBits);
	nRefTotalCount = (UINT)(1<<12);
	fConv = (double)((double)nTotalCount/(double)nRefTotalCount);

#ifdef USING_GLOBAL_MEMORY
	pHistogramTemp = theGlobal.m_GlobalMemoryManager.max_histogram_pointer;
#else
	pHistogramTemp = new double[nTotalCount*2];
#endif
	if(!pHistogramTemp)
	{
		return FALSE;
	}
	memset(pHistogram, 0, sizeof(double)*nTotalCount*2);
	memset(pHistogramTemp, 0, sizeof(double)*nTotalCount*2);
	dx = nWidth;
	dy = nHeight;
	maver_width_half = (int)(128*fConv);
	maver_width = (int)maver_width_half*2;

#ifdef USING_GLOBAL_MEMORY
	arImage = (IMAGE_TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arImage = (IMAGE_TYPE**)malloc(dy * sizeof(IMAGE_TYPE*));
#endif
	if(!arImage)
	{
#ifndef USING_GLOBAL_MEMORY
		if(pHistogramTemp)
		{
			delete []pHistogramTemp;
			pHistogramTemp = NULL;
		}
#endif

		return FALSE;
	}
	arImage[0] = (IMAGE_TYPE*)pImage;
	for(i=1; i<dy; i++) 
		arImage[i] = arImage[i-1]+dx;

	UINT nHistIndex = 0;
	for(i=0; i<dy; i++)
	{
		if(pMask)
		{
			x_st = pMask[i];
			x_end = dx - pMask[i];
		}
		else
		{
			x_st = 0;
			x_end = dx;
		}

		for(j=x_st; j<x_end; j++)
		{
			nHistIndex = (UINT)arImage[i][j];
			if(nHistIndex<0)
				nHistIndex = 0;
			if(nHistIndex>nMaxValue)
				nHistIndex = nMaxValue;
			pHistogramTemp[nHistIndex]++;
		}
	}


	//pHistogramTemp[0] = 0;
	memset(pHistogramTemp, 0, sizeof(double)*(int)((double)maver_width_half/2));

	sum = 0;
	ratio = (double)1/maver_width;
	ip = pHistogramTemp;
	jp = ip;
	for(i=0; i<maver_width_half; i++, jp++)
	{
		sum += (double)*jp;
	}
	pHistogram[0] = (double)(sum/maver_width_half);
	max_histogram = pHistogram[0];
	for(i=1; i<(nTotalCount+maver_width); i++)
	{
		if(i<=maver_width_half)
		{
			sum += (double)*jp;
			pHistogram[i] = (double)(sum/(maver_width_half+i));
			jp++;
		}
		else
		{
			sum -= (double)*ip;
			sum += (double)*jp;
			pHistogram[i] = (double)(sum*ratio);
			ip++;
			jp++;
		}
		if(max_histogram<pHistogram[i])
			max_histogram = pHistogram[i];
	}

	pHistogram[0] = 0;	//mask gray level은 0으로 reset함.

#ifndef USING_GLOBAL_MEMORY
	if(pHistogramTemp)
	{
		delete []pHistogramTemp;
		pHistogramTemp = NULL;
	}

	if(arImage)
	{
		free(arImage);
	}
#endif

	return TRUE;
}

template <class TYPE> BOOL adjust_circle_mask_data(TYPE* pImage, WORD* pMask, UINT nWidth, UINT nHeight)
{
	if (!pImage)
	{
		return FALSE;
	}

	if (!pMask)
	{
		return FALSE;
	}

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	int i;
	int dx, dy;
	TYPE** arImage;

	dx = nWidth;
	dy = nHeight;
#ifdef USING_GLOBAL_MEMORY
	arImage = (TYPE**)theGlobal.m_GlobalMemoryManager.max_line1_pointer;
#else
	arImage = (TYPE**)malloc(dy * sizeof(TYPE*));
#endif
	arImage[0] = (TYPE*)pImage;
	for(i=1; i<dy; i++) arImage[i] = arImage[i-1]+dx;

	omp_set_dynamic(1);
#pragma omp parallel for num_threads(GetOmpProcCount())
	for (i=0; i<dy; i++)
	{
		if (pMask[i]>0)
		{
			memset(arImage[i], 0, pMask[i]*sizeof(TYPE));
			memset(&arImage[i][dx-pMask[i]], 0, pMask[i]*sizeof(TYPE));
		}
	}

#ifndef USING_GLOBAL_MEMORY
	free(arImage);
#endif

	return TRUE;
}

template <> BOOL burn_dc_object<RGBTRIPLE*>(RGBTRIPLE** pImage, UINT nWidth, UINT nHeight, UINT nUsingBits, CDC* pDC, CBitmap* pBitmap, COLORREF crTransparent, BOOL bFlip)
{
	BITMAP bm;
	BITMAPINFO bi;
	RGBQUAD* pDib;
	RGBTRIPLE *ip, *jp;
	RGBQUAD* kp, *lp;

	if (!pImage)
	{
		return FALSE;
	}

	if (!(*pImage))
	{
		return  FALSE;
	}	

	if (nWidth<1)
	{
		return FALSE;
	}

	if (nHeight<1)
	{
		return FALSE;
	}

	if (nUsingBits<1)
	{
		return FALSE;
	}

	if(!pDC)
	{
		return FALSE;
	}

	if (!pDC->GetSafeHdc())
	{
		return FALSE;
	}

	if (!pBitmap)
	{
		return FALSE;
	}

	if (!pBitmap->GetSafeHandle())
	{
		return FALSE;
	}

	if (!::GetObject((HBITMAP)pBitmap->GetSafeHandle(), sizeof(BITMAP), &bm))
	{
		return FALSE;
	}

	if (bm.bmWidth!=nWidth)
	{
		return FALSE;
	}

	if (bm.bmHeight!=nHeight)
	{
		return FALSE;
	}

	if (bm.bmBitsPixel!=32)
	{
		return FALSE;
	}

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bm.bmWidth;
	bi.bmiHeader.biHeight = bm.bmHeight;
	bi.bmiHeader.biPlanes = bm.bmPlanes;
	bi.bmiHeader.biBitCount = bm.bmBitsPixel;	
	bi.bmiHeader.biCompression = BI_RGB;		
	bi.bmiHeader.biSizeImage = (( (bm.bmWidth * 8 + 31) & (~31) ) / 8) * bm.bmHeight;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	pDib = NULL;
#ifdef USING_GLOBAL_MEMORY
	pDib = theGlobal.m_GlobalMemoryManager.max_dib_pointer;
#else
	pDib = new RGBQUAD[bi.bmiHeader.biSizeImage];
#endif
	if(!pDib)
	{
		return FALSE;
	}

	if (!::GetDIBits(pDC->GetSafeHdc(), (HBITMAP)pBitmap->GetSafeHandle(), 0, bi.bmiHeader.biHeight, pDib , &bi, DIB_RGB_COLORS))
	{
#ifndef USING_GLOBAL_MEMORY
		if(pDib)
		{
			delete[] pDib;
			pDib = NULL;
		}
#endif

		return FALSE;
	}

	UINT i, j;
	UINT dx, dy;
	float fImageMaxValue = (float)((1 << nUsingBits)-1);		
	float fDibMaxValue = (float)((1 << 8)-1);		
	float fConv = (float)(fImageMaxValue/fDibMaxValue);
	float fTemp = 0.0f;

	dx = nWidth;
	dy = nHeight;

	if (bFlip)
	{
		ip = *pImage;
		kp = pDib+(dy-1)*dx;
		for (i=0; i<dy; i++, ip+=dx, kp-=dx)
		{
			jp = ip;
			lp = kp;
			for (j=0; j<dx; j++, jp++, lp++)
			{
				if (crTransparent != RGB(lp->rgbRed, lp->rgbGreen, lp->rgbBlue))
				{
					jp->rgbtRed = lp->rgbRed;
					jp->rgbtGreen = lp->rgbGreen;
					jp->rgbtBlue = lp->rgbBlue;
				}
			}
		}
	}
	else
	{
		ip = *pImage;
		kp = pDib;
		for (i=0; i<dy; i++, ip+=dx, kp+=dx)
		{
			jp = ip;
			lp = kp;
			for (j=0; j<dx; j++, jp++, lp++)
			{
				if (crTransparent != RGB(lp->rgbRed, lp->rgbGreen, lp->rgbBlue))
				{
					jp->rgbtRed = lp->rgbRed;
					jp->rgbtGreen = lp->rgbGreen;
					jp->rgbtBlue = lp->rgbBlue;
				}
			}
		}
	}

#ifndef USING_GLOBAL_MEMORY
	if(pDib)
	{
		delete[] pDib;
		pDib = NULL;
	}
#endif

	return TRUE;
}

template < > BOOL burn_dc_object<RGBQUAD*>(RGBQUAD** pImage, UINT nWidth, UINT nHeight, UINT nUsingBits, CDC* pDC, CBitmap* pBitmap, COLORREF crTransparent, BOOL bFlip)
{
	BITMAP bm;
	BITMAPINFO bi;
	RGBQUAD *pDib;
	RGBQUAD *ip, *jp;
	RGBQUAD *kp, *lp;

	if (!pImage)
	{
		return FALSE;
	}

	if (!(*pImage))
	{
		return  FALSE;
	}	

	if (nWidth < 1)
	{
		return FALSE;
	}

	if (nHeight < 1)
	{
		return FALSE;
	}

	if (nUsingBits < 1)
	{
		return FALSE;
	}

	if (!pDC)
	{
		return FALSE;
	}

	if (!pDC->GetSafeHdc())
	{
		return FALSE;
	}

	if (!pBitmap)
	{
		return FALSE;
	}

	if (!pBitmap->GetSafeHandle())
	{
		return FALSE;
	}

	if (!::GetObject((HBITMAP)pBitmap->GetSafeHandle(), sizeof(BITMAP), &bm))
	{
		return FALSE;
	}

	if (bm.bmWidth != nWidth)
	{
		return FALSE;
	}

	if (bm.bmHeight != nHeight)
	{
		return FALSE;
	}

	if (bm.bmBitsPixel!=32)
	{
		return FALSE;
	}

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bm.bmWidth;
	bi.bmiHeader.biHeight = bm.bmHeight;
	bi.bmiHeader.biPlanes = bm.bmPlanes;
	bi.bmiHeader.biBitCount = bm.bmBitsPixel;	
	bi.bmiHeader.biCompression = BI_RGB;		
	bi.bmiHeader.biSizeImage = (( (bm.bmWidth * 8 + 31) & (~31) ) / 8) * bm.bmHeight;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	pDib = NULL;
#ifdef USING_GLOBAL_MEMORY
	pDib = theGlobal.m_GlobalMemoryManager.max_dib_pointer;
#else
	pDib = new RGBQUAD[bi.bmiHeader.biSizeImage];
#endif
	if (!pDib)
	{
		return FALSE;
	}

	if (!::GetDIBits(pDC->GetSafeHdc(), (HBITMAP)pBitmap->GetSafeHandle(), 0, bi.bmiHeader.biHeight, pDib , &bi, DIB_RGB_COLORS))
	{
#ifndef USING_GLOBAL_MEMORY
		if(pDib)
		{
			delete[] pDib;
			pDib = NULL;
		}
#endif

		return FALSE;
	}

	UINT i, j;
	UINT dx, dy;
	float fImageMaxValue = (float)((1 << nUsingBits)-1);		
	float fDibMaxValue = (float)((1 << 8)-1);		
	float fConv = (float)(fImageMaxValue/fDibMaxValue);
	float fTemp = 0.0f;

	dx = nWidth;
	dy = nHeight;

	if (bFlip)
	{
		ip = *pImage;
		kp = pDib+(dy-1)*dx;
		for (i=0; i<dy; i++, ip+=dx, kp-=dx)
		{
			jp = ip;
			lp = kp;
			for (j=0; j<dx; j++, jp++, lp++)
			{
				if (crTransparent != RGB(lp->rgbRed, lp->rgbGreen, lp->rgbBlue))
				{
					jp->rgbRed = lp->rgbRed;
					jp->rgbGreen = lp->rgbGreen;
					jp->rgbBlue = lp->rgbBlue;
					jp->rgbReserved = lp->rgbReserved;
				}
			}
		}
	}
	else
	{
		ip = *pImage;
		kp = pDib;
		for (i=0; i<dy; i++, ip+=dx, kp+=dx)
		{
			jp = ip;
			lp = kp;
			for (j=0; j<dx; j++, jp++, lp++)
			{
				if (crTransparent != RGB(lp->rgbRed, lp->rgbGreen, lp->rgbBlue))
				{
					jp->rgbRed = lp->rgbRed;
					jp->rgbGreen = lp->rgbGreen;
					jp->rgbBlue = lp->rgbBlue;
					jp->rgbReserved = lp->rgbReserved;
				}
			}
		}
	}

#ifndef USING_GLOBAL_MEMORY
	if(pDib)
	{
		delete[] pDib;
		pDib = NULL;
	}
#endif

	return TRUE;
}

template <class TYPE> BOOL burn_dc_object(TYPE* pImage, UINT nWidth, UINT nHeight, UINT nUsingBits, CDC* pDC, CBitmap* pBitmap, COLORREF crTransparent, BOOL bFlip)
{
	BITMAP bm;
	BITMAPINFO bi;
	RGBQUAD* pDib;
	TYPE *ip, *jp;
	RGBQUAD* kp, *lp;

	if (!pImage)
	{
		return FALSE;
	}

	if (nWidth<1)
	{
		return FALSE;
	}

	if (nHeight<1)
	{
		return FALSE;
	}

	if (nUsingBits<1)
	{
		return FALSE;
	}

	if (!pDC)
	{
		return FALSE;
	}

	if (!pDC->GetSafeHdc())
	{
		return FALSE;
	}

	if (!pBitmap)
	{
		return FALSE;
	}

	if (!pBitmap->GetSafeHandle())
	{
		return FALSE;
	}

	if (!::GetObject((HBITMAP)pBitmap->GetSafeHandle(), sizeof(BITMAP), &bm))
	{
		return FALSE;
	}

	if (bm.bmWidth!=nWidth)
	{
		return FALSE;
	}

	if (bm.bmHeight!=nHeight)
	{
		return FALSE;
	}

	if (bm.bmBitsPixel!=32)
	{
		return FALSE;
	}

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bm.bmWidth;
	bi.bmiHeader.biHeight = bm.bmHeight;
	bi.bmiHeader.biPlanes = bm.bmPlanes;
	bi.bmiHeader.biBitCount = bm.bmBitsPixel;	
	bi.bmiHeader.biCompression = BI_RGB;		
	bi.bmiHeader.biSizeImage = (( (bm.bmWidth * 8 + 31) & (~31) ) / 8) * bm.bmHeight;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	pDib = NULL;
#ifdef USING_GLOBAL_MEMORY
	pDib = theGlobal.m_GlobalMemoryManager.max_dib_pointer;
#else
	pDib = new RGBQUAD[bi.bmiHeader.biSizeImage];
#endif
	if(!pDib)
	{
		return FALSE;
	}

	if (!::GetDIBits(pDC->GetSafeHdc(), (HBITMAP)pBitmap->GetSafeHandle(), 0, bi.bmiHeader.biHeight, pDib , &bi, DIB_RGB_COLORS))
	{
#ifndef USING_GLOBAL_MEMORY
		if(pDib)
		{
			delete[] pDib;
			pDib = NULL;
		}
#endif

		return FALSE;
	}

	UINT i, j;
	UINT dx, dy;
	float fImageMaxValue = (float)((1 << nUsingBits)-1);		
	float fDibMaxValue = (float)((1 << 8)-1);		
	float fConv = (float)(fImageMaxValue/fDibMaxValue);
	float fTemp = 0.0f;

	dx = nWidth;
	dy = nHeight;

	if(bFlip)
	{
		ip = pImage;
		kp = pDib+(dy-1)*dx;
		for (i=0; i<dy; i++, ip+=dx, kp-=dx)
		{
			jp = ip;
			lp = kp;
			for (j=0; j<dx; j++, jp++, lp++)
			{
				if (crTransparent != RGB(lp->rgbRed, lp->rgbGreen, lp->rgbBlue))
				{
					fTemp = (float)(lp->rgbRed);
					fTemp += (float)(lp->rgbGreen);
					fTemp += (float)(lp->rgbBlue);
					fTemp /= (float)3;
					fTemp *= (float)fConv;
					*jp = (TYPE)fTemp;
				}
			}
		}
	}
	else
	{
		ip = pImage;
		kp = pDib;
		for (i=0; i<dy; i++, ip+=dx, kp+=dx)
		{
			jp = ip;
			lp = kp;
			for (j=0; j<dx; j++, jp++, lp++)
			{
				if (crTransparent != RGB(lp->rgbRed, lp->rgbGreen, lp->rgbBlue))
				{
					fTemp = (float)(lp->rgbRed);
					fTemp += (float)(lp->rgbGreen);
					fTemp += (float)(lp->rgbBlue);
					fTemp /= (float)3;
					fTemp *= (float)fConv;
					*jp = (TYPE)fTemp;
				}
			}
		}
	}

#ifndef USING_GLOBAL_MEMORY
	if(pDib)
	{
		delete[] pDib;
		pDib = NULL;
	}
#endif

	return TRUE;
}

template <class TYPE> BOOL rotate_image_arbitrarily(TYPE* pDestImage, UINT nDestWidth, UINT nDestHeight, UINT nDestBytesPerLine, TYPE* pSrcImage, UINT nSrcWidth, UINT nSrcHeight, UINT nSrcBytesPerLine, float fDestCenterX, float fDestCenterY, float fSrcCenterX, float fSrcCenterY, float fRadian, float fScale)
{
	if (!pDestImage)
	{
		return FALSE;
	}

	if (nDestWidth < 1)
	{
		return FALSE;
	}

	if (nDestHeight < 1)
	{
		return FALSE;
	}

	if (!pSrcImage)
	{
		return FALSE;
	}

	if (nSrcWidth < 1)
	{
		return FALSE;
	}

	if (nSrcHeight < 1)
	{
		return FALSE;
	}


	//
	float fColU = (float)sin(fRadian) * (1.0f / fScale);
	float fColV = (float)cos(fRadian) * (1.0f / fScale);
	
	//
	float fRowU = fColV;
	float fRowV = -fColU;
	//
	float fStartingU = fSrcCenterX - (fDestCenterX * fColV + fDestCenterY * fColU);
	float fStartingV = fSrcCenterY - (fDestCenterX * fRowV + fDestCenterY * fRowU);

	//
	float fNewRowU = fStartingU;
	float fNewRowV = fStartingV;

	for (UINT nHeight = 0; nHeight < nDestHeight; nHeight++)
	{
		float fU = fNewRowU;
		float fV = fNewRowV;

		TYPE* pDest = pDestImage + (nDestBytesPerLine * nHeight);

		for (UINT nWidth = 0; nWidth < nDestWidth; nWidth++)
		{   
			if (fU > 0 && fV > 0 && fU < nSrcWidth && fV < nSrcHeight)
			{   
				TYPE* pSrc = pSrcImage + (int)fU + ((int)fV * nSrcBytesPerLine);

				*pDest++ = *pSrc++;
			}
			else
			{
				*pDest++ = 0;
			}

			fU += fRowU;
			fV += fRowV;
		}

		fNewRowU += fColU;
		fNewRowV += fColV;
	}

	return TRUE;
}

#endif