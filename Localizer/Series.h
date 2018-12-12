#pragma once
#include "afx.h"
#include "LLDicomDS.h"
#include "Instance.h"

class CSeries :
	public CObject
{
public:
	CSeries(CLLDicomDS dsDicomDS);
	~CSeries();

protected:
	CLLDicomDS m_dsDicomDS;
	CArray<CInstance*, CInstance*> m_arrInstance;

public:
	BOOL IsSameSeriesID(CString strSeriesId);

	// Add
	void AddInstance(CLLDicomDS dsLLDicomDS);

	// Update
	void UpdateDicomDs(CLLDicomDS dsLLDicom);

	// Get
	CArray<CInstance*, CInstance*>* GetInstanceArray();
	CLLDicomDS* GetDicomDS();


	// 
	CSeries& operator=(const CSeries& obj);
};

