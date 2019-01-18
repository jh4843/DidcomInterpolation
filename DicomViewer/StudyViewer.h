#pragma once
#include "afxwin.h"
#include "Study.h"
#include "Instance.h"
#include "DicomImage.h"
#include "MyInterpolation.h"

#define IDC_IMGLISTCTRL 0

// Zoom Ratio
#define MIN_ZOOM_RATIO	0.1
#define MAX_ZOOM_RATIO	100.0

// Mode
#define MODE_NORMAL		0
#define MODE_PAN		1
#define MODE_PAN_DOWN	2

//
class CDicomImage;
struct DIBINFO : public BITMAPINFO
{
	RGBQUAD arColors[255];
	operator LPBITMAPINFO()
	{
		return (LPBITMAPINFO)this;
	}
	operator LPBITMAPINFOHEADER()
	{
		return &bmiHeader;
	}
	RGBQUAD* ColorTable()
	{
		return bmiColors;
	}
	void Init()
	{
		LPRGBQUAD pColorTable = ColorTable();
		for (UINT nColor = 0; nColor < 256; nColor++)
		{
			pColorTable[nColor].rgbReserved = 0;
			pColorTable[nColor].rgbBlue = nColor;
			pColorTable[nColor].rgbRed = nColor;
			pColorTable[nColor].rgbGreen = nColor;
		}
	}
};

class CStudyViewer :	public CWnd
{
	typedef struct _IMAGE_INFO_EX_
	{
		int nWidth;
		int nHeight;
		int nChannel;
		
		unsigned char* pImage;

		void Init()
		{
			nWidth = 0;
			nHeight = 0;
			nChannel = 1;

			pImage = nullptr;
		}

	}IMAGE_INFO_EX;

public:

	friend class CLayoutManager;

	CStudyViewer(INT_PTR nLayoutIndex);
	~CStudyViewer();

public:
	//HWND m_hLImgControl;

protected:
	CInstance* m_pDisplayedInstance;	// 가지고 있을 필요 없을거 같긴하다.
	
	INT_PTR m_nCurSeriesIndex;
	INT_PTR m_nCurInstanceIndex;
	INT_PTR m_nCurFrameIndex;

	CStudy* m_pStudy;
	CLLDicomDS* m_pDisplayDicomDS;

	DIBINFO m_DibInfo;
	INT_PTR m_nOperationMode;

	INTERPOLATION_TYPE m_eInterpolationType;

private:
	INT_PTR m_nLayoutIndex;
	CWnd* m_pParent;

	// for Debug
	IMAGE_INFO_EX m_stOriginDisplayImg;
	IMAGE_INFO_EX m_stROIImg;
	IMAGE_INFO_EX m_stInterpolatedImg;
			
	BYTE* m_pDisplayImage;				// image processed image
	BYTE* m_pDispRoiImage;				// Screen image
	BYTE* m_pRoiImage;					// image for ROI

	CRect m_rtImage;
	CRect m_rtCanvas;
	Gdiplus::RectF m_rtImageEx;
	CRect m_rtDrawRectOnCanvas;
	CRect m_rtDisplayedROIOnImage;
	CRect m_rtDisplayedROIOnCanvas;
	
	double m_dCanvasPerImageRatio;
	double m_dZoomValue;
	double m_dOldZoomValue;

	CPoint m_ptPanDelta;
	CPoint m_ptOldPointBeforePan;

	BOOL m_bOnlySeriesViewer;

public:
	CStudyViewer& operator=(const CStudyViewer& obj);
	
	// Set
	void SetStudy(CStudy* pStudy);
	void SetDisplayInstance();
	//
	void SetViewOnlySameSeries();
	void SetViewAllSeries();
	void SetCurrentInstanceIndex(INT_PTR nInstanceIndex);
	void SetInterpolationMode(INTERPOLATION_TYPE eMode);

	// Get
	INT_PTR GetLayoutIndex();
	CStudy* GetStudy();
	INT_PTR GetInstanceCount();
	INT_PTR GetSeriesCount();
	INT_PTR GetCurrentInstanceIndex();
	INT_PTR GetCurrentSeriesIndex();
	INT_PTR GetCurrentFrameIndex();
	CLLDicomDS* GetDisplayingDicomDS();
	void RedrawWnd();

	// Load
	void LoadImageFromDcm(CDicomImage* pDsImage);

		// Operate
	void ResetPan();
	void ResetZoom();

	void OperatePan(CPoint point);

	void ZoomIn(BOOL bIsDynamic, double dDelta = 1);
	void ZoomOut(BOOL bIsDynamic, double dDelta = 1);

	BOOL ProcessHotKey(UINT nKey);

protected:
	BOOL IsValidStudy(CStudy* pStudy);
	BOOL DrawInstanceImage(CDC* pDC);
	BOOL DrawImageInfo(CDC* pDC);
	BOOL DrawPatientOrientation(CDC* pDC);

	void ChangeInstanceIndex(BOOL bIsIncrease);
	void ChangeSeriesIndex(BOOL bIsIncrease);

	void ChangeInstanceImageByWheel(short zDelta);
	void ChangeSeriesImageByWheel(short zDelta);

	void SetOldMousePosBeforePan(CPoint ptOldPoint);

	void SetOperationModeByKey(BOOL bShift, BOOL bCtrl, BOOL bAlt, BOOL bLeftDown, BOOL bRightDown);

private:
	UINT BytesPerLine(UINT nPixelCount, UINT nBitPerPixel);

// Overriding
protected:
	virtual void Init(INT_PTR nCurSeriesIndex = 0, INT_PTR nCurInstanceIndex = 0, INT_PTR nCurFrameIndex = 0);
	virtual BOOL CreateCtrl(CWnd* pParent);
	//virtual L_INT CreateLEADImageList();
	virtual BOOL Draw(CDC* pDC);
	virtual DIBINFO* GetDibInfo();
	virtual BOOL CalcLayout();
	virtual INT_PTR CalcImageRect(CDicomImage* imageInfo);
	virtual BOOL CalcImageRectEx(CDicomImage* imageInfo);
	virtual BOOL CalcDisplayImageROI(CDicomImage * pImageInfo);
	virtual BOOL CalcDisplayCanvasROI(CDicomImage * pImageInfo);
	virtual BOOL CalcDrawRectOnCanvasRect(CDicomImage* imageInfo);
	virtual void CalcZoomAndPan();
	virtual BOOL AllocDisplayImage();
	virtual BOOL AllocDispRoiBuffer();
	virtual BOOL AllocRoiBuffer();
	virtual void FreeDisplayImage();
	virtual void FreeDispRoiBuffer();
	virtual void FreeRoiBuffer();
	//virtual void UpdateScreenData();
	virtual void CreateDragDropObjects();
	//virtual void CopyROIImageFromOrigin(BYTE* pSrc, BYTE* pDest, INT_PTR nSrcWidth, INT_PTR nSrcHeight, INT_PTR nROIWidth, INT_PTR nROIHeight);
	//virtual BOOL DoInterpolate(BYTE* pSrcImage, BYTE* pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

