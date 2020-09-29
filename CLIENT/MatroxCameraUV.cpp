// MatroxCamera.cpp: implementation of the CMatroxCameraUV class.
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

CMatroxCameraUV::CMatroxCameraUV()
{	
	bSave = false;

	iModeAcquisition=1;
	iSnap=0;

	m_Handle = 0;
	IsFenetreOpened = false;

	lX = 0;
	lY = 0;
	m_pDiag = NULL;
	milDig = 0;
	milSys = 0;

	m_iModeInspection = CAM_MODE_ARRET;

	Simulation_Enable = false;

	m_iMode = 0;
	m_bIsOK = FALSE;

	lNumeroImage = 0;

	m_dOrigine = 0;

	Date_ImageUV = 0;	//V23
}

CMatroxCameraUV::~CMatroxCameraUV()
{
}
#pragma warning(disable : 4995)
BOOL CMatroxCameraUV::LireFromBase(CADODatabase *daoBase, CString sApplication, CString sDossier)
{

	CADORecordset m_Recordset (daoBase);
	CString strNumero;
	strNumero.Format("%ld",m_iNumero);
	m_sRepertoire = sDossier;

	CString strCritere=_T("");
	strCritere = "Numero = " + strNumero + " AND Application = '" + sApplication  + "'";

	if(ACBaseReqRsOpen(strCritere,"[V_Camera]",&m_Recordset))
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
BOOL CMatroxCameraUV::Charger(MIL_ID idSystem)
{
	int i;
	CString sTemp;
	PARAM_ROI RoiTemp;

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
			MdigControl(milDig,M_ROTARY_ENCODER_POSITION_TRIGGER,pParamVisionDiviseur->value);
			MdigControl(milDig,M_ROTARY_ENCODER_POSITION,0);

			lX =(long) MdigInquire(milDig, M_SIZE_X, M_NULL);
			lY =(long) MdigInquire(milDig, M_SIZE_Y, M_NULL);

			Param_Camera.iMode = 1;
			break;
	}

	//IMAGES ACQUISITION
	imgGrab[0].sLibelle = m_sLibelle + " Image Acquisition 0";
	imgGrab[1].sLibelle = m_sLibelle + " Image Acquisition 1";
	if (milDig == 0)
	{
		for(i=0;i<2;i++)
		{
			imgGrab[i].Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
			//sTemp.Format("_%d",i);
			//sTemp = m_sFichier.Left(m_sFichier.GetLength()-4) + sTemp + ".bmp";
			//mFile = sTemp.GetBuffer(sTemp.GetLength());
			MbufLoad(mFile,imgGrab[i].milBuf);
		}
	}
	else
	{
		for(i=0;i<2;i++)
		{
			imgGrab[i].Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_GRAB+M_PROC+M_DISP);
		}
	}

	//V2.30 Plan couleur source1
	imgSrc1.sLibelle = m_sLibelle + " UV SRC1";
	imgSrc1.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//V2.30 Plan couleur source2
	imgSrc2.sLibelle = m_sLibelle + " UV SRC2";
	imgSrc2.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//V2.30 Plan couleur Additionnel
	imgSrcPlus.sLibelle = m_sLibelle + " UV SRCPLUS";
	imgSrcPlus.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//V2.30 Différence des plans couleur
	imgDiff.sLibelle = m_sLibelle + " UV DIFF";
	imgDiff.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//Image binaire de détection des tâches
	imgBin.sLibelle = m_sLibelle + " Image binaire detection taches";
	imgBin.Charger(milSys,lX,lY,1,1+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);


	//Images source par piste
	RoiTemp.X1 = pParamVisionPiste1G_UV->value;
	RoiTemp.X2 = pParamVisionPiste1D_UV->value;
	RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
	RoiTemp.Y1 = 0;
	RoiTemp.dY = lY;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	roiSrc[0][0].Charger(&imgGrab[0]);
	roiSrc[0][0].SetROI(&RoiTemp);
	roiSrc[1][0].Charger(&imgGrab[1]);
	roiSrc[1][0].SetROI(&RoiTemp);
	roiBin[0].Charger(&imgBin);
	roiBin[0].SetROI(&RoiTemp);

	RoiTemp.X1 = pParamVisionPiste2G_UV->value;
	RoiTemp.X2 = pParamVisionPiste2D_UV->value;
	RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
	RoiTemp.Y1 = 0;
	RoiTemp.dY = lY;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	roiSrc[0][1].Charger(&imgGrab[0]);
	roiSrc[0][1].SetROI(&RoiTemp);
	roiSrc[1][1].Charger(&imgGrab[1]);
	roiSrc[1][1].SetROI(&RoiTemp);
	roiBin[1].Charger(&imgBin);
	roiBin[1].SetROI(&RoiTemp);

	RoiTemp.X1 = pParamVisionPiste3G_UV->value;
	RoiTemp.X2 = pParamVisionPiste3D_UV->value;
	RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
	RoiTemp.Y1 = 0;
	RoiTemp.dY = lY;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	roiSrc[0][2].Charger(&imgGrab[0]);
	roiSrc[0][2].SetROI(&RoiTemp);
	roiSrc[1][2].Charger(&imgGrab[1]);
	roiSrc[1][2].SetROI(&RoiTemp);
	roiBin[2].Charger(&imgBin);
	roiBin[2].SetROI(&RoiTemp);

	//Pour l'affichage de la position des pistes
	imgGrabVisu.sLibelle = m_sLibelle + " Image Acquisition Live";
	imgGrabVisu.Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//NOYAUX DE CONVOLUTION ET MORPHOLOGIE
	ker[0].Charger(milSys,3,3);
	ker[1].Charger(milSys,20,20);

	//ACQUISITION
	Param_Acquisition.ThreadId = 0;
   
	// Intitalize threads parameter structures
	Param_Acquisition.iMode				= 0;

	return TRUE;
}

BOOL CMatroxCameraUV::Decharger()
{
	int i,j;

	if(!m_bIsOK) return TRUE;

	BlobUV.Decharger();
	
	//MappControlThread(Param_Acquisition.EventEndId, M_EVENT_FREE, M_DEFAULT, M_NULL);
	if (Param_Acquisition.ThreadId != 0)
	{
		MthrFree(Param_Acquisition.ThreadId) ;
		Param_Acquisition.ThreadId = 0;
	}

	for(i = 0; i < 20; i++)
	{
		Simulation_Roi[i].Decharger();
	}

	Simulation_Image.Decharger();

	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 3; j++)
		{
			roiSrc[i][j].Decharger();
		}
	}
	for(i = 0; i < 2; i++)
	{
		imgGrab[i].Decharger();
	}
	for(i = 0; i < 3; i++)
	{
		roiBin[i].Decharger();
	}

	imgSrc1.Decharger();	//V2.30
	imgSrc2.Decharger();	//V2.30
	imgDiff.Decharger();	//V2.30
	imgSrcPlus.Decharger();	//V2.51
	imgBin.Decharger();

	imgGrabVisu.Decharger();

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
	
	m_bIsOK = FALSE;

	return TRUE;
}

void CMatroxCameraUV::Fenetre_Ouvrir()
{
	theApp.ExecuterFonctionExt(_T("AFF_CAMVIEW"),NULL);
}

void CMatroxCameraUV::Fenetre_Fermer()
{
	m_pDiag = NULL;
	m_Handle = 0;
}

void CMatroxCameraUV::Start()
{
	if(Param_Acquisition.iMode > 0) return;

	Param_Acquisition.iMode = 1;

	lNumeroImage = 0;
	
	BlobUV.MF_System = &milSys;
	BlobUV.Charger(NB_BLOBS_UV_MAX,false);

	MthrAlloc(milSys, M_THREAD, M_DEFAULT, AcquisitionUV, this, &Param_Acquisition.ThreadId);
}

void CMatroxCameraUV::Stop()
{
	if(Param_Acquisition.iMode == 0) return;

	Param_Acquisition.iMode = 0;
	if (milDig > 0)
	{
		MdigControl(milDig, M_GRAB_ABORT , M_DEFAULT );
	}

	Sleep(500);
//	#ifdef MIL10
	if (Param_Acquisition.ThreadId != 0)
	{
		MthrFree(Param_Acquisition.ThreadId) ;
		Param_Acquisition.ThreadId = 0;
	}
//	
//#else
//	MappControlThread(Param_Acquisition.ThreadId,  M_THREAD_FREE, M_DEFAULT, M_NULL);
//#endif

	Sleep(500);
}

void CMatroxCameraUV::AppliquerParametres()
{
	if (milDig != 0)
	{
//		Aviiva.SetParam(&Param_Camera);
	}
}

void CMatroxCameraUV::AfficherData()
{
	/*if (m_pDiag == NULL) return;
	((CDiagCamera*)m_pDiag)->AfficherData();*/
//	((CDiagCamera*)m_pDiag)->MAJScrollBar();
}

#pragma warning(default : 4995)