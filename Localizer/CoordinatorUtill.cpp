#include "stdafx.h"
#include "CoordinatorUtill.h"


CCoordinatorUtill::CCoordinatorUtill()
{
}


CCoordinatorUtill::~CCoordinatorUtill()
{
}

Gdiplus::PointF CCoordinatorUtill::ConvertPointF(CPoint point)
{
	Gdiplus::PointF ptNew;
	ptNew.X = (float)point.x;
	ptNew.Y = (float)point.y;

	return ptNew;
}

Gdiplus::Point CCoordinatorUtill::ConvertPoint(CPoint point)
{
	Gdiplus::Point ptNew;
	ptNew.X = point.x;
	ptNew.Y = point.y;

	return ptNew;
}

CPoint CCoordinatorUtill::ConvertPointF(Gdiplus::PointF point)
{
	CPoint ptNew;
	ptNew.x = (LONG)point.X;
	ptNew.y = (LONG)point.Y;

	return ptNew;
}

CPoint CCoordinatorUtill::ConvertPoint(Gdiplus::Point point)
{
	CPoint ptNew;
	ptNew.x = point.X;
	ptNew.y = point.Y;

	return ptNew;
}

CPoint CCoordinatorUtill::ConvertImage2ScreenCoordinate(CPoint point, CRect rtCanvas, CRect rtImage)
{
	Gdiplus::PointF ptNew = ConvertImage2ScreenCoordinateEx(point, rtCanvas, rtImage);
	//
	point.x = (long)ptNew.X;
	point.y = (long)ptNew.Y;
	//
	return point;
}

Gdiplus::PointF CCoordinatorUtill::ConvertImage2ScreenCoordinateEx(CPoint point, CRect rtCanvas, CRect rtImage)
{
	Gdiplus::PointF ptNew((float)point.x, (float)point.y);

	return ConvertImage2ScreenCoordinateEx(ptNew, rtCanvas, rtImage);
}

Gdiplus::PointF CCoordinatorUtill::ConvertImage2ScreenCoordinateEx(Gdiplus::PointF point, CRect rtCanvas, CRect rtImage)
{
	Gdiplus::PointF ptResult(0.0f, 0.0f);
	//
	float fRatioX = (double)rtCanvas.Width() / (float)rtImage.Width();
	float fRatioY = (double)rtCanvas.Height() / (float)rtImage.Height();
	ptResult.X = (point.X - rtImage.left)*fRatioX + rtCanvas.left;
	ptResult.Y = (point.Y - rtImage.top)*fRatioY + rtCanvas.top;
	//ptResult.Y = rtCanvas.Height() - (point.Y - rtImage.top)*fRatioY + rtCanvas.top;

	return ptResult;
}

CPoint CCoordinatorUtill::ConvertScreen2ImageCoordinate(CPoint point, CRect rtCanvas, CRect rtImage)
{
	Gdiplus::PointF ptNew = ConvertScreen2ImageCoordinateEx(point, rtCanvas, rtImage);
	//
	point.x = (long)ptNew.X;
	point.y = (long)ptNew.Y;
	//
	return point;
}

Gdiplus::PointF CCoordinatorUtill::ConvertScreen2ImageCoordinateEx(CPoint point, CRect rtCanvas, CRect rtImage)
{
	Gdiplus::PointF ptNew((float)point.x, (float)point.y);

	return ConvertScreen2ImageCoordinateEx(ptNew, rtCanvas, rtImage);
}

Gdiplus::PointF CCoordinatorUtill::ConvertScreen2ImageCoordinateEx(Gdiplus::PointF point, CRect rtCanvas, CRect rtImage)
{
	Gdiplus::PointF ptResult(0.0f, 0.0f);

	float fRatioX = (double)rtImage.Width() / (float)rtCanvas.Width();
	float fRatioY = (double)rtImage.Height() / (float)rtCanvas.Height();
	ptResult.X = (point.X - rtCanvas.left)*fRatioX + rtImage.left;
	ptResult.Y = rtImage.Height() - (point.Y - rtCanvas.top)*fRatioY + rtImage.top;
	//
	return ptResult;
}

Gdiplus::PointF CCoordinatorUtill::ConvertCanvas(Gdiplus::PointF point, CRect rtCanvas, CRect rtImage)
{
	Gdiplus::PointF ptResult(0.0f, 0.0f);
	//
	float fRatioX = (double)rtCanvas.Width() / (float)rtImage.Width();
	float fRatioY = (double)rtCanvas.Height() / (float)rtImage.Height();
	ptResult.X = (point.X - rtImage.left)*fRatioX + rtCanvas.left;
	ptResult.Y = (point.Y - rtImage.top)*fRatioY + rtCanvas.top;
	//ptResult.Y = rtCanvas.Height() - (point.Y - rtImage.top)*fRatioY + rtCanvas.top;

	return ptResult;
}