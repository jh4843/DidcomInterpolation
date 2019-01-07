#pragma once
class COverLayImage
{
	typedef struct _OVERLAY_INFO_
	{
		INT_PTR m_nWidth;				// Column
		INT_PTR m_nHeight;				// Row
		UINT m_nBitsPerPixel;
		UINT m_nBytesPerPixel;
		UINT m_nSamplesPerPixel;
		UINT m_nTotalAllocatedBytes;
		UINT m_nBytesPerLine;
	}OVERLAY_INFO_;

public:
	COverLayImage();
	~COverLayImage();

public:
	INT_PTR m_nCurOverlayIndex;

};

