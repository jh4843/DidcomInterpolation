#pragma once
#include "afxole.h"
#include "StudyViewer.h"

class COleDataSourceEx : public COleDataSource
{
public:
	COleDataSourceEx();
	~COleDataSourceEx();

protected:
	static CObject* m_pDragSourceObject;

public:
	void SetDragSourceObject(CObject* pObject, CRect rtEffect = CRect(0,0,0,0));

private:
	BOOL CacheString(CLIPFORMAT cfFormat, LPCTSTR lpszText);
};

