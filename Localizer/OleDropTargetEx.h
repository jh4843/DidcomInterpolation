#pragma once
#include "afxole.h"
#include "DicomViewer.h"



class COleDropTargetEx :
	public COleDropTarget
{
public:
	COleDropTargetEx(CWnd* pParentWnd);
	~COleDropTargetEx();

protected:
	CWnd* m_pParentWnd;
	static CString m_strStudyViewInfoPrefix;

protected:
	CStudy* ParseStudyViewerInfo(CString strData, INT_PTR& nCurSeriesIndex, INT_PTR& nCurInstanceIndex);

protected:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);

};

