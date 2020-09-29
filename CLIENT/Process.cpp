#include "StdAfx.h"
#include "Process.h"
#include "BaseFenetre.h"
#include "splitfrm.h"
#include "DialChargement.h"
#include "DialPesons.h"
#include "DialRobot.h"
#include "DialValidLot.h"
#include "DialMachine.h"
#include "DialInitialisation.h"
#include "DialSplashInit.h"
//#include "ListBoxNotify.h"
#include "DialFinOF.h"
#include "DiagPadAlpha.h"
#include "DiagPadNum.h"
#include "DialAffGridRecette.h"
#include "AC_Base.h"
#include "DialSelectPrint.h"
#include "FormBase.h"
#include "DialCameras.h"
#include "DialEtiquette.h"
#include "FormStatus.h"
#include "DialDefautPiste.h"

CProcess::CProcess(void)
{
	iCyclePulse = 0;
	Step = 0;
	EtatMachine = 0;
	FonctStartCycle = 0;
	bStopThread = false;
	bThreadStopped = true;
	
	IndexStartCycle = -1;
	m_OPCClient_R = NULL;
	m_OPCClient_W = NULL;
	iNbCycleInit = 0;

	iOldEtat = ETAT_PROCESS_HORSPRODUCTION;
	iEtat = ETAT_PROCESS_INITAPP;
	iCmdAuto = 0;
	iCmdInitOK = 0;

	pBitAuto = NULL;
	pBitMaintenance = NULL;
	pBitVidage = NULL;
	pBitVidage2 = NULL;		//V2.42
	pBitAcquitDefaut = NULL;
	pBitMachineVide = NULL;
	pBitDeVie = NULL;
	pBitEtalonnageEtui = NULL;
	pBitAutorisationDebutOF = NULL;

	pBitEclairageSup = NULL;
	pBitEclairageLat = NULL;
	pBitEclairageUV = NULL;

	/*pDMX[0] = NULL;
	pDMX[1] = NULL;
	pDMX[2] = NULL;*/

	dCadence = 0;

	for(int y = 0; y < 8; y++)
	{
		WBit9[y] = FALSE;

		//V 30-ALL
		ROctetForcage[y] = TRUE;
		WOctetFictif[y] = FALSE;

		for(int i = 0 ; i < 5; i ++) Def301[i][y] = FALSE;
		for(int i = 0 ; i < 34; i ++) Def302[i][y] = FALSE;	//V 42 de 32 à 34
		for(int i = 0 ; i < 18; i ++) Def303[i][y] = FALSE;
	}

	sEtat[ETAT_PROCESS_HORSPRODUCTION] = _T("HORS PRODUCTION");
	sEtat[ETAT_PROCESS_CHARGEMENTOF] = _T("CHARGEMENT OF");
	sEtat[ETAT_PROCESS_VALIDATIONFINLOT] = _T("VALIDATION FIN LOT");
	sEtat[ETAT_PROCESS_INITIALISATIONMACHINE] = _T("INITIALISATION");
	sEtat[ETAT_PROCESS_VALIDATIONDEBUTLOT] = _T("VALIDATION DEBUT LOT");
	sEtat[ETAT_PROCESS_VALIDATIONDEBUTOF] = _T("VALIDATION DEBUT OF");
	sEtat[ETAT_PROCESS_PRODUCTIONARRET] = _T("PRODUCTION ARRET");
	sEtat[ETAT_PROCESS_PRODUCTIONDEMARRAGE] = _T("DEMARRAGE");
	sEtat[ETAT_PROCESS_PRODUCTIONMARCHE] = _T("PRODUCTION MARCHE");
	sEtat[ETAT_PROCESS_VALIDATIONFINOF] = _T("VALIDATION FIN OF");
	sEtat[ETAT_PROCESS_MIXUP] = _T("MIXUP");
	sEtat[ETAT_PROCESS_VERIFDYNAMIQUE] = _T("VERIFICATION DYNAMIQUE");
	sEtat[ETAT_PROCESS_INITAPP] = _T("DEMARRAGE APP");
	sEtat[ETAT_PROCESS_ARRETENCOURS] = _T("ARRET EN COURS");
	sEtat[ETAT_PROCESS_VIDAGEARRET] = _T("VIDAGE ARRET");
	sEtat[ETAT_PROCESS_VIDAGEMARCHE] = _T("VIDAGE MARCHE");
	sEtat[ETAT_PROCESS_VERIFETALONNAGE] = _T("VERIFICATION ETALONNAGE");
	sEtat[ETAT_PROCESS_VIDAGE2] = _T("VIDAGE INTRODUCTION");
	sEtat[ETAT_PROCESS_ETALONNAGE_ETUI_ARRET] = _T("ETALONNAGE ETUI ARRET");
	sEtat[ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE] = _T("ETALONNAGE ETUI MARCHE");
	
	Def301_AdresseMax = 0;//V 44.0
	Def302_AdresseMax = 0;//V 44.0
	Def303_AdresseMax = 0;//V 44.0

	//V 44.7
	for (int i = 0; i < NB_VAL_CADENCE_MAX; i++)
	{
		dTab_CadenceMachine[i] = 0;
	}
	iModulo_CadenceMachine = 0;
	iNbMoy_CadenceMachine = 0;
	dMoy_CadenceMachine = 0;

	//V 44.8
	for (int i = 0; i < 3; i++)
	{
		CadencePrises[i].dMoy_Cadence = 0;
		CadencePrises[i].ulTickTimeoutAcquit = 0;
		CadencePrises[i].iModulo_Cadence = 0;

		for (int y = 0; y < NB_VAL_CADENCE_MAX; y++)
		{
			CadencePrises[i].dTab_Cadence[y] = 0;
			CadenceTapis[i].dTab_Cadence[y] = 0;
		}

		CadenceTapis[i].dMoy_Cadence = 0;
		CadenceTapis[i].ulTickTimeoutAcquit = 0;
		CadenceTapis[i].iModulo_Cadence = 0;
	}

	iModeCadence = 0;
	ulTickDebutControle = 0;
	

	iCadence_NbValCalcul = 0;
}

CProcess::~CProcess(void)
{
}

int CProcess::Gerer(int iCommande)
{

	switch(iEtat)
	{
		
		case ETAT_PROCESS_HORSPRODUCTION:
			GererEtat_HorsProduction(iCommande);
			break;

		case ETAT_PROCESS_CHARGEMENTOF:
			GererEtat_ChargementOf(iCommande);
			break;

		case ETAT_PROCESS_VALIDATIONFINLOT:
			GererEtat_ValidationFinLot(iCommande,iCmdAuto);
			break;

		case ETAT_PROCESS_INITIALISATIONMACHINE:
			if((iCommande > 0)&&(iCmdAuto == 0))//on autorise pas les boutons tant que auto est pas fini
			{
				GererEtat_Initialisation(iCommande);

				iCommande = 0;
				theApp.MAJMenuEtat();
			}
			else if(iCmdAuto > 0)
			{
				int iCmd = iCmdAuto;
				iCmdAuto = 0;
				
				GererEtat_InitialisationAuto(iCmd);

			}
			//Validation Auto si init OK
			else if(iCmdInitOK == CMD_VALIDER)
			{
				iCmdInitOK = 0;
				GererEtat_Initialisation(CMD_VALIDER);
				
				iCommande = 0;
			}
			break;

		case ETAT_PROCESS_VALIDATIONDEBUTLOT:

			GererEtat_ValidationDebutLot(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_VALIDATIONDEBUTOF	:

			GererEtat_ValidationDebutOf(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_PRODUCTIONARRET:

			GererEtat_ProductionArret(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;
		case ETAT_PROCESS_PRODUCTIONDEMARRAGE:
			
			GererEtat_Demarrage(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;	

		case ETAT_PROCESS_PRODUCTIONMARCHE:
			
			GererEtat_ProductionMarche(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_VALIDATIONFINOF:

			GererEtat_ValidationFinOf(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_MIXUP:

			GererEtat_Mixup(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;	

		case ETAT_PROCESS_VERIFDYNAMIQUE:

			GererEtat_VerifDynamique(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_INITAPP:

			GererEtat_InitApp();
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_ARRETENCOURS:

			GererEtat_ArretEnCours(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_VIDAGEARRET:

			GererEtat_VidageArret(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_VIDAGEMARCHE:

			GererEtat_VidageMarche(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_VIDAGE2:
			GererEtat_Vidage2(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_VERIFETALONNAGE:

			GererEtat_VerifEtalonnage(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_ETALONNAGE_ETUI_ARRET:

			GererEtat_EtalonnageEtui_Arret(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		case ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE:

			GererEtat_EtalonnageEtui_Marche(iCommande,iCmdAuto);
			if(iCmdAuto > 0)
			{
				iCmdAuto = 0;
			}
			break;

		default:
			break;
	}
	return TRUE;

}

bool CProcess::GererCycle(int iNum)
{
	CProcess * m_Process = &theApp.Process;
	CClientOPC * m_Client = m_Process->m_OPCClient_W ;
	BOOL bValues[8] = {FALSE};

	for(int i = 0; i < 8; i++)
	{
		bValues[i] = ((BOOL*)(m_Client->my_Items[m_Process->IndexStartCycle].pValue))[i];
	}

	//GESTION Pulse Start cycle [en N passages ~ (N*10ms + 10ms)]
	if(m_Process->IndexStartCycle != -1)
	{
		if(bValues[INDEX_BIT_STARTCYCLE_W] == TRUE)
		{
			//on attend N cycles
			if(m_Process->iCyclePulse >= pParamNbCyclePulse->value)
			{
				m_Process->iCyclePulse = 0;
				//on repasse à 0
				((BOOL*)(m_Client->my_Items[m_Process->IndexStartCycle].pValue))[INDEX_BIT_STARTCYCLE_W] = FALSE;
			}
			else
			{
				m_Process->iCyclePulse++ ;
			}
		}
		else
		{
			//on fait rien

		}
		
		//Pulse STOP cycle
		if(bValues[INDEX_BIT_STOPCYCLE_W] == TRUE)
		{
			//on attend N cycles
			if(m_Process->iCyclePulse >= pParamNbCyclePulse->value)
			{
				m_Process->iCyclePulse = 0;
				//on repasse à 0
				((BOOL*)(m_Client->my_Items[m_Process->IndexStartCycle].pValue))[INDEX_BIT_STOPCYCLE_W] = FALSE;
			}
			else
			{
				m_Process->iCyclePulse++ ;
			}
		}
		else
		{
			//on fait rien

		}
	}

	//GESTION Pulse Acquittement défauts
	if(*m_Process->pBitAcquitDefaut == TRUE)
	{
		//on attend N cycles
		if(m_Process->iCyclePulse >= pParamNbCyclePulse->value)
		{
			m_Process->iCyclePulse = 0;
			//on repasse à 0
			*m_Process->pBitAcquitDefaut = FALSE;
		}
		else
		{
			m_Process->iCyclePulse++ ;
		}
	}

	//Gestion défauts
	m_Process->GererDefautsMachine();

	//Gestion Snap Etiquette
	if((*theApp.VisionBoite.pBitSnap == TRUE)&&(theApp.VisionBoite.oldBitSnap == FALSE))
	{
		//Front montant
		theApp.VisionBoite.Process_BitSnap();
	}
	else
	{
		//Front descendant
	}	
	theApp.VisionBoite.oldBitSnap = *theApp.VisionBoite.pBitSnap;
	//FIN Gestion Snap Etiquette


	//Gestion Lecture DMX Leuze //#3 DMX
	if ((*theApp.m_DMXEtui.pBitLecture == TRUE) && (theApp.m_DMXEtui.oldBitLecture == FALSE))
	{
		//Front montant
		theApp.m_DMXEtui.Process_BitLecture();
	}
	else
	{
		//Front descendant
	}
	theApp.m_DMXEtui.oldBitLecture = *theApp.m_DMXEtui.pBitLecture;

	if ((*theApp.m_DMXNotice1.pBitLecture == TRUE) && (theApp.m_DMXNotice1.oldBitLecture == FALSE))
	{
		//Front montant
		theApp.m_DMXNotice1.Process_BitLecture();
	}
	else
	{
		//Front descendant
	}
	theApp.m_DMXNotice1.oldBitLecture = *theApp.m_DMXNotice1.pBitLecture;

	if ((*theApp.m_DMXNotice2.pBitLecture == TRUE) && (theApp.m_DMXNotice2.oldBitLecture == FALSE))
	{
		//Front montant
		theApp.m_DMXNotice2.Process_BitLecture();
	}
	else
	{
		//Front descendant
	}
	theApp.m_DMXNotice2.oldBitLecture = *theApp.m_DMXNotice2.pBitLecture;
	//FIN Gestion Lecture DMX Leuze

//TO REMOVECString strTemp;

	switch(m_Process->iEtat)
	{
	case ETAT_PROCESS_PRODUCTIONARRET:
	case ETAT_PROCESS_VIDAGEARRET:
	case ETAT_PROCESS_ETALONNAGE_ETUI_ARRET:
	case ETAT_PROCESS_PRODUCTIONDEMARRAGE:
		//Gestion Entrée dans Cycle Auto
			
			//TO REMOVEstrTemp.Format("|%d|%d|%d|%d|%d|%d|%d|%d|\n",m_Process->RBit3[0],m_Process->RBit3[1],m_Process->RBit3[2],m_Process->RBit3[3],m_Process->RBit3[4],m_Process->RBit3[5],m_Process->RBit3[6],m_Process->RBit3[7]);
			//TO REMOVETRACE2(strTemp,NULL,NULL);

			if((m_Process->RBit3[INDEX_BIT_STATUSPROD_R] == TRUE))
			{
				// on passe en Etalonnage Etui si Bit Actif
				if(*m_Process->pBitEtalonnageEtui == TRUE)
				{
					m_Process->iEtat = ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE;
				}
				// on passe en vidage si Bit Actif
				else if(*m_Process->pBitVidage == TRUE)
				{
					m_Process->iEtat = ETAT_PROCESS_VIDAGEMARCHE;
				}
				else
				{
					m_Process->iEtat = ETAT_PROCESS_PRODUCTIONMARCHE;
				}
				m_Process->iCmdAuto = 1;//Rafraichissement fenetre voir Process::Gerer Case PRODUCTION MARCHE/ARRET
			}
		//A VOIR
			else if((m_Process->RBit3[INDEX_BIT_STATUSARRET_R] == TRUE))
			{
				m_Process->iNbCycleInit ++;

				if(m_Process->iNbCycleInit > PROCESS_NBCYCLE_BITARRETINIT)
				{
					m_Process->iNbCycleInit = 0;
					
					if(*m_Process->pBitVidage == TRUE)
					{
						m_Process->iEtat = ETAT_PROCESS_VIDAGEARRET;
					}
					else if(*m_Process->pBitEtalonnageEtui == TRUE)
					{
						m_Process->iEtat = ETAT_PROCESS_ETALONNAGE_ETUI_ARRET;
					}
					else
					{
						//MAJ sortie du mode vidage
						if(m_Process->iEtat!=ETAT_PROCESS_PRODUCTIONARRET)m_Process->iCmdAuto = 1;

						m_Process->iEtat = ETAT_PROCESS_PRODUCTIONARRET;
					}
					
					if(m_Process->iEtat!=ETAT_PROCESS_PRODUCTIONARRET)m_Process->iCmdAuto = 1;//Rafraichissement fenetre voir Process::Gerer Case PRODUCTION MARCHE/ARRET
				}
			}
			else if((m_Process->RBit3[INDEX_BIT_STATUSINIT_R] == TRUE))
			{
				if(m_Process->iEtat!=ETAT_PROCESS_PRODUCTIONDEMARRAGE)m_Process->iCmdAuto = 1;

				m_Process->iEtat = ETAT_PROCESS_PRODUCTIONDEMARRAGE;
			}

		break;

	case ETAT_PROCESS_PRODUCTIONMARCHE:
		//Gestion Sortie du cycle Marche
			if((m_Process->RBit3[INDEX_BIT_STATUSPROD_R] != TRUE))
			{
				m_Process->iEtat = ETAT_PROCESS_ARRETENCOURS;
				m_Process->iCmdAuto = 1;//Rafraichissement fenetre voir Process::Gerer Case PRODUCTION MARCHE/ARRET
			}

		break;
		
	case ETAT_PROCESS_MIXUP:
			//Sortie de l'Etat Mixup
			if((m_Process->RBit2[INDEX_BIT_DEFAUTROBOTS_MIXUPVISION_R] == FALSE)&&(m_Process->RBit2[INDEX_BIT_DEFAUTROBOTS_MIXUPETUYEUSE_R] == FALSE))
			{
				m_Process->iEtat = m_Process->iOldEtat;
				
				////MAJ fenetre précédent le mixup
				SendMessage(theApp.MachineView->GetSafeHwnd(),WM_MAJ_VIEWMACHINE,0,0);

				//MAj de l'Etat du process
				SendMessage(theApp.MachineView->GetSafeHwnd(),WM_MAJ_ETAT,0,0);
			}
		break;

	case ETAT_PROCESS_ARRETENCOURS:
		//Gestion Sortie de l'arret en cours
			if((m_Process->RBit3[INDEX_BIT_STATUSARRET_R] == TRUE ))
			{
				if(*m_Process->pBitVidage == TRUE)
				{
					m_Process->iEtat = ETAT_PROCESS_VIDAGEARRET;
				}
				else
				{
					m_Process->iEtat = ETAT_PROCESS_PRODUCTIONARRET;
				}
				
				m_Process->iCmdAuto = 1;//Rafraichissement fenetre voir Process::Gerer Case PRODUCTION MARCHE/ARRET
			}
		break;

	case ETAT_PROCESS_VIDAGEMARCHE:
		//Gestion Sortie du cycle Vidage pour revenir à l'état précédent
			if((m_Process->RBit3[INDEX_BIT_STATUSPROD_R] != TRUE))
			{
				m_Process->iEtat = ETAT_PROCESS_VIDAGEARRET;
				
				//MAj de l'Etat du process
				SendMessage(theApp.MachineView->GetSafeHwnd(),WM_MAJ_ETAT,0,0);
			}
			break;

	case ETAT_PROCESS_VIDAGE2:
		//Rien a faire
		break;

	case ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE:
			//Gestion Sortie du cycle Etalonnage Etui pour revenir à l'état précédent
			if((m_Process->RBit3[INDEX_BIT_STATUSPROD_R] != TRUE))
			{
				m_Process->iEtat = ETAT_PROCESS_ETALONNAGE_ETUI_ARRET;
				
				//MAj de l'Etat du process
				SendMessage(theApp.MachineView->GetSafeHwnd(),WM_MAJ_ETAT,0,0);
			}

			//Sortie si fin cyle OK
			if(*theApp.PesonEtui.pBitFinEtalonnageOK == TRUE)
			{
				//arret du cycle si fin OK
				m_Process->WBit0[INDEX_BIT_STOPCYCLE_W] = TRUE;
			}
			break;

	default:
		break;

	}

	
	

	return false;
}

int CProcess::GererDefautsMachine(void)
{
	//Alertes = DB 303
	//Ai = DB 301
	//AFCy = DB 302
	CString strPrev = strErreur;
	CString strDefauts;
	int iNbDefauts = 0;

	//Defaut MIXUP
	if((RBit2[INDEX_BIT_DEFAUTROBOTS_MIXUPVISION_R] == TRUE)||(RBit2[INDEX_BIT_DEFAUTROBOTS_MIXUPETUYEUSE_R] == TRUE))
	{
		//Passage dans l'état MIXUP
		if(iEtat != ETAT_PROCESS_MIXUP)
		{
			*theApp.VisionBoite.pBitResAcquisition_OK = FALSE;
			*theApp.VisionBoite.pBitResAcquisition_NOK = FALSE;
			*theApp.VisionBoite.pBitMixup = FALSE;

			iOldEtat = iEtat;
			iEtat = ETAT_PROCESS_MIXUP;
			iCmdAuto = 1;//MAJ etat IHM
			strErreur  = theApp.TableMessage.GetStr(MESSAGE_DEFAUT_MIXUP);
		}
	}

	//Lecture des alertes GCA V1.18
	m_OPCClient_Alertes->ReadItemsGroup();
	m_OPCClient_Alertes->OPCToData();

	//Lecture Tous les Defauts
	if(iEtat != ETAT_PROCESS_PRODUCTIONMARCHE)
	{
		m_OPCClient_Ai->ReadItemsGroup();
		m_OPCClient_Ai->OPCToData();

		m_OPCClient_AFCy->ReadItemsGroup();
		m_OPCClient_AFCy->OPCToData();
	}
	return 0;
}

int CProcess::GererAlarme(void)
{

	return 0;
}
int CProcess::GererEtat_HorsProduction(int iCommande)
{
	if(iCommande > 0)
	{
							
		CString strMess;

		switch(iCommande)
		{
		case CMD_VALIDER:

			//Vérification Machine Vide
			if(theConfig.str_Machine.Right(5) != "SIMUL")
			{
				if(*pBitMachineVide == FALSE)
				{
					strMess.Format(theApp.TableMessage.GetStr(MESSAGE_ASK_MACHINENONVIDE));
					//Message
					if(AfxMessageBox(strMess,MB_YESNO,MB_ICONQUESTION)== IDYES)
					{
						CString sUserMachNonVide;
						CString sdateMachNonVide;

						//Demande login Manager
						if(theApp.DemandeLoginManager(sUserMachNonVide,sdateMachNonVide))
						{
							//Traca forcage machine non vide
							CString strCom;
							strCom.Format(theApp.TableMessage.GetStr(MESSAGE_JOURNAL_MACHINENONVIDE));
							theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE,strCom,sdateMachNonVide,sUserMachNonVide);
							//on passe a la suite
						}
						else
						{
							//Refus suite Erreur Login ou Not Manager
							iCommande = 0;
							theApp.MAJMenuEtat();
							return -1;
						}
					}
					else
					{
						//Refus suite
						iCommande = 0;
						theApp.MAJMenuEtat();
						return -1;
					}
				}
			}

			//Verification Recette En Cours GCA V1.17
			if(theApp.m_RecetteNew.IsRecetteEnCours())
			{
				//Reprise de la recette en cours
				theApp.m_RecetteNew.ReCharger(theApp.m_RecetteNew.OfNum);
				
				theApp.DialMachineEnCours = theApp.InitialisationView;
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				iEtat = ETAT_PROCESS_INITIALISATIONMACHINE;
				iCmdAuto = 1;

				//Ecriture OPC Etat Machine
				EtatMachine = ETATMACHINE_INITIALISATION;
			}
			else
			{
				//Reset de la RecetteNew
				theApp.m_RecetteNew.Reset();
			

				((CDialChargement * )theApp.ChargementView)->Recette = &theApp.m_RecetteNew;
				((CDialChargement * )theApp.ChargementView)->m_ListEvents.ResetContent();
				((CDialChargement * )theApp.ChargementView)->MAJRecetteWindows();

				theApp.DialMachineEnCours = theApp.ChargementView;
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				//Verification Fichier CSV
				((CDialChargement * )theApp.ChargementView)->CSV_Charger();

				iEtat = ETAT_PROCESS_CHARGEMENTOF;

				*theApp.m_Recette.pBitRazEtalons = TRUE;
			}
			break;

		case CMD_ANNULER:
			//On ne fait rien
			break;

		default:
			break;
		}

	iCommande = 0;
	theApp.MAJMenuEtat();

	}

	return 0;
}

int CProcess::GererEtat_ChargementOf(int iCommande)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		case CMD_VALIDER:
			//recette inexistante ou non chargée
			if(!theApp.m_RecetteNew.IsLoaded)
			{
				CString strMess;
				strMess.Format(theApp.TableMessage.GetStr(MESSAGE_RECETTE_NOK));
				//Message
				theApp.SetEventsJournalStr(strMess);
				theApp.SetInfosString(strMess);

				//refus de la validation
				iCommande = 0;
				theApp.MAJMenuEtat();
				return 0;
			}
			
			//V19 : Controle de vidage
			//#1 Forcage
			if(*pBitAutorisationDebutOF == FALSE)
			{
				theApp.SetInfosString("VIDAGE NON EFFECTUE");
				//refus de la validation
				iCommande = 0;
				theApp.MAJMenuEtat();
				return 0;
			}

			//Recette en cours & Lot Different
			if((theApp.m_Recette.strLot!= theApp.m_RecetteNew.strLot)&&(theApp.m_Recette.strLot != _T("")))
			{
				//Validation du lot précédent
				for(int i= 0; i < 6; i++)
				{
					//theApp.Pesons[i].CurrentRecette = &theApp.m_Recette;

					//Verification poids Etalons//GCA1412
					if((theApp.Pesons[i].PoidsEtalon1 != 0)||(theApp.Pesons[i].PoidsEtalon2 != 0))
					{
						CString strMess;
						strMess.Format(theApp.TableMessage.GetStr(MESSAGE_ETALONS_NOK));
						//Message
						theApp.SetEventsJournalStr(strMess);
						theApp.SetInfosString(strMess);
						//Refus Validation A VOIR
						iCommande = 0;
						theApp.MAJMenuEtat();
						return 0;
					}

				}

				theApp.m_Recette.iStatut = RECETTE_STATUT_FINLOT;
				theApp.m_Recette.Update();

				*theApp.m_Recette.pBitRazEtalons = TRUE;

				((CDialValidLot*)theApp.ValidLotView)->Recette = &theApp.m_Recette;
				((CDialValidLot*)theApp.ValidLotView)->bAllPesons = true;
				((CDialValidLot*)theApp.ValidLotView)->bDebutLot = false;

				theApp.DialMachineEnCours = theApp.ValidLotView;
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				((CDialValidLot*)theApp.ValidLotView)->StartLecture();

				iEtat = ETAT_PROCESS_VALIDATIONFINLOT;

				
			}
			//Recette en cours & Lot Egaux
			else if((theApp.m_Recette.strLot == theApp.m_RecetteNew.strLot)&&(theApp.m_Recette.strLot != _T("")))
			{
				theApp.m_Recette.iStatut = RECETTE_STATUT_TERMINEE;
				theApp.m_Recette.Update();
				theApp.m_Recette.EndRecetteEnCours();//GCA V1.17
				theApp.m_Recette.SetUtilisateurFin();//V2.50
				Gerer_ImpressionAutoFinOF(theApp.m_Recette.OfNum);

				theApp.DialMachineEnCours = theApp.InitialisationView;
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				iEtat = ETAT_PROCESS_INITIALISATIONMACHINE;
				iCmdAuto = 1;

				//Ecriture OPC Etat Machine
				EtatMachine = ETATMACHINE_INITIALISATION;
			}
			else
			{
				theApp.DialMachineEnCours = theApp.InitialisationView;
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				iEtat = ETAT_PROCESS_INITIALISATIONMACHINE;
				iCmdAuto = 1;

				//Ecriture OPC Etat Machine
				EtatMachine = ETATMACHINE_INITIALISATION;

			}

			break;

		case CMD_ANNULER:
			if(theApp.m_Recette.strLot != _T(""))
			{
				//Validation du lot précédent
				for(int i= 0; i < 6; i++)
				{
				//	theApp.Pesons[i].CurrentRecette = &theApp.m_Recette;

					//Verification poids Etalons//GCA1412
					if((theApp.Pesons[i].PoidsEtalon1 != 0)||(theApp.Pesons[i].PoidsEtalon2 != 0))
					{
						//Message Erreur
						CString strMess;
						strMess.Format(theApp.TableMessage.GetStr(MESSAGE_ETALONS_NOK));
						//Message
						theApp.SetEventsJournalStr(strMess);
						theApp.SetInfosString(strMess);

						//Refus Validation A VOIR
						iCommande = 0;
						theApp.MAJMenuEtat();
						return 0;
					}
				}

				theApp.m_Recette.iStatut = RECETTE_STATUT_FINLOT;
				theApp.m_Recette.Update();

				((CDialValidLot*)theApp.ValidLotView)->Recette = &theApp.m_Recette;
				((CDialValidLot*)theApp.ValidLotView)->bAllPesons = true;
				((CDialValidLot*)theApp.ValidLotView)->bDebutLot = false;

				theApp.DialMachineEnCours = theApp.ValidLotView;
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				((CDialValidLot*)theApp.ValidLotView)->StartLecture();//Rafraichissement fenetre
				
				iEtat = ETAT_PROCESS_VALIDATIONFINLOT;
			}
			else
			{
				theApp.DialMachineEnCours = theApp.MachineView;
				//A VOIR reset de la recette 
				theApp.m_RecetteOld.Reset();
				theApp.m_Recette.Reset();

				//MAJ fenetre en focntion de la recette en cours
				((CDialMachine * )theApp.MachineView)->InitialiserTextes();
				//Changement Etat bouton DialMachine
				((CDialMachine * )theApp.MachineView)->SetStateButProcess(0);
				
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				iEtat = ETAT_PROCESS_HORSPRODUCTION;

				*pBitEclairageSup = FALSE;
				*pBitEclairageUV = FALSE;
			}

			break;

		default:
			break;
		}

	iCommande = 0;
	theApp.MAJMenuEtat();
	}

	return 0;
}

int CProcess::GererEtat_ValidationFinLot(int iCommande,int iCmdAuto)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		case CMD_VALIDER:

			// Verifier que les pesons hors tolerance soient inhibes
			if(!theApp.m_Recette.IsFinLot_Forced)
			{
				for(int i = 0; i < 6; i++)
				{
					if( ((theApp.Pesons[i].StatutEtalon1 == PESON_STATUTETALON_NOK)||
					(theApp.Pesons[i].StatutEtalon2 == PESON_STATUTETALON_NOK))
					&& (theApp.Pesons[i].PesonMode != PESON_MODEOFF))
					{
						CString strMess;
						strMess.Format(theApp.TableMessage.GetStr(MESSAGE_VALID_PESON_NOK),theApp.Pesons[i].iNumPesonBL);
						//Message
						theApp.SetEventsJournalStr(strMess);
						theApp.SetInfosString(strMess);

						//Refus de la validation
						iCommande = 0;
						theApp.MAJMenuEtat();
						return 0;
					}
				}
			}

			//Sauvegarde des poids etalon
			theApp.m_Recette.SavePoidsEtalonFin();

			theApp.m_Recette.iStatut = RECETTE_STATUT_TERMINEE;
			theApp.m_Recette.Update();
			theApp.m_Recette.UpdateLot();
			theApp.m_Recette.EndRecetteEnCours();//GCA V1.17
			theApp.m_Recette.SetUtilisateurFin();//V2.50
			Gerer_ImpressionAutoFinOF(theApp.m_Recette.OfNum);
			((CDialValidLot*)theApp.ValidLotView)->StopLecture();

			//Si on a fait quitter dans la fenêtre chargement d'OF
			//on peut passer en hors production
			if(theApp.m_RecetteNew.strLot == _T(""))
			{
				//GCA09.01 Reset recette 
				theApp.m_Recette.Reset();
				theApp.m_RecetteNew.Reset();
				
				theApp.DialMachineEnCours = theApp.MachineView;
				//MAJ fenetre en focntion de la recette en cours
				((CDialMachine * )theApp.MachineView)->InitialiserTextes();
				//Changement Etat bouton DialMachine
				((CDialMachine * )theApp.MachineView)->SetStateButProcess(0);

				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				iEtat = ETAT_PROCESS_HORSPRODUCTION;

				//Ecriture OPC Etat Machine
				EtatMachine = ETATMACHINE_HORSCYCLE;
				*pBitVidage = FALSE;
				*pBitVidage2 = FALSE;		//V2.42
				*pBitMaintenance = FALSE;
				*pBitAuto = FALSE;
				*theApp.m_Recette.pBitRecetteChargee = FALSE;
				*pBitEclairageSup = FALSE;
				*pBitEclairageUV = FALSE;
			}
			else//Si on a chargé un nouvel OF on reprend 
			{
				theApp.DialMachineEnCours = theApp.InitialisationView;
				((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
				iEtat = ETAT_PROCESS_INITIALISATIONMACHINE;
				iCmdAuto = 1;

				//Ecriture OPC Etat Machine
				EtatMachine = ETATMACHINE_INITIALISATION;
			}
			break;

		default:
			break;
		}

	iCommande = 0;
	theApp.MAJMenuEtat();
	}
	return 0;
}

int CProcess::GererEtat_Initialisation(int iCommande)
{
	switch(iCommande)
	{
	case CMD_VALIDER:
		
		//Verifications 
		if(theConfig.str_Machine.Right(5) != "SIMUL")
		{
			if(!Autoriser_ValidInit())
			{
				//Refus de la validation
				iCommande = 0;
				theApp.MAJMenuEtat();
				return 0;
			}
		}

		//Chargement Recette Vision
		if(!m_RecetteVision.Charger(theApp.m_Recette.ArtMinims.iParam1))//theApp.m_Recette.Article.Type
		{
			//Message Erreur
			CString strMess;
			strMess.Format(theApp.TableMessage.GetStr(MESSAGE_RECETTEVISION_NOK),m_RecetteVision.strErreur);
			//Message
			theApp.SetEventsJournalStr(strMess);
			theApp.SetInfosString(strMess);

			//Refus de la validation
			iCommande = 0;
			theApp.MAJMenuEtat();
			return 0;
		}
		//GCA V1.17 Passage sans procédure de validation
		if(theApp.m_Recette.bIsEnCours)
		{
			theApp.m_Recette.iStatut = RECETTE_STATUT_DEBUTOF;
			theApp.m_Recette.Update();

			theApp.DialMachineEnCours = theApp.ValidationView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
			iEtat = ETAT_PROCESS_VALIDATIONDEBUTOF;
			break;
		}

		//Poursuite du cycle
		if((theApp.m_Recette.strLot!= theApp.m_RecetteOld.strLot)/*&&(theApp.m_PrevRecette.strLot != _T(""))*/)//A VOIR
		{
			//Puis Validation du lot en cours
			theApp.m_Recette.iStatut = RECETTE_STATUT_DEBUTLOT;
			theApp.m_Recette.Update();

			((CDialValidLot*)theApp.ValidLotView)->Recette = &theApp.m_Recette;
			((CDialValidLot*)theApp.ValidLotView)->bAllPesons = true;
			((CDialValidLot*)theApp.ValidLotView)->bDebutLot = true;

			theApp.DialMachineEnCours = theApp.ValidLotView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
			((CDialValidLot*)theApp.ValidLotView)->StartLecture();

			iEtat = ETAT_PROCESS_VALIDATIONDEBUTLOT;
		}
		else if(theApp.m_Recette.strLot == theApp.m_RecetteOld.strLot)
		{
			theApp.m_Recette.iStatut = RECETTE_STATUT_DEBUTOF;
			theApp.m_Recette.Update();

			theApp.DialMachineEnCours = theApp.ValidationView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
			iEtat = ETAT_PROCESS_VALIDATIONDEBUTOF;
		}
		break;

	case CMD_ANNULER:
		theApp.m_Recette.EndRecetteEnCours();//V2.50
		theApp.m_Recette.Reset();
		theApp.m_RecetteNew.Reset();
			
		theApp.DialMachineEnCours = theApp.MachineView;
		//MAJ fenetre en focntion de la recette en cours
		((CDialMachine * )theApp.MachineView)->InitialiserTextes();
		//Changement Etat bouton DialMachine
		((CDialMachine * )theApp.MachineView)->SetStateButProcess(0);

		((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
		iEtat = ETAT_PROCESS_HORSPRODUCTION;

		*pBitEclairageSup = FALSE;
		*pBitEclairageUV = FALSE;

		break;

	default:
		break;
	}
	return 0;
}

int CProcess::GererEtat_ValidationDebutLot(int iCommande,int iCmdAuto)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		case CMD_VALIDER:

			//Verifier que les pesons hors tolerance soient inhibes
			if(theConfig.str_Machine.Right(5) != "SIMUL")
			{
				for(int i = 0; i < 6; i++)
				{
					if( ((theApp.Pesons[i].StatutEtalon1 == PESON_STATUTETALON_NOK)||
					(theApp.Pesons[i].StatutEtalon2 == PESON_STATUTETALON_NOK))
					&& (theApp.Pesons[i].PesonMode != PESON_MODEOFF))
					{
						CString strMess;
						strMess.Format(theApp.TableMessage.GetStr(MESSAGE_VALID_PESON_NOK),theApp.Pesons[i].iNumPesonBL);
						//Message
						theApp.SetEventsJournalStr(strMess);
						theApp.SetInfosString(strMess);

						//Refus de la validation
						iCommande = 0;
						theApp.MAJMenuEtat();
						return 0;
					}
				}
			}

			//Sauvegarde des poids etalon
			theApp.m_Recette.SavePoidsEtalon();
			
			((CDialValidLot*)theApp.ValidLotView)->StopLecture();

			theApp.DialMachineEnCours = theApp.ValidationView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();

			iEtat = ETAT_PROCESS_VALIDATIONDEBUTOF;
			break;

		case CMD_ANNULER://A VOIR
			*theApp.m_Recette.pBitRecetteChargee = FALSE;

			theApp.m_Recette.EndRecetteEnCours();//V2.50
			theApp.m_Recette.Reset();
			theApp.m_RecetteNew.Reset();
				
			theApp.DialMachineEnCours = theApp.MachineView;
			//MAJ fenetre en focntion de la recette en cours
			((CDialMachine * )theApp.MachineView)->InitialiserTextes();
			//Changement Etat bouton DialMachine
			((CDialMachine * )theApp.MachineView)->SetStateButProcess(0);

			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
			iEtat = ETAT_PROCESS_HORSPRODUCTION;

			*pBitEclairageSup = FALSE;
			*pBitEclairageUV = FALSE;

		default:
			break;
		}

		iCommande = 0;
		theApp.MAJMenuEtat();
	}
	
	return 0;
}

int CProcess::GererEtat_ValidationDebutOf(int iCommande,int iCmdAuto)
{
    //Ecriture OPC Etat Machine
    EtatMachine = ETATMACHINE_CYCLEAUTO;
    *pBitVidage = FALSE;
    *pBitVidage2 = FALSE;		//V2.42
    *pBitMaintenance = FALSE;
    *pBitAuto = TRUE;
    *theApp.m_Recette.pBitRecetteChargee = TRUE;
			
    theApp.DialMachineEnCours = theApp.MachineView;
    //MAJ fenetre en focntion de la recette en cours
    ((CDialMachine * )theApp.MachineView)->InitialiserTextes();

    ((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();

    //A VOIR
    theApp.m_Recette.iStatut = RECETTE_STATUT_PRODUCTION;
    theApp.m_Recette.Update();

    //Changement Etat bouton DialMachine
    ((CDialMachine * )theApp.MachineView)->SetStateButProcess(1);
			
    //Validation du début d'OF
    iEtat = ETAT_PROCESS_PRODUCTIONARRET;

    iCommande = 0;
    theApp.MAJMenuEtat();

    iCmdAuto = 1;//refresh etat suivant

	//V 45.0
	Cadence_Reset();
	Cadence_AcquitAlarmes();

    return 0;
}

int CProcess::GererEtat_ProductionArret(int iCommande,int iCmdAuto)
{

	int resultBox = 0;
	CDialFinOF frameFinOf;
	CString sCommande;

	if(iCommande > 0)
	{
		
		CString strMess = _T("");

		switch(iCommande)
		{

		case CMD_ANNULER:
			if(theConfig.str_Machine.Right(5) != "SIMUL")
			{
				if(*pBitMachineVide == FALSE)
				{
					strMess.Format(theApp.TableMessage.GetStr(MESSAGE_ASK_MACHINENONVIDE));
					//Message
					if(AfxMessageBox(strMess,MB_YESNO,MB_ICONQUESTION)== IDYES)
					{
						CString sUserMachNonVide;
						CString sdateMachNonVide;

						//Demande login Manager
						if(theApp.DemandeLoginManager(sUserMachNonVide,sdateMachNonVide))
						{
							//Traca forcage machine non vide
							CString strCom;
							strCom.Format(theApp.TableMessage.GetStr(MESSAGE_JOURNAL_MACHINENONVIDE));
							theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE,strCom,sdateMachNonVide,sUserMachNonVide);
							//on passe a la suite
						}
						else
						{
							//Refus suite Erreur Login ou Not Manager
							iCommande = 0;
							theApp.MAJMenuEtat();
							return -1;
						}
					}
					else
					{
						//Refus suite
						iCommande = 0;
						theApp.MAJMenuEtat();
						return -1;
					}
				}
			}

			//V21 : Controle de vidage
			//V29 : Contrôle de vidage sauf si ShutDown
			//if(*pBitAutorisationDebutOF == FALSE)
			//{
			//	theApp.SetInfosString("VIDAGE NON EFFECTUE");
			//	//refus de la validation
			//	iCommande = 0;
			//	theApp.MAJMenuEtat();
			//	return 0;
			//}

			//V2.50 : DialogBox de fin d'OF
			strMess.Format(theApp.TableMessage.GetStr(MESSAGE_ASK_ARRETCYCLE));
			frameFinOf.DoModal();
			resultBox = frameFinOf.ResultModal;

			if( resultBox == IDYES)//Solder OF
			{
				//V29 : Vidage en cas de solde OF
				if(*pBitAutorisationDebutOF == FALSE)
				{
					theApp.SetInfosString("VIDAGE NON EFFECTUE");
					//refus de la validation
					iCommande = 0;
					theApp.MAJMenuEtat();
					return 0;
				}

				///V2.50
				//Demande Login (non manager) pour utilisateur Fin OF
				CString strDate;
				if(theApp.DemandeLogin(theApp.m_Recette.UserFin,strDate))
				{
					iEtat = ETAT_PROCESS_VALIDATIONFINOF;
				}
			}
			else if( resultBox == IDNO)//Arret Systéme
			{
				Gerer_Shutdown();
			}

			break;

		case CMD_DEPARTCYCLE:
			//Si Maintenance on interdit le depart cycle
			if(theApp.CurrentUser.bMaintenance)
			{
				strMess.Format(theApp.TableMessage.GetStr(MESSAGE_MAINTENANCE_NOK));
				//Message
				theApp.SetEventsJournalStr(strMess);
				theApp.SetInfosString(strMess);

				//Refus depart cycle
				iCommande = 0;
				theApp.MAJMenuEtat();
				return 0;
			}
			
			//Si l'etalonnage Etui n'a pas été fait on interdit le depart cycle
			if((*theApp.PesonEtui.pBitFinEtalonnageOK == FALSE)&&(*theApp.PesonEtui.pBitInhibition == FALSE))
			{
				strMess.Format(theApp.TableMessage.GetStr(MESSAGE_ETALONNAGE_ETUI_NOK));
				//Message
				theApp.SetEventsJournalStr(strMess);
				theApp.SetInfosString(strMess);

				//Refus depart cycle
				iCommande = 0;
				theApp.MAJMenuEtat();
				return 0;
			}

			//V 45.0
			//Acquittement des alarmes cadence si une active
			if ((theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT1].NewValue > 0) ||
				(theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT2].NewValue> 0) ||
				(theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT3].NewValue > 0) ||
				(theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_Machine].NewValue > 0))
			{
				Cadence_AcquitAlarmes();
			}

			//V20 : Mise à jour du gain pour la caméra tracking
			sCommande.Format("ssg %ld",m_RecetteVision.pLong_CamGain->value);
			theApp.DalsaCamera1.Ecrire_commande(sCommande);

			theApp.DialMachineEnCours = theApp.MachineView;
			//MAJ fenetre en focntion de la recette en cours
			((CDialMachine * )theApp.MachineView)->InitialiserTextes();

			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();

			iEtat = ETAT_PROCESS_PRODUCTIONDEMARRAGE;
			//A VOIR
			theApp.m_Recette.Update();

			EtatMachine = ETATMACHINE_CYCLEAUTO;
			*pBitVidage = FALSE;
			*pBitVidage2 = FALSE;		//V2.42
			*pBitMaintenance = FALSE;
			*pBitAuto = TRUE;

			//Ecriture OPC Etat Machine
			WBit0[INDEX_BIT_STARTCYCLE_W] = TRUE;

			break;

		default:
			break;
		}

		iCommande = 0;
		theApp.MAJMenuEtat();
	}
	else if(iCmdAuto > 0)
	{
		
		theApp.m_Recette.Update();

		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}

	return 0;
}

int CProcess::GererEtat_Demarrage(int iCommande,int iCmdAuto)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		case CMD_ARRETCYCLE:
			//A VOIR si on fait un Stop cycle ici
			iEtat = ETAT_PROCESS_PRODUCTIONARRET;
			iCmdAuto = 1;
			break;

		default:
			break;
		}
	}
	else if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}
	
	return 0;
}

int CProcess::GererEtat_ProductionMarche(int iCommande,int iCmdAuto)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		case CMD_ARRETCYCLE:

			//Ecriture OPC Etat Machine
			WBit0[INDEX_BIT_STOPCYCLE_W] = TRUE;
			break;

		default:
			break;
		}

		iCommande = 0;
		theApp.MAJMenuEtat();
	}
	else if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}

	Cadence_Gerer();

	return 0;
}

int CProcess::GererEtat_ValidationFinOf(int iCommande,int iCmdAuto)
{
	theApp.MAJMenuEtat();
	Sleep(50);
	
	//TODO Validation Fin OF
	theApp.m_Recette.iStatut = RECETTE_STATUT_FINOF;
	theApp.m_Recette.Update();

	if(theApp.CurrentUser.IdBase > 0)
	{
		 theApp.theJournal.Clore(theApp.CurrentUser.IdBase);
		 theApp.CurrentUser.IdBase =  theApp.theJournal.Ajouter(JOURNAL_TYPELOGIN,theApp.CurrentUser.strLibelle,"Login");
	}

	theApp.BackupBase();//Sauvegarde compléte de la BDD
	

	// Reset RecetteNew
	theApp.m_RecetteNew.Reset();

	((CDialChargement * )theApp.ChargementView)->Recette = &theApp.m_RecetteNew;
	((CDialChargement * )theApp.ChargementView)->m_ListEvents.ResetContent();
	((CDialChargement * )theApp.ChargementView)->MAJRecetteWindows();

	theApp.DialMachineEnCours = theApp.ChargementView;
	((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();

	//Verification Fichier CSV
	((CDialChargement * )theApp.ChargementView)->CSV_Charger();

	iEtat = ETAT_PROCESS_CHARGEMENTOF;

	*theApp.m_Recette.pBitRazEtalons = TRUE;

	theApp.MAJMenuEtat();
	return 0;
}

int CProcess::GererEtat_Mixup(int iCommande,int iCmdAuto )
{
	if(iCmdAuto > 0)
	{
		theApp.DialMachineEnCours = theApp.DialMixup;
		((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();

		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}
	return 0;
}

int CProcess::GererEtat_VerifDynamique(int iCommande,int iCmdAuto)
{
	return 0;
}

int CProcess::GererEtat_InitApp()
{
	//Initialisation de l'application
	
	//Recuperation handle fenêtre affichage Infos
	CWnd* wnd_main=0;
	CFormBase* wnd=0;

	wnd_main = AfxGetApp()->GetMainWnd();

	if(wnd_main->IsKindOf(RUNTIME_CLASS(CSplitFrame)))
	{	
		wnd=((CFormBase*)((CSplitFrame*)wnd_main)->m_wndSplitter.GetPane(1,0));	
	}

	//Initialisation Registre
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Initialisation de la configuration registre");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	if(!theConfig.Lire())
	{
		AfxMessageBox("Impossible de charger la configuration");
		return FALSE;
	}
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Initialisation de la Base De Données");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	//Ouverture de la base de données
	//V18

	//V 40 Log ADO
	strADOLogFile = _T("");
	//Date
	CString strDate = _T("");
	CTime time;
	tm t1;
	int iJour, iMois, iAnnee;
	int iHeure, iMinute, iSeconde;

	//recupération de la date et l'heure 
	time = CTime::GetCurrentTime();
	t1 = *(time.GetLocalTm(&t1));

	iJour = t1.tm_mday;
	iMois = (t1.tm_mon + 1);//+1-> janvier =0
	iAnnee = (t1.tm_year + 1900);

	iHeure = t1.tm_hour;
	iMinute = t1.tm_min;
	iSeconde = t1.tm_sec;

	//Format: RepertoireData\Logs\20121124_182536.Acylog
	strDate.Format(_T("%04d%02d%02d_%02d%02d%02d"), iAnnee, iMois, iJour, iHeure, iMinute, iSeconde);
	strADOLogFile.Format(_T("%s\\Logs\\%s.Acylog"),theConfig.str_RepertoireData, strDate);

	CString strDirLogs = theConfig.str_RepertoireData + _T("\\Logs\\");
	if (!PathFileExists(strDirLogs))
	{
		CreateDirectory(strDirLogs, NULL);
	}

	if(!theBase.Ouvrir(theConfig.str_RepertoireBase,theConfig.str_NomBase,theConfig.str_Provider))
	{
		((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Echec ouverture base de données... Attente 5 secondes");
		((CFormBase*)wnd)->UpdateData(FALSE);
		((CFormBase*)wnd)->UpdateWindow();
		Sleep(5000);
		return FALSE;
	}
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Initialisation des jeux de paramètres");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	//V18
	theApp.InitJeuParam();
	theApp.InitJeuParamVision();

	//Config Cookie DMX 1
	//V18
	//CString URL = "http://" + ParamStrIPDMX1->value;
	//theApp.session->SetCookie(URL, "ProcessModeConfirmDialogDisabled", "true");
	//theApp.session->SetCookie(URL, "ServiceModeConfirmDialogDisabled", "true");

	//theApp.session->SetCookie(URL, "http-session-id", "0");
	//theApp.session->SetCookie(URL, "acceptCookies", "1"); 
	//theApp.session->SetCookie(URL, "languageId", "FR"); 

	////Config Cookie DMX 2
	//URL = "http://" + ParamStrIPDMX2->value;
	//theApp.session->SetCookie(URL, "ProcessModeConfirmDialogDisabled", "true");
	//theApp.session->SetCookie(URL, "ServiceModeConfirmDialogDisabled", "true");

	//theApp.session->SetCookie(URL, "http-session-id", "0");
	//theApp.session->SetCookie(URL, "acceptCookies", "1"); 
	//theApp.session->SetCookie(URL, "languageId", "FR");  

	////Config Cookie DMX 3
	//URL = "http://" + ParamStrIPDMX3->value;
	//theApp.session->SetCookie(URL, "ProcessModeConfirmDialogDisabled", "true");
	//theApp.session->SetCookie(URL, "ServiceModeConfirmDialogDisabled", "true");

	//theApp.session->SetCookie(URL, "http-session-id", "0");
	//theApp.session->SetCookie(URL, "acceptCookies", "1"); 
	//theApp.session->SetCookie(URL, "languageId", "FR"); 
	

	//RAZ des recettes
	theApp.m_Recette.Reset();
	theApp.m_RecetteNew.Reset();
	theApp.m_RecetteOld.Reset();

	theApp.theJournal.pBase = &theBase.dao_Base;
		
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Initialisation de l'utilisateur");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
		
	//Initialisation Utilisateur par défaut
	theApp.CurrentUser.Lire_Base(CODE_UTILISATEUR_DEFAUT);

	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Initialisation des Fenetres/Menus");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	//Initialisation des fenetres et Menus
	theApp.InitFenetres();


	//#3 DMX
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, _T("Initialisation communication lecteurs Leuze"));
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	//V2 pointeurs sur les fonctions (evite les sendmessages)
	//ServeurSocket.HwndParent = theApp.DMXView->GetSafeHwnd();
	ServeurSocket.pExecuteOnConnect = &ExecuteOnConnect;
	theApp.m_DMXEtui.ClientSocket.pExecuteOnValueReceive    = &ExecuteOnreceive_DMXEtui;
	theApp.m_DMXEtui.ClientSocket.pExecuteOnDeconnect		= &ExecuteOnDeconnexion_DMXEtui;

	theApp.m_DMXNotice1.ClientSocket.pExecuteOnValueReceive = &ExecuteOnreceive_DMXNotice1;
	theApp.m_DMXNotice1.ClientSocket.pExecuteOnDeconnect = &ExecuteOnDeconnexion_DMXNotice1;

	theApp.m_DMXNotice2.ClientSocket.pExecuteOnValueReceive = &ExecuteOnreceive_DMXNotice2;
	theApp.m_DMXNotice2.ClientSocket.pExecuteOnDeconnect = &ExecuteOnDeconnexion_DMXNotice2;

	//SockV3
	theApp.m_DMXNotice2.ClientSocket.pExecuteOnArretThread = &ExecuteOnArret_DMXNotice2;
	theApp.m_DMXEtui.ClientSocket.pExecuteOnArretThread = &ExecuteOnArret_DMXEtui;
	theApp.m_DMXNotice1.ClientSocket.pExecuteOnArretThread = &ExecuteOnArret_DMXNotice1;

	ServeurSocket.StartServeur((int)pParamPortEcouteLecteurDMX->value);

	if (ServeurSocket.str_Erreur != _T(""))
	{
		((CFormBase*)wnd)->ListeInitialisation.InsertString(0, ServeurSocket.str_Erreur);
	}

	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	//CWnd* WinDisp;
	//HWND WDisp;

	//WinDisp = theApp.VisionView->GetDlgItem(IDC_IMAGE);
	//WDisp = WinDisp->m_hWnd;
	
	theApp.DialMachineEnCours = theApp.MachineView;

	//HANDLE sur la fenetre principale
	MainHandle = theApp.MachineView->GetSafeHwnd();
	MainDiag = theApp.MachineView;

	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Initialisation des Alarmes");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	//Chargement des Alarmes from BDD
	if(theApp.m_GestAlarmes.ChargerFromBase() == -1)
	{
		AfxMessageBox(theApp.m_GestAlarmes.strErreur,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	//V 44.0
	Get_MaxAdresses_DB30X();

	//Initialisation String Table (Messages)
	if(theApp.TableMessage.ChargerFromBase() == -1)
	{
		AfxMessageBox(theApp.TableMessage.strErreur,MB_OK|MB_ICONSTOP);
		return FALSE;
	}
		
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Initialisation de la communication Automate");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	//Init Robots
	theApp.m_Robot1.iNumRobot = 1;
	theApp.m_Robot2.iNumRobot = 2;
	theApp.m_Robot3.iNumRobot = 3;

	//Initialisation de l'Etat Machine
	EtatMachine = ETATMACHINE_HORSCYCLE;

	//Initialisation du client OPC
	//A VOIR Alarme OPC = Numero Erreur
		
	//Pointeur Client/groupe
	theApp.m_Robot1.peson[0] = &theApp.Pesons[0];
	theApp.m_Robot1.peson[1] = &theApp.Pesons[1];
	theApp.m_Robot2.peson[0] = &theApp.Pesons[2];
	theApp.m_Robot2.peson[1] = &theApp.Pesons[3];
	theApp.m_Robot3.peson[0] = &theApp.Pesons[4];
	theApp.m_Robot3.peson[1] = &theApp.Pesons[5];

	theApp.m_Recette.m_OPCClient = &theApp.m_Groupes[GROUPE_RECETTE];
	theApp.m_Robot1.m_OPCClient_TW = &theApp.m_Groupes[GROUPE_ROBOT1_TW];
	theApp.m_Robot1.m_OPCClient_TR = &theApp.m_Groupes[GROUPE_ROBOT1_TR];

	theApp.m_Robot2.m_OPCClient_TW = &theApp.m_Groupes[GROUPE_ROBOT2_TW];
	theApp.m_Robot2.m_OPCClient_TR = &theApp.m_Groupes[GROUPE_ROBOT2_TR];

	theApp.m_Robot3.m_OPCClient_TW = &theApp.m_Groupes[GROUPE_ROBOT3_TW];
	theApp.m_Robot3.m_OPCClient_TR = &theApp.m_Groupes[GROUPE_ROBOT3_TR];

	m_OPCClient_R = &theApp.m_Groupes[GROUPE_PROCESS_R];
	m_OPCClient_W = &theApp.m_Groupes[GROUPE_PROCESS_W];

	theApp.Compteurs.m_OPCClient_R = &theApp.m_Groupes[GROUPE_COMPTEURS];//GCA 1412

	m_OPCClient_Ai      = &theApp.m_GroupeDefauts[GROUPE_DEFAUTS_AIMMEDIAT];
	m_OPCClient_AFCy    = &theApp.m_GroupeDefauts[GROUPE_DEFAUTS_AFCYCLE];
	m_OPCClient_Alertes = &theApp.m_GroupeDefauts[GROUPE_DEFAUTS_ALERTES];

	//Pointeurs Variables
	pBitMaintenance =  &WBit0[INDEX_BIT_MAINTENANCE_W];
	pBitVidage =  &WBit0[INDEX_BIT_VIDAGE_W];
	pBitVidage2 =  &WBit0[INDEX_BIT_VIDAGE2_W];	//V2.42
	pBitAuto =  &WBit0[INDEX_BIT_AUTO_W];
	pBitAcquitDefaut =  &WBit0[5];
	pBitMachineVide =  &RBit3[5];
	
	if (theConfig.str_Machine.Find(_T("#2")) >= 0)
	{
		pBitAutorisationDebutOF =  &RBit4[2];	//V19
		pBitEclairageSup = &WBit9[0];
		pBitEclairageLat = &WBit9[1];
		pBitEclairageUV = &WBit9[2];
	}
	else
	{
		pBitAutorisationDebutOF =  &ROctetForcage[0];	//V30-ALL
		pBitEclairageSup = &WOctetFictif[0];
		pBitEclairageLat = &WOctetFictif[1];
		pBitEclairageUV = &WOctetFictif[2];
	}

	pBitDeVie =  &WBit0[6];
	
	theApp.m_Recette.pBitRecetteChargee = &theApp.m_Recette.WBit2[RECETTE_INDEXBIT_RECETTECHARGEE];
	theApp.m_Recette.pBitRazEtalons = &theApp.m_Recette.WBit1[RECETTE_INDEXBIT_RAZETALONS];
	pBitEtalonnageEtui =  &WBit1[INDEX_BIT_MODES_CYCLE_ETALON_PESEE_W];

	theApp.m_DMXEtui.pBitInhibition = &WBit3[1];
	theApp.m_DMXNotice1.pBitInhibition = &WBit3[2];
	theApp.m_DMXNotice2.pBitInhibition = &WBit3[3];

	theApp.m_DMXEtui.pBitEtatConnexion = &WBit7[0];//DB 101.10[0]
	theApp.m_DMXNotice1.pBitEtatConnexion = &WBit7[1];//DB 101.10[1]
	theApp.m_DMXNotice2.pBitEtatConnexion = &WBit7[2];//DB 101.10[2]
	theApp.m_DMXEtui.iValResult = &iValResult_DmxEtui;
	theApp.m_DMXNotice1.iValResult = &iValResult_DmxNotice1;
	theApp.m_DMXNotice2.iValResult = &iValResult_DmxNotice2;
	//TODO : mettre les vrais valeurs de registres OPC et les lier/declarer/lire
	theApp.m_DMXEtui.pBitLecture			= &RBit4[3];//DB 110 X5.3
	theApp.m_DMXNotice1.pBitLecture			= &RBit4[3];
	theApp.m_DMXNotice2.pBitLecture			= &RBit4[3];


	theApp.Pesons[0].pBitInhibition = &WBit2[3];
	theApp.Pesons[1].pBitInhibition = &WBit2[4];
	theApp.Pesons[2].pBitInhibition = &WBit2[5];
	theApp.Pesons[3].pBitInhibition = &WBit2[6];
	theApp.Pesons[4].pBitInhibition = &WBit2[7];
	theApp.Pesons[5].pBitInhibition = &WBit3[0];
	
	theApp.PesonEtui.pBitInhibition = &WBit3[5];

	theApp.Pesons[0].pBitAcquittement = &theApp.m_Robot1.WBit3[0];
	theApp.Pesons[1].pBitAcquittement = &theApp.m_Robot1.WBit3[1];
	theApp.Pesons[2].pBitAcquittement = &theApp.m_Robot2.WBit3[0];
	theApp.Pesons[3].pBitAcquittement = &theApp.m_Robot2.WBit3[1];
	theApp.Pesons[4].pBitAcquittement = &theApp.m_Robot3.WBit3[0];
	theApp.Pesons[5].pBitAcquittement = &theApp.m_Robot3.WBit3[1];

	theApp.PesonEtui.pBitAcquittement = &theApp.PesonEtui.UndefBits[0];

	theApp.Pesons[0].pBitEtalon1OK = &WBit5[0];
	theApp.Pesons[0].pBitEtalon2OK = &WBit5[1];

	theApp.Pesons[1].pBitEtalon1OK = &WBit5[2];
	theApp.Pesons[1].pBitEtalon2OK = &WBit5[3];

	theApp.Pesons[2].pBitEtalon1OK = &WBit5[4];
	theApp.Pesons[2].pBitEtalon2OK = &WBit5[5];

	theApp.Pesons[3].pBitEtalon1OK = &WBit5[6];
	theApp.Pesons[3].pBitEtalon2OK = &WBit5[7];

	theApp.Pesons[4].pBitEtalon1OK = &WBit6[0];
	theApp.Pesons[4].pBitEtalon2OK = &WBit6[1];

	theApp.Pesons[5].pBitEtalon1OK = &WBit6[2];
	theApp.Pesons[5].pBitEtalon2OK = &WBit6[3];

	theApp.PesonEtui.pBitEtalon1OK = &theApp.PesonEtui.UndefBits[1];
	theApp.PesonEtui.pBitEtalon2OK = &theApp.PesonEtui.UndefBits[2];

	theApp.Pesons[0].pBitDoTare = &WBit4[0];
	theApp.Pesons[1].pBitDoTare = &WBit4[1];
	theApp.Pesons[2].pBitDoTare = &WBit4[2];
	theApp.Pesons[3].pBitDoTare = &WBit4[3];
	theApp.Pesons[4].pBitDoTare = &WBit4[4];
	theApp.Pesons[5].pBitDoTare = &WBit4[5];

	theApp.PesonEtui.pBitDoTare = &WBit4[6];

	theApp.Pesons[0].pBitVerifDyna = &WBit45[0];
	theApp.Pesons[1].pBitVerifDyna = &WBit45[1];
	theApp.Pesons[2].pBitVerifDyna = &WBit45[2];
	theApp.Pesons[3].pBitVerifDyna = &WBit45[3];
	theApp.Pesons[4].pBitVerifDyna = &WBit45[4];
	theApp.Pesons[5].pBitVerifDyna = &WBit45[5];
	
	theApp.PesonEtui.pBitVerifDyna = &theApp.PesonEtui.UndefBits[3];

	theApp.m_Robot1.pBitInhibition = &WBit2[0];
	theApp.m_Robot2.pBitInhibition = &WBit2[1];
	theApp.m_Robot3.pBitInhibition = &WBit2[2];

	theApp.m_Robot1.pBitParking = &WBit8[0];
	theApp.m_Robot2.pBitParking = &WBit8[1];
	theApp.m_Robot3.pBitParking = &WBit8[2];

	theApp.PesonEtui.pBitFinEtalonnageOK = &RBit4[0];
	//V37
	//V 44.0 theApp.VisionBoite.pBitInhibitionLectureBoite = &WBit3[6];
	theApp.EtiqScellement.pBitInhibLecture = &WBit3[6];
	theApp.EtiqWolke.pBitInhibLecture = &BitUndef[0];

	theApp.VisionBoite.pBitResAcquisition_OK = &WBit1[INDEX_BIT_ETIQ_RESULT_OK_W];
	theApp.VisionBoite.pBitResAcquisition_NOK = &WBit1[INDEX_BIT_ETIQ_RESULT_NOK_W];
	theApp.VisionBoite.pBitMixup = &WBit1[INDEX_BIT_ETIQ_MIXUP_W];
	theApp.VisionBoite.pBitSnap = &RBit4[1];//DB 110 X5.1

	//V37
	theApp.EtiqScellement.pBitInhibImpression = &WBit3[4];
	theApp.EtiqWolke.pBitInhibImpression = &WBit3[7];

	//Groupe Poids Etui Read
	theApp.m_Groupes[GROUPE_PESAGE_R].strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Groupes[GROUPE_PESAGE_R].Area = daveDB;
	theApp.m_Groupes[GROUPE_PESAGE_R].iTypeGroupe = GROUPE_READ;
	theApp.m_Groupes[GROUPE_PESAGE_R].NumDataBlock = 211;
	theApp.m_Groupes[GROUPE_PESAGE_R].IndexDataBlock = 0;
	theApp.m_Groupes[GROUPE_PESAGE_R].AlarmeVal = 4; //V38

	//AJOUT DES ITEMS
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//00
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//01
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//02
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//03
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//04
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//05
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//06
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//07
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//08
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//09
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//10
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//11

	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(&theApp.PesonEtui.iVal211_12,VT_I1,_T("DB221.12 Last Poids Peson Etui High"));//12 
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(&theApp.PesonEtui.iVal211_13,VT_I1,_T("DB221.13 Last Poids Peson Etui Low"));//13
	
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//14
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//15
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//16
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//17
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//18
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//19
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//20
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//21
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//22
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//23
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//24
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(NULL,VT_EMPTY,_T(""));//25

	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(&theApp.PesonEtui.iVal211_26,VT_I1,_T("DB221.26 Stab. Poids Peson Etui High"));//26
	theApp.m_Groupes[GROUPE_PESAGE_R].Lier(&theApp.PesonEtui.iVal211_27,VT_I1,_T("DB221.27 Stab. Poids Peson Etui Low"));//27

	theApp.m_Groupes[GROUPE_PESAGE_R].GererCycle = &theApp.PesonEtui.GererCycle;

	theApp.m_Groupes[GROUPE_PESAGE_R].InitialiseServeur();
	theApp.m_Groupes[GROUPE_PESAGE_R].InitialiseGroupe();
	theApp.m_Groupes[GROUPE_PESAGE_R].InitialiseItems();


//GROUPE RECETTE
	//Initialisation Groupes
	theApp.m_Recette.m_OPCClient->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Recette.m_OPCClient->Area = daveDB;
	theApp.m_Recette.m_OPCClient->iTypeGroupe = GROUPE_WRITE;
	theApp.m_Recette.m_OPCClient->NumDataBlock = 100;
	theApp.m_Recette.m_OPCClient->IndexDataBlock = 0;
	theApp.m_Recette.m_OPCClient->GererCycle = &theApp.m_Recette.GererCycle;
	theApp.m_Recette.m_OPCClient->AlarmeVal = 8;

	//AJOUT DES ITEMS
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.PoidsNominal,VT_R4,_T("DB100.0 Poids Nominal"));
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.PoidsMini,VT_R4,_T("DB100.4 Poids Mini"));
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.PoidsMaxi,VT_R4,_T("DB100.8 Poids Maxi"));
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.ArtMinims.iParam1,VT_I2,_T("DB100.12 Type"));//theApp.m_Recette.Article.Type
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.QuantiteParBoite,VT_I2,_T("DB100. 14 QuantiteparBoite"));//14
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.WBit1,VT_ACYBOOL,_T("DB100.16 Bits 1"));//16
	theApp.m_Recette.m_OPCClient->Lier(NULL,VT_EMPTY,_T(""));//17
	theApp.m_Recette.m_OPCClient->Lier(NULL,VT_EMPTY,_T(""));//18
	theApp.m_Recette.m_OPCClient->Lier(NULL,VT_EMPTY,_T(""));//19
	theApp.m_Recette.m_OPCClient->Lier(NULL,VT_EMPTY,_T(""));//20
	theApp.m_Recette.m_OPCClient->Lier(NULL,VT_EMPTY,_T(""));//21
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.ArtEtiquette.iParam1,VT_I2,_T("DB100.22 Format etiquette"));//22//GCA V1.17
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.ArtNotice1.iParam1,VT_I2,_T("DB100.24 Format notice1"));//24
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.ArtNotice2.iParam1,VT_I2,_T("DB100.26 Format notice2"));//26
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.WBit2,VT_ACYBOOL,_T("DB100.28 Bits 2"));//28
	theApp.m_Recette.m_OPCClient->Lier(NULL,VT_EMPTY,_T(""));//29
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.PoidsNominal_mg,VT_I2,_T("DB100.30 Poids Nominal mg"));//30
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.PoidsMini_mg,VT_I2,_T("DB100.32 Poids Mini mg"));//32
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.PoidsMaxi_mg,VT_I2,_T("DB100.34 Poids Maxi mg"));//34
	theApp.m_Recette.m_OPCClient->Lier(&theApp.m_Recette.TolEtuiPlein_cg,VT_I2,_T("DB100.36 Tol. Etui Plein (cg)"));//36

		
	theApp.m_Recette.m_OPCClient->InitialiseServeur();
	theApp.m_Recette.m_OPCClient->InitialiseGroupe();
	theApp.m_Recette.m_OPCClient->InitialiseItems();

//GROUPE ROBOT1
//------------------------------------------------------------------------------------------------------
	//Groupe Robot1 Track Write
	theApp.m_Robot1.m_OPCClient_TW ->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Robot1.m_OPCClient_TW ->Area = daveOutputs;
	theApp.m_Robot1.m_OPCClient_TW ->iTypeGroupe = GROUPE_WRITE;
	theApp.m_Robot1.m_OPCClient_TW ->NumDataBlock = 0;
	theApp.m_Robot1.m_OPCClient_TW ->IndexDataBlock = 53;
	theApp.m_Robot1.m_OPCClient_TW->GererCycle = &theApp.m_Robot1.GererCycle;
	theApp.m_Robot1.m_OPCClient_TW ->AlarmeVal = 16;

	//AJOUT DES ITEMS
	theApp.m_Robot1.m_OPCClient_TW->Lier(&theApp.m_Robot1.WBit1,VT_ACYBOOL,_T("Verif Etalon OK"));
	theApp.m_Robot1.m_OPCClient_TW->Lier(&theApp.m_Robot1.WBit2,VT_ACYBOOL,_T("demande tare"));
	theApp.m_Robot1.m_OPCClient_TW->Lier(&theApp.m_Robot1.WBit3,VT_ACYBOOL,_T("Acquittement Pesons"));
	theApp.m_Robot1.m_OPCClient_TW->Lier(&theApp.m_Robot1.X,VT_I2,_T("Position X"));
	theApp.m_Robot1.m_OPCClient_TW->Lier(&theApp.m_Robot1.Y,VT_I2,_T("Position Y"));
	theApp.m_Robot1.m_OPCClient_TW->Lier(&theApp.m_Robot1.Z,VT_I2,_T("Position Z"));
	theApp.m_Robot1.m_OPCClient_TW->Lier(&theApp.m_Robot1.PosStatut,VT_I2,_T("Statut Position"));
	
	theApp.m_Robot1.m_OPCClient_TW->InitialiseServeur();
	theApp.m_Robot1.m_OPCClient_TW->InitialiseGroupe();
	theApp.m_Robot1.m_OPCClient_TW->InitialiseItems();

	//Groupe Robot1 Track Read

	theApp.m_Robot1.m_OPCClient_TR ->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Robot1.m_OPCClient_TR ->Area = daveInputs;
	theApp.m_Robot1.m_OPCClient_TR ->iTypeGroupe = GROUPE_READ;
	theApp.m_Robot1.m_OPCClient_TR ->NumDataBlock = 0;
	theApp.m_Robot1.m_OPCClient_TR ->IndexDataBlock = 56;
	theApp.m_Robot1.m_OPCClient_TR->GererCycle = &theApp.m_Robot1.GererCycle;
	theApp.m_Robot1.m_OPCClient_TR ->AlarmeVal = 32;

	//AJOUT DES ITEMS
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.m_Robot1.DefWord,VT_I2,_T("Mot défaut"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[0].StabilisedValue,VT_I2,_T("Poids Peson1"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[1].StabilisedValue,VT_I2,_T("Poids Peson2"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[0].LastMesure,VT_I2,_T("Dernier Poids Peson1"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[1].LastMesure,VT_I2,_T("Dernier Poids Peson2"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.m_Robot1.NbElementsBoiteUnused,VT_I2,_T("Compteur OK"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.m_Robot1.NbRebusPoidsUnused,VT_I2,_T("Compteur NOK Poids"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.m_Robot1.NbRebusVisionUnused,VT_I2,_T("Compteur NOK Vision"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(NULL,VT_EMPTY,_T(""));
	theApp.m_Robot1.m_OPCClient_TR->Lier(NULL,VT_EMPTY,_T(""));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[0].StatutLastMesure,VT_I2,_T("Statut Dernier Poids Peson1"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[1].StatutLastMesure,VT_I2,_T("Statut Dernier Poids Peson2"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[0].PoidsEtalon1,VT_I2,_T("Poids P1 Etalon1"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[0].PoidsEtalon2,VT_I2,_T("Poids P1 Etalon2"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[1].PoidsEtalon1,VT_I2,_T("Poids P2 Etalon1"));
	theApp.m_Robot1.m_OPCClient_TR->Lier(&theApp.Pesons[1].PoidsEtalon2,VT_I2,_T("Poids P2 Etalon2"));
	
	theApp.m_Robot1.m_OPCClient_TR->InitialiseServeur();
	theApp.m_Robot1.m_OPCClient_TR->InitialiseGroupe();
	theApp.m_Robot1.m_OPCClient_TR->InitialiseItems();

//GROUPE ROBOT2
//------------------------------------------------------------------------------------------------------
	//Groupe Robot1 Track Write
	theApp.m_Robot2.m_OPCClient_TW ->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Robot2.m_OPCClient_TW ->Area = daveOutputs;
	theApp.m_Robot2.m_OPCClient_TW ->iTypeGroupe = GROUPE_WRITE;
	theApp.m_Robot2.m_OPCClient_TW ->NumDataBlock = 0;
	theApp.m_Robot2.m_OPCClient_TW ->IndexDataBlock = 103;
	theApp.m_Robot2.m_OPCClient_TW->GererCycle = &theApp.m_Robot2.GererCycle;
	theApp.m_Robot2.m_OPCClient_TW ->AlarmeVal = 64;

	//AJOUT DES ITEMS
	theApp.m_Robot2.m_OPCClient_TW->Lier(&theApp.m_Robot2.WBit1,VT_ACYBOOL,_T("Verif Etalon OK"));
	theApp.m_Robot2.m_OPCClient_TW->Lier(&theApp.m_Robot2.WBit2,VT_ACYBOOL,_T("demande tare"));
	theApp.m_Robot2.m_OPCClient_TW->Lier(&theApp.m_Robot2.WBit3,VT_ACYBOOL,_T("Acquittement Pesons"));
	theApp.m_Robot2.m_OPCClient_TW->Lier(&theApp.m_Robot2.X,VT_I2,_T("Position X"));
	theApp.m_Robot2.m_OPCClient_TW->Lier(&theApp.m_Robot2.Y,VT_I2,_T("Position Y"));
	theApp.m_Robot2.m_OPCClient_TW->Lier(&theApp.m_Robot2.Z,VT_I2,_T("Position Z"));
	theApp.m_Robot2.m_OPCClient_TW->Lier(&theApp.m_Robot2.PosStatut,VT_I2,_T("Statut Position"));
	
	theApp.m_Robot2.m_OPCClient_TW->InitialiseServeur();
	theApp.m_Robot2.m_OPCClient_TW->InitialiseGroupe();
	theApp.m_Robot2.m_OPCClient_TW->InitialiseItems();

	//Groupe Robot1 Track Read

	theApp.m_Robot2.m_OPCClient_TR ->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Robot2.m_OPCClient_TR ->Area = daveInputs;
	theApp.m_Robot2.m_OPCClient_TR ->iTypeGroupe = GROUPE_READ;
	theApp.m_Robot2.m_OPCClient_TR ->NumDataBlock = 0;
	theApp.m_Robot2.m_OPCClient_TR ->IndexDataBlock = 106;
	theApp.m_Robot2.m_OPCClient_TR->GererCycle = &theApp.m_Robot2.GererCycle;
	theApp.m_Robot2.m_OPCClient_TR ->AlarmeVal = 128;

	//AJOUT DES ITEMS
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.m_Robot2.DefWord,VT_I2,_T("Mot défaut"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[2].StabilisedValue,VT_I2,_T("Poids Peson1"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[3].StabilisedValue,VT_I2,_T("Poids Peson2"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[2].LastMesure,VT_I2,_T("Dernier Poids Peson1"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[3].LastMesure,VT_I2,_T("Dernier Poids Peson2"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.m_Robot2.NbElementsBoiteUnused,VT_I2,_T("Compteur OK"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.m_Robot2.NbRebusPoidsUnused,VT_I2,_T("Compteur NOK Poids"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.m_Robot2.NbRebusVisionUnused,VT_I2,_T("Compteur NOK Vision"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(NULL,VT_EMPTY,_T(""));
	theApp.m_Robot2.m_OPCClient_TR->Lier(NULL,VT_EMPTY,_T(""));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[2].StatutLastMesure,VT_I2,_T("Statut Dernier Poids Peson1"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[3].StatutLastMesure,VT_I2,_T("Statut Dernier Poids Peson2"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[2].PoidsEtalon1,VT_I2,_T("Poids P1 Etalon1"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[2].PoidsEtalon2,VT_I2,_T("Poids P1 Etalon2"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[3].PoidsEtalon1,VT_I2,_T("Poids P2 Etalon1"));
	theApp.m_Robot2.m_OPCClient_TR->Lier(&theApp.Pesons[3].PoidsEtalon2,VT_I2,_T("Poids P2 Etalon2"));
	
	theApp.m_Robot2.m_OPCClient_TR->InitialiseServeur();
	theApp.m_Robot2.m_OPCClient_TR->InitialiseGroupe();
	theApp.m_Robot2.m_OPCClient_TR->InitialiseItems();


//GROUPE ROBOT3
//------------------------------------------------------------------------------------------------------
			
		//Groupe Robot1 Track Write
	theApp.m_Robot3.m_OPCClient_TW ->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Robot3.m_OPCClient_TW ->Area = daveOutputs;
	theApp.m_Robot3.m_OPCClient_TW ->iTypeGroupe = GROUPE_WRITE;
	theApp.m_Robot3.m_OPCClient_TW ->NumDataBlock = 0;
	theApp.m_Robot3.m_OPCClient_TW ->IndexDataBlock = 153;
	theApp.m_Robot3.m_OPCClient_TW->GererCycle = &theApp.m_Robot3.GererCycle;
	theApp.m_Robot3.m_OPCClient_TW ->AlarmeVal = 256;

	//AJOUT DES ITEMS
	theApp.m_Robot3.m_OPCClient_TW->Lier(&theApp.m_Robot3.WBit1,VT_ACYBOOL,_T("Verif Etalon OK"));
	theApp.m_Robot3.m_OPCClient_TW->Lier(&theApp.m_Robot3.WBit2,VT_ACYBOOL,_T("demande tare"));
	theApp.m_Robot3.m_OPCClient_TW->Lier(&theApp.m_Robot3.WBit3,VT_ACYBOOL,_T("Acquittement Pesons"));
	theApp.m_Robot3.m_OPCClient_TW->Lier(&theApp.m_Robot3.X,VT_I2,_T("Position X"));
	theApp.m_Robot3.m_OPCClient_TW->Lier(&theApp.m_Robot3.Y,VT_I2,_T("Position Y"));
	theApp.m_Robot3.m_OPCClient_TW->Lier(&theApp.m_Robot3.Z,VT_I2,_T("Position Z"));
	theApp.m_Robot3.m_OPCClient_TW->Lier(&theApp.m_Robot3.PosStatut,VT_I2,_T("Statut Position"));
	
	theApp.m_Robot3.m_OPCClient_TW->InitialiseServeur();
	theApp.m_Robot3.m_OPCClient_TW->InitialiseGroupe();
	theApp.m_Robot3.m_OPCClient_TW->InitialiseItems();

	//Groupe Robot1 Track Read

	theApp.m_Robot3.m_OPCClient_TR ->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.m_Robot3.m_OPCClient_TR ->Area = daveInputs;
	theApp.m_Robot3.m_OPCClient_TR ->iTypeGroupe = GROUPE_READ;
	theApp.m_Robot3.m_OPCClient_TR ->NumDataBlock = 0;
	theApp.m_Robot3.m_OPCClient_TR ->IndexDataBlock = 156;
	theApp.m_Robot3.m_OPCClient_TR->GererCycle = &theApp.m_Robot3.GererCycle;
	theApp.m_Robot3.m_OPCClient_TR ->AlarmeVal = 512;

	//AJOUT DES ITEMS
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.m_Robot3.DefWord,VT_I2,_T("Mot défaut"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[4].StabilisedValue,VT_I2,_T("Poids Peson1"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[5].StabilisedValue,VT_I2,_T("Poids Peson2"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[4].LastMesure,VT_I2,_T("Dernier Poids Peson1"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[5].LastMesure,VT_I2,_T("Dernier Poids Peson2"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.m_Robot3.NbElementsBoiteUnused,VT_I2,_T("Compteur OK"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.m_Robot3.NbRebusPoidsUnused,VT_I2,_T("Compteur NOK Poids"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.m_Robot3.NbRebusVisionUnused,VT_I2,_T("Compteur NOK Vision"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(NULL,VT_EMPTY,_T(""));
	theApp.m_Robot3.m_OPCClient_TR->Lier(NULL,VT_EMPTY,_T(""));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[4].StatutLastMesure,VT_I2,_T("Statut Dernier Poids Peson1"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[5].StatutLastMesure,VT_I2,_T("Statut Dernier Poids Peson2"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[4].PoidsEtalon1,VT_I2,_T("Poids P1 Etalon1"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[4].PoidsEtalon2,VT_I2,_T("Poids P1 Etalon2"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[5].PoidsEtalon1,VT_I2,_T("Poids P2 Etalon1"));
	theApp.m_Robot3.m_OPCClient_TR->Lier(&theApp.Pesons[5].PoidsEtalon2,VT_I2,_T("Poids P2 Etalon2"));
	
	theApp.m_Robot3.m_OPCClient_TR->InitialiseServeur();
	theApp.m_Robot3.m_OPCClient_TR->InitialiseGroupe();
	theApp.m_Robot3.m_OPCClient_TR->InitialiseItems();

//FIN GroUPE ROBOTS

	//Groupe Process Read
	m_OPCClient_R ->strNomServeur = ParamStrNomServeurOpc->value;
	m_OPCClient_R ->Area = daveDB;
	m_OPCClient_R ->iTypeGroupe = GROUPE_READ;
	m_OPCClient_R ->NumDataBlock = 110;
	m_OPCClient_R ->IndexDataBlock = 0;
	m_OPCClient_R->GererCycle = &GererCycle;
	m_OPCClient_R ->AlarmeVal = 1024;

	//AJOUT DES ITEMS
	m_OPCClient_R->Lier(&RBit1,VT_ACYBOOL,_T("DB110.0 Acquit. Defauts"));//00
	m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//01
	m_OPCClient_R->Lier(&RBit2,VT_ACYBOOL,_T("DB110.2 Defauts"));//02
	m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//03
	m_OPCClient_R->Lier(&RBit3,VT_ACYBOOL,_T("DB110.4 Defauts"));//04
	m_OPCClient_R->Lier(&RBit4,VT_ACYBOOL,_T("DB110.5 Defauts"));//05
	m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//06
	m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//07
	//V37 : activation cadence sur #1
	//if (theConfig.str_Machine.Find(_T("#2"))>= 0)	
	//{
		m_OPCClient_R->Lier(&iVal110_8,VT_I1,_T("DB110.8 Cadence High"));//08 
		m_OPCClient_R->Lier(&iVal110_9,VT_I1,_T("DB110.9 Cadence Low"));//09
	//}
	//else
	//{
	//	m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//08
	//	m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//09
	//}

	m_OPCClient_R->Lier(&theApp.PesonEtui.iVal110_10,VT_I1,_T("DB110.10 Poids Moyen Minims High"));//10 
	m_OPCClient_R->Lier(&theApp.PesonEtui.iVal110_11,VT_I1,_T("DB110.11 Poids Moyen Minims Low"));//11

	m_OPCClient_R->Lier(&theApp.PesonEtui.iVal110_12,VT_I1,_T("DB110.12 Tare Etui Vide High"));//12 
	m_OPCClient_R->Lier(&theApp.PesonEtui.iVal110_13,VT_I1,_T("DB110.13 Tare Etui Vide Low"));//13

	m_OPCClient_R->Lier(&theApp.PesonEtui.iVal110_14,VT_I1,_T("DB110.14 Tare Etui Plein High"));//14 
	m_OPCClient_R->Lier(&theApp.PesonEtui.iVal110_15,VT_I1,_T("DB110.15 Tare Etui Plein Low"));//15

	m_OPCClient_R->InitialiseServeur();
	m_OPCClient_R->InitialiseItems();

	//Groupe Process Write
	m_OPCClient_W ->strNomServeur = ParamStrNomServeurOpc->value;
	m_OPCClient_W ->Area = daveDB;
	m_OPCClient_W ->iTypeGroupe = GROUPE_WRITE;
	m_OPCClient_W ->NumDataBlock = 101;
	m_OPCClient_W ->IndexDataBlock = 0;
	m_OPCClient_W->GererCycle = &GererCycle;
	m_OPCClient_W ->AlarmeVal = 2048;

	//AJOUT DES ITEMS
	IndexStartCycle = m_OPCClient_W->Lier(&WBit0,VT_ACYBOOL,_T("DB101.0 Modes machines"));//00
	m_OPCClient_W->Lier(&WBit1,VT_ACYBOOL,_T("DB101.1 Modes Ctrl Etiquette"));//01
	m_OPCClient_W->Lier(&WBit2,VT_ACYBOOL,_T("DB101.2 Inhib. machines"));//02
	m_OPCClient_W->Lier(&WBit3,VT_ACYBOOL,_T("DB101.3 Inhib. machines"));//03

	m_OPCClient_W->Lier(&theApp.CurrentUser.NiveauOperateur,VT_I2,_T("DB101.4/5 Niveau Operateur"));//Octets 4/5
	m_OPCClient_W->Lier(&WBit4,VT_ACYBOOL,_T("DB101.6 Demandes Tare Pesons"));//06
	m_OPCClient_W->Lier(&WBit45,VT_ACYBOOL,_T("DB101.7 Demandes Verif. Dyna. Pesons"));//07
	m_OPCClient_W->Lier(&WBit5,VT_ACYBOOL,_T("DB101.8 Valid. Pesons"));//08
	m_OPCClient_W->Lier(&WBit6,VT_ACYBOOL,_T("DB101.9 Valid. Pesons"));//09
	m_OPCClient_W->Lier(&WBit7,VT_ACYBOOL,_T("DB101.10 Cmd DMX"));//10
	m_OPCClient_W->Lier(NULL,VT_EMPTY,_T(""));//11
	m_OPCClient_W->Lier(&WBit8,VT_ACYBOOL,_T("DB101.12 Cmd Repli Robots"));//12

//V 39
	/*if (theConfig.str_Machine.Find(_T("#2"))>= 0)
	{*/
		m_OPCClient_W->Lier(NULL,VT_EMPTY,_T(""));//13
		m_OPCClient_W->Lier(&WBit9,VT_ACYBOOL,_T("DB101.14 Eclairage"));//14
	//}
	//else
	//{
	//	
	//	//m_OPCClient_W->Lier(NULL,VT_EMPTY,_T(""));//13
	//	//m_OPCClient_W->Lier(NULL,VT_EMPTY,_T(""));//14
	//}

	m_OPCClient_W->Lier(NULL, VT_EMPTY, _T(""));//15
	m_OPCClient_W->Lier(&iValResult_DmxEtui, VT_I1, _T("DB101.16 Res DMX Etui"));//16 WOctet1
	m_OPCClient_W->Lier(&iValResult_DmxNotice1, VT_I1, _T("DB101.17 Res DMX Notice1"));//17 WOctet1
	m_OPCClient_W->Lier(&iValResult_DmxNotice2, VT_I1, _T("DB101.18 Res DMX Notice2"));//18 WOctet1

	m_OPCClient_W->InitialiseServeur();
	m_OPCClient_W->InitialiseGroupe();

	m_OPCClient_W->InitialiseItems();

	//GCA 1412
	//GROUPE COMPTEURS
	theApp.Compteurs.m_OPCClient_R ->strNomServeur = ParamStrNomServeurOpc->value;
	theApp.Compteurs.m_OPCClient_R ->Area = daveDB;
	theApp.Compteurs.m_OPCClient_R ->iTypeGroupe = GROUPE_READ;
	theApp.Compteurs.m_OPCClient_R ->NumDataBlock = 400;
	theApp.Compteurs.m_OPCClient_R ->IndexDataBlock = 0;
	theApp.Compteurs.m_OPCClient_R ->GererCycle = &theApp.Compteurs.GererCycle;
	theApp.Compteurs.m_OPCClient_R ->AlarmeVal = 4096;

	//AJOUT DES ITEMS
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.RBit1,VT_ACYBOOL,_T("DB400.0 RAZ Count"));//0
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.RBit2,VT_ACYBOOL,_T("DB400.1 RAZ Count"));//1
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbFormes[0],VT_I2,_T("DB400.2 Etuis Formés"));//2 

	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbDMXNonLu[0],VT_I2,_T("DB400.4 DMX Etui NonLu"));//4 DMX Etuis non lus
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbDMXLu[0],VT_I2,_T("DB400.6 Etuis OK"));//6 DMX OK
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbFormes[1],VT_I2,_T("DB400.8 Notice1 Formés"));//8
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbDMXNonLu[1],VT_I2,_T("DB400.10 DMX Notice1 NonLu"));//10
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbDMXLu[1],VT_I2,_T("DB400.12 Notice1 OK"));//12
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbFormes[2],VT_I2,_T("DB400.14 Notice2 Formés"));//14
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbDMXNonLu[2],VT_I2,_T("DB400.16 DMX Notice2 NonLu"));//16
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbDMXLu[2],VT_I2,_T("DB400.18 Notice2 OK"));//18
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbBoitesFermees,VT_I2,_T("DB400.20 Boite Fermées"));//20
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbEtiqCollees,VT_I2,_T("DB400.22 Etiq. Collées"));//22
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbEtiqNonVues,VT_I2,_T("DB400.24 Etiq. Non Vues"));//24

	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.m_Recette.NbBoiteNOK[1],VT_I2,_T("DB400.26 Etuis Ejectées Mauvais"));//26
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.m_Recette.NbBoiteOK[1],VT_I2,_T("DB400.28 Etuis Ejectées Bon"));//28

	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbEtuis[0],VT_I2,_T("DB400.30 Etuis Robot 1"));//30
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbEtuis[1],VT_I2,_T("DB400.32 Etuis Robot 2"));//32
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbEtuis[2],VT_I2,_T("DB400.34 Etuis Robot 3"));//34
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbMinimsEtuis,VT_I4,_T("DB400.36 Tot. Minims Etuis"));//36
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbElementsBoite[0],VT_I2,_T("DB400.40 Minims Etuis Robot 1"));//40
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbElementsBoite[1],VT_I2,_T("DB400.42 Minims Etuis Robot 2"));//42
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbElementsBoite[2],VT_I2,_T("DB400.44 Minims Etuis Robot 3"));//44
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbRebusPoids[0],VT_I2,_T("DB400.46 Rebus Poids Robot 1"));//46
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbRebusPoids[1],VT_I2,_T("DB400.48 Rebus Poids Robot 2"));//48
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbRebusPoids[2],VT_I2,_T("DB400.50 Rebus Poids Robot 3"));//50
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbRebusVision[0],VT_I2,_T("DB400.52 Rebus Poids Robot 1"));//52
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbRebusVision[1],VT_I2,_T("DB400.54 Rebus Poids Robot 2"));//54
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbRebusVision[2],VT_I2,_T("DB400.56 Rebus Poids Robot 3"));//56
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbMinimsRebusPoids,VT_I2,_T("DB400.58 Tot. Rebus Poids"));//58
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbMinimsRebusVision,VT_I2,_T("DB400.60 Tot. Rebus Vision"));//60
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.NbMinimsPrisRobot,VT_I4,_T("DB400.62 Tot. Minims Pris"));//62
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.EtuisVidesDistri,VT_I2,_T("DB400.66 Etuis Vides M10"));//66
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.EtuisPleinsPris,VT_I2,_T("DB400.68 Etuis Pleins M10"));//68
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.TotalNoRAZ,VT_I4,_T("DB400.70 Tot. no RAZ"));//70
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//74
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//75
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//76
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//77
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.EncoursEtui[0],VT_I2,_T("DB400.78 Encours Etui 1"));//78
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.EncoursEtui[1],VT_I2,_T("DB400.80 Encours Etui 2"));//80
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.EncoursEtui[2],VT_I2,_T("DB400.82 Encours Etui 3"));//82

	//#3 DMX Correction recalage (le 83 c'est le deuxieme mot du precédent) 
	//theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//83
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//84
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//85
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//86
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//87
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//88
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//89
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//90
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//91
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//92
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//93
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//94
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//95
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//96
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//97
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//98
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//99
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//100 Compteur Minims HS
	theApp.Compteurs.m_OPCClient_R->Lier(NULL,VT_EMPTY,_T(""));//101
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.iNbEtuisNCPoids,VT_I2,_T("DB400.102 Rebut Etuis Poids"));//102
	theApp.Compteurs.m_OPCClient_R->Lier(&theApp.Compteurs.iNbEtuisNCEtiquette,VT_I2,_T("DB400.104 Rebut Etuis Etiquette"));//104


	theApp.Compteurs.m_OPCClient_R->InitialiseServeur();
	theApp.Compteurs.m_OPCClient_R->InitialiseGroupe();

	theApp.Compteurs.m_OPCClient_R->InitialiseItems();
	//FIN GROUPE COMPTEURS

	//V 43.3 
	//Liaison dynamique des groupes alarmes/alertes en fonction des declarations de la base de données
	//notamment pour éviter les oublis

	//GROUPES DEFAUTS 
	//DB301
	m_OPCClient_Ai->strNomServeur = ParamStrNomServeurOpc->value;
	m_OPCClient_Ai->Area = daveDB;
	m_OPCClient_Ai->iTypeGroupe = GROUPE_READ;
	m_OPCClient_Ai->NumDataBlock = 301;
	m_OPCClient_Ai->IndexDataBlock = 0;
	m_OPCClient_Ai->GererCycle = &GererCycle;
	m_OPCClient_Ai->AlarmeVal = 8192;
	//DB302
	m_OPCClient_AFCy->strNomServeur = ParamStrNomServeurOpc->value;
	m_OPCClient_AFCy->Area = daveDB;
	m_OPCClient_AFCy->iTypeGroupe = GROUPE_READ;
	m_OPCClient_AFCy->NumDataBlock = 302;
	m_OPCClient_AFCy->IndexDataBlock = 0;
	m_OPCClient_AFCy->GererCycle = &GererCycle;
	m_OPCClient_AFCy->AlarmeVal = 16384;
	//DB303
	m_OPCClient_Alertes->strNomServeur = ParamStrNomServeurOpc->value;
	m_OPCClient_Alertes->Area = daveDB;
	m_OPCClient_Alertes->iTypeGroupe = GROUPE_READ;
	m_OPCClient_Alertes->NumDataBlock = 303;
	m_OPCClient_Alertes->IndexDataBlock = 0;
	m_OPCClient_Alertes->GererCycle = &GererCycle;
	m_OPCClient_Alertes->AlarmeVal = 32768;

	CString strLibelle = _T("");
	//V 44.0 BOOL Def301_IsLie[NB_MOTS_DB301] = { FALSE };
	for (int i = 0; i < (Def301_AdresseMax + 1); i++)
	{
		//V 44.0 	Def301_IsLie[i] =  FALSE ;
		strLibelle.Format(_T("Def 301.%d"), i);
		m_OPCClient_Ai->Lier(&Def301[i], VT_ACYBOOL, strLibelle);
	}

	//V 44.0 BOOL Def302_IsLie[NB_MOTS_DB302] = { FALSE };
	for (int i = 0; i < (Def302_AdresseMax + 1); i++)
	{
		//V 44.0 Def302_IsLie[i] = FALSE;
		strLibelle.Format(_T("Def 302.%d"), i);
		m_OPCClient_AFCy->Lier(&Def302[i], VT_ACYBOOL, strLibelle);
	}

	//V 44.0 BOOL Def303_IsLie[NB_MOTS_DB303] = { FALSE };
	for (int i = 0; i < (Def303_AdresseMax + 1); i++)
	{
		//V 44.0 Def303_IsLie[i] = FALSE;
		strLibelle.Format(_T("Def 303.%d"), i);
		m_OPCClient_Alertes->Lier(&Def303[i], VT_ACYBOOL, strLibelle);

	}

	//for (int i = 0; i < NB_ALARMES; i++)
	//{
	//	if (theApp.m_GestAlarmes.Alarme[i].DB_Num > 0)
	//	{
	//		switch (theApp.m_GestAlarmes.Alarme[i].DB_Num)
	//		{

	//		case 301:
	//			if (theApp.m_GestAlarmes.Alarme[i].DB_Adresse >= NB_MOTS_DB301)
	//			{
	//				CString strMessage = _T("");
	//				strMessage.Format(_T("Alarme %d Adresse %d trop haute ( >= NB_MOTS_DB301)"), i, theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//				AfxMessageBox(strMessage, MB_OK, MB_ICONWARNING);
	//				continue;
	//			}

	//			/*if (Def301_IsLie[theApp.m_GestAlarmes.Alarme[i].DB_Adresse] == FALSE)
	//			{
	//				strLibelle.Format(_T("Def Bit %d"), theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//				m_OPCClient_Ai->Lier(&Def301[theApp.m_GestAlarmes.Alarme[i].DB_Adresse], VT_ACYBOOL, strLibelle);

	//				Def301_IsLie[theApp.m_GestAlarmes.Alarme[i].DB_Adresse] = TRUE;
	//			}*/
	//			
	//			break;

	//		case 302:
	//			if (theApp.m_GestAlarmes.Alarme[i].DB_Adresse >= NB_MOTS_DB302)
	//			{
	//				CString strMessage = _T("");
	//				strMessage.Format(_T("Alarme %d Adresse %d trop haute ( >= NB_MOTS_DB302)"), i, theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//				AfxMessageBox(strMessage, MB_OK, MB_ICONWARNING);
	//				continue;
	//			}

	//			/*if (Def302_IsLie[theApp.m_GestAlarmes.Alarme[i].DB_Adresse] == FALSE)
	//			{
	//				strLibelle.Format(_T("Def Bit %d"), theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//				m_OPCClient_AFCy->Lier(&Def302[theApp.m_GestAlarmes.Alarme[i].DB_Adresse], VT_ACYBOOL, strLibelle);

	//				Def302_IsLie[theApp.m_GestAlarmes.Alarme[i].DB_Adresse] = TRUE;
	//			}*/
	//			break;

	//		case 303:
	//			if (theApp.m_GestAlarmes.Alarme[i].DB_Adresse >= NB_MOTS_DB303)
	//			{
	//				CString strMessage = _T("");
	//				strMessage.Format(_T("Alarme %d Adresse %d trop haute ( >= NB_MOTS_DB303)"), i, theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//				AfxMessageBox(strMessage, MB_OK, MB_ICONWARNING);
	//				continue;
	//			}

	//		/*	if (Def303_IsLie[theApp.m_GestAlarmes.Alarme[i].DB_Adresse] == FALSE)
	//			{
	//				strLibelle.Format(_T("Def Bit %d"), theApp.m_GestAlarmes.Alarme[i].DB_Adresse);
	//				m_OPCClient_Alertes->Lier(&Def303[theApp.m_GestAlarmes.Alarme[i].DB_Adresse], VT_ACYBOOL, strLibelle);

	//				Def303_IsLie[theApp.m_GestAlarmes.Alarme[i].DB_Adresse] = TRUE;
	//			}*/
	//			break;

	//		default:
	//			break;

	//		}
	//	}
	//	
	//}

	//AJOUT DES ITEMS
	//m_OPCClient_Ai->Lier(&Def301[0],VT_ACYBOOL,_T("Def Bit 0"));//0
	//m_OPCClient_Ai->Lier(&Def301[1],VT_ACYBOOL,_T("Def Bit 1"));//1
	//m_OPCClient_Ai->Lier(&Def301[2],VT_ACYBOOL,_T("Def Bit 2"));//2
	//m_OPCClient_Ai->Lier(NULL,VT_EMPTY,_T(""));//3
	//m_OPCClient_Ai->Lier(&Def301[4],VT_ACYBOOL,_T("Def Bit 4"));//4

	m_OPCClient_Ai->InitialiseServeur();
	m_OPCClient_Ai->InitialiseGroupe();

	m_OPCClient_Ai->InitialiseItems();
		
	

	//AJOUT DES ITEMS
		
	//m_OPCClient_AFCy->Lier(&Def302[0],VT_ACYBOOL,_T("Def Bit 0"));//0
	//m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//1
	//m_OPCClient_AFCy->Lier(&Def302[2],VT_ACYBOOL,_T("Def Bit 2"));//2
	//if (theConfig.str_Machine.Find(_T("#2"))>= 0)
	//{
	//	//Machine #2
	//	m_OPCClient_AFCy->Lier(&Def302[3],VT_ACYBOOL,_T("Def Bit 3"));//3
	//}
	//else
	//{
	//	//Machine #1
	//	m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//3
	//}

	//m_OPCClient_AFCy->Lier(&Def302[4],VT_ACYBOOL,_T("Def Bit 4"));//4
	//pBitMixup[0] = &Def302[4][0];
	//m_OPCClient_AFCy->Lier(&Def302[5],VT_ACYBOOL,_T("Def Bit 5"));//5
	//m_OPCClient_AFCy->Lier(&Def302[6],VT_ACYBOOL,_T("Def Bit 6"));//6
	//pBitMixup[1] = &Def302[6][0];
	//m_OPCClient_AFCy->Lier(&Def302[7],VT_ACYBOOL,_T("Def Bit 7"));//7
	//m_OPCClient_AFCy->Lier(&Def302[8],VT_ACYBOOL,_T("Def Bit 8"));//8
	//pBitMixup[2] = &Def302[8][0];
	//m_OPCClient_AFCy->Lier(&Def302[9],VT_ACYBOOL,_T("Def Bit 9"));//9
	//m_OPCClient_AFCy->Lier(&Def302[10],VT_ACYBOOL,_T("Def Bit 10"));//10
	//m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//11
	//m_OPCClient_AFCy->Lier(&Def302[12],VT_ACYBOOL,_T("Def Bit 12"));//12
	pBitMixup[3] = &Def302[12][0];
	pBitMixup[4] = &Def302[12][1];
	pBitMixup[5] = &Def302[12][2];

	//m_OPCClient_AFCy->Lier(&Def302[13],VT_ACYBOOL,_T("Def Bit 13"));//13
	//m_OPCClient_AFCy->Lier(&Def302[14],VT_ACYBOOL,_T("Def Bit 14"));//14
	//m_OPCClient_AFCy->Lier(&Def302[15],VT_ACYBOOL,_T("Def Bit 15"));//15
	//m_OPCClient_AFCy->Lier(&Def302[16],VT_ACYBOOL,_T("Def Bit 16"));//16
	//m_OPCClient_AFCy->Lier(&Def302[17],VT_ACYBOOL,_T("Def Bit 17"));//17
	//m_OPCClient_AFCy->Lier(&Def302[18],VT_ACYBOOL,_T("Def Bit 18"));//18
	//m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//19
	//m_OPCClient_AFCy->Lier(&Def302[20],VT_ACYBOOL,_T("Def Bit 20"));//20	//V2.52 : Ajout
	//m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//21
	//m_OPCClient_AFCy->Lier(&Def302[22],VT_ACYBOOL,_T("Def Bit 22"));//22	//V2.52 : Ajout
	//m_OPCClient_AFCy->Lier(&Def302[23],VT_ACYBOOL,_T("Def Bit 23"));//23	//GCA 3.7
	//m_OPCClient_AFCy->Lier(&Def302[24],VT_ACYBOOL,_T("Def Bit 24"));//24	//V2.52 : Ajout
	//m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//25
	//m_OPCClient_AFCy->Lier(&Def302[26],VT_ACYBOOL,_T("Def Bit 26"));//26	//V2.52 : Ajout
	//m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//27
	//m_OPCClient_AFCy->Lier(&Def302[28],VT_ACYBOOL,_T("Def Bit 28"));//28	//V2.52 : Ajout
	//m_OPCClient_AFCy->Lier(&Def302[29],VT_ACYBOOL,_T("Def Bit 29"));//29	//V2.54 : Ajout
	//m_OPCClient_AFCy->Lier(&Def302[30],VT_ACYBOOL,_T("Def Bit 30"));//30	//GCA 3.7
	
	//if (theConfig.str_Machine.Find(_T("#2"))>= 0)
	//{
	//	m_OPCClient_AFCy->Lier(&Def302[31],VT_ACYBOOL,_T("Def Bit 31"));//31	//GCA 3.7

	/*
	V 42 Ajout Alarmes serialisation
	1.1.	Liste des défauts supplémentaires
	DB302 32.0           P9 : Syst. Sérialisation : non prêt ou erreur
	DB302 32.1           P9 : Pas de réponse caméra de sérialisation
	DB302 32.2           P9 : 3 défauts consécutifs de relecture sérialisation
	DB302 32.3           P13 : Défaut autocontrôle cellule présence étui
	DB302 32.4           P13 : L’étui ne doit pas être présent
	DB302 32.5           P13 : L’étui doit être présent
	DB302 32.6           P9 : Fin cartouches encre impression poste 9
	DB302 32.7           P9 : Incohérence pile LSM ne doit pas être vide
	DB302 32.8           P9 : Incohérence pile LSM  doit être vid


	*/
//	m_OPCClient_AFCy->Lier(&Def302[32], VT_ACYBOOL, _T("Def Bit 32"));//32	//GCA V 42
//	m_OPCClient_AFCy->Lier(&Def302[33], VT_ACYBOOL, _T("Def Bit 33"));//33	//GCA V 42
	//}
	//else
	//{
	//	m_OPCClient_AFCy->Lier(NULL,VT_EMPTY,_T(""));//31
	//}

	bool bThreadOK = m_OPCClient_AFCy->InitialiseServeur();
	m_OPCClient_AFCy->InitialiseGroupe();

	m_OPCClient_AFCy->InitialiseItems();

	

	//AJOUT DES ITEMS
	//m_OPCClient_Alertes->Lier(&Def303[0],VT_ACYBOOL,_T("Def Bit 0"));//0
	//m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//1
	//m_OPCClient_Alertes->Lier(&Def303[2],VT_ACYBOOL,_T("Def Bit 2"));//2
	//

	//m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//3
	//

	//m_OPCClient_Alertes->Lier(&Def303[4],VT_ACYBOOL,_T("Def Bit 4"));//4
	//m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//5
	//m_OPCClient_Alertes->Lier(&Def303[6],VT_ACYBOOL,_T("Def Bit 6"));//6
	////m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//7
	////V 43.3 
	//m_OPCClient_Alertes->Lier(&Def303[7], VT_ACYBOOL, _T("Def Bit 7"));//7
	//m_OPCClient_Alertes->Lier(&Def303[8],VT_ACYBOOL,_T("Def Bit 8"));//8
	//m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//9
	//m_OPCClient_Alertes->Lier(&Def303[10],VT_ACYBOOL,_T("Def Bit 10"));//10
	//m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//11
	//m_OPCClient_Alertes->Lier(&Def303[12],VT_ACYBOOL,_T("Def Bit 12"));//12
	//m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//13
	//m_OPCClient_Alertes->Lier(&Def303[14],VT_ACYBOOL,_T("Def Bit 14"));//14
	//m_OPCClient_Alertes->Lier(NULL,VT_EMPTY,_T(""));//15
	//m_OPCClient_Alertes->Lier(&Def303[16],VT_ACYBOOL,_T("Def Bit 16"));//16
	//m_OPCClient_Alertes->Lier(&Def303[17],VT_ACYBOOL,_T("Def Bit 17"));//17
		
	bThreadOK = m_OPCClient_Alertes->InitialiseServeur();
	m_OPCClient_Alertes->InitialiseGroupe();

	m_OPCClient_Alertes->InitialiseItems();


	//initialisation des alarmes
	theApp.m_GestAlarmes.InitialiserAlarmes();

	//FIN GROUPES DEFAUTS

	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Lancement Communication Automate");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	//lancement lecture OPC
	if(bThreadOK)
		theApp.StartThreadOPC();

	// Initialisation HTML Lecteurs DMX
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Initialisation des lecteurs DMX");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	/*theApp.m_DMXEtui.ComHTMLConfig.pSession = theApp.session;
	theApp.m_DMXEtui.ComHTMLConfig.strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	theApp.m_DMXEtui.ComHTMLConfig.strServeur = ParamStrIPDMX1->value;
	theApp.m_DMXEtui.strNomJob = _T("Default");*/
//	theApp.m_DMXEtui.Initialise();//Ouverture de la connexion DMX1

	/*theApp.m_DMXNotice1.ComHTMLConfig.pSession = theApp.session;
	theApp.m_DMXNotice1.ComHTMLConfig.strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	theApp.m_DMXNotice1.ComHTMLConfig.strServeur = ParamStrIPDMX2->value;
	theApp.m_DMXNotice1.strNomJob = _T("Default");*/
//	theApp.m_DMXNotice1.Initialise();//Ouverture de la connexion DMX2

	/*theApp.m_DMXNotice2.ComHTMLConfig.pSession = theApp.session;
	theApp.m_DMXNotice2.ComHTMLConfig.strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	theApp.m_DMXNotice2.ComHTMLConfig.strServeur = ParamStrIPDMX3->value;
	theApp.m_DMXNotice2.strNomJob = _T("Default");*/
//	theApp.m_DMXNotice2.Initialise();//Ouverture de la connexion DMX3

	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Chargement Application Vision MATROX");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	//Ouverture du systeme Matrox
	if(!Application.LireFromBase(&theBase.dao_Base,theConfig.str_Machine))
	{
		AfxMessageBox(Application.sErrMsg,MB_OK|MB_ICONSTOP);
	}
	if(!Application.Charger())
	{
		AfxMessageBox(Application.sErrMsg,MB_OK|MB_ICONSTOP);
	}

	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement System[0] MATROX = Solios TRACKING");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	Systeme[0].m_iNumero = 0;
	if(!Systeme[0].LireFromBase(&theBase.dao_Base, Application.m_StrApplication))
	{
		AfxMessageBox(Systeme[0].sErrMsg,MB_OK|MB_ICONSTOP);
	}
	if(!Systeme[0].Charger())
	{
		AfxMessageBox(Systeme[0].sErrMsg,MB_OK|MB_ICONSTOP);
	}

	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement System[1] MATROX = Solios UV");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	Systeme[1].m_iNumero = 1;
	if(!Systeme[1].LireFromBase(&theBase.dao_Base, Application.m_StrApplication))
	{
		AfxMessageBox(Systeme[1].sErrMsg,MB_OK|MB_ICONSTOP);
	}
	if(!Systeme[1].Charger())
	{
		AfxMessageBox(Systeme[1].sErrMsg,MB_OK|MB_ICONSTOP);
	}

	//SYSTEME_HOST
	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement System[3] MATROX = HOST");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	Systeme[3].m_iNumero = 3;
	MsysAlloc(M_SYSTEM_HOST,M_DEFAULT,M_COMPLETE, &Systeme[3].milSys);


	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Chargement Cameras");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();


	//theCam = Caméra Trackong
	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Initialisation Camera[0] = TRACKING");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	theCam.m_iNumero = 0;
	//Association Piste/Robot
	theCam.Piste[0].m_Robot = &theApp.m_Robot1;
	theCam.Piste[1].m_Robot = &theApp.m_Robot2;
	theCam.Piste[2].m_Robot = &theApp.m_Robot3;

	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Lecture base");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	if(!theCam.LireFromBase(&theBase.dao_Base, Application.m_StrApplication, theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine))
	{
		AfxMessageBox(theCam.sErrMsg,MB_OK|MB_ICONSTOP);
	}
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	CString sTemp = _T("Fichier = ") + theCam.m_sRepertoire + _T("\\") + theCam.m_sFichier;
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, sTemp);
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	sTemp.Format(_T("NumSys = %d"), theCam.m_lNumSys);
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, sTemp);
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	if(!theCam.Charger(Systeme[theCam.m_lNumSys].milSys))
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue = 1;
	}
	else
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue = 0;
	}

	//theCamUV = Caméra UV
	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement Camera[1] = UV");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	theCamUV.m_iNumero = 1;

	if(!theCamUV.LireFromBase(&theBase.dao_Base, Application.m_StrApplication, theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine))
	{
		AfxMessageBox(theCam.sErrMsg,MB_OK|MB_ICONSTOP);
	}
	if(!theCamUV.Charger(Systeme[theCamUV.m_lNumSys].milSys))
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2].NewValue = 1;
	}
	else
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2].NewValue = 0;
	}

	//uEye[0] = Caméra BOITE 0
	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement Camera[0] = boite SUP");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	theCamUV.m_iNumero = 1;
	uEye[ID_CAM_UEYE1].m_iNumero = 2;				//N° Chargement BDD
	uEye[ID_CAM_UEYE1].m_iNumuEye = ID_CAM_UEYE1;	//Index dans le tableau uEye
	uEye[ID_CAM_UEYE1].iAdresseuEye = 1;			//(ID uEye Camera Manager)

	if(!uEye[ID_CAM_UEYE1].LireFromBase(&theBase.dao_Base, Application.m_StrApplication, theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine))
	{
		AfxMessageBox(theCam.sErrMsg,MB_OK|MB_ICONSTOP);
	}

	if (!uEye[ID_CAM_UEYE1].Charger(Systeme[uEye[ID_CAM_UEYE1].m_lNumSys].milSys))
	{
		SetTimer(theApp.UeyeView->GetSafeHwnd(),ID_TIMER_RECONNECT_UEYE1,1000,NULL);
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM3].NewValue = 1;
	}
	else
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM3].NewValue = 0;
	}
		
	//uEye[1] = Caméra BOITE 1
	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement Camera[1] = boite INF");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	uEye[ID_CAM_UEYE2].m_iNumero = 3;//N° Chargement BDD
	uEye[ID_CAM_UEYE2].m_iNumuEye = ID_CAM_UEYE2;	//Index dans le tableau uEye
	uEye[ID_CAM_UEYE2].iAdresseuEye = 2;			//(ID uEye Camera Manager)

	if(!uEye[ID_CAM_UEYE2].LireFromBase(&theBase.dao_Base, Application.m_StrApplication, theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine))
	{
		AfxMessageBox(theCam.sErrMsg,MB_OK|MB_ICONSTOP);
	}
	
	if (!uEye[ID_CAM_UEYE2].Charger(Systeme[uEye[ID_CAM_UEYE2].m_lNumSys].milSys))
	{
		SetTimer(theApp.UeyeView->GetSafeHwnd(),ID_TIMER_RECONNECT_UEYE2,1000,NULL);
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM4].NewValue = 1;
	}
	else
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM4].NewValue = 0;
	}

	//V37 Chargement VisionBoite
	//V44.4 : Ajout de commentaires
	((CFormBase*)wnd)->ListeInitialisation.InsertString(0, (LPTSTR)"Chargement Vision boite");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	if (!theApp.VisionBoite.Vision_Charger(Systeme[3].milSys))
	{
		//V37TODO : Gérer erreur
		//theApp.m_GestAlarmes.Alarme[ALARME_CAM4].NewValue = 1;
	}
	else
	{
		((CDialEtiquette* )theApp.EtiqView)->SetImageCellule();

		
		//V37TODO : Gérer erreur
		//theApp.m_GestAlarmes.Alarme[ALARME_CAM4].NewValue = 0;
	}

	//V37 Chargement Etiquette Scellement
	theApp.EtiqScellement.Etiq_strTitre = _T("Etiquette scellement");;
	theApp.EtiqScellement.Etiq_strCode = _T("NC");
	theApp.EtiqScellement.imgSource = &theApp.VisionBoite.imgSource;
	theApp.EtiqScellement.imgVisu = &theApp.VisionBoite.imgVisu;
	if (!theApp.EtiqScellement.Vision_Charger(Systeme[3].milSys))
	{
		//V37TODO : Gérer erreur
		//theApp.m_GestAlarmes.Alarme[ALARME_CAM4].NewValue = 1;
	}

	//V37 Chargement Etiquette Wolke
	theApp.EtiqWolke.Etiq_strTitre = _T("Impression boite");;
	theApp.EtiqWolke.Etiq_strCode = _T("NC");
	theApp.EtiqWolke.imgSource = &theApp.VisionBoite.imgSource;
	theApp.EtiqWolke.imgVisu = &theApp.VisionBoite.imgVisu;
	if (!theApp.EtiqWolke.Vision_Charger(Systeme[3].milSys))
	{
		//V37TODO : Gérer erreur
		//theApp.m_GestAlarmes.Alarme[ALARME_CAM4].NewValue = 1;
	}

	//V37 Pöinteur sur étiquette en cours
	theApp.EtiqTest = &theApp.EtiqScellement;

	//Initialisation des parametres cameras
	theApp.InitParamuEyes();


	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Ouverture Port Serie Camera");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();

	//Ouverture port configuration Dalsa VOIR si arret Appli pour connection
	if(theCam.Param_Camera.lNumSerial > 0)
	{
		if (!theApp.DalsaCamera1.OpenPort(theCam.Param_Camera.lNumSerial,pParamVitesseCOM->value,pParamPariteCOM->value.GetAt(0),pParamBitsDataCOM->value,pParamBitsStopCOM->value))
		{
			theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue = 1;
		}
		else
		{
			theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue = 0;
		}
	}
	else
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue = 1;
	}

	//CAM UV
	if(theCamUV.Param_Camera.lNumSerial > 0) 
	{
		if (!theApp.DalsaCamera2.OpenPort(theCamUV.Param_Camera.lNumSerial,pParamVitesseCOM->value,pParamPariteCOM->value.GetAt(0),pParamBitsDataCOM->value,pParamBitsStopCOM->value))
		{
			theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2].NewValue = 1;
		}
		else
		{
			theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2].NewValue = 0;
		}
	}
	else
	{
		theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM2].NewValue = 1;
	}

	((CFormBase*)wnd)->ListeInitialisation.InsertString(0,(LPTSTR)"Demarrage Application");
	((CFormBase*)wnd)->UpdateData(FALSE);
	((CFormBase*)wnd)->UpdateWindow();
	

	//Initialisation de l'application
	Application.Initialiser();
	//Demarre l'acquisition
	theCam.iModeAcquisition = 0;
	theCam.iSnap = 1;
	//CAM2
	theCamUV.iModeAcquisition = 0;
	theCamUV.iSnap = 1;

	//Demarre l'acquisition
	uEye[ID_CAM_UEYE1].iModeAcquisition = 1;
	uEye[ID_CAM_UEYE1].iSnap = 1;
	uEye[ID_CAM_UEYE1].Start();

	//Demarre l'acquisition
	uEye[ID_CAM_UEYE2].iModeAcquisition = 1;
	uEye[ID_CAM_UEYE2].iSnap = 1;
	uEye[ID_CAM_UEYE2].Start();

	theCam.Start();  
	
	theCamUV.lNumeroImageMaitre = &theCam.lNumeroImage;
	theCamUV.Start();

	//Initailisation Visualisation
	//theCam.imgWnd.sLibelle = _T("Camera Minims Image Wnd");
	//theCam.imgWnd.Charger(Systeme[3].milSys,theCam.lX,theCam.lYCumul,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	//theCam.imgWnd.FenetreOuvrir_WindowsHWND(theApp.VisionView->GetDlgItem(IDC_IMAGE)->GetSafeHwnd(),false);
	//MdispZoom(theCam.imgWnd.milDisp,1,1);
	//MdispControl(theCam.imgWnd.milDisp, M_OVERLAY, M_ENABLE);
	//MdispInquire(theCam.imgWnd.milDisp, M_OVERLAY_ID, &theCam.imgWnd.milOverlay);

	((CDialRobot *)theApp.RobotsView)->m_robot = &theApp.m_Robot1;

	//Compteur RAZ
	//TODO CAM2
	if(theCam.milDig > 0 ) MdigControl(theCam.milDig,M_ROTARY_ENCODER_POSITION,0);
		
	//V29
	//theCam.imgRotationZoom.FenetreOuvrir_WindowsHWND(((CDialDefautPiste * )theApp.DefautPisteView)->Image.GetSafeHwnd(),false);
	//((CDialDefautPiste * )theApp.DefautPisteView)->imgParent = &theCam.imgRotationZoom;
	//((CDialDefautPiste * )theApp.DefautPisteView)->MAJ_Scrollbars();
	theCam.imgResultat.FenetreOuvrir_WindowsHWND(((CDialDefautPiste * )theApp.DefautPisteView)->Image.GetSafeHwnd(),true);
	((CDialDefautPiste * )theApp.DefautPisteView)->imgParent = &theCam.imgResultat;
	((CDialDefautPiste * )theApp.DefautPisteView)->MAJ_Scrollbars();

	if(theApp.m_Recette.IsRecetteEnCours())
	{
		//Reprise de la recette en cours
		theApp.m_Recette.ReCharger(theApp.m_Recette.OfNum);
	}

	CString strDir = pParamDirBackup->value + _T("\\") + NOM_DOSSIER_BASE;
	theApp.PurgeDirNbJours((int) pParamNbJoursFichiersBDD->value,strDir );

	//V 45.0 DEV theApp.BackupBase();//Sauvegarde compléte de la BDD GCA V2

	//GCA V1.18 
	//Ajout evenement journal (type INFO) demarrage PC
	theApp.theJournal.Evenement(JOURNAL_TYPEINFO,_T("Demarrage App"),_T(""),_T(""));

	JParamGeneral.GetMessageParam(theApp.UeyeView->GetSafeHwnd(),MSG_PARAM_CHANGED);
	JParamVisionGeneral.GetMessageParam(theApp.UeyeView->GetSafeHwnd(),MSG_PARAM_CHANGED);
	JParamuEye[ID_CAM_UEYE1].GetMessageParam(theApp.UeyeView->GetSafeHwnd(),MSG_PARAM_CHANGED);
	JParamuEye[ID_CAM_UEYE2].GetMessageParam(theApp.UeyeView->GetSafeHwnd(),MSG_PARAM_CHANGED);

	((CDialCameras * )theApp.UeyeView)->MAJ_Grille();
	((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
	((CFormStatus*)theApp.pFormStatus)->TimerAlarme = ((CFormStatus*)theApp.pFormStatus)->SetTimer(ID_TIMER_ALARME, 500,NULL);	

	theCam.m_iModeInspection = CAM_MODE_INSPECTION;
	theCamUV.m_iModeInspection = CAM_MODE_INSPECTION;
	
	for(int iCam=0;iCam<2;iCam++)
	{
		uEye[iCam].m_iModeInspection = CAM_MODE_INSPECTION;
		uEye[iCam].AppliquerParametres();
	}

	iEtat = ETAT_PROCESS_HORSPRODUCTION;

	theApp.MAJMenuEtat();

	

	return 0;
}

int CProcess::GererEtat_ArretEnCours(int iCommande,int iCmdAuto)
{
	//on attend que les systémes soient arretes
	if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}
	return 0;
}

int CProcess::GererEtat_VidageArret(int iCommande,int iCmdAuto)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		case CMD_DEPARTCYCLE:
			//Depatr Cycle
			WBit0[INDEX_BIT_STARTCYCLE_W] = TRUE;
			break;

		default:
			break;
		}

	}
	else if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}
	return 0;
}

int CProcess::GererEtat_VidageMarche(int iCommande,int iCmdAuto)
{
	//on attend que la machine soit vide
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		case CMD_ARRETCYCLE:
			//Arret Cycle
			WBit0[INDEX_BIT_STOPCYCLE_W] = TRUE;
			break;

		default:
			break;
		}

	}
	else if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}
	return 0;
}

int CProcess::GererEtat_Vidage2(int iCommande,int iCmdAuto)
{
	//Rien a faire
	if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}
	return 0;
}

int CProcess::GererEtat_VerifEtalonnage(int iCommande,int iCmdAuto)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{
		
		case CMD_VALIDER:

			for(int i = 0; i < 6; i++)
			{
				if( ((theApp.Pesons[i].StatutEtalon1 == PESON_STATUTETALON_NOK)||
				(theApp.Pesons[i].StatutEtalon2 == PESON_STATUTETALON_NOK))
				&& (theApp.Pesons[i].PesonMode != PESON_MODEOFF))
				{
					CString strMess;
					strMess.Format(theApp.TableMessage.GetStr(MESSAGE_VALID_PESON_NOK),theApp.Pesons[i].iNumPesonBL);
					//Message
					theApp.SetEventsJournalStr(strMess);
					theApp.SetInfosString(strMess);

					//Refus de la validation
					iCommande = 0;
					theApp.MAJMenuEtat();
					return 0;
				}
			}

			//Enregistrement des valeurs
			if(((CDialValidLot * )theApp.ValidLotView)->bAllPesons)
				theApp.m_Recette.SavePoidsEtalon();
			else
				theApp.m_Recette.SavePoidsEtalonPeson(((CDialValidLot * )theApp.ValidLotView)->iPesonSelected);


			//changement Fenetre
			theApp.DialMachineEnCours = theApp.MachineView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();

			//Arret Cycle
			iEtat = iOldEtat;
			iCommande = 0;
			theApp.MAJMenuEtat();
			break;

		case CMD_ANNULER:
			//changement Fenetre
			theApp.DialMachineEnCours = theApp.MachineView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();

			//Arret Cycle
			iEtat = iOldEtat;
			iCommande = 0;
			theApp.MAJMenuEtat();
			break;

		default:
			break;
		}

	}
	else if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}

	return 0;
}
int CProcess::GererEtat_EtalonnageEtui_Arret(int iCommande,int iCmdAuto)
{
	CString sCommande;
	if(iCommande > 0)
	{
		switch(iCommande)
		{

		case CMD_DEPARTCYCLE:
			//Ecriture OPC Etat Machine
			WBit0[INDEX_BIT_STARTCYCLE_W] = TRUE;
			//V20 : Mise à jour du gain pour la caméra tracking
			sCommande.Format("ssg %ld",m_RecetteVision.pLong_CamGain->value);
			theApp.DalsaCamera1.Ecrire_commande(sCommande);
			break;

		default:
			break;
		}

	}
	else if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}

	return 0;
}
int CProcess::GererEtat_EtalonnageEtui_Marche(int iCommande,int iCmdAuto)
{
	if(iCommande > 0)
	{
		switch(iCommande)
		{

		case CMD_ARRETCYCLE:
			//Ecriture OPC Etat Machine
			WBit0[INDEX_BIT_STOPCYCLE_W] = TRUE;
			break;

		default:
			break;
		}

	}
	else if(iCmdAuto > 0)
	{
		iCmdAuto = 0;
		theApp.MAJMenuEtat();
	}

	return 0;
}

int CProcess::GererEtat_InitialisationAuto(int iCmd)
{
	CString strMess;
	CClientOPC * m_OPC;	

	CString strDir;
	CString sCommande = _T("");
	bool bReponseOK = false;

	switch(iCmd)
	{
	case 1:
		*theApp.m_Recette.pBitRazEtalons = TRUE;
		*theApp.m_Recette.pBitRecetteChargee = FALSE;//V2.44

		//Copie des Recettes
		m_OPC = theApp.m_Recette.m_OPCClient;
		theApp.m_Recette.Copy(&theApp.m_RecetteOld);
		theApp.m_RecetteNew.Copy(&theApp.m_Recette);
		theApp.m_Recette.m_OPCClient = m_OPC;

		//A VOIR MAJ articles 
		theApp.m_Recette.Article.Article1 = theApp.m_Recette.ArtNotice1.Code;
		theApp.m_Recette.Article.Article2 = theApp.m_Recette.ArtNotice2.Code;
		theApp.m_Recette.Article.Article3 = theApp.m_Recette.ArtEtui.Code;
		theApp.m_Recette.Article.Article4 = theApp.m_Recette.ArtMinims.Code;
		theApp.m_Recette.Article.Article5 = theApp.m_Recette.ArtEtiquette.Code;//GCA V1.17
		theApp.m_Recette.Article.Article6 = theApp.m_Recette.ArtEmballage.Code;//#2 Emballage

		theApp.m_Recette.Article.SourceDataRecette = &theApp.m_Recette.SourceData;
		theApp.m_Recette.ArtEtui.SourceDataRecette = &theApp.m_Recette.SourceData;
		theApp.m_Recette.ArtNotice1.SourceDataRecette = &theApp.m_Recette.SourceData;
		theApp.m_Recette.ArtNotice2.SourceDataRecette = &theApp.m_Recette.SourceData;
		theApp.m_Recette.ArtMinims.SourceDataRecette = &theApp.m_Recette.SourceData;
		theApp.m_Recette.ArtEtiquette.SourceDataRecette = &theApp.m_Recette.SourceData;//GCA V1.17
//V 44.6		theApp.m_Recette.ArtEtiqBoite.SourceDataRecette = &theApp.m_Recette.SourceData;//GCA 3.7
		theApp.m_Recette.ArtEmballage.SourceDataRecette = &theApp.m_Recette.SourceData;//#2 Emballage

		theApp.m_Recette.AjouterLot();
		
		//MAJ Statut
		theApp.m_Recette.iStatut = RECETTE_STATUT_DEBUTLOT;
		if((!theApp.m_Recette.Reprise)&&(!theApp.m_Recette.bIsEnCours))
		{
			//TODO TRACA : Refaire différemment + Vérifier le résultat
			theApp.m_Recette.AjouterArticle();
			theApp.m_Recette.Ajouter();
		}

		theApp.m_Recette.SetRecetteEnCours();//GCA V1.17

		//#2 Gestion repertoire : Création repertoire si inexistant
		strDir.Format("%s\\%s\\%s",theConfig.str_RepertoireData,NOM_DOSSIER_IMAGES,theApp.m_Recette.OfNum);
		if(!PathFileExists(strDir))
		{
			BOOL bRep = FALSE;
			bRep = CreateDirectory(strDir, NULL);
			if(bRep == FALSE)
			{
				strDir.Format("%s\\%s",theConfig.str_RepertoireData,NOM_DOSSIER_IMAGES);
			}
		}
		theApp.m_Recette.RepertoireImages = strDir;

		if(theApp.CurrentUser.IdBase > 0)
		{
			 theApp.theJournal.Clore(theApp.CurrentUser.IdBase);
			 theApp.CurrentUser.IdBase =  theApp.theJournal.Ajouter(JOURNAL_TYPELOGIN,theApp.CurrentUser.strLibelle,"Login");
		}

		switch(theApp.m_Recette.SourceData)
		{

		case RECETTE_SOURCE_CSVOLD:
			theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE,theApp.TableMessage.GetStr(MESSAGE_JOURNAL_CSVOLD),_T(""),theApp.m_Recette.UserForcage);
			break;

		case RECETTE_SOURCE_MANU:
			theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE,theApp.TableMessage.GetStr(MESSAGE_JOURNAL_RECETTEMANU),_T(""),theApp.m_Recette.UserForcage);
			break;

		default:
			break;
		}

		//V37 : Récupération systématique de ITEM_MET pour gestion impression boite
		if(!((CDialChargement * )theApp.ChargementView)->ITEM_MET_Copier())
		{
			AfxMessageBox(((CDialChargement * )theApp.ChargementView)->ITEM_MET_Copier_Erreur, MB_OK | MB_ICONERROR);
		}

		//V36 : Mise à jour de la liste des codes sachets
		if(theApp.m_Recette.Article.Type == RECETTE_TYPESACHET)
		{
			if(!((CDialChargement * )theApp.ChargementView)->CSVListeCodesSachets_Charger())
			{
				AfxMessageBox(((CDialChargement * )theApp.ChargementView)->CSVListeCodesSachets_Erreur, MB_OK | MB_ICONERROR);
			}
			else
			{
				for(int j=0;j<theApp.CB_NbCodes;j++)
				{
					theApp.SetStringListVision(theApp.CB_ListeCodes[j]);
				}
				CString xx = "";
				xx.Format("Nb Codes = %d",theApp.CB_NbCodes);
				theApp.SetStringListVision(xx);
			}
		}

		//InitOF de tous les modules Robot+Pesons+DMX
		for(int i = 0; i < 6;i++)
		{
			theApp.Pesons[i].InitOF();
		}

		theApp.PesonEtui.InitOF();

		//Init OF Robots
		theApp.m_Robot1.InitOF();
		theApp.m_Robot2.InitOF();
		theApp.m_Robot3.InitOF();

		//ETIQ Init Etiquette
		theApp.VisionBoite.InitOF();
		theApp.EtiqTest = &theApp.EtiqScellement;	//V37TODO : A tester
		theApp.EtiqScellement.InitOF();
		theApp.EtiqWolke.InitOF();

		//MAJ des Valeurs DMX (Recette->Lecteurs)
		//#3 DMX conversion en CString
		theApp.m_DMXEtui.IdDMX = theApp.m_Recette.ArtEtui.CodeDMX;
		theApp.m_DMXNotice1.IdDMX = theApp.m_Recette.ArtNotice1.CodeDMX;
		theApp.m_DMXNotice2.IdDMX = theApp.m_Recette.ArtNotice2.CodeDMX;

		//SockV4 correction inhibition lecteur si aucun code à lire
		//Anciennement effectué dans InitOF (a voir refaire la fonction si besoin de géré autre chose)
		if (theApp.m_DMXEtui.IdDMX == _T(""))
		{
			theApp.m_DMXEtui.Inhiber();
		}
		else
		{
			theApp.m_DMXEtui.Activer();//SockV4 .2 (Re-activation du lecteur si code à lire)
		}
		
		if (theApp.m_DMXNotice1.IdDMX == _T(""))
		{
			theApp.m_DMXNotice1.Inhiber();
		}
		else
		{
			theApp.m_DMXNotice1.Activer();//SockV4 .2 (Re-activation du lecteur si code à lire) //Sock V5
		}

		if (theApp.m_DMXNotice2.IdDMX == _T(""))
		{
			theApp.m_DMXNotice2.Inhiber();
		}
		else
		{
			theApp.m_DMXNotice2.Activer();//SockV4 .2 (Re-activation du lecteur si code à lire) //Sock V5
		}

//#3 DMX
		/*
		//Reset de la liste
		((CDialInitialisation*)theApp.InitialisationView )->ResetListe();
		((CDialInitialisation*)theApp.InitialisationView )->bTimeOutHappened = false;

		//Initialisation des Lecteurs DMX
		pDMX[0] = &theApp.m_DMXEtui;
		pDMX[1] = &theApp.m_DMXNotice1;
		pDMX[2] = &theApp.m_DMXNotice2;
		
		iCurentDmx = 0;*/
		iCmdAuto = 4;
		if(theConfig.str_Machine.Right(5) == "SIMUL")
		{
			//ETIQ iCmdAuto = 8;
			iCmdAuto = 90;
		}

		break;

	//case 2://Start Config DMX
	//	strMess.Format(theApp.TableMessage.GetStr(MESSAGE_DMXCONF_ENCOURS),pDMX[iCurentDmx]->iNumDMX);
	//	((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);

	//	pDMX[iCurentDmx]->ListeAffichage = &((CDialInitialisation*)theApp.InitialisationView )->ListeInit;
	//	pDMX[iCurentDmx]->InitOF(pDMX[iCurentDmx]->IdDMX);
	//	iCmdAuto = 3;
	//	break;

	//case 3://Attente fin Config DMX
	//	if(pDMX[iCurentDmx]->bFinConfig)	
	//	{
	//		iCurentDmx ++;
	//		if(iCurentDmx > 2)
	//		{
	//			iCmdAuto = 4;//on passe à la suite
	//		}
	//		else
	//		{
	//			iCmdAuto = 2;//on passe au Dmx suivant
	//		}
	//	}
	//	else
	//	{
	//		iCmdAuto = 3;
	//	}
	//	break;

	case 4:
		//Envoi Commande Test Communication camera 1
		//theApp.DalsaCamera1.Ecrire_commande(ParamCommandeTestCamera->value);
		//V17 : Affectation du gain
		sCommande.Format("ssg %ld",m_RecetteVision.pLong_CamGain->value);
		theApp.DalsaCamera1.Ecrire_commande(sCommande);
		//Message Statut
		strMess = theApp.TableMessage.GetStr(MESSAGE_CAMERA_ATTENTEREPONSE);
		((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
		theApp.SetInfosString(strMess);

		((CDialInitialisation*)theApp.InitialisationView )->StartTimerTimeout();
		iCmdAuto = 5;

		break;

	case 5://Attente/Vérification de la réponse camera		
		strMess = CString(theApp.DalsaCamera1.BuffReception);

		if(strMess.Find(_T("OK>")) != -1)
		{
			((CDialInitialisation*)theApp.InitialisationView )->StopTimerTimeout();

			//Reponse camera OK
			strMess = theApp.TableMessage.GetStr(MESSAGE_CAMERA_REPONSEOK);
			((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
			theApp.SetInfosString(strMess);

			*theApp.m_Recette.pBitRazEtalons = TRUE;

			iCmdAuto = 6;
		}
		else
		{

			if(((CDialInitialisation*)theApp.InitialisationView )->bTimeOut == true)
			{
				//Timeout 
				strMess = theApp.TableMessage.GetStr(MESSAGE_TIMEOUT);
				((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
				theApp.SetInfosString(strMess);

				iCmdAuto = 6;
			}
			else
			{
				//Reponse Camera NOK ou Camera en Erreur
				iCmdAuto = 5;//On reste à cette étape
			}
		}
		break;

	case 6:
		//Envoi Commande Test Communication camera 2
		theApp.DalsaCamera2.Ecrire_commande(ParamCommandeTestCamera->value);
		//Message Statut
		strMess = theApp.TableMessage.GetStr(MESSAGE_CAMERA_ATTENTEREPONSE);
		((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
		theApp.SetInfosString(strMess);

		((CDialInitialisation*)theApp.InitialisationView )->StartTimerTimeout();
		iCmdAuto = 7;

		break;

	case 7://Attente/Vérification de la réponse camera		
		strMess = CString(theApp.DalsaCamera2.BuffReception);

		if(strMess.Find(_T("OK>")) != -1)
		{
			((CDialInitialisation*)theApp.InitialisationView )->StopTimerTimeout();

			//Reponse camera OK
			strMess = theApp.TableMessage.GetStr(MESSAGE_CAMERA_REPONSEOK);
			((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
			theApp.SetInfosString(strMess);

			*theApp.m_Recette.pBitRazEtalons = TRUE;

			//ETIQ iCmdAuto = 8;//on passe à la suite
			iCmdAuto = 90;//on passe à la suite
		}
		else
		{

			if(((CDialInitialisation*)theApp.InitialisationView )->bTimeOut == true)
			{
				//Timeout 
				strMess = theApp.TableMessage.GetStr(MESSAGE_TIMEOUT);
				((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
				theApp.SetInfosString(strMess);

				//ETIQ iCmdAuto = 8;//on passe à la suite
				iCmdAuto = 90;//on passe à la suite
			}
			else
			{
				//Reponse Camera NOK ou Camera en Erreur
				iCmdAuto = 7;//On reste à cette étape
			}
		}
		break;

	case 90: //ETIQ Chargement des etiquettes 
		theApp.EtiqTest = &theApp.EtiqScellement;	//V37 : On repart toujours sur étiquette cellement

		theApp.EtiqScellement.Etiq_strCode = _T("NC");
		*theApp.EtiqScellement.pBitInhibLecture = FALSE;//V 44.0
		*theApp.EtiqScellement.pBitInhibImpression = FALSE;
		((CDialEtiquette*)theApp.EtiqView )->ChargerFormulaire(theApp.EtiqScellement.Etiq_strCode);

		//V 44.0
		/*if(*theApp.VisionBoite.pBitInhibitionLectureBoite == TRUE)
		{
			strMess = theApp.EtiqScellement.Etiq_strTitre + _T(" : MODULE DESACTIVE");
			((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
			theApp.VisionBoite.strEtat = "Caméras Boites : Module désactivé";

			theApp.EtiqScellement.bInhibLecture = TRUE;
			theApp.EtiqScellement.Etiq_strEtat = theApp.EtiqScellement.Etiq_strTitre + _T(" : MODULE DESACTIVE");
		}
		else
		{*/
		((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.ResetContent();
		strMess = theApp.EtiqScellement.Etiq_strTitre + _T(" : Chargement des données Eticoncept...");
		((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);

		bReponseOK = theApp.EtiqScellement.ETC_ChargerData(theApp.m_Recette.OfNum);
		if (!bReponseOK)
		{
			strMess = theApp.EtiqScellement.Etiq_strEtat;
			((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
			theApp.SetInfosString(strMess);
		}
		else
		{
			strMess.Format(_T("%s : Fichier Eticoncept chargé = %s"),theApp.EtiqScellement.Etiq_strTitre,theApp.EtiqScellement.Etiq_strCode);
			((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
			((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strMess);
			for(int i=0; i<NB_CHAMP_MAX; i++)
			{
				if(theApp.EtiqScellement.ETC_strChampBrut[i]!=_T(""))
				{
					strMess.Format(_T("%s = %s"),theApp.EtiqScellement.ETC_strChampTitre[i],theApp.EtiqScellement.ETC_strChampFormat[i]);
					((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
					((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strMess);
				}
			}
		}

		//Chargement de l'étiquette
		if(bReponseOK)
		{
			bReponseOK = theApp.EtiqScellement.LireEntete(true);
			if(!bReponseOK)
			{
				strMess = theApp.EtiqScellement.Etiq_strEtat;
				((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
				((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strMess);
				theApp.SetInfosString(strMess);
			}
		}

		//Chargement des extractions
		if(bReponseOK)
		{
			theApp.EtiqScellement.ETC_AffecterDataTo_EtiqDetail();

			strMess = theApp.EtiqScellement.Etiq_strTitre + _T(" : Chargement des Extractions...");
			((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);

			bReponseOK = theApp.EtiqScellement.Extractions_Charger();

			//V 44.1
			for (int i = 0; i < theApp.EtiqScellement.Etiq_iNbExtract; i++)
			{
				if (theApp.EtiqScellement.Extract[i].Extract_bIsOK == false)
				{
					((CDialInitialisation*)theApp.InitialisationView)->SetStringListe(theApp.EtiqScellement.Extract[i].strResult);
					((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0, theApp.EtiqScellement.Extract[i].strResult);
					bReponseOK = false;
				}
			}


			if (!bReponseOK)
			{
				strMess = theApp.EtiqScellement.Etiq_strEtat;
				((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
				((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strMess);
				theApp.SetInfosString(strMess);
			}
			else
			{
				if (theApp.EtiqScellement.Etiq_iNbExtract == 0)
				{
					theApp.EtiqScellement.Etiq_bChargementOK = false;
					strMess.Format(_T("%s : Chargement NC, Nombre extractions = %d"),theApp.EtiqScellement.Etiq_strTitre,theApp.EtiqScellement.Etiq_iNbExtract);
					((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
					((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strMess);
					theApp.SetInfosString(strMess);
				}
				else
				{

					strMess.Format(_T("%s : Chargement OK, Nombre extractions = %d"),theApp.EtiqScellement.Etiq_strTitre,theApp.EtiqScellement.Etiq_iNbExtract);
					((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
					((CDialEtiquette*)theApp.EtiqView )->ChargerFormulaire(theApp.EtiqScellement.Etiq_strCode);
				}
			}
		}
		//}
		iCmdAuto = 91;
		break;

	case 91: //V37TODO : Chargement étiquette WOLKE 

		//Initialisation par défaut
		theApp.EtiqWolke.Etiq_strCode = _T("NC");
		*theApp.EtiqWolke.pBitInhibLecture = TRUE;//V 44.0
		*theApp.EtiqWolke.pBitInhibImpression = TRUE;

		/*if (*theApp.VisionBoite.pBitInhibitionLectureBoite == TRUE)
		{
			theApp.EtiqWolke.bInhibLecture = TRUE;
			theApp.EtiqWolke.Etiq_strEtat = theApp.EtiqWolke.Etiq_strTitre + " : Module désactivé";
		}
		else
		{*/
		{
		int iReponse = -1;
		iReponse = ((CDialChargement *)theApp.ChargementView)->ITEM_MET_CodeBoite_Extract(theApp.m_Recette.Article.Code);	//V41ALL
		switch (iReponse)
		{
		case -1:	//"Fichier ITEM_MET  - Introuvable"
			theApp.EtiqWolke.Etiq_strEtat = ((CDialChargement *)theApp.ChargementView)->ITEM_MET_CodeBoite_Erreur;
			strMess = theApp.EtiqWolke.Etiq_strTitre + _T(" : ") + theApp.EtiqWolke.Etiq_strEtat;
			((CDialInitialisation*)theApp.InitialisationView)->SetStringListe(strMess);
			((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0, strMess);
			theApp.SetInfosString(strMess);
			break;

		case 1:		//Demande impression active
			*theApp.EtiqWolke.pBitInhibImpression = FALSE;				//V41ALL : On active la demande d'impression, même si aucune lecture demandée en sortie
			theApp.EtiqWolke.Etiq_strCode = pParamFormatWolke->value;
			bReponseOK = theApp.EtiqWolke.LireEntete(false);
			if (!bReponseOK)
			{
				strMess = theApp.EtiqWolke.Etiq_strEtat;
				((CDialInitialisation*)theApp.InitialisationView)->SetStringListe(strMess);
				((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0, strMess);
				theApp.SetInfosString(strMess);
			}

			if (bReponseOK)
			{
				bReponseOK = theApp.EtiqWolke.Extractions_Charger();

				//V 44.1
				for (int i = 0; i < theApp.EtiqWolke.Etiq_iNbExtract; i++)
				{
					if (theApp.EtiqWolke.Extract[i].Extract_bIsOK == false)
					{
						((CDialInitialisation*)theApp.InitialisationView)->SetStringListe(theApp.EtiqWolke.Extract[i].strResult);
						((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0, theApp.EtiqWolke.Extract[i].strResult);
						bReponseOK = false;
					}
				}

				if (!bReponseOK)
				{
					strMess = theApp.EtiqWolke.Etiq_strEtat;
					((CDialInitialisation*)theApp.InitialisationView)->SetStringListe(strMess);
					//V41ALL PLANTAGE A VOIR ??? ((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strMess.GetBuffer());
					theApp.SetInfosString(strMess);
				}
				else
				{
					if (theApp.EtiqWolke.Etiq_iNbExtract == 0)
					{
						theApp.EtiqWolke.Etiq_bChargementOK = false;
						strMess.Format(_T("%s : Chargement NC, Nombre extractions = %d"), theApp.EtiqWolke.Etiq_strTitre, theApp.EtiqWolke.Etiq_iNbExtract);
						((CDialInitialisation*)theApp.InitialisationView)->SetStringListe(strMess);
						((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0, strMess);
						theApp.SetInfosString(strMess);
					}
					else
					{
						strMess.Format(_T("%s : Chargement OK, Nombre extractions = %d"), theApp.EtiqWolke.Etiq_strTitre, theApp.EtiqWolke.Etiq_iNbExtract);
						((CDialInitialisation*)theApp.InitialisationView)->SetStringListe(strMess);
						*theApp.EtiqWolke.pBitInhibLecture = FALSE;
					}
				}
			}
			
			break;

		default:
			break;
		}
		}
		//}
		iCmdAuto = 10;
		break;


	case 10: //Allumage éclairage
		strMess.Format(_T("Eclairage des lampes"));
		((CDialInitialisation*)theApp.InitialisationView )->SetStringListe(strMess);
		
		*pBitEclairageSup = TRUE;
		*pBitEclairageUV = TRUE;

		//Eclairage étiquettes
		for(int iCam=0;iCam<2;iCam++)
		{
			uEye[iCam].AppliquerParametres();
			//V 44.0 if(*theApp.VisionBoite.pBitInhibitionLectureBoite == FALSE)
			if (*theApp.EtiqScellement.pBitInhibLecture == FALSE)
			{
				uEye[iCam].SetFlash(IO_FLASH_MODE_CONSTANT_HIGH);
			}
			else
			{
				uEye[iCam].SetFlash(IO_FLASH_MODE_CONSTANT_LOW);
			}
		}


		iCmdAuto = 8;
		break;

	case 8:
		//Verification + validation automatique si Verif paramétres Init OK
		if(Autoriser_ValidInit())
		{
			//on passe à la suite
			//en validant
			iCmdAuto = 0;
			iCmdInitOK = CMD_VALIDER;
		}
		else
		{
			//on passe à la suite
			//sans valider
			iCmdAuto = 0;
		}
		break;
	default:
		break;
	}
	return 0;
}

bool CProcess::Autoriser_ValidInit(void)
{
	CString strTemp;

	////V44.4 : Shint ACYRUS
	//CString strUserEC = theApp.CurrentUser.strCode.MakeUpper();
	//if (strUserEC == _T("ACYRUS"))
	//{
	//	return true;
	//}

	//Verification des robots Si actif
	if(theApp.m_Robot1.RobotMode != ROBOT_MODEON)
	{
		//Message
		strTemp.Format(theApp.TableMessage.GetStr(MESSAGE_INITNOK_ROBOT),theApp.m_Robot1.iNumRobot);
		theApp.SetInfosString(strTemp);

		//Refus de la validation
		return false;
	}
	if(theApp.m_Robot2.RobotMode != ROBOT_MODEON)
	{
		//Message
		strTemp.Format(theApp.TableMessage.GetStr(MESSAGE_INITNOK_ROBOT),theApp.m_Robot2.iNumRobot);
		theApp.SetInfosString(strTemp);

		//Refus de la validation
		return false;
	}
	if(theApp.m_Robot3.RobotMode != ROBOT_MODEON)
	{
		//Message
		strTemp.Format(theApp.TableMessage.GetStr(MESSAGE_INITNOK_ROBOT),theApp.m_Robot3.iNumRobot);
		theApp.SetInfosString(strTemp);

		//Refus de la validation
		return false;
	}

	//Verification DMX Si actif ou Inhibé
	/*if(theApp.m_DMXEtui.DMXMode == DMX_ON)
	{*/
		if(*theApp.m_DMXEtui.pBitInhibition == FALSE)
		{
			if (*theApp.m_DMXEtui.pBitEtatConnexion != DMX_CONNECTED)
			{
				//Message
				strTemp.Format(theApp.TableMessage.GetStr(MESSAGE_INITNOK_DMX),theApp.m_DMXEtui.iNumDMX);
				theApp.SetInfosString(strTemp);

				//Refus de la validation
				return false;
			}
		}
	/*}
	if(theApp.m_DMXNotice1.DMXMode == DMX_ON)
	{*/
		if(*theApp.m_DMXNotice1.pBitInhibition == FALSE)
		{
			if (*theApp.m_DMXNotice1.pBitEtatConnexion != DMX_CONNECTED)
			{
				//Message
				strTemp.Format(theApp.TableMessage.GetStr(MESSAGE_INITNOK_DMX),theApp.m_DMXNotice1.iNumDMX);
				theApp.SetInfosString(strTemp);

				//Refus de la validation
				return false;
			}
		}
	/*}
	if(theApp.m_DMXNotice2.DMXMode == DMX_ON)
	{*/
		if(*theApp.m_DMXNotice2.pBitInhibition == FALSE)
		{
			if (*theApp.m_DMXNotice2.pBitEtatConnexion != DMX_CONNECTED)
			{
				//Message
				strTemp.Format(theApp.TableMessage.GetStr(MESSAGE_INITNOK_DMX),theApp.m_DMXNotice2.iNumDMX);
				theApp.SetInfosString(strTemp);

				//Refus de la validation
				return false;
			}
		}
	//}

	//Verification Poids Etalons pour chaque Pesons
	for(int i= 0; i < 6; i++)
	{
	//	theApp.Pesons[i].CurrentRecette = &theApp.m_Recette;
		//Verification poids Etalons//GCA1412
		if((theApp.Pesons[i].PoidsEtalon1 != 0)||(theApp.Pesons[i].PoidsEtalon2 != 0))
		{
			//Message Erreur
			CString strMess;
			strMess.Format(theApp.TableMessage.GetStr(MESSAGE_ETALONS_NOK));
			//Message
			theApp.SetEventsJournalStr(strMess);
			theApp.SetInfosString(strMess);

			//Refus Validation 
			return false;
		}

	}
	
	//Communication Camera NOK
	if(((CDialInitialisation*)theApp.InitialisationView )->bTimeOutHappened == true)
	{
		//Message Erreur
		CString strMess;
		strMess.Format(theApp.TableMessage.GetStr(MESSAGE_CAMERA_TIMEOUT));
		//Message
		theApp.SetEventsJournalStr(strMess);
		theApp.SetInfosString(strMess);

		//Refus Validation
		return false;
	}

	//ETIQ
	//V 44.0 	if(theApp.EtiqScellement.bInhibLecture == FALSE)
	if (*theApp.EtiqScellement.pBitInhibLecture == FALSE)
	{
		if(theApp.EtiqScellement.Etiq_bChargementOK == false)
		{
			//Message
			theApp.SetInfosString(theApp.EtiqScellement.Etiq_strEtat);
			//Refus de la validation
			return false;
		}
	}

	//V37TODO : vérifier theApp.EtiqWolke.Etiq_bChargementOK
	//V 44.0 	if(theApp.EtiqWolke.bInhibLecture == FALSE)
	if (*theApp.EtiqWolke.pBitInhibLecture == FALSE)
	{
		if(theApp.EtiqWolke.Etiq_bChargementOK == false)
		{
			//Message
			theApp.SetInfosString(theApp.EtiqWolke.Etiq_strEtat);
			//Refus de la validation
			return false;
		}
	}

	//Message Statut si Verifications OK
	theApp.SetInfosString(theApp.TableMessage.GetStr(MESSAGE_INIT_OK));
	return true;
	
}
void CProcess::Gerer_Shutdown()
{
	
	// Validation Fin OF
	theApp.m_Recette.iStatut = RECETTE_STATUT_FINOF;
	theApp.m_Recette.Update();

	theApp.m_Recette.SetRecetteEnCours();

	if(theApp.CurrentUser.IdBase > 0)
	{
		 theApp.theJournal.Clore(theApp.CurrentUser.IdBase);
		 theApp.CurrentUser.IdBase =  theApp.theJournal.Ajouter(JOURNAL_TYPELOGIN,theApp.CurrentUser.strLibelle, _T("Login"));
	}

	theApp.BackupBase();//Sauvegarde compléte de la BDD

	// Reset RecetteNew
	theApp.m_RecetteNew.Reset();
	
	iEtat = ETAT_PROCESS_HORSPRODUCTION;

	*theApp.m_Recette.pBitRazEtalons = TRUE;
		
	*pBitEclairageSup = FALSE;
	*pBitEclairageUV = FALSE;

	theApp.MAJMenuEtat();

	//envoi commande arret application
	AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND,SC_CLOSE,NULL);

	//Envoi commande arret de l'ordinateur
	::ShellExecute(NULL,_T("open"),"cmd.exe"," /C shutdown -s -t 60","C:\\",SW_SHOW);
}

void CProcess::Gerer_ImpressionAutoFinOF(CString sOF)
{
	if(pParamImpressionType->value == 0)	return;	//Pas d'impression automatique
	Gerer_Impression(sOF,pParamImpressionType->value);
}

void CProcess::Gerer_Impression(CString sOF, long lOption)
{
		CStringA strA;
		CStringA requete;
		CStringA File;
		strA = sOF;

		//Previsu
		if(lOption == PREVISU)
		{
			//ACYCL
			System::String^ sConnect = gcnew System::String (theConfig.str_NomBase);
			System::String^ sRepertoire = gcnew System::String (theConfig.str_RepertoireBase);

			System::String^ sCoupeLocale = gcnew System::String (strA);



			gcroot<clsMain^> m_Export;
			m_Export = gcnew clsMain();
			m_Export->Init(sConnect,sRepertoire,"","");
			m_Export->LoadForm("Coupes", "C_ENT", "Phase de Production", false, false, false, false, true, true, true, false);
			m_Export->SetFormMenu(false,true,true);

			m_Export->cfrm->Data_Trouver(sCoupeLocale,false,false);
			m_Export->cfrm->Data_Lire();

			m_Export->cfrm->Visualiser();
			strA.ReleaseBuffer();
			return ;
		}
		
		//Recuperation ID OF selectionné
		CADORecordset rsPrint (&theBase.dao_Base);
		long IDPrint = 0;
		if(ACBaseStrRsOpen(strA,"Numero","Coupes",&rsPrint))
		{
			//Recuperation ID article 
			IDPrint = ACBaseGetLong("ID",&rsPrint);
		}

		requete.Format("{Coupes.ID}=%ld",IDPrint);


		if((lOption == PDF) || (lOption == EXCEL))
		{
			strA.Format("%s\\%s",pParamDossierExport->value,sOF);
			if(lOption == PDF)//2 = PDF 1 = IMPRESSION 3 = EXCEL
				File.Format("PDF/%s.pdf", strA);
			else
				File.Format("XLS/%s.xls", strA);
		}
	
		System::String^ sConnect = gcnew System::String (theConfig.str_NomBase);
		System::String^ sRepertoire = gcnew System::String (theConfig.str_RepertoireBase);

		System::String^ sRequest = gcnew System::String (requete);
		System::String^ sFile = gcnew System::String (File);

		gcroot<clsMain^> m_Export;
		//ETIQ : modification lancement impression
		strA = sOF;
		System::String^ sCoupeLocale = gcnew System::String (strA);
		m_Export = gcnew clsMain();
		m_Export->Init(sConnect,sRepertoire,"","");
		m_Export->LoadForm("Coupes", "C_ENT", "Phase de Production", false, false, false, false, false, true, true, false);
		m_Export->cfrm->Data_Trouver(sCoupeLocale,false,false);
		
		m_Export->cfrm->Data_Lire();
		
		m_Export->cfrm->Report_SetData(0, sFile, 0, 0,"Default"); 
		m_Export->cfrm->Report_Imprimer(0);

		//m_Export->cfrm->Report_SetData(0, "", 0, 0,"Default"); 
		//m_Export->cfrm->Report_Imprimer(0);

		//m_Export->cfrm->Report_SetData(0, "", 0, 0,"PDF Complete"); 
		//m_Export->cfrm->Report_Imprimer(0);

		//m_Export->cfrm->Report_Imprimer2("Coupes","C_ENT","Phase de production",0,sFile,0,0);

		strA.ReleaseBuffer();
}

void CProcess::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	if(strFonction == _T("PROCESS_START"))
	{
		// Verification User != AUCUN
		if(theApp.CurrentUser.strCode != CODE_UTILISATEUR_DEFAUT)
		{
			theApp.SetInfosString("");	//V21 : Anuulation des messages
			Gerer(CMD_DEPARTCYCLE);
			theApp.MAJMenu();
		}
	}
	else if(strFonction == _T("PROCESS_STOP"))
	{
		Gerer(CMD_ARRETCYCLE);

		theApp.MAJMenu();
	}
	else if(strFonction == _T("PROCESS_VALIDER"))
	{
		theApp.SetInfosString(_T(""));	//V21 : Anuulation des messages
		Gerer(CMD_VALIDER);
		theApp.MAJMenu();
	}
	else if(strFonction == _T("PROCESS_ANNULER"))
	{
		Gerer(CMD_ANNULER);

		theApp.MAJMenu();
	}
	//GCA 1412
	else if(strFonction == _T("PROCESS_VIDAGE_START"))
	{
		CString strMess;
		if(*pBitMaintenance == TRUE)
		{
			strMess = theApp.TableMessage.GetStr(MESSAGE_VIDAGE_NOKMAINTENANCE);
			theApp.SetEventsJournalStr(strMess);
			AfxMessageBox(strMess, MB_OK | MB_ICONWARNING);

			return ;
		}
		if(iEtat != ETAT_PROCESS_PRODUCTIONARRET)
		{
			strMess = theApp.TableMessage.GetStr(MESSAGE_VIDAGE_NOK);
			theApp.SetEventsJournalStr(strMess);
			AfxMessageBox(strMess, MB_OK | MB_ICONWARNING);
			return ;
		}
		else
		{
			iOldEtat = iEtat;
			iEtat = ETAT_PROCESS_VIDAGEARRET;

			EtatMachine = ETATMACHINE_CYCLEAUTOVIDAGE;	
			
			*pBitVidage = TRUE;//Mode Vidage
			*pBitVidage2 = FALSE;//Mode Vidage2 //V2.42
			*pBitMaintenance = FALSE;
			//V24 *pBitAuto = FALSE;
			*pBitEtalonnageEtui = FALSE;

			//Changement etat bouton
			ParentBtn->iFonction = 1;
			((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();	
		}

	}
	else if(strFonction == _T("PROCESS_VIDAGE_STOP"))
	{
		//Si le mode vidage est toujours en cours on fait rien
		if(RBit3[INDEX_BIT_STATUSPROD_R] != TRUE)
		{
			*pBitVidage = FALSE;
			*pBitVidage2 = FALSE;	//V2.42
			*pBitMaintenance = FALSE;
			//V24 *pBitAuto = TRUE;
			*pBitEtalonnageEtui = FALSE;

			iEtat = ETAT_PROCESS_PRODUCTIONARRET;
			
			//Changement etat bouton
			ParentBtn->iFonction = 0;
			((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();

			
			if(theApp.m_Recette.IsLoaded)
			{
				iEtat = ETAT_PROCESS_PRODUCTIONARRET;
				*pBitAuto = TRUE;
			}
			else
			{
				iEtat = ETAT_PROCESS_HORSPRODUCTION;
				*pBitAuto = FALSE;
			}

			theApp.MAJMenuEtat();

		}
		else
		{
			AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_ATTENTE_CYCLEENCOURS),MB_OK|MB_ICONINFORMATION);
			return ;
		}

	}
	//V2.42 : Ajout VIDAGE2
	else if(strFonction == _T("PROCESS_VIDAGE2_START"))
	{
		CString strMess;
		if(*pBitMaintenance == TRUE)
		{
			strMess = theApp.TableMessage.GetStr(MESSAGE_VIDAGE_NOKMAINTENANCE);
			theApp.SetEventsJournalStr(strMess);
			AfxMessageBox(strMess, MB_OK | MB_ICONWARNING);

			return ;
		}
		//V2.53 if(theApp.iEtat != ETAT_PROCESS_PRODUCTIONARRET)
		//Ajout de la possibilité de lancer le cycle Hors Production
		if((iEtat == ETAT_PROCESS_PRODUCTIONARRET) ||(iEtat == ETAT_PROCESS_HORSPRODUCTION))
		{
			iOldEtat = iEtat;
			iEtat = ETAT_PROCESS_VIDAGE2;

			EtatMachine = ETATMACHINE_CYCLEAUTOVIDAGE;	
			
			*pBitVidage = FALSE;//Mode Vidage
			*pBitVidage2 = TRUE;//Mode Vidage //V2.42
			*pBitMaintenance = FALSE;
			//V24 *pBitAuto = FALSE;
			*pBitEtalonnageEtui = FALSE;

			//Changement etat bouton
			ParentBtn->iFonction = 1;
			((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();	

			theApp.MAJMenuEtat();
		}
		else
		{
			strMess = theApp.TableMessage.GetStr(MESSAGE_VIDAGE_NOK);
			theApp.SetEventsJournalStr(strMess);
			AfxMessageBox(strMess, MB_OK | MB_ICONWARNING);
			return ;
		}
	}
	else if(strFonction == _T("PROCESS_VIDAGE2_STOP"))
	{
		if(iEtat != ETAT_PROCESS_VIDAGE2)
		{
			return ;
		}
		*pBitVidage = FALSE;
		*pBitVidage2 = FALSE;	//V2.42
		*pBitMaintenance = FALSE;
		*pBitEtalonnageEtui = FALSE;
		//V2.53 : Récupération ancien état
		//V24 if(iOldEtat == ETAT_PROCESS_PRODUCTIONARRET)	*pBitAuto = TRUE;	
		//V24 else *pBitAuto = FALSE;
		iEtat = iOldEtat;
		
		//Changement etat bouton
		ParentBtn->iFonction = 0;
		((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();

		theApp.MAJMenuEtat();
	}
	else if ((strFonction == _T("PROCESS_ACQMIXUP"))//V 42 Correction Acquittement MIXUP
		||(strFonction == _T("PROCESS_ACQDEFAUT")))
	{
		Defauts_Acquitement();
		theApp.SetInfosString("");	//V24 : Anuulation des messages
	}
	else if(strFonction == _T("PROCESS_AFF_INFOSRECETTE"))
	{
		//Affichage de la fenêtre d'information de la recette
		CDialAffGridRecette RecetteView(NULL,&theApp.m_Recette);

		RecetteView.DoModal();//AVOIR si Modal pose pblm
	}
	else if(strFonction == _T("PROCESS_IMPRIMER"))
	{
		//V 45.0
		((CSplitFrame *)AfxGetMainWnd())->SetFormulaireView();

		/*theApp.DialMachineEnCours = theApp.DialFormulaire;
		((CSplitFrame *)AfxGetMainWnd())->OnViewMachine();*/


		//Fenetre Selection impression rapport
		/*CDialSelectPrint frameSelect;
		frameSelect.strOfEnCours = theApp.m_Recette.OfNum;
		if(frameSelect.DoModal()!= IDOK)
		{
			return ;
		}
		Gerer_Impression(frameSelect.strFrameOF,frameSelect.iExport);*/
	}
	else if(strFonction == _T("PROCESS_PRELEVEMENT"))
	{
		if(!theApp.m_Recette.IsLoaded)
		{
			theApp.SetInfosString("Fonction non valide hors production");
			return ;
		}

		long NbPrelev = 0;
		//Demande du nombre de prelevement
		CDiagPadNum FrmSaisiePrelev;
		CString strMessage;
		
		FrmSaisiePrelev.strTitre = theApp.TableMessage.GetStr(MESSAGE_PRELEV_TITRE);
		FrmSaisiePrelev.strInfos = theApp.TableMessage.GetStr(MESSAGE_PRELEV_TITRE);

		if(FrmSaisiePrelev.DoModal() == IDOK)
		{
			NbPrelev = atoi(FrmSaisiePrelev.strSaisie);
			strMessage.Format(theApp.TableMessage.GetStr(MESSAGE_PRELEV_TRACA_F),atoi(FrmSaisiePrelev.strSaisie));
			//Enregistrement dans la base
			#pragma warning (disable: 4995)
			theApp.theJournal.Evenement(JOURNAL_TYPEINFO, _T("PRELEVEMENT"),strMessage,theApp.CurrentUser.strCode);
			#pragma warning (default: 4995)
			//Mise à jour des compteurs de la recette en cours
			theApp.m_Recette.NbBoitePrelev[1] = theApp.m_Recette.NbBoitePrelev[1] + NbPrelev;
		}
	}
	//V 42 Correction Acquittement MIXUP
	//else if(strFonction == "PROCESS_ACQMIXUP")
	//{
	//	//Demande Login Manager
	//	CString strUserMixup,strDateMixup;

	//	if(theApp.DemandeLoginManager(strUserMixup,strDateMixup))
	//	{
	//		CString strCom;
	//		CString strErr;
	//		CString strDef;
	//		strCom = _T("");

	//		//Demander si ajout de commentaires
	//		if(AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_ASK_AJOUTCOM_MIXUP),MB_YESNO,MB_ICONQUESTION) == IDYES)
	//		{
	//			
	//			//Ajout commentaire
	//			CDiagPadAlpha frmSaisie;

	//			frmSaisie.strTitre = _T("MIXUP");
	//			frmSaisie.strInfos = _T("Commentaire/observation MIXUP");
	//			frmSaisie.strSaisie = _T("");

	//			if (frmSaisie.DoModal() == IDOK)
	//			strCom = frmSaisie.strSaisie;

	//			//Ajout evenement Journal
	//			theApp.theJournal.Evenement(JOURNAL_TYPEMIXUP, "Commentaire", strCom, strUserMixup);
	//		}

	//		*pBitAcquitDefaut = TRUE;//Acquittement Defauts
	//	}
	//	else
	//	{
	//		//Erreur Mot de passe ou User Non manager
	//		return;
	//	}
	//}
	else if (strFonction == _T("PROCESS_ETALETUI_START"))
	{
		if (*theApp.PesonEtui.pBitInhibition == TRUE)
		{
			return;
		}

		CString strMess;
		if (*pBitMaintenance == TRUE)
		{
			strMess = theApp.TableMessage.GetStr(MESSAGE_MAINTENANCE_NOK);
			theApp.SetEventsJournalStr(strMess);
			AfxMessageBox(strMess, MB_OK|MB_ICONWARNING);
			return;
		}

		//V22
		if (*pBitMachineVide == FALSE)
		{
			strMess = _T("Machine non vide, Action refusée");
			AfxMessageBox(strMess, MB_OK|MB_ICONWARNING);
			return;
		}

		if (iEtat == ETAT_PROCESS_PRODUCTIONARRET)
		{
			iOldEtat = iEtat;
			iEtat = ETAT_PROCESS_ETALONNAGE_ETUI_ARRET;

			EtatMachine = ETATMACHINE_CYCLEAUTOETUI;

			*pBitVidage = FALSE;//Mode Vidage
			*pBitVidage2 = FALSE;//Mode Vidage //V2.42
			*pBitMaintenance = FALSE;

			//V22 : on ne touche pas ce bit en production
			//*pBitAuto = TRUE;//V44 pour Automate Auto quand Etalonnage Etui

			*pBitEtalonnageEtui = TRUE;

			//Changement etat bouton
			ParentBtn->iFonction = 1;
			((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();

			theApp.MAJMenuEtat();
		}
		else
		{
			strMess = theApp.TableMessage.GetStr(MESSAGE_PRODUCTIONARRET_NOK);
			theApp.SetEventsJournalStr(strMess);
			AfxMessageBox(strMess, MB_OK|MB_ICONWARNING);
			return;
		}
	}
	else if (strFonction == _T("PROCESS_ETALETUI_STOP"))
	{

		if ((iEtat != ETAT_PROCESS_ETALONNAGE_ETUI_ARRET) && (iEtat != ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE))
		{
			return;
		}

		*pBitVidage = FALSE;
		*pBitVidage2 = FALSE;	//V2.42
		*pBitMaintenance = FALSE;
		*pBitEtalonnageEtui = FALSE;

		//V2.53 : Récupération ancien état
		//V22 : Annulation car trop dangereux : risque de perdre le bit auto
		//if(iOldEtat == ETAT_PROCESS_PRODUCTIONARRET)	*pBitAuto = TRUE;	
		//else *pBitAuto = FALSE;
		//iEtat = iOldEtat;
		iEtat = ETAT_PROCESS_PRODUCTIONARRET;

		//Changement etat bouton
		ParentBtn->iFonction = 0;
		((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();

		theApp.MAJMenuEtat();
	}
}
void CProcess::Defauts_Acquitement()
{
	//V 42 Correction acuittement mixup en passant par dialAlarme.
	if(iEtat == ETAT_PROCESS_MIXUP)
	{
		//Demande Login Manager
		CString strUserMixup, strDateMixup;

		if (theApp.DemandeLoginManager(strUserMixup, strDateMixup))
		{
			CString strCom;
			CString strErr;
			CString strDef;
			strCom = _T("");

			//Demander si ajout de commentaires
			if (AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_ASK_AJOUTCOM_MIXUP), MB_YESNO, MB_ICONQUESTION) == IDYES)
			{

				//Ajout commentaire
				CDiagPadAlpha frmSaisie;

				frmSaisie.strTitre = _T("MIXUP");
				frmSaisie.strInfos = _T("Commentaire/observation MIXUP");
				frmSaisie.strSaisie = _T("");

				if (frmSaisie.DoModal() == IDOK)
					strCom = frmSaisie.strSaisie;

				//Ajout evenement Journal
				theApp.theJournal.Evenement(JOURNAL_TYPEMIXUP, "Commentaire", strCom, strUserMixup);
			}

		}
		else
		{
			//Erreur Mot de passe ou User Non manager
			return;
		}
	}

	*pBitAcquitDefaut = TRUE;//Acquittement Defauts

	//V 45.0
	//Acquittement des alarmes cadence si une active
	if ((theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT1].NewValue > 0)||
	(theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT2].NewValue> 0) ||
	(theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT3].NewValue > 0) ||
	(theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_Machine].NewValue > 0) )
	{
		Cadence_AcquitAlarmes();
	}

}
//V 44.0
void CProcess::Get_MaxAdresses_DB30X()
{
	// Lecture des adresses max déclarées dans la bdd pour chaque DB alarmes (301/302/303)

	/*SELECT TOP(1) Adresse As AdresseMax
		FROM   Alarmes WHERE DB = 301 ORDER BY Adresse DESC

		SELECT TOP(1) Adresse As AdresseMax
		FROM   Alarmes WHERE DB = 302 ORDER BY Adresse DESC

		SELECT TOP(1) Adresse As AdresseMax
		FROM   Alarmes WHERE DB = 303 ORDER BY Adresse DESC*/

	Def301_AdresseMax = 0;
	Def302_AdresseMax = 0;
	Def303_AdresseMax = 0;
	CString strRequete = _T("SELECT TOP(1) Adresse As AdresseMax FROM Alarmes WHERE DB = 301 ORDER BY Adresse DESC");
	CADORecordset rsAdressesMax(&theBase.dao_Base);
	try
	{
		rsAdressesMax.Open( strRequete);

		if (rsAdressesMax.IsOpen())
		{
			Def301_AdresseMax = ACBaseGetInt(_T("AdresseMax"), &rsAdressesMax);
		}

		rsAdressesMax.Close();

		strRequete = _T("SELECT TOP(1) Adresse As AdresseMax FROM Alarmes WHERE DB = 302 ORDER BY Adresse DESC");
		rsAdressesMax.Open(strRequete);

		if (rsAdressesMax.IsOpen())
		{
			Def302_AdresseMax = ACBaseGetInt(_T("AdresseMax"), &rsAdressesMax);
		}

		rsAdressesMax.Close();

		strRequete = _T("SELECT TOP(1) Adresse As AdresseMax FROM Alarmes WHERE DB = 303 ORDER BY Adresse DESC");
		rsAdressesMax.Open(strRequete);

		if (rsAdressesMax.IsOpen())
		{
			Def303_AdresseMax = ACBaseGetInt(_T("AdresseMax"), &rsAdressesMax);
		}

		rsAdressesMax.Close();

	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause, 255);
		e->Delete();
		CString strErreur = _T("ERREUR CProcess::Get_MaxAdresses_DB30X\r\nDescription :\n");
		strErreur = strErreur + szCause;
	
	}
}
//V 44.7
void CProcess::Cadence_CalculerMoy()
{
	//V 45.0 calcul du nombre de valeur à prendre en compte pour moyenne cadence
	iCadence_NbValCalcul = (int)(pParamCadence_Periode->value * 6);

	dTab_CadenceMachine[iModulo_CadenceMachine] = dCadence;

	iModulo_CadenceMachine++;
	if (iModulo_CadenceMachine == iCadence_NbValCalcul)
	{
		iModulo_CadenceMachine = 0;
	}

	iNbMoy_CadenceMachine++;
	if (iNbMoy_CadenceMachine >= iCadence_NbValCalcul)
	{
		iNbMoy_CadenceMachine = iCadence_NbValCalcul;
	}

	dMoy_CadenceMachine = 0;
	for (int i = 0; i < iNbMoy_CadenceMachine; i++)
	{
		dMoy_CadenceMachine = dMoy_CadenceMachine + dTab_CadenceMachine[i];
	}
	dMoy_CadenceMachine = dMoy_CadenceMachine / iNbMoy_CadenceMachine;
}

//V 44.8
void CProcess::Cadence_CalculerRobots()
{
	//V 45.0 calcul du nombre de valeur à prendre en compte pour moyenne cadence
	iCadence_NbValCalcul = (int)(pParamCadence_Periode->value * 6);

	//Calcul des la cadence des pièces vues par piste
	for (int iRobot = 0; iRobot < 3; iRobot++)
	{
		CPiste * pPiste = NULL;
		switch (iRobot)
		{
		case 0:
		default:
			pPiste = &theCam.Piste[0];
			break;

		case 1:
			pPiste = &theCam.Piste[1];
			break;

		case 2:
			pPiste = &theCam.Piste[2];
			break;
		}

		/*long lTotalPrises = 0;

		lTotalPrises = pPiste->lCountVision_OK;

		dCadence_Vues[iRobot] = (lTotalPrises - lLastNbVus[iRobot]) * 60;

		lLastNbVus[iRobot] = lTotalPrises;*/

	/*	dTab_CadenceTapis[iRobot][iModulo_CadenceTapis[iRobot]] = (double) pPiste->lCountVision_OK;

		iModulo_CadenceTapis[iRobot]++;
		if (iModulo_CadenceTapis[iRobot] == iCadence_NbValCalcul)
		{
			iModulo_CadenceTapis[iRobot] = 0;
		}

		iNbMoy_CadenceTapis[iRobot]++;
		if (iNbMoy_CadenceTapis[iRobot] >= iCadence_NbValCalcul)
		{
			iNbMoy_CadenceTapis[iRobot] = iCadence_NbValCalcul;
		}

		dMoy_CadenceTapis[iRobot] = 0;
		for (int i = 1; i < iNbMoy_CadenceTapis[iRobot]; i++)
		{
			double dDiff = dTab_CadenceTapis[iRobot][i] - dTab_CadenceTapis[iRobot][i - 1];
			dMoy_CadenceTapis[iRobot] = dMoy_CadenceTapis[iRobot] + dDiff;
		}

		dMoy_CadenceTapis[iRobot] = (dMoy_CadenceTapis[iRobot] / iNbMoy_CadenceTapis[iRobot]) * 6;*/

		CadenceTapis[iRobot].iModulo_Cadence++;
		if (CadenceTapis[iRobot].iModulo_Cadence == iCadence_NbValCalcul)
		{
			CadenceTapis[iRobot].iModulo_Cadence = 0;
		}
		CadenceTapis[iRobot].dMoy_Cadence = (double)pPiste->lCountVision_OK - CadenceTapis[iRobot].dTab_Cadence[CadenceTapis[iRobot].iModulo_Cadence];
		CadenceTapis[iRobot].dMoy_Cadence = CadenceTapis[iRobot].dMoy_Cadence / pParamCadence_Periode->value;
		CadenceTapis[iRobot].dTab_Cadence[CadenceTapis[iRobot].iModulo_Cadence] = (double)pPiste->lCountVision_OK;
	}

	//calcul de la cadence des pièces prises par piste ( = nombre de mesures effectuées sur les pesons)
	for (int iRobot = 0; iRobot < 3; iRobot++)
	{
		CRobot * pRobot = NULL;
		switch (iRobot)
		{
		case 0:
		default:
			pRobot = &theApp.m_Robot1;
			break;

		case 1:
			pRobot = &theApp.m_Robot2;
			break;

		case 2:
			pRobot = &theApp.m_Robot3;
			break;
		}

		/*long lTotalPrises = 0;

		lTotalPrises = pRobot->peson[0]->lNbMesuresEffectuees + pRobot->peson[1]->lNbMesuresEffectuees;

		dCadence_Prise[iRobot] = (lTotalPrises - lLastNbPris[iRobot]) * 60;

		lLastNbPris[iRobot] = lTotalPrises;*/

		//faire difference avec la valeur précédente puis faire moy sur periode
		//dTab_CadencePrises[iRobot][iModulo_CadencePrises[iRobot]] = (double)(pRobot->peson[0]->lNbMesuresEffectuees + pRobot->peson[1]->lNbMesuresEffectuees);

		//iModulo_CadencePrises[iRobot]++;
		//if (iModulo_CadencePrises[iRobot] == iCadence_NbValCalcul)
		//{
		//	iModulo_CadencePrises[iRobot] = 0;
		//}

		//iNbMoy_CadencePrises[iRobot]++;
		//if (iNbMoy_CadencePrises[iRobot] >= iCadence_NbValCalcul)
		//{
		//	iNbMoy_CadencePrises[iRobot] = iCadence_NbValCalcul;
		//}

		//dMoy_CadencePrises[iRobot] = 0;
		//for (int i = 1; i < iNbMoy_CadencePrises[iRobot]; i++)
		//{
		//	double dDiff = dTab_CadencePrises[iRobot][i] - dTab_CadencePrises[iRobot][i - 1];
		//	dMoy_CadencePrises[iRobot] = dMoy_CadencePrises[iRobot] + dDiff;
		//}

		//dMoy_CadencePrises[iRobot] = (dMoy_CadencePrises[iRobot] / iNbMoy_CadencePrises[iRobot]) * 6;//lecture toutes les 10 secondes

		CadencePrises[iRobot].iModulo_Cadence++;
		if (CadencePrises[iRobot].iModulo_Cadence == iCadence_NbValCalcul)
		{
			CadencePrises[iRobot].iModulo_Cadence = 0;
		}
		CadencePrises[iRobot].dMoy_Cadence = (double)(pRobot->peson[0]->lNbMesuresEffectuees + pRobot->peson[1]->lNbMesuresEffectuees) - CadencePrises[iRobot].dTab_Cadence[CadencePrises[iRobot].iModulo_Cadence];
		CadencePrises[iRobot].dMoy_Cadence = CadencePrises[iRobot].dMoy_Cadence / pParamCadence_Periode->value;
		CadencePrises[iRobot].dTab_Cadence[CadencePrises[iRobot].iModulo_Cadence] = (double)(pRobot->peson[0]->lNbMesuresEffectuees + pRobot->peson[1]->lNbMesuresEffectuees);

	}

	////Calcul du ratio de prise
	//for (int iRobot = 0; iRobot < 3; iRobot++)
	//{
	//	dCadence_Ratio[iRobot] = (dCadence_Prise[iRobot] / dCadence_Vues[iRobot]) * 100.;
	//}

	
}

void CProcess::Cadence_AcquitAlarmes()
{
	//RAZ des alarmes associées
	theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT1].NewValue = 0;
	theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT2].NewValue = 0;
	theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT3].NewValue = 0;
	theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_Machine].NewValue = 0;

	iModeCadence = 0;
	ulTickDebutControle = (ULONGLONG)( GetTickCount64() + (pParamCadence_Periode->value * 60 * 1000));

	for (int i = 0; i < 3; i++)
	{
		CadenceTapis[i].ulTickTimeoutAcquit = 0;
		CadencePrises[i].ulTickTimeoutAcquit = 0;
	}
	

}
void CProcess::Cadence_Gerer()
{

	//V 45.0
	//changement couleur cadence si < seuil
	if (pParamCadence_MinMachine->value > 0)
	{
		if ((theApp.Process.dMoy_CadenceMachine < pParamCadence_MinMachine->value) && (theApp.Process.iEtat == ETAT_PROCESS_PRODUCTIONMARCHE))
		{
			if (iIndex_ALARME_Cadence_Machine >= 0)
			{
				theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_Machine].NewValue = 1;
			}

			((CDialMachine*)theApp.MachineView)->TextCadence.CouleurText = RGB(255, 0, 0);
		}
		else
		{
			//theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_Machine].NewValue = 0; // A VOIR RAZ alamre auto ou Acquittement manuel
			if (iIndex_ALARME_Cadence_Machine >= 0)
			{
				theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_Machine].NewValue = 0;
			}

			((CDialMachine*)theApp.MachineView)->TextCadence.CouleurText = RGB(0, 255, 0);
		}
	}

	switch (iModeCadence)
	{
	case 0://Attente en debut de lot
	default:
		if (ulTickDebutControle < GetTickCount64())
		{
			iModeCadence = 1;
		}

		break;

	case 1:
	{
		//V 44.9 declenchement des alertes
		for (int iRobot = 0; iRobot < 3; iRobot++)
		{
			//Prod en cours, declecnchement des alarmes si la cadence "tombe"
			switch (iRobot)
			{
			case 0:
			default:
				if (iIndex_ALARME_Cadence_ROBOT1 < 0)
				{
					break;
				}

				if (CadencePrises[iRobot].dMoy_Cadence < pParamCadence_MinRobot->value)
				{
					CadencePrises[iRobot].ulTickTimeoutAcquit = (ULONGLONG)(GetTickCount64() + (pParamCadence_Timeout->value * 60 * 1000));
					theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT1].NewValue = 1;
				}
				else
				{
					theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT1].NewValue = 0;
				}
				break;

			case 1:
				if (iIndex_ALARME_Cadence_ROBOT2 < 0)
				{
					break;
				}

				if (CadencePrises[iRobot].dMoy_Cadence < pParamCadence_MinRobot->value)
				{
					CadencePrises[iRobot].ulTickTimeoutAcquit = (ULONGLONG)(GetTickCount64() + (pParamCadence_Timeout->value * 60 * 1000));
					theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT2].NewValue = 1;
				}
				else
				{
					theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT2].NewValue = 0;
				}
				break;

			case 2:
				if (iIndex_ALARME_Cadence_ROBOT3 < 0)
				{
					break;
				}

				if (CadencePrises[iRobot].dMoy_Cadence < pParamCadence_MinRobot->value)
				{
					CadencePrises[iRobot].ulTickTimeoutAcquit = (ULONGLONG)(GetTickCount64() + (pParamCadence_Timeout->value * 60 * 1000));
					theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT3].NewValue = 1;
				}
				else
				{
					theApp.m_GestAlarmes.Alarme[iIndex_ALARME_Cadence_ROBOT3].NewValue = 0;
				}
				break;
			}
			
			//alarme toujours activée, on vérifie que le timeout acquittement ne soit pas depassé
			if (GetTickCount64() < CadencePrises[iRobot].ulTickTimeoutAcquit)
			{
				//Timeout depassé => demande d'arret machine
				//Arret Cycle
				WBit0[INDEX_BIT_STOPCYCLE_W] = TRUE;
				CadencePrises[iRobot].ulTickTimeoutAcquit = 0;
			}

		}

	}
		break;

	}
			
}

void CProcess::Cadence_Reset()
{
	//V 45.0 RAZ des valeurs calculées pour la cadence Robot
	for (int i = 0; i < NB_VAL_CADENCE_MAX; i++)
	{
		dTab_CadenceMachine[i] = 0;
	}
	iModulo_CadenceMachine = 0;
	iNbMoy_CadenceMachine = 0;
	dMoy_CadenceMachine = 0;

	for (int i = 0; i < 3; i++)
	{
		CadencePrises[i].dMoy_Cadence = 0;
		CadencePrises[i].ulTickTimeoutAcquit = 0;
		CadencePrises[i].iModulo_Cadence = 0;

		for (int y = 0; y < NB_VAL_CADENCE_MAX; y++)
		{
			CadencePrises[i].dTab_Cadence[y] = 0;
			CadenceTapis[i].dTab_Cadence[y] = 0;
		}

		CadenceTapis[i].dMoy_Cadence = 0;
		CadenceTapis[i].ulTickTimeoutAcquit = 0;
		CadenceTapis[i].iModulo_Cadence = 0;

		theCam.Piste[i].lCountVision_OK = 0;
	}

	theApp.m_Robot1.peson[0]->lNbMesuresEffectuees = 0;
	theApp.m_Robot1.peson[1]->lNbMesuresEffectuees = 0;
												   
	theApp.m_Robot2.peson[0]->lNbMesuresEffectuees = 0;
	theApp.m_Robot2.peson[1]->lNbMesuresEffectuees = 0;
												   
	theApp.m_Robot3.peson[0]->lNbMesuresEffectuees = 0;
	theApp.m_Robot3.peson[1]->lNbMesuresEffectuees = 0;
	
}