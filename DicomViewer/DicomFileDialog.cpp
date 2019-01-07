#include "stdafx.h"
#include "DicomFileDialog.h"


CDicomFileDialog::CDicomFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
	DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_ofn.lpstrFilter = _T("DICOM Files (*.dcm;*.dic)\0*.dcm;*.dic\0All Files (*.*)\0*.*\0\0");
	m_ofn.lpstrDefExt = _T("*.dcm");
	m_ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
}


CDicomFileDialog::~CDicomFileDialog()
{
}
