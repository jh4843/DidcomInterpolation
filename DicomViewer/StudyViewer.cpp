#include "stdafx.h"
#include "StudyViewer.h"
#include "CoordinatorUtill.h"
#include "DicomParser.h"
#include "LayoutManager.h"
#include "MainFrm.h"
#include "MyImageInterpolationDrawUtils.h"


CStudyViewer::CStudyViewer(INT_PTR nLayoutIndex)
{
	m_nLayoutIndex = nLayoutIndex;

	m_pDisplayDicomDS = nullptr;

	m_stOriginDisplayImg.Init();
	m_stROIImg.Init();
	m_stInterpolatedImg.Init();

	m_bOnlySeriesViewer = FALSE;

	m_nOperationMode = MODE_NORMAL;
	m_eInterpolationType = InterpolationTypeBilinear;
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
	FreeRoiBuffer();
	FreeDispRoiBuffer();
	AllocDisplayImage();

	CDicomImage* pDsInputImage = &m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex);
	
	LoadImageFromDcm(pDsInputImage);
	pDsInputImage->GetImageProcessedImage(m_pDisplayImage, &imageDisplayInfo);
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

void CStudyViewer::SetInterpolationMode(INTERPOLATION_TYPE eMode)
{
	m_eInterpolationType = eMode;
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

void CStudyViewer::LoadImageFromDcm(CDicomImage* pDsImage)
{
	TRY
	{
		CDicomParser dicomParser;

		dicomParser.LoadDS((LPTSTR)(LPCTSTR)m_pDisplayDicomDS->m_dcmHeaderInfo.m_strFileName, 0);
		dicomParser.ParseDicomHeader();
		dicomParser.ParseImageInfo();

		BITMAPHANDLE OrgImageBitmapHandle;

		pDICOMELEMENT pElement;
		UINT nFlags = DICOM_GETIMAGE_AUTO_APPLY_MODALITY_LUT | DICOM_GETIMAGE_AUTO_APPLY_VOI_LUT;

		CString strModality = dicomParser.GetValue(TAG_MODALITY);

		if (strModality.CompareNoCase(_T("CT")) == 0 ||
			strModality.CompareNoCase(_T("MR")) == 0)
		{
			nFlags |= DICOM_GETIMAGE_AUTO_LOAD_OVERLAYS;
		}

		pElement = dicomParser.FindLastElement(NULL, TAG_PIXEL_DATA, FALSE);
		L_UINT16 nResult = dicomParser.GetImage(pElement,
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
			nResult = dicomParser.GetImage(pElement,
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
				nResult = dicomParser.GetImage(pElement,
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
				pDsImage->SetTiledPiexlData(TRUE);
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

			pDsImage->FreeDicomImage();
			pDsImage->m_stImageInfo.Init();

			if (OrgImageBitmapHandle.HighBit < 1)
			{
				AfxThrowUserException();
			}
			else
			{
				pDsImage->m_stImageInfo.m_nBitsPerPixel = (UINT)(OrgImageBitmapHandle.HighBit + 1);
			}

			// RGB : SamplesPerPixel = 3
			if (OrgImageBitmapHandle.BitsPerPixel == 24)
			{
				pDsImage->m_stImageInfo.m_nSamplesPerPixel = 3;
			}
			// Gray : SamplesPerPixel = 1 (default)
			else if (OrgImageBitmapHandle.BitsPerPixel > 0 && OrgImageBitmapHandle.BitsPerPixel < 24)
			{
				pDsImage->m_stImageInfo.m_nSamplesPerPixel = 1;
			}
			else
			{
				AfxThrowUserException();
			}
		}

		pDsImage->m_stImageInfo.m_nBytesPerPixel = (UINT)Bits2Bytes(pDsImage->m_stImageInfo.m_nBitsPerPixel);
		pDsImage->m_stImageInfo.m_nTotalAllocatedBytes = (UINT)(pDsImage->m_stImageInfo.m_nBytesPerPixel*pDsImage->m_stImageInfo.m_nSamplesPerPixel);
		pDsImage->m_stImageInfo.m_nWidth = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth;
		pDsImage->m_stImageInfo.m_nHeight = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
		int nBytesPerLineX = ((pDsImage->m_stImageInfo.m_nWidth * pDsImage->m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
		int nBytesPerLineY = ((pDsImage->m_stImageInfo.m_nHeight * pDsImage->m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
		pDsImage->m_stImageInfo.m_nWidth = nBytesPerLineX / pDsImage->m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
		pDsImage->m_stImageInfo.m_nHeight = nBytesPerLineY / pDsImage->m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
		pDsImage->m_stImageInfo.m_nBytesPerLine = pDsImage->m_stImageInfo.m_nWidth * pDsImage->m_stImageInfo.m_nTotalAllocatedBytes;

		pDsImage->m_stImageInfo.m_fW1 = 0.0f;
		pDsImage->m_stImageInfo.m_fW2 = Bits2MaxValue(pDsImage->m_stImageInfo.m_nBitsPerPixel);
		//
		// Get W/L
		L_DOUBLE dWindowCenter = 0;
		L_DOUBLE dWindowWidth = 0;
		// Window Center
		pElement = dicomParser.FindFirstElement(NULL, TAG_WINDOW_CENTER, FALSE);
		if (pElement && pElement->nLength)
		{
			dWindowCenter = *(dicomParser.GetDoubleValue(pElement, 0, 1));
			pDsImage->m_stImageInfo.m_nW1 = (int)dWindowCenter;
		}
		else
		{
			pDsImage->m_stImageInfo.m_nW1 = (int)pDsImage->m_stImageInfo.m_fW1;
		}

		// Window Width
		pElement = dicomParser.FindFirstElement(NULL, TAG_WINDOW_WIDTH, FALSE);
		if (pElement && pElement->nLength)
		{
			dWindowWidth = *(dicomParser.GetDoubleValue(pElement, 0, 1));
			pDsImage->m_stImageInfo.m_nW2 = (int)dWindowWidth;
		}
		else
		{
			pDsImage->m_stImageInfo.m_nW2 = (int)pDsImage->m_stImageInfo.m_fW2;
		}
		//
		BITMAPHANDLE OverlayBitmapHandle;
		if (dicomParser.GetOverlayImageFromOrigin(&OrgImageBitmapHandle, &OverlayBitmapHandle) > 0)
		{
			pDsImage->LoadDicomImage(&OrgImageBitmapHandle, &OverlayBitmapHandle);
		}
		else
		{
			pDsImage->LoadDicomImage(&OrgImageBitmapHandle);
		}


		L_FreeBitmap(&OrgImageBitmapHandle);

		dicomParser.ResetDS();
	}
	CATCH_ALL(e)
	{
		if (e->IsKindOf(RUNTIME_CLASS(CMemoryException)))
		{
			AfxMessageBox(_T("MEMORY EXCEPTION"));
		}
		else
		{
			AfxMessageBox(_T("Fail"));
		}
	}
	END_CATCH_ALL
}

void CStudyViewer::ResetPan()
{
	m_ptOldPointBeforePan = CPoint(0, 0);
	m_ptPanDelta = CPoint(0, 0);
}

void CStudyViewer::ResetZoom()
{
	m_dZoomValue = 1.0;
	m_dOldZoomValue = 1.0;
}

void CStudyViewer::OperatePan(CPoint point)
{
	if (!m_pDisplayDicomDS)
		return;

	if (!m_pDisplayImage)
		return;

	CPoint ptDelta;
	ptDelta = point - m_ptOldPointBeforePan;

	ptDelta.x = (int)((double)ptDelta.x);
	ptDelta.y = (int)((double)ptDelta.y);

// 	ptDelta.x = (int)((double)ptDelta.x / m_dCanvasPerImageRatio*m_dZoomValue + 0.5);
// 	ptDelta.y = (int)((double)ptDelta.y / m_dCanvasPerImageRatio*m_dZoomValue + 0.5);
	m_ptPanDelta += ptDelta;

	SetOldMousePosBeforePan(point);

	CalcImageRectEx(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	RedrawWnd();
}

void CStudyViewer::ZoomIn(BOOL bIsDynamic, double dDelta)
{
	if (!m_pDisplayDicomDS)
		return;

	if (!m_pDisplayImage)
		return;

	double dRatio = 0.01;

	if (bIsDynamic)
		m_dZoomValue += dRatio * dDelta;
	else
		m_dZoomValue += dRatio * 20.0;	// 20%

	if (m_dZoomValue < MIN_ZOOM_RATIO)
	{
		m_dZoomValue = MIN_ZOOM_RATIO;
	}
	else if (m_dZoomValue > MAX_ZOOM_RATIO)
	{
		m_dZoomValue = MAX_ZOOM_RATIO;
	}

	CalcImageRectEx(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	RedrawWnd();
}

void CStudyViewer::ZoomOut(BOOL bIsDynamic, double dDelta)
{
	if (!m_pDisplayDicomDS)
		return;

	if (!m_pDisplayImage)
		return;

	double dRatio = 0.01;
	dDelta = dDelta * -1;

	if (bIsDynamic)
		m_dZoomValue -= dRatio * dDelta;	// 2%
	else
		m_dZoomValue -= dRatio * 20.0;	// 20%

	if (m_dZoomValue < MIN_ZOOM_RATIO)
	{
		m_dZoomValue = MIN_ZOOM_RATIO;
	}
	else if (m_dZoomValue > MAX_ZOOM_RATIO)
	{
		m_dZoomValue = MAX_ZOOM_RATIO;
	}

	CalcImageRectEx(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
	RedrawWnd();
}

BOOL CStudyViewer::ProcessHotKey(UINT nKey)
{
	if ((m_pStudy == nullptr) || (m_pDisplayDicomDS == nullptr))
	{
		return FALSE;
	}

	switch (nKey)
	{
	case VK_ADD:
		ZoomIn(FALSE);
		break;
	case VK_SUBTRACT:
		ZoomOut(FALSE);
		break;
	case VK_ESCAPE:
		ResetZoom();
		ResetPan();
		RedrawWnd();
		break;
	default:
		break;
	}

	return TRUE;
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
	m_pDispRoiImage = nullptr;
	m_pRoiImage = nullptr;
	m_pDisplayDicomDS = nullptr;
	
	FreeDisplayImage();
	FreeDispRoiBuffer();
	FreeRoiBuffer();
	
	m_nCurSeriesIndex = nCurSeriesIndex;
	m_nCurInstanceIndex = nCurInstanceIndex;
	m_nCurFrameIndex = nCurFrameIndex;

	m_DibInfo.Init();

	m_rtCanvas = CRect(0, 0, 0, 0);
	m_rtImage = CRect(0, 0, 0, 0);
	m_rtImageEx = Gdiplus::RectF(0.0f, 0.0f, 0.0f, 0.0f);
	m_rtDrawRectOnCanvas = CRect(0, 0, 0, 0);

	m_dCanvasPerImageRatio = 1.0;
	m_dZoomValue = 1.0;
	m_dOldZoomValue = 1.0;

	m_ptPanDelta = CPoint(0, 0);
	m_ptOldPointBeforePan = CPoint(0, 0);

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

	if (!m_pDisplayImage)
	{
		return FALSE;
	}
		

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	clock_t clockStart, clockEnd;

	clockStart = clock();

#ifdef BY_GDI_INTERPOLATION
	BITMAPINFOHEADER& bih = GetDibInfo()->bmiHeader;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth;
	bih.biHeight = -m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 8 * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).BytesPerLine((UINT)((double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth*(double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel), 8) * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	// 
	Graphics g(pDC->GetSafeHdc());

	switch (m_eInterpolationType)
	{
	case InterpolationTypeBicubicPolynomial_050:
	case InterpolationTypeBicubicPolynomial_075:
	case InterpolationTypeBicubicPolynomial_100:
	case InterpolationTypeBicubicPolynomial_000:
	case InterpolationTypeBicubicPolynomial_300:
	case InterpolationTypeBicubicMichell:
	case InterpolationTypeBicubicLanczos:
	case InterpolationTypeBicubicCatmullRom:
		g.SetInterpolationMode(InterpolationModeBicubic);
		pMainFrm->SetStatusBarText(1, _T("Bicubic"));
		break;
	case InterpolationTypeBicubicBSpline:
		g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		pMainFrm->SetStatusBarText(1, _T("BicubicHighQuality"));
		break;
	case InterpolationTypeBilinear:
	default:
		g.SetInterpolationMode(InterpolationModeBilinear);
		pMainFrm->SetStatusBarText(1, _T("Bilinear"));
		break;
	}

	Gdiplus::Bitmap bitmap((BITMAPINFO*)GetDibInfo(), m_pDisplayImage);

	m_rtImage.OffsetRect(0, 0);

	INT nSrcWidth = m_rtImage.Width();
	INT nSrcHeight = m_rtImage.Height();

	//
	g.DrawImage(&bitmap,
		Rect(m_rtCanvas.left, m_rtCanvas.top, m_rtCanvas.Width(), m_rtCanvas.Height()),
		m_rtImage.left,
		m_rtImage.top,
		nSrcWidth,
		nSrcHeight,
		UnitPixel);

#elif BY_GDI_NO_INTERPOLATION
	BITMAPINFOHEADER& bih = GetDibInfo()->bmiHeader;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth;
	bih.biHeight = -m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 8 * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).BytesPerLine((UINT)((double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nWidth*(double)m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel), 8) * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nHeight;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	// 
	Graphics g(pDC->GetSafeHdc());

	Gdiplus::Bitmap bitmap((BITMAPINFO*)GetDibInfo(), m_pDisplayImage);

	m_rtImage.OffsetRect(0, 0);

	INT nSrcWidth = m_rtImage.Width();
	INT nSrcHeight = m_rtImage.Height();

	pMainFrm->SetStatusBarText(1, _T("No Interpolation"));

	//
	g.DrawImage(&bitmap,
		Rect(m_rtCanvas.left, m_rtCanvas.top, m_rtCanvas.Width(), m_rtCanvas.Height()),
		m_rtImage.left,
		m_rtImage.top,
		nSrcWidth,
		nSrcHeight,
		UnitPixel);

#else
	CDicomImage::IMAGE_INFO* pImageInfo = &m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo;

	CMyImageInterpolationDrawUtils interDrawUtil(pImageInfo,
		m_pDisplayImage, 
		m_rtCanvas, 
		m_ptPanDelta,
		m_dCanvasPerImageRatio,
		m_dZoomValue,
		m_eInterpolationType);

	BOOL bRes = interDrawUtil.UpdateDisplayRoiImage();

	if (bRes)
	{
		CRect rtDisplayedRoiOnCanvas = interDrawUtil.GetDisplayedRoiRectOnCanvas();

		BITMAPINFOHEADER& bih = GetDibInfo()->bmiHeader;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = rtDisplayedRoiOnCanvas.Width();
		bih.biHeight = -1 * rtDisplayedRoiOnCanvas.Height();
		bih.biPlanes = 1;
		bih.biBitCount = 8 * m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex).m_stImageInfo.m_nSamplesPerPixel;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = interDrawUtil.CalcScreenImageSize();
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		CRect rtDrawRectOnCanvas = interDrawUtil.GetDrawRectOnCanvas();
		//
		::SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
		::StretchDIBits(pDC->GetSafeHdc(),
			m_rtCanvas.left,
			m_rtCanvas.top,
			m_rtCanvas.Width(),
			m_rtCanvas.Height(),
			rtDrawRectOnCanvas.left,
			rtDrawRectOnCanvas.top,
			rtDrawRectOnCanvas.Width(),
			rtDrawRectOnCanvas.Height(),
			(void*)interDrawUtil.GetScreenImage(),
			(BITMAPINFO*)GetDibInfo(),
			DIB_RGB_COLORS,
			SRCCOPY);
#endif
	}

	clockEnd = clock();

	CString strTimeStamp;
	float fResultTime = (float)(clockEnd - clockStart) / 1000;

	strTimeStamp.Format(_T("Update total time : %0.2f sec"), fResultTime);
	pMainFrm->SetStatusBarText(2, strTimeStamp);
	
	return TRUE;
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
	font.CreatePointFont(nFontSize, _T("Segoe UI"));

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
	CString strZoomValue;
	strZoomValue.Format(_T("Zoom Value : %0.2f"), m_dZoomValue);
	aryImageInfo.Add(strZoomValue);

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
	font.CreatePointFont(nFontSize, _T("Segoe UI"));

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

	DrawImageInfo(pDC);

	//pDC->SetBkMode(TRANSPARENT);
	
	return TRUE;
}

BOOL CStudyViewer::CalcLayout()
{
	BOOL bRes = TRUE;

	CRect rtOldCanvas = m_rtCanvas;

	GetClientRect(&m_rtCanvas);

	if ((m_rtCanvas.Width() != rtOldCanvas.Width()) ||
		(m_rtCanvas.Height() != rtOldCanvas.Height()))
	{
		m_rtDisplayedROIOnImage.SetRectEmpty();
		m_rtDisplayedROIOnCanvas.SetRectEmpty();
		m_rtDrawRectOnCanvas.SetRectEmpty();
	}

	if (m_pDisplayDicomDS)
	{
		CalcImageRectEx(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
		// Gdiplus
		CalcImageRect(&m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex));
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

BOOL CStudyViewer::CalcImageRectEx(CDicomImage * pImageInfo)
{
	double dImgWidth = 0., dImgHeight = 0.;
	double dImgWidthOnCanvas = 0., dImgHeightOnCanvas = 0.;
	double dCanvasWidth = 0., dCanvasHeight = 0.;
	double dRatio = 0.;
	double dRatioBasedOnHeight = 0.;
	double dRatioBasedOnWidth = 0.;

	//
	dImgWidth = pImageInfo->m_stImageInfo.m_nWidth;
	dImgHeight = pImageInfo->m_stImageInfo.m_nHeight;
	dCanvasWidth = (double)m_rtCanvas.Width();
	dCanvasHeight = (double)m_rtCanvas.Height();

	dRatioBasedOnHeight = dCanvasHeight / dImgHeight;
	dRatioBasedOnWidth = dCanvasWidth / dImgWidth;

	if (dRatioBasedOnHeight < dRatioBasedOnWidth)
	{
		dRatio = dRatioBasedOnHeight;
	}
	else
	{
		dRatio = dRatioBasedOnWidth;
	}

	dImgWidthOnCanvas = dImgWidth * dRatio * m_dZoomValue;
	dImgHeightOnCanvas = dImgHeight * dRatio * m_dZoomValue;

	m_rtImageEx.X = (dCanvasWidth * 0.5f) - (dImgWidthOnCanvas * 0.5);
	m_rtImageEx.Width = dImgWidthOnCanvas;
	m_rtImageEx.Y = (dCanvasHeight * 0.5f) - (dImgHeightOnCanvas * 0.5);
	m_rtImageEx.Height = dImgHeightOnCanvas;

	m_rtImageEx.X	+= (double)m_ptPanDelta.x;
	m_rtImageEx.Y	+= (double)m_ptPanDelta.y;

	return TRUE;
}

BOOL CStudyViewer::CalcDisplayImageROI(CDicomImage * pImageInfo)
{
	if (!pImageInfo)
		return FALSE;

	INT_PTR nImageWidth = (INT_PTR)pImageInfo->m_stImageInfo.m_nWidth;
	INT_PTR nImageHeight = (INT_PTR)pImageInfo->m_stImageInfo.m_nHeight;

	double nCanvasHeight = (double)m_rtCanvas.Height();
	double nCanvasWidth = (double)m_rtCanvas.Width();
	
	double dWidthRatio = (double)nImageWidth / (double)m_rtImageEx.Width;
	double dHeightRatio = (double)nImageHeight / (double)m_rtImageEx.Height;

	CRect rtOldRoiImage = m_rtDisplayedROIOnImage;

	if (m_rtImageEx.GetLeft() > nCanvasWidth || m_rtImageEx.GetTop() > nCanvasHeight ||
		m_rtImageEx.GetRight() < 0 || m_rtImageEx.GetBottom() < 0)
	{
		m_rtDisplayedROIOnImage.left = 0;
		m_rtDisplayedROIOnImage.right = 0;
		m_rtDisplayedROIOnImage.top = 0;
		m_rtDisplayedROIOnImage.bottom = 0;

		return TRUE;
	}

	if (m_rtImageEx.GetLeft() > 0) // 좌측이 다 보이는경우,
	{
		m_rtDisplayedROIOnImage.left = 0;
	}
	else // 좌측이 다 보이지 않는 경우
	{
		m_rtDisplayedROIOnImage.left = m_rtImageEx.X * dWidthRatio * (-1);
		//m_rtDisplayedROIOnImage.left = m_rtImage.left;
	}
	
	if (m_rtImageEx.GetRight() > nCanvasWidth) // 우측이 다 보이는 경우
	{
		m_rtDisplayedROIOnImage.right = nImageWidth - ((m_rtImageEx.GetRight() - nCanvasWidth) * dWidthRatio);
	}
	else // 우측이 다 보이지 않는 경우
	{
		m_rtDisplayedROIOnImage.right = nImageWidth;
	}

	if (m_rtImageEx.GetTop() > 0) // 상단이 다 보이는 경우,
	{
		m_rtDisplayedROIOnImage.top = 0;
	}
	else // 상단이 다 보이지 않는 경우
	{
		m_rtDisplayedROIOnImage.top = m_rtImageEx.GetTop() * dHeightRatio * (-1);
	}
	
	if (m_rtImageEx.GetBottom() < nCanvasHeight) // 하단이 다 보이는 경우,
	{
		m_rtDisplayedROIOnImage.bottom = nImageHeight;
	}
	
	else // 하단이 다 보이지 않는 경우,
	{
		m_rtDisplayedROIOnImage.bottom = nImageHeight - ((m_rtImageEx.GetBottom() - nCanvasHeight) * dHeightRatio);
	}

	INT_PTR nRoiImageWidth = m_rtDisplayedROIOnImage.Width();
	INT_PTR nRoiImageHeight = m_rtDisplayedROIOnImage.Height();

	m_rtDisplayedROIOnImage.right = m_rtDisplayedROIOnImage.left + BytesPerLine(nRoiImageWidth, 8);
	m_rtDisplayedROIOnImage.bottom = m_rtDisplayedROIOnImage.top + BytesPerLine(nRoiImageHeight, 8);

	if (m_rtDisplayedROIOnImage.right > nImageWidth)
	{
		m_rtDisplayedROIOnImage.right = nImageWidth;
	}

	if (m_rtDisplayedROIOnImage.bottom > nImageHeight)
	{
		m_rtDisplayedROIOnImage.bottom = nImageHeight;
	}
	
	if (m_dOldZoomValue != m_dZoomValue &&
		m_rtDisplayedROIOnImage.Width() != nImageWidth &&	// 영상이 전부 보일 때는 고려하지 않는다.
		m_rtDisplayedROIOnImage.Height() != nImageHeight)
	{
		if (m_rtDisplayedROIOnImage.left != rtOldRoiImage.left &&
			m_rtDisplayedROIOnImage.Width() == rtOldRoiImage.Width())
		{
			double dRatio = (double)rtOldRoiImage.Height() / (double)rtOldRoiImage.Width();
			INT_PTR nDiff = m_rtDisplayedROIOnImage.left - rtOldRoiImage.left;
			m_rtDisplayedROIOnImage.right = rtOldRoiImage.right - nDiff;

			double dHeight = (double)m_rtDisplayedROIOnImage.Width() * dRatio;
			m_rtDisplayedROIOnImage.top = ((double)m_rtDisplayedROIOnImage.top + (double)m_rtDisplayedROIOnImage.bottom - dHeight) / 2;
			m_rtDisplayedROIOnImage.bottom = (double)m_rtDisplayedROIOnImage.top + dHeight;

//			m_rtDisplayedROIOnImage = rtOldRoiImage;
			return FALSE;  
		}

		if (m_rtDisplayedROIOnImage.top != rtOldRoiImage.top &&
			(m_rtDisplayedROIOnImage.Height() == rtOldRoiImage.Height()))
		{
			double dRatio = (double)rtOldRoiImage.Width() / (double)rtOldRoiImage.Height();
			INT_PTR nDiff = m_rtDisplayedROIOnImage.top - rtOldRoiImage.top;
			m_rtDisplayedROIOnImage.bottom = rtOldRoiImage.bottom - nDiff;

			double dWidth = (double)m_rtDisplayedROIOnImage.Height() * dRatio;
			m_rtDisplayedROIOnImage.left = ((double)m_rtDisplayedROIOnImage.left + (double)m_rtDisplayedROIOnImage.right - dWidth) / 2;
			m_rtDisplayedROIOnImage.right = (double)m_rtDisplayedROIOnImage.left + dWidth;

//			m_rtDisplayedROIOnImage = rtOldRoiImage;
			return FALSE;  
		}
	}
	
	m_dOldZoomValue = m_dZoomValue;
	return TRUE;
}

BOOL CStudyViewer::CalcDisplayCanvasROI(CDicomImage * pImageInfo)
{
	if (!pImageInfo)
		return FALSE;

	if (m_rtDisplayedROIOnImage.Width() == 0 || m_rtDisplayedROIOnImage.Height() == 0)
	{
		m_rtDisplayedROIOnCanvas.left	= 0;
		m_rtDisplayedROIOnCanvas.right	= 0;
		m_rtDisplayedROIOnCanvas.top	= 0;
		m_rtDisplayedROIOnCanvas.bottom = 0;

		return TRUE;
	}

	INT_PTR nImageWidth = (INT_PTR)pImageInfo->m_stImageInfo.m_nWidth;
	INT_PTR nImageHeight = (INT_PTR)pImageInfo->m_stImageInfo.m_nHeight;

	double dCanvasHeight = (double)m_rtCanvas.Height();
	double dCanvasWidth = (double)m_rtCanvas.Width();

////////////////////////////////////////////////////////////////////////

	m_rtDisplayedROIOnCanvas.left = m_rtImageEx.X;
	m_rtDisplayedROIOnCanvas.right = m_rtImageEx.Width + m_rtImageEx.X;
	m_rtDisplayedROIOnCanvas.top = m_rtImageEx.Y;
	m_rtDisplayedROIOnCanvas.bottom = m_rtImageEx.Height + m_rtImageEx.Y;

	if (m_rtDisplayedROIOnCanvas.left < 0)
	{
		m_rtDisplayedROIOnCanvas.left = 0;
	}

	if (m_rtDisplayedROIOnCanvas.right > dCanvasWidth)
	{
		m_rtDisplayedROIOnCanvas.right = dCanvasWidth;
	}

	if (m_rtDisplayedROIOnCanvas.top < 0)
	{
		m_rtDisplayedROIOnCanvas.top = 0;
	}

	if (m_rtDisplayedROIOnCanvas.bottom > dCanvasHeight)
	{
		m_rtDisplayedROIOnCanvas.bottom = dCanvasHeight;
	}

	INT_PTR nRoiCanvasWidth = m_rtDisplayedROIOnCanvas.Width();
	INT_PTR nRoiCanvasHeight = m_rtDisplayedROIOnCanvas.Height();

	m_rtDisplayedROIOnCanvas.right = m_rtDisplayedROIOnCanvas.left + BytesPerLine(nRoiCanvasWidth, 8);
	m_rtDisplayedROIOnCanvas.bottom = m_rtDisplayedROIOnCanvas.top + BytesPerLine(nRoiCanvasHeight, 8);


	m_stInterpolatedImg.Init();
	m_stInterpolatedImg.nChannel = 1;
	m_stInterpolatedImg.nWidth = m_rtDisplayedROIOnCanvas.Width();
	m_stInterpolatedImg.nHeight = m_rtDisplayedROIOnCanvas.Height();

	//////////////////////////////////////////
	return TRUE;
}


BOOL CStudyViewer::CalcDrawRectOnCanvasRect(CDicomImage * pImageInfo)
{
	if (!pImageInfo)
		return FALSE;

	if (m_rtDisplayedROIOnImage.Width() == 0 || m_rtDisplayedROIOnImage.Height() == 0)
	{
		m_rtDrawRectOnCanvas.left = 0;
		m_rtDrawRectOnCanvas.right = 0;
		m_rtDrawRectOnCanvas.top = 0;
		m_rtDrawRectOnCanvas.bottom = 0;

		return TRUE;
	}

	double nCanvasHeight = (double)m_rtCanvas.Height();
	double nCanvasWidth = (double)m_rtCanvas.Width();

	if (m_rtImageEx.GetLeft() < 0)
	{
		m_rtDrawRectOnCanvas.left = 0;
	}
	else
	{
		m_rtDrawRectOnCanvas.left = (-1) * m_rtDisplayedROIOnCanvas.left;
	}

	m_rtDrawRectOnCanvas.right = nCanvasWidth + m_rtDrawRectOnCanvas.left;

	INT_PTR nTopSpare = m_rtDisplayedROIOnCanvas.top;
	INT_PTR nBottomSpare = nCanvasHeight - m_rtDisplayedROIOnCanvas.Height();

	m_rtDrawRectOnCanvas.top = m_rtDisplayedROIOnCanvas.top - nBottomSpare;
	m_rtDrawRectOnCanvas.bottom = m_rtDisplayedROIOnCanvas.Height() + nTopSpare;

	double dDrawCanvasWidth = BytesPerLine((UINT)(m_rtDrawRectOnCanvas.Width()), 8);
	double dDrawCanvasHeight = BytesPerLine((UINT)(m_rtDrawRectOnCanvas.Height()), 8);
	
	m_rtDrawRectOnCanvas.right = m_rtDrawRectOnCanvas.left + dDrawCanvasWidth;
	m_rtDrawRectOnCanvas.bottom = m_rtDrawRectOnCanvas.top + dDrawCanvasHeight;

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

	// panning
	double dPannedDeltaX = -1 * (double)m_ptPanDelta.x / m_dZoomValue;
	double dPannedDeltaY = (double)m_ptPanDelta.y / m_dZoomValue;

	m_rtImage.OffsetRect((int)(dPannedDeltaX + 0.5f), (int)(dPannedDeltaY + 0.5f));
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

BOOL CStudyViewer::AllocDispRoiBuffer()
{
	BOOL bRes = TRUE;

	INT_PTR nSceenSize = m_rtDisplayedROIOnCanvas.Width() * m_rtDisplayedROIOnCanvas.Height();

	if (nSceenSize <= 0)
		return FALSE;

	m_pDispRoiImage = new BYTE[nSceenSize];

	if (!m_pDispRoiImage)
	{
		return FALSE;
	}

	m_stInterpolatedImg.pImage = m_pDispRoiImage;

	memset(m_pDispRoiImage, 0, nSceenSize);

	return bRes;
}

BOOL CStudyViewer::AllocRoiBuffer()
{
	BOOL bRes = TRUE;

	INT_PTR nRoiSize = m_rtDisplayedROIOnImage.Width() * m_rtDisplayedROIOnImage.Height();

	if (nRoiSize <= 0)
		return FALSE;

	m_pRoiImage = new BYTE[nRoiSize];

	if (!m_pRoiImage)
	{
		return FALSE;
	}

	m_stROIImg.Init();
	m_stROIImg.nChannel = 1;
	m_stROIImg.nWidth = m_rtDisplayedROIOnImage.Width();
	m_stROIImg.nHeight = m_rtDisplayedROIOnImage.Height();
	m_stROIImg.pImage = m_pRoiImage;

	memset(m_pRoiImage, 0, nRoiSize);
}

void CStudyViewer::FreeDisplayImage()
{
	if (m_pDisplayImage)
	{
		delete[] m_pDisplayImage;
		m_pDisplayImage = nullptr;
	}
}

void CStudyViewer::FreeDispRoiBuffer()
{
	if (m_pDispRoiImage)
	{
		delete[] m_pDispRoiImage;
		m_pDispRoiImage = nullptr;
	}
}

void CStudyViewer::FreeRoiBuffer()
{
	if (m_pRoiImage)
	{
		delete[] m_pRoiImage;
		m_pRoiImage = nullptr;
	}
}

// void CStudyViewer::UpdateScreenData()
// {
// 	if (!m_pDisplayImage)
// 		return;
// 
// 	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
// 
// 	CDicomImage imageDisplayInfo;	// different with DICOM information
// 	imageDisplayInfo = m_pDisplayDicomDS->m_aryDicomImage.GetAt(m_nCurFrameIndex);
// 
// 	INT_PTR nOrgWidth = BytesPerLine((UINT)imageDisplayInfo.m_stImageInfo.m_nWidth, 8) ;
// 	INT_PTR nOrgHeight = BytesPerLine((UINT)imageDisplayInfo.m_stImageInfo.m_nHeight, 8);
// 
// 	if (!CalcDisplayImageROI(&imageDisplayInfo))
// 	{
// 		return;
// 	}
// 
// 	if (!CalcDisplayCanvasROI(&imageDisplayInfo))
// 	{
// 		return;
// 	}
// 
// 	if (!CalcDrawRectOnCanvasRect(&imageDisplayInfo))
// 	{
// 		return;
// 	}
// 	
// 
// 	if (m_rtDisplayedROIOnImage.Width() <= 0 || m_rtDisplayedROIOnImage.Height() <= 0)
// 	{
// 		m_pDispRoiImage = nullptr;
// 	}
// 
// 	FreeRoiBuffer();
// 	AllocRoiBuffer();
// 
// 	CopyROIImageFromOrigin(m_pDisplayImage, m_pRoiImage,
// 		nOrgWidth, nOrgHeight,
// 		m_rtDisplayedROIOnImage.Width(), m_rtDisplayedROIOnImage.Height());
// 
// 	m_stROIImg.Init();
// 	m_stROIImg.nChannel = 1;
// 	m_stROIImg.nWidth = m_rtDisplayedROIOnImage.Width();
// 	m_stROIImg.nHeight = m_rtDisplayedROIOnImage.Height();
// 	m_stROIImg.pImage = m_pRoiImage;
// 
// 	FreeDispRoiBuffer();
// 	AllocDispRoiBuffer();
// 
// 	if (!m_pDispRoiImage)
// 		return;
// 
// 	clock_t clockStart, clockEnd;
// 	clockStart = clock();
// 
// 	CMyInterpolation myInter;
// 	myInter.SetUseParallelCalc(pMainFrm->IsUsingParallelCalc());
// 
// 	switch (m_eInterpolationType)
// 	{
// 	case InterpolationTypeBicubicPolynomial_050:
// 		myInter.DoBiCubicInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			FALSE,
// 			-0.5);
// 		break;
// 	case InterpolationTypeBicubicPolynomial_075:
// 		myInter.DoBiCubicInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			FALSE,
// 			-0.75);
// 		break;
// 	case InterpolationTypeBicubicPolynomial_100:
// 		myInter.DoBiCubicInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			FALSE,
// 			-1.0);
// 
// 		break;
// 	case InterpolationTypeBicubicPolynomial_000:
// 		myInter.DoBiCubicInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			FALSE,
// 			0.0);
// 		break;
// 	case InterpolationTypeBicubicPolynomial_300:
// 		myInter.DoBiCubicInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			FALSE,
// 			-3.0);
// 		break;
// 	case InterpolationTypeBicubicBSpline:
// 		myInter.DoBiCubicInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			TRUE);
// 		
// 		break;
// 	case InterpolationTypeBicubicLanczos:
// 		myInter.DoLanczosInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			2.0);
// 		
// 		break;
// 	case InterpolationTypeBicubicMichell:
// 		myInter.DoMitchellInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height(),
// 			0.33333,
// 			0.33333);
// 		
// 		break;
// 	case InterpolationTypeBicubicCatmullRom:
// 		myInter.DoCatmullRomSplineInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height());
// 		
// 		break;
// 	case InterpolationTypeBilinear:
// 	default:
// 		myInter.DoBilinearInterpolation(m_pRoiImage,
// 			m_pDispRoiImage,
// 			m_rtDisplayedROIOnImage.Width(),
// 			m_rtDisplayedROIOnImage.Height(),
// 			m_rtDisplayedROIOnCanvas.Width(),
// 			m_rtDisplayedROIOnCanvas.Height());
// 		
// 	
// 		break;
// 	}
// 
// 	clockEnd = clock();
// 
// 	CString strTimeStamp;
// 	float fResultTime = (float)(clockEnd - clockStart) / 1000;
// 
// 	strTimeStamp.Format(_T("Calc time : %0.2f sec"), fResultTime);
// 	pMainFrm->SetStatusBarText(3, strTimeStamp);
// 
// 	return;
// }

void CStudyViewer::CreateDragDropObjects()
{

}

// void CStudyViewer::CopyROIImageFromOrigin(BYTE* pSrc, BYTE* pDest, INT_PTR nSrcWidth, INT_PTR nSrcHeight, INT_PTR nROIWidth, INT_PTR nROIHeight)
// {
// 	UINT nLine = 0, nTotalSize = 0;
// 	UINT nDestLine, nSrcLine;
// 	nTotalSize = nROIHeight;
// 	nDestLine = nROIWidth;
// 	nSrcLine = (UINT)nSrcWidth;
// 
// 	pSrc += m_rtDisplayedROIOnImage.top * nSrcWidth;
// 	pSrc += m_rtDisplayedROIOnImage.left;
// 	
// 	for (nLine = 0; nLine < nTotalSize; nLine++, pDest += nDestLine, pSrc += nSrcLine)
// 	{
// 		memcpy_s(pDest, nDestLine, pSrc, nROIWidth);
// 	}
// }
// 
// BOOL CStudyViewer::DoInterpolate(BYTE * pSrcImage, BYTE * pDestImage, UINT nSrcWidth, UINT nSrcHeight, UINT nDestWidth, UINT nDestHeight)
// {
// 	UINT nrow, ncol, nIndex, nAllow1, nAllow2;
// 	double dEWweight, dNSweight, dEWtop, dEWbottom;
// 	INT_PTR nSrcCol, nSrcRow;
// 	double dDestCol, dDestRow;
// 	INT_PTR nSrcNextCol, nSrcNextRow;
// 	double dDestNextCol, dDestNextRow;
// 	double dWidthScale, dHeightScale;
// 	double dInWidth, dOutWidth, dInHeight, dOutHeight;
// 	UCHAR NW, NE, SW, SE;
// 	ULONG nAddress, nSrcImageLength;
// 	BYTE* pLineBuffer = NULL;
// 
// 	TRY
// 	{
// 		pLineBuffer = new BYTE[nDestWidth];	memset(pLineBuffer, 0, nDestWidth);
// 
// 		dInWidth = (double)nSrcWidth;	dOutWidth = (double)nDestWidth;
// 		dInHeight = (double)nSrcHeight;	dOutHeight = (double)nDestHeight;
// 
// 		dWidthScale = dOutWidth / dInWidth;
// 		dHeightScale = dOutHeight / dInHeight;
// 
// 		nAllow1 = (dWidthScale > 1) ? (UINT)dWidthScale + 1 : (2);
// 		nAllow2 = (dHeightScale > 1) ? (UINT)dHeightScale + 1 : (2);
// 
// 		nSrcImageLength = nDestWidth * nDestHeight;
// 
// 		for (nrow = 0; nrow < nDestHeight; nrow++)
// 		{
// 			nIndex = 0;
// 			for (ncol = 0; ncol < nDestWidth; ncol++)
// 			{
// 				dDestCol = (double)(ncol / dWidthScale);
// 				dDestRow = (double)(nrow / dHeightScale);
// 
// 				nSrcCol = (int)dDestCol;
// 				nSrcRow = (int)dDestRow;
// 
// 				dDestNextCol = dDestCol + 1;
// 				dDestNextRow = dDestRow + 1;
// 
// 				if (dDestNextCol > (nSrcWidth-1))
// 					dDestNextCol = nSrcWidth-1;
// 
// 				if (dDestNextRow > (nSrcHeight-1))
// 					dDestNextRow = nSrcHeight-1;
// 
// 				nSrcNextCol = (int)dDestNextCol;
// 				nSrcNextRow = (int)dDestNextRow;
// 
// 				dEWweight = dDestCol - (int)nSrcCol;
// 				dNSweight = dDestRow - (int)nSrcRow;
// 
// 				if (ncol >= (nDestWidth - nAllow1) || nrow >= (nDestHeight - nAllow2))
// 				{
// 					nAddress = (int)nSrcRow     * nSrcWidth + (int)nSrcCol;
// 					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
// 					NW = (UCHAR)*(pSrcImage + nAddress);
// 
// 					nAddress = (int)nSrcRow     * nSrcWidth + ((int)nSrcNextCol);
// 					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
// 					NE = (UCHAR)*(pSrcImage + nAddress);
// 
// 					nAddress = ((int)nSrcNextRow) * nSrcWidth + (int)nSrcCol;
// 					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
// 					SW = (UCHAR)*(pSrcImage + nAddress);
// 
// 					nAddress = ((int)nSrcNextRow) * nSrcWidth + ((int)nSrcNextCol);
// 					nAddress = (0 < nAddress && nAddress < nSrcImageLength) ? nAddress : 0;
// 					SE = (UCHAR)*(pSrcImage + nAddress);
// 				}
// 				else
// 				{
// 					NW = (UCHAR)*(pSrcImage + (int)nSrcRow     * nSrcWidth + (int)nSrcCol);
// 					NE = (UCHAR)*(pSrcImage + (int)nSrcRow     * nSrcWidth + (int)nSrcNextCol);
// 					SW = (UCHAR)*(pSrcImage + ((int)nSrcNextRow) * nSrcWidth + (int)nSrcCol);
// 					SE = (UCHAR)*(pSrcImage + ((int)nSrcNextRow) * nSrcWidth + (int)nSrcNextCol);
// 				}
// 
// 				dEWtop = NW + dEWweight * (NE - NW);
// 				dEWbottom = SW + dEWweight * (SE - SW);
// 
// 				*(pLineBuffer + nIndex) = (BYTE)(dEWtop + dNSweight * (dEWbottom - dEWtop));
// 				nIndex++;
// 			}
// 			memcpy(pDestImage + nrow * nDestWidth, pLineBuffer, nDestWidth);
// 		}
// 
// 		delete[] pLineBuffer;
// 	}
// 	CATCH_ALL(e)
// 	{
// 		if (e->IsKindOf(RUNTIME_CLASS(CUserException)))
// 		{
// 			return 0L;
// 		}
// 		else
// 		{
// 			return (LRESULT)e;
// 		}
// 	}
// 	END_CATCH_ALL
// 
// 	return TRUE;
// }





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

// 	if (bIsShiftKeyPressed)
// 	{
// 		ChangeSeriesImageByWheel(zDelta);
// 	}
// 	else
// 	{
// 		ChangeInstanceImageByWheel(zDelta);
// 	}

	if (bIsCtrlKeyPressed)
	{
		if(zDelta > 0)
		{
			ZoomIn(bIsShiftKeyPressed, zDelta);
		}
		else if (zDelta < 0)
		{
			ZoomOut(bIsShiftKeyPressed, zDelta);
		}
	}
	else
	{
		if (bIsShiftKeyPressed)
		{
			ChangeSeriesImageByWheel(zDelta);
		}
		else
		{
			ChangeInstanceImageByWheel(zDelta);
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

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}

	CWnd::OnLButtonDown(nFlags, point);
}


void CStudyViewer::OnDestroy()
{
	FreeDisplayImage();
	FreeDispRoiBuffer();
	FreeRoiBuffer();

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


