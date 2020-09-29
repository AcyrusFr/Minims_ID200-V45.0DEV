#include "StdAfx.h"
#include "GestAlarmes.h"

#include "BaseFenetre.h"//Application
#include "DialAlarmes.h"

#include "AC_Base.h"


//V 44.0
int iIndex_ALARME_OPC;
int iIndex_ALARME_DMXETUI;
int iIndex_ALARME_DMXNOTICE1;
int iIndex_ALARME_DMXNOTICE2;
//V44.1
int iIndex_ALARME_COM_DMXETUI;
int iIndex_ALARME_COM_DMXNOTICE1;
int iIndex_ALARME_COM_DMXNOTICE2;
int iIndex_ALARME_TIMEOUT_DMXETUI;
int iIndex_ALARME_TIMEOUT_DMXNOTICE1;
int iIndex_ALARME_TIMEOUT_DMXNOTICE2;

int iIndex_ALARME_CAM1;
int iIndex_ALARME_CAM2;
int iIndex_ALARME_CAM3;
int iIndex_ALARME_CAM4;
int iIndex_ALARME_CAM1_SERIAL;
int iIndex_ALARME_CAM1_LINK;
int iIndex_ALARME_CAM1_SYNCHRO;
int iIndex_ALARME_CAM2_SERIAL;
int iIndex_ALARME_CAM2_LINK;
int iIndex_ALARME_CAM2_SYNCHRO;

int iIndex_ALARME_PESAGE;//V44.1

//V 44.9 
int iIndex_ALARME_Cadence_ROBOT1;
int iIndex_ALARME_Cadence_ROBOT2;
int iIndex_ALARME_Cadence_ROBOT3;
int iIndex_ALARME_Cadence_Machine;

CGestAlarmes::CGestAlarmes(void)
{
	NbTotal = 0;

	//Initialisation des alarmes
	for(int i = 0; i < NB_ALARMES; i++)
	{
		Alarme[i].Traca = false;
		Alarme[i].strLibelle = _T("");
		Alarme[i].strOrigine = _T("");
		Alarme[i].Type = -1;
		Alarme[i].idGroupe = -1;
		Alarme[i].Numero = -1;
		
		Alarme[i].DB_Val = NULL;
		Alarme[i].DB_Num = -1;
		Alarme[i].DB_Adresse = -1;
		Alarme[i].DB_Bit = -1;
		Alarme[i].Value = ALARME_OFF;
		Alarme[i].NewValue = ALARME_OFF;
		Alarme[i].IdBase = 0;
	}

	for(int i = 0; i < NBGROUPES; i++)
	{
		GrpAlarmes[i].NbAlarmesLiees = -1;
		GrpAlarmes[i].NbDefauts = -1;
	}

	//V 44.0
	iIndex_ALARME_OPC = -1;
	iIndex_ALARME_DMXETUI = -1;
	iIndex_ALARME_DMXNOTICE1 = -1;
	iIndex_ALARME_DMXNOTICE2 = -1;
	iIndex_ALARME_CAM1 = -1;
	iIndex_ALARME_CAM2 = -1;
	iIndex_ALARME_CAM3 = -1;
	iIndex_ALARME_CAM4 = -1;
	iIndex_ALARME_CAM1_SERIAL = -1;
	iIndex_ALARME_CAM1_LINK = -1;
	iIndex_ALARME_CAM1_SYNCHRO = -1;
	iIndex_ALARME_CAM2_SERIAL = -1;
	iIndex_ALARME_CAM2_LINK = -1;
	iIndex_ALARME_CAM2_SYNCHRO = -1;

	//V 44.9 
	iIndex_ALARME_Cadence_ROBOT1 = -1;
	iIndex_ALARME_Cadence_ROBOT2 = -1;
	iIndex_ALARME_Cadence_ROBOT3 = -1;
	iIndex_ALARME_Cadence_Machine = -1;
}

CGestAlarmes::~CGestAlarmes(void)
{
}

int CGestAlarmes::ChargerFromBase(void)
{
	//Chargement des alarmes depuis la BDD
	#pragma warning (disable: 4995)
	CADORecordset rsAlarme (&theBase.dao_Base);

	int NbAlarmesLoaded = 0;
	//int indexAlarme = -1;
	int iTemp;

	CString strvalue = "";

	if (ACBaseStrRsOpen("","","Alarmes",&rsAlarme))	
	{
		rsAlarme.MoveFirst();

		do
		{
			/*indexAlarme = ACBaseGetInt("Code",&rsAlarme);

			if(indexAlarme < 0) break;*/

			//V 43.3
			if (NbAlarmesLoaded >= NB_ALARMES)
			{
				CString strMessage = _T("");
				strMessage.Format(_T("Alarme %d en dehors du tableau NB_ALARMES"), NbAlarmesLoaded);
				AfxMessageBox(strMessage, MB_OK|MB_ICONWARNING);
				continue;
			}
			
			////V 43.3
			//if (Alarme[NbAlarmesLoaded].Numero >= 0)
			//{
			//	CString strMessage = _T("");
			//	strMessage.Format(_T("Alarme %d deja définie (doublon Alarmes.Code)"), NbAlarmesLoaded);
			//	AfxMessageBox(strMessage, MB_OK, MB_ICONWARNING);
			//	continue;
			//}

			Alarme[NbAlarmesLoaded].Numero = NbAlarmesLoaded;
			Alarme[NbAlarmesLoaded].Type = ACBaseGetInt("Tp",&rsAlarme);
			Alarme[NbAlarmesLoaded].strLibelle = ACBaseGetStr("Libelle",&rsAlarme);

			//gestion caractére ' dans les messages d'erreurs => provoque une Erreur SQL
			Alarme[NbAlarmesLoaded].strLibelle.Replace(_T("'"),_T(" "));

			Alarme[NbAlarmesLoaded].idGroupe = ACBaseGetInt("Groupe",&rsAlarme);
			GrpAlarmes[Alarme[NbAlarmesLoaded].idGroupe].NbAlarmesLiees++;
			Alarme[NbAlarmesLoaded].strOrigine = ACBaseGetStr("Origine",&rsAlarme);
			Alarme[NbAlarmesLoaded].DB_Num = ACBaseGetInt("DB",&rsAlarme);

			//V 44.0 
			if (Alarme[NbAlarmesLoaded].DB_Num < 0)
			{
				//Alarme Gérée par l'application
				//il faut conservé l'index correspondant à ce code, cet index est celui utilisé pour activer/reseter l'alarme
				int iCodeAlarme = ACBaseGetInt("Code", &rsAlarme);

				switch (iCodeAlarme)
				{
				case CODE_ALARME_OPC			:
					iIndex_ALARME_OPC = NbAlarmesLoaded;
					break;//V44.1 correction
				case CODE_ALARME_DMXETUI		:
					iIndex_ALARME_DMXETUI = NbAlarmesLoaded;
					break;
				case CODE_ALARME_DMXNOTICE1		:
					iIndex_ALARME_DMXNOTICE1 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_DMXNOTICE2		:
					iIndex_ALARME_DMXNOTICE2 = NbAlarmesLoaded;
					break;
				//V 44.1 
				case CODE_ALARME_COM_DMXETUI:
					iIndex_ALARME_COM_DMXETUI = NbAlarmesLoaded;
					break;
				case CODE_ALARME_COM_DMXNOTICE1:
					iIndex_ALARME_COM_DMXNOTICE1 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_COM_DMXNOTICE2:
					iIndex_ALARME_COM_DMXNOTICE2 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_TIMEOUT_DMXETUI:
					iIndex_ALARME_TIMEOUT_DMXETUI = NbAlarmesLoaded;
					break;
				case CODE_ALARME_TIMEOUT_DMXNOTICE1:
					iIndex_ALARME_TIMEOUT_DMXNOTICE1 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_TIMEOUT_DMXNOTICE2:
					iIndex_ALARME_TIMEOUT_DMXNOTICE2 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_PESAGE:
					iIndex_ALARME_PESAGE = NbAlarmesLoaded;
					break;

					//V 44.1 
				case CODE_ALARME_CAM1			:
					iIndex_ALARME_CAM1 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM2			:
					iIndex_ALARME_CAM2 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM3			:
					iIndex_ALARME_CAM3 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM4			:
					iIndex_ALARME_CAM4 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM1_SERIAL	:
					iIndex_ALARME_CAM1_SERIAL = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM1_LINK		:
					iIndex_ALARME_CAM1_LINK = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM1_SYNCHRO	:
					iIndex_ALARME_CAM1_SYNCHRO = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM2_SERIAL	:
					iIndex_ALARME_CAM2_SERIAL = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM2_LINK		:
					iIndex_ALARME_CAM2_LINK = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CAM2_SYNCHRO	:
					iIndex_ALARME_CAM2_SYNCHRO = NbAlarmesLoaded;
					break;
					//V 44.9 
				case CODE_ALARME_CADENCE_ROBOT1		:
					iIndex_ALARME_Cadence_ROBOT1 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CADENCE_ROBOT2		:
					iIndex_ALARME_Cadence_ROBOT2 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CADENCE_ROBOT3		:
					iIndex_ALARME_Cadence_ROBOT3 = NbAlarmesLoaded;
					break;
				case CODE_ALARME_CADENCE_MACHINE	:
					iIndex_ALARME_Cadence_Machine = NbAlarmesLoaded;
					break;


				default:
					break;
				}
				

			}


			Alarme[NbAlarmesLoaded].DB_Adresse = ACBaseGetInt("Adresse",&rsAlarme);
			Alarme[NbAlarmesLoaded].DB_Bit = ACBaseGetInt("Bit",&rsAlarme);
			iTemp = ACBaseGetInt("Traca",&rsAlarme);
			if(iTemp == 1)
				Alarme[NbAlarmesLoaded].Traca = true;
			else
				Alarme[NbAlarmesLoaded].Traca = false;
			
			rsAlarme.MoveNext();
			NbAlarmesLoaded++;
		}
		while(!rsAlarme.IsEOF());


		rsAlarme.Close();
	}

#pragma warning (default: 4995)
	NbTotal = NbAlarmesLoaded ;

	return NbAlarmesLoaded;
}

void CGestAlarmes::ResetAlarmes(void)
{
	//RESET des alarmes
	for (int i = 0; i < NbTotal;i++)
	{
		Alarme[i].NewValue = ALARME_OFF;
	}
}

int CGestAlarmes::GererAlarmes(void)
{
	//GESTION DES ALARMES EXECUTER SUR TIMER 500MS
	//Alarme = 0 -> pas d'erreur

	//BM V1.16,	Boolean de test de présence d'une alarme
	bool AlarmeEnCours = false;

	//Gestion des alarmes ROBOTS
	theApp.m_Robot1.GererAlarme();
	theApp.m_Robot2.GererAlarme();
	theApp.m_Robot3.GererAlarme();

	//V 44.1
	theApp.m_DMXEtui.GererAlarme();
	theApp.m_DMXNotice1.GererAlarme();
	theApp.m_DMXNotice2.GererAlarme();

	//Alarmes Pesons
	for(int i = 0; i < 6 ; i++)
	{
		theApp.Pesons[i].GererAlarme();
	}
	
	//REset Affichage des défauts en cours
	theApp.ResetDefautsEnCours();
	
	CString strTemp;
	
	//BM V1.16,	Reset du nombre de défauts par groupe
	for(int i = 0; i < NBGROUPES;i++)
	{
		GrpAlarmes[i].NbDefauts = 0;
	}

	for(int i = 0; i < NbTotal;i++)
	{
		strTemp = _T("");
		//Gestion Affichage des defauts
		//Alarme en cours
		if(Alarme[i].DB_Val != NULL)
		{
			Alarme[i].NewValue = *Alarme[i].DB_Val;
		}

		if(Alarme[i].NewValue != ALARME_OFF)
		{
			GrpAlarmes[Alarme[i].idGroupe].NbDefauts ++;

			strTemp = Alarme[i].strOrigine + ": " + Alarme[i].strLibelle;
		
			if(Alarme[i].idGroupe == GROUPE_ALERTES)
			{
				theApp.SetAlertesEnCours(strTemp);
			}
			else
			{
				theApp.SetDefautsEnCours(strTemp);
			}

			AlarmeEnCours = true;
		}

		//Gestion Traca Alarmes
		if((Alarme[i].Value != Alarme[i].NewValue))
		{
			if(Alarme[i].Traca == true)
			{
				if((Alarme[i].Value == ALARME_OFF)&&(Alarme[i].NewValue != ALARME_OFF))
				{
					Alarme[i].IdBase = theApp.theJournal.Ajouter(Alarme[i].Type,Alarme[i].strOrigine,Alarme[i].strLibelle);
				}
				else if((Alarme[i].Value != ALARME_OFF)&&(Alarme[i].NewValue == ALARME_OFF))
				{
					if(Alarme[i].IdBase > 0)
					{
						theApp.theJournal.Clore(Alarme[i].IdBase );
					}
					else
					{
						Alarme[i].IdBase = theApp.theJournal.Ajouter(Alarme[i].Type,Alarme[i].strOrigine,Alarme[i].strLibelle);
						theApp.theJournal.Clore(Alarme[i].IdBase );
					}
				}
			}
		}

		Alarme[i].Value = Alarme[i].NewValue;
	}

	//MAJ de la grille dans la fenetre alarme
	if(IsWindow(theApp.AlarmesView->GetSafeHwnd()))((CDialAlarmes *) theApp.AlarmesView)->UpdateAlarme();

	
	//gestion bouton alarme de la forme status
	if(AlarmeEnCours)
	{
		theApp.SetButAlarme(true);
	}
	else
	{
		theApp.SetButAlarme(false);
	}



	return 0;
}
void CGestAlarmes::InitialiserAlarmes()//GCA V1.16
{
	//Initialiser les groupes d'alarmes pour affichage
	GrpAlarmes[GROUPE_ROBOT1].strLibelle = _T("Robot 1");
	GrpAlarmes[GROUPE_ROBOT2].strLibelle = _T("Robot 2");
	GrpAlarmes[GROUPE_ROBOT3].strLibelle = _T("Robot 3");
	GrpAlarmes[GROUPE_OPC].strLibelle = _T("Com. OPC");
	GrpAlarmes[GROUPE_MACHINE].strLibelle = _T("Machine");
	GrpAlarmes[GROUPE_DMXETUI].strLibelle = _T("DMX Etui");
	GrpAlarmes[GROUPE_DMXNOTICE1].strLibelle = _T("DMX Notice 1");
	GrpAlarmes[GROUPE_DMXNOTICE2].strLibelle = _T("DMX Notice 2");
	GrpAlarmes[GROUPE_ALERTES].strLibelle = _T("Alertes");
	GrpAlarmes[GROUPE_CAM1].strLibelle = _T("Camera 1");
	GrpAlarmes[GROUPE_CAMUV].strLibelle = _T("Camera UV");
	GrpAlarmes[GROUPE_CAM_UEYE1].strLibelle = _T("Camera uEye 1");
	GrpAlarmes[GROUPE_CAM_UEYE2].strLibelle = _T("Camera uEye 2");
	GrpAlarmes[GROUPE_PESAGE].strLibelle = _T("Pesage");

	//Initialisations des alarmes Gérées par application
	
	if ((iIndex_ALARME_OPC < 0)||
		(iIndex_ALARME_DMXETUI< 0) ||
		(iIndex_ALARME_DMXNOTICE1< 0) ||
		(iIndex_ALARME_DMXNOTICE2< 0) ||

		//V44.1
		(iIndex_ALARME_COM_DMXETUI< 0) ||
		(iIndex_ALARME_COM_DMXNOTICE1< 0) ||
		(iIndex_ALARME_COM_DMXNOTICE2< 0) ||
		(iIndex_ALARME_TIMEOUT_DMXETUI< 0) ||
		(iIndex_ALARME_TIMEOUT_DMXNOTICE1< 0) ||
		(iIndex_ALARME_TIMEOUT_DMXNOTICE2< 0) ||
		(iIndex_ALARME_PESAGE< 0) ||

		(iIndex_ALARME_CAM1< 0) ||
		(iIndex_ALARME_CAM2< 0) ||
		(iIndex_ALARME_CAM3< 0) ||
		(iIndex_ALARME_CAM4< 0) ||
		(iIndex_ALARME_CAM1_SERIAL< 0) ||
		(iIndex_ALARME_CAM1_LINK< 0) ||
		(iIndex_ALARME_CAM1_SYNCHRO< 0) ||
		(iIndex_ALARME_CAM2_SERIAL< 0) ||
		(iIndex_ALARME_CAM2_LINK< 0) ||
		(iIndex_ALARME_CAM2_SYNCHRO< 0) )
	{
		CString strMessage = _T("il manque une definition d'alarme pour l'application !");
		AfxMessageBox(strMessage, MB_OK| MB_ICONERROR);
		return;
	}


	Alarme[iIndex_ALARME_OPC].NewValue = theApp.m_Recette.m_OPCClient->AlarmeInit;

	theApp.m_Recette.m_OPCClient->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.m_Robot1.m_OPCClient_TW->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.m_Robot1.m_OPCClient_TR->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.m_Robot2.m_OPCClient_TW->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.m_Robot2.m_OPCClient_TR->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.m_Robot3.m_OPCClient_TW->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.m_Robot3.m_OPCClient_TR->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.Process.m_OPCClient_R->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.Process.m_OPCClient_W->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.Compteurs.m_OPCClient_R->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;//GCA1412
	theApp.Process.m_OPCClient_Ai->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.Process.m_OPCClient_AFCy->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;
	theApp.Process.m_OPCClient_Alertes->Alarme = &Alarme[iIndex_ALARME_OPC].NewValue;

	theApp.m_Groupes[GROUPE_PESAGE_R].Alarme = &Alarme[iIndex_ALARME_PESAGE].NewValue;//V44.1
																										   //Alarmes DMX//V44.1
	theApp.m_DMXEtui.Alarme[DMXALM_PING] = &Alarme[iIndex_ALARME_DMXETUI];
	theApp.m_DMXEtui.Alarme[DMXALM_COMM] = &Alarme[iIndex_ALARME_COM_DMXETUI];
	theApp.m_DMXEtui.Alarme[DMXALM_TIMEOUT] = &Alarme[iIndex_ALARME_TIMEOUT_DMXETUI];

	theApp.m_DMXNotice1.Alarme[DMXALM_PING] = &Alarme[iIndex_ALARME_DMXNOTICE1];
	theApp.m_DMXNotice1.Alarme[DMXALM_COMM] = &Alarme[iIndex_ALARME_COM_DMXNOTICE1];
	theApp.m_DMXNotice1.Alarme[DMXALM_TIMEOUT] = &Alarme[iIndex_ALARME_TIMEOUT_DMXNOTICE1];

	theApp.m_DMXNotice2.Alarme[DMXALM_PING] = &Alarme[iIndex_ALARME_DMXNOTICE2];
	theApp.m_DMXNotice2.Alarme[DMXALM_COMM] = &Alarme[iIndex_ALARME_COM_DMXNOTICE2];
	theApp.m_DMXNotice2.Alarme[DMXALM_TIMEOUT] = &Alarme[iIndex_ALARME_TIMEOUT_DMXNOTICE2];

	//Alarmes Cameras
	theApp.DalsaCamera1.p_Alarme[CAMALM_UNUSED] = &Alarme[iIndex_ALARME_CAM1];
	theApp.DalsaCamera1.p_Alarme[CAMALM_SERIAL] = &Alarme[iIndex_ALARME_CAM1_SERIAL];
	theApp.DalsaCamera1.p_Alarme[CAMALM_LINK] = &Alarme[iIndex_ALARME_CAM1_LINK];
	theApp.DalsaCamera1.p_Alarme[CAMALM_SYNCHRO] = &Alarme[iIndex_ALARME_CAM1_SYNCHRO];

	theApp.DalsaCamera2.p_Alarme[CAMALM_UNUSED] = &Alarme[iIndex_ALARME_CAM2];
	theApp.DalsaCamera2.p_Alarme[CAMALM_SERIAL] = &Alarme[iIndex_ALARME_CAM2_SERIAL];
	theApp.DalsaCamera2.p_Alarme[CAMALM_LINK] = &Alarme[iIndex_ALARME_CAM2_LINK];
	theApp.DalsaCamera2.p_Alarme[CAMALM_SYNCHRO] = &Alarme[iIndex_ALARME_CAM2_SYNCHRO];

	//Liaison des Bits OPC avec les alarmes
	for (int i = 0; i < NbTotal;i++)
	{
		if (Alarme[i].DB_Num > 0)
		{
			switch (Alarme[i].DB_Num)
			{

			case 301:
				if (Alarme[i].DB_Adresse >= NB_MOTS_DB30X)
				{
					CString strMessage = _T("");
					strMessage.Format(_T("Alarme %d Adresse 301.%d trop haute ( >= NB_MOTS_DB30X (%d))"), i, Alarme[i].DB_Adresse, NB_MOTS_DB30X);
					AfxMessageBox(strMessage, MB_OK|MB_ICONWARNING);
					continue;
				}

				Alarme[i].DB_Val = &theApp.Process.Def301[Alarme[i].DB_Adresse][Alarme[i].DB_Bit];
				break;

			case 302:
				if (Alarme[i].DB_Adresse >= NB_MOTS_DB30X)
				{
					CString strMessage = _T("");
					strMessage.Format(_T("Alarme %d Adresse 302.%d trop haute ( >= NB_MOTS_DB30X (%d))"), i, Alarme[i].DB_Adresse, NB_MOTS_DB30X);
					AfxMessageBox(strMessage, MB_OK|MB_ICONWARNING);
					continue;
				}

				Alarme[i].DB_Val = &theApp.Process.Def302[Alarme[i].DB_Adresse][Alarme[i].DB_Bit];
				break;

			case 303:
				if (Alarme[i].DB_Adresse >= NB_MOTS_DB30X)
				{
					CString strMessage = _T("");
					strMessage.Format(_T("Alarme %d Adresse 303.%d trop haute ( >= NB_MOTS_DB30X (%d))"), i, Alarme[i].DB_Adresse, NB_MOTS_DB30X);
					AfxMessageBox(strMessage, MB_OK|MB_ICONWARNING);
					continue;
				}

				Alarme[i].DB_Val = &theApp.Process.Def303[Alarme[i].DB_Adresse][Alarme[i].DB_Bit];
				break;

			default:
				break;

			}
		}
	}

	//Recuperation Valeur Alarme Init GCA V1.17
	//theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue = theApp.m_Recette.m_OPCClient->AlarmeInit;

	//theApp.m_Recette.m_OPCClient->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.m_Robot1.m_OPCClient_TW->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.m_Robot1.m_OPCClient_TR->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.m_Robot2.m_OPCClient_TW->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.m_Robot2.m_OPCClient_TR->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.m_Robot3.m_OPCClient_TW->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.m_Robot3.m_OPCClient_TR->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.Process.m_OPCClient_R->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.Process.m_OPCClient_W->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.Compteurs.m_OPCClient_R->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;//GCA1412
	//theApp.Process.m_OPCClient_Ai->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.Process.m_OPCClient_AFCy->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;
	//theApp.Process.m_OPCClient_Alertes->Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue;

	//theApp.m_Groupes[GROUPE_PESAGE_R].Alarme = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_PESAGE].NewValue;//V44.1
	////Alarmes DMX//V44.1
	//theApp.m_DMXEtui.Alarme[DMXALM_PING] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_DMXETUI];
	//theApp.m_DMXEtui.Alarme[DMXALM_COMM] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_COM_DMXETUI];
	//theApp.m_DMXEtui.Alarme[DMXALM_TIMEOUT] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_TIMEOUT_DMXETUI];

	//theApp.m_DMXNotice1.Alarme[DMXALM_PING] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_DMXNOTICE1];
	//theApp.m_DMXNotice1.Alarme[DMXALM_COMM] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_COM_DMXNOTICE1];
	//theApp.m_DMXNotice1.Alarme[DMXALM_TIMEOUT] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_TIMEOUT_DMXNOTICE1];

	//theApp.m_DMXNotice2.Alarme[DMXALM_PING] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_DMXNOTICE2];
	//theApp.m_DMXNotice2.Alarme[DMXALM_COMM] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_COM_DMXNOTICE2];
	//theApp.m_DMXNotice2.Alarme[DMXALM_TIMEOUT] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_TIMEOUT_DMXNOTICE2];

	////Alarmes Cameras
	//theApp.DalsaCamera1.p_Alarme[CAMALM_UNUSED] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1];
	//theApp.DalsaCamera1.p_Alarme[CAMALM_SERIAL] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1_SERIAL];
	//theApp.DalsaCamera1.p_Alarme[CAMALM_LINK] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1_LINK];
	//theApp.DalsaCamera1.p_Alarme[CAMALM_SYNCHRO] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1_SYNCHRO];

	//theApp.DalsaCamera2.p_Alarme[CAMALM_UNUSED] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2];
	//theApp.DalsaCamera2.p_Alarme[CAMALM_SERIAL] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2_SERIAL];
	//theApp.DalsaCamera2.p_Alarme[CAMALM_LINK] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2_LINK];
	//theApp.DalsaCamera2.p_Alarme[CAMALM_SYNCHRO] = &theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2_SYNCHRO];
 //	
	////Liaison des Bits OPC avec les alarmes
	//for(int i = 0; i < NbTotal;i++)
	//{	
	//	if(theApp.m_GestAlarmes.Alarme[i].DB_Num > 0)
	//	{
	//		switch(	theApp.m_GestAlarmes.Alarme[i].DB_Num)
	//		{

	//			case 301:
	//				if (theApp.m_GestAlarmes.Alarme[i].DB_Adresse >= NB_MOTS_DB301)
	//				{
	//					CString strMessage = _T("");
	//					strMessage.Format(_T("Alarme %d Adresse %d trop haute ( >= NB_MOTS_DB301)"), i, theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//					AfxMessageBox(strMessage, MB_OK, MB_ICONWARNING);
	//					continue;
	//				}

	//				theApp.m_GestAlarmes.Alarme[i].DB_Val = &theApp.Process.Def301[theApp.m_GestAlarmes.Alarme[i].DB_Adresse][theApp.m_GestAlarmes.Alarme[i].DB_Bit];
	//				break;

	//			case 302:
	//				if (theApp.m_GestAlarmes.Alarme[i].DB_Adresse >= NB_MOTS_DB302)
	//				{
	//					CString strMessage = _T("");
	//					strMessage.Format(_T("Alarme %d Adresse %d trop haute ( >= NB_MOTS_DB302)"), i, theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//					AfxMessageBox(strMessage, MB_OK, MB_ICONWARNING);
	//					continue;
	//				}

	//				theApp.m_GestAlarmes.Alarme[i].DB_Val = &theApp.Process.Def302[theApp.m_GestAlarmes.Alarme[i].DB_Adresse][theApp.m_GestAlarmes.Alarme[i].DB_Bit];
	//				break;

	//			case 303:
	//				if (theApp.m_GestAlarmes.Alarme[i].DB_Adresse >= NB_MOTS_DB303)
	//				{
	//					CString strMessage = _T("");
	//					strMessage.Format(_T("Alarme %d Adresse %d trop haute ( >= NB_MOTS_DB303)"), i, theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//					AfxMessageBox(strMessage, MB_OK, MB_ICONWARNING);
	//					continue;
	//				}

	//				theApp.m_GestAlarmes.Alarme[i].DB_Val = &theApp.Process.Def303[theApp.m_GestAlarmes.Alarme[i].DB_Adresse][theApp.m_GestAlarmes.Alarme[i].DB_Bit];
	//				break;

	//			default:
	//				break;

	//		}
	//	}
	//}
}

