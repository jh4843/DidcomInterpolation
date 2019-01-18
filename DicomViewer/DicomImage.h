#pragma once

class CDicomImage
{
public:
	typedef struct _IMAGE_INFO_
	{
		INT_PTR m_nWidth;				// Column
		INT_PTR m_nHeight;				// Row
		UINT m_nBitsPerPixel;
		UINT m_nBytesPerPixel;
		UINT m_nSamplesPerPixel;
		UINT m_nTotalAllocatedBytes;
		UINT m_nBytesPerLine;

		FLOAT m_fW1;
		FLOAT m_fW2;

		INT_PTR m_nW1;
		INT_PTR m_nW2;

		// Image Position
		FLOAT m_fImagePosition_X;
		FLOAT m_fImagePosition_Y;
		FLOAT m_fImagePosition_Z;

		// Image Orientation
		FLOAT m_fImageOrientationRowX;
		FLOAT m_fImageOrientationRowY;
		FLOAT m_fImageOrientationRowZ;
		FLOAT m_fImageOrientationColX;
		FLOAT m_fImageOrientationColY;
		FLOAT m_fImageOrientationColZ;
		FLOAT m_fImageOrientationOrthogonalX;		// Direct cosine of Orthogonal plane X-axis
		FLOAT m_fImageOrientationOrthogonalY;		// Direct cosine of Orthogonal plane Y-axis
		FLOAT m_fImageOrientationOrthogonalZ;		// Direct cosine of Orthogonal plane Z-axis

		// Pixel Information
		FLOAT m_fPixelXSpacing;
		FLOAT m_fPixelYSpacing;
		FLOAT m_fImagerXSpacing;
		FLOAT m_fImagerYSpacing;
		FLOAT m_fMagFactor;

		// Frame
		UINT m_nFramePerSecond;
		UINT m_nFrameIndex;

		void Init()
		{
			m_nWidth = 0;
			m_nHeight = 0;
			m_nBitsPerPixel = 0;
			m_nBytesPerPixel = 0;
			m_nSamplesPerPixel = 0;
			m_nTotalAllocatedBytes = 0;
			m_nBytesPerLine = 0;

			m_fW1 = 0;
			m_fW2 = 0;

			m_nW1 = 0;
			m_nW2 = 0;

			m_fImagePosition_X = 0.0;
			m_fImagePosition_Y = 0.0;
			m_fImagePosition_Z = 0.0;

			m_fImageOrientationRowX = 0.0;
			m_fImageOrientationRowY = 0.0;
			m_fImageOrientationRowZ = 0.0;
			m_fImageOrientationColX = 0.0;
			m_fImageOrientationColY = 0.0;
			m_fImageOrientationColZ = 0.0;
			m_fImageOrientationOrthogonalX = 0.0;
			m_fImageOrientationOrthogonalY = 0.0;
			m_fImageOrientationOrthogonalZ = 0.0;
			
			m_nFramePerSecond = 0;
			m_nFrameIndex = 0;
		}

		BOOL IsValid()
		{
			if (m_nWidth < 1)
			{
				return FALSE;
			}

			if (m_nHeight < 1)
			{
				return FALSE;
			}

			if (m_nBitsPerPixel < 1)
			{
				return FALSE;
			}

			if (m_nSamplesPerPixel != 3 && m_nSamplesPerPixel != 1)
			{
				return FALSE;
			}

			return TRUE;
		}

	}IMAGE_INFO;

	

public:
	CDicomImage();
	CDicomImage(const CDicomImage& dicomImage);
	~CDicomImage();

public:
	IMAGE_INFO m_stImageInfo;
	BYTE* m_pImageData;

protected:
	BOOL m_bTiledPixelData;

public:
	void Init();
	BOOL LoadDicomImage(pBITMAPHANDLE pBitmapHandle, pBITMAPHANDLE pOverlayBitmapHandle = nullptr);
	BOOL FreeDicomImage();

	BOOL CopyLead2UserBuf(LBitmap* pBitmap, void* pImage, int nWidth, int nHeight, int iAllocBytes);
	UINT BytesPerLine(UINT nPixelCount, UINT nBitPerPixel);
		
	UINT GetImageSize();
	void GetImageProcessedImage(BYTE* pOutImage, CDicomImage* pOutImageInfo);

	void SetTiledPiexlData(BOOL bSet);
	BOOL GetTiledPiexlData();

	void operator=(const CDicomImage& obj);
};

