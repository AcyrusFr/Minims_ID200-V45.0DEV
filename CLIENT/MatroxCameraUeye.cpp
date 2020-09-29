// MatroxCamera.cpp: implementation of the CMatroxCameraUeye class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseFenetre.h"
#include "General.h"

#include "AC_Base.h"
#include "Vision.h"
#include "MatroxCameraUeye.h"

#include "DialCameras.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatroxCameraUeye::CMatroxCameraUeye()
{	
	bSave = false;
	
	iModeAcquisition=1;
	iSnap=0;

	m_Handle = 0;
	IsFenetreOpened = false;

	lX = 0;
	lY = 0;
	m_pDiag = NULL;
	milSys = 0;

	m_iModeInspection = CAM_MODE_ARRET;

	m_bIsOK = FALSE;

	lNumeroImage = 0;

//Positionnement physique	
	m_dOrigine = 0;
	bFromFile = false; 
	
	m_iNumuEye = 0;
	iAdresseuEye = 0;
}

CMatroxCameraUeye::~CMatroxCameraUeye()
{
}
#pragma warning(disable : 4995)
BOOL CMatroxCameraUeye::LireFromBase(CADODatabase *daoBase, CString sApplication, CString sDossier)
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

		//V42 Param_Camera.lNumSerial = ACBaseGetLong("CLSerial",&m_Recordset);
		Param_Camera.iFormat = ACBaseGetInt("CLFormat",&m_Recordset);
		//Param_Camera.pParamGain->value = ACBaseGetInt("CLGain",&m_Recordset);
		Param_Camera.iGain = ACBaseGetInt("CLGain",&m_Recordset);
		//V42 Param_Camera.iGainInit = Param_Camera.iGain;
		Param_Camera.iMode = ACBaseGetInt("CLMode",&m_Recordset);
		//Param_Camera.pParamExpo->value = ACBaseGetInt("CLTempsExpo",&m_Recordset);
		Param_Camera.iExpo = ACBaseGetInt("CLTempsExpo",&m_Recordset);
		//V42 Param_Camera.iTempsExpoInit = Param_Camera.iExpo;

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
BOOL CMatroxCameraUeye::Charger(MIL_ID idSystem)
{
	CString sTemp;

	milSys = idSystem;
	sTemp = m_sRepertoire + _T("\\") + m_sFichier;
	MIL_TEXT_PTR mFile = sTemp.GetBuffer(sTemp.GetLength());

	bool bOpenCam = false;
	switch(m_iType)
	{
		case 1:	//Fichier
			lX = (long)MbufDiskInquire(mFile,M_SIZE_X,M_NULL);
			lY = (long)MbufDiskInquire(mFile,M_SIZE_Y,M_NULL);
			break;

		case 3:	//µEye
			lX = (long)MbufDiskInquire(mFile,M_SIZE_X,M_NULL);
			lY = (long)MbufDiskInquire(mFile,M_SIZE_Y,M_NULL);

			((CDialCameras * )theApp.UeyeView)->m_nRenderMode = IS_RENDER_FIT_TO_WINDOW;

			bOpenCam = ((CDialCameras * )theApp.UeyeView)->OpenUeyeCam(m_iNumuEye,iAdresseuEye);

			if(!bOpenCam)
			{
				m_hCam = -1;
				m_nBytesPerPixel = 1;
			}
			break;
	}

	//IMAGES ACQUISITION
	imgGrabuEye.sLibelle.Format(_T("uEye%d Image Acquisition"),m_iNumero);
	if((m_iType == 3)&&(m_nBytesPerPixel >=3))
		imgGrabuEye.Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	else
		imgGrabuEye.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	
	//V 45.0 chargement fichier uniquement si la camera n'est pas connectée
	if (!bOpenCam)
	{
		sTemp = m_sFichier;
		CString str;
		str = m_sRepertoire + _T("\\") + sTemp;
		mFile = str.GetBuffer(str.GetLength());
		MbufLoad(mFile, imgGrabuEye.milBuf);
		str.ReleaseBuffer();
	}

	//Calibration ImgGrab
	if(imgGrabuEye.Calibration > 0) delete(imgGrabuEye.Calibration);

	imgGrabuEye.Calibration = new CCalibration(milSys);
	imgGrabuEye.Calibration->NomFichier.Format(_T("Calibration%d.txt"),m_iNumero);
	imgGrabuEye.Calibration->ChargerFromFile(theConfig.str_RepertoireApp, imgGrabuEye.Calibration->NomFichier);
	
	//ACQUISITION
	Param_Acquisition.ThreadId = 0;
   
	// Intitalize threads parameter structures
	Param_Acquisition.iMode				= 0;

	//gestion particulière µEye
	if(m_iType == 3)
	{
		if(m_hCam == -1)
			return FALSE;
	}

	return TRUE;
}

BOOL CMatroxCameraUeye::Decharger()
{

	if(!m_bIsOK) return TRUE;

//	MappControlThread(Param_Acquisition.EventEndId, M_EVENT_FREE, M_DEFAULT, M_NULL);
	if(Param_Acquisition.ThreadId > 0) 
	{
		MthrFree(Param_Acquisition.ThreadId) ;
		Param_Acquisition.ThreadId = 0;
	}

	//V 44.8 imgGrab.DechargerAll();
	if (imgGrabuEye.Calibration > 0) delete(imgGrabuEye.Calibration);

	//N.U imgRoiGrab.Decharger();
	imgGrabuEye.Decharger();
	

	/*TEST IMAGE MASQUE
	MaskImage.Decharger();*/


	//GEstion µEye
	if((m_iType == 3)&&(m_hCam != -1))
	{
		//Sauvegarde des paramètres de la camera
		INT nRet = -1;

		//V 44.3 
#if UEYE_VERSION_CODE <= UEYE_VERSION(4, 20, 0)
		nRet = is_SaveParameters(m_hCam, "\\cam\\set1");
#else
		is_ParameterSet(m_hCam, IS_PARAMETERSET_CMD_SAVE_EEPROM, ".\\cam\\set1", (UINT)strlen(".\\cam\\set1"));
#endif
//#if UEYE_VERSION_CODE == UEYE_VERSION(4, 70, 0)
//		is_ParameterSet(m_hCam,IS_PARAMETERSET_CMD_SAVE_EEPROM,".\\cam\\set1",(UINT) strlen(".\\cam\\set1"));
//#elif UEYE_VERSION_CODE == UEYE_VERSION(4, 30, 0)
//		is_ParameterSet(m_hCam,IS_PARAMETERSET_CMD_SAVE_EEPROM,".\\cam\\set1",(UINT) strlen(".\\cam\\set1"));
//#else
//		nRet = is_SaveParameters(m_hCam,"\\cam\\set1");
//#endif
		((CDialCameras * )theApp.UeyeView)->StopGrabUeyeCam(m_iNumuEye);
	}

	m_bIsOK = FALSE;

	return TRUE;
}

void CMatroxCameraUeye::Fenetre_Ouvrir()
{
	theApp.ExecuterFonctionExt(_T("AFF_CAMVIEW"),NULL);
}

void CMatroxCameraUeye::Fenetre_Fermer()
{
	m_pDiag = NULL;
	m_Handle = 0;
}

void CMatroxCameraUeye::Stop()
{
	//GEstion µEye
	if((m_iType == 3)&&(m_hCam != -1))
	{
		((CDialCameras * )theApp.UeyeView)->AcquisitionSoftTriggerCam(m_iNumuEye);
		iSnap = 1;
	}

	if(Param_Acquisition.iMode == 0) return;

	iSnap = 1;
	iModeAcquisition = 0;
	Param_Acquisition.iMode = 0;

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
}

void CMatroxCameraUeye::AppliquerParametres()
{
 	SetTempsExpo(UEye_Temps_Expo[m_iNumuEye]->value);
	SetTriggerDelai(UEye_Delai_Trigger[m_iNumuEye]->value);
	SetTriggerSource(UEye_Source_Trigger[m_iNumuEye]->value);
	SetTriggerTimeOut(36000000);
	SetFlash(IO_FLASH_MODE_CONSTANT_HIGH);
	is_CaptureVideo(m_hCam, IS_DONT_WAIT);
}

void CMatroxCameraUeye::Snap()
{
	iModeAcquisition = 1;
	if (m_hCam != -1)
	{
		SetTriggerSource(IS_SET_TRIGGER_SOFTWARE);
		is_StopLiveVideo(m_hCam, IS_WAIT);			//Débloque l'acquisition en cas d'attente de trigger
		is_FreezeVideo(m_hCam,IS_DONT_WAIT);
		return;
	}
	if((m_iType == 1) || (bFromFile))	
	{
		iSnap = 1;
	}
}
void CMatroxCameraUeye::Freerun()
{
	//V53
	if (m_hCam != -1)
	{
		iModeAcquisition = 1;
		SetTriggerSource(IS_SET_TRIGGER_SOFTWARE);
		is_StopLiveVideo(m_hCam, IS_WAIT);			//Débloque l'acquisition en cas d'attente de trigger
		is_CaptureVideo(m_hCam, IS_DONT_WAIT);
		return;
	}
	if((m_iType == 1) || (bFromFile))
	{
		iSnap = 1;
		iModeAcquisition = 0;
	}
}

void CMatroxCameraUeye::SetFlash(int iVal)
{
	//IO_FLASH_MODE_OFF                   0
	//IO_FLASH_MODE_TRIGGER_LO_ACTIVE     1
	//IO_FLASH_MODE_TRIGGER_HI_ACTIVE     2
	//IO_FLASH_MODE_CONSTANT_HIGH         3
	//IO_FLASH_MODE_CONSTANT_LOW          4
	//IO_FLASH_MODE_FREERUN_LO_ACTIVE     5
	//IO_FLASH_MODE_FREERUN_HI_ACTIVE     6
	Param_Camera.Flash = (long) iVal;
	UINT nMode = iVal;
	if (m_hCam != -1)
	{
		is_IO(m_hCam, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode));
	}
}

void CMatroxCameraUeye::SetTriggerSource(int iSource)
{
	//IS_SET_TRIGGER_HI_LO                (IS_SET_TRIGGER_CONTINUOUS | 0x0001) 
	//IS_SET_TRIGGER_LO_HI                (IS_SET_TRIGGER_CONTINUOUS | 0x0002) 
	//IS_SET_TRIGGER_SOFTWARE             (IS_SET_TRIGGER_CONTINUOUS | 0x0008) 
	Param_Camera.Source_Trigger = iSource;
	if (m_hCam != -1)
	{
		is_SetExternalTrigger(m_hCam, (int)Param_Camera.Source_Trigger);
	}
}

void CMatroxCameraUeye::SetTriggerDelai(int iValµs)
{
	Param_Camera.Delai_Trigger = iValµs;
	if (m_hCam != -1)
	{
		is_SetTriggerDelay (m_hCam, (int)Param_Camera.Delai_Trigger);
	}
}

void CMatroxCameraUeye::SetTriggerTimeOut(int iVal)
{
	Param_Camera.Trigger_TimeOut = iVal;
	if (m_hCam != -1)
	{
		is_SetTimeout(m_hCam, IS_TRIGGER_TIMEOUT, (int)Param_Camera.Trigger_TimeOut);
	}
}

void CMatroxCameraUeye::SetTempsExpo(double dVal_ms)
{
	Param_Camera.Temps_Expo = dVal_ms;
	if (m_hCam != -1)
	{
		is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &Param_Camera.Temps_Expo, sizeof(double));
	}
}

void CMatroxCameraUeye::AfficherData()
{
	/*if (m_pDiag == NULL) return;
	((CDiagCamera*)m_pDiag)->AfficherData();*/
//	((CDiagCamera*)m_pDiag)->MAJScrollBar();
}
void CMatroxCameraUeye::Start()
{
//	int i;
	if(Param_Acquisition.iMode > 0) return;

	Param_Acquisition.iMode = 1;

	lNumeroImage = 0;

	MthrAlloc(milSys, M_THREAD, M_DEFAULT, AcquisitionUeye, this, &Param_Acquisition.ThreadId);
	MthrControl(Param_Acquisition.ThreadId,M_THREAD_PRIORITY ,M_TIME_CRITICAL);
}

