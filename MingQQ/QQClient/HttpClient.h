#pragma once

#include <WinInet.h>
#include <string>
#include <vector>

#pragma comment(lib, "wininet.lib")

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

// IE8 on Windows 7
#define		IE8_USER_AGENT	 _T("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)")

enum HTTP_REQ_METHOD	// HTTP���󷽷�
{
	REQ_METHOD_GET,
	REQ_METHOD_POST,
};

enum HTTP_STATUS_EVENT	// HTTP״̬�¼���ʶ
{
	CONNECTED_EVENT,
	REQUEST_OPENED_EVENT,
	REQUEST_COMPLETE_EVENT,
	USER_CANCEL_EVENT
};

class CHttpClient		// WinInet Http�첽��װ��
{
public:
	CHttpClient(void);
	~CHttpClient(void);

public:
	BOOL OpenRequest(LPCTSTR lpszUrl, HTTP_REQ_METHOD nReqMethod = REQ_METHOD_GET);	// ��HTTP������
	BOOL AddReqHeaders(LPCTSTR lpHeaders);				// ���һ������HTTP����ͷ����
	BOOL SendRequest(const CHAR * lpData, DWORD dwLen);	// ����HTTP������
	BOOL SendRequestEx(DWORD dwLen);					// ��ʼ����HTTP������
	BOOL SendReqBodyData(const CHAR * lpBuf, DWORD dwLen, DWORD& dwSendLen);	// ����HTTP������Ϣ�����ݺ���
	BOOL EndSendRequest();								// ��������HTTP������
	DWORD GetRespCode();								// ��ȡHTTP��Ӧ�뺯��
	tstring GetRespHeader();							// ��ȡȫ��HTTPͷ
	tstring GetRespHeader(LPCTSTR lpszName, int nIndex = 0);	// ��ȡHTTPͷָ���ֶ����Ƶ�ֵ
	int GetRespHeaderByInt(LPCTSTR lpszName, int nIndex = 0);	// ��ȡHTTPͷָ���ֶ����Ƶ�ֵ(Int)
	BOOL GetRespBodyData(CHAR * lpBuf, DWORD dwLen, DWORD& dwRecvLen);	// ��ȡHTTP��Ӧ��Ϣ�����ݺ���
	void CloseRequest();								// �ر�HTTP������

	void SetConnectTimeOut(DWORD dwTimeOut);			// �������ӳ�ʱ(��λ������)
	void SetCancalEvent();								// ����ȡ���¼�����
	void SetProxy(LPCTSTR lpszServer, WORD nPort,		// ����HTTP���������
		LPCTSTR lpszUser = NULL, LPCTSTR lpszPwd = NULL);

private:
	static void __stdcall StatusCallback(HINTERNET hInternet, DWORD dwContext,	// ״̬�ص�����
		DWORD dwInternetStatus, LPVOID lpStatusInfo, DWORD dwStatusInfoLen);

	BOOL ParseURL(LPCTSTR lpszUrl, LPTSTR lpszScheme, DWORD dwSchemeLength,
		LPTSTR lpszHostName, DWORD dwHostNameLength, WORD& nPort, 
		LPTSTR lpszUrlPath, DWORD dwUrlPathLength);		// ����Url����(Э�飬���������˿ڣ��ļ�·��)
	BOOL WaitForEvent(HTTP_STATUS_EVENT nEvent, DWORD dwTimeOut);	// �ȴ��¼�����

	DWORD __GetRespHeaderLen();							// ��ȡHTTP��Ӧͷ����
	BOOL __GetRespHeader();								// ��ȡHTTP��Ӧͷ�����б�����m_arrRespHeader����

private:
	HINTERNET m_hInternet;					// �Ự���
	HINTERNET m_hConnect;					// ���Ӿ��
	HINTERNET m_hRequest;					// ������

	HANDLE m_hCompleteEvent;				// ��������¼����
	HANDLE m_hCancelEvent;					// �ⲿȡ���¼����

	DWORD m_dwConnectTimeOut;				// ���ӳ�ʱʱ��
	DWORD m_dwContext;						// ��ǰ����������

	std::vector<tstring> m_arrRespHeader;	// Http��Ӧͷ����

	tstring m_strProxy;						// ���������(������:�˿�)
	tstring m_strUser, m_strPwd;			// ������������û���������
};
