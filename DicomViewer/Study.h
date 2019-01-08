#pragma once
#include "afx.h"
#include "Series.h"

class CStudy :
	public CObject
{
	friend class CSeries;

public:
	CStudy(CLLDicomDS dsDicomDS);
	CStudy();
	~CStudy();

public:
	CArray<INT_PTR, INT_PTR> m_arrIndexDisplayStudyViewer;

protected:
	CLLDicomDS m_dsDicomDS;
	CArray<CSeries*, CSeries*> m_arrSeries;

public:
	BOOL IsSameStudyID(CString strStudyId);

	// Add
	void AddSeries(CLLDicomDS& dsLLDicomDS);
	void AddStudyViewerIndex(INT_PTR nStudyViewerIndex);

	// Update
	void UpdateDicomDs(CLLDicomDS dsLLDicom);

	// Get
	CLLDicomDS GetLLDicomDS();
	CString GetStudyId();
	CArray<CSeries*, CSeries*>* GetSeriesArray();

	CStudy& operator=(const CStudy& obj);

protected:
	void Init();
};

