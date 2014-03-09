#pragma once

#include <vector>
#include <string>
#include <time.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

WCHAR * AnsiToUnicode(const CHAR * lpszStr);
CHAR * UnicodeToAnsi(const WCHAR * lpszStr);
CHAR * AnsiToUtf8(const CHAR * lpszStr);
CHAR * Utf8ToAnsi(const CHAR * lpszStr);
CHAR * UnicodeToUtf8(const WCHAR * lpszStr);
WCHAR * Utf8ToUnicode(const CHAR * lpszStr);

BOOL AnsiToUnicode(const CHAR * lpszAnsi, WCHAR * lpszUnicode, int nLen);
BOOL UnicodeToAnsi(const WCHAR * lpszUnicode, CHAR * lpszAnsi, int nLen);
BOOL AnsiToUtf8(const CHAR * lpszAnsi, CHAR * lpszUtf8, int nLen);
BOOL Utf8ToAnsi(const CHAR * lpszUtf8, CHAR * lpszAnsi, int nLen);
BOOL UnicodeToUtf8(const WCHAR * lpszUnicode, CHAR * lpszUtf8, int nLen);
BOOL Utf8ToUnicode(const CHAR * lpszUtf8, WCHAR * lpszUnicode, int nLen);

std::wstring AnsiToUnicode(const std::string& strAnsi);
std::string UnicodeToAnsi(const std::wstring& strUnicode);
std::string AnsiToUtf8(const std::string& strAnsi);
std::string Utf8ToAnsi(const std::string& strUtf8);
std::string UnicodeToUtf8(const std::wstring& strUnicode);
std::wstring Utf8ToUnicode(const std::string& strUtf8);

BOOL ToHexStr(const CHAR * lpStr, int nSrcLen, CHAR * lpHex, int nDestLen);
BOOL StrToHex(const CHAR * lpStr, CHAR * lpHex, int nLen);

u_short Swap16(u_short nValue);		// 16λ�������������ֽ����������ֽ���ת��
u_long Swap32(u_long nValue);		// 32λ�������������ֽ����������ֽ���ת��

COLORREF HexStrToRGB(LPCTSTR lpszStr);
void RGBToHexStr(COLORREF color, TCHAR * lpBuf, int nLen);

void FormatTime(time_t lTime, LPCTSTR lpFmt, TCHAR * lpBuf, int nLen);
BOOL IsToday(time_t lTime);

BOOL IsToday(SYSTEMTIME * lpTime);
BOOL GetFileLastWriteTime(LPCTSTR lpszFileName, SYSTEMTIME * lpSysTime);	// ��ȡ�ļ�����޸�ʱ��

BOOL File_ReadAll(const TCHAR * lpszFileName, CHAR **lpData, LONG *lSize);	// ��ȡ�����ļ�

BOOL my_isalnum(unsigned char cChar);		// ���ָ���ַ��Ƿ�����ĸ(A-Z��a-z)������(0-9)
BOOL my_isalnum(wchar_t cChar);				// ���ָ���ַ��Ƿ�����ĸ(A-Z��a-z)������(0-9)

BOOL EnumSysFont(std::vector<tstring> * arrSysFont);	// ö��ϵͳ����
BOOL FlashWindowEx(HWND hWnd, int nCount);				// ��˸���ڱ�����
BOOL GetTrayWndRect(RECT * lpRect);						// ��ȡϵͳ����������

BOOL IsDigit(const WCHAR * lpStr);
void Replace(std::string& strText, const CHAR * lpOldStr, const CHAR * lpNewStr);
void Replace(std::wstring& strText, const WCHAR * lpOldStr, const WCHAR * lpNewStr);
WCHAR ToLower(WCHAR c);
void ToLower(WCHAR * lpText);
void ToLower(std::wstring& strText);
WCHAR ToUpper(WCHAR c);
void ToUpper(WCHAR * lpText);
void ToUpper(std::wstring& strText);
void EncodeHtmlSpecialChars(std::wstring& strText);		// ����Html�����ַ�
void DecodeHtmlSpecialChars(std::wstring& strText);		// ����Html�����ַ�

tstring GetBetweenString(const TCHAR * pStr, TCHAR cStart, TCHAR cEnd);
int GetBetweenInt(const TCHAR * pStr, TCHAR cStart, TCHAR cEnd, int nDefValue = 0);

tstring GetBetweenString(const WCHAR * pStr, const WCHAR * pStart, const WCHAR * pEnd);
int GetBetweenInt(const WCHAR * pStr, const WCHAR * pStart, 
				  const WCHAR * pEnd, int nDefValue = 0);

BOOL DllRegisterServer(LPCTSTR lpszFileName);
BOOL DllUnregisterServer(LPCTSTR lpszFileName);

tstring GetMimeTypeByExtension(const TCHAR * lpExtension);
GUID GetFileTypeGuidByExtension(const WCHAR * lpExtension);
CLSID GetEncoderClsidByExtension(const WCHAR * lpExtension);
CLSID GetEncoderClsidByFileType(REFGUID guidFileType);
CLSID GetEncoderClsidByMimeType(const WCHAR * lpMimeType);

time_t httpdate_parse(const TCHAR *date);