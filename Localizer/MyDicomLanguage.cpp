#include "StdAfx.h"
#include "MyDicomLanguage.h"

#include <ASSERT.h>
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// static char* ISO_2022_ESC[] =
// {
// 	"\x1B\x28\x42",		//  0: ISO 2022 IR 6: ESC ( B
// 	"\x1B\x2D\x41",		//  1: ISO 2022 IR 100: ESC - A
// 	"\x1B\x2D\x42",		//  2: ISO 2022 IR 101: ESC - B
// 	"\x1B\x2D\x43",		//  3: ISO 2022 IR 109: ESC - C
// 	"\x1B\x2D\x44",		//  4: ISO 2022 IR 110: ESC - D
// 	"\x1B\x2D\x4C",		//  5: ISO 2022 IR 144: ESC - L
// 	"\x1B\x2D\x47",		//  6: ISO 2022 IR 127: ESC - G
// 	"\x1B\x2D\x46",		//  7: ISO 2022 IR 126: ESC - F
// 	"\x1B\x2D\x48",		//  8: ISO 2022 IR 138: ESC - H
// 	"\x1B\x2D\x4D",		//  9: ISO 2022 IR 148: ESC - M
// 	"\x1B\x29\x49",		// 10: ISO 2022 IR 13: ESC ) I
// 	"\x1B\x2D\x54",		// 11: ISO 2022 IR 166: ESC -
// 	"\x1B\x28\x4A",		// 12: ISO 2022 IR 14: ESC ( J
// 	"\x1B\x24\x42",		// 13: ISO 2022 IR 87: ESC $ B
// 	"\x1B\x24\x28\x44",	// 14: ISO 2022 IR 159: ESC $ ( D
// 	"\x1B\x24\x29\x43",	// 15: ISO 2022 IR 149: ESC $ ) C
// 	"",					// 16: UTF-8
// 	"",					// 17: Chinese GB18030
// 	"\x1B\x24\x29\x41", // 18: Chinese GB2312-80
// };


CMyDicomLanguage::CMyDicomLanguage()
{
	m_aryIso2022Esc.Add("\x1B\x28\x42");		//  0: ISO 2022 IR 6: ESC ( B
	m_aryIso2022Esc.Add("\x1B\x2D\x41");		//  1: ISO 2022 IR 100: ESC - A
	m_aryIso2022Esc.Add("\x1B\x2D\x42");		//  2: ISO 2022 IR 101: ESC - B
	m_aryIso2022Esc.Add("\x1B\x2D\x43");		//  3: ISO 2022 IR 109: ESC - C
	m_aryIso2022Esc.Add("\x1B\x2D\x44");		//  4: ISO 2022 IR 110: ESC - D
	m_aryIso2022Esc.Add("\x1B\x2D\x4C");		//  5: ISO 2022 IR 144: ESC - L
	m_aryIso2022Esc.Add("\x1B\x2D\x47");		//  6: ISO 2022 IR 127: ESC - G
	m_aryIso2022Esc.Add("\x1B\x2D\x46");		//  7: ISO 2022 IR 126: ESC - F
	m_aryIso2022Esc.Add("\x1B\x2D\x48");		//  8: ISO 2022 IR 138: ESC - H
	m_aryIso2022Esc.Add("\x1B\x2D\x4D");		//  9: ISO 2022 IR 148: ESC - M
	m_aryIso2022Esc.Add("\x1B\x29\x49");		// 10: ISO 2022 IR 13: ESC ) I
	m_aryIso2022Esc.Add("\x1B\x2D\x54");		// 11: ISO 2022 IR 166: ESC -
	m_aryIso2022Esc.Add("\x1B\x28\x4A");		// 12: ISO 2022 IR 14: ESC ( J
	m_aryIso2022Esc.Add("\x1B\x24\x42");		// 13: ISO 2022 IR 87: ESC $ B
	m_aryIso2022Esc.Add("\x1B\x24\x28\x44");	// 14: ISO 2022 IR 159: ESC $ ( D
	m_aryIso2022Esc.Add("\x1B\x24\x29\x43");	// 15: ISO 2022 IR 149: ESC $ ) C
	m_aryIso2022Esc.Add("");					// 16: UTF-8
	m_aryIso2022Esc.Add("");					// 17: Chinese GB18030
	m_aryIso2022Esc.Add("\x1B\x24\x29\x41");	// 18: Chinese GB2312-80
}

CMyDicomLanguage::~CMyDicomLanguage()
{
}

CStringA CMyDicomLanguage::EncodeNLS(const CStringA& strInValue, int nNLS)
{
	AfxDebugBreak();
	CStringA strOutValue;

	switch (nNLS)
	{
	case DCM_NLS_ISO_IR_6:
	case DCM_NLS_ISO_IR_100:
	case DCM_NLS_ISO_IR_101:
	case DCM_NLS_ISO_IR_109:
	case DCM_NLS_ISO_IR_110:
	case DCM_NLS_ISO_IR_144:
	case DCM_NLS_ISO_IR_127:
	case DCM_NLS_ISO_IR_126:
	case DCM_NLS_ISO_IR_138:
	case DCM_NLS_ISO_IR_148:
	case DCM_NLS_ISO_IR_13:
	case DCM_NLS_ISO_IR_166:
	case DCM_NLS_ISO_IR_14:
		return strInValue;
	case DCM_NLS_ISO_IR_192:
		{
			//#ifndef _UNICODE
		AfxDebugBreak();
			const char* pMB = strInValue;
			wchar_t* pWC = new wchar_t[strInValue.GetLength()+1];
			int nLength = MultiByteToWideChar(CP_OEMCP, 0, pMB, strInValue.GetLength(), pWC, strInValue.GetLength()+1);
			char* pUTF8 = new char[nLength*4+1];
			nLength = WideCharToMultiByte(CP_UTF8, 0, pWC, nLength, pUTF8, nLength*4, NULL, NULL);
			strOutValue = pUTF8;
			delete[] pUTF8;
			delete[] pWC;
			//#endif
		}
		return strOutValue;
	}

	if (nNLS == DCM_NLS_GB18030)
	{
		AfxDebugBreak();
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	const BYTE* pIn = (const BYTE*)(const char*)strInValue;
	int nNLS1 = (nNLS >> 0) & 0xFF;
	int nNLS2 = (nNLS >> 8) & 0xFF;
	int nNLS3 = (nNLS >> 16) & 0xFF;
	UINT nCurrentNLS = nNLS1;

	while (*pIn)
	{
		if (*pIn == LF || *pIn == FF || *pIn == CR || (*pIn >= 0x20 && *pIn <= 0x7F))
		{
			ASSERT(nNLS1 == DCM_NLS_ISO_IR_6 || nNLS1 == DCM_NLS_ISO_IR_13);
			ASSERT(nNLS2 != DCM_NLS_ISO_IR_6);
			if (nNLS1 == DCM_NLS_ISO_IR_6 && nCurrentNLS != DCM_NLS_ISO_IR_6)
			{
				if (nNLS2 != DCM_NLS_ISO_IR_149)
					//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_6];
					strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_6);
				nCurrentNLS = DCM_NLS_ISO_IR_6;
			}
			else if (nNLS1 == DCM_NLS_ISO_IR_13 && nCurrentNLS != DCM_NLS_ISO_IR_13 && nCurrentNLS != DCM_NLS_ISO_IR_14)
			{
				//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_14];
				strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_14);
				nCurrentNLS = DCM_NLS_ISO_IR_14;
			}
			strOutValue += *pIn++;
			continue;
		}
		if (*pIn == 0xA0)
		{
			if (nNLS1 == DCM_NLS_ISO_IR_13 || nNLS2 == DCM_NLS_ISO_IR_13 || nNLS3 == DCM_NLS_ISO_IR_13 ||
				nNLS2 == DCM_NLS_ISO_IR_87 || nNLS3 == DCM_NLS_ISO_IR_87)
			{
				strOutValue += (const TCHAR*)(*pIn++ & 0x7F);	// Modified
				continue;
			}
		}
		if (*pIn >= 0xA1 && *pIn <= 0xDF)
		{
			if (nNLS1 == DCM_NLS_ISO_IR_13)
			{
				nCurrentNLS = DCM_NLS_ISO_IR_13;
				strOutValue += *pIn++;
				continue;
			}
			else if (nNLS2 == DCM_NLS_ISO_IR_13 || nNLS3 == DCM_NLS_ISO_IR_13)
			{
				if (nCurrentNLS != DCM_NLS_ISO_IR_13)
				{
					//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_13];
					strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_13);
					nCurrentNLS = DCM_NLS_ISO_IR_13;
				}
				strOutValue += *pIn++;
				continue;
			}
		}
		if ((*pIn >= 0x81 && *pIn <= 0x9F) || (*pIn >= 0xE0 && *pIn <= 0xEF))
		{
			ASSERT(nNLS1 != DCM_NLS_ISO_IR_87);
			if (nNLS2 == DCM_NLS_ISO_IR_87 || nNLS3 == DCM_NLS_ISO_IR_87)
			{
				if (nCurrentNLS != DCM_NLS_ISO_IR_87)
				{
					//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_87];
					strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_87);
					nCurrentNLS = DCM_NLS_ISO_IR_87;
				}
				BYTE c1 = *pIn++;
				BYTE c2 = *pIn++;
				int nA = c2 < 159;
				int nR = c1 < 160 ? 112 : 176;
				int nC = nA ? (c2 > 127 ? 32 : 31) : 126;
				/**********************************************************************/
				// strOutValue += ((((c1 - nR) << 1) - nA + 128) & 0x7F);	// Original
				// strOutValue += ((c2 - nC + 128) & 0x7F);
				strOutValue += (char)((((c1 - nR) << 1) - nA + 128) & 0x7F);// Modified
				strOutValue += (char)((c2 - nC + 128) & 0x7F);
				/**********************************************************************/
				continue;
			}
		}
		if (*pIn >= 0xA0 && *pIn <= 0xFF)
		{
			ASSERT(nNLS1 != DCM_NLS_ISO_IR_149 && nNLS1 != DCM_NLS_ISO_IR_159);
			ASSERT(nNLS3 != DCM_NLS_ISO_IR_149 && nNLS3 != DCM_NLS_ISO_IR_159);
			if (nNLS2 == DCM_NLS_ISO_IR_149)
			{
				if (nCurrentNLS != DCM_NLS_ISO_IR_149)
				{
					//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_149];
					strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_149);
					nCurrentNLS = DCM_NLS_ISO_IR_149;
				}
				strOutValue += *pIn++;
				strOutValue += *pIn++;
				continue;
			}
			/**********************************************************************/
			else if (nNLS1 == DCM_NLS_ISO_IR_13 || nNLS2 == DCM_NLS_ISO_IR_13 || nNLS3 == DCM_NLS_ISO_IR_13 ||
				nNLS2 == DCM_NLS_ISO_IR_87 || nNLS3 == DCM_NLS_ISO_IR_87)
			{
				if (nCurrentNLS != DCM_NLS_ISO_IR_159)
				{
					//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_159];
					strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_159);
					nCurrentNLS = DCM_NLS_ISO_IR_159;
				}
				strOutValue += *pIn++;
				strOutValue += *pIn++;
				continue;
			}
			else
			{
				pIn++;
			}
			/**********************************************************************/
		}
		else
		{
			pIn++;
		}
	}
	if (nNLS1 == DCM_NLS_ISO_IR_6 && nCurrentNLS != DCM_NLS_ISO_IR_6)
	{
		if (nNLS2 != DCM_NLS_ISO_IR_149)
			//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_6];
			strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_6);
		nCurrentNLS = DCM_NLS_ISO_IR_6;
	}
	else if (nNLS1 == DCM_NLS_ISO_IR_13 && nCurrentNLS != DCM_NLS_ISO_IR_13 && nCurrentNLS != DCM_NLS_ISO_IR_14)
	{
		//strOutValue += ISO_2022_ESC[DCM_NLS_ISO_IR_14];
		strOutValue += m_aryIso2022Esc.GetAt((int)DCM_NLS_ISO_IR_14);
		nCurrentNLS = DCM_NLS_ISO_IR_14;
	}
	return strOutValue;
}

CStringA CMyDicomLanguage::DecodeNLS(const CStringA& strInValue, int nNLS)
{
	AfxDebugBreak();

	return "";
}

CString CMyDicomLanguage::DecodeNLSEx(const CStringA& strInValue, int nNLS)
{
	CString strOutValue;

	///////////////////////////////////////////////////////////////////////////////////////////
	if (nNLS == DCM_NLS_ISO_IR_192)
	{
		const char* pUTF8 = strInValue;
		int nLength = strInValue.GetLength();
		wchar_t* pWC = new wchar_t[nLength+1];
		nLength = ::MultiByteToWideChar(CP_UTF8, 0, pUTF8, nLength, pWC, nLength);
		pWC[nLength] = '\0';
		strOutValue = pWC;
		delete[] pWC;
		return strOutValue;
	}
	else if (nNLS == DCM_NLS_GB18030)
	{
		const char* pSrcMB = strInValue;
		int nLength = strInValue.GetLength();
		wchar_t* pWC = new wchar_t[nLength+1];
		ZeroMemory(pWC, nLength+1);
		nLength = ::MultiByteToWideChar(54936, 0, pSrcMB, nLength, pWC, nLength);
		pWC[nLength] = '\0';
		strOutValue = pWC;
		delete[] pWC;
		//
		return strOutValue;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	const BYTE* pIn = (const BYTE*)(const char*)strInValue;
	UINT nCurrentNLS = DCM_NLS_ISO_IR_6;

	CStringA strOutValueA;

	BOOL bJISIncluded = FALSE;
	BOOL bGBIncluded = FALSE;
	while (*pIn)
	{
		if (*pIn == ESC)
		{
			for (int nIndex = 0; nIndex < DCM_NLS_COUNT; nIndex++)
			{
				CStringA strEsc = m_aryIso2022Esc.GetAt(nIndex);
				if (!strEsc.GetLength())
					continue;

				if (memcmp(pIn, strEsc.GetBuffer(), strEsc.GetLength()) == 0)
				//if (memcmp(pIn, ISO_2022_ESC[nIndex], strlen(ISO_2022_ESC[nIndex])) == 0)
				{
					nCurrentNLS = nIndex;
					break;
				}
			}
			while (*pIn)
			{
				if (*pIn++ >= 0x40)
					break;
			}
		}
		else
		{
			switch (nCurrentNLS)
			{
			case DCM_NLS_ISO_IR_6:			// ISO 646 (G0)
			case DCM_NLS_ISO_IR_13:			// JIS X 0201 (G1)
			case DCM_NLS_ISO_IR_14:			// JIS X 0201 (G0)
			case DCM_NLS_ISO_IR_149:		// KS X 1001 (G1)
			case DCM_NLS_ISO_IR_159:		// JIS X 0212 (G0)
			case DCM_NLS_ISO_IR_58:			// GB2312-80
			default:
				{
					strOutValueA += (*pIn++);
				}
				break;
			case DCM_NLS_ISO_IR_87:		// JIS X 0208 (G0)
				{
					BYTE c1 = *pIn++;
					BYTE c2 = *pIn++;
					int nR = c1 < 95 ? 112 : 176;
					int nC = c1 % 2 ? (c2 > 95 ? 32 : 31) : 126;
					strOutValueA += (char)(((c1 + 1) >> 1) + nR);	// Modified
					strOutValueA += (char)(c2 + nC);
				}
				break;
			}

			// Check JIS from Escape Sequence
			switch (nCurrentNLS)
			{
			case DCM_NLS_ISO_IR_13:
			case DCM_NLS_ISO_IR_14:
			case DCM_NLS_ISO_IR_159:
			case DCM_NLS_ISO_IR_87:
				{
					bJISIncluded = TRUE;
				}
				break;
			case DCM_NLS_ISO_IR_58:
				{
					bGBIncluded = TRUE;
				}
				break;
			}
			// Check NLS from Specific Char Set
			switch (nNLS)
			{
			case DCM_NLS_ISO_IR_13:
			case DCM_NLS_ISO_IR_14:
			case DCM_NLS_ISO_IR_159:
			case DCM_NLS_ISO_IR_87:
				{
					bJISIncluded = TRUE;
				}
				break;
			case DCM_NLS_ISO_IR_58:
				{
					bGBIncluded = TRUE;
				}
				break;
			}
		}
	}

	if (bJISIncluded)
	{
		const char* pMB = (LPSTR)(LPCSTR)strOutValueA;
		WCHAR szWideChar[65535] = {0, };
		int nLength = ::MultiByteToWideChar(932, 0, pMB, strlen(pMB), szWideChar, 65535);
		strOutValue.Format(_T("%s"), szWideChar);
		strOutValue.TrimRight();
	}
	else if (bGBIncluded)
	{
		const char* pMB = (LPSTR)(LPCSTR)strOutValueA;
		WCHAR szWideChar[65535] = {0, };
		int nLength = ::MultiByteToWideChar(20936, 0, pMB, strlen(pMB), szWideChar, 65535);
		strOutValue.Format(_T("%s"), szWideChar);
		strOutValue.TrimRight();
	}
	else
	{
		//strOutValue = CA2W(strOutValueA);
		//strOutValue.Trim();
		const char* pMB = (LPSTR)(LPCSTR)strOutValueA;
		WCHAR szWideChar[65535] = {0, };
		int nLength = ::MultiByteToWideChar(CP_ACP, 0, pMB, strlen(pMB), szWideChar, 65535);
		strOutValue.Format(_T("%s"), szWideChar);
		strOutValue.Trim();
	}

	return strOutValue;
}