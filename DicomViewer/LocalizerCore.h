#pragma once
#include "StudyViewer.h"

class CStudyViewer;
class CLocalizerCore
{
public:
	typedef struct _LOCALIZER_IMAGE_INFO_
	{
		FLOAT fRectPosX[4];		// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)
		FLOAT fRectPosY[4];		// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)
		FLOAT fRectPosZ[4];		// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)

		// Row & Col
		INT_PTR nRow;
		INT_PTR nCol;
		INT_PTR nRowLength;
		INT_PTR nColLength;

		// Position
		FLOAT fOriginX;
		FLOAT fOriginY;
		FLOAT fOriginZ;

		// Orientation
		FLOAT fRowOrientX;
		FLOAT fRowOrientY;
		FLOAT fRowOrientZ;

		FLOAT fColOrientX;
		FLOAT fColOrientY;
		FLOAT fColOrientZ;

		FLOAT fOrthoOrientX;
		FLOAT fOrthoOrientY;
		FLOAT fOrthoOrientZ;

		FLOAT m_fPixelXSpacing;
		FLOAT m_fPixelYSpacing;
		FLOAT m_fImagerXSpacing;
		FLOAT m_fImagerYSpacing;

		void Init()
		{
			for (INT_PTR i = 0; i < 4; i++)
			{
				fRectPosX[i] = 0.0;
				fRectPosY[i] = 0.0;
				fRectPosZ[i] = 0.0;
			}

			nRow = 0;
			nCol = 0;
			nRowLength = 0;
			nColLength = 0;

			// Position
			fOriginX = 0.0;
			fOriginY = 0.0;
			fOriginZ = 0.0;

			// Orientation
			fRowOrientX = 0.0;
			fRowOrientY = 0.0;
			fRowOrientZ = 0.0;

			fColOrientX = 0.0;
			fColOrientY = 0.0;
			fColOrientZ = 0.0;

			fOrthoOrientX = 0.0;
			fOrthoOrientY = 0.0;
			fOrthoOrientZ = 0.0;

			m_fPixelXSpacing = 0.0;
			m_fPixelYSpacing = 0.0;
			m_fImagerXSpacing = 0.0;
			m_fImagerYSpacing = 0.0;
		}

	}LOCALIZER_IMAGE_INFO_;

// 	typedef struct _THIRD_DIMENSION_POINT_
// 	{
// 		// Position
// 		float fX;
// 		float fY;
// 		float fZ;
// 
// 		_THIRD_DIMENSION_POINT_()
// 		{
// 			Init();
// 		}
// 
// 		void Init()
// 		{
// 			fX = 0.0;
// 			fY = 0.0;
// 			fZ = 0.0;
// 		}
// 
// 		void Set(float x, float y, float z)
// 		{
// 			fX = x;
// 			fY = y;
// 			fZ = z;
// 		}
// 
// 		void operator=(const _THIRD_DIMENSION_POINT_& obj)
// 		{
// 			fX = obj.fX;
// 			fY = obj.fY;
// 			fZ = obj.fZ;
// 		}
// 
// 	} POINT_3D_;

public:
	CLocalizerCore();
	~CLocalizerCore();

protected:
	CArray<CStudyViewer*, CStudyViewer*> m_aryStudyViewer;
	INT_PTR m_nSelectedIndex;

public:
	void Init();
	// Set
	void SetSelectedIndex(INT_PTR nSelectedIndex);
	
	// Add
	void AddStudyViewer(CStudyViewer* pStudyViewer);	

	// Do
	void CalcReferenceLine();

private:
	LOCALIZER_IMAGE_INFO_ GetLocalizerImageInfo(CLLDicomDS* dsSrcDicomDS, INT_PTR nFrameIndex);
	//void RotateBasedOnDestOrientation(LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo, FLOAT fSrcPosX, FLOAT fSrcPosY, FLOAT fSrcPosZ, FLOAT &fDestPosX, FLOAT &fDestPosY, FLOAT &fDestPosZ);
	void RotateBasedOnDestOrientation(LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo, double fSrcPosX, double fSrcPosY, double fSrcPosZ, double &fDestPosX, double &fDestPosY, double &fDestPosZ);
	BOOL IsSamePatientOrientation(LOCALIZER_IMAGE_INFO_ stSrcLocalizerImageInfo, LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo);
	//BOOL CalcContactZSurface(CArray<CLocalizerCore::POINT_3D_, CLocalizerCore::POINT_3D_> *ary3DPoint, Gdiplus::PointF &ptStart, Gdiplus::PointF &ptEnd);
	//double CalcDistance(CLocalizerCore::POINT_3D_ ptSrc, CLocalizerCore::POINT_3D_ ptDest);
	//CLocalizerCore::POINT_3D_ GetInterSectionWithZ0(CLocalizerCore::POINT_3D_ ptStart, CLocalizerCore::POINT_3D_ ptEnd);
};

