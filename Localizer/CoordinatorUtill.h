#pragma once
class CCoordinatorUtill
{
public:
	CCoordinatorUtill();
	~CCoordinatorUtill();

public:
	virtual Gdiplus::PointF ConvertPointF(CPoint point);
	virtual Gdiplus::Point ConvertPoint(CPoint point);
	virtual CPoint ConvertPointF(Gdiplus::PointF point);
	virtual CPoint ConvertPoint(Gdiplus::Point point);

	// TODO fixme
	CPoint ConvertImage2ScreenCoordinate(CPoint point, CRect rtCanvas, CRect rtImage);
	Gdiplus::PointF ConvertImage2ScreenCoordinateEx(CPoint point, CRect rtCanvas, CRect rtImage);
	Gdiplus::PointF ConvertImage2ScreenCoordinateEx(Gdiplus::PointF point, CRect rtCanvas, CRect rtImage);

	CPoint ConvertScreen2ImageCoordinate(CPoint point, CRect rtCanvas, CRect rtImage);
	Gdiplus::PointF ConvertScreen2ImageCoordinateEx(CPoint point, CRect rtCanvas, CRect rtImage);
	Gdiplus::PointF ConvertScreen2ImageCoordinateEx(Gdiplus::PointF point, CRect rtCanvas, CRect rtImage);

	// Not change Y position
	Gdiplus::PointF CCoordinatorUtill::ConvertCanvas(Gdiplus::PointF point, CRect rtCanvas, CRect rtImage);
};

