#include "StdAfx.h"
#include "Peson.h"

#include "BaseFenetre.h"
#include "StaticMenu.h"

CPeson::CPeson(void)
{
	LastMesure = 0;
	StatutLastMesure = 0; //0->lu par vision; 1->pesage OK; 2-> Pesage NOK (à archiver)
	
	StabilisedValue = 0;

	PoidsEtalon1 = 0;
	PoidsEtalon2 = 0;

	StatutEtalon1 = 0;
	StatutEtalon2 = 0;

	PesonMode = PESON_MODEON;

	iEtat = 0;

	//Tableau des n derniéres mesures glissantes
	indexHisto = 0;
	for(int i = 0; i < PESON_NB_MESURES_HISTO; i++)
	{
		TabMesures[i] = 0;
	}

	indexAffichage = 0;//GCA V1.18

	NbMesuresVerifDyna = 0;

	IndexStatutLastPoids = -1;
	IndexDemandeTare = -1;
	iCyclePulse = 0;
	//CurrentRecette = NULL;

	pBitInhibition = NULL;
	pBitAcquittement = NULL;
	pBitEtalon1OK = NULL ;
	pBitEtalon2OK = NULL;
	pBitDoTare = NULL;
	pBitVerifDyna = NULL;

	//V 44.7 comptage des produits pour cadence par robot
	lNbMesuresEffectuees = 0;

}

CPeson::~CPeson(void)
{
}

int CPeson::Do_Tare(void)
{
	// demande de tare pour le peson
	
	*pBitDoTare = TRUE;

	return 0;
}

int CPeson::Do_VerifDynamique(void)
{
	//TODO demande de verification dynamique sur N mesures pour le peson A VOIR AUTOMATE
	//NbMesuresVerifDyna  = N mesures a faire
	
	//RAZ du tableau des Derniéres mesures
	indexHisto = 0;
	for(int i = 0; i < PESON_NB_MESURES_HISTO; i++)
	{
		TabMesures[i] = 0;
	}

	//lancement du cycle
	theApp.Process.iOldEtat = theApp.Process.iEtat;
	theApp.Process.iEtat = ETAT_PROCESS_VERIFDYNAMIQUE;
	*pBitVerifDyna = TRUE;

	return 0;
}

int CPeson::GererMesure() 
{
	//V 44.7 comptage des produits pour cadence par robot
	lNbMesuresEffectuees++;

	//A VOIR Prendre valeur Mesure + Historique 
	if(StatutLastMesure == PESON_MESURE_OK)
	{
		
		
	}
	else //if(iFlagVal == PESON_MESURE_NOK)
	{
		//Ecriture dans la BDD mesure NOK
		//Ajout défaut
		if(LastMesure < ((int) (theApp.m_Recette.PoidsMini * 1000)))
		{
			DefautPoidsMin.Metrage = theApp.m_Recette.NbPieceOK[1];
			DefautPoidsMin.Position = iNumPeson;
			DefautPoidsMin.Valeur = LastMesure;
			//DefautPoidsMin.Ajouter();
			SendMessage(theApp.PesonsView->GetSafeHwnd(),WM_PESON_DEFAUT_POIDSMIN,iNumPeson,0);
		}
		else
		{
			DefautPoidsMax.Metrage = theApp.m_Recette.NbPieceOK[1];
			DefautPoidsMax.Position = iNumPeson;
			DefautPoidsMax.Valeur = LastMesure;
			//DefautPoidsMax.Ajouter();
			SendMessage(theApp.PesonsView->GetSafeHwnd(),WM_PESON_DEFAUT_POIDSMAX,iNumPeson,0);
		}
		
	}

	TabMesures[indexHisto] = LastMesure;
	indexHisto ++;
	if(indexHisto == PESON_NB_MESURES_HISTO) indexHisto = 0;

	return 0;
}

int CPeson::inhiber(void * BoutonParent,int Mode)
{
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	if((theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONDEMARRAGE)&&
		(theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONMARCHE))
	{
		if( Mode ==PESON_INHIBER_NORMAL)
		{
			if(PesonMode == PESON_MODEOFF)
			{
				PesonMode = PESON_MODEON;
				if(ParentBtn != NULL)ParentBtn->iFonction = 0;

				*pBitInhibition = FALSE;
			}
			else
			{
				PesonMode = PESON_MODEOFF;
				if(ParentBtn != NULL)ParentBtn->iFonction = 1;

				*pBitInhibition = TRUE;
			}
		}
		else if( Mode ==PESON_INHIBER_FORCEON)
		{
			PesonMode = PESON_MODEON;
			if(ParentBtn != NULL)ParentBtn->iFonction = 0;

			*pBitInhibition = FALSE;
		}
		else if( Mode ==PESON_INHIBER_FORCEOFF)
		{
			PesonMode = PESON_MODEOFF;
			if(ParentBtn != NULL)ParentBtn->iFonction = 1;

			*pBitInhibition = TRUE;
		}
		
		if(ParentBtn != NULL)((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();
	}
	else
	{
		//Message Inhibition Interdite dans ce mode
		theApp.SetInfosString(theApp.TableMessage.GetStr(MESSAGE_INHIBITION_INTERDITE));
	}

	//Traca si Différent de HORS PRODUCTION
	if(theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION)
	{
		CString strLibelle;
		if(PesonMode == PESON_MODEON)
		{
			strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_ACTIVATION_PESON),iNumPeson);
		}
		else
		{
			strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_DESACTIVATION_PESON),iNumPeson);
		}

		theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE,strLibelle,_T(""),theApp.CurrentUser.strCode);
	}
	return 0;
}


int CPeson::GererAlarme(void)
{
	//TODO

	return 0;
}

bool CPeson::GererValidation(void)
{
	//lecture valeurs etalons

	//Gestion Staut Poids Etalon
	if(PesonMode == PESON_MODEOFF)
	{
		StatutEtalon1 = PESON_MODEOFF;
		StatutEtalon2 = PESON_MODEOFF;
	}
	else
	{
		if((PoidsEtalon1/1000. > pParamPoidsMaxEtalon1->value)||(PoidsEtalon1/1000. < pParamPoidsMinEtalon1->value))
		{
			StatutEtalon1 = PESON_STATUTETALON_NOK;
			*pBitEtalon1OK = FALSE;
		}
		else
		{
			StatutEtalon1 = PESON_STATUTETALON_OK;
			*pBitEtalon1OK = TRUE;
		}

		if((PoidsEtalon2/1000. > pParamPoidsMaxEtalon2->value)||(PoidsEtalon2/1000. < pParamPoidsMinEtalon2->value))
		{
			StatutEtalon2 = PESON_STATUTETALON_NOK;
			*pBitEtalon2OK = FALSE;
		}
		else
		{
			StatutEtalon2 = PESON_STATUTETALON_OK;
			*pBitEtalon2OK = TRUE;
		}
	}


	return false;
}

bool CPeson::GererProduction(void)
{
	//Lecture poids en prod
	
	//Values = valeurs OPC
	if((StatutLastMesure != 0 )&&(*pBitAcquittement == FALSE))
	{
		//Prise en compte de la mesure
		GererMesure();
		*pBitAcquittement = TRUE;
		return true;
	}
	
	if((StatutLastMesure == 0 )&&(*pBitAcquittement == TRUE))
	{
		*pBitAcquittement = FALSE;
	}

	
	return false;
}

bool CPeson::GererVerifDyna(void)
{
	//Lecture poids en prod

	if(*pBitVerifDyna != TRUE)return false;
	
	//Values = valeurs OPC
	if((StatutLastMesure != 0 )&&(*pBitAcquittement == FALSE))
	{
		//Prise en compte de la mesure
		GererMesure();
		*pBitAcquittement = TRUE;
		return true;
	}
	
	if((StatutLastMesure == 0 )&&(*pBitAcquittement == TRUE))
	{
		*pBitAcquittement = FALSE;
	}

	//Verification Fin de la verification dynamique
	if(indexHisto >= NbMesuresVerifDyna)
	{
		*pBitVerifDyna = FALSE;
	}

	
	return false;
}


int CPeson::InitOF(void)
{
	//Debut OF
	//Chargement des défauts
	CString strCode;

	strCode.Format(_T("P%dMIN"),iNumPeson);
	DefautPoidsMin.IDCoupe = theApp.m_Recette.ID;
	DefautPoidsMin.Charger(strCode);

	strCode.Format(_T("P%dMAX"),iNumPeson);
	DefautPoidsMax.IDCoupe = theApp.m_Recette.ID;
	DefautPoidsMax.Charger(strCode);

	//Reset Inhibition + currentRecette

	PesonMode = PESON_MODEON;
	//CurrentRecette = &theApp.m_Recette;

	//V 44.7 comptage des produits pour cadence par robot
	lNbMesuresEffectuees = 0;
	return 0;
}
