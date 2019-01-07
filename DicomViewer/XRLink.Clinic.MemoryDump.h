#pragma once

#pragma comment(lib,"dbghelp.lib")

class CXRLinkClinicMemoryDump
{
private:
	CString		dumpFileFolder;
	CString		dumpFileName;
	CString		dumpApplicationName;
	CString		dumpPlatformType;
public:
	void	SetDumpFileFolder(CString _folderPath);
	CString	GetDumpFileFolder()	{	return dumpFileFolder;	}
	void	SetDumpFileName( CString strDumpFileName ) { dumpFileName = strDumpFileName; }
	void	SetDumpAppName( CString strAppName ) { dumpApplicationName = strAppName; }
	void	SetDumpPlatformType( CString strType ) { dumpPlatformType = strType; }
public:
	CXRLinkClinicMemoryDump(void);
	CXRLinkClinicMemoryDump(CString _folderPath);
	~CXRLinkClinicMemoryDump(void);

public:
	static LONG __stdcall ExceptionFilter(EXCEPTION_POINTERS* _pEp);
	BOOL DumpMemory(EXCEPTION_POINTERS* _pEp);
};

extern CXRLinkClinicMemoryDump* g_pMemoryDump;