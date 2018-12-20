#include "StdAfx.h"
#include "XRLink.Clinic.MemoryDump.h"
#include "DbgHelp.h"

CXRLinkClinicMemoryDump* g_pMemoryDump = nullptr;

CXRLinkClinicMemoryDump::CXRLinkClinicMemoryDump(void)
{
	::SetUnhandledExceptionFilter(CXRLinkClinicMemoryDump::ExceptionFilter);
	g_pMemoryDump = this;
}

CXRLinkClinicMemoryDump::CXRLinkClinicMemoryDump(CString _folderPath)
{
	::SetUnhandledExceptionFilter(CXRLinkClinicMemoryDump::ExceptionFilter);
	dumpFileFolder = _folderPath;
	g_pMemoryDump = this;
}

CXRLinkClinicMemoryDump::~CXRLinkClinicMemoryDump(void)
{
}

LONG CXRLinkClinicMemoryDump::ExceptionFilter(EXCEPTION_POINTERS* _pEp)
{
	if(g_pMemoryDump)
	{
		g_pMemoryDump->DumpMemory(_pEp);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL CXRLinkClinicMemoryDump::DumpMemory(EXCEPTION_POINTERS* _pEp)
{
	// AfxMessageBox(_T("DumpMemory called"));
	CTime currentTime = CTime::GetCurrentTime();
	CString fileName;
	
	// 추가한 이름이 있으면 그 이름을 사용합니다.
	if ( dumpApplicationName.IsEmpty() &&
		 dumpPlatformType.IsEmpty() )
	{
		fileName.Format(_T("%s.dmp"), currentTime.Format(_T("%Y_%m_%d_%H_%M_%S")));	
	}
	else
	{
		fileName.Format( _T("%s_%s_%s.dmp"), dumpApplicationName, currentTime.Format(_T("%Y_%m_%d_%H_%M_%S")), dumpPlatformType );
	}
	
	CFile	file;
	CFileException fileEx;
	BOOL bRet = FALSE;
	bRet = file.Open(fileName,CFile::modeCreate | CFile::modeReadWrite,&fileEx);
	if (bRet)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = _pEp;
		mdei.ClientPointers = FALSE;
		MINIDUMP_TYPE mdt = MiniDumpWithFullMemory;

		// 
		bRet = MiniDumpWriteDump(::GetCurrentProcess(),
								 ::GetCurrentProcessId(),
								 file.m_hFile,
								 mdt,
								 (_pEp != 0) ? &mdei : 0,
								 0,
								 0);
		if(bRet)
		{
			// AfxMessageBox(_T("Dump succeed"));
			return TRUE;
		}
		else
		{
			// AfxMessageBox(_T("Dump failed"));
		}
		file.Close();
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("Failed to create dumpe file (%s)"), fileName);
		AfxMessageBox(strMsg, MB_ICONERROR);
		//AfxMessageBox(_T("File Open error"));
	}
	return FALSE;
}