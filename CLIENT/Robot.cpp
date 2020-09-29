#include "StdAfx.h"
#include "Robot.h"

#include "BaseFenetre.h"
#include "StaticMenu.h"

CRobot::CRobot(void)
{
	X = 0;
	Y = 0;
	Z = 0;
	PosStatut = ROBOT_POSITION_RIEN;
	PosStatutSimul = ROBOT_POSITION_OK;
	bPosSimul = false;

	m_OPCClient_TR = NULL;
	m_OPCClient_TW = NULL;

	NumPrise = 0;

	RobotMode = ROBOT_MODEON;

	iEtat = 0;
	//IndexAckPos = -1;

	iIndexHisto = 0;
	for(int i =0; i< 50; i++)
	{
		HistoPos[i].iNumImg = 0;
		HistoPos[i].X = 0;
		HistoPos[i].Y = 0;
		HistoPos[i].Z = 0;
		HistoPos[i].PosStatut = 0;
	}

	pBitParking = NULL;
	pBitInhibition = NULL;

}

CRobot::~CRobot(void)
{
}

//void CRobot::GererAcquittement()
//{
//	//A VOIR gestion acquittement
//	NumPrise ++;
//}

int CRobot::SetPosition(int TypePosition, int fX, int fY, int fZ)
{
	PosStatut = TypePosition;
	X = fX + pParamOffsetPosX->value;
	Y = fY + pParamOffsetPosY->value;
	Z = fZ + pParamOffsetPosZ->value;

	NumPrise ++;

	//Sauvegarde historique
	HistoPos[iIndexHisto].iNumImg = NumPrise;
	HistoPos[iIndexHisto].X = X;
	HistoPos[iIndexHisto].Y = Y;
	HistoPos[iIndexHisto].Z = Z;
	HistoPos[iIndexHisto].PosStatut = PosStatut;

	iIndexHisto++;
	if(iIndexHisto >=50) 
		iIndexHisto = 0;
	
	return 0;
}

int CRobot::inhiber(void* BoutonParent,int iMode)
{
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	if((theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONDEMARRAGE)&&
	(theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONMARCHE))
	{
		if( iMode ==PESON_INHIBER_NORMAL)
		{
			if(RobotMode == ROBOT_MODEOFF)
			{
				RobotMode = ROBOT_MODEON;
				if(ParentBtn != NULL)ParentBtn->iFonction = 1;
				*pBitInhibition = FALSE;
			}
			else
			{
				RobotMode = ROBOT_MODEOFF;
				if(ParentBtn != NULL)ParentBtn->iFonction = 0;
				*pBitInhibition = TRUE;
			}
		}
		else if( iMode ==PESON_INHIBER_FORCEON)
		{
			RobotMode = ROBOT_MODEON;
			if(ParentBtn != NULL)ParentBtn->iFonction = 1;
			*pBitInhibition = FALSE;
		}
		else if( iMode ==PESON_INHIBER_FORCEOFF)
		{
			RobotMode = ROBOT_MODEOFF;
			if(ParentBtn != NULL)ParentBtn->iFonction = 0;
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
		if(RobotMode == ROBOT_MODEON)
		{
			strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_ACTIVATION_ROBOT),iNumRobot);
		}
		else
		{
			strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_DESACTIVATION_ROBOT),iNumRobot);
		}

		theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE,strLibelle,_T(""),theApp.CurrentUser.strCode);
	}
	return 0;
}

int CRobot::GererAlarme(void)
{
	//TODO GCAV1.16

	return 0;
}

bool CRobot::GererCycle(int iNumRobot)
{
	CRobot* m_Robot = NULL;
	CProcess * m_Process = &theApp.Process;

	if(iNumRobot == 1)
	{
		m_Robot = &theApp.m_Robot1;

	}else if(iNumRobot == 2)
	{
		m_Robot = &theApp.m_Robot2;

	}else if(iNumRobot == 3)
	{
		m_Robot = &theApp.m_Robot3;
	}

	if(*m_Robot->peson[0]->pBitDoTare == TRUE)
	{
		//on attend N cycles
		if(m_Process->iCyclePulse >= pParamNbCyclePulse->value)
		{
			m_Process->iCyclePulse = 0;
			//on repasse à 0
			*m_Robot->peson[0]->pBitDoTare = FALSE;
		}
		else
		{
			m_Process->iCyclePulse++ ;
		}
	}
	
	if(*m_Robot->peson[1]->pBitDoTare == TRUE)
	{
		//on attend N cycles
		if(m_Process->iCyclePulse >= pParamNbCyclePulse->value)
		{
			m_Process->iCyclePulse = 0;
			//on repasse à 0
			*m_Robot->peson[1]->pBitDoTare = FALSE;
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
				m_Robot->peson[0]->GererValidation();
				m_Robot->peson[1]->GererValidation();
			break;

		case ETAT_PROCESS_PRODUCTIONARRET:
		case ETAT_PROCESS_ARRETENCOURS:
		case ETAT_PROCESS_PRODUCTIONMARCHE:
		case ETAT_PROCESS_VIDAGEARRET:
		case ETAT_PROCESS_VIDAGEMARCHE:
		case ETAT_PROCESS_ETALONNAGE_ETUI_ARRET:
		case ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE:
			m_Robot->peson[0]->GererProduction();
			m_Robot->peson[1]->GererProduction();
			break;

		case ETAT_PROCESS_VIDAGE2:
			//rien a faire
			break;

		case ETAT_PROCESS_VERIFDYNAMIQUE:
				m_Robot->peson[0]->GererVerifDyna();//Prise en compte des mesures
				m_Robot->peson[1]->GererVerifDyna();
			break;

		case ETAT_PROCESS_VERIFETALONNAGE:
				m_Robot->peson[0]->GererValidation();
				m_Robot->peson[1]->GererValidation();
			break;
		default:
			break;

	}
	//CPeson * m_Peson = (CPeson*)pPeson;
	//CRecette* Recette = (CRecette*) m_Peson->CurrentRecette;

	//

	//
	////GESTION Pulse Tare
	//if(m_Peson->IndexDemandeTare != -1)
	//{
	//	if(iTypeItem[m_Peson->IndexDemandeTare] == OPC_READ)
	//	{
	//		//Lecture
	//		//on repasse en lecture pour changer la variable d'Etat
	//		//et ainsi faire la PULSE
	//		if((Values[m_Peson->IndexDemandeTare].intVal == 1)&&(*(int*)pValues[m_Peson->IndexDemandeTare] == 1))
	//		{
	//			//on remet la valeur à 0
	//			*(int*)pValues[m_Peson->IndexDemandeTare] = 0;
	//			Values[m_Peson->IndexDemandeTare].intVal = 0;

	//			//on repasse en lecture
	//			iTypeItem[m_Peson->IndexDemandeTare] = OPC_WRITE;
	//		}

	//	}
	//	else
	//	{
	//		//Ecriture
	//		if((Values[m_Peson->IndexDemandeTare].intVal == 1)&&(*(int*)pValues[m_Peson->IndexDemandeTare] == 1))
	//		{
	//			//on attend N cycles
	//			if(m_Peson->iCyclePulse >= pParamNbCyclePulse->value)
	//			{
	//				m_Peson->iCyclePulse = 0;
	//				//on repasse en lecture
	//				iTypeItem[m_Peson->IndexDemandeTare] = OPC_READ;
	//			}
	//			else
	//			{
	//				m_Peson->iCyclePulse++ ;
	//			}
	//		}

	//	}
	//}

	
	//}
	return false;
}

bool CRobot::InitOF(void)
{
	//TODO Verif Debut d'OF
	RobotMode = ROBOT_MODEON;

	return false;
}
