
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#define LTV17_CONFIG
#include "ltwrappr.h"
#include "ltdic.h"
#include "ltlck.h"
#include "Ltdis.h"

#ifdef _WIN64
#pragma comment(lib, "ltwvc_x.lib")
#pragma comment(lib, "ltdic_x.lib")	
#pragma comment(lib, "ltkrn_x.lib")
#pragma comment(lib, "ltdis_x.lib")
#else
#pragma comment(lib, "ltwvc_u.lib")
#pragma comment(lib, "ltdic_u.lib")
#pragma comment(lib, "ltkrn_u.lib")
#pragma comment(lib, "ltdis_u.lib")
#endif

#define Bits2Bytes(n)			(((int)(n%8)==0) ? ((int)(n/8)) : ((int)(n/8)+1))
#define Bits2MaxValue(n)		((1<<n) - 1)
#define Bits2TotalCount(n)		(1<<n)

// gdi+
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// ID Collection
// CWnd ID
#define	ID_LAYOUT_MANAGER	10100
#define	ID_LAYOUT_VIEWER	11000

#define DEFAULT_LAYOUT_COUNT	1
#define MAX_LAYOUT_COUNT		16


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


