#pragma once
#include <afxsock.h>		// extensions du socket MFC
//#include "resource.h"

//V2 pointeurs sur les fonctions (evite les sendmessages)
//#define WM_MESSRECVVALUESOCKET	WM_USER+102
//#define WM_MESSCONNEXIONSOCKET	WM_USER+103
//#define WM_MESSDECONNEXIONSOCKET		WM_USER+104

//V2 Allocations des clients dans application
//#define NB_MAX_CLIENTS	10

//LEUZE V 2.0
//#define ID_TIMER_TIMEOUT_SOCKET	3013
//#define MODULESOCKET_TIMEOUT_COM	500
//
//class CDynamicDlgSocket : public CDialog
//{
//	// Construction
//public:
//
//	void * ModuleParent;
//	UINT nID;
//	CEdit EditBox;
//	DECLARE_DYNCREATE(CDynamicDlgSocket)
//	CDynamicDlgSocket();
//	CDynamicDlgSocket(int x, int y, int cx, int cy, const char *szTitle, CWnd* pParent = NULL);// standard constructor
//
//	~CDynamicDlgSocket();
//
//	void SetDynamicDlg(int x, int y, int cx, int cy, const char *szTitle, CWnd* pParent = NULL);
//	// Implementation
//private:
//	HGLOBAL m_hgbl;
//protected:
//
//	DECLARE_MESSAGE_MAP()
//public:
//	virtual BOOL OnInitDialog();
//	LRESULT OnReceive(WPARAM Param1, LPARAM Param2);
//	void DoEvents(void);
//	void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnDestroy();
//};

#define ATTENTE_THREAD_PING	5000 //ms -> 5 secondes

DWORD WINAPI ThreadServeur(LPVOID pParam);
DWORD WINAPI ThreadEventsSocket(LPVOID pParam);
DWORD WINAPI ThreadPingAlive(LPVOID pParam);

class CClientSocket
{
public:
	CClientSocket();
	~CClientSocket();

	//LEUZE V 2.0 
	SOCKET sockAccept;

	char buffRecv[300];
	int BufflenRecv;

	CString strIP;
	bool bIsConnected;

	WSAEVENT NewEvent;
	DWORD			ThreadID;
	bool bStopThread;
	HANDLE H_ThreadEvent;

	bool bDoThreadPing;
	DWORD			ThreadIDPing;
	HANDLE H_ThreadPing;

	//V2 HWND  HwndParent;
	/*
	void (*pExecuteOnDeconnect)(void):
	executée lors d'une deconnexion sur le serveur

	le clientsoket n'a pas besoin d'être deconnecter, c'est fait dans le thread de gestion

	remplace WM_MESSDECONNEXIONSOCKET
	*/
	void(*pExecuteOnDeconnect)(void);
	/*
	void (*pExecuteOnValueReceive)(CString strReceive, CString strIpReceive):
	executée lors d'une reception de donnée sur le socket
	l'adresse IP est passée dans le cas ou une fonction générale est appelée du coté de l'application (pour retrouver le client concerné si multi-clients)

	remplace WM_MESSRECVVALUESOCKET
	*/
	void(*pExecuteOnValueReceive)(CString strReceive, CString strIpReceive);

	//SockV3
	/*
	void (*pExecuteOnArretThread)(void):
	executée lors de l'arret du thread de gestion du client (sur une erreur par exemple)
	*/
	void(*pExecuteOnArretThread)(void);

	
	int LireReponseTrame(int NbRecv);
	bool IsConnected();
	void Deconnecter();
	void Connecter(CString strIpConnexion, SOCKET SockAccept);

};
class CServSocketDMX
{
public:
	CServSocketDMX();
	~CServSocketDMX();

	//variables pour socket
	int iPort;
	//LEUZE V 2.0 CDynamicDlgSocket * DialAffRecv;
	sockaddr_in clientService;
	SOCKET sock;
	WSAEVENT NewEvent;
	DWORD			ThreadID;
	HANDLE H_ThreadServeur;
	bool bStopThread;

	int iErreur;//0 pas d'erreur
	CString str_Erreur;
	
	int iNbClientsConnected;
	//V2 Allocations des clients dans application
	//CClientSocket Clients[NB_MAX_CLIENTS];
	bool bIsConnected;

	//V2 pointeurs sur les fonctions (evite les sendmessages)
	//HWND  HwndParent;

	/*
	CClientSocket * (*pExecuteOnConnect)(CString strIpConnexion):
	executée lors de la demande de connexion sur le serveur
	la fonction doit retourner NULL si la connexion est refusée
	et un CClientSocket alloué si la connexion est acceptée

	remplace WM_MESSCONNEXIONSOCKET
	*/
	CClientSocket * (*pExecuteOnConnect)(CString strIpConnexion);

	bool StartServeur(int Port);
	int Deconnecter(void);
	int GetError(void);
	
};

