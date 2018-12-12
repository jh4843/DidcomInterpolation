#include "stdafx.h"
#include "StudyViewer.h"
#include "CoordinatorUtill.h"
#include "DicomParser.h"
#include "LayoutManager.h"

CStudyViewer::CStudyViewer(INT_PTR nLayoutIndex)
{
	m_nLayoutIndex = nLayoutIndex;

	m_pDisplayDicomDS = nullptr;

	m_stOriginDisplayImg.Init();
	m_stROIImg.Init();
	m_stInterpolatedImg.Init();

	m_bOnlySeriesViewer = FALSE;

	m_nOperationMode = MODE_NORMAL;
}


CStudyViewer::~CStudyViewer()
{
}

CStudyViewer& CStudyViewer::operator=(const CStudyViewer& obj)
{
	m_pParent = obj.m_pParent;
	m_pStudy = obj.m_pStudy;

	m_nCurSeriesIndex = obj.m_nCurSeriesIndex;
	m_nCurInstanceIndex = obj.m_nCurInstanceIndex;
	m_pDisplayDicomDS = obj.m_pDisplayDicomDS;
	m_nCurFrameIndex = obj.m_nCurFrameIndex;

	return *this;
}

void CStudyViewer::SetStudy(CStudy* pStudy)
{
	if (!IsValidStudy(pStudy))
		return;

	m_pStudy = pStudy;

	SetDisplayInstance();

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}
}

void CStudyViewer::SetDisplayInstance()
{
	CArray<CSeries*, CSeries*>* pArySeries;
	CArray<CInstance*, CInstance*>* pAryInstance;
	CInstance* pDisplayedInstance;

	pArySeries = m_pStudy->GetSeriesArray();
	pAryInstance = pArySeries->GetAt(m_nCurSeriesIndex)->GetInstanceArray();

	pDisplayedInstance = pAryInstance->GetAt(m_nCurInstanceIndex);

	m_pDisplayDicomDS = pDisplayedInstance->GetDicomDS();

	CDicomImage imageDisplayInfo;	// different with DICOM information
	imageDisplayInfo = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex);

	imageDisplayInfo.m_stImageInfo.m_nBitsPerPixel = 8;
	imageDisplayInfo.m_stImageInfo.m_nBytesPerPixel = (UINT)Bits2Bytes(imageDisplayInfo.m_stImageInfo.m_nBitsPerPixel);
	imageDisplayInfo.m_stImageInfo.m_nTotalAllocatedBytes = (UINT)(imageDisplayInfo.m_stImageInfo.m_nBytesPerPixel*imageDisplayInfo.m_stImageInfo.m_nSamplesPerPixel);
	imageDisplayInfo.m_stImageInfo.m_nBytesPerLine = imageDisplayInfo.BytesPerLine((UINT)(imageDisplayInfo.m_stImageInfo.m_nWidth*imageDisplayInfo.m_stImageInfo.m_nSamplesPerPixel), 8);

	imageDisplayInfo.m_stImageInfo.m_fW1 = 0.0f;
	imageDisplayInfo.m_stImageInfo.m_fW2 = 0.0f;

	FreeDisplayImage();
	AllocDisplayImage();

	CDicomImage dsInputImage = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex);
	
	LoadImageFromDcm(dsInputImage);
	dsInputImage.GetImageProcessedImage(m_pDisplayImage, &imageDisplayInfo);
}

void CStudyViewer::SetViewOnlySameSeries()
{
	m_bOnlySeriesViewer = TRUE;
}

void CStudyViewer::SetViewAllSeries()
{
	m_bOnlySeriesViewer = FALSE;
}

void CStudyViewer::SetCurrentInstanceIndex(INT_PTR nInstanceIndex)
{
	m_nCurInstanceIndex = nInstanceIndex;
	SetDisplayInstance();

}

void CStudyViewer::SetLocalizerPoints(Gdiplus::PointF ptLocalizerStart, Gdiplus::PointF ptLocalizerEnd)
{
	m_ptLocalizerStart = ptLocalizerStart;
	m_ptLocalizerEnd = ptLocalizerEnd;
}

INT_PTR CStudyViewer::GetLayoutIndex()
{
	return m_nLayoutIndex;
}

CStudy* CStudyViewer::GetStudy()
{
	return m_pStudy;
}

INT_PTR CStudyViewer::GetCurrentInstanceIndex()
{
	return m_nCurInstanceIndex;
}

INT_PTR CStudyViewer::GetCurrentSeriesIndex()
{
	return m_nCurSeriesIndex;
}

INT_PTR CStudyViewer::GetCurrentFrameIndex()
{
	return m_nCurFrameIndex;
}

CLLDicomDS* CStudyViewer::GetDisplayingDicomDS()
{
	return m_pDisplayDicomDS;
}

void CStudyViewer::RedrawWnd()
{
	Invalidate(FALSE);
	UpdateWindow();
}

void CStudyViewer::LoadImageFromDcm(CDicomImage& dsImage)
{
	CDicomParser* pDicomParser = new CDicomParser;

	pDicomParser->LoadDS((LPTSTR)(LPCTSTR)m_pDisplayDicomDS->m_dcmHeaderInfo.m_strFileName, 0);
	pDicomParser->SetDcmHeaderInfo(m_pDisplayDicomDS->m_dcmHeaderInfo);
	pDicomParser->AddDcmImageInfo(m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));

	BITMAPHANDLE OrgImageBitmapHandle;
	

	pDICOMELEMENT pElement;
	dsImage = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex);

	UINT nFlags = DICOM_GETIMAGE_AUTO_APPLY_MODALITY_LUT | DICOM_GETIMAGE_AUTO_APPLY_VOI_LUT | DICOM_GETIMAGE_AUTO_LOAD_OVERLAYS;

	pElement = pDicomParser->FindLastElement(NULL, TAG_PIXEL_DATA, FALSE);
	L_UINT16 nResult = pDicomParser->GetImage(pElement,
		&OrgImageBitmapHandle,
		sizeof(BITMAPHANDLE),
		m_nCurFrameIndex,
		0,
		ORDER_RGBORGRAY,
		nFlags,
		NULL,
		NULL);


	if (nResult == DICOM_ERROR_MEMORY)
	{
		AfxThrowMemoryException();
	}
	else if (nResult != DICOM_SUCCESS)
	{
		Sleep(0);
		nResult = pDicomParser->GetImage(pElement,
			&OrgImageBitmapHandle,
			sizeof(BITMAPHANDLE),
			m_nCurFrameIndex,
			0,
			ORDER_RGBORGRAY,
			nFlags,
			NULL,
			NULL);

		if (nResult != DICOM_SUCCESS)
		{
			Sleep(0);
			nResult = pDicomParser->GetImage(pElement,
				&OrgImageBitmapHandle,
				sizeof(BITMAPHANDLE),
				m_nCurFrameIndex,
				0,
				ORDER_RGBORGRAY,
				nFlags,
				NULL,
				NULL);
		}

		if (nResult != DICOM_SUCCESS)
		{
			AfxThrowUserException();
		}

		if (OrgImageBitmapHandle.Flags.Allocated != 1)
		{
			AfxThrowUserException();
		}

		if (OrgImageBitmapHandle.Flags.Compressed == 1)
		{
			AfxThrowUserException();
		}

		// Not linear image data.
		if (OrgImageBitmapHandle.Flags.Tiled == 1)
		{
			dsImage.SetTiledPiexlData(TRUE);
		}

		if (OrgImageBitmapHandle.Flags.SuperCompressed == 1)
		{
			AfxThrowUserException();
		}

		if (OrgImageBitmapHandle.Flags.UseLUT == 1)
		{
			if (!OrgImageBitmapHandle.pLUT)
			{
				AfxThrowUserException();
			}

			if (OrgImageBitmapHandle.LUTLength < 1)
			{
				AfxThrowUserException();
			}
		}

		if (!OrgImageBitmapHandle.Addr.Windows.pData)
		{
			AfxThrowUserException();
		}

		if (OrgImageBitmapHandle.Width < 2)
		{
			AfxThrowUserException();
		}

		if (OrgImageBitmapHandle.Height < 2)
		{
			AfxThrowUserException();
		}

		if (OrgImageBitmapHandle.BitsPerPixel < 1)
		{
			AfxThrowUserException();
		}

		if (OrgImageBitmapHandle.BytesPerLine < 4)
		{
			AfxThrowUserException();
		}

		dsImage.FreeDicomImage();
		dsImage.m_stImageInfo.Init();

		if (OrgImageBitmapHandle.HighBit < 1)
		{
			AfxThrowUserException();
		}
		else
		{
			dsImage.m_stImageInfo.m_nBitsPerPixel = (UINT)(OrgImageBitmapHandle.HighBit + 1);
		}

		// RGB : SamplesPerPixel = 3
		if (OrgImageBitmapHandle.BitsPerPixel == 24)
		{
			dsImage.m_stImageInfo.m_nSamplesPerPixel = 3;
		}
		// Gray : SamplesPerPixel = 1 (default)
		else if (OrgImageBitmapHandle.BitsPerPixel > 0 && OrgImageBitmapHandle.BitsPerPixel < 24)
		{
			dsImage.m_stImageInfo.m_nSamplesPerPixel = 1;
		}
		else
		{
			AfxThrowUserException();
		}
	}

	dsImage.m_stImageInfo.m_nBytesPerPixel = (UINT)Bits2Bytes(dsImage.m_stImageInfo.m_nBitsPerPixel);
	dsImage.m_stImageInfo.m_nTotalAllocatedBytes = (UINT)(dsImage.m_stImageInfo.m_nBytesPerPixel*dsImage.m_stImageInfo.m_nSamplesPerPixel);
	dsImage.m_stImageInfo.m_nWidth = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth;
	dsImage.m_stImageInfo.m_nHeight = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
	int nBytesPerLineX = ((dsImage.m_stImageInfo.m_nWidth * dsImage.m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
	int nBytesPerLineY = ((dsImage.m_stImageInfo.m_nHeight * dsImage.m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
	dsImage.m_stImageInfo.m_nWidth = nBytesPerLineX / dsImage.m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
	dsImage.m_stImageInfo.m_nHeight = nBytesPerLineY / dsImage.m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
	dsImage.m_stImageInfo.m_nBytesPerLine = dsImage.m_stImageInfo.m_nWidth * dsImage.m_stImageInfo.m_nTotalAllocatedBytes;

	dsImage.m_stImageInfo.m_fW1 = 0.0f;
	dsImage.m_stImageInfo.m_fW2 = Bits2MaxValue(dsImage.m_stImageInfo.m_nBitsPerPixel);
	//
	// Get W/L
	L_DOUBLE dWindowCenter = 0;
	L_DOUBLE dWindowWidth = 0;
	// Window Center
	pElement = pDicomParser->FindFirstElement(NULL, TAG_WINDOW_CENTER, FALSE);
	if (pElement && pElement->nLength)
	{
		dWindowCenter = *(pDicomParser->GetDoubleValue(pElement, 0, 1));
		dsImage.m_stImageInfo.m_nW1 = (int)dWindowCenter;
	}
	else
	{
		dsImage.m_stImageInfo.m_nW1 = (int)dsImage.m_stImageInfo.m_fW1;
	}

	// Window Width
	pElement = pDicomParser->FindFirstElement(NULL, TAG_WINDOW_WIDTH, FALSE);
	if (pElement && pElement->nLength)
	{
		dWindowWidth = *(pDicomParser->GetDoubleValue(pElement, 0, 1));
		dsImage.m_stImageInfo.m_nW2 = (int)dWindowWidth;
	}
	else
	{
		dsImage.m_stImageInfo.m_nW2 = (int)dsImage.m_stImageInfo.m_fW2;
	}
	//
	BITMAPHANDLE OverlayBitmapHandle;
	if (pDicomParser->GetOverlayImageFromOrigin(&OrgImageBitmapHandle, &OverlayBitmapHandle) > 0)
	{
		dsImage.LoadDicomImage(&OrgImageBitmapHandle, &OverlayBitmapHandle);
	}
	else
	{
		dsImage.LoadDicomImage(&OrgImageBitmapHandle);
	}

	
	L_FreeBitmap(&OrgImageBitmapHandle);

	pDicomParser->ResetDS();
}

void CStudyViewer::ClearLocalizerPoints()
{
	m_ptLocalizerStart = Gdiplus::PointF(0.0, 0.0);
	m_ptLocalizerEnd = Gdiplus::PointF(0.0, 0.0);

	return;
}

void CStudyViewer::ResetPan()
{
	m_ptOldPointBeforePan = CPoint(0, 0);
	m_ptPanDelta = CPoint(0, 0);
}

void CStudyViewer::ResetZoom()
{
	m_dZoomValue = 1.0;
}

void CStudyViewer::OperatePan(CPoint point)
{
	if (!m_pDisplayDicomDS)
		return;

	if (!m_pDisplayImage)
		return;

	CPoint ptDelta;
	ptDelta = point - m_ptOldPointBeforePan;

	ptDelta.x = (int)((double)ptDelta.x / m_dCanvasPerImageRatio*m_dZoomValue + 0.5);
	ptDelta.y = (int)((double)ptDelta.y / m_dCanvasPerImageRatio*m_dZoomValue + 0.5);
	m_ptPanDelta += ptDelta;

	SetOldMousePosBeforePan(point);

	CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	RedrawWnd();
}

void CStudyViewer::ZoomIn(BOOL bIsDetail)
{
	if (!m_pDisplayDicomDS)
		return;

	if (!m_pDisplayImage)
		return;

	double dRatio = 0.01;

	if (bIsDetail)
		m_dZoomValue += dRatio * 5.0;	// 5%
	else
		m_dZoomValue += dRatio * 100.0;	// 100%

	if (m_dZoomValue < MIN_ZOOM_RATIO)
	{
		m_dZoomValue = MIN_ZOOM_RATIO;
	}
	else if (m_dZoomValue > MAX_ZOOM_RATIO)
	{
		m_dZoomValue = MAX_ZOOM_RATIO;
	}

	CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	RedrawWnd();
}

void CStudyViewer::ZoomOut(BOOL bIsDetail)
{
	if (!m_pDisplayDicomDS)
		return;

	if (!m_pDisplayImage)
		return;

	double dRatio = 0.01;

	if (bIsDetail)
		m_dZoomValue -= dRatio * 5.0;	// 5%
	else
		m_dZoomValue -= dRatio * 100.0;	// 30%

	if (m_dZoomValue < MIN_ZOOM_RATIO)
	{
		m_dZoomValue = MIN_ZOOM_RATIO;
	}
	else if (m_dZoomValue > MAX_ZOOM_RATIO)
	{
		m_dZoomValue = MAX_ZOOM_RATIO;
	}

	CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	RedrawWnd();
}

INT_PTR CStudyViewer::GetInstanceCount()
{
	CArray<CSeries*, CSeries*>* pArySeries;
	CArray<CInstance*, CInstance*>* pAryInstance;

	pArySeries = m_pStudy->GetSeriesArray();

	pAryInstance = pArySeries->GetAt(m_nCurSeriesIndex)->GetInstanceArray();

	return pAryInstance->GetCount();
}

INT_PTR CStudyViewer::GetSeriesCount()
{
	CArray<CSeries*, CSeries*>* pArySeries;

	pArySeries = m_pStudy->GetSeriesArray();

	return pArySeries->GetCount();
}

void CStudyViewer::Init(INT_PTR nCurSeriesIndex, INT_PTR nCurInstanceIndex, INT_PTR nCurFrameIndex)
{
	m_pStudy = nullptr;
	m_pDisplayImage = nullptr;
	m_pScreenImage = nullptr;
	m_pDisplayDicomDS = nullptr;
	
	FreeDisplayImage();
	FreeScreenBuffer();
	
	m_nCurSeriesIndex = nCurSeriesIndex;
	m_nCurInstanceIndex = nCurInstanceIndex;
	m_nCurFrameIndex = nCurFrameIndex;

	m_DibInfo.Init();

	m_rtCanvas = CRect(0, 0, 0, 0);
	m_rtImage = CRect(0, 0, 0, 0);
	m_rtImageOnCanvas = CRect(0, 0, 0, 0);

	m_dCanvasPerImageRatio = 1.0;
	m_dZoomValue = 1.0;

	m_ptPanDelta = CPoint(0, 0);
	m_ptOldPointBeforePan = CPoint(0, 0);

	ClearLocalizerPoints();

	/*
	if (m_hLImgControl == nullptr)
	{
		if (CreateLEADImageList() != SUCCESS)
		{
			MessageBox(TEXT("Error creating image list control"), TEXT("Error"), MB_OK);
			return;
		}
	}
	*/

	RedrawWnd();
}

BOOL CStudyViewer::CreateCtrl(CWnd* pParent)
{
	if (!CWnd::Create(NULL, _T("UIStudyViewer"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pParent, ID_LAYOUT_VIEWER))
	{
		return FALSE;
	}

	m_pParent = pParent;

	Init();

	CreateDragDropObjects();

	return TRUE;
}
/*
L_INT CStudyViewer::CreateLEADImageList()
{
	L_UINT32       uFlags;
	LILSELOPTION   Opt;
	LILITEMOPTION  MOpt;

	GetClientRect(&m_rtCanvas);

	m_hLImgControl = L_CreateImageListControl(WS_CHILD | WS_VISIBLE | WS_BORDER,
		0,
		0,
		m_rtCanvas.Width(),			// 
		m_rtCanvas.Height(),
		this->GetSafeHwnd(),
		IDC_IMGLISTCTRL,
		RGB(128, 128, 128));

	if (!IsWindow(m_hLImgControl))
	{
		return 0;
	}

	uFlags = L_GetDisplayMode();
	uFlags |= DISPLAYMODE_RESAMPLE | DISPLAYMODE_SCALETOGRAY;
	L_ImgListSetDisplayFlags(m_hLImgControl, uFlags);

	// Enable / Disable selection style & method
	L_ImgListGetSelOptions(m_hLImgControl, &Opt);
	Opt.uStructSize = sizeof(LILSELOPTION);
	Opt.uAllowSelection = ALLOWSELECTION_SINGLE;
	Opt.uSelectionStyle = SELECTIONSTYLE_PRESSEDBUTTON;
	L_ImgListSetSelOptions(m_hLImgControl, &Opt);

	// Enable / Disable display text, and change the item size
	memset(&MOpt, 0, sizeof(MOpt));
	MOpt.uStructSize = sizeof(LILITEMOPTION);
	L_ImgListGetItemOptions(m_hLImgControl, &MOpt);
	MOpt.bDisplayItemText = TRUE;
	MOpt.uWidth = 145;
	MOpt.uHeight = 145;
	L_ImgListSetItemOptions(m_hLImgControl, &MOpt);

	// Enable / Disable keyboard
	L_ImgListEnableKeyboard(m_hLImgControl, TRUE);

	// Set scroll style Vert / Horz
	L_ImgListSetScrollStyle(m_hLImgControl, SCROLLSTYLE_VERTICAL);

	::SetFocus(m_hLImgControl);
	return 1;
}
*/

BOOL CStudyViewer::IsValidStudy(CStudy* pStudy)
{
	CArray<CSeries*, CSeries*>* pArrSeries = pStudy->GetSeriesArray();

	if (pArrSeries->GetCount() <= 0)
		return FALSE;

	CArray<CInstance*, CInstance*>* pArrInstance = pArrSeries->GetAt(0)->GetInstanceArray();

	if (pArrInstance->GetCount() <= 0)
		return FALSE;

	return TRUE;
}

BOOL CStudyViewer::DrawInstanceImage(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	BITMAPINFOHEADER& bih = GetDibInfo()->bmiHeader;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = m_stInterpolatedImg.nWidth;
	bih.biHeight = -m_stInterpolatedImg.nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 8 * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = m_stInterpolatedImg.nWidth * m_stInterpolatedImg.nHeight;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	UpdateScreenData();

	::SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
	::StretchDIBits(pDC->GetSafeHdc(),
		m_rtCanvas.left,
		m_rtCanvas.top,
		m_rtCanvas.Width(),
		m_rtCanvas.Height(),
		m_rtImageOnCanvas.left,
		m_rtImageOnCanvas.top,
		m_rtImageOnCanvas.Width(),
		m_rtImageOnCanvas.Height(),
		(void*)m_pScreenImage,
		(BITMAPINFO*)GetDibInfo(),
		DIB_RGB_COLORS,
		SRCCOPY);


	//
// 	BITMAPINFOHEADER& bih = GetDibInfo()->bmiHeader;
// 	bih.biSize = sizeof(BITMAPINFOHEADER);
// 	bih.biWidth = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth;
// 	bih.biHeight = -m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
// 	bih.biPlanes = 1;
// 	bih.biBitCount = 8 * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel;
// 	bih.biCompression = BI_RGB;
// 	bih.biSizeImage = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).BytesPerLine((UINT)((double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth*(double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel), 8) * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
// 	bih.biXPelsPerMeter = 0;
// 	bih.biYPelsPerMeter = 0;
// 	bih.biClrUsed = 0;
// 	bih.biClrImportant = 0;
// 
// 	::SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
// 	::StretchDIBits(pDC->GetSafeHdc(),
// 		m_rtCanvas.left,
// 		m_rtCanvas.top,
// 		m_rtCanvas.Width(),
// 		m_rtCanvas.Height(),
// 		m_rtImage.left,
// 		m_rtImage.top,
// 		m_rtImage.Width(),
// 		m_rtImage.Height(),
// 		(void*)m_pDisplayImage,
// 		(BITMAPINFO*)GetDibInfo(),
// 		DIB_RGB_COLORS,
// 		SRCCOPY);

//	

// 	BITMAPINFOHEADER& bih = GetDibInfo()->bmiHeader;
// 	bih.biSize = sizeof(BITMAPINFOHEADER);
// 	bih.biWidth = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth;
// 	bih.biHeight = -m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
// 	bih.biPlanes = 1;
// 	bih.biBitCount = 8 * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel;
// 	bih.biCompression = BI_RGB;
// 	bih.biSizeImage = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).BytesPerLine((UINT)((double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth*(double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel), 8) * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
// 	bih.biXPelsPerMeter = 0;
// 	bih.biYPelsPerMeter = 0;
// 	bih.biClrUsed = 0;
// 	bih.biClrImportant = 0;
// 
// 	Graphics g(pDC->GetSafeHdc());
// 
// 	// 나중에 시간날떄 한번씩 써보자. enum InterpolationMode
// 	g.SetInterpolationMode(InterpolationModeBilinear);
// 
// 	Gdiplus::Bitmap bitmap((BITMAPINFO*)GetDibInfo(), m_pDisplayImage);
// 
// 	m_rtImage.OffsetRect(0, 0);
// 
// 	INT nSrcWidth = m_rtImage.Width();
// 	INT nSrcHeight = m_rtImage.Height();
// 
// 	//
// 	g.DrawImage(&bitmap,
// 		Rect(m_rtCanvas.left, m_rtCanvas.top, m_rtCanvas.Width(), m_rtCanvas.Height()),
// 		m_rtImage.left,
// 		m_rtImage.top,
// 		nSrcWidth,
// 		nSrcHeight,
// 		UnitPixel);

	//

// 	::StretchDIBits(pDC->GetSafeHdc(),
// 		m_rtCanvas.left,
// 		m_rtCanvas.top,
// 		m_rtCanvas.Width(),
// 		m_rtCanvas.Height(),
// 		m_rtImage.left,
// 		m_rtImage.top,
// 		m_rtImage.Width(),
// 		m_rtImage.Height(),
// 		(void*)m_pDisplayImage,
// 		(BITMAPINFO*)GetDibInfo(),
// 		DIB_RGB_COLORS,
// 		SRCCOPY);


	
	return TRUE;
}

BOOL CStudyViewer::DrawLocalizer(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	if (m_ptLocalizerStart.X == m_ptLocalizerEnd.X &&
		m_ptLocalizerStart.Y == m_ptLocalizerEnd.Y)
	{
		return FALSE;
	}

	CCoordinatorUtill util;

	Gdiplus::Graphics graphics(pDC->m_hDC);
	FLOAT fLineThick = 2; 
	Gdiplus::Pen penLine(Color(120, 240, 155), fLineThick);

	Gdiplus::PointF ptStart;
	Gdiplus::PointF ptEnd;

	INT_PTR iEnd = 0;

	ptStart = util.ConvertCanvas(m_ptLocalizerStart, m_rtCanvas, m_rtImage);
	ptEnd = util.ConvertCanvas(m_ptLocalizerEnd, m_rtCanvas, m_rtImage);

	graphics.DrawLine(&penLine, ptStart, ptEnd);

}

BOOL CStudyViewer::DrawImageInfo(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	INT_PTR nMargin = 3;
	INT_PTR nSpace = 1;
	COLORREF crTextColor = RGB(255, 255, 255);
	COLORREF crShadowColor = RGB(0, 0, 0);

	INT_PTR nFontSize = 85;
	CFont font;
	font.CreatePointFont(nFontSize, L"Segoe UI");

	CFont* pOldFont = pDC->SelectObject(&font);

	INT_PTR nPosX = m_rtCanvas.left + nMargin;
	INT_PTR nPosY = m_rtCanvas.top + nMargin;

	CStringArray aryImageInfo;

	CString strStudyID = m_pDisplayDicomDS->GetStudyID();
	aryImageInfo.Add(strStudyID);
	CString strSeriesID = m_pDisplayDicomDS->GetSeriesID();
	aryImageInfo.Add(strSeriesID);
	CString strInstanceID = m_pDisplayDicomDS->GetInstanceID();
	aryImageInfo.Add(strInstanceID);

	CString strSeriesNumber;
	strSeriesNumber.Format(_T("Series : %d"), m_pDisplayDicomDS->GetSeriesNumber());
	aryImageInfo.Add(strSeriesNumber);
	CString strInstanceNumber;
	strInstanceNumber.Format(_T("Instance : %d"), m_pDisplayDicomDS->GetInstanceNumber());
	aryImageInfo.Add(strInstanceNumber);

	for (INT_PTR nIndex = 0; nIndex < aryImageInfo.GetCount(); nIndex++)
	{
		pDC->SetTextColor(crTextColor);
		pDC->TextOut(nPosX, nPosY, aryImageInfo.GetAt(nIndex));

		nPosY += 10 + nSpace;
	}

	pDC->SelectObject(pOldFont);
}

BOOL CStudyViewer::DrawPatientOrientation(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	INT_PTR nMargin = 5;
	COLORREF crTextColor = RGB(255, 255, 255);
	COLORREF crShadowColor = RGB(0, 0, 0);

	INT_PTR nFontSize = 2;
	CFont font;
	font.CreatePointFont(nFontSize, L"Segoe UI");

	CFont* pOldFont = pDC->SelectObject(&font);

	CString strPatientOrientationUp = m_pDisplayDicomDS->GetPatientOrientationUp(m_nCurFrameIndex);
	CString strPatientOrientationDown = m_pDisplayDicomDS->GetPatientOrientationDown(m_nCurFrameIndex);
	CString strPatientOrientationLeft = m_pDisplayDicomDS->GetPatientOrientationLeft(m_nCurFrameIndex);
	CString strPatientOrientationRight = m_pDisplayDicomDS->GetPatientOrientationRight(m_nCurFrameIndex);

	CSize sizeMax = pDC->GetTextExtent(strPatientOrientationRight);

	CSize szText = CSize(5, 5);
	INT_PTR nLeftStartPosX = m_rtCanvas.left + nMargin;
	INT_PTR nRightStartPosX = m_rtCanvas.right - sizeMax.cx;
	INT_PTR nMiddleStartPosX = (INT_PTR)(m_rtCanvas.Width()/2) - (INT_PTR)(sizeMax.cx/2);

	INT_PTR nTopStartPosY = m_rtCanvas.top + nMargin;
	INT_PTR nBottomStartPosY = m_rtCanvas.bottom - sizeMax.cy;
	INT_PTR nMiddleStartPosY = (INT_PTR)(m_rtCanvas.Height() / 2) - (INT_PTR)(sizeMax.cy / 2);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nMiddleStartPosX, nTopStartPosY, strPatientOrientationUp);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nMiddleStartPosX, nBottomStartPosY, strPatientOrientationDown);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nLeftStartPosX, nMiddleStartPosY, strPatientOrientationLeft);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nRightStartPosX, nMiddleStartPosY, strPatientOrientationRight);

	pDC->SelectObject(pOldFont);

	return TRUE;

}

void CStudyViewer::ChangeInstanceIndex(BOOL bIsIncrease)
{
	INT_PTR nLastIndex = GetInstanceCount() - 1;

	if (bIsIncrease == TRUE)
	{
		if (m_nCurInstanceIndex >= nLastIndex)
		{
			if (m_bOnlySeriesViewer == FALSE)
			{
				ChangeSeriesIndex(TRUE);
			}
			else
			{
				m_nCurInstanceIndex = 0;
			}
		}
		else
		{
			m_nCurInstanceIndex++;
		}
	}
	else
	{
		if (m_nCurInstanceIndex <= 0)
		{
			if (m_bOnlySeriesViewer == FALSE)
			{
				ChangeSeriesIndex(FALSE);
			}
			else
			{
				m_nCurInstanceIndex = nLastIndex;
			}
		}
		else
		{
			m_nCurInstanceIndex--;
		}
	}
}

void CStudyViewer::ChangeSeriesIndex(BOOL bIsIncrease)
{
	INT_PTR nLastIndex = GetSeriesCount() - 1;
	INT_PTR nLastInstanceIndex = m_pStudy->GetSeriesArray()->GetAt(nLastIndex)->GetInstanceArray()->GetCount() - 1;
	INT_PTR nLastFrameIndex = m_pStudy->GetSeriesArray()->GetAt(nLastIndex)->GetInstanceArray()->GetAt(nLastInstanceIndex)->GetDicomDS()->m_aryDicomImage.GetCount() - 1;

	if (bIsIncrease == TRUE)
	{
		if (m_nCurSeriesIndex >= nLastIndex)
		{
			m_nCurSeriesIndex = 0;
			m_nCurInstanceIndex = 0;
			m_nCurFrameIndex = 0;
		}
		else
		{
			m_nCurSeriesIndex++;
			m_nCurInstanceIndex = 0;
			m_nCurFrameIndex = 0;
		}
	}
	else
	{
		if (m_nCurSeriesIndex <= 0)
		{
			m_nCurSeriesIndex = nLastIndex;
			m_nCurInstanceIndex = nLastInstanceIndex;
			m_nCurFrameIndex = nLastFrameIndex;
		}
		else
		{
			m_nCurSeriesIndex--;
			m_nCurInstanceIndex = 0;
			m_nCurFrameIndex = 0;
		}
	}
}

void CStudyViewer::ChangeInstanceImageByWheel(short zDelta)
{
	if (!m_pDisplayDicomDS)
		return;

	if (m_pDisplayDicomDS->m_aryDicomImage.GetCount() <= 0)
		return;

	INT_PTR nLastFrameIndex = m_pDisplayDicomDS->m_aryDicomImage.GetCount() - 1;

	if (zDelta > 0)
	{
		if (m_nCurFrameIndex  == nLastFrameIndex || nLastFrameIndex == 0)
		{
			ChangeInstanceIndex(FALSE);
			m_nCurFrameIndex = 0;
		}
		else
		{
			m_nCurFrameIndex--;
			if (m_nCurFrameIndex < 0)
			{
				m_nCurFrameIndex = nLastFrameIndex;
			}
		}
	}
	else
	{
		if (m_nCurFrameIndex == nLastFrameIndex || nLastFrameIndex == 0)
		{
			ChangeInstanceIndex(TRUE);
			m_nCurFrameIndex = 0;
		}
		else
		{
			m_nCurFrameIndex++;
			if (m_nCurFrameIndex > nLastFrameIndex)
			{
				m_nCurFrameIndex = 0;
			}
		}
	}

	SetDisplayInstance();

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}

	RedrawWnd();

	return;
}

void CStudyViewer::ChangeSeriesImageByWheel(short zDelta)
{
	INT_PTR nLastIndex = GetSeriesCount() - 1;

	if (zDelta > 0)
	{
		ChangeSeriesIndex(FALSE);
	}
	else
	{
		ChangeSeriesIndex(TRUE);
	}

	SetDisplayInstance();

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}

	RedrawWnd();

	return;
}

void CStudyViewer::SetOldMousePosBeforePan(CPoint ptOldPoint)
{
	m_ptOldPointBeforePan = ptOldPoint;
}

void CStudyViewer::SetOperationModeByKey(BOOL bShift, BOOL bCtrl, BOOL bAlt, BOOL bLeftDown, BOOL bRightDown)
{
	switch (m_nOperationMode)
	{
	case MODE_NORMAL:
		if (bCtrl)
		{
			m_nOperationMode = MODE_PAN;
		}
		break;
	case MODE_PAN:
		if (bLeftDown)
		{
			m_nOperationMode = MODE_PAN_DOWN;
		}
		break;
	case MODE_PAN_DOWN:
		if (bCtrl && bLeftDown)
		{
			m_nOperationMode = MODE_PAN_DOWN;
		}
		else
		{
			m_nOperationMode = MODE_NORMAL;
		}
		break;
	default:
		m_nOperationMode = MODE_NORMAL;
		break;
	}
}

UINT CStudyViewer::BytesPerLine(UINT nPixelCount, UINT nBitPerPixel)
{
	return ((nPixelCount * nBitPerPixel + 31) & (~31)) / 8;
}



BOOL CStudyViewer::Draw(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	CMemDC memDC(*pDC, this);
	pDC = &memDC.GetDC();

	CRect rtClient;
	GetClientRect(&rtClient);

	pDC->FillSolidRect(&rtClient, RGB(100, 100, 100));

	DrawInstanceImage(pDC);

	DrawPatientOrientation(pDC);

	DrawLocalizer(pDC);

	DrawImageInfo(pDC);

	//pDC->SetBkMode(TRANSPARENT);
	
	return TRUE;
}


BOOL CStudyViewer::CalcLayout()
{
	BOOL bRes = TRUE;

	CRect rtOldCanvas = m_rtCanvas;

	GetClientRect(&m_rtCanvas);

	if (m_pDisplayDicomDS)
	{
		CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));

		if (!m_rtCanvas.EqualRect(rtOldCanvas))
		{
			FreeScreenBuffer();
			if (CalcImageOnCanvasRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex)));
			{
				AllocScreenBuffer();
			}
		}
	}

	return bRes;
}

INT_PTR CStudyViewer::CalcImageRect(CDicomImage* pImageInfo)
{
	INT_PTR nRetCode = 1;

	double dImgWidth = 0., dImgHeight = 0.;
	double dCanvasWidth = 0., dCanvasHeight = 0.;
	double dRatio = 0.;
	double dExtendedLength = 0.;

	//
	dImgWidth = pImageInfo->m_stImageInfo.m_nWidth;
	dImgHeight = pImageInfo->m_stImageInfo.m_nHeight;
	dCanvasWidth = (double)m_rtCanvas.Width();
	dCanvasHeight = (double)m_rtCanvas.Height();

	if (dCanvasWidth >= dCanvasHeight)
	{
		dRatio = dCanvasWidth / dCanvasHeight;
		dExtendedLength = dImgHeight * dRatio;

		if (dExtendedLength >= dImgWidth)
		{
			m_rtImage.left = (int)(dImgWidth / 2) - (int)(dExtendedLength / 2);
			m_rtImage.top = 0;
			m_rtImage.right = m_rtImage.left + (int)(dExtendedLength + 0.5f);
			m_rtImage.bottom = (int)dImgHeight;

			CalcZoomAndPan();
			m_dCanvasPerImageRatio = dCanvasHeight / (float)m_rtImage.Height();
		}
		else
		{
			dRatio = dCanvasHeight / dCanvasWidth;
			dExtendedLength = dImgWidth * dRatio;

			m_rtImage.left = 0;
			m_rtImage.top = (int)(dImgHeight / 2) - (int)(dExtendedLength / 2);
			m_rtImage.right = (int)dImgWidth;
			m_rtImage.bottom = m_rtImage.top + (int)(dExtendedLength + 0.5f);

			CalcZoomAndPan();
			m_dCanvasPerImageRatio = dCanvasWidth / (float)m_rtImage.Width();
		}
	}
	else
	{
		dRatio = dCanvasHeight / dCanvasWidth;
		dExtendedLength = dImgWidth * dRatio;

		if (dExtendedLength >= dImgHeight)
		{
			m_rtImage.left = 0;
			m_rtImage.top = (int)(dImgHeight / 2) - (int)(dExtendedLength / 2);
			m_rtImage.right = (int)dImgWidth;
			m_rtImage.bottom = m_rtImage.top + (int)(dExtendedLength + 0.5f);

			CalcZoomAndPan();
			m_dCanvasPerImageRatio = dCanvasWidth / (float)m_rtImage.Width();
		}
		else
		{
			dRatio = dCanvasWidth / dCanvasHeight;
			dExtendedLength = dImgHeight * dRatio;

			m_rtImage.left = (int)(dImgWidth / 2) - (int)(dExtendedLength / 2);
			m_rtImage.top = 0;
			m_rtImage.right = m_rtImage.left + (int)(dExtendedLength + 0.5f);
			m_rtImage.bottom = (int)dImgHeight;

			CalcZoomAndPan();
			m_dCanvasPerImageRatio = dCanvasHeight / (float)m_rtImage.Height();
		}
	}

	return nRetCode;
}

BOOL CStudyViewer::CalcImageOnCanvasRect(CDicomImage * pImageInfo)
{
	if (!pImageInfo)
		return FALSE;

	double dImgWidth = 0., dImgHeight = 0.;
	double dCanvasWidth = 0., dCanvasHeight = 0.;
	double dCanvasRatio = 0.;
	double dImgRatio = 0.;
	double dExpectLength = 0.;

	INT_PTR nWidth = 0;
	INT_PTR nHeight = 0;

	//
	dImgWidth = pImageInfo->m_stImageInfo.m_nWidth;
	dImgHeight = pImageInfo->m_stImageInfo.m_nHeight;
	dCanvasWidth = (double)m_rtCanvas.Width();
	dCanvasHeight = (double)m_rtCanvas.Height();

	if (dCanvasWidth >= dCanvasHeight)
	{
		dImgRatio = dImgWidth / dImgHeight;
		dExpectLength = dCanvasHeight * dImgRatio;

		if (dExpectLength >= dCanvasWidth)
		{
			nWidth = dCanvasWidth;
			nHeight = dCanvasWidth / dImgRatio;
		}
		else
		{
			nWidth = dExpectLength;
			nHeight = dCanvasHeight;
		}
	}
	else
	{
		dImgRatio = dImgHeight / dImgWidth;
		dExpectLength = dCanvasWidth * dImgRatio;

		if (dExpectLength >= dCanvasHeight)
		{
			nWidth = dExpectLength;
			nHeight = dCanvasHeight;
		}
		else
		{
			nWidth = dCanvasWidth;
			nHeight = dCanvasWidth * dImgRatio;
		}
	}

	nWidth = BytesPerLine(nWidth, 8);
	nHeight = BytesPerLine(nHeight, 8);

	INT_PTR nWidthSpare = (int)((m_rtCanvas.Width() - nWidth) / 2);
	INT_PTR nHeightSpare = (int)((m_rtCanvas.Height() - nHeight) / 2);

	if (nWidthSpare < 0)
		nWidthSpare = 0;

	if (nHeightSpare < 0)
		nHeightSpare = 0;

	m_rtImageOnCanvas.left = nWidthSpare * -1;
	m_rtImageOnCanvas.top = nHeightSpare * -1;
	m_rtImageOnCanvas.right = m_rtImageOnCanvas.left + m_rtCanvas.Width();
	m_rtImageOnCanvas.bottom = m_rtImageOnCanvas.top + m_rtCanvas.Height();

	m_stInterpolatedImg.Init();
	m_stInterpolatedImg.nChannel = 1;
	m_stInterpolatedImg.nWidth = nWidth;
	m_stInterpolatedImg.nHeight = nHeight;

	return TRUE;
}

void CStudyViewer::CalcZoomAndPan()
{
	// zoom
	if (m_dZoomValue < MIN_ZOOM_RATIO)
	{
		m_dZoomValue = MIN_ZOOM_RATIO;
	}
	else if (m_dZoomValue > MAX_ZOOM_RATIO)
	{
		m_dZoomValue = MAX_ZOOM_RATIO;
	}

	// panning
	double dPannedDeltaX = -1 * (double)m_ptPanDelta.x / m_dZoomValue;
	double dPannedDeltaY = (double)m_ptPanDelta.y / m_dZoomValue;

	//
	CPoint ptCenter = CPoint((int)((m_rtImage.right + m_rtImage.left)*0.5f), (int)((m_rtImage.bottom + m_rtImage.top)*0.5f));
	int nConst = (int)(m_rtImage.Width()*0.5f / m_dZoomValue);
	int nConst1 = (int)(m_rtImage.Width() / m_dZoomValue);
	m_rtImage.left = ptCenter.x - nConst;
	m_rtImage.right = m_rtImage.left + nConst1;
	nConst = (int)(m_rtImage.Height()*0.5f / m_dZoomValue);
	nConst1 = (int)(m_rtImage.Height() / m_dZoomValue);
	m_rtImage.top = ptCenter.y - nConst;
	m_rtImage.bottom = m_rtImage.top + nConst1;

	m_rtImage.OffsetRect((int)(dPannedDeltaX + 0.5f), (int)(dPannedDeltaY + 0.5f));

// 	ptCenter = CPoint((int)((m_rtImageOnCanvas.right + m_rtImageOnCanvas.left)*0.5f), (int)((m_rtImageOnCanvas.bottom + m_rtImageOnCanvas.top)*0.5f));
// 	nConst = (int)(m_rtImageOnCanvas.Width()*0.5f / m_dZoomValue);
// 	nConst1 = (int)(m_rtImageOnCanvas.Width() / m_dZoomValue);
// 	m_rtImageOnCanvas.left = ptCenter.x - nConst;
// 	m_rtImageOnCanvas.right = m_rtImageOnCanvas.left + nConst1;
// 	nConst = (int)(m_rtImageOnCanvas.Height()*0.5f / m_dZoomValue);
// 	nConst1 = (int)(m_rtImageOnCanvas.Height() / m_dZoomValue);
// 	m_rtImageOnCanvas.top = ptCenter.y - nConst;
// 	m_rtImageOnCanvas.bottom = m_rtImageOnCanvas.top + nConst1;
// 
// 	m_rtImageOnCanvas.OffsetRect((int)(dPannedDeltaX + 0.5f), (int)(dPannedDeltaY + 0.5f));
}

DIBINFO* CStudyViewer::GetDibInfo()
{
	return &m_DibInfo;
}

BOOL CStudyViewer::AllocDisplayImage()
{
	BOOL bRes = TRUE;

	INT_PTR nWidth = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth;
	INT_PTR nHeight = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
	INT_PTR nSamplePerPixel = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel;

	INT_PTR nImageSize = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).BytesPerLine((UINT)(nWidth*nSamplePerPixel), 8) *
		m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).BytesPerLine((UINT)(nHeight*nSamplePerPixel), 8);

	if (nImageSize <= 0)
		return FALSE;

	m_pDisplayImage = new BYTE[nImageSize];

	if (!m_pDisplayImage)
	{
		return FALSE;
	}

	memset(m_pDisplayImage, 0, nImageSize);

	m_stOriginDisplayImg.nWidth = nWidth;
	m_stOriginDisplayImg.nHeight = nHeight;
	m_stOriginDisplayImg.pImage = m_pDisplayImage;
	m_stOriginDisplayImg.nChannel = 1;

	return bRes;
}

BOOL CStudyViewer::AllocScreenBuffer()
{
	BOOL bRes = TRUE;

	INT_PTR nSceenSize = m_stInterpolatedImg.nWidth * m_stInterpolatedImg.nHeight;

	if (nSceenSize <= 0)
		return FALSE;

	m_pScreenImage = new BYTE[nSceenSize];

	if (!m_pScreenImage)
	{
		return FALSE;
	}

	m_stInterpolatedImg.pImage = m_pScreenImage;

	memset(m_pScreenImage, 0, nSceenSize);
}

void CStudyViewer::FreeDisplayImage()
{
	if (m_pDisplayImage)
	{
		delete[] m_pDisplayImage;
		m_pDisplayImage = nullptr;
	}
}

void CStudyViewer::FreeScreenBuffer()
{
	if (m_pScreenImage)
	{
		delete[] m_pScreenImage;
		m_pScreenImage = nullptr;
	}
}

void CStudyViewer::UpdateScreenData()
{
	if (!m_pDisplayImage)
		return;

	if (!m_pScreenImage)
		return;

	CDicomImage imageDisplayInfo;	// different with DICOM information
	imageDisplayInfo = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex);

	INT_PTR nOrgWidth = BytesPerLine((UINT)imageDisplayInfo.m_stImageInfo.m_nWidth, 8) ;
	INT_PTR nOrgHeight = BytesPerLine((UINT)imageDisplayInfo.m_stImageInfo.m_nHeight, 8);
	INT_PTR nImgROIWidth = 0; 
	INT_PTR nImgROIHeight = 0; 
	INT_PTR nImgWidthOnCanvas = m_rtCanvas.Width();;
	INT_PTR nImgHeightOnCanvas = m_rtCanvas.Height();;

// 	if (m_rtImage.left <= 0)
// 	{
// 		nImgROIWidth = m_rtImage.right + m_rtImage.left;
// 	}
// 	else
// 	{
// 		nImgROIWidth = m_rtImage.right - m_rtImage.left;
// 	}
// 	
// 	if (m_rtImage.top <= 0)
// 	{
// 		nImgROIHeight = m_rtImage.bottom + m_rtImage.top;
// 	}
// 	else
// 	{
// 		nImgROIHeight = m_rtImage.bottom - m_rtImage.top;
// 	}

	if (m_rtImage.left <= 0)
	{
		nImgROIWidth = BytesPerLine(m_rtImage.right + m_rtImage.left, 8) ;
	}
	else
	{
		nImgROIWidth = BytesPerLine(m_rtImage.right - m_rtImage.left, 8) ;
	}

	if (m_rtImage.top <= 0)
	{
		nImgROIHeight = BytesPerLine(m_rtImage.bottom + m_rtImage.top, 8) ;
	}
	else
	{
		nImgROIHeight = BytesPerLine(m_rtImage.bottom - m_rtImage.top, 8) ;
	}
	

	if (nImgROIWidth > imageDisplayInfo.m_stImageInfo.m_nWidth)
		nImgROIWidth = nOrgWidth;
	
	if (nImgROIHeight > imageDisplayInfo.m_stImageInfo.m_nHeight)
		nImgROIHeight = nOrgHeight;


	BYTE* pOriginROIImg = new BYTE[nImgROIWidth * nImgROIHeight];

	m_stROIImg.Init();
	m_stROIImg.nChannel = 1;
	m_stROIImg.nWidth = nImgROIWidth;
	m_stROIImg.nHeight = nImgROIHeight;
	m_stROIImg.pImage = pOriginROIImg;

	CopyROIImageFromOrigin(m_pDisplayImage, pOriginROIImg, nOrgWidth, nOrgHeight, nImgROIWidth, nImgROIHeight);

	DoInterpolate(pOriginROIImg,
		m_pScreenImage,
		nImgROIWidth,
		nImgROIHeight,
		m_stInterpolatedImg.nWidth,
		m_stInterpolatedImg.nHeight);

	delete[] pOriginROIImg;

	return;
}

void CStudyViewer::CreateDragDropObjects()
{

}

void CStudyViewer::CopyROIImageFromOrigin(BYTE* pSrc, BYTE* pDest, INT_PTR nSrcWidth, INT_PTR nSrcHeight, INT_PTR nROIWidth, INT_PTR nROIHeight)
{
	UINT nLine = 0, nTotalSize = 0;
	UINT nDestLine, nSrcLine;
	UINT nCopySize;
	nTotalSize = nROIHeight;
	nDestLine = nROIWidth;
	nSrcLine = (UINT)nSrcWidth;
	nCopySize = (UINT)nROIWidth;

	if (m_rtImage.left > 0)
	{
		pSrc += m_rtImage.left;
	}
	
	for (nLine = 0; nLine < nTotalSize; nLine++, pDest += nDestLine, pSrc += nSrcLine)
	{
		memcpy_s(pDest, nDestLine, pSrc, nROIWidth);
	}
}

BOOL CStudyViewer::DoInterpolate(BYTE * pSrcImage, BYTE * pOutImage, UINT nInWidth, UINT nInHeight, UINT nOutWidth, UINT nOutHeight)
{
	UINT nrow, ncol, nIndex, nAllow1, nAllow2;
	double dEWweight, dNSweight, dEWtop, dEWbottom;
	double doublex, doubley;
	double dWidthScale, dHeightScale;
	double dInWidth, dOutWidth, dInHeight, dOutHeight;
	UCHAR NW, NE, SW, SE;
	ULONG nAddress, nSrcImageLength;
	BYTE* pLineBuffer = NULL;

	TRY
	{
		pLineBuffer = new BYTE[nOutWidth];	memset(pLineBuffer, 0, nOutWidth);

		dInWidth = (double)nInWidth;	dOutWidth = (double)nOutWidth;
		dInHeight = (double)nInHeight;	dOutHeight = (double)nOutHeight;

		dWidthScale = dOutWidth / dInWidth;
		dHeightScale = dOutHeight / dInHeight;

		nAllow1 = (dWidthScale > 1) ? (UINT)dWidthScale + 1 : (2);
		nAllow2 = (dHeightScale > 1) ? (UINT)dHeightScale + 1 : (2);

		nSrcImageLength = nOutWidth * nOutHeight;

		for (nrow = 0; nrow < nOutHeight; nrow++)
		{
			nIndex = 0;
			for (ncol = 0; ncol < nOutWidth; ncol++)
			{
				doublex = (double)(ncol / dWidthScale);
				doubley = (double)(nrow / dHeightScale);

				dEWweight = doublex - (int)doublex;
				dNSweight = doubley - (int)doubley;

				if (ncol >= (nOutWidth - nAllow1) || nrow >= (nOutHeight - nAllow2))
				{
					nAddress = (int)doubley     * nInWidth + (int)doublex;
					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
					NW = (UCHAR)*(pSrcImage + nAddress);

					nAddress = (int)doubley     * nInWidth + ((int)doublex + 1);
					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
					NE = (UCHAR)*(pSrcImage + nAddress);

					nAddress = ((int)doubley + 1) * nInWidth + (int)doublex;
					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
					SW = (UCHAR)*(pSrcImage + nAddress);

					nAddress = ((int)doubley + 1) * nInWidth + ((int)doublex + 1);
					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
					SE = (UCHAR)*(pSrcImage + nAddress);
				}
				else
				{
					NW = (UCHAR)*(pSrcImage + (int)doubley     * nInWidth + (int)doublex);
					NE = (UCHAR)*(pSrcImage + (int)doubley     * nInWidth + (int)doublex + 1);
					SW = (UCHAR)*(pSrcImage + ((int)doubley + 1) * nInWidth + (int)doublex);
					SE = (UCHAR)*(pSrcImage + ((int)doubley + 1) * nInWidth + (int)doublex + 1);
				}

				dEWtop = NW + dEWweight * (NE - NW);
				dEWbottom = SW + dEWweight * (SE - SW);

				*(pLineBuffer + nIndex) = (BYTE)(dEWtop + dNSweight * (dEWbottom - dEWtop));
				nIndex++;
			}
			memcpy(pOutImage + nrow * nOutWidth, pLineBuffer, nOutWidth);
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


BEGIN_MESSAGE_MAP(CStudyViewer, CWnd)
ON_WM_SIZE()
ON_WM_PAINT()
//ON_WM_MOUSEHWHEEL()
ON_WM_MOUSEWHEEL()
ON_WM_LBUTTONDOWN()
ON_WM_DESTROY()
ON_WM_MOUSEMOVE()
ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void CStudyViewer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}


void CStudyViewer::OnPaint()
{
	CalcLayout();
	CPaintDC dc(this); 
	Draw(&dc);
}

BOOL CStudyViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if ((m_pStudy == nullptr) ||
		(m_pDisplayDicomDS == nullptr))
	{
		CWnd::OnMouseWheel(nFlags, zDelta, pt);
	}

	BOOL bIsCtrlKeyPressed = (nFlags & MK_CONTROL) ? TRUE : FALSE;
	BOOL bIsAltKeyPressed = (nFlags & MK_ALT) ? TRUE : FALSE;
	BOOL bIsShiftKeyPressed = (nFlags & MK_SHIFT) ? TRUE : FALSE;

	ClearLocalizerPoints();

	if (bIsShiftKeyPressed)
	{
		ChangeSeriesImageByWheel(zDelta);
	}
	else
	{
		ChangeInstanceImageByWheel(zDelta);
	}

	if (bIsCtrlKeyPressed)
	{
		if(zDelta > 0)
		{
			ZoomIn(bIsAltKeyPressed);
		}
		else if (zDelta < 0)
		{
			ZoomOut(bIsAltKeyPressed);
		}
	}


	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CStudyViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

// 	if (!m_pStudy)
// 		return;

	BOOL bIsShiftKeyPressed = (nFlags & MK_SHIFT) ? TRUE : FALSE;
	BOOL bIsCtrlKeyPressed = (nFlags & MK_CONTROL) ? TRUE : FALSE;
	BOOL bIsAltKeyPressed = (nFlags & MK_ALT) ? TRUE : FALSE;
	BOOL bIsLButtonPressed = (nFlags & MK_LBUTTON) ? TRUE : FALSE;
	BOOL bIsRButtonPressed = (nFlags & MK_RBUTTON) ? TRUE : FALSE;

	SetOperationModeByKey(bIsShiftKeyPressed, bIsCtrlKeyPressed, bIsAltKeyPressed, bIsLButtonPressed, bIsRButtonPressed);

	switch (m_nOperationMode)
	{
	case MODE_PAN:
		break;
	case MODE_PAN_DOWN:
		SetOldMousePosBeforePan(point);
		break;
	}

	ClearLocalizerPoints();

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}

	CWnd::OnLButtonDown(nFlags, point);
}


void CStudyViewer::OnDestroy()
{
	CWnd::OnDestroy();
	// TODO: Add your message handler code here
}


void CStudyViewer::OnMouseMove(UINT nFlags, CPoint point)
{
// 	if (!m_pStudy)
// 	{
// 		return ;
// 	}

	BOOL bIsShiftKeyPressed = (nFlags & MK_SHIFT) ? TRUE : FALSE;
	BOOL bIsCtrlKeyPressed = (nFlags & MK_CONTROL) ? TRUE : FALSE;
	BOOL bIsAltKeyPressed = (nFlags & MK_ALT) ? TRUE : FALSE;
	BOOL bIsLButtonPressed = (nFlags & MK_LBUTTON) ? TRUE : FALSE;
	BOOL bIsRButtonPressed = (nFlags & MK_RBUTTON) ? TRUE : FALSE;

	SetOperationModeByKey(bIsShiftKeyPressed, bIsCtrlKeyPressed, bIsAltKeyPressed, bIsLButtonPressed, bIsRButtonPressed);

	switch (m_nOperationMode)
	{
	case MODE_PAN:
		break;
	case MODE_PAN_DOWN:
		{
			OperatePan(point);
			break;
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


void CStudyViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// 	if (!m_pStudy)
// 	{
// 		return;
// 	}
// 
// 	if (nChar == VK_SHIFT && nChar == VK_CONTROL &&m_nOperationMode != MODE_PAN_DOWN)
// 	{
// 		m_nOperationMode = MODE_PAN;
// 	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
