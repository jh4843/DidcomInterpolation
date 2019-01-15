#include "StdAfx.h"
#include "MyVisualManagerWindows7Ex.h"
#include "MyRibbonBar.h"
#include "MyRibbonPanel.h"
#include "MyGlobalData.h"
#include <afxcustomizebutton.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMyVisualManagerWindows7Ex, CMFCVisualManagerWindows7)

CMyVisualManagerWindows7Ex::CMyVisualManagerWindows7Ex()
{
	((MY_GLOBAL_DATA*)&afxGlobalData)->UpdateSysColors();
}


CMyVisualManagerWindows7Ex::~CMyVisualManagerWindows7Ex()
{
}


CFont* CMyVisualManagerWindows7Ex::GetAppCaptionFont()
{
	return &m_AppCaptionFont;
}

void CMyVisualManagerWindows7Ex::FillReBarPane(CDC* pDC, CBasePane* pBar, CRect rectClient)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pBar);

	if (/*m_pfDrawThemeBackground == NULL || */m_hThemeRebar == nullptr)
	{
		pDC->FillRect(rectClient, &GetGlobalData()->brBarFace);
		return;
	}

	CWnd* pWndParent = AFXGetParentFrame(pBar);
	if (pWndParent->GetSafeHwnd() == nullptr)
	{
		pWndParent = pBar->GetParent();
	}

	ASSERT_VALID(pWndParent);

	CRect rectParent;
	pWndParent->GetWindowRect(rectParent);

	pBar->ScreenToClient(&rectParent);

	rectClient.right = max(rectClient.right, rectParent.right);
	rectClient.bottom = max(rectClient.bottom, rectParent.bottom);

	if (!pBar->IsFloating() && pBar->GetParentMiniFrame() == nullptr)
	{
		rectClient.left = rectParent.left;
		rectClient.top = rectParent.top;

		if (!pBar->IsKindOf(RUNTIME_CLASS(CDockSite)))
		{
			CFrameWnd* pMainFrame = AFXGetTopLevelFrame(pWndParent);
			if (pMainFrame->GetSafeHwnd() != nullptr)
			{
				CRect rectMain;
				pMainFrame->GetClientRect(rectMain);
				pMainFrame->MapWindowPoints(pBar, &rectMain);

				rectClient.top = rectMain.top;
			}
		}
	}

// 	if (m_pfDrawThemeBackground != nullptr)
// 	{
// 		(*m_pfDrawThemeBackground)(m_hThemeRebar, pDC->GetSafeHdc(), 0, 0, &rectClient, 0);
// 	}
	pDC->FillSolidRect(&rectClient, GetGlobalData()->clrBarFace/*RGB(255,0,0)*/);
}

BOOL CMyVisualManagerWindows7Ex::DrawCheckBox(CDC *pDC, CRect rect, BOOL bHighlighted, int nState, BOOL bEnabled, BOOL bPressed)
{
	if (m_hThemeButton == nullptr)
	{
		return FALSE;
	}

	nState = max(0, nState);
	nState = min(2, nState);

	ASSERT_VALID(pDC);

 	int nDrawState = nState == 1 ? CBS_CHECKEDNORMAL : nState == 2 ? CBS_MIXEDNORMAL : CBS_UNCHECKEDNORMAL;
 
	if (!bEnabled)
	{
		nDrawState = nState == 1 ? CBS_CHECKEDDISABLED : nState == 2 ? CBS_MIXEDDISABLED : CBS_UNCHECKEDDISABLED;
	}
	else if (bPressed)
	{
		nDrawState = nState == 1 ? CBS_CHECKEDPRESSED : nState == 2 ? CBS_MIXEDPRESSED : CBS_UNCHECKEDPRESSED;
	}
	else if (bHighlighted)
	{
		nDrawState = nState == 1 ? CBS_CHECKEDHOT : nState == 2 ? CBS_MIXEDHOT : CBS_UNCHECKEDHOT;
	}

// 	if (m_pfDrawThemeBackground != nullptr)
// 	{
// 		(*m_pfDrawThemeBackground)(m_hThemeButton, pDC->GetSafeHdc(), BP_CHECKBOX, nDrawState, &rect, 0);
// 	}

	rect.DeflateRect(1,1);


	if (bEnabled)
	{
		pDC->FillSolidRect(rect, RGB(13,13,13));
		rect.DeflateRect(3,3);

//		TRACE(L"%d\n", nDrawState);

		switch (nDrawState)
		{
		case 1:		// no checked
			{
				
			}
			break;
		case 2:		// highlighted
			{
			}
			break;
		case 5:		// checked only
		case 6:		// checked and highlighted
			{
				pDC->FillSolidRect(rect, RGB(120,120,120));
			}
			break;
		case 7:
			{

			}
			break;
		}
	}
	else
	{
		pDC->FillSolidRect(rect, RGB(82,82,82));
	}

	return TRUE;
}

void CMyVisualManagerWindows7Ex::DrawNcBtn(CDC* pDC, const CRect& rect, UINT nButton, AFX_BUTTON_STATE state, BOOL bSmall, BOOL bActive, BOOL bMDI/* = FALSE*/)
{
	ASSERT_VALID(pDC);

//	if (m_hThemeWindow == nullptr)
	{
		UINT nState = 0;

		switch (nButton)
		{
		case SC_CLOSE:
			nState = DFCS_CAPTIONCLOSE;
			break;

		case SC_MINIMIZE:
			nState = DFCS_CAPTIONMIN;
			break;

		case SC_MAXIMIZE:
			nState = DFCS_CAPTIONMAX;
			break;

		case SC_RESTORE:
			nState = DFCS_CAPTIONRESTORE;
			break;

		case SC_CONTEXTHELP:
			nState = DFCS_CAPTIONHELP;
			break;

		default:
			return;
		}

		if (!bActive)
		{
			nState |= DFCS_INACTIVE;
		}

		if (state != ButtonsIsRegular)
		{
			nState |= state == ButtonsIsHighlighted ? DFCS_HOT : DFCS_PUSHED;
		}


		CRect rt(rect);
//		pDC->FillSolidRect(&rt, RGB(255,0,0));

//		pDC->DrawFrameControl(rt, DFC_CAPTION, nState);
//		COLORREF crBk = RGB(40,40,40);
		COLORREF crBk = RGB(0,0,0);
		//
		
		Gdiplus::Color crColor = Gdiplus::Color::DarkGray;
		
		//
		if ((nState & DFCS_HOT) == DFCS_HOT)
		{
			crBk = RGB(33,33,33);
			crColor = Gdiplus::Color::White;

			if (nButton == SC_CLOSE)
			{
				crBk = RGB(191,30,46);
			}
		}
		else if ((nState & DFCS_PUSHED) == DFCS_PUSHED)
		{
			crBk = RGB(46,46,46);
			crColor = Gdiplus::Color::White;
		}

		pDC->FillSolidRect(&rt, crBk);

		//
		Gdiplus::Graphics graphics(pDC->GetSafeHdc());
		graphics.SetSmoothingMode(SmoothingModeHighQuality);

		Gdiplus::RectF rcClient;
		rcClient.X = (float)rt.left;
		rcClient.Y = (float)rt.top;
		rcClient.Width = (float)rt.Width();
		rcClient.Height = (float)rt.Height();
		//
		float fDeflate = 4.0f;
		Gdiplus::RectF rcWindow = rcClient;
		rcWindow.Width -= fDeflate;
		rcWindow.Height -= fDeflate;
		rcWindow.X += fDeflate/2.0f;
		rcWindow.Y += fDeflate/2.0f;
		//
		float fDivWidthUnit = rcClient.Width / 10.0f;
		float fDivHeightUnit = rcClient.Height / 10.0f;
		//
		
		float fPenThick = 2.0f;
		Gdiplus::Pen penLine(crColor, fPenThick);
		
		// remove some flags
		nState &= ~DFCS_HOT;
		nState &= ~DFCS_PUSHED;

		switch (nState)
		{
		case DFCS_CAPTIONCLOSE:
			{
				float fSize = fDivWidthUnit*3.5f;
				rcWindow.Width -= fSize;
				rcWindow.Height -= fSize;
				rcWindow.X += fSize/2.0f;
				rcWindow.Y += fSize/2.0f;

				//
				Gdiplus::PointF ptStart(rcWindow.X, rcWindow.Y);
				Gdiplus::PointF ptEnd(rcWindow.X+rcWindow.Width, rcWindow.Y+rcWindow.Height);
				//
				graphics.DrawLine(&penLine, ptStart, ptEnd);

				//
				ptStart.X = rcWindow.X;
				ptStart.Y = rcWindow.Y+rcWindow.Height;
				//
				ptEnd.X = rcWindow.X+rcWindow.Width;
				ptEnd.Y = rcWindow.Y;
				//
				graphics.DrawLine(&penLine, ptStart, ptEnd);
			}
			break;
		case DFCS_CAPTIONMIN:
			{
				float fWidthGap = fDivWidthUnit*1.5f;
				float fHeightGap = fDivHeightUnit*2.0f;
				Gdiplus::PointF ptStart(rcWindow.X+fWidthGap, rcWindow.Y+rcWindow.Height-fHeightGap);
				Gdiplus::PointF ptEnd(rcWindow.X+rcWindow.Width-fWidthGap, ptStart.Y);
				//
				graphics.DrawLine(&penLine, ptStart, ptEnd);
			}
			break;
		case DFCS_CAPTIONMAX:
			{
				float fSize = fDivWidthUnit*3.5f;
				rcWindow.Width -= fSize;
				rcWindow.Height -= fSize;
				rcWindow.X += fSize/2.0f;
				rcWindow.Y += fSize/2.0f;
				//
				//
				graphics.DrawRectangle(&penLine, rcWindow);

				//
				Gdiplus::SolidBrush brush(crColor);
				Gdiplus::RectF rcNew = rcWindow;
				rcNew.Height /= 3.0f;
				graphics.FillRectangle(&brush, rcNew);
			}
			break;
		case DFCS_CAPTIONRESTORE:
			{
				float fSize = fDivWidthUnit*3.5f;
				rcWindow.Width -= fSize;
				rcWindow.Height -= fSize;
				rcWindow.X += fSize/2.0f;
				rcWindow.Y += fSize/2.0f;
				//
//				rcWindow.Offset(fSize/2.0f, fSize/2.0f);
				graphics.DrawRectangle(&penLine, rcWindow);

  				rcWindow.X += fSize/2.0f;
  				rcWindow.Y += fSize/2.0f;

				// Draw black
				Gdiplus::SolidBrush brush2(Gdiplus::Color::Black);
				Gdiplus::RectF rcNew2 = rcWindow;
				//
				graphics.FillRectangle(&brush2, rcNew2);
				 
				// outline
				graphics.DrawRectangle(&penLine, rcWindow);

				//
				Gdiplus::SolidBrush brush(crColor);
				Gdiplus::RectF rcNew = rcWindow;
				rcNew.Height /= 3.0f;
				graphics.FillRectangle(&brush, rcNew);
			}
			break;
		}
		//
		return;
	}

// 	int nPart = 0;
// 	int nState = 0;
// 	if (nButton == SC_CLOSE)
// 	{
// 		if (bMDI)
// 		{
// 			nPart = WP_MDICLOSEBUTTON;
// 		}
// 		else
// 		{
// 			nPart = bSmall ? WP_SMALLCLOSEBUTTON : WP_CLOSEBUTTON;
// 		}
// 
// 		nState = bActive ? CBS_NORMAL : CBS_DISABLED;
// 		if (state != ButtonsIsRegular)
// 		{
// 			nState = state == ButtonsIsHighlighted ? CBS_HOT : CBS_PUSHED;
// 		}
// 	}
// 	else if (nButton == SC_MINIMIZE)
// 	{
// 		if (bMDI)
// 		{
// 			nPart = WP_MDIMINBUTTON;
// 		}
// 		else if (!bSmall)
// 		{
// 			nPart = WP_MINBUTTON;
// 		}
// 
// 		nState = bActive ? MINBS_NORMAL : MINBS_DISABLED;
// 		if (state != ButtonsIsRegular)
// 		{
// 			nState = state == ButtonsIsHighlighted ? MINBS_HOT : MINBS_PUSHED;
// 		}
// 	}
// 	else if (nButton == SC_MAXIMIZE)
// 	{
// 		if (!bMDI && !bSmall)
// 		{
// 			nPart = WP_MAXBUTTON;
// 		}
// 
// 		nState = bActive ? MAXBS_NORMAL : MAXBS_DISABLED;
// 		if (state != ButtonsIsRegular)
// 		{
// 			nState = state == ButtonsIsHighlighted ? MAXBS_HOT : MAXBS_PUSHED;
// 		}
// 	}
// 	else if (nButton == SC_RESTORE)
// 	{
// 		if (bMDI)
// 		{
// 			nPart = WP_MDIRESTOREBUTTON;
// 		}
// 		else
// 		{
// 			nPart = WP_RESTOREBUTTON;
// 		}
// 
// 		nState = bActive ? RBS_NORMAL : RBS_DISABLED;
// 		if (state != ButtonsIsRegular)
// 		{
// 			nState = state == ButtonsIsHighlighted ? RBS_HOT : RBS_PUSHED;
// 		}
// 	}
// 	else if (nButton == SC_CONTEXTHELP)
// 	{
// 		if (bMDI)
// 		{
// 			nPart = WP_MDIHELPBUTTON;
// 		}
// 		else if (!bSmall)
// 		{
// 			nPart = WP_HELPBUTTON;
// 		}
// 
// 		nState = bActive ? HBS_NORMAL : HBS_DISABLED;
// 		if (state != ButtonsIsRegular)
// 		{
// 			nState = state == ButtonsIsHighlighted ? HBS_HOT : HBS_PUSHED;
// 		}
// 	}
// 
// 	if (nPart == 0)
// 	{
// 		return;
// 	}
// 
// 	(*m_pfDrawThemeBackground)(m_hThemeWindow, pDC->GetSafeHdc(), nPart, nState, &rect, 0);
}

COLORREF CMyVisualManagerWindows7Ex::OnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pTab);

	if (!CanDrawImage())
	{
		return CMFCVisualManagerWindows::OnDrawRibbonCategoryTab (pDC, pTab, bIsActive);
	}

	CMFCRibbonCategory* pCategory = pTab->GetParentCategory();
	ASSERT_VALID(pCategory);
	CMFCRibbonBar* pBar = pCategory->GetParentRibbonBar();
	ASSERT_VALID(pBar);

	bIsActive = bIsActive && ((pBar->GetHideFlags() & AFX_RIBBONBAR_HIDE_ELEMENTS) == 0 || pTab->GetDroppedDown() != nullptr);

	const BOOL bPressed       = pTab->IsPressed();
	const BOOL bIsFocused	  = pTab->IsFocused() &&(pBar->GetHideFlags() & AFX_RIBBONBAR_HIDE_ELEMENTS);
	const BOOL bIsHighlighted = (pTab->IsHighlighted() || bIsFocused) && !pTab->IsDroppedDown();

	CRect rectTab(pTab->GetRect());
	rectTab.bottom++;

	int ratio = 0;
	if (m_ctrlRibbonCategoryTabSep.IsValid())
	{
		ratio = pBar->GetTabTruncateRatio();
	}

	if (ratio > 0)
	{
		rectTab.left++;
	}

	int nImage = bIsActive ? 3 : 0;

	if (bPressed)
	{
		if (bIsHighlighted)
		{
			nImage = bIsActive ? 2 : 1;
		}
	}
	
	if(bIsHighlighted)
	{
		nImage += 1;
	}

	CMFCControlRenderer* pRenderer = &m_ctrlRibbonCategoryTab;
/*
	COLORREF clrText = m_clrRibbonCategoryText;
	COLORREF clrTextHighlighted = m_clrRibbonCategoryTextHighlighted;

	if (pCategory->GetTabColor() != AFX_CategoryColor_None || pTab->IsSelected())
	{
		CMFCRibbonContextCategory& context = m_ctrlRibbonContextCategory[(pTab->IsSelected() || nImage == 4) ? AFX_CategoryColor_Orange - 1 : pCategory->GetTabColor() - 1];
		pRenderer = &context.m_ctrlTab;
		clrText  = context.m_clrText;
		clrTextHighlighted = context.m_clrTextHighlighted;
	}
*/
	pRenderer->Draw(pDC, rectTab, nImage);

	if (ratio > 0)
	{
		CRect rectSep(rectTab);
		rectSep.left = rectSep.right;
		rectSep.right++;
		rectSep.bottom--;

		m_ctrlRibbonCategoryTabSep.Draw(pDC, rectSep, 0, (BYTE)min(ratio * 255 / 100, 255));
	}

	if (bIsActive || bIsHighlighted)
	{
		return GetGlobalData()->clrBarText;
	}
	
	//
	return GetGlobalData()->clrInactiveCaptionText;
}

COLORREF CMyVisualManagerWindows7Ex::OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel, CRect rectCaption)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerWindows::OnDrawRibbonPanel(pDC, pPanel, rectPanel, rectCaption);
	}

	ASSERT_VALID (pDC);
	ASSERT_VALID (pPanel);

	if (pPanel->IsKindOf(RUNTIME_CLASS(CMFCRibbonMainPanel)))
	{
		const int nBorderSize = GetPopupMenuBorderSize();
		rectPanel.InflateRect(nBorderSize, nBorderSize);

		m_ctrlRibbonMainPanel.Draw(pDC, rectPanel);
	}
	else if (pPanel->IsKindOf(RUNTIME_CLASS(CMyRibbonPanel)))
	{
		CMyRibbonPanel* pMyPanel = (CMyRibbonPanel*)pPanel;
		if (!pMyPanel->IsMenuMode() && !pMyPanel->IsPanelCollapsed())
		{
			int nWidth = m_ctrlRibbonPanelBackSep.GetParams().m_rectImage.Width();
			int nWidth2 = nWidth / 2;

			rectPanel.left = rectPanel.right - nWidth2;
			rectPanel.right += (nWidth - nWidth2);
			rectPanel.DeflateRect(0, 4);

			m_ctrlRibbonPanelBackSep.Draw(pDC, rectPanel);
		}
	}
	else
	{
		if (!pPanel->IsMenuMode () && !pPanel->IsCollapsed())
		{
			int nWidth = m_ctrlRibbonPanelBackSep.GetParams().m_rectImage.Width();
			int nWidth2 = nWidth / 2;

			rectPanel.left = rectPanel.right - nWidth2;
			rectPanel.right += (nWidth - nWidth2);
			rectPanel.DeflateRect(0, 4);

			m_ctrlRibbonPanelBackSep.Draw(pDC, rectPanel);
		}
	}

	return GetGlobalData()->clrBarText;
}

void CMyVisualManagerWindows7Ex::OnDrawRibbonDefaultPaneButtonContext(CDC* pDC, CMFCRibbonButton* pButton)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pButton);

	CRect rectMenuArrow = pButton->GetRect();

	INT_PTR nWidthMargin = 0;
	INT_PTR nHeightMargin = 0;

	CMFCRibbonButton::RibbonImageType eImageType = CMFCRibbonButton::RibbonImageSmall;
	if (theParam.GetParam()->app.bUseLargeRibbonIcon)
	{
		eImageType = CMFCRibbonButton::RibbonImageLarge;
	}

	if (pButton->IsQATMode())
	{
		pButton->DrawImage(pDC, eImageType, pButton->GetRect());
		return;
	}

	CRect rectImage = pButton->GetRect();

	const int nMarginX = 11;
	const int nMarginY = 10;

	rectImage.top += nMarginY / 2;
	rectImage.bottom = rectImage.top + pButton->GetImageSize(eImageType).cy + 2 * nMarginY;
	rectImage.top -= 2;
	rectImage.left = rectImage.CenterPoint().x - pButton->GetImageSize(eImageType).cx / 2 - nMarginX;
	rectImage.right = rectImage.left + pButton->GetImageSize(eImageType).cx + 2 * nMarginX;

	rectImage.left += m_ctrlRibbonBtnDefaultIcon.GetParams().m_rectSides.left;
	rectImage.top += m_ctrlRibbonBtnDefaultIcon.GetParams().m_rectSides.top;
	rectImage.right -= m_ctrlRibbonBtnDefaultIcon.GetParams().m_rectSides.right;
	rectImage.bottom -= m_ctrlRibbonBtnDefaultIcon.GetParams().m_rectSides.bottom;

// 	rectImage.top += 10;
// 	rectImage.bottom = rectImage.top + pButton->GetImageSize(eImageType).cy;
// 	rectImage.bottom += m_ctrlRibbonBtnDefaultIcon.GetParams().m_rectSides.bottom;

	pButton->DrawImage(pDC, eImageType, rectImage);

	// Draw text:
// 	pButton->DrawBottomText(pDC, FALSE);
// 	return;

	CMyRibbonDefaultPanelButton* pDefButton = dynamic_cast<CMyRibbonDefaultPanelButton*>(pButton);
	ASSERT_VALID(pDefButton);

	CString m_strText = pDefButton->GetText();
	if (m_strText.IsEmpty())
	{
		return;
	}
 
	const CSize sizeImageLarge = pDefButton->GetParent()->GetImageSize(TRUE);
	if (sizeImageLarge == CSize(0, 0))
	{
		return;
	}
 
 	CSize sizeText = pDC->GetTextExtent(m_strText);
 
 	const int nTextLineHeight = sizeText.cy;
 	int nMenuArrowWidth = CMenuImages::Size().cx;
 
 	if (nMenuArrowWidth != NULL && GetGlobalData()->GetRibbonImageScale() > 1.)
 	{
 		nMenuArrowWidth = (int)(.5 + GetGlobalData()->GetRibbonImageScale() * nMenuArrowWidth);
 	}
 
 	int y = pDefButton->GetRect().top + 1/*nLargeButtonMarginY */+ sizeImageLarge.cy + 5;
 	CRect rectMenuArrowNew(0, 0, 0, 0);
 
 	//
 	y += 2/*nDefaultPaneButtonMargin*/;
 
 	CRect rectText = pDefButton->GetRect();
 	rectText.top = y;
 
 	UINT uiDTFlags = DT_SINGLELINE | DT_CENTER;
 	uiDTFlags |= DT_NOPREFIX | DT_END_ELLIPSIS;
	// Single line text
	pDC->DrawText(m_strText, rectText, uiDTFlags);

	//if (HasMenu() || IsDefaultPanelButton())
	{
		rectMenuArrowNew = pDefButton->GetRect();

		rectMenuArrowNew.top = y + nTextLineHeight + 2;
		rectMenuArrowNew.left = pDefButton->GetRect().CenterPoint().x - CMenuImages::Size().cx / 2 - 1;
	}

	if (!rectMenuArrowNew.IsRectEmpty())
	{
		int nMenuArrowHeight = CMenuImages::Size().cy;

		rectMenuArrowNew.bottom = rectMenuArrowNew.top + nMenuArrowHeight;
		rectMenuArrowNew.right = rectMenuArrowNew.left + nMenuArrowWidth;

		CRect rectWhite = rectMenuArrowNew;
		rectWhite.OffsetRect(0, 1);

		CMenuImages::IMAGES_IDS id = GetGlobalData()->GetRibbonImageScale() > 1. ? CMenuImages::IdArrowDownLarge : CMenuImages::IdArrowDown;

		CMenuImages::Draw(pDC, id, rectWhite, CMenuImages::ImageWhite);
		CMenuImages::Draw(pDC, id, rectMenuArrowNew, pDefButton->IsDisabled() ? CMenuImages::ImageGray : CMenuImages::ImageBlack);
	}
}

void CMyVisualManagerWindows7Ex::OnDrawPaneDivider(CDC* pDC, CPaneDivider* pSlider, CRect rect, BOOL bAutoHideMode)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pSlider);

	CRect rectScreen = GetGlobalData()->m_rectVirtual;
	pSlider->ScreenToClient(&rectScreen);

	CRect rectFill = rect;
	rectFill.left = min(rectFill.left, rectScreen.left);

//	OnFillBarBackground(pDC, pSlider, rectFill, rect);
//	pDC->FillSolidRect(&rectFill, RGB(140,140,140));
	
	//
	pDC->FillSolidRect(&rectFill, RGB(0,0,0));
	//
	switch (pSlider->GetCurrentAlignment())
	{
	case CBRS_ALIGN_LEFT:
	case CBRS_ALIGN_RIGHT:
		{
			rectFill.DeflateRect(1, 0);
		}
		break;
	case CBRS_ALIGN_TOP:
	case CBRS_ALIGN_BOTTOM:
		{
			rectFill.DeflateRect(0, 1);
		}
		break;
	}
	
	pDC->FillSolidRect(&rectFill, RGB(33,33,33));



	if (bAutoHideMode)
	{
		// draw outer edge;

		DWORD dwAlgn = pSlider->GetCurrentAlignment();
		CRect rectBorder = rect;

		COLORREF clrBorder = GetGlobalData()->clrBarDkShadow;

		if (dwAlgn & CBRS_ALIGN_LEFT)
		{
			rectBorder.left = rectBorder.right;
		}
		else if (dwAlgn & CBRS_ALIGN_RIGHT)
		{
			rectBorder.right = rectBorder.left;
			clrBorder = GetGlobalData()->clrBarHilite;
		}
		else if (dwAlgn & CBRS_ALIGN_TOP)
		{
			rectBorder.top = rectBorder.bottom;
		}
		else if (dwAlgn & CBRS_ALIGN_BOTTOM)
		{
			rectBorder.bottom = rectBorder.top;
			clrBorder = GetGlobalData()->clrBarHilite;
		}
		else
		{
			ASSERT(FALSE);
			return;
		}

		pDC->Draw3dRect(rectBorder, clrBorder, clrBorder);
	}
}

void CMyVisualManagerWindows7Ex::OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnDrawMenuBorder(pDC, pMenu, rect);
		return;
	}

	if (pMenu != nullptr)
	{
		CMFCRibbonPanelMenuBar* pRibbonMenuBar = DYNAMIC_DOWNCAST(CMFCRibbonPanelMenuBar, pMenu->GetMenuBar());

		if (pRibbonMenuBar != nullptr)
		{
			ASSERT_VALID(pRibbonMenuBar);

			if (pRibbonMenuBar->IsMainPanel())
			{
				if (m_ctrlRibbonMainPanel.IsValid())
				{
					m_ctrlRibbonMainPanel.DrawFrame(pDC, rect);
				}

				return;
			}

			if (!pRibbonMenuBar->IsMenuMode())
			{
				if (pRibbonMenuBar->IsQATPopup() && m_ctrlRibbonBorder_QAT.IsValid())
				{
					m_ctrlRibbonBorder_QAT.DrawFrame(pDC, rect);
					return;
				}
				else if (pRibbonMenuBar->IsCategoryPopup())
				{
					return;
				}
				else if (pRibbonMenuBar->IsRibbonMiniToolBar() /*&& m_ctrlRibbonBorder_Floaty.IsValid()*/)
				{
/*
					m_ctrlRibbonBorder_Floaty.DrawFrame (pDC, rect);
					return;
*/
				}
				else
				{
					if (pRibbonMenuBar->GetPanel() != nullptr)
					{
						return;
					}
				}
			}
		}
	}


	//CMFCVisualManagerWindows::OnDrawMenuBorder(pDC, pMenu, rect);
// 	if (m_hThemeWindow == NULL || m_bOfficeStyleMenus)
// 	{
// 		ASSERT_VALID(pMenu);
// 
// 		BOOL bConnectMenuToParent = m_bConnectMenuToParent;
// 		m_bConnectMenuToParent = FALSE;
// 
// 		if (m_hThemeWindow == nullptr)
// 		{
// 			m_bConnectMenuToParent = TRUE;
// 		}
// 		else if (!CMFCToolBar::IsCustomizeMode())
// 		{
// 			CMFCToolBarMenuButton* pMenuButton = pMenu->GetParentButton();
// 
// 			if (pMenuButton != nullptr)
// 			{
// 				BOOL bIsMenuBar = pMenuButton->GetParentWnd() != NULL && pMenuButton->GetParentWnd()->IsKindOf(RUNTIME_CLASS(CMFCMenuBar));
// 				if (bIsMenuBar)
// 				{
// 					m_bConnectMenuToParent = TRUE;
// 				}
// 			}
// 		}
// 
// 		CMFCVisualManagerOfficeXP::OnDrawMenuBorder(pDC, pMenu, rect);
// 
// 		m_bConnectMenuToParent = bConnectMenuToParent;
// 	}
// 	else
	{
		//CMFCVisualManager::OnDrawMenuBorder(pDC, pMenu, rect);
// 		pDC->Draw3dRect(&rect, RGB(51,51,51), RGB(51,51,51));
// 		rect.DeflateRect(1, 1);
// 		pDC->Draw3dRect(&rect, RGB(58,58,58), RGB(58,58,58));
		//
		pDC->Draw3dRect(&rect, RGB(118,118,118), RGB(118,118,118));
 		rect.DeflateRect(1, 1);
		// for inside
		pDC->Draw3dRect(&rect, RGB(43,43,43), RGB(43,43,43));
	}
}

void CMyVisualManagerWindows7Ex::OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnFillButtonInterior(pDC, pButton, rect, state);
		return;
	}

	CMFCCustomizeButton* pCustButton = DYNAMIC_DOWNCAST(CMFCCustomizeButton, pButton);

	if (pCustButton == nullptr)
	{
		if (CMFCToolBar::IsCustomizeMode() && 
			!CMFCToolBar::IsAltCustomizeMode() && !pButton->IsLocked())
		{
			return;
		}

		CMFCControlRenderer* pRenderer = nullptr;
		int index = 0;

		BOOL bDisabled = (pButton->m_nStyle & TBBS_DISABLED) == TBBS_DISABLED;
		//BOOL bPressed  = (pButton->m_nStyle & TBBS_PRESSED ) == TBBS_PRESSED;
		BOOL bChecked  = (pButton->m_nStyle & TBBS_CHECKED ) == TBBS_CHECKED;

		CBasePane* pBar = DYNAMIC_DOWNCAST(CBasePane, pButton->GetParentWnd());

		CMFCToolBarMenuButton* pMenuButton = 
			DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, pButton);
		if (pMenuButton != NULL && pBar != nullptr)
		{
			if (pBar->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)) == NULL && pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
			{
				if (bChecked)
				{
					pRenderer = &m_ctrlMenuItemBack;

					if (bDisabled)
					{
						index = 1;
					}

					rect.InflateRect(0, 0, 0, 1);
				}
				else if (state == ButtonsIsPressed || state == ButtonsIsHighlighted)
				{
					pRenderer = &m_ctrlMenuHighlighted[bDisabled ? 1 : 0];
				}
				else
				{
					return;
				}
			}
		}

		if (pRenderer != nullptr)
		{
			pRenderer->Draw(pDC, rect, index);
			return;
		}
	}


	//CMFCVisualManagerWindows::OnFillButtonInterior(pDC, pButton, rect, state);
	if (m_hThemeToolBar == nullptr)
	{
		CMFCVisualManagerOfficeXP::OnFillButtonInterior(pDC, pButton, rect, state);
		return;
	}

	BOOL bIsMenuBar = FALSE;
	BOOL bIsPopupMenu = FALSE;

	CMFCToolBarMenuButton* pMenuButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, pButton);
	if (pMenuButton != nullptr)
	{
		bIsMenuBar = pMenuButton->GetParentWnd() != NULL && pMenuButton->GetParentWnd()->IsKindOf(RUNTIME_CLASS(CMFCMenuBar));
		bIsPopupMenu = pMenuButton->GetParentWnd() != NULL && pMenuButton->GetParentWnd()->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar));
	}

	if (m_bOfficeStyleMenus &&(bIsPopupMenu || bIsMenuBar))
	{
		CMFCVisualManagerOfficeXP::OnFillButtonInterior(pDC, pButton, rect, state);
		return;
	}

	if (bIsPopupMenu && state != ButtonsIsHighlighted && state != ButtonsIsPressed)
	{
		return;
	}

	int nState = TS_NORMAL;

	if (pButton->m_nStyle & TBBS_DISABLED)
	{
		nState = TS_DISABLED;
	}
	else if ((pButton->m_nStyle & TBBS_PRESSED) && state == ButtonsIsHighlighted)
	{
		nState = TS_PRESSED;
	}
	else if (pButton->m_nStyle & TBBS_CHECKED)
	{
		nState = (state == ButtonsIsHighlighted) ? TS_HOTCHECKED : TS_CHECKED;
	}
	else if (state == ButtonsIsHighlighted)
	{
		nState = TS_HOT;

		if (pMenuButton != NULL && pMenuButton->IsDroppedDown())
		{
			nState = TS_PRESSED;
			rect.bottom--;
		}
	}

// 	if (m_pfDrawThemeBackground != nullptr)
// 	{
// 		(*m_pfDrawThemeBackground)(m_hThemeToolBar, pDC->GetSafeHdc(), TP_BUTTON, nState, &rect, 0);
// 	}
	
	rect.DeflateRect(1,1);

	//
	COLORREF crBk = RGB(0,0,0);
	switch (nState)
	{
	case TS_HOT:
		{
			crBk = RGB(97,97,97);
		}
		break;
	case TS_DISABLED:
		{
			crBk = RGB(53,53,53);
		}
		break;
	case TS_CHECKED:
	case TS_HOTCHECKED:
		{
			crBk = RGB(127,127,127);
		}
		break;

	default:
		{
			return;
		}
		break;
	}

	pDC->FillSolidRect(&rect, crBk);
}

void CMyVisualManagerWindows7Ex::OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bHorz)
{
	ASSERT_VALID(pBar);
	ASSERT_VALID(pDC);

	CRect rectSeparator = rect;

	if (bHorz)
	{
		rectSeparator.left += rectSeparator.Width() / 2 - 1;
		rectSeparator.right = rectSeparator.left + 1;
	}
	else
	{
		rectSeparator.top += rectSeparator.Height() / 2 - 1;
		rectSeparator.bottom = rectSeparator.top + 1;
	}


// 	const COLORREF clrHilite = pBar->IsDialogControl() ? GetGlobalData()->clrBtnHilite : GetGlobalData()->clrBarHilite;
// 	const COLORREF clrShadow = pBar->IsDialogControl() ? GetGlobalData()->clrBtnShadow : GetGlobalData()->clrBarShadow;
// 	COLORREF clrHilite = RGB(83,83,83);
// 	COLORREF clrShadow = RGB(89,89,89);
	COLORREF clrHilite = RGB(30,30,30);
	COLORREF clrShadow = clrHilite;//RGB(33,33,33);


	pDC->Draw3dRect(rectSeparator, clrShadow, clrHilite);
}

void CMyVisualManagerWindows7Ex::OnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rectCaption, CRect rectText)
{
	//CMFCVisualManagerWindows7::OnDrawRibbonCaption(pDC, pBar, rectCaption, rectText);
	ASSERT_VALID(pDC);
	ASSERT_VALID(pBar);

	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnDrawRibbonCaption(pDC, pBar, rectCaption, rectText);
		return;
	}

	CWnd* pWnd = pBar->GetParent();
	ASSERT_VALID (pWnd);

	const DWORD dwStyleEx = pWnd->GetExStyle();

	const BOOL bIsRTL = (dwStyleEx & WS_EX_LAYOUTRTL) == WS_EX_LAYOUTRTL;
	/*const */BOOL bActive = IsWindowActive(pWnd);
	if (pBar->IsKindOf(RUNTIME_CLASS(CMyRibbonBar)))
	{
		bActive = static_cast<CMyRibbonBar*>(pBar)->IsParentWindowActivated();
	}
	//
	const BOOL bGlass = pBar->IsTransparentCaption();

	{
		CSize szSysBorder(GetSystemBorders(TRUE));

		if (!bGlass)
		{
			CRect rectCaption1(rectCaption);
			rectCaption1.InflateRect(szSysBorder.cx, szSysBorder.cy, szSysBorder.cx, 0);

			if (m_hThemeWindow != nullptr)
			{
//				(*m_pfDrawThemeBackground)(m_hThemeWindow, pDC->GetSafeHdc(), WP_CAPTION, bActive ? CS_ACTIVE : CS_INACTIVE, &rectCaption1, 0);
//				pDC->FillSolidRect(&rectCaption1, RGB(40,40,40));
				pDC->FillSolidRect(&rectCaption1, RGB(0,0,0));

				COLORREF crTitle = ::GetSysColor(COLOR_HIGHLIGHT);
				if (!bActive)
				{
					// fixme
					crTitle = ::GetSysColor(COLOR_WINDOWFRAME);
				}

				//
				CRect rcTopBorder = rectCaption1;
				rcTopBorder.top = 0;
				rcTopBorder.bottom = rcTopBorder.top + 1;
				//
				pDC->FillSolidRect(&rcTopBorder, crTitle);

				

				// set as false
				bActive = FALSE;
			}
			else
			{
				CDrawingManager dm(*pDC);
				dm.FillGradient(rectCaption1, bActive ? GetGlobalData()->clrActiveCaption : GetGlobalData()->clrInactiveCaption, bActive ? GetGlobalData()->clrActiveCaptionGradient : GetGlobalData()->clrInactiveCaptionGradient, FALSE);
			}
		}

		CRect rectQAT = pBar->GetQuickAccessToolbarLocation();

		if (rectQAT.left > rectQAT.right)
		{
			rectText.left = rectQAT.left + 1;
		}

		BOOL bHide  = (pBar->GetHideFlags() & AFX_RIBBONBAR_HIDE_ALL) != 0;
		BOOL bExtra = !bHide && pBar->IsQuickAccessToolbarOnTop() && rectQAT.left < rectQAT.right && !pBar->IsQATEmpty();

		BOOL bDrawIcon = (bHide && !bExtra) || pBar->IsWindows7Look();

		if (bExtra)
		{
			CRect rectQAFrame(rectQAT);
			rectQAFrame.right = rectText.left - 6;
			rectQAFrame.InflateRect(1, 1, 1, 1);

			const CMFCControlRendererInfo& params = m_ctrlRibbonCaptionQA.GetParams();

			if (rectQAFrame.Height() < params.m_rectImage.Height())
			{
				rectQAFrame.top = rectQAFrame.bottom - params.m_rectImage.Height();
			}

			m_ctrlRibbonCaptionQA.Draw(pDC, rectQAFrame, bActive ? 0 : 1);
		}


		if (bDrawIcon && ((CMyRibbonBar*)pBar)->IsIconDraw())
		{
			//HICON hIcon = afxGlobalUtils.GetWndIcon(pWnd);
			// Get Wnd Icon
			HICON hIcon = pWnd->GetIcon(FALSE);

			if (hIcon == nullptr)
			{
				hIcon = pWnd->GetIcon(TRUE);

				if (hIcon != nullptr)
				{
					CImageList il;
					il.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);
					il.Add(hIcon);

					if (il.GetImageCount() == 1)
					{
						hIcon = il.ExtractIcon(0);
					}
				}
			}

			if (hIcon == nullptr)
			{
				hIcon = (HICON)(LONG_PTR)::GetClassLongPtr(pWnd->GetSafeHwnd(), GCLP_HICONSM);
			}

			if (hIcon == nullptr)
			{
				hIcon = (HICON)(LONG_PTR)::GetClassLongPtr(pWnd->GetSafeHwnd(), GCLP_HICON);
			}

			if (hIcon != nullptr)
			{
				CSize szIcon(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));

				long x = rectCaption.left + 2;
				long y = rectCaption.top  + max(0, (rectCaption.Height() - szIcon.cy) / 2);

				pDC->DrawState(CPoint(x, y), szIcon, hIcon, DSS_NORMAL, (CBrush*)NULL);

				if (rectText.left < (x + szIcon.cx + 4))
				{
					rectText.left = x + szIcon.cx + 4;
				}
			}
		}
	}

	CString strText;
	pWnd->GetWindowText(strText);

	CFont* pOldFont = (CFont*)pDC->SelectObject(&m_AppCaptionFont);
	ASSERT(pOldFont != nullptr);

// 	CRect rc;
// 	pDC->GetClipBox(&rc);
// 	pDC->FillSolidRect(&rc, RGB(255,0,0));

	//DrawNcText(pDC, rectText, strText, bActive, bIsRTL, FALSE, bGlass, pWnd->IsZoomed() ? 0 : 10, pWnd->IsZoomed() ? RGB(255, 255, 255) :(COLORREF)-1);
	DrawNcText(pDC, rectText, strText, bActive, bIsRTL, FALSE, bGlass, pWnd->IsZoomed() ? 0 : 10, RGB(12,12,12));

	pDC->SelectObject(pOldFont);
}

void CMyVisualManagerWindows7Ex::OnDrawCaptionButton(CDC* pDC, CMFCCaptionButton* pButton, BOOL bActive, BOOL bHorz, BOOL bMaximized, BOOL bDisabled, int nImageID /*= -1*/)
{
 	CMFCVisualManagerWindows7::OnDrawCaptionButton(pDC, pButton, bActive, bHorz, bMaximized, bDisabled, nImageID);
}

COLORREF CMyVisualManagerWindows7Ex::OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons)
{
	if (m_hThemeExplorerBar == nullptr)
	{
		return CMFCVisualManagerOfficeXP::OnDrawPaneCaption(pDC, pBar, bActive, rectCaption, rectButtons);
	}


	//pDC->FillSolidRect(&rectCaption, RGB(68,68,68));
	pDC->FillSolidRect(&rectCaption, RGB(0,0,0));
	//
	return GetGlobalData()->clrCaptionText;

// 	if (GetGlobalData()->bIsWindowsVista)
// 	{
// 		CDrawingManager dm(*pDC);
// 		dm.FillGradient(rectCaption, 
// 			bActive ? GetGlobalData()->clrActiveCaptionGradient : GetGlobalData()->clrInactiveCaptionGradient, 
// 			bActive ? GetGlobalData()->clrActiveCaption : GetGlobalData()->clrInactiveCaption, TRUE);
// 
// 		return bActive ? GetGlobalData()->clrCaptionText : GetGlobalData()->clrInactiveCaptionText;
// 	}

// 	if (m_pfDrawThemeBackground != nullptr)
// 	{
// 		(*m_pfDrawThemeBackground)(m_hThemeExplorerBar, pDC->GetSafeHdc(), EBP_HEADERBACKGROUND, 0, &rectCaption, 0);
// 	}
// 
// 	COLORREF clrText;
// 	if ((m_pfGetThemeColor == nullptr) || ((*m_pfGetThemeColor)(m_hThemeExplorerBar, EBP_HEADERBACKGROUND, 0, TMT_TEXTCOLOR, &clrText) != S_OK))
// 	{
// 		clrText = GetGlobalData()->clrInactiveCaptionText;
// 	}
// 
// 	if (bActive)
// 	{
// 		CDrawingManager dm(*pDC);
// 		int nPercentage = 110;
// 
// 		if (GetRValue(clrText) > 128 &&
// 			GetGValue(clrText) > 128 &&
// 			GetBValue(clrText) > 128)
// 		{
// 			nPercentage = 80;
// 		}
// 
// 		dm.HighlightRect(rectCaption, nPercentage);
// 	}
// 
// 	return clrText;
}

void CMyVisualManagerWindows7Ex::OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* pProgress, CRect rectProgress, CRect rectChunk, BOOL bInfiniteMode)
{
// 	if (m_hThemeProgress == nullptr)
// 	{
// 		CMFCVisualManagerWindows::OnDrawRibbonProgressBar(pDC, pProgress, rectProgress, rectChunk, bInfiniteMode);
// 		return;
// 	}

 	rectProgress.DeflateRect(0, 2);
 	rectChunk.DeflateRect(0, 2);


// 	Gdiplus::Pen penProgress(Gdiplus::Color(89,89,89), (float)rectProgress.Height()/*/2.0f*/);
// 	penProgress.SetStartCap(LineCapRound);
// 	penProgress.SetEndCap(LineCapRound);
// 	//
// 	Gdiplus::Pen penChunk(Gdiplus::Color(52,52,52), (float)rectChunk.Height()/*/2.0f*/);
// 	penChunk.SetStartCap(LineCapRound);
// 	penChunk.SetEndCap(LineCapRound);
// 
// 	//
// 	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
// 	graphics.SetSmoothingMode(SmoothingModeHighQuality);
// 	//
// 	Gdiplus::PointF ptStart;
// 	ptStart.X = (float)rectProgress.left;
// 	ptStart.Y = (float)rectProgress.top + (float)rectProgress.Height()/2.0f;
// 	//
// 	Gdiplus::PointF ptEnd;
// 	ptEnd.X = (float)rectProgress.right;
// 	ptEnd.Y = (float)rectProgress.top + (float)rectProgress.Height()/2.0f;
// 	graphics.DrawLine(&penProgress, ptStart, ptEnd);
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	Gdiplus::PointF ptStart2;
// 	ptStart2.X = (float)rectChunk.left;
// 	ptStart2.Y = (float)rectChunk.top + (float)rectChunk.Height()/2.0f;
// 	//
// 	Gdiplus::PointF ptEnd2;
// 	ptEnd2.X = (float)rectChunk.right;
// 	ptEnd2.Y = (float)rectChunk.top + (float)rectChunk.Height()/2.0f;
// 	graphics.DrawLine(&penChunk, ptStart2, ptEnd2);
// 	return;

	//*m_pfDrawThemeBackground)(m_hThemeProgress, pDC->GetSafeHdc(), PP_BAR, 0, &rectProgress, 0);
	pDC->FillSolidRect(&rectProgress, RGB(69,69,69));

	if (!bInfiniteMode)
	{
		if (!rectChunk.IsRectEmpty() || pProgress->GetPos() != pProgress->GetRangeMin())
		{
			//(*m_pfDrawThemeBackground)(m_hThemeProgress, pDC->GetSafeHdc(), PP_CHUNK, 0, &rectChunk, 0);
			pDC->FillSolidRect(&rectChunk, RGB(46,46,46));
		}
	}
	else if (pProgress->GetPos() != pProgress->GetRangeMin())
	{
		CRgn rgn;
		rgn.CreateRectRgnIndirect(rectProgress);
		pDC->SelectClipRgn(&rgn);

		double index = (pProgress->GetPos() - pProgress->GetRangeMin()) * 2.0 / double(pProgress->GetRangeMax() - pProgress->GetRangeMin());

		rectProgress.OffsetRect((int)(rectProgress.Width() *(index - 1.0)), 0);

		//(*m_pfDrawThemeBackground)(m_hThemeProgress, pDC->GetSafeHdc(), PP_MOVEOVERLAY, 0, &rectProgress, 0);
		pDC->FillSolidRect(&rectProgress, RGB(46,46,46));

		pDC->SelectClipRgn(NULL);
	}

}

void CMyVisualManagerWindows7Ex::OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pBar);

	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnFillBarBackground (pDC, pBar, rectClient, rectClip, bNCArea);
		return;
	}

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonStatusBar)))
	{
		if (m_hThemeWindow != nullptr)
		{
			DrawThemeBackground(m_hThemeStatusBar, pDC->GetSafeHdc(), 0, 0, &rectClient, 0);
			return;
		}
	}
	else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonBar)))
	{
		CDrawingManager dm(*pDC);
		dm.FillGradient(rectClient, m_clrRibbonBarGradientDark, m_clrRibbonBarGradientLight, TRUE);

		return;
	}

	//CMFCVisualManagerWindows::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCStatusBar)) && m_hThemeStatusBar != nullptr)
	{
		DrawThemeBackground(m_hThemeStatusBar, pDC->GetSafeHdc(), 0, 0, &rectClient, 0);
		return;
	}

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonStatusBar)))
	{
		if (m_hThemeStatusBar != nullptr)
		{
			DrawThemeBackground(m_hThemeStatusBar, pDC->GetSafeHdc(), 0, 0, &rectClient, 0);
			return;
		}
	}

// 	if (m_pfDrawThemeBackground == NULL || m_hThemeRebar == NULL || pBar->IsDialogControl() || pBar->IsKindOf(RUNTIME_CLASS(CMFCCaptionBar)) || pBar->IsKindOf(RUNTIME_CLASS(CMFCColorBar)))
// 	{
// 		CMFCVisualManagerOfficeXP::OnFillBarBackground(pDC, pBar,rectClient, rectClip, bNCArea);
// 		return;
// 	}

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
	{
		if (m_bOfficeStyleMenus)
		{
			CMFCVisualManagerOfficeXP::OnFillBarBackground(pDC, pBar,rectClient, rectClip, bNCArea);
		}
		else
		{
			//::FillRect(pDC->GetSafeHdc(), rectClient, ::GetSysColorBrush(COLOR_MENU));
			//pDC->FillSolidRect(rectClient, RGB(58,58,58));
			pDC->FillSolidRect(rectClient, RGB(43,43,43));

// 			CMFCPopupMenuBar* pMenuBar = DYNAMIC_DOWNCAST(CMFCPopupMenuBar, pBar);
// 			if (m_hThemeMenu != NULL && !pMenuBar->m_bDisableSideBarInXPMode)
// 			{
// 				CRect rectGutter = rectClient;
// 				rectGutter.right = rectGutter.left + pMenuBar->GetGutterWidth() + 2;
// 				rectGutter.DeflateRect(0, 1);
// 
// 				(*m_pfDrawThemeBackground)(m_hThemeMenu, pDC->GetSafeHdc(), MENU_POPUPGUTTER, 0, &rectGutter, 0);
// 			}
		}

		return;
	}

	FillReBarPane(pDC, pBar, rectClient);
}

void CMyVisualManagerWindows7Ex::OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton)
{
// 	if (!CanDrawImage () || !pButton->IsRibbonButton())
// 	{
// 		CMFCVisualManagerWindows::OnDrawComboDropButton(pDC, rect, bDisabled, bIsDropped, bIsHighlighted, pButton);
// 		return;
// 	}

	rect.InflateRect(0, 1, 1, 1);

	int nIndex = 0;
	if (bDisabled)
	{
		nIndex = 3;
	}
	else
	{
		if (bIsDropped)
		{
			nIndex = 2;
		}
		else if (bIsHighlighted)
		{
			nIndex = 1;
		}
	}

	m_ctrlRibbonComboBoxBtn.Draw(pDC, rect, nIndex);

	rect.DeflateRect(0, 1, 1, 1);

	rect.bottom -= 2;

	CMenuImages::Draw(pDC, CMenuImages::IdArrowDown, rect, bDisabled ? CMenuImages::ImageGray : CMenuImages::ImageBlack);
}

void CMyVisualManagerWindows7Ex::OnDrawToolBarComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton)
{
	rect.InflateRect(0, 1, 1, 1);

	int nIndex = 0;
	if (bDisabled)
	{
		nIndex = 3;
	}
	else
	{
		if (bIsDropped)
		{
			nIndex = 2;
		}
		else if (bIsHighlighted)
		{
			nIndex = 1;
		}
	}

	// Do not draw outline
//	m_ctrlRibbonComboBoxBtn.Draw(pDC, rect, nIndex);

	rect.DeflateRect(0, 1, 1, 1);

	rect.bottom -= 2;

	CMenuImages::Draw(pDC, CMenuImages::IdArrowDown, rect, bDisabled ? CMenuImages::ImageGray : CMenuImages::ImageBlack);
}

void CMyVisualManagerWindows7Ex::OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted)
{
	if (m_hThemeHeader == nullptr)
	{
		CMFCVisualManagerOfficeXP::OnDrawHeaderCtrlBorder(pCtrl, pDC, rect, bIsPressed, bIsHighlighted);
		return;
	}

	//
	CRect rcHeader = rect;
	if (rcHeader.left)
	{
		rcHeader.left++;
	}

	if (bIsPressed)
	{
		pDC->FillSolidRect(&rcHeader, GetGlobalData()->clrActiveCaption);
	}
	else if (bIsHighlighted)
	{
		pDC->FillSolidRect(&rcHeader, GetGlobalData()->clrBarDkShadow);
	}
	else
	{
		pDC->FillSolidRect(&rcHeader, GetGlobalData()->clrInactiveCaption);
	}

// 	int nState = HIS_NORMAL;
// 
// 	if (bIsPressed)
// 	{
// 		nState = HIS_PRESSED;
// 	}
// 	else if (bIsHighlighted)
// 	{
// 		nState = HIS_HOT;
// 	}
// 
// 	if (m_pfDrawThemeBackground != nullptr)
// 	{
// 		(*m_pfDrawThemeBackground)(m_hThemeHeader, pDC->GetSafeHdc(), HP_HEADERITEM, nState, &rect, 0);
// 	}
}

void CMyVisualManagerWindows7Ex::OnDrawRibbonSliderChannel(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect)
{
	if (m_hThemeTrack == nullptr)
	{
		CMFCVisualManagerWindows::OnDrawRibbonSliderChannel(pDC, pSlider, rect);
		return;
	}

	ASSERT_VALID(pDC);

	BOOL bVert = FALSE;

	if (bVert)
	{
		if (rect.Width() < 3)
		{
			rect.right++;
		}
	}
	else
	{
		if (rect.Height() < 3)
		{
			rect.bottom++;
		}
	}

//	(*m_pfDrawThemeBackground)(m_hThemeTrack, pDC->GetSafeHdc(), bVert ? TKP_TRACKVERT : TKP_TRACK, 1, &rect, 0);
	pDC->FillSolidRect(&rect, RGB(64,64,64));
}

void CMyVisualManagerWindows7Ex::OnDrawRibbonSliderThumb(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled)
{
	if (m_hThemeTrack == nullptr)
	{
		CMFCVisualManagerWindows::OnDrawRibbonSliderThumb(pDC, pSlider, rect, bIsHighlighted, bIsPressed, bIsDisabled);
		return;
	}

	ASSERT_VALID(pDC);

	int nPart = 0;
	int nState = 0;

	BOOL bVert = FALSE;
	BOOL bLeftTop = FALSE;
	BOOL bRightBottom = TRUE;

	if (bLeftTop && bRightBottom)
	{
		nPart = bVert ? TKP_THUMBVERT : TKP_THUMB;
	}
	else if (bLeftTop)
	{
		nPart = bVert ? TKP_THUMBLEFT : TKP_THUMBTOP;
	}
	else
	{
		nPart = bVert ? TKP_THUMBRIGHT : TKP_THUMBBOTTOM;
	}

	if (bIsDisabled)
	{
		nState = TUS_DISABLED;
	}
	else if (bIsPressed)
	{
		nState = TUS_PRESSED;
	}
	else if (bIsHighlighted)
	{
		nState = TUS_HOT;
	}
	else
	{
		nState = TUS_NORMAL;
	}

	//(*m_pfDrawThemeBackground)(m_hThemeTrack, pDC->GetSafeHdc(), nPart, nState, &rect, 0);

	rect.DeflateRect(1,1);

	COLORREF crColor = RGB(0,0,0);
	switch (nState)
	{
	case TUS_NORMAL:
		crColor = RGB(128,128,128);
		break;
	case TUS_HOT:
		crColor = RGB(168,168,168);
		break;
	case TUS_PRESSED:
		crColor = RGB(108,108,108);
		break;
	case TUS_FOCUSED:
		crColor = RGB(128,128,128);
		break;
	case TUS_DISABLED:
		crColor = RGB(64,64,64);
		break;
	}

	pDC->FillSolidRect(&rect, crColor);
}


// void CMyVisualManagerWindows7Ex::GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack,
// 	COLORREF& clrHighlight, COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack)
// {
// 	ASSERT_VALID(pTabWnd);
// 
// 	COLORREF clrActiveTab = pTabWnd->GetTabBkColor(pTabWnd->GetActiveTab());
// 
// 	if (pTabWnd->IsOneNoteStyle() && clrActiveTab != (COLORREF)-1)
// 	{
// 		clrFace = clrActiveTab;
// 	}
// 	else if (pTabWnd->IsDialogControl())
// 	{
// 		clrFace = GetGlobalData()->clrBtnFace;
// 	}
// 	else
// 	{
// 		clrFace = GetGlobalData()->clrBarFace;
// 	}
// 
// 	if (pTabWnd->IsDialogControl())
// 	{
// 		clrDark = GetGlobalData()->clrBtnShadow;
// 		clrBlack = GetGlobalData()->clrBtnText;
// 		clrHighlight = pTabWnd->IsVS2005Style() ? GetGlobalData()->clrBtnShadow : GetGlobalData()->clrBtnHilite;
// 		clrDarkShadow = GetGlobalData()->clrBtnDkShadow;
// 		clrLight = GetGlobalData()->clrBtnLight;
// 
// 		pbrFace = &(GetGlobalData()->brBtnFace);
// 	}
// 	else
// 	{
// 		clrDark = GetGlobalData()->clrBarShadow;
// 		clrBlack = GetGlobalData()->clrBarText;
// 		clrHighlight = pTabWnd->IsVS2005Style() ? GetGlobalData()->clrBarShadow : GetGlobalData()->clrBarHilite;
// 		clrDarkShadow = GetGlobalData()->clrBarDkShadow;
// 		clrLight = GetGlobalData()->clrBarLight;
// 
// 		pbrFace = &(GetGlobalData()->brBarFace);
// 	}
// 
// 	pbrBlack = &(GetGlobalData()->brBlack);
// }
// 
// void CMyVisualManagerWindows7Ex::OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* /*pTabWnd*/)
// {
// 	ASSERT_VALID(pDC);
// 	//pDC->FillRect(rect, &(GetGlobalData()->brBarFace));
// 	//
// 	CBrush brush(RGB(33, 33, 33));
// 	pDC->FillRect(rect, &brush);
// }

void CMyVisualManagerWindows7Ex::OnDrawMiniFrameBorder(CDC* pDC, CPaneFrameWnd* pFrameWnd, CRect rectBorder, CRect rectBorderSize)
{
// 	if (m_hThemeWindow == NULL)
// 	{
// 		CMFCVisualManager::OnDrawMiniFrameBorder(pDC, pFrameWnd, rectBorder, rectBorderSize);
// 		return;
// 	}

	//DrawThemeBackground(m_hThemeWindow, pDC->GetSafeHdc(), WP_SMALLCAPTION, 0, &rectBorder, 0);
	
	pDC->FillSolidRect(&rectBorder, RGB(33,33,33));
	//
	rectBorder.DeflateRect(1, 1);
	pDC->FillSolidRect(&rectBorder, RGB(0, 0, 0));

	//
// 	pDC->Draw3dRect(rectBorder, GetGlobalData()->clrBarFace, GetGlobalData()->clrBarDkShadow);
// 	rectBorder.DeflateRect(1, 1);
// 	pDC->Draw3dRect(rectBorder, GetGlobalData()->clrBarHilite, GetGlobalData()->clrBarShadow);
}
