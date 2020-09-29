#pragma once
#include "afxinet.h"

class CComHTML
{
public:
	CComHTML(void);
	~CComHTML(void);

	CString strServeur;
	CString strHeaders;
	CString strRep;
	CString strErreur;
	bool IsInitialized;
	

	CHttpConnection* pConnection ;
	CInternetSession* pSession;
	CHttpFile* pFile;
	bool SendPOSTcmd(CStringA strCmd);
	bool SendGETcmd(CStringA strCmd);
	bool Connect(void);
	bool Disconnect(void);
	bool TraiterReponse(CString sChamp, CString sParam, CString* sValue);
	bool IsWebConnected(CString strWebSiteOrIP);

	long lSessionId;
};
