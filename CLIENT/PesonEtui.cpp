#include "stdafx.h"
#include "PesonEtui.h"
#include "BaseFenetre.h"

CPesonEtui::CPesonEtui(void)
{
	oldLastMesure = -1;
	for (int i = 0; i < 8; i++)
	{
		UndefBits[i] = 0;
	}
}


CPesonEtui::~CPesonEtui(void)
{
}

void CPesonEtui::InitOF(void)
{
	CPeson::InitOF();

	*pBitInhibition = FALSE;
}
bool CPesonEtui::GererProduction(void)
{
	//Lecture poids en prod
	
	//Ecriture dans la BDD mesure NOK
	if ((oldLastMesure != -1)&&(LastMesure != oldLastMesure))
	{
		//Ajout défaut
		if(LastMesure < ((int) (iTareEtuiPlein - theApp.m_Recette.TolEtuiPlein_cg)))
		{
			DefautPoidsMin.Metrage = theApp.m_Recette.NbPieceOK[1];
			DefautPoidsMin.Position = iNumPeson;
			DefautPoidsMin.Valeur = LastMesure;
			//DefautPoidsMin.Ajouter();
			SendMessage(theApp.PesonsView->GetSafeHwnd(),WM_PESON_DEFAUT_POIDSMIN,iNumPeson,0);
		}
		else if(LastMesure > ((int) (iTareEtuiPlein + theApp.m_Recette.TolEtuiPlein_cg)))
		{
			DefautPoidsMax.Metrage = theApp.m_Recette.NbPieceOK[1];
			DefautPoidsMax.Position = iNumPeson;
			DefautPoidsMax.Valeur = LastMesure;
			//DefautPoidsMax.Ajouter();
			SendMessage(theApp.PesonsView->GetSafeHwnd(),WM_PESON_DEFAUT_POIDSMAX,iNumPeson,0);
		}
		
		TabMesures[indexHisto] = LastMesure;
		indexHisto ++;
		if(indexHisto == PESON_NB_MESURES_HISTO) indexHisto = 0;
	}
	
	oldLastMesure = LastMesure;
	
	return false;
}
bool CPesonEtui::GererCycle(int iParam)
{
	CProcess * m_Process = &theApp.Process;

	if(*theApp.PesonEtui.pBitDoTare == TRUE)
	{
		//on attend N cycles
		if(m_Process->iCyclePulse >= pParamNbCyclePulse->value)
		{
			m_Process->iCyclePulse = 0;
			//on repasse à 0
			*theApp.PesonEtui.pBitDoTare = FALSE;
		}
		else
		{
			m_Process->iCyclePulse++ ;
		}
	}
	
	switch(theApp.Process.iEtat)
	{
		case ETAT_PROCESS_VALIDATIONFINLOT:
		case ETAT_PROCESS_VALIDATIONDEBUTLOT:
				theApp.PesonEtui.GererValidation();
			break;

		case ETAT_PROCESS_PRODUCTIONARRET:
		case ETAT_PROCESS_ARRETENCOURS:
		case ETAT_PROCESS_PRODUCTIONMARCHE:
		case ETAT_PROCESS_VIDAGEARRET:
		case ETAT_PROCESS_VIDAGEMARCHE:
		case ETAT_PROCESS_ETALONNAGE_ETUI_ARRET:
		case ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE:
			theApp.PesonEtui.GererProduction();
			break;

		case ETAT_PROCESS_VIDAGE2:
			//rien a faire
			break;

		case ETAT_PROCESS_VERIFDYNAMIQUE:
				theApp.PesonEtui.GererVerifDyna();//Prise en compte des mesures
			break;

		case ETAT_PROCESS_VERIFETALONNAGE:
				theApp.PesonEtui.GererValidation();
			break;
		default:
			break;

	}
	return false;
}