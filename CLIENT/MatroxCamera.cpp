// MatroxCamera.cpp: implementation of the CMatroxCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseFenetre.h"
#include "General.h"

#include "AC_Base.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatroxCamera::CMatroxCamera()
{	
	bSave = false;
	lCleCoupe= 0;
	lCleOF= 0;
	CycleTestIO = 0;
	
	iModeAcquisition=1;
	iSnap=0;

	m_Handle = 0;
	IsFenetreOpened = false;

	lX = 0;
	lY = 0;
	m_pDiag = NULL;
	milDig = 0;
	milDigs = 0;
	milSys = 0;

	m_iModeInspection = CAM_MODE_ARRET;

	Simulation_Enable = false;

	m_iMode = 0;
	m_bIsOK = FALSE;

	lNumeroImage = 0;
	lNumeroImageDernierArret = 0;
	//Diviseur = 15;

	Temps_Image = 0;
	Temps_Calcul = 0;
	Date_Image = 0;
	Date_Calcul = 0;

//Positionnement physique	
	m_dOrigine = 0;

	for (int i = 0; i < 3; i++)
	{
		milAUXIO[i] = M_AUX_IO;
		milUSERBIT[i] = M_USER_BIT;
	}
	
}

CMatroxCamera::~CMatroxCamera()
{
}
#pragma warning(disable : 4995)
BOOL CMatroxCamera::LireFromBase(CADODatabase *daoBase, CString sApplication, CString sDossier)
{

	CADORecordset m_Recordset (daoBase);
	CString strNumero;
	strNumero.Format("%ld",m_iNumero);
	m_sRepertoire = sDossier;

	CString strCritere=_T("");
	strCritere = "Numero = " + strNumero + " AND Application = '" + sApplication  + "'";

	if(ACBaseReqRsOpen(strCritere,"V_Camera",&m_Recordset))
	{
		m_iNumeroGroupe = ACBaseGetInt("Groupe",&m_Recordset);
		m_iGroupeIndex = ACBaseGetInt("GroupeIndex",&m_Recordset);
		m_lNumSys = ACBaseGetLong("Systeme",&m_Recordset);

		m_sLibelle = ACBaseGetStr("Libelle",&m_Recordset);
		m_iType = ACBaseGetLong("Type",&m_Recordset);
		m_sFichier = ACBaseGetStr("Fichier",&m_Recordset);
		m_lNumPort = ACBaseGetLong("NumPort",&m_Recordset);

		Param_Camera.lNumSerial = ACBaseGetLong("CLSerial",&m_Recordset);
		Param_Camera.iFormat = ACBaseGetInt("CLFormat",&m_Recordset);
		Param_Camera.iGain = ACBaseGetInt("CLGain",&m_Recordset);
		Param_Camera.iGainInit = Param_Camera.iGain;
		Param_Camera.iMode = ACBaseGetInt("CLMode",&m_Recordset);
		Param_Camera.iTempsExpo = ACBaseGetInt("CLTempsExpo",&m_Recordset);
		Param_Camera.iTempsExpoInit = Param_Camera.iTempsExpo;

		dPixelSizeX = ACBaseGetDouble("PixelSizeX",&m_Recordset);
		dPixelSizeY = ACBaseGetDouble("PixelSizeY",&m_Recordset);
		m_dOrigine = ACBaseGetDouble("Origine",&m_Recordset);

		m_Recordset.Close();
		return TRUE;
	}
	else
	{
		sErrMsg = "ERREUR Camera.Lire \nCamera inexistante";
		return FALSE;
	}

	
}
#pragma warning(default : 4995)
BOOL CMatroxCamera::Charger(MIL_ID idSystem)
{
	int i;
	int iPiste;
	CString sTemp;
	PARAM_ROI RoiTemp;

	//V2.53
	dPixelSizeYZoom = 1;

	milSys = idSystem;
	sTemp = m_sRepertoire + _T("\\") + m_sFichier;
	MIL_TEXT_PTR mFile = sTemp.GetBuffer(sTemp.GetLength());
	switch(m_iType)
	{
		case 0:	//Mire
			break;

		case 1:	//Fichier
			lX = (long)MbufDiskInquire(mFile,M_SIZE_X,M_NULL);
			lY = (long)MbufDiskInquire(mFile,M_SIZE_Y,M_NULL);
			break;

		case 2:	//Camera
			MdigAlloc(milSys, m_lNumPort, mFile, M_DEFAULT, &milDig);
			MdigControl(milDig, M_GRAB_TIMEOUT, M_INFINITE );
			//MdigControl(milDig,M_ROTARY_ENCODER,M_ENABLE);
			//MdigControl(milDig,M_ROTARY_ENCODER_DIRECTION,M_BACKWARD);	//M_BACKWARD; M_FORWARD
			MdigControl(milDig,M_ROTARY_ENCODER_POSITION_TRIGGER,pParamVisionDiviseur->value);
			MdigControl(milDig,M_ROTARY_ENCODER_POSITION,0);

			//V 30-ALL
			//V44.4 : Carte Full sur toutes les machine => pas de DigIO spécifique
			//if (theConfig.str_Machine.Find(_T("#1")) >= 0)
			//{
			//	//#1 -> Sorties sur Port1
			//	sTemp =  m_sRepertoire + _T("\\DigIO.dcf");
			//	mFile = sTemp.GetBuffer();
			//	MdigAlloc(milSys, m_lNumPort + 1, mFile, M_DEFAULT, &milDigs);
			//	sTemp.ReleaseBuffer();
			//}
			//else
			//{
				//#2 
				milDigs = milDig;
			//}

			milAUXIO[0] = (MIL_INT64) (M_AUX_IO0 + pParamVisionAuxIORobot1->value);
			milAUXIO[1] = (MIL_INT64) (M_AUX_IO0 + pParamVisionAuxIORobot2->value);
			milAUXIO[2] = (MIL_INT64) (M_AUX_IO0 + pParamVisionAuxIORobot3->value);

			for (int i = 0; i < 3; i++)
			{
				//Definitions SOLIOS
				switch (milAUXIO[i])
				{
					case M_AUX_IO2  :  
						milUSERBIT[i] = M_USER_BIT4;
						break;
					case M_AUX_IO3  :  
						milUSERBIT[i] = M_USER_BIT5;
						break;
					case M_AUX_IO8  :  
						milUSERBIT[i] = M_USER_BIT2;
						break;
					case M_AUX_IO9  :  
						milUSERBIT[i] = M_USER_BIT3;
						break;
					
					case M_AUX_IO12 :  
						milUSERBIT[i] = M_USER_BIT0;
						break;
					case M_AUX_IO13 :  
						milUSERBIT[i] = M_USER_BIT1;
						break;
					
				}


				MdigControl(milDigs,M_IO_SOURCE + milAUXIO[i],milUSERBIT[i]);	
				MdigControl(milDigs,M_IO_MODE + milAUXIO[i],M_OUTPUT);
				MdigControl(milDigs,M_USER_BIT_STATE + milUSERBIT[i] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);

			}

			lX =(long) MdigInquire(milDig, M_SIZE_X, M_NULL);
			lY =(long) MdigInquire(milDig, M_SIZE_Y, M_NULL);

			Param_Camera.iMode = 1;
			sTemp.ReleaseBuffer();
			break;
	}

	Acq_Nb = 4;
	lYCumul = lY * NBCUMUL;

	dZoom = lX/(double)pParamVisionDXZoom->value;
	lXZoom = (long)(lX/dZoom);
	lYZoom = (long)(lY/dZoom);
	lYZoomCumul = lYZoom * NBCUMUL;

	//Parametrage des pistes
	Piste[0].lNumPiste = 0;
	Piste[1].lNumPiste = 1;
	Piste[2].lNumPiste = 2;

	Piste[0].XMin = pParamVisionPiste1G->value;		
	Piste[0].XMax = pParamVisionPiste1D->value;
	Piste[1].XMin = pParamVisionPiste2G->value;
	Piste[1].XMax = pParamVisionPiste2D->value;
	Piste[2].XMin = pParamVisionPiste3G->value;
	Piste[2].XMax = pParamVisionPiste3D->value;
	for(i=0;i<NBPISTE;i++)
	{
		Piste[i].XMinZoom = (long) (Piste[i].XMin/dZoom);
		Piste[i].XMaxZoom = (long)(Piste[i].XMax/dZoom);
		Piste[i].IndexLastRoi = 0;
		Piste[i].IndexRoi = 0;
	}


	//IMAGES ACQUISITION
	imgGrab[0].sLibelle = m_sLibelle + " Image Acquisition 0";
	imgGrab[1].sLibelle = m_sLibelle + " Image Acquisition 1";
	imgGrab[2].sLibelle = m_sLibelle + " Image Acquisition 2";
	imgGrab[3].sLibelle = m_sLibelle + " Image Acquisition 3";
	imgGrab[4].sLibelle = m_sLibelle + " Image Acquisition 4";
	imgGrab[5].sLibelle = m_sLibelle + " Image Acquisition 5";
	imgGrab[6].sLibelle = m_sLibelle + " Image Acquisition 6";
	imgGrab[7].sLibelle = m_sLibelle + " Image Acquisition 7";
	//imgGrab[8].sLibelle = m_sLibelle + " Image Acquisition 8";
	//imgGrab[9].sLibelle = m_sLibelle + " Image Acquisition 9";
	if (milDig == 0)
	{
		for(i=0;i<Acq_Nb;i++)
		{
			imgGrab[i].Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
			sTemp.Format("_%d",i);
			sTemp = m_sFichier.Left(m_sFichier.GetLength()-4) + sTemp + ".bmp";
			CString str;
			str = m_sRepertoire + _T("\\") + sTemp;
			mFile = str.GetBuffer(str.GetLength());
			MbufLoad(mFile,imgGrab[i].milBuf);
			str.ReleaseBuffer();
		}
	}
	else
	{
		for(i=0;i<Acq_Nb;i++)
		{
			imgGrab[i].Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_GRAB+M_PROC+M_DISP);
		}
	}

	//Image de selection couleur
	imgSelectionColor.sLibelle = "Image Selection couleur";
	imgSelectionColor.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//Image de simulation
	Simulation_Image.sLibelle = m_sLibelle + " Image Simulation";
	Simulation_Image.Charger(milSys,lX,lY*10,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	for(i=0;i<10;i++)
	{
		RoiTemp.X1 = 0;
		RoiTemp.Y1 = lY*i;
		RoiTemp.dX = lX;
		RoiTemp.dY = lY;
		RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Simulation_Roi[i].Charger(&Simulation_Image);
		Simulation_Roi[i].SetROI(&RoiTemp);
	}
	RoiTemp.X1 = 0;
	RoiTemp.Y1 = 0;
	RoiTemp.dX = lX;
	RoiTemp.dY = lY;
	RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	Simulation_RoiPiste.Charger(&Simulation_Image);
	Simulation_RoiPiste.SetROI(&RoiTemp);

	//Images source par piste
	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		for(i=0;i<Acq_Nb;i++)
		{
			RoiTemp.X1 = Piste[iPiste].XMin;
			RoiTemp.X2 = Piste[iPiste].XMax;
			RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
			RoiTemp.Y1 = 0;
			RoiTemp.dY = lY;
			RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
			Piste[iPiste].roiGrab[i].Charger(&imgGrab[i]);
			Piste[iPiste].roiGrab[i].SetROI(&RoiTemp);
		}
	}

	//Pour l'affichage de la position des pistes
	imgGrabVisu.sLibelle = m_sLibelle + " Image Acquisition Live";
	imgGrabVisu.Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//Pour l'affichage uniquement
	//V 44.1
	long lYZoomCumul_Scroll = lYZoom * NBCUMUL_SCROLL;
	
	imgScroll.sLibelle = m_sLibelle + " Image Scrolling";
	imgScroll.Charger(milSys,lXZoom, lYZoomCumul_Scroll,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	RoiTemp.X1 = 0;
	RoiTemp.Y1 = lYZoom;
	RoiTemp.dX = lXZoom;
	RoiTemp.dY = lYZoom * (NBCUMUL_SCROLL -1);
	RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	ROIScroll[0].Charger(&imgScroll);
	ROIScroll[0].SetROI(&RoiTemp);
	
	RoiTemp.X1 = 0;
	RoiTemp.Y1 = 0;
	RoiTemp.dX = lXZoom;
	RoiTemp.dY = lYZoom * (NBCUMUL_SCROLL -1);
	RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	ROIScroll[1].Charger(&imgScroll);
	ROIScroll[1].SetROI(&RoiTemp);

	RoiTemp.X1 = 0;
	RoiTemp.Y1 = lYZoom * (NBCUMUL_SCROLL -1);
	RoiTemp.dX = lXZoom;
	RoiTemp.dY = lYZoom;
	RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	ROIScroll[2].Charger(&imgScroll);
	ROIScroll[2].SetROI(&RoiTemp);

	//BUG Mil 9 : Besoin d'une image intermédiaire pour les copies de roi à roi
	imgScrollTemp.sLibelle = m_sLibelle + " Image Scrolling Temporaire";
	imgScrollTemp.Charger(milSys,lXZoom, lYZoomCumul_Scroll -lYZoom,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//IDEM imgGrab
	imgGrabZoom.sLibelle = "Image Grab Zoom";
	imgGrabZoom.Charger(milSys,lXZoom,lYZoom,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		RoiTemp.X1 = Piste[iPiste].XMinZoom;
		RoiTemp.X2 = Piste[iPiste].XMaxZoom;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.Y1 = 0;
		RoiTemp.dY = lYZoom;
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Piste[iPiste].roiGrabZoom.Charger(&imgGrabZoom);
		Piste[iPiste].roiGrabZoom.SetROI(&RoiTemp);
	}

	//Image de projection par piste
	imgProj.sLibelle = "Image Projection";
	imgProj.Charger(milSys,lXZoom,lYZoom,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		RoiTemp.X1 = Piste[iPiste].XMinZoom;
		RoiTemp.X2 = Piste[iPiste].XMaxZoom;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.Y1 = 0;
		RoiTemp.dY = lYZoom;
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Piste[iPiste].roiBinZoom.Charger(&imgProj);
		Piste[iPiste].roiBinZoom.SetROI(&RoiTemp);
		//Buffers de projection
		imgRes[iPiste].Charger(milSys,RoiTemp.dY,M_PROJ_LIST);	//Projection Repère vertical
	}
	imgRes[3].Charger(milSys,lYCumul,M_PROJ_LIST);	//Code sachet gauche
	imgRes[4].Charger(milSys,lYCumul,M_PROJ_LIST);	//Code sachet droite
	imgRes[5].Charger(milSys,lYCumul,M_PROJ_LIST);	//Code sachet gauche V32ALL
	imgRes[6].Charger(milSys,lYCumul,M_PROJ_LIST);	//Code sachet droite V32ALL
	imgRes[7].Charger(milSys,lYCumul,M_PROJ_LIST);	//Code sachet gauche V33ALL
	imgRes[8].Charger(milSys,lYCumul,M_PROJ_LIST);	//Code sachet droite V33ALL

	imgTrack.sLibelle = "Image Tracking";
	imgTrack.Charger(milSys,lX,lYCumul,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	imgTrackTache.sLibelle = "Image Tracking TACHE";
	imgTrackTache.Charger(milSys,lX,lYCumul,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	imgTrackCouleur.sLibelle = "Image Tracking COULEUR";
	imgTrackCouleur.Charger(milSys,lX,lYCumul,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	imgTrackZoom.sLibelle = "Image Tracking Zoom";
	imgTrackZoom.Charger(milSys,lXZoom,lYZoomCumul,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	imgRotationZoom.sLibelle = "Image Rotation Zoom";
	imgRotationZoom.Charger(milSys,300,300,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//Roi de rotation pour redimentionnement des objets
	RoiTemp.X1 = 0;
	RoiTemp.X2 = 300;
	RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
	RoiTemp.Y1 = 0;
	RoiTemp.dY = 300;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	roiRotation.Charger(&imgRotationZoom);
	roiRotation.SetROI(&RoiTemp);

	imgSachetVertical.sLibelle = "Sachet Vertical";
	imgSachetVertical.Charger(milSys,2400,800,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	imgResultat.sLibelle = "Resultats vision";
	imgResultat.Charger(milSys,2400,800,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	CString strDir;
	strDir = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine ;

	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		//roi de recherche du datamatrix, dimension réelle
		//TODOBM : A dimensionner en hauteur en fonction des objets
		RoiTemp.X1 = Piste[iPiste].XMin;
		RoiTemp.X2 = Piste[iPiste].XMax;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.Y1 = 0;
		RoiTemp.dY = lYCumul/2;		//TODOBM
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Piste[iPiste].roiDetection[0].Charger(&imgTrack);
		Piste[iPiste].roiDetection[0].SetROI(&RoiTemp);
		//Calibration
		Piste[iPiste].roiDetection[0].Calibration = new CCalibration(milSys);
		switch(iPiste)	//TODOBM : paramétrer le fichier
		{
			case 0:
				Piste[iPiste].roiDetection[0].Calibration->ChargerFromFile(strDir,"B&LPiste0.txt");
				break;
			case 1:
				Piste[iPiste].roiDetection[0].Calibration->ChargerFromFile(strDir,"B&LPiste1.txt");
				break;
			case 2:
				Piste[iPiste].roiDetection[0].Calibration->ChargerFromFile(strDir,"B&LPiste2.txt");
				break;
		}

		RoiTemp.Y1 = lYCumul/2;		//TODOBM
		RoiTemp.dY = lYCumul/2;
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Piste[iPiste].roiDetection[1].Charger(&imgTrack);
		Piste[iPiste].roiDetection[1].SetROI(&RoiTemp);
		//Calibration
		Piste[iPiste].roiDetection[1].Calibration = new CCalibration(milSys);
		switch(iPiste)	//TODOBM : paramétrer le fichier
		{
			
			case 0:
				Piste[iPiste].roiDetection[1].Calibration->ChargerFromFile(strDir,"B&LPiste0.txt");
				break;
			case 1:
				Piste[iPiste].roiDetection[1].Calibration->ChargerFromFile(strDir,"B&LPiste1.txt");
				break;
			case 2:
				Piste[iPiste].roiDetection[1].Calibration->ChargerFromFile(strDir,"B&LPiste2.txt");
				break;
		}

		//Sections de cumul
		RoiTemp.X1 = Piste[iPiste].XMin;
		RoiTemp.X2 = Piste[iPiste].XMax;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.dY = lY;
		for(i=0;i<NBCUMUL;i++)
		{
			RoiTemp.Y1 = lY*i;
			RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
			Piste[iPiste].roiTrack[i].Charger(&imgTrack);
			Piste[iPiste].roiTrack[i].SetROI(&RoiTemp);
			Piste[iPiste].roiTrackTache[i].Charger(&imgTrackTache);
			Piste[iPiste].roiTrackTache[i].SetROI(&RoiTemp);
			Piste[iPiste].roiTrackCouleur[i].Charger(&imgTrackCouleur);
			Piste[iPiste].roiTrackCouleur[i].SetROI(&RoiTemp);
		}
		Piste[iPiste].roiSaveToFile.Charger(&imgTrackCouleur);

		//roi de recherche du blob
		//TODOBM : A dimensionner en hauteur en fonction des objets
		RoiTemp.X1 = Piste[iPiste].XMinZoom;
		RoiTemp.X2 = Piste[iPiste].XMaxZoom;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.Y1 = 0;
		RoiTemp.dY = lYZoomCumul/2;		//TODOBM
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Piste[iPiste].roiDetectionZoom[0].Charger(&imgTrackZoom);
		Piste[iPiste].roiDetectionZoom[0].SetROI(&RoiTemp);

		Piste[iPiste].roiModele.Charger(&imgTrackZoom);
		Piste[iPiste].roiModele.SetROI(&RoiTemp);
		Piste[iPiste].roiBlob.Charger(&imgTrackZoom);
		Piste[iPiste].roiBlob.SetROI(&RoiTemp);

		//Calibration
		Piste[iPiste].roiDetectionZoom[0].Calibration = new CCalibration(milSys);
		for(i=0;i<Piste[iPiste].roiDetection[0].Calibration->iNbPoints;i++)
		{
			Piste[iPiste].roiDetectionZoom[0].Calibration->XPixArray[i] = Piste[iPiste].roiDetection[0].Calibration->XPixArray[i]/dZoom;
			Piste[iPiste].roiDetectionZoom[0].Calibration->YPixArray[i] = Piste[iPiste].roiDetection[0].Calibration->YPixArray[i]/dZoom;
			Piste[iPiste].roiDetectionZoom[0].Calibration->XmmArray[i] = Piste[iPiste].roiDetection[0].Calibration->XmmArray[i];
			Piste[iPiste].roiDetectionZoom[0].Calibration->YmmArray[i] = Piste[iPiste].roiDetection[0].Calibration->YmmArray[i];
		}
		Piste[iPiste].roiDetectionZoom[0].Calibration->iNbPoints = Piste[iPiste].roiDetection[0].Calibration->iNbPoints;
		Piste[iPiste].roiDetectionZoom[0].Calibration->ChargerFromListe();

		RoiTemp.Y1 = lYZoomCumul/2;		//TODOBM
		RoiTemp.dY = lYZoomCumul/2;
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Piste[iPiste].roiDetectionZoom[1].Charger(&imgTrackZoom);
		Piste[iPiste].roiDetectionZoom[1].SetROI(&RoiTemp);
		//Calibration
		Piste[iPiste].roiDetectionZoom[1].Calibration = new CCalibration(milSys);

		for(i=0;i<Piste[iPiste].roiDetection[1].Calibration->iNbPoints;i++)
		{
			Piste[iPiste].roiDetectionZoom[1].Calibration->XPixArray[i] = Piste[iPiste].roiDetection[1].Calibration->XPixArray[i]/dZoom;
			Piste[iPiste].roiDetectionZoom[1].Calibration->YPixArray[i] = Piste[iPiste].roiDetection[1].Calibration->YPixArray[i]/dZoom;
			Piste[iPiste].roiDetectionZoom[1].Calibration->XmmArray[i] = Piste[iPiste].roiDetection[1].Calibration->XmmArray[i];
			Piste[iPiste].roiDetectionZoom[1].Calibration->YmmArray[i] = Piste[iPiste].roiDetection[1].Calibration->YmmArray[i];
		}
		Piste[iPiste].roiDetectionZoom[1].Calibration->iNbPoints = Piste[iPiste].roiDetection[1].Calibration->iNbPoints;
		Piste[iPiste].roiDetectionZoom[1].Calibration->ChargerFromListe();

		//Sections de cumul Zoom
		RoiTemp.X1 = Piste[iPiste].XMinZoom;
		RoiTemp.X2 = Piste[iPiste].XMaxZoom;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.dY = lYZoom;
		for(i=0;i<NBCUMUL;i++)
		{
			RoiTemp.Y1 = lYZoom*i;
			RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
			Piste[iPiste].roiTrackZoom[i].Charger(&imgTrackZoom);
			Piste[iPiste].roiTrackZoom[i].SetROI(&RoiTemp);
		}
	}

	CString StrFile;
	//StrFile = "C:\\ACYBL\\B&L_Blister.mmf";
	StrFile = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\") +  _T("B&L_Blister.mmf");
	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		ModeleBlister[iPiste].MF_System = &milSys;
		ModeleBlister[iPiste].mImg = &Piste[iPiste].roiDetectionZoom[0];
		ModeleBlister[iPiste].strFile = StrFile;
		ModeleBlister[iPiste].mTitre = "Objet";
		ModeleBlister[iPiste].Charger();
	}

	//StrFile = "C:\\ACYBL\\B&L_Sachet.mmf";
	StrFile = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\") + _T("B&L_Sachet.mmf");
	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		ModeleSachet[iPiste].MF_System = &milSys;
		ModeleSachet[iPiste].mImg = &Piste[iPiste].roiDetectionZoom[0];
		ModeleSachet[iPiste].strFile = StrFile;
		ModeleSachet[iPiste].mTitre = "Objet";
		ModeleSachet[iPiste].Charger();
	}

	StrFile = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\") + _T("Datamatrix.mco");
	//StrFile = "C:\\ACYBL\\Datamatrix.mco";
	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		//roi de recherche du code
		RoiTemp.X1 = Piste[iPiste].XMin+240;
		RoiTemp.dX = 100;
		RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
		RoiTemp.Y1 = 340;
		RoiTemp.dY = 100;
		RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
		Piste[iPiste].roiCode.Charger(&imgTrack);
		Piste[iPiste].roiCode.SetROI(&RoiTemp);

		Piste[iPiste].PtDatamatrixOrigine.Xpix = 14;
		Piste[iPiste].PtDatamatrixOrigine.Ypix = 43;


		Piste[iPiste].CodeObj.MF_System = &milSys;
		Piste[iPiste].CodeObj.mImg = &Piste[iPiste].roiCode;
		Piste[iPiste].CodeObj.MF_File = StrFile;
		Piste[iPiste].CodeObj.mTitre = "Objet";
		//Piste[iPiste].CodeObj.Charger();
		Piste[iPiste].CodeObj.ChargerFromFile();
	}

	//V2.10 : Analyse des taches
	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		//roi de recherche des taches
		RoiTemp.X1 = Piste[iPiste].XMin;
		RoiTemp.X2 = Piste[iPiste].XMax;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.Y1 = 0;
		RoiTemp.Y2 = 100;
		RoiTemp.dX = RoiTemp.Y2 + RoiTemp.Y1;
		Piste[iPiste].roiTacheSrc.Charger(&imgTrack);
		Piste[iPiste].roiTacheSrc.SetROI(&RoiTemp);
		Piste[iPiste].roiTacheDst.Charger(&imgTrackTache);
		Piste[iPiste].roiTacheDst.SetROI(&RoiTemp);
	}

	for(iPiste=0;iPiste<NBPISTE;iPiste++)
	{
		//roi de controle des codes barre
		RoiTemp.X1 = Piste[iPiste].XMin;
		RoiTemp.X2 = Piste[iPiste].XMax;
		RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
		RoiTemp.Y1 = 0;
		RoiTemp.Y2 = lY;
		RoiTemp.dY = RoiTemp.Y2 - RoiTemp.Y1;
		Piste[iPiste].roiSachetRotationSrc.Charger(&imgTrack);
		Piste[iPiste].roiSachetRotationSrc.SetROI(&RoiTemp);

		RoiTemp.X1 = iPiste*800;
		RoiTemp.dX = 800;
		RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
		RoiTemp.Y1 = 0;
		RoiTemp.Y2 = 800;
		RoiTemp.dY = RoiTemp.Y2 - RoiTemp.Y1;
		Piste[iPiste].roiSachetRotationDst.Charger(&imgSachetVertical);
		Piste[iPiste].roiSachetRotationDst.SetROI(&RoiTemp);

		Piste[iPiste].CodeSachet[0].roiLecture.Charger(&imgSachetVertical);
		Piste[iPiste].CodeSachet[0].roiLecture.SetROI(&RoiTemp);
		Piste[iPiste].CodeSachet[0].mImgRes = &imgRes[3];
		Piste[iPiste].CodeSachet[1].roiLecture.Charger(&imgSachetVertical);
		Piste[iPiste].CodeSachet[1].roiLecture.SetROI(&RoiTemp);
		Piste[iPiste].CodeSachet[1].mImgRes = &imgRes[4];
		//V32ALL
		Piste[iPiste].CodeSachet[2].roiLecture.Charger(&imgSachetVertical);
		Piste[iPiste].CodeSachet[2].roiLecture.SetROI(&RoiTemp);
		Piste[iPiste].CodeSachet[2].mImgRes = &imgRes[5];
		Piste[iPiste].CodeSachet[3].roiLecture.Charger(&imgSachetVertical);
		Piste[iPiste].CodeSachet[3].roiLecture.SetROI(&RoiTemp);
		Piste[iPiste].CodeSachet[3].mImgRes = &imgRes[6];
		//V33ALL
		Piste[iPiste].CodeSachet[4].roiLecture.Charger(&imgSachetVertical);
		Piste[iPiste].CodeSachet[4].roiLecture.SetROI(&RoiTemp);
		Piste[iPiste].CodeSachet[4].mImgRes = &imgRes[7];
		Piste[iPiste].CodeSachet[5].roiLecture.Charger(&imgSachetVertical);
		Piste[iPiste].CodeSachet[5].roiLecture.SetROI(&RoiTemp);
		Piste[iPiste].CodeSachet[5].mImgRes = &imgRes[8];

		Piste[iPiste].roiResultatSrc.Charger(&imgTrackCouleur);
		Piste[iPiste].roiResultatSrc.SetROI(&RoiTemp);
		Piste[iPiste].roiResultatDst.Charger(&imgResultat);
		Piste[iPiste].roiResultatDst.SetROI(&RoiTemp);
	}

	//NOYAUX DE CONVOLUTION ET MORPHOLOGIE
	ker[0].Charger(milSys,3,3);
	ker[1].Charger(milSys,20,20);

	//LUT
	//Acquisition
	unsigned char lTab[256];
	lut[0].Charger(milSys,256,8L);
	for (i=0; i<256; i++)
	{
		lTab[i]=i;
	}
	lTab[0] = 128;
	lTab[1] = 129;
	lTab[2] = 130;
	lTab[3] = 131;
	lTab[4] = 132;
	lTab[5] = 133;
	lTab[6] = 134;
	lTab[7] = 135;
	MbufPut1d(lut[0].milLut,0,256,lTab);

	//ACQUISITION
	Param_Acquisition.ThreadId = 0;
   
	// Intitalize threads parameter structures
	Param_Acquisition.iMode				= 0;

	return TRUE;
}

BOOL CMatroxCamera::Decharger()
{
	int i;

	if(!m_bIsOK) return TRUE;

	InitialBlob.Decharger();
	CouleurBlob.Decharger();
	RotationBlob.Decharger();

//	MappControlThread(Param_Acquisition.EventEndId, M_EVENT_FREE, M_DEFAULT, M_NULL);
	if (Param_Acquisition.ThreadId != 0)
	{
		MthrFree(Param_Acquisition.ThreadId) ;
		Param_Acquisition.ThreadId = 0;
	}

	for(i = 0; i < 20; i++)
	{
		Simulation_Roi[i].Decharger();
	}
	Simulation_RoiPiste.Decharger();

	Simulation_Image.Decharger();

	for(i=0;i<3;i++)
	{
		Piste[i].CodeObj.Decharger();
		Piste[i].roiGrabZoom.Decharger();
		Piste[i].roiBinZoom.Decharger();

		for(int y = 0; y < NBCUMUL; y++)
		{
			Piste[i].roiTrack[y].Decharger();
			Piste[i].roiTrackTache[y].Decharger();
			Piste[i].roiTrackCouleur[y].Decharger();
			Piste[i].roiTrackZoom[y].Decharger();
			Piste[i].roiGrab[y].Decharger();
		}
		Piste[i].roiSaveToFile.Decharger();

		for(int y = 0; y < 2; y++)
		{
			//V 44.8
			delete(Piste[i].roiDetectionZoom[y].Calibration);
			Piste[i].roiDetectionZoom[y].Calibration = NULL;

			Piste[i].roiDetectionZoom[y].Decharger();
			delete(Piste[i].roiDetection[y].Calibration);
			Piste[i].roiDetection[y].Calibration = NULL;

			Piste[i].roiDetection[y].Decharger();
		}

		Piste[i].roiModele.Decharger();
		Piste[i].roiBlob.Decharger();
		
		Piste[i].roiCode.Decharger();
		Piste[i].roiSachetRotationSrc.Decharger();
		Piste[i].roiSachetRotationDst.Decharger();
		Piste[i].CodeSachet[0].roiLecture.Decharger();
		Piste[i].CodeSachet[1].roiLecture.Decharger();
		Piste[i].CodeSachet[2].roiLecture.Decharger();
		Piste[i].CodeSachet[3].roiLecture.Decharger();
		Piste[i].CodeSachet[4].roiLecture.Decharger();
		Piste[i].CodeSachet[5].roiLecture.Decharger();
		
		//V 44.8 deja fait au-dessus Piste[i].CodeObj.Decharger();

		Piste[i].roiResultatSrc.Decharger();
		Piste[i].roiResultatDst.Decharger();

		ModeleBlister[i].Decharger();
		ModeleSachet[i].Decharger();

		Piste[i].roiTacheSrc.Decharger();
		Piste[i].roiTacheDst.Decharger();

		ROIScroll[i].Decharger();
	}

	for(i=0;i<NBCUMUL;i++)
	{
		imgGrab[i].Decharger();
	}

	imgGrabVisu.Decharger();
	imgGrabZoom.Decharger();
	imgSelectionColor.Decharger();
	imgScroll.Decharger();
	imgScrollTemp.Decharger();
	imgProj.Decharger();
	imgTrack.Decharger();
	imgTrackTache.Decharger();
	imgTrackCouleur.Decharger();
	imgTrackZoom.Decharger();
	roiRotation.Decharger();
	imgRotationZoom.Decharger();
	imgSachetVertical.Decharger();
	imgResultat.Decharger();//V 44.8
	//MdispFree(imgWnd.milDisp);
	//imgWnd.milDisp = 0;
	//MbufFree(imgWnd.milBuf);
	//imgWnd.milBuf = 0;

	for(i=0;i<10;i++)
	{
		imgRes[i].Decharger();
	}
	for(i=0;i<10;i++)
	{
		ker[i].Decharger();
	}
	for(i=0;i<10;i++)
	{
		lut[i].Decharger();
	}
	if (milDig != 0)
	{
		MdigFree(milDig);
		milDig=0;
	}

	//V44.4 : Carte Full sur toutes les machine => pas de DigIO spécifique
	//if (theConfig.str_Machine.Find(_T("#1")) >= 0)//V 30-ALL
	//{
	//	//#12
	//	if (milDigs != 0)
	//	{
	//		MdigFree(milDigs);
	//		milDigs=0;
	//	}
	//}
	

	m_bIsOK = FALSE;

	return TRUE;
}

void CMatroxCamera::Fenetre_Ouvrir()
{

	theApp.ExecuterFonctionExt(_T("AFF_CAMVIEW"),NULL);
//	if (m_pDiag == NULL)
//	{
//		m_pDiag = new CDiagCamera(AfxGetMainWnd( ));
//		((CDiagCamera*)m_pDiag)->m_Camera = this;
//		if (((CDiagCamera*) m_pDiag)->Create() == TRUE)
//		{
//			((CDiagCamera*)m_pDiag)->SetWindowPos(0,200,200,100,100,SWP_NOSIZE);
//			((CDiagCamera*)m_pDiag)->ShowWindow(SW_SHOWNORMAL);
//		}
//		//Fenetre pour Aviiva
////		Aviiva.m_pDiag = m_pDiag;
//	}
//	else
//		((CDiagCamera*)m_pDiag)->SetActiveWindow();
}

void CMatroxCamera::Fenetre_Fermer()
{
//	Aviiva.m_pDiag = NULL;
	m_pDiag = NULL;
	m_Handle = 0;
}

void CMatroxCamera::Start()
{
//	int i;
	if(Param_Acquisition.iMode > 0) return;

	Param_Acquisition.iMode = 1;

	lNumeroImage = 0;
	lNumeroImageDernierArret = 0;
	
	InitialBlob.MF_System = &milSys;
	InitialBlob.Charger(NB_MAX_BLOBS_INITIAL,false);
	CouleurBlob.MF_System = &milSys;
	CouleurBlob.Charger(NB_MAX_BLOBS_COULEUR,false);
	RotationBlob.MF_System = &milSys;
	RotationBlob.Charger(NB_MAX_BLOBS_ROTATION,false);

	MthrAlloc(milSys, M_THREAD, M_DEFAULT, Acquisition, this, &Param_Acquisition.ThreadId);
}

void CMatroxCamera::Stop()
{
	if(Param_Acquisition.iMode == 0) return;

	Param_Acquisition.iMode = 0;
	if (milDig > 0)
	{
		MdigControl(milDig, M_GRAB_ABORT , M_DEFAULT );
	}

	Sleep(500);
//#ifdef MIL10
	if (Param_Acquisition.ThreadId != 0)
	{
		MthrFree(Param_Acquisition.ThreadId) ;
		Param_Acquisition.ThreadId = 0;
	}
	
//#else
//	MappControlThread(Param_Acquisition.ThreadId,  M_THREAD_FREE, M_DEFAULT, M_NULL);
//#endif
	Sleep(500);
}

void CMatroxCamera::AppliquerParametres()
{
	if (milDig != 0)
	{
//		Aviiva.SetParam(&Param_Camera);
	}
}

void CMatroxCamera::AfficherData()
{
	/*if (m_pDiag == NULL) return;
	((CDiagCamera*)m_pDiag)->AfficherData();*/
//	((CDiagCamera*)m_pDiag)->MAJScrollBar();
}

#pragma warning(default : 4995)