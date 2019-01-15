#pragma once

#include "MyVisualManagerWindows7.h"

class CMyVisualManagerWindows7Ex : public CMyVisualManagerWindows7
{
	friend class CMyRibbonDateTimeCtrl;
	friend class CMyRibbonComboBox;

	DECLARE_DYNCREATE(CMyVisualManagerWindows7Ex)

public:
	CMyVisualManagerWindows7Ex();
	virtual ~CMyVisualManagerWindows7Ex();


public:
	CFont* GetAppCaptionFont();
	virtual void FillReBarPane(CDC* pDC, CBasePane* pBar, CRect rectClient);
	virtual BOOL DrawCheckBox(CDC *pDC, CRect rect, BOOL bHighlighted, int nState, BOOL bEnabled, BOOL bPressed);
	virtual void DrawNcBtn(CDC* pDC, const CRect& rect, UINT nButton, AFX_BUTTON_STATE state, BOOL bSmall, BOOL bActive, BOOL bMDI = FALSE);

public:
	virtual COLORREF OnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive);
	virtual COLORREF OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel, CRect rectCaption);
	virtual void OnDrawRibbonDefaultPaneButtonContext(CDC* pDC, CMFCRibbonButton* pButton);
	// Slider
	virtual void OnDrawPaneDivider(CDC* pDC, CPaneDivider* pSlider, CRect rect, BOOL bAutoHideMode);
	//
	virtual void OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);
	//
	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	//
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);
	//
	virtual void OnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rectCaption, CRect rectText);
	//
	virtual void OnDrawCaptionButton(CDC* pDC, CMFCCaptionButton* pButton, BOOL bActive, BOOL bHorz, BOOL bMaximized, BOOL bDisabled, int nImageID = -1);
	//
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);
	//
	virtual void OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* pProgress, CRect rectProgress, CRect rectChunk, BOOL bInfiniteMode);
	//
	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	//
	virtual void OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawToolBarComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	//
	virtual void OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	//
	virtual void OnDrawRibbonSliderChannel(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect);
	virtual void OnDrawRibbonSliderThumb(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	//
// 	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
// 		COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
// 	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawMiniFrameBorder(CDC* pDC, CPaneFrameWnd* pFrameWnd, CRect rectBorder, CRect rectBorderSize);
};

