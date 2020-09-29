#include "StdAfx.h"
#include "ProcessDMX.h"

#include "BaseFenetre.h"//pour paramètres et pointeurs fonctions "generales"


CProcessDMX::CProcessDMX(void)
{

	IndexDemandeLecture = -1;

	ListeAffichage = NULL;
	iNbCodesRecus = 0;
	strCodesBrutes.RemoveAll();
	strCodes.RemoveAll();

}

CProcessDMX::~CProcessDMX(void)
{
}

//SockV4
bool CProcessDMX::Inhiber()
{
	*pBitInhibition = TRUE;

	return true;
}

//SockV4
bool CProcessDMX::Activer()
{
	*pBitInhibition = FALSE;

	return true;
}

int CProcessDMX::OpenWebConfig(void)
{
	if (ClientSocket.bIsConnected == false)
	{
		return 0;
	}

	//Ouverture de la page Web de configuration du lecteur
	CString strFile = _T("");
	strFile.Format(_T("http://%s"), ClientSocket.strIP);
	::ShellExecute(NULL,_T("open"),strFile,NULL,NULL,SW_SHOW);
	return 0;
}

void CProcessDMX::Process_ReceptionCode(CString strResultsLeuze)
{

	CString strComplete;
	strEnCours += strResultsLeuze;

	if (strEnCours.Find(FIN_TRAME_LEUZE, 0) > 0)
	{
		//Fin de trame recue
		strComplete = strEnCours;
		strEnCours = _T("");
		
		if (ListeAffichage != NULL)
		{
			CString strAffichage = _T("[") + ClientSocket.strIP + _T("]:") + strComplete;
			ListeAffichage->SetCustomColor(COLOR_NOIR);
			ListeAffichage->InsertString(0, strAffichage);
		}
	}
	else
	{
		//on fait que ajouter en attendant la fin de trame
		return;
	}

	//Cas trame non-complète le coup précédent
	int indexSearch = 0;
	int iNbResult = 0;
	int iTemp = 0;
	do
	{
		iTemp = strComplete.Find(_T("Result:"), indexSearch);
		if (iTemp >= 0)
		{
			iNbResult++;
		}
		indexSearch = iTemp + 1;

	} while (iTemp >= 0);

	if (iNbResult != 1)
	{
		if (ListeAffichage != NULL)
		{
			CString strAffichage = _T("");

			strAffichage.Format(_T("Erreur Reception Code multiple (len %d) : [%s]"), iNbResult, strComplete);
			ListeAffichage->SetCustomColor(COLOR_ROUGE);
			ListeAffichage->InsertString(0, strAffichage);
		}

		if (iValResult != NULL)	*iValResult = 3;
		Alarme[DMXALM_COMM]->NewValue = 1;//V44.1
		return;
	}

	//Format a voir:
	//[7]		Result:
	//[3 + SP]	XXX
	//[10 + SP]	CCCCCCCCCC
	//[30 + |]	YYYYYYYYYYYYYYYYYYYYYYYYYYYYYY
	//[3]		END

	//Result: : Flag de debut trame
	//XXX : Nombre de codes lus
	//CCCCCCCCCC : Nb Caracteres Lus (CodeLenght)
	//YYYYYYYYYY : Resultat de lecture du code sur 30 caracteres
	//END : Flag de Fin de trame

	CString strFlagDebut = strComplete.Left(7);
	CString striNbCodes = strComplete.Mid(7,3);
	int iNbCodes = 0;
	strCodesBrutes.RemoveAll();
	strCodes.RemoveAll();

#ifdef _UNICODE
	iNbCodes = _wtoi(striNbCodes);
#else
	iNbCodes = atoi(striNbCodes);
#endif

	CString strTemp;
	for (int i = 0; i < iNbCodes; i++)
	{
		strTemp = strComplete.Mid(10 + (i * 40), 40);
		strCodesBrutes.SetAtGrow(strCodesBrutes.GetCount(), strTemp);
		CString strCodeLenght;
		strCodeLenght = strComplete.Mid(10 + (i * 40), 10);
		strCodeLenght.Remove(_T(' '));

		int iCodeLenght = 0;
#ifdef _UNICODE
		iCodeLenght = _wtoi(strCodeLenght);
#else
		iCodeLenght = atoi(strCodeLenght);
#endif

		strCodes.SetAtGrow(strCodes.GetCount(), strTemp.Right(iCodeLenght));

		if (ListeAffichage != NULL)
		{
			CString strAffichage = _T("");

			strAffichage.Format(_T("Code (len %d) : [%s]"), iCodeLenght, strTemp.Right(iCodeLenght));//SockV3 correction Format
			ListeAffichage->SetCustomColor(COLOR_NOIR);
			ListeAffichage->InsertString(0, strAffichage);
		}
	}

	iNbCodesRecus = iNbCodes;

	if (iNbCodesRecus == 0)
	{
		//pas de lecture/ code illisible
		if (iValResult != NULL)	*iValResult = 3;

		if (ListeAffichage != NULL)
		{
			CString strAffichage = _T("");
			strAffichage.Format(_T("Result = %d Pas de Code Lus (Nb = 0)"), (int)*iValResult);
			ListeAffichage->SetCustomColor(COLOR_ROUGE);
			ListeAffichage->InsertString(0, strAffichage);
		}
		return;
	}

	if (iNbCodesRecus == 1)
	{
		if (strCodes.GetAt(0) == IdDMX)
		{
			//lecture OK
			if (iValResult != NULL)	*iValResult = 1;
		}
		else
		{
			//MIXUP
			if (iValResult != NULL)	*iValResult = 2;
		}
		
		if (ListeAffichage != NULL)
		{
			CString strAffichage = _T("");
			strAffichage.Format(_T("Result = %d Code recu (%s)"), (int)*iValResult, strCodes.GetAt(0));
			if (*iValResult == 2)//MIXUP
			{
				ListeAffichage->SetCustomColor(COLOR_ROUGE);
			}
			else
			{
				ListeAffichage->SetCustomColor(COLOR_VERT);
			}

			ListeAffichage->InsertString(0, strAffichage);
		}
		return;
	}

	//comparaison codes recus/ code à lire
	for (int i = 0; i < iNbCodesRecus; i++)
	{
		if (strCodes.GetAt(i) == IdDMX)
		{
			//lecture OK sur un des codes lus
			if (iValResult != NULL)	*iValResult = 1;
			break;
		}

		//Aucun codes lus n'est confirmé
		if (iValResult != NULL)	*iValResult = 3;
	}

	if (ListeAffichage != NULL)
	{
		CString strTemp2 = _T("");
		strTemp = _T("");

		for (int i = 0; i < iNbCodesRecus; i++)
		{
			strTemp2.Format("%s", strCodes.GetAt(i));
			strTemp = strTemp + _T("|") + strTemp2;
		}

		CString strAffichage = _T("");
		strAffichage.Format(_T("Result = %d %d Codes recus (%s)"), (int)*iValResult, iNbCodesRecus, strTemp);
		if (*iValResult == 3)//Erreur de lecture (aucun code OK)
		{
			ListeAffichage->SetCustomColor(COLOR_ROUGE);
		}
		else
		{
			ListeAffichage->SetCustomColor(COLOR_VERT);
		}
		ListeAffichage->InsertString(0, strAffichage);
	}

}
void CProcessDMX::Process_Connexion(CString strIpConnexion)
{
	ClientSocket.bDoThreadPing = true;

	//OK
	*pBitEtatConnexion = DMX_CONNECTED;

	strEnCours = _T("");

	//Affichage
	if (ListeAffichage != NULL)
	{
		CString strAffichage = _T("");
		strAffichage.Format(_T("[") + strIpConnexion + _T("]: CONNEXION Lecteur %d"),iNumDMX);
		ListeAffichage->SetCustomColor(COLOR_BLEU);
		ListeAffichage->InsertString(0, strAffichage);
	}
}
void CProcessDMX::Process_Deconnexion()
{
	//NOK
	*pBitEtatConnexion = DMX_DISCONNECTED;

	//Affichage
	if (ListeAffichage != NULL)
	{
		CString strAffichage = _T("[") + ClientSocket.strIP + _T("]: DECONNEXION");
		ListeAffichage->SetCustomColor(COLOR_ROUGE);
		ListeAffichage->InsertString(0, strAffichage);
	}
}
void CProcessDMX::Process_BitLecture()
{
	//Inhibition gérée coté automate
	bLectureAttente = true;

	//RAZ des resultats
	if (iValResult != NULL)	*iValResult = 0;

	strCodesBrutes.RemoveAll();
	strCodes.RemoveAll();
	iNbCodesRecus = 0;

	//Affichage
	if (ListeAffichage != NULL)
	{
		CString strAffichage = _T("");
		strAffichage.Format(_T("[%d] ATTENTE Lecture ....(Bit Lecture)"),iNumDMX);
		ListeAffichage->SetCustomColor(COLOR_BLEU);
		ListeAffichage->InsertString(0, strAffichage);
	}
}

//44.1
void CProcessDMX::GererAlarme()
{
	if (pBitInhibition == NULL)
	{
		return;
	}

	if (*pBitInhibition == TRUE)
	{
		Alarme[DMXALM_PING]->NewValue = 0;
		Alarme[DMXALM_COMM]->NewValue = 0;
		Alarme[DMXALM_TIMEOUT]->NewValue = 0;
		return;
	}
	
	if (*pBitEtatConnexion == DMX_DISCONNECTED)
	{
		Alarme[DMXALM_PING]->NewValue = 1;
	}
	else
	{
		Alarme[DMXALM_PING]->NewValue = 0;
	}
}

/*
CClientSocket * (*pExecuteOnConnect)(CString strIpConnexion):
executée lors de la demande de connexion sur le serveur
la fonction doit retourner NULL si la connexion est refusée
et un CClientSocket alloué si la connexion est acceptée

remplace WM_MESSCONNEXIONSOCKET
*/
CClientSocket * ExecuteOnConnect(CString strIpConnexion)
{
		
	if (strIpConnexion == ParamStrIPDMX1->value)
	{
		theApp.m_DMXEtui.Process_Connexion(strIpConnexion);

		return &theApp.m_DMXEtui.ClientSocket;
	}
	else if (strIpConnexion == ParamStrIPDMX2->value)
	{
		theApp.m_DMXNotice1.Process_Connexion(strIpConnexion);

		return &theApp.m_DMXNotice1.ClientSocket;
	}
	else if (strIpConnexion == ParamStrIPDMX3->value)
	{
		theApp.m_DMXNotice2.Process_Connexion(strIpConnexion);

		return &theApp.m_DMXNotice2.ClientSocket;
	}
	else
	{
		return NULL;
	}
}

void ExecuteOnreceive_DMXEtui(CString strReceive, CString strIPReceive)
{

	//SockV3
	if (strIPReceive != theApp.m_DMXEtui.ClientSocket.strIP)
	{
		//Affichage
		if (theApp.m_DMXEtui.ListeAffichage != NULL)
		{
			CString strMessage = _T("");
			strMessage.Format(_T("IP Recv [%s]"), strIPReceive);
			CString strAffichage = _T("[") + theApp.m_DMXEtui.ClientSocket.strIP + _T("] Melange Reception!!! : ") + strMessage;
			theApp.m_DMXEtui.ListeAffichage->SetCustomColor(COLOR_ROUGE);
			theApp.m_DMXEtui.ListeAffichage->InsertString(0, strAffichage);
		}
	}

	theApp.m_DMXEtui.Process_ReceptionCode(strReceive);
}

void ExecuteOnreceive_DMXNotice1(CString strReceive, CString strIPReceive)
{
	//SockV3
	if (strIPReceive != theApp.m_DMXNotice1.ClientSocket.strIP)
	{
		//Affichage
		if (theApp.m_DMXNotice1.ListeAffichage != NULL)
		{
			CString strMessage = _T("");
			strMessage.Format(_T("IP Recv [%s]"), strIPReceive);
			CString strAffichage = _T("[") + theApp.m_DMXNotice1.ClientSocket.strIP + _T("] Melange Reception!!! : ") + strMessage;
			theApp.m_DMXNotice1.ListeAffichage->SetCustomColor(COLOR_ROUGE);
			theApp.m_DMXNotice1.ListeAffichage->InsertString(0, strAffichage);
		}
	}

	theApp.m_DMXNotice1.Process_ReceptionCode(strReceive);
}

void ExecuteOnreceive_DMXNotice2(CString strReceive, CString strIPReceive)
{
	//SockV3
	if (strIPReceive != theApp.m_DMXNotice2.ClientSocket.strIP)
	{
		//Affichage
		if (theApp.m_DMXNotice2.ListeAffichage != NULL)
		{
			CString strMessage = _T("");
			strMessage.Format(_T("IP Recv [%s]"), strIPReceive);
			CString strAffichage = _T("[") + theApp.m_DMXNotice2.ClientSocket.strIP + _T("] Melange Reception!!! : ") + strMessage;
			theApp.m_DMXNotice2.ListeAffichage->SetCustomColor(COLOR_ROUGE);
			theApp.m_DMXNotice2.ListeAffichage->InsertString(0, strAffichage);
		}
	}

	theApp.m_DMXNotice2.Process_ReceptionCode(strReceive);
}

void ExecuteOnDeconnexion_DMXEtui()
{
	theApp.m_DMXEtui.Process_Deconnexion();
}

void ExecuteOnDeconnexion_DMXNotice1()
{
	theApp.m_DMXNotice1.Process_Deconnexion();
}

void ExecuteOnDeconnexion_DMXNotice2()
{
	theApp.m_DMXNotice2.Process_Deconnexion();
}

void ExecuteOnArret_DMXEtui()
{
	CString strMessage = _T("");

	if (theApp.m_DMXEtui.ClientSocket.bIsConnected)
	{
		theApp.m_DMXEtui.ClientSocket.Deconnecter();

		if (theApp.m_DMXEtui.ClientSocket.BufflenRecv > 0)
		{
			strMessage.Format(_T("ARRET THREAD Client encore Connecté, Data En Cours[%s]"), CString(theApp.m_DMXEtui.ClientSocket.buffRecv, theApp.m_DMXEtui.ClientSocket.BufflenRecv));
		}
		else
		{
			strMessage.Format(_T("ARRET THREAD Client encore Connecté, Data En Cours[NULL]"));
		}
	}
	else
	{
		if (theApp.m_DMXEtui.ClientSocket.BufflenRecv > 0)
		{
			strMessage.Format(_T("ARRET THREAD Client Deconnecté, Data En Cours[%s]"), CString(theApp.m_DMXEtui.ClientSocket.buffRecv, theApp.m_DMXEtui.ClientSocket.BufflenRecv));
		}
		else
		{
			strMessage.Format(_T("ARRET THREAD Client Deconnecté, Data En Cours[NULL]"));
		}
	}

	//Affichage
	if (theApp.m_DMXEtui.ListeAffichage != NULL)
	{
		CString strAffichage = _T("[") + theApp.m_DMXEtui.ClientSocket.strIP + _T("]: ") + strMessage;
		theApp.m_DMXEtui.ListeAffichage->SetCustomColor(COLOR_ROUGE);
		theApp.m_DMXEtui.ListeAffichage->InsertString(0, strAffichage);
	}
}
void ExecuteOnArret_DMXNotice1()
{
	CString strMessage = _T("");

	if (theApp.m_DMXNotice1.ClientSocket.bIsConnected)
	{
		theApp.m_DMXNotice1.ClientSocket.Deconnecter();

		if (theApp.m_DMXNotice1.ClientSocket.BufflenRecv > 0)
		{
			strMessage.Format(_T("ARRET THREAD Client encore Connecté, Data En Cours[%s]"), CString(theApp.m_DMXNotice1.ClientSocket.buffRecv, theApp.m_DMXNotice1.ClientSocket.BufflenRecv));
		}
		else
		{
			strMessage.Format(_T("ARRET THREAD Client encore Connecté, Data En Cours[NULL]"));
		}
	}
	else
	{
		if (theApp.m_DMXNotice1.ClientSocket.BufflenRecv > 0)
		{
			strMessage.Format(_T("ARRET THREAD Client Deconnecté, Data En Cours[%s]"), CString(theApp.m_DMXNotice1.ClientSocket.buffRecv, theApp.m_DMXNotice1.ClientSocket.BufflenRecv));
		}
		else
		{
			strMessage.Format(_T("ARRET THREAD Client Deconnecté, Data En Cours[NULL]"));
		}
	}


	//Affichage
	if (theApp.m_DMXNotice1.ListeAffichage != NULL)
	{
		CString strAffichage = _T("[") + theApp.m_DMXNotice1.ClientSocket.strIP + _T("]: ") + strMessage;
		theApp.m_DMXNotice1.ListeAffichage->SetCustomColor(COLOR_ROUGE);
		theApp.m_DMXNotice1.ListeAffichage->InsertString(0, strAffichage);
	}
}
void ExecuteOnArret_DMXNotice2()
{
	CString strMessage = _T("");

	if (theApp.m_DMXNotice2.ClientSocket.bIsConnected)
	{
		theApp.m_DMXNotice2.ClientSocket.Deconnecter();
		
		if (theApp.m_DMXNotice2.ClientSocket.BufflenRecv > 0)
		{
			strMessage.Format( _T("ARRET THREAD Client encore Connecté, Data En Cours[%s]"),CString(theApp.m_DMXNotice2.ClientSocket.buffRecv, theApp.m_DMXNotice2.ClientSocket.BufflenRecv));
		}
		else
		{
			strMessage.Format(_T("ARRET THREAD Client encore Connecté, Data En Cours[NULL]"));
		}
	}
	else
	{
		if (theApp.m_DMXNotice2.ClientSocket.BufflenRecv > 0)
		{
			strMessage.Format(_T("ARRET THREAD Client Deconnecté, Data En Cours[%s]"), CString(theApp.m_DMXNotice2.ClientSocket.buffRecv, theApp.m_DMXNotice2.ClientSocket.BufflenRecv));
		}
		else
		{
			strMessage.Format(_T("ARRET THREAD Client Deconnecté, Data En Cours[NULL]"));
		}
	}

	//Affichage
	if (theApp.m_DMXNotice2.ListeAffichage != NULL)
	{
		CString strAffichage = _T("[") + theApp.m_DMXNotice2.ClientSocket.strIP + _T("]: ") + strMessage;
		theApp.m_DMXNotice2.ListeAffichage->SetCustomColor(COLOR_ROUGE);
		theApp.m_DMXNotice2.ListeAffichage->InsertString(0, strAffichage);
	}
}
