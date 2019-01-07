#include "stdafx.h"
#include "LocalizerCore.h"
#include "Vectors.h"
#include "Line.h"
#include "Plane.h"


CLocalizerCore::CLocalizerCore()
{
}


CLocalizerCore::~CLocalizerCore()
{
}

void CLocalizerCore::Init()
{
	m_nSelectedIndex = 0;
	m_aryStudyViewer.RemoveAll();
}

void CLocalizerCore::SetSelectedIndex(INT_PTR nSelectedIndex)
{
	if (nSelectedIndex < 0)
		m_nSelectedIndex = 0;

	m_nSelectedIndex = nSelectedIndex;
}

void CLocalizerCore::AddStudyViewer(CStudyViewer* pStudyViewer)
{
	m_aryStudyViewer.Add(pStudyViewer);
}

CLocalizerCore::LOCALIZER_IMAGE_INFO_ CLocalizerCore::GetLocalizerImageInfo(CLLDicomDS* dsSrcDicomDS, INT_PTR nFrameIndex)
{
	LOCALIZER_IMAGE_INFO_ stLocalizerImageInfo;

	// Row & Col
	stLocalizerImageInfo.nRow = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_nHeight;
	stLocalizerImageInfo.nCol = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_nWidth;

	FLOAT nImageXSpacing;
	FLOAT nImageYSpacing;
	if (dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fPixelXSpacing <= 0)
	{
		nImageXSpacing = 1;
	}
	else
	{
		nImageXSpacing = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fPixelXSpacing;
	}


	if (dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fPixelYSpacing <= 0)
	{
		nImageYSpacing = 1;
	}
	else
	{
		nImageYSpacing = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fPixelYSpacing;
	}


	stLocalizerImageInfo.nRowLength = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_nWidth * nImageXSpacing;
	stLocalizerImageInfo.nColLength = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_nHeight * nImageYSpacing;

	// Position
	stLocalizerImageInfo.fOriginX = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_X;
	stLocalizerImageInfo.fOriginY = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_Y;
	stLocalizerImageInfo.fOriginZ = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagePosition_Z;

	// Orientation
	stLocalizerImageInfo.fRowOrientX = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowX;
	stLocalizerImageInfo.fRowOrientY = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowY;
	stLocalizerImageInfo.fRowOrientZ = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationRowZ;

	stLocalizerImageInfo.fColOrientX = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColX;
	stLocalizerImageInfo.fColOrientY = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColY;
	stLocalizerImageInfo.fColOrientZ = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationColZ;

	stLocalizerImageInfo.fOrthoOrientX = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationOrthogonalX;
	stLocalizerImageInfo.fOrthoOrientY = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationOrthogonalY;
	stLocalizerImageInfo.fOrthoOrientZ = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImageOrientationOrthogonalZ;

	stLocalizerImageInfo.m_fPixelXSpacing = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fPixelXSpacing;
	stLocalizerImageInfo.m_fPixelYSpacing = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fPixelYSpacing;
	stLocalizerImageInfo.m_fImagerXSpacing = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagerXSpacing;
	stLocalizerImageInfo.m_fImagerYSpacing = dsSrcDicomDS->m_aryDicomImage.GetAt(nFrameIndex).m_stImageInfo.m_fImagerYSpacing;

	// TLHC
	stLocalizerImageInfo.fRectPosX[0] = stLocalizerImageInfo.fOriginX;
	stLocalizerImageInfo.fRectPosY[0] = stLocalizerImageInfo.fOriginY;
	stLocalizerImageInfo.fRectPosZ[0] = stLocalizerImageInfo.fOriginZ;

	// TRHC
	stLocalizerImageInfo.fRectPosX[1] = stLocalizerImageInfo.fOriginX + stLocalizerImageInfo.fRowOrientX * stLocalizerImageInfo.nRowLength;
	stLocalizerImageInfo.fRectPosY[1] = stLocalizerImageInfo.fOriginY + stLocalizerImageInfo.fRowOrientY * stLocalizerImageInfo.nRowLength;
	stLocalizerImageInfo.fRectPosZ[1] = stLocalizerImageInfo.fOriginZ + stLocalizerImageInfo.fRowOrientZ * stLocalizerImageInfo.nRowLength;

	// BRHC
	stLocalizerImageInfo.fRectPosX[2] = stLocalizerImageInfo.fOriginX + stLocalizerImageInfo.fRowOrientX * stLocalizerImageInfo.nRowLength + stLocalizerImageInfo.fColOrientX * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosY[2] = stLocalizerImageInfo.fOriginY + stLocalizerImageInfo.fRowOrientY * stLocalizerImageInfo.nRowLength + stLocalizerImageInfo.fColOrientY * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosZ[2] = stLocalizerImageInfo.fOriginZ + stLocalizerImageInfo.fRowOrientZ * stLocalizerImageInfo.nRowLength + stLocalizerImageInfo.fColOrientZ * stLocalizerImageInfo.nColLength;

	// BLHC
	stLocalizerImageInfo.fRectPosX[3] = stLocalizerImageInfo.fOriginX + stLocalizerImageInfo.fColOrientX * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosY[3] = stLocalizerImageInfo.fOriginY + stLocalizerImageInfo.fColOrientY * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosZ[3] = stLocalizerImageInfo.fOriginZ + stLocalizerImageInfo.fColOrientZ * stLocalizerImageInfo.nColLength;

	return stLocalizerImageInfo;
}

void CLocalizerCore::CalcReferenceLine()
{
	if (m_aryStudyViewer.GetCount() <= 0)
		return;

	CStudyViewer* pSrcStudyViewer = m_aryStudyViewer.GetAt(m_nSelectedIndex);

	if (!pSrcStudyViewer->GetStudy())
		return;

	GeometryMath::Plane planeSrc;
	GeometryMath::Plane planeDest;
	GeometryMath::Line lineIntersect;
	GeometryMath::Line lineDestRect[4];
	GeometryMath::Vector3 vec3SrcRect[4];
	GeometryMath::Vector3 vec3DestRect[4];
	GeometryMath::Vector3 vecDrawPoint[4];

	GeometryMath::Vector3 vec3Start;
	GeometryMath::Vector3 vec3End;

	INT_PTR nIntersectCount = 0;
	
	CLLDicomDS* dsSrcDicomDS = pSrcStudyViewer->GetDisplayingDicomDS();	// Selected Image (having location information)
	LOCALIZER_IMAGE_INFO_ stSrcLocalizerImageInfo = GetLocalizerImageInfo(dsSrcDicomDS, pSrcStudyViewer->GetCurrentFrameIndex());

	CLLDicomDS* dsDestDicomDS = nullptr;								// Canvas Image (Localizer image)
	LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo;	

	FLOAT fSrcXPosInDestImage_Pixel[4];	// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)
	FLOAT fSrcYPosInDestImage_Pixel[4];	// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)
	FLOAT fSrcZPosInDestImage_Pixel[4];	// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)

	double dSrcPosX[4] = { 0.0, };
	double dSrcPosY[4] = { 0.0, };
	double dSrcPosZ[4] = { 0.0, };

	double dDestPosX[4] = { 0.0, };
	double dDestPosY[4] = { 0.0, };
	double dDestPosZ[4] = { 0.0, };

	double dSrcRotatedPosX[4] = { 0.0, };
	double dSrcRotatedPosY[4] = { 0.0, };
	double dSrcRotatedPosZ[4] = { 0.0, };

	double dDestRotatedPosX[4] = { 0.0, };
	double dDestRotatedPosY[4] = { 0.0, };
	double dDestRotatedPosZ[4] = { 0.0, };

	Gdiplus::PointF ptStart;
	Gdiplus::PointF ptEnd;

	for (int iStudyViewer = 0; iStudyViewer < m_aryStudyViewer.GetCount(); iStudyViewer++)
	{
		if (iStudyViewer == m_nSelectedIndex)
			continue;

		CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);

		if (!pStudyViewer)
			continue;

		if (!pStudyViewer->GetStudy())
			continue;

		if (pSrcStudyViewer->GetStudy()->IsSameStudyID(pStudyViewer->GetStudy()->GetStudyId()) == FALSE)
			continue;

		pStudyViewer->ClearLocalizerPoints();
		dsDestDicomDS = pStudyViewer->GetDisplayingDicomDS();
		stDestLocalizerImageInfo = GetLocalizerImageInfo(dsDestDicomDS, pStudyViewer->GetCurrentFrameIndex());

		if (IsSamePatientOrientation(stSrcLocalizerImageInfo, stDestLocalizerImageInfo))
			continue;

		for (INT_PTR iRect = 0; iRect < 4; iRect++)
		{
			dDestPosX[iRect] = stDestLocalizerImageInfo.fRectPosX[iRect] - stDestLocalizerImageInfo.fRectPosX[0];
			dDestPosY[iRect] = stDestLocalizerImageInfo.fRectPosY[iRect] - stDestLocalizerImageInfo.fRectPosY[0];
			dDestPosZ[iRect] = stDestLocalizerImageInfo.fRectPosZ[iRect] - stDestLocalizerImageInfo.fRectPosZ[0];

			RotateBasedOnDestOrientation(stDestLocalizerImageInfo, dDestPosX[iRect], dDestPosY[iRect], dDestPosZ[iRect],
				dDestRotatedPosX[iRect], dDestRotatedPosY[iRect], dDestRotatedPosZ[iRect]);

			dDestRotatedPosZ[iRect] = 0;

			vec3DestRect[iRect].set(dDestRotatedPosX[iRect], dDestRotatedPosY[iRect], dDestRotatedPosZ[iRect]);
		}

		planeDest.set(vec3DestRect[0], vec3DestRect[1], vec3DestRect[2]);

		for (INT_PTR iRect = 0; iRect < 4; iRect++)
		{
			// Move points to calculate based on destination image information.
			dSrcPosX[iRect] = stSrcLocalizerImageInfo.fRectPosX[iRect] - stDestLocalizerImageInfo.fRectPosX[0];
			dSrcPosY[iRect] = stSrcLocalizerImageInfo.fRectPosY[iRect] - stDestLocalizerImageInfo.fRectPosY[0];
			dSrcPosZ[iRect] = stSrcLocalizerImageInfo.fRectPosZ[iRect] - stDestLocalizerImageInfo.fRectPosZ[0];

			RotateBasedOnDestOrientation(stDestLocalizerImageInfo, dSrcPosX[iRect], dSrcPosY[iRect], dSrcPosZ[iRect],
				dSrcRotatedPosX[iRect], dSrcRotatedPosY[iRect], dSrcRotatedPosZ[iRect]);

			vec3SrcRect[iRect].set(dSrcRotatedPosX[iRect], dSrcRotatedPosY[iRect], dSrcRotatedPosZ[iRect]);
		}

		planeSrc.set(vec3SrcRect[0], vec3SrcRect[1], vec3SrcRect[2]);

		lineIntersect = planeSrc.intersect(planeDest);
		if (lineIntersect.getDirection() == GeometryMath::Vector3(INFINITY, INFINITY, INFINITY) ||
			lineIntersect.getPoint() == GeometryMath::Vector3(INFINITY, INFINITY, INFINITY))
		{
			continue;
		}

		INT_PTR iSide = 0;
		for (iSide = 0; iSide < 4; iSide++)
		{
			INT_PTR iNext;
			if (iSide == 3)
			{
				iNext = 0;
			}
			else
			{
				iNext = iSide + 1;
			}

			vec3Start.set(dSrcRotatedPosX[iSide], dSrcRotatedPosY[iSide], dSrcRotatedPosZ[iSide]);
			vec3End.set(dSrcRotatedPosX[iNext], dSrcRotatedPosY[iNext], dSrcRotatedPosZ[iNext]);

			lineDestRect[iSide].setWith2Points(vec3Start, vec3End);
		}

		nIntersectCount = 0;
		float fOffset = 0.01;	// sometime epsilon data occur. so, not equal in the case.
		BOOL bCanDraw = FALSE;
		for (iSide = 0; iSide < 4; iSide++)
		{
			GeometryMath::Vector3 vec3Temp = lineDestRect[iSide].intersect(lineIntersect);
			if (vec3Temp != GeometryMath::Vector3(INFINITY, INFINITY, INFINITY) &&
				vec3Temp.x >= (dDestRotatedPosX[0] - fOffset) &&
				vec3Temp.x <= (dDestRotatedPosX[2] + fOffset) &&
				vec3Temp.y >= (dDestRotatedPosY[0] - fOffset) &&
				vec3Temp.y <= (dDestRotatedPosY[2] + fOffset) &&
				vec3Temp.z >= (dDestRotatedPosZ[0] - fOffset) &&
				vec3Temp.z <= (dDestRotatedPosZ[2] + fOffset))
			{
				vecDrawPoint[nIntersectCount++] = vec3Temp;
			}

			if (nIntersectCount >= 2)
			{
				bCanDraw = TRUE;
				break;
			}
		}

		if (bCanDraw == FALSE)
		{
			INT_PTR iSide = 0;
			for (iSide = 0; iSide < 4; iSide++)
			{
				INT_PTR iNext;
				if (iSide == 3)
				{
					iNext = 0;
				}
				else
				{
					iNext = iSide + 1;
				}

				vec3Start.set(dDestRotatedPosX[iSide], dDestRotatedPosY[iSide], dDestRotatedPosZ[iSide]);
				vec3End.set(dDestRotatedPosX[iNext], dDestRotatedPosY[iNext], dDestRotatedPosZ[iNext]);

				lineDestRect[iSide].setWith2Points(vec3Start, vec3End);
			}

			nIntersectCount = 0;
			float fOffset = 0.01;	// sometime epsilon data(very small value) occur
			for (iSide = 0; iSide < 4; iSide++)
			{
				GeometryMath::Vector3 vec3Temp = lineDestRect[iSide].intersect(lineIntersect);

				if (vec3Temp != GeometryMath::Vector3(INFINITY, INFINITY, INFINITY) &&
					vec3Temp.x >= (dDestRotatedPosX[0] - fOffset) &&
					vec3Temp.x <= (dDestRotatedPosX[2] + fOffset) &&
					vec3Temp.y >= (dDestRotatedPosY[0] - fOffset) &&
					vec3Temp.y <= (dDestRotatedPosY[2] + fOffset) &&
					vec3Temp.z >= (dDestRotatedPosZ[0] - fOffset) &&
					vec3Temp.z <= (dDestRotatedPosZ[2] + fOffset))
				{
					vecDrawPoint[nIntersectCount++] = vec3Temp;
				}

				if (nIntersectCount >= 2)
				{
					bCanDraw = TRUE;
					break;
				}
			}
		}

		if (bCanDraw == TRUE)
		{
			ptStart = Gdiplus::PointF(vecDrawPoint[0].x / stDestLocalizerImageInfo.m_fPixelXSpacing + 0.5, vecDrawPoint[0].y / stDestLocalizerImageInfo.m_fPixelYSpacing + 0.5);
			ptEnd = Gdiplus::PointF(vecDrawPoint[1].x / stDestLocalizerImageInfo.m_fPixelXSpacing + 0.5, vecDrawPoint[1].y / stDestLocalizerImageInfo.m_fPixelYSpacing + 0.5);

			pStudyViewer->SetLocalizerPoints(ptStart, ptEnd);
		}
	}
}

void CLocalizerCore::RotateBasedOnDestOrientation(LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo, 
	double dSrcPosX, double dSrcPosY, double dSrcPosZ,
	double &dDestPosX, double &dDestPosY, double &dDestPosZ)
{
	dDestPosX = stDestLocalizerImageInfo.fRowOrientX * dSrcPosX
		+ stDestLocalizerImageInfo.fRowOrientY * dSrcPosY
		+ stDestLocalizerImageInfo.fRowOrientZ * dSrcPosZ;

	dDestPosY = stDestLocalizerImageInfo.fColOrientX * dSrcPosX
		+ stDestLocalizerImageInfo.fColOrientY * dSrcPosY
		+ stDestLocalizerImageInfo.fColOrientZ * dSrcPosZ;

	dDestPosZ = stDestLocalizerImageInfo.fOrthoOrientX * dSrcPosX
		+ stDestLocalizerImageInfo.fOrthoOrientY * dSrcPosY
		+ stDestLocalizerImageInfo.fOrthoOrientZ * dSrcPosZ;

}

BOOL CLocalizerCore::IsSamePatientOrientation(LOCALIZER_IMAGE_INFO_ stSrcLocalizerImageInfo, LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo)
{
	if (stSrcLocalizerImageInfo.fRowOrientX != stDestLocalizerImageInfo.fRowOrientX)
		return FALSE;

	if (stSrcLocalizerImageInfo.fRowOrientY != stDestLocalizerImageInfo.fRowOrientY)
		return FALSE;

	if (stSrcLocalizerImageInfo.fRowOrientZ != stDestLocalizerImageInfo.fRowOrientZ)
		return FALSE;

	if (stSrcLocalizerImageInfo.fColOrientX != stDestLocalizerImageInfo.fColOrientX)
		return FALSE;

	if (stSrcLocalizerImageInfo.fColOrientY != stDestLocalizerImageInfo.fColOrientY)
		return FALSE;

	if (stSrcLocalizerImageInfo.fColOrientZ != stDestLocalizerImageInfo.fColOrientZ)
		return FALSE;

	return TRUE;
}

/*
BOOL CLocalizerCore::CalcContactZSurface(CArray<CLocalizerCore::POINT_3D_, CLocalizerCore::POINT_3D_> *ary3DPoint, Gdiplus::PointF &ptStart, Gdiplus::PointF &ptEnd)
{
	BOOL bRes = FALSE;

	if (ary3DPoint->GetCount() < 4)
		return FALSE;

	CLocalizerCore::POINT_3D_ aryUpperPoint[4];
	CLocalizerCore::POINT_3D_ aryLowerPoint[4];
	INT_PTR arySetIndex[4] = { 0,0,0,0 };

	INT_PTR nUpperPointCount = 0;
	INT_PTR nLowerPointCount = 0;

	if (ary3DPoint->GetAt(0).fZ >= 0)
	{
		aryUpperPoint[nUpperPointCount++] = ary3DPoint->GetAt(0);
	}
	else
	{
		aryLowerPoint[nLowerPointCount++] = ary3DPoint->GetAt(0);
	}

	if (ary3DPoint->GetAt(1).fZ >= 0)
	{
		aryUpperPoint[nUpperPointCount++] = ary3DPoint->GetAt(1);
	}
	else
	{
		aryLowerPoint[nLowerPointCount++] = ary3DPoint->GetAt(1);
	}

	if (ary3DPoint->GetAt(2).fZ >= 0)
	{
		aryUpperPoint[nUpperPointCount++] = ary3DPoint->GetAt(2);
	}
	else
	{
		aryLowerPoint[nLowerPointCount++] = ary3DPoint->GetAt(2);
	}

	if (ary3DPoint->GetAt(3).fZ >= 0)
	{
		aryUpperPoint[nUpperPointCount++] = ary3DPoint->GetAt(3);
	}
	else
	{
		aryLowerPoint[nLowerPointCount++] = ary3DPoint->GetAt(3);
	}

	if (nUpperPointCount >= 4)
		return FALSE;

	if (nLowerPointCount >= 4)
		return FALSE;


	double distPoints;


	for (INT_PTR iUpper = 0; iUpper < nUpperPointCount; iUpper++)
	{
		distPoints = -1;
		for (INT_PTR iLower = 0; iLower < nLowerPointCount; iLower++)
		{
			double tempDist = CalcDistance(aryUpperPoint[iUpper], aryLowerPoint[iLower]);

			if (distPoints == -1 || tempDist < distPoints)
			{
				arySetIndex[iUpper] = iLower;
				distPoints = tempDist;
			}
		}
	}

	CLocalizerCore::POINT_3D_ ptFirstLineStart;
	CLocalizerCore::POINT_3D_ ptFirstLineEnd;
	CLocalizerCore::POINT_3D_ ptSecLineStart;
	CLocalizerCore::POINT_3D_ ptSecLineEnd;

	CLocalizerCore::POINT_3D_ ptResultStart;
	CLocalizerCore::POINT_3D_ ptResultEnd;

	INT_PTR nLongestIndex = 0;
	INT_PTR nLongestDistance = 0;

	double dTempDist;

	switch (nUpperPointCount)
	{
	case 1:
		ptFirstLineStart = aryUpperPoint[0];
		ptSecLineStart = aryUpperPoint[0];

		ptFirstLineEnd = aryLowerPoint[0];
		ptSecLineEnd = aryLowerPoint[1];

		for (INT_PTR iDist = 0; iDist < nLowerPointCount; iDist++)
		{
			dTempDist = CalcDistance(ptSecLineStart, aryLowerPoint[iDist]);
			if (dTempDist > nLongestDistance)
			{
				nLongestDistance = dTempDist;
				nLongestIndex = iDist;
			}
		}

		if (nLongestIndex == 0)
		{
			ptFirstLineEnd = aryLowerPoint[2];
		}
		else if (nLongestIndex == 1)
		{
			ptSecLineEnd = aryLowerPoint[2];
		}
		break;
	case 2:
		ptFirstLineStart = aryUpperPoint[0];
		ptSecLineStart = aryUpperPoint[1];

		ptFirstLineEnd = aryLowerPoint[arySetIndex[0]];
		ptSecLineEnd = aryLowerPoint[arySetIndex[1]];
		break;
	case 3:
		ptFirstLineStart = aryLowerPoint[0];
		ptSecLineStart = aryLowerPoint[0];

		ptFirstLineEnd = aryUpperPoint[0];
		ptSecLineEnd = aryUpperPoint[1];

		
		for (INT_PTR iDist = 0; iDist < nLowerPointCount; iDist++)
		{
			dTempDist = CalcDistance(ptSecLineStart, aryLowerPoint[iDist]);
			if (dTempDist > nLongestDistance)
			{
				nLongestDistance = dTempDist;
				nLongestIndex = iDist;
			}
		}

		if (nLongestIndex == 0)
		{
			ptFirstLineEnd = aryUpperPoint[2];
		}
		else if (nLongestIndex == 1)
		{
			ptSecLineEnd = aryUpperPoint[2];
		}
		break;
	default:
		CString Msg;
		Msg.Format(_T("[LocalizerCore] This(%d) Upper point count is abnormal "), nUpperPointCount);
		AfxMessageBox(Msg);
		return FALSE;
	}

	ptResultStart = GetInterSectionWithZ0(ptFirstLineStart, ptFirstLineEnd);
	ptResultEnd = GetInterSectionWithZ0(ptSecLineStart, ptSecLineEnd);
	
	ptStart.X = ptResultStart.fX;
	ptStart.Y = ptResultStart.fY;
	ptEnd.X = ptResultEnd.fX;
	ptEnd.Y = ptResultEnd.fY;

	return TRUE;
}

double CLocalizerCore::CalcDistance(CLocalizerCore::POINT_3D_ ptSrc, CLocalizerCore::POINT_3D_ ptDest)
{
	double dX = ptSrc.fX - ptDest.fX;
	double dY = ptSrc.fY - ptDest.fY;
	double dZ = ptSrc.fZ - ptDest.fZ;

	return sqrt(dX*dX + dY*dY + dZ*dZ);
}

CLocalizerCore::POINT_3D_ CLocalizerCore::GetInterSectionWithZ0(CLocalizerCore::POINT_3D_ ptStart, CLocalizerCore::POINT_3D_ ptEnd)
{
	CLocalizerCore::POINT_3D_ ptRes;

	double r = -ptStart.fZ / ptEnd.fZ;
	
	ptRes.fX = (r*ptEnd.fX + ptStart.fX) / (r + 1);
	ptRes.fY = (r*ptEnd.fY + ptStart.fY) / (r + 1);
	ptRes.fZ = 0.0;

	return ptRes;
}
*/