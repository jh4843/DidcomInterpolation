#pragma once

#define	LF							((BYTE)'\x0A')
#define	FF							((BYTE)'\x0C')
#define	CR							((BYTE)'\x0D')
#define	ESC							((BYTE)'\x1B')
#define	SS1							((BYTE)'\x8E')
#define	SS2							((BYTE)'\x8F')

#define	DCM_NLS_ISO_IR_6			0
#define	DCM_NLS_ISO_IR_100			1
#define	DCM_NLS_ISO_IR_101			2
#define	DCM_NLS_ISO_IR_109			3
#define	DCM_NLS_ISO_IR_110			4
#define	DCM_NLS_ISO_IR_144			5
#define	DCM_NLS_ISO_IR_127			6
#define	DCM_NLS_ISO_IR_126			7
#define	DCM_NLS_ISO_IR_138			8
#define	DCM_NLS_ISO_IR_148			9
#define	DCM_NLS_ISO_IR_13			10
#define	DCM_NLS_ISO_IR_166			11
#define	DCM_NLS_ISO_IR_14			12
#define	DCM_NLS_ISO_IR_87			13
#define	DCM_NLS_ISO_IR_159			14
#define	DCM_NLS_ISO_IR_149			15
#define	DCM_NLS_ISO_IR_192			16
#define	DCM_NLS_GB18030				17
#define	DCM_NLS_ISO_IR_58			18

#define DCM_NLS_COUNT				19

#define	DCM_NLS_ISO_2022_IR_6_87	(DCM_NLS_ISO_IR_6 | DCM_NLS_ISO_IR_87 << 8)		// Japanese: ISO 646 + JIS X 0208
#define	DCM_NLS_ISO_2022_IR_13_87	(DCM_NLS_ISO_IR_13 | DCM_NLS_ISO_IR_87 << 8)	// Japanese: JIS X 0201 + JIS X 0208
#define	DCM_NLS_ISO_2022_IR_6_149	(DCM_NLS_ISO_IR_6 | DCM_NLS_ISO_IR_149 << 8)	// Korean: ISO 646 + KS X 1001

#define	DCM_NLS_DEFAULT				DCM_NLS_ISO_IR_6
#define	DCM_NLS_LATIN1				DCM_NLS_ISO_IR_100
#define	DCM_NLS_LATIN2				DCM_NLS_ISO_IR_101
#define	DCM_NLS_LATIN3				DCM_NLS_ISO_IR_109
#define	DCM_NLS_LATIN4				DCM_NLS_ISO_IR_110
#define	DCM_NLS_CYRILLIC			DCM_NLS_ISO_IR_144
#define	DCM_NLS_ARABIC				DCM_NLS_ISO_IR_127
#define	DCM_NLS_GREEK				DCM_NLS_ISO_IR_126
#define	DCM_NLS_HEBREW				DCM_NLS_ISO_IR_138
#define	DCM_NLS_LATIN5				DCM_NLS_ISO_IR_148
#define	DCM_NLS_JAPANESE0			DCM_NLS_ISO_IR_13
#define	DCM_NLS_THAI				DCM_NLS_ISO_IR_166
#define	DCM_NLS_JAPANESE1			DCM_NLS_ISO_2022_IR_6_87
#define	DCM_NLS_JAPANESE2			DCM_NLS_ISO_2022_IR_13_87
#define	DCM_NLS_JAPANESE			DCM_NLS_JAPANESE2
#define	DCM_NLS_KOREAN				DCM_NLS_ISO_2022_IR_6_149
#define	DCM_NLS_UNICODE_UTF8		DCM_NLS_ISO_IR_192
#define	DCM_NLS_CHINESE_GB18030		DCM_NLS_GB18030
#define	DCM_NLS_CHINESE_GB2312_80	DCM_NLS_ISO_IR_58

class CMyDicomLanguage
{
public:
	CMyDicomLanguage();
	~CMyDicomLanguage();

	CStringA EncodeNLS(const CStringA& strInValue, int NLS);
	CStringA DecodeNLS(const CStringA& strInValue, int nNLS = DCM_NLS_ISO_IR_6);
	CString DecodeNLSEx(const CStringA& strInValue, int nNLS = DCM_NLS_ISO_IR_6);

protected:
	CArray<CStringA,CStringA> m_aryIso2022Esc;
};
