#include "stdafx.h"
#include "ServSocketDMX.h"

//Ping
#include "WS2tcpip.h"
#include "Ipexport.h"
#include <icmpapi.h>

/////////////////////////////////////////////////////////////////////////////
// CDynamicDlgSocket dialog
//
//LEUZE V 2.0 
//IMPLEMENT_DYNCREATE(CDynamicDlgSocket, CDialog)
//
//CDynamicDlgSocket::CDynamicDlgSocket()
//{
//	m_hgbl = NULL;
//	nID = 2000;
//}
//CDynamicDlgSocket::CDynamicDlgSocket(int x, int y, int cx, int cy, const char *szTitle, CWnd* pParent /*=NULL*/)
//	: CDialog()
//{
//	//{{AFX_DATA_INIT(CDynamicDlgModbus)
//	// NOTE: the ClassWizard will add member initialization here
//	//}}AFX_DATA_INIT 
//	m_hgbl = NULL;
//	SetDynamicDlg(x, y, cx, cy, szTitle, pParent);
//	nID = 2000;
//}
//
//void CDynamicDlgSocket::SetDynamicDlg(int x, int y, int cx, int cy, const char *szTitle, CWnd* pParent /*=NULL*/)
//{
//	LPDLGTEMPLATE lpdt;
//	LPWORD lpw;
//
//	if (m_hgbl) GlobalFree(m_hgbl);
//	m_hDialogTemplate = NULL;
//	m_hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
//	lpdt = (LPDLGTEMPLATE)GlobalLock(m_hgbl);
//	if (lpdt == NULL)
//	{
//		return;
//	}
//	lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
//	lpdt->cdit = 0;
//	lpdt->x = x;
//	lpdt->y = y;
//	lpdt->cx = cx;
//	lpdt->cy = cy;
//	lpw = (LPWORD)(lpdt + 1);
//	*lpw++ = 0;
//	*lpw++ = 0;
//	while (*szTitle) *lpw++ = *szTitle++;
//	*lpw++ = 0;
//
//	GlobalUnlock(m_hgbl);
//	// InitModalIndirect(m_hgbl);
//	CreateIndirect(m_hgbl);
//
//}
//
//CDynamicDlgSocket::~CDynamicDlgSocket()
//{
//	if (m_hgbl) GlobalFree(m_hgbl);
//}
//
//BEGIN_MESSAGE_MAP(CDynamicDlgSocket, CDialog)
//	ON_MESSAGE(WM_MESSRECVSOCKETSOCKET, &CDynamicDlgSocket::OnReceive)
//	ON_WM_TIMER()
//	ON_WM_DESTROY()
//END_MESSAGE_MAP()
//
//BOOL CDynamicDlgSocket::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	//Initialisation des controles
//	CRect rect(10, 10, 280, 180);
//
//	this->GetWindowRect(&rect);
//	rect.NormalizeRect();
//	this->ScreenToClient(&rect);
//
//	rect.top += 50;
//	rect.bottom -= 10;
//	rect.left += 10;
//	rect.right -= 10;
//
//	EditBox.Create(ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | ES_AUTOHSCROLL, rect, this, nID);
//	EditBox.ShowScrollBar(SB_BOTH, TRUE);
//	EditBox.ShowWindow(SW_SHOW);
//
//	return TRUE;  // return TRUE unless you set the focus to a control
//				  // EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
//}
//
//LRESULT CDynamicDlgSocket::OnReceive(WPARAM Param1, LPARAM Param2)
//{
//	CString strEdit, strTemp;
//	int NbRecv = 0;
//	if (WSAGETSELECTEVENT(Param2) == FD_READ)
//	{
//		KillTimer(ID_TIMER_TIMEOUT_SOCKET);//Arret du timeout
//
//		ZeroMemory(((CServSocketDMX *)ModuleParent)->buffRecv, sizeof(((CServSocketDMX *)ModuleParent)->buffRecv));
//		NbRecv = recv(((CServSocketDMX *)ModuleParent)->sockAccept, ((CServSocketDMX *)ModuleParent)->buffRecv, 300, 0);
//
//		////Decodage reponse module impression
//		((CServSocketDMX *)ModuleParent)->LireReponseTrame(NbRecv);
//
//		//if(::IsWindow(this->GetSafeHwnd()))
//		if (this->IsWindowVisible())
//		{
//			CString strXX;
//			int i;
//			this->GetDlgItemText(nID, strEdit);
//
//			//Verification remplissage memoire
//			if (strEdit.GetLength() > 3000)
//			{
//				strEdit.Truncate(3000);
//			}
//			strTemp = "IN ";
//			for (i = 0;i<NbRecv;i++)
//			{
//				strXX.Format(_T("|%02x"), (((CServSocketDMX *)ModuleParent)->buffRecv[i] & 0x00FF));
//				strTemp += strXX;
//			}
//			strTemp += _T("\r\n");
//			strTemp += strEdit;
//			strEdit = strTemp;
//			this->SetDlgItemText(nID, strEdit);
//		}
//	}
//	else if (WSAGETSELECTEVENT(Param2) == FD_CONNECT)
//	{
//		this->GetDlgItemText(nID, strEdit);
//
//		strEdit += _T("CONNEXION OK");
//		this->SetDlgItemText(nID, strEdit);
//	}
//	else if (WSAGETSELECTEVENT(Param2) == FD_ACCEPT)
//	{
//		this->GetDlgItemText(nID, strEdit);
//
//		((CServSocketDMX *)ModuleParent)->sockAccept = accept(((CServSocketDMX *)ModuleParent)->sock, NULL, NULL);
//		
//		strEdit += _T("CONNEXION OK");
//		this->SetDlgItemText(nID, strEdit);
//	}
//	else if (WSAGETSELECTEVENT(Param2) == FD_CLOSE)
//	{
//
//		int iErreur = ((CServSocketDMX *)ModuleParent)->GetError();
//		if (iErreur != ERROR_SUCCESS)
//		{
//			this->GetDlgItemText(nID, strEdit);
//
//			strEdit += _T("CONNEXION TERMINEE avec ERREUR");
//			this->SetDlgItemText(nID, strEdit);
//		}
//		else
//		{
//			this->GetDlgItemText(nID, strEdit);
//
//			strEdit += _T("CONNEXION TERMINEE");
//			this->SetDlgItemText(nID, strEdit);
//		}
//
//
//		//Envoi message au parent
//		if (::IsWindow(((CServSocketDMX *)ModuleParent)->HwndParent))
//		{
//			::SendNotifyMessage(((CServSocketDMX *)ModuleParent)->HwndParent, WM_MESSDECONNEXIONSOCKET, (WPARAM)this, (LPARAM)0);
//		}
//	}
//	return 0;
//}
//void CDynamicDlgSocket::DoEvents(void)
//{
//	MSG msg;
//
//	// Process existing messages in the application's message queue.
//	// When the queue is empty, do clean up and return.
//	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
//	{
//		if (!AfxGetThread()->PumpMessage())
//		{
//			return;
//		}
//	}
//}
//
//void CDynamicDlgSocket::OnTimer(UINT_PTR nIDEvent)
//{
//	if (nIDEvent == ID_TIMER_TIMEOUT_SOCKET)
//	{
//		KillTimer(ID_TIMER_TIMEOUT_SOCKET);
//		if (ModuleParent == NULL)
//			return;
//
//		//Gestion Timeout Comm Modbus
//		ZeroMemory(((CServSocketDMX *)ModuleParent)->buffRecv, sizeof(((CServSocketDMX *)ModuleParent)->buffRecv));
//		((CServSocketDMX *)ModuleParent)->LireReponseTrame(0);
//	}
//
//	CDialog::OnTimer(nIDEvent);
//}
//
//void CDynamicDlgSocket::OnDestroy()
//{
//
//	KillTimer(ID_TIMER_TIMEOUT_SOCKET);
//
//	Sleep(MODULESOCKET_TIMEOUT_COM);
//
//	CDialog::OnDestroy();
//}
CServSocketDMX::CServSocketDMX()
{
	
	iPort = 1000;
	//LEUZE V 2.0 DialAffRecv = NULL;
	ZeroMemory(&clientService,sizeof(sockaddr_in));
	sock = 0;

	iErreur = 0;//0 pas d'erreur
	str_Erreur = _T("");
	//V2 HwndParent = NULL;

	 bIsConnected = false;
}


CServSocketDMX::~CServSocketDMX()
{
}


bool CServSocketDMX::StartServeur(int Port)
{
	//LEUZE V 2.0 
	//if (DialAffRecv != NULL)
	//{
	//	delete(DialAffRecv);
	//	DialAffRecv = NULL;
	//}

	////Création de la fenetre d'affichage des trames
	//DialAffRecv = new CDynamicDlgSocket();
	//DialAffRecv->SetDynamicDlg(0, 0, 300, 200, "Affichage Socket");
	//DialAffRecv->ModuleParent = this;

	//CString sTitre = _T("");
	//sTitre.Format(_T("SOCKET %s %d"), strIP, iPort);
	//DialAffRecv->SetWindowText(sTitre);


	/*//V2 for (int i = 0; i < NB_MAX_CLIENTS; i++)
	{
		Clients[i].sockAccept = 0;

		ZeroMemory(Clients[i].buffRecv, sizeof(char) * 300);
		Clients[i].BufflenRecv = 0;

		Clients[i].bIsConnected = false;
		Clients[i].strIP = _T("");
	}*/

	//Initialisation de WINSOCK	
	iPort = Port;

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		iErreur = GetError();
		WSACleanup();
		return false;
	}

	//Creation du socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		iErreur = GetError();
		WSACleanup();
		return false;
	}


	//Binding du socket
	clientService.sin_family = AF_INET;
	clientService.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr(IP);
	clientService.sin_port = htons(iPort);

	iResult = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, "TRUE", 4);
	if (iResult != 0)
	{
		iResult = WSAGetLastError();
		str_Erreur.Format(_T("Erreur setsockopt : %i"), iResult);
		iErreur = iResult;
	}

	/*ESSAI pour verifier la deconnexion du client
	long opt = 1;
	iResult = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt));
	if (iResult != 0)
	{
		iResult = WSAGetLastError();
		str_Erreur.Format(_T("Erreur setsockopt : %i"), iResult);
		iErreur = iResult;
	}*/

	iResult = bind(sock, (SOCKADDR*)&clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR)
	{
		iResult = WSAGetLastError();
		str_Erreur.Format(_T("Erreur bind : %i"), iResult);
		closesocket(sock);
		iErreur = iResult;
		return false;
	}

	//Ecoute sur le socket
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
	{
		iErreur = GetError();
		WSACleanup();
		return false;
	}

	//LEUZE V 2.0 
	//-------------------------
	// Create new event
	NewEvent = WSACreateEvent();

	if (WSAEventSelect(sock,NewEvent, FD_ACCEPT | FD_CLOSE| FD_ROUTING_INTERFACE_CHANGE | FD_ADDRESS_LIST_CHANGE) == SOCKET_ERROR)
	{
		iErreur = GetError();
		WSACleanup();
		return false;
	}
	else
	{
		//Demarrage thread gestion evenements
		bStopThread = false;
		H_ThreadServeur = ::CreateThread(NULL, 0, ThreadServeur, (LPVOID)this, NULL, &ThreadID);

		bIsConnected = true;
		return true;
	}
	/*if (WSAAsyncSelect(sock, DialAffRecv->GetSafeHwnd(), WM_MESSRECVSOCKETSOCKET, FD_READ | FD_WRITE | FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		iErreur = GetError();
		WSACleanup();
		return false;
	}
	else
	{
		return true;
	}*/

	return true;
}
int CServSocketDMX::Deconnecter(void)
{
	//LEUZE V 2.0 
	if (!bIsConnected)
	{
		return TRUE;
	}

	//Deblocage/arret thread events
	WSASetEvent(NewEvent);
	bStopThread = true;
	WSACloseEvent(NewEvent);

	CloseHandle(H_ThreadServeur);

	//Deconnexion sockets
	closesocket(sock);
	//V2 
	/*for (int i = 0; i < NB_MAX_CLIENTS; i++)
	{
		if (Clients[i].bIsConnected)
		{
			

			Clients[i].Deconnecter();
		}
		
	}*/
	

	WSACleanup();
	
	iErreur = 0;
	str_Erreur = _T("Deconnexion OK");
	sock = NULL;
	
	bIsConnected = false;
	return TRUE;
	
}
int CServSocketDMX::GetError(void)
{
	int err = -1;
	static char Message[1024];
	LPVOID lpMsgBuf;
	lpMsgBuf = (LPVOID)"Unknown error";

	err = WSAGetLastError();
	//affiche le message d'erreur en fonction du code d'erreur reçu par WSAGetLastError()
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);


	str_Erreur.Format(_T("%s"), (LPTSTR)lpMsgBuf);

	return(err);
}
int CClientSocket::LireReponseTrame(int NbRecv)
{

	//CString strLue = CString(buffRecv,NbRecv);


	//V2 if (::IsWindow(HwndParent))
	//{
	//	//::SendNotifyMessage(HwndParent, WM_MESSRECVVALUESOCKET, (WPARAM)this, (LPARAM)0);
	//	//Si la reception est trop rapide, on loupe des caractéres à cause du notify...
	//	::SendMessage(HwndParent, WM_MESSRECVVALUESOCKET, (WPARAM)this, (LPARAM)NbRecv);
	//}
	
	if (pExecuteOnValueReceive != NULL)
	{
		pExecuteOnValueReceive(CString(buffRecv,BufflenRecv),strIP);
	}


	return TRUE;
}
bool CClientSocket::IsConnected()
{
	//TEST TO REMOVEreturn false;

	// --- Quit if incorrect website to ping
	if (strIP == _T("")) return false; // no string


												// --- Initialise the use of "Ws2_32.dll"
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) return false;

	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	DWORD dwRetval = 0;
	struct addrinfo *result = NULL;

	CString strService = _T("");
	unsigned long ipaddr = INADDR_NONE;

	if (strIP.Find(_T("www.")) >= 0)
	{
		strService = _T("http");

		char chWebsiteOrIP[100] = { 0 };
		char chService[50] = { 0 };

#ifdef _UNICODE
		size_t size;
		wcstombs_s(&size, chWebsiteOrIP, strIP, wcslen(strIP));
		strIP.ReleaseBuffer();
		wcstombs_s(&size, chService, strService, wcslen(strService));
		strService.ReleaseBuffer();
#else
		strcpy_s(chWebsiteOrIP, strIP);

		strcpy_s(chService, strService);
#endif
		dwRetval = getaddrinfo(chWebsiteOrIP, chService, &hints, &result);
		if (dwRetval != 0)
		{
			CString strMessage = _T("");
			strMessage.Format(_T("getaddrinfo failed with error: %d\n"), dwRetval);
			OutputDebugString(strMessage);
			WSACleanup();
			return false;
		}

		struct sockaddr_in  *sockaddr_ipv4;
		sockaddr_ipv4 = (struct sockaddr_in *) result->ai_addr;

		ipaddr = (unsigned long)(sockaddr_ipv4->sin_addr.S_un.S_addr);
		freeaddrinfo(result);
	}
	else
	{

		InetPton(AF_INET, strIP, &ipaddr);
	}


	if (ipaddr == INADDR_NONE)
	{
		return false;
	}

	IP_OPTION_INFORMATION ioi;
	HANDLE hFile = IcmpCreateFile();

	ioi.Ttl = 255;
	ioi.Tos = 0;
	ioi.Flags = 0;
	ioi.OptionsSize = 0;
	ioi.OptionsData = NULL;

	bool     bRet = false;
	char SendData[32] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;
	DWORD dwRetVal = 0;

	ReplySize = (DWORD)(sizeof(ICMP_ECHO_REPLY) + sizeof(SendData));
	ReplyBuffer = (VOID*)malloc(ReplySize);

	if (ReplyBuffer == NULL)
	{
		return false;
	}

	dwRetVal = IcmpSendEcho(hFile, ipaddr, SendData, sizeof(SendData), &ioi, ReplyBuffer, ReplySize, 1000);

	if (dwRetVal != 0)
	{
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
		struct in_addr ReplyAddr;
		if (pEchoReply == NULL)
		{
			return false;
		}

		ReplyAddr.S_un.S_addr = pEchoReply->Address;
		CString strReponse = _T("");

		strReponse.Format(_T("Received from %s\nStatus = %ld\nRoundtrip time = %ld milliseconds\n"), strIP.GetBuffer(), pEchoReply->Status, pEchoReply->RoundTripTime);
		strIP.ReleaseBuffer();
		OutputDebugString(strReponse);

		bRet = true;
	}
	else
	{
		//Erreur
		bRet = false;
	}

	IcmpCloseHandle(hFile);
	free(ReplyBuffer);


	// --- Release the resources
	WSACleanup();

	return bRet;
}
void CClientSocket::Deconnecter()
{
	if (!bIsConnected)
	{
		return;
	}

	WSASetEvent(NewEvent);
	bStopThread = true;

	CloseHandle(H_ThreadEvent);
	
	if (H_ThreadPing != 0)
	{
		CloseHandle(H_ThreadPing);
	}

	bIsConnected = false;
	WSACloseEvent(NewEvent);

	closesocket(sockAccept);
	sockAccept = NULL;
}
void CClientSocket::Connecter(CString strIpConnexion,SOCKET SockAccept)
{
	strIP = strIpConnexion;

	sockAccept = SockAccept;

	NewEvent = WSACreateEvent();
	bIsConnected = true;

	if (WSAEventSelect(sockAccept, NewEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
	{
		//iErreur = GetError();
		WSACleanup();
		//return 1;
	}
	else
	{
		//Demarrage thread gestion evenements pour le nouveau client connecté
		bStopThread = false;
		H_ThreadEvent = ::CreateThread(NULL, 0, ThreadEventsSocket, (LPVOID)this, NULL, &ThreadID);

		if (bDoThreadPing)
		{
			H_ThreadPing = ::CreateThread(NULL, 0, ThreadPingAlive, (LPVOID)this, NULL, &ThreadIDPing);
		}
		else
		{
			H_ThreadPing = 0;
		}
	}
}
//LEUZE V 2.0 

DWORD  WINAPI ThreadServeur(LPVOID pParam)
{
	CServSocketDMX * pThis = (CServSocketDMX*)(pParam);

	DWORD Index = 0;
	BOOL bResult = FALSE;
	WSAOVERLAPPED AcceptOverlapped;
	DWORD Flags = 0;
	DWORD BytesTransferred = 0;
	DWORD RecvBytes = 0;
	WSABUF DataBuf;
	char buffer[300];
	ZeroMemory(buffer, sizeof(char) * 300);

	DWORD iResult = 0;

	DataBuf.len = 300;
	DataBuf.buf = buffer;

	ZeroMemory(&AcceptOverlapped, sizeof(WSAOVERLAPPED));
	AcceptOverlapped.hEvent = pThis->NewEvent;


	while (pThis->bStopThread == false)
	{
		//-----------------------------------------         
		// Reset the changed flags and overlapped structure
		Flags = 0;
		ZeroMemory(&AcceptOverlapped, sizeof(WSAOVERLAPPED));

		AcceptOverlapped.hEvent = pThis->NewEvent;

		//-----------------------------------------
		// Reset the data buffer
		DataBuf.len = 300;
		DataBuf.buf = buffer;
		ZeroMemory(buffer, sizeof(char) * 300);

		//-----------------------------------------
		// Wait for the overlapped I/O call to complete
		Index = WSAWaitForMultipleEvents(1, &pThis->NewEvent, FALSE, WSA_INFINITE, FALSE);
		
		if (pThis->bStopThread)
		{
			return 0;
		}
		WSANETWORKEVENTS NetworkEvents;
		WSAEnumNetworkEvents(pThis->sock, pThis->NewEvent, &NetworkEvents);

		//-----------------------------------------
		// Reset the signaled event
		bResult = WSAResetEvent(pThis->NewEvent);
		if (bResult == FALSE)
		{
			pThis->str_Erreur.Format(_T("WSAResetEvent failed with error = %d\n"), WSAGetLastError());
		}
		//-----------------------------------------
		// Determine the status of the overlapped event
		bResult = WSAGetOverlappedResult(pThis->sock, &AcceptOverlapped, &BytesTransferred, FALSE, &Flags);
		if (bResult == FALSE)
		{
			pThis->str_Erreur.Format(_T("WSAGetOverlappedResult failed with error = %d\n"), WSAGetLastError());
		}

		//Attente de client connecté
		if (WSAGETSELECTEVENT(NetworkEvents.lNetworkEvents) == FD_ACCEPT)
		{
			//Recupérration adresse Ip client
			struct sockaddr_in saClient;
			int iClientSize = sizeof(saClient);
			SOCKET SockTemp = WSAAccept(pThis->sock, (SOCKADDR*)&saClient, &iClientSize, NULL, NULL);

			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(saClient.sin_addr), str, INET_ADDRSTRLEN);

			CString strIpTemp = CString(str);

			//V2 
			CClientSocket * ValReturn = NULL;

			if (pThis->pExecuteOnConnect != NULL)
			{
				ValReturn = pThis->pExecuteOnConnect(strIpTemp);
			}

			if (ValReturn == NULL)
			{
				//refus de la connexion
				pThis->str_Erreur = _T("CONNEXION REFUSEE");
				closesocket(SockTemp);

			}
			else
			{
				//on accepte la connexion sur le client retourné
				ValReturn->Connecter(strIpTemp, SockTemp);

				pThis->iNbClientsConnected++;
				pThis->str_Erreur = _T("CONNEXION ACCEPTEE");
			}

			//Gestion reconnexion client si l'adresse ip a déjà été connectée
			//int iIndexClient = pThis->iNbClientsConnected;

			//for (int i = 0; i < NB_MAX_CLIENTS; i++)
			//{
			//	/*if ((pThis->Clients[i].bIsConnected)&&
			//		(pThis->Clients[i].strIP == strIpTemp))
			//	{
			//		bReuseClient = true;

			//		iIndexClient = i;
			//		break;
			//	}*/

			//	if (!pThis->Clients[i].bIsConnected)
			//	{
			//		iIndexClient = i;
			//		break;
			//	}
			//}

			//pThis->Clients[iIndexClient].strIP = strIpTemp;

			//pThis->Clients[iIndexClient].sockAccept = SockTemp;

			//pThis->Clients[iIndexClient].HwndParent = pThis->HwndParent;
			//pThis->Clients[iIndexClient].NewEvent = WSACreateEvent();
			//pThis->Clients[iIndexClient].bIsConnected = true;
			//	
			//if (WSAEventSelect(pThis->Clients[iIndexClient].sockAccept, pThis->Clients[iIndexClient].NewEvent, FD_READ | FD_WRITE | FD_CLOSE ) == SOCKET_ERROR)
			//{
			//	pThis->iErreur = pThis->GetError();
			//	WSACleanup();
			//	return 1;
			//}
			//else
			//{
			//	//Demarrage thread gestion evenements pour le nouveau client connecté
			//	pThis->Clients[iIndexClient].bStopThread = false;
			//	pThis->Clients[iIndexClient].H_ThreadEvent = ::CreateThread(NULL, 0, ThreadEventsSocket, (LPVOID)&pThis->Clients[iIndexClient], NULL, &pThis->Clients[iIndexClient].ThreadID);

			//	pThis->Clients[iIndexClient].H_ThreadPing = ::CreateThread(NULL, 0, ThreadPingAlive, (LPVOID)&pThis->Clients[iIndexClient], NULL, &pThis->Clients[iIndexClient].ThreadIDPing);
			//}

			//pThis->iNbClientsConnected++;

			/*if (pThis->iNbClientsConnected > NB_MAX_CLIENTS)
			{
				return 0;
			}
*/
			//pThis->str_Erreur = _T("CONNEXION ACCEPTEE");
				
			////Envoi message au parent
			//if (::IsWindow(pThis->HwndParent))
			//{
			//	::SendMessage(pThis->HwndParent, WM_MESSCONNEXIONSOCKET, (WPARAM)&pThis->Clients[iIndexClient], (LPARAM)0);
			//}

		}
		//V2 Unused
		//else if (WSAGETSELECTEVENT(NetworkEvents.lNetworkEvents) == FD_CLOSE)
		//{
		//		
		//		
		//		
		//		
		//	//Envoi message au parent
		//	if (::IsWindow(pThis->HwndParent))
		//	{
		//		::SendMessage(pThis->HwndParent, WM_MESSDECONNEXIONSOCKET, (WPARAM)pThis, (LPARAM)0);
		//	}
		//}
		//else if (WSAGETSELECTEVENT(NetworkEvents.lNetworkEvents) == FD_ROUTING_INTERFACE_CHANGE)
		//{

		//	pThis->str_Erreur = _T("ROUTING INTERFACE CHANGE");

		//	//Envoi message au parent
		//	if (::IsWindow(pThis->HwndParent))
		//	{
		//		::SendMessage(pThis->HwndParent, WM_MESSDECONNEXIONSOCKET, (WPARAM)pThis, (LPARAM)0);
		//	}
		//}
		//else if (WSAGETSELECTEVENT(NetworkEvents.lNetworkEvents) == FD_ADDRESS_LIST_CHANGE)
		//{

		//	pThis->str_Erreur = _T("ADDRESS LIST CHANGE");


		//	//Envoi message au parent
		//	if (::IsWindow(pThis->HwndParent))
		//	{
		//		::SendMessage(pThis->HwndParent, WM_MESSDECONNEXIONSOCKET, (WPARAM)pThis, (LPARAM)0);
		//	}
		//}
		
	}

	return 0;
}

//Thread gestion evenements par client

DWORD WINAPI ThreadEventsSocket(LPVOID pParam)
{
	CClientSocket * pThis = (CClientSocket*)(pParam);

	DWORD Index = 0;
	BOOL bResult = FALSE;
	WSAOVERLAPPED AcceptOverlapped;
	DWORD Flags = 0;
	DWORD BytesTransferred = 0;
	DWORD RecvBytes = 0;
	WSABUF DataBuf;
	char buffer[300];
	ZeroMemory(buffer, sizeof(char) * 300);

	DWORD iResult = 0;

	DataBuf.len = 300;
	DataBuf.buf = buffer;

	ZeroMemory(&AcceptOverlapped, sizeof(WSAOVERLAPPED));
	AcceptOverlapped.hEvent = pThis->NewEvent;


	while (pThis->bStopThread == false)
	{
		//-----------------------------------------         
		// Reset the changed flags and overlapped structure
		Flags = 0;
		ZeroMemory(&AcceptOverlapped, sizeof(WSAOVERLAPPED));

		AcceptOverlapped.hEvent = pThis->NewEvent;

		//-----------------------------------------
		// Reset the data buffer
		DataBuf.len = 300;
		DataBuf.buf = buffer;
		ZeroMemory(buffer, sizeof(char) * 300);

		//-----------------------------------------
		// Wait for the overlapped I/O call to complete
		Index = WSAWaitForMultipleEvents(1, &pThis->NewEvent, FALSE, WSA_INFINITE, FALSE);

		//Client connecté
		WSANETWORKEVENTS NetworkEvents;
		WSAEnumNetworkEvents(pThis->sockAccept, pThis->NewEvent, &NetworkEvents);

		//-----------------------------------------
		// Reset the signaled event
		bResult = WSAResetEvent(pThis->NewEvent);
		if (bResult == FALSE)
		{
			//pThis->str_Erreur.Format(_T("WSAResetEvent failed with error = %d\n"), WSAGetLastError());
		}
		//-----------------------------------------
		// Determine the status of the overlapped event
		bResult = WSAGetOverlappedResult(pThis->sockAccept, &AcceptOverlapped, &BytesTransferred, FALSE, &Flags);
		if (bResult == FALSE)
		{
			//pThis->str_Erreur.Format(_T("WSAGetOverlappedResult failed with error = %d\n"), WSAGetLastError());
		}

		if (WSAGETSELECTEVENT(NetworkEvents.lNetworkEvents) == FD_READ)
		{
			if (WSARecv(pThis->sockAccept, &DataBuf, 1, &RecvBytes, &Flags, &AcceptOverlapped, NULL) == SOCKET_ERROR)
			{
				iResult = WSAGetLastError();
			/*	if (iResult != WSA_IO_PENDING)
					pThis->str_Erreur.Format(_T("WSARecv failed with error = %d\n"), iResult);*/
			}

			if (RecvBytes > 0)
			{
				//Decodage reponse module impression
				pThis->BufflenRecv = RecvBytes;
				//Terminaison par \0 pour strcpy
				if (DataBuf.buf[pThis->BufflenRecv - 1] != _T('\0'))
				{
					pThis->BufflenRecv++;
					DataBuf.buf[pThis->BufflenRecv - 1] = _T('\0');
				}


				strcpy_s(pThis->buffRecv, 300, DataBuf.buf);
				pThis->LireReponseTrame(pThis->BufflenRecv);
			}

		}
		else if (WSAGETSELECTEVENT(NetworkEvents.lNetworkEvents) == FD_CLOSE)
		{
			pThis->Deconnecter();

			//Envoi message au parent
			/*//V2 if (::IsWindow(pThis->HwndParent))
			{
				::SendMessage(pThis->HwndParent, WM_MESSDECONNEXIONSOCKET, (WPARAM)pThis, (LPARAM)0);
			}*/
			if (pThis->pExecuteOnDeconnect != NULL)
			{
				pThis->pExecuteOnDeconnect();
			}
		
			return 0;
		}
	}

	//SockV3
	if (pThis->pExecuteOnArretThread != NULL)
	{
		pThis->pExecuteOnArretThread();
	}
	

	return 0;
}

DWORD WINAPI ThreadPingAlive(LPVOID pParam)
{
	CClientSocket * pThis = (CClientSocket*)(pParam);

	while (pThis->bStopThread == false)
	{

		if (pThis->IsConnected())
		{
			Sleep(ATTENTE_THREAD_PING);
		}
		else 
		{
			pThis->Deconnecter();

			//Envoi message au parent
			/*//V2 if (::IsWindow(pThis->HwndParent))
			{
			::SendMessage(pThis->HwndParent, WM_MESSDECONNEXIONSOCKET, (WPARAM)pThis, (LPARAM)0);
			}*/
			if (pThis->pExecuteOnDeconnect != NULL)
			{
				pThis->pExecuteOnDeconnect();
			}

			return 0;
		}
	}


	return 0;
}

CClientSocket::CClientSocket()
{
	sockAccept = 0;

	ZeroMemory(buffRecv, sizeof(char) * 300);
	BufflenRecv =0;

	strIP = _T("");
	bIsConnected = false;
	
	pExecuteOnDeconnect = NULL;
	pExecuteOnValueReceive = NULL;
	bDoThreadPing = false;
}

CClientSocket::~CClientSocket()
{
}
