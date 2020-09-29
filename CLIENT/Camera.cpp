#include "StdAfx.h"
#include "Camera.h"
#include "Form_camsim.h"
 

CCamera::CCamera(bool b_att)
{
	this->b_att_rep=b_att;
	m_comerr=0;
	m_nInputBufferSize=0;
	m_nOutputBufferSize=0;
	m_count=0;

	b_XonXoff=false;
	m_baudrate=0;
	m_parity=0;
	m_wordlength=0;
	m_stopbits=0;
	m_numport=0;

	str_charfin=_T("");

	//variables pour controles camera
	Gain_min=0;
	Gain_max=0;
	Expo_min=0;
	Expo_max=0;
	Mode_min=0;
	Mode_max=0;
	Format_min=0;
	Format_max=0;
	val_gainP=0;

	//variables par défaut pour gestion d'erreur 
	b_aff_err=false;
	b_att_rep=true;
	frame=NULL;
	winMessCom = NULL;
	ThreadLecture = NULL;
	ZeroMemory(BuffReception,512);
	iCurrentBufReceptLenght = 0;
	DiagCam = NULL;
	scruter = FALSE;
	ThreadEnded = FALSE;

	p_Alarme[0] = NULL;
	p_Alarme[1] = NULL;
	p_Alarme[2] = NULL;
	p_Alarme[3] = NULL;

};
CCamera::~CCamera(void)
{
	//Arret du thread
	if (scruter==TRUE)
	{
		scruter = FALSE;
		do
		{
			SetEvent(osLect.hEvent);//débloque l'opération en attente
			Sleep(10);			// Attente fin communication avec Flex
		}
		while(ThreadEnded == FALSE);
	}
	
	if(ThreadLecture!=NULL)
	{
		ThreadLecture = NULL;
	}
/* frame = theApp.CamView deja deleted ici
	if(frame >0)
	{
		delete(frame);
		frame  =NULL;
	}*/
	if(winMessCom >0)
	{
		delete(winMessCom);
		winMessCom = NULL;
	}

	CloseHandle(m_hCom);
	m_hCom = NULL;

}

bool CCamera::Ecrire_commande(CString commande)
{

	bool result;
	CString trame;
//	wchar_t* buf;
	char buff[300];
	size_t size;
	
	str_received .Empty();

	//sauvegarde de la derniére commande envoyée
	last_command=commande;
	//ajout des caractéres de fin pour protocole communication camera
	commande+=str_charfin;
	// puis on l'envoit sur le port série
	/*buf=commande.GetBuffer();
	commande.ReleaseBuffer();
	wcstombs_s(&size, buff, buf, wcslen(buf) );*/
	strcpy_s(buff,commande.GetBuffer());
	commande.ReleaseBuffer();
	size = commande.GetLength();
	//size-1 parce qu'on envoit pas le 0 de fin de chaine
	result=this->Ecrire_buff(buff,(unsigned int)(size));

	return result;
}

bool CCamera::Ecrire_buff(char* buff, unsigned int nbre)
{	
	BOOL        bResult ;	// valeur retournée par writeFile
	DWORD       dwBytesWritten ;
	DWORD       dwEvtMask = 0;
	COMSTAT    ComStat ;
	DWORD      dwErrorFlags;
		
	ClearCommError( m_hCom, &dwErrorFlags, &ComStat ) ;
	PurgeComm( m_hCom, PURGE_TXABORT | PURGE_TXCLEAR ) ;  	

	/*m_CommDev valeur retournée par le CreateFile
	lpByte pointeur sur la zone dans laquelle se trouvent les données à écrire
	dwBytesToWrite	Nombre de caractères à écrire
	dwBytesWritten	Adresse d'une variable dans laquelle windows copie le nombre de caractères écrits
	osWrite pointeur sur une structure OVERLAPPED (concerne les opérations asynchrones)*/
	
	ZeroMemory(&m_Ov, sizeof(m_Ov));
	m_Ov.Internal	= 0;
	m_Ov.InternalHigh= 0;
	m_Ov.Offset		= 0;
	m_Ov.OffsetHigh	= 0;

	bResult = WriteFile( m_hCom, buff, nbre,
	                     &dwBytesWritten, &m_Ov ) ;
	// Traitement des cas d'erreur d'ecriture
	CString sMessage;
	if (!bResult)
	{	DWORD dwError = GetLastError();
		CString message;
		switch (dwError)
		{	case 0:	// Tout s'est bien deroule: On continue normalement
				break;
			case ERROR_INVALID_HANDLE:	// Erreur bloquante: On sort
				message.Format("*** CRS232::Ecriture impossible sur la liaison série: Le handle est invalide **\n");
				return FALSE;
			case ERROR_IO_PENDING:
				// Operation en cours: Verifie si l'ecriture est bien terminee mais
				// la fonction n'est pas bloquante car lors des tests,
				// elle stoppait l'appli durant 1 bonne minute (impossible de faire autre chose)
				bResult = GetOverlappedResult(m_hCom, &m_Ov, &dwBytesWritten, FALSE);
				if (!bResult)
				{	// Message seulement en mode Debug
					message.Format("* CRS232::Ecriture: Overlap...\n");
				}
				break;
			default:	// Erreur non identifiee : On sort
				message.Format("CRS232::Ecriture: Error during writing on serial port with dwError=%d\n",dwError);
				::OutputDebugString(message);
				return FALSE;
		}
	}

	bool bReturn = false;
	bResult == TRUE ? bReturn = true : bReturn = false;
	return bReturn;
}

UINT CCamera::ReadThread(LPVOID pParam)
{
	CCamera* This = (CCamera*) pParam;
	
	BOOL	   bResult = FALSE; // valeur retournée par ReadFile
	COMSTAT    EtatPort;		// ComStat
	ZeroMemory(&EtatPort, sizeof(EtatPort));
	DWORD      dwErrorFlags	= 0;
	DWORD      dwEvtMask	= 0;
	DWORD	   dwError		= 0;
	
	int iNbLus = 0;
	CString messLecture;
	BYTE Octet_lu		= 0;
	DWORD dwBytesRead	= 0;
	DWORD dwOverBytes = 0;
	HANDLE Port_Serie	= This->m_hCom;


	// initialisation des champs de la structure overlapped
	
	ZeroMemory(&This->osLect, sizeof(This->osLect));
	
	// création d'un événement pour les opérations de lecture asynchrones 
	This->osLect.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (This->osLect.hEvent==NULL)
	{	dwError = GetLastError();
//		AfxMessageBox("ERR1\n");

		messLecture.Format("*** CProtoCom::LectureCOM: Erreur lors de l'appel a 'CreateEvent' with dwError= %d ***\n",dwError);
	}

	// demande un signalement sur reception de caractères
	bResult = SetCommMask(Port_Serie, EV_RXCHAR);
	if (!bResult)
	{	dwError = GetLastError();
//		AfxMessageBox("ERR2\n");
		messLecture.Format("*** CProtoCom::LectureCOM: Erreur lors de l'appel a 'SetCommMask' with dwError= %d ***\n",dwError);
	}

	BOOL bNouvelOctet;

	while (This->scruter)	// booléen externe pour arrêter la Sous Tâche
	{
		bNouvelOctet = FALSE;

		// ----------------------------------
		if(!ClearCommError(Port_Serie, &dwErrorFlags, &EtatPort ))	// permet de lire le nombre d'octet en queue
		{	dwError = GetLastError();
			messLecture.Format("*** CRS232::LectureCOM: Erreur lors de l'appel a 'ClearCommError' with dwError= %d (dwErrorFlag = %08X) ***\n",dwError, dwErrorFlags);
		}
		else
		{

			dwBytesRead=0;
			 // Get error flags.
			BOOL    fOOP, fOVERRUN, fPTO, fRXOVER, fRXPARITY, fTXFULL;
		    BOOL    fBREAK, fDNS, fFRAME, fIOE, fMODE;

			fDNS = dwErrorFlags & CE_DNS;
			fIOE = dwErrorFlags & CE_IOE;
			fOOP = dwErrorFlags & CE_OOP;
			fPTO = dwErrorFlags & CE_PTO;
			fMODE = dwErrorFlags & CE_MODE;
			fBREAK = dwErrorFlags & CE_BREAK;//signal rate mismatch on line (probablement le cable est debranché)
			fFRAME = dwErrorFlags & CE_FRAME;//Start ET/OU stop bits invalides(format transmission incorrect)
			fRXOVER = dwErrorFlags & CE_RXOVER;
			fTXFULL = dwErrorFlags & CE_TXFULL;
			fOVERRUN = dwErrorFlags & CE_OVERRUN;
			fRXPARITY = dwErrorFlags & CE_RXPARITY;

			if(fRXOVER)
			{
				if(( fBREAK)||( fFRAME))
					EtatPort.cbInQue = 0;
				else
				{
					//RAZ buffer RX
					// Purge des buffers d'entrée et de sortie de la RS232
					 PurgeComm(This->m_hCom, /*PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR |*/ PURGE_RXCLEAR);
					 EtatPort.cbInQue = 0;
				}
			}
			if(/*( fDNS)||( fIOE)||( fOOP)||( fPTO)||( fMODE)||*/( fBREAK)||( fFRAME)/*||( fRXOVER)||( fTXFULL)||( fOVERRUN)||( fRXPARITY)*/)
			{
				EtatPort.cbInQue = 0;
			}

			if (EtatPort.cbInQue >0) 
			{

//							AfxMessageBox("CAR RS232\n");


				// Non: on lit l'octet suivant
				// ---------------------------
				if(!ReadFile(Port_Serie, // Handle de la RS232
									 &Octet_lu,	// &dwBytesToRead // buffer de reception de l'octet
									 1UL,			// nombre d'octets à lire
									 &dwBytesRead,	//  nombre d'octets lus
									 &This->osLect ))		//	pointeur sur une structure OVERLAPPED (concerne les operations asynchrones)
				{
					dwError = GetLastError();
					if(dwError==ERROR_IO_PENDING)	//l'operation de lecture n'est pas termine: on la termine
					{
						messLecture.Format("* LectureCOM: Continu\n");
						if(!GetOverlappedResult(Port_Serie, &This->osLect, &dwOverBytes, TRUE))	//la lecture est-elle termine (fonction bloquante)
						{
							if(dwError = GetLastError() == ERROR_HANDLE_EOF)
							{
								messLecture.Format("* Ben ca marche pas non plus...\n");
							}
						}
						else
						{
							dwBytesRead = dwOverBytes;
							bNouvelOctet = TRUE;
						}
					}// fin du if(dwError==ERROR_IO_PENDING)
					else
					{	messLecture.Format("*** CRS232::LectureCOM: Erreur lors de l'appel a 'ReadFile' with dwError= %d ***\n",dwError);
					}
				}
				else
					bNouvelOctet = TRUE;
			}
			else	
			{
				// Oui: Attend que le prochain evenement survienne sur le PortSerie
				// ----
				if(!WaitCommEvent(Port_Serie, &dwEvtMask, &This->osLect))
				{
					dwError=GetLastError();
					// Si l'operation de lecture n'est pas terminee: On cherche les Bytes manquants
					if(dwError==ERROR_IO_PENDING)
					{
						if(!GetOverlappedResult(Port_Serie, &This->osLect, &dwOverBytes, TRUE))
							messLecture.Format("* Oups\n");
						else
						{
							dwBytesRead = dwOverBytes;
							bNouvelOctet = TRUE;
						}
					}
					else
						messLecture.Format("* Oups2\n");
				}
				else
				{
					// si aucun evenement thread termine, il faut finir
					if(dwEvtMask==0)
						messLecture.Format("* Oups3\n");

					// s'il s'agit d'une reception de caracteres
					if((dwEvtMask & EV_RXCHAR))
					{
						if(!ReadFile(Port_Serie, // Handle de la RS232
											 &Octet_lu,	// &dwBytesToRead // buffer de reception de l'octet
											 1UL,			// nombre d'octet à lire
											 &dwBytesRead,	// nombre d'octets lus
											 &This->osLect ))		// &osRead
						{
							dwError = GetLastError();
							if(dwError==ERROR_IO_PENDING)	//l'operation de lecture n'est pas termine
							{
								if(!GetOverlappedResult(Port_Serie, &This->osLect, &dwOverBytes, TRUE))	//la lecture est-elle termine (fonction bloquante)
								// Attend de facon bloquante la fin de la lecture puis sort de la boucle
									messLecture.Format("* Over3\n");
								else
								{
									dwBytesRead = dwOverBytes;
									bNouvelOctet = TRUE;
								}
							}// fin du if(dwError==ERROR_IO_PENDING)
							else
								messLecture.Format("* Oups5\n");
						}
						else
							bNouvelOctet = TRUE;
					}
				}
			}
		}


		if(bNouvelOctet)	// si resultat de lecture est vrai
		{
			if (dwBytesRead ==1)	// si le nombre d'octet lu est de 1
			{
				This->BuffReception[iNbLus] = (char)Octet_lu;
				This->iCurrentBufReceptLenght = iNbLus++;
				if(iNbLus > 512) iNbLus = 0;
				

				if(This->frame!= NULL)
				{
					if(IsWindow(This->frame->GetSafeHwnd()))
						::SendNotifyMessage(This->frame->GetSafeHwnd(),WM_MESSFORMCAM,(WPARAM)Octet_lu,(LPARAM)dwBytesRead);
					else
					{
						This->frame = NULL;
						CString StrTemp;
						int temp =(int) Octet_lu;
						char chTemp = (char) temp;
						temp  = temp & (0x000000FF);
						StrTemp.Format(_T("%c"),temp);
						This->str_received +=  StrTemp;
					}
				}
				else
				{
					CString StrTemp;
					bool bComplete = false;
					//int indice =(int) strlen(buffer);
					int temp =(int) Octet_lu;
					char chTemp = (char) temp;
					temp  = temp & (0x000000FF);
					StrTemp.Format(_T("%c"),temp);

					//verification fin de trame

					if((chTemp == This->str_charfin.GetAt(1)) && (This->PrevCarac == This->str_charfin.GetAt(0)))
					{
						bComplete = true;
					}
					else
					{
						bComplete = false;
					}

					This->PrevCarac = chTemp;

					if (bComplete)
					{
						//tous les caractéres sont bons => fin de trame
						This->str_received +=  StrTemp;
						//SendMessage
						if(This->DiagCam!= NULL)
						{
							if(IsWindow(This->DiagCam->GetSafeHwnd()))
							{
								::SendNotifyMessage(This->DiagCam->GetSafeHwnd(),WM_AFFCHAINE,(WPARAM)This->str_received.GetBuffer(),(LPARAM) This->str_received.GetLength());
								This->str_received.ReleaseBuffer();
							}
						}
					}
					else
					{
						This->str_received +=  StrTemp;
					}
				}
			}
		}
	}


	This->ThreadEnded = TRUE;
	AfxEndThread(100,TRUE);
	return 1;
}
bool CCamera::Lire_buff(char* buff,size_t size_buff)
{
	if((iCurrentBufReceptLenght!=0)&&(size_buff >= iCurrentBufReceptLenght))
	{
	//	DWORD nLength =0;
	//	::ReadFile(SerialPort.m_hComm, buff,(DWORD) size_buff, &nLength, NULL);

	//	if(nLength !=0)
	//	return true; // succes
	//else
	//	return false;

		strcpy_s(buff,iCurrentBufReceptLenght,BuffReception);
		m_count = iCurrentBufReceptLenght;
		return true;
	}
	else
	{
		this->err=_T("Pas de données");
	    return false;
	}
}


bool CCamera::OpenPort(int num_port,long vitesse, char parite, int longueur,int bit_stop)
{
// Fonction d'ouverture de la liaison RS232

	bool bResult = false;
	char *szPort = new char[50];
	char *szBaud = new char[50];

	// if the port is already opened: close it
	if (m_hCom != NULL)
	{
		CloseHandle(m_hCom);
		m_hCom = NULL;
	}

	// prepare port strings
	sprintf_s(szPort,50, _T("\\\\.\\COM%d"), num_port);
	

	m_hCom = CreateFile(szPort, // Nom du "fichier"
							GENERIC_READ | GENERIC_WRITE, // Ouvert pour E/S (lecture, ecriture)
							0,	// Mode partage. Comme une ligne série ne peut être utilisée par plusieurs programmes en même temps, alors 0
							NULL, // Pas d'attributs de protection
							OPEN_EXISTING, // ouverture d'un "fichier" existant
							FILE_FLAG_OVERLAPPED, // Operations Asynchrones
							NULL ); // Pas de copie d'attributs d'un autre fichier

    // Validation de la commande d'ouverture
	CString message;
	if(INVALID_HANDLE_VALUE == m_hCom)	// Probleme lors de l'ouverture du port
	// Ce probleme n'est pas reference par GetLastError
	// et se traite donc en dehors du switch precedent
    {
		message = "Erreur d'ouverture liaison serie\n";
		//AfxMessageBox(message,MB_ICONERROR);
		TRACE(message);
		if(p_Alarme[CAMALM_SERIAL] != NULL) p_Alarme[CAMALM_SERIAL]->NewValue = TRUE;
        return false;
    }
	else
	{
		m_baudrate = vitesse;
		m_parity = parite;
		m_wordlength = longueur;
		m_stopbits = bit_stop;
		m_numport = num_port;

		message = _T("liaison serie OK \n");
		TRACE(message);
		if(p_Alarme[CAMALM_SERIAL] != NULL) p_Alarme[CAMALM_SERIAL]->NewValue = FALSE;
	}

	// Appel eventuel a GetLastError
	DWORD dwError;
	if (!m_hCom)
	{	dwError = GetLastError();
		switch (dwError) {
			case 0:	// Tout s'est bien deroule: On continue normalement
				break;
			case ERROR_FILE_NOT_FOUND:
				message.Format("Port serie non trouvé. dwError=%d\n",dwError);
				return false;
			default:
				message.Format("Error during writing on serial port. dwError=%d",dwError);
				return false;
				break;
		}
	}

	// Initialisation de la taille du buffer d'entrée et de sortie
	// -----------------------------------------------------------
    BOOL bRes =  SetupComm(m_hCom, 4096, 4096) ;
	if (!bRes)
	{	dwError = GetLastError();
	}

    // Purge des buffers d'entrée et de sortie de la RS232
    PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    // Définition des Timeouts E/S sur la RS232
    m_ComTimeouts.ReadIntervalTimeout		= 0; // Intervalle d'attente du ReadFile entre deux caractères non utilisé
    m_ComTimeouts.ReadTotalTimeoutMultiplier = 100; //1 
    m_ComTimeouts.ReadTotalTimeoutConstant	= 1000; //10// Read Timeout = 100*(nombre d'octets à lire) + 1000
    m_ComTimeouts.WriteTotalTimeoutMultiplier = 0;
    m_ComTimeouts.WriteTotalTimeoutConstant	= 0;// Write Timeout = 0
	SetCommTimeouts(m_hCom, &m_ComTimeouts) ;	// envoi

	// Appel à la fonction de configuration du port dans "liaison" (DCB)
	m_dcb.BaudRate = vitesse;
	m_dcb.ByteSize = longueur;	

	switch(bit_stop)
	{
		case 1:
			m_dcb.StopBits = 0;
			break;
		//case 1.5
		//	m_dcb.StopBits = 1;
		//	break;
		case 2:
			m_dcb.StopBits = 2;
			break;
		default:
			m_dcb.StopBits = 0;
			break;
	}

	switch(parite)
	{
		case 'N':
			m_dcb.Parity = NOPARITY;
			m_dcb.fParity = 0;
			break;
		case 'O':
			m_dcb.Parity = ODDPARITY;
			m_dcb.fParity = 1;
			break;
		case 'E':
			m_dcb.Parity = EVENPARITY;
			m_dcb.fParity = 1;
			break;
		default:
			m_dcb.Parity = NOPARITY;
			m_dcb.fParity = 0;	
			break;
	}

	if (!Initialise_DCB()) // Si la fonction échoue
		 return false;

	LanceLecture_RS232();//LAncement thread reception
	
	delete [] szPort;
	delete [] szBaud;

	return true;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CCamera::Initialise_DCB()
{
// Fonction de configuration de la RS232	

	m_dcb.DCBlength = sizeof(DCB);			// Renseigne la longueur de la structure
    
	// Configuration correspondant à la communication avec la balance 3S/Mettler
	//dcb.BaudRate= CBR_9600;				// 9600 bauds
 //   dcb.ByteSize= 8;					// 8 bits 
 //   dcb.Parity	= 0;					// Pas de parité
 //   dcb.StopBits= 0;					// 1 bit d'arrêt

	// Autre paramètres de configuration
    m_dcb.fBinary = 1;						// binary mode, no EOF check , should be 1 for WIN32 API
    m_dcb.fOutxCtsFlow = 0;					// no CTS output flow control 
    m_dcb.fOutxDsrFlow = 0;					// no DSR output flow control 
    m_dcb.fDtrControl = DTR_CONTROL_DISABLE;  // no DTR flow control type 
    m_dcb.fDsrSensitivity = 0;				// no DSR sensitivity 
    m_dcb.fTXContinueOnXoff = 0;				// no XOFF continues Tx 
    m_dcb.fOutX = 0;							// no XON/XOFF out flow control 
    m_dcb.fInX = 0;							// no XON/XOFF in flow control 
    m_dcb.fErrorChar = 0;						// disable error replacement 
    m_dcb.fNull = 0;							// enable null stripping ? 
    m_dcb.fRtsControl = RTS_CONTROL_DISABLE;	// no RTS flow control 
    m_dcb.fAbortOnError = 0;					// no abort reads/writes on error 
	m_dcb.wReserved1 = 0;						// not currently used 
	m_dcb.XonLim = 0;							// transmit XON threshold 
    m_dcb.XoffLim = 0;						// transmit XOFF threshold	
    m_dcb.XonChar = ASCII_XON;				// Tx and Rx XON character 
    m_dcb.XoffChar = ASCII_XOFF;				// Tx and Rx XOFF character

	return (SetCommState(m_hCom,&m_dcb)); // initialisation du port
}

BOOL CCamera::LanceLecture_RS232()
{
// Fonction de lancement de la sous tâche de lecture

	scruter	= TRUE;

	ThreadLecture = AfxBeginThread( ReadThread,			// pointeur sur la fonction de soustâche
					(LPVOID) this,			// LPVOID pParam // paramètre passé à la fonction
					THREAD_PRIORITY_NORMAL,	// int nPriority // priorité de la tache = priorité process 
					0,						// UINT nStackSize // taille de la pile = taille pile process
					0,						// DWORD dwCreateFlags // drapreaux de création ici départ immédiat
					NULL );					// LPSECURITY_ATTRIBUTES lpSecurityAttrs // protections	
	::OutputDebugString("-Lancement thread Com OK\n");
	if (ThreadLecture!=NULL)
		return TRUE;

	OutputDebugString("*** Thread de lecture nulle ***\n");
	return FALSE;
}


bool CCamera::SetExpoMicroSec(int expoms)
{
	//vérification des valeurs
	if((expoms<this->Expo_min)||(expoms>this->Expo_max))
	{
		this->err=_T("Valeurs hors tolérance");
		return false;
	}
	else
	{
		return true;
	}

}
bool CCamera::SetGainPourcent(int gainP)
{
	//calcul du gain en fonction des valeurs
	int gain=(gainP*(Gain_max-Gain_min))/100;
	//vérification des valeurs
	if((gain<this->Gain_min)||(gain>this->Gain_max))
	{
		this->err=_T("Valeurs hors tolérance");
		return false;
	}
	else
	{	
		this->val_gainP=gain;
		return true;
	}

}
void CCamera::Open_testFrame()
{	
	//si la fenêtre a déjà étée ouverte 
	//on libére la mémoire avant d'en reprendre
	if(frame->GetSafeHwnd()>0)
	{
		delete(frame);
		frame = NULL;
	}
	else frame = NULL;

	frame= new CForm_camsim();
	((CForm_camsim*)frame)->iIdCam = this->iIdCam;
	frame->Create(CForm_camsim::IDD);

	//affichage de la fenêtre
	frame->UpdateData(FALSE);
	frame->SetForegroundWindow();
	frame->ShowWindow(SW_SHOW);

}
void CCamera::AfficheRep(char CharRecv)
{	
	CharRecv;

	frame->DestroyWindow();
	delete(frame);
	frame  =NULL;

}





