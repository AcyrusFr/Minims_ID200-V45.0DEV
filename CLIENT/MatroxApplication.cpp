// MatroxApplication.cpp: implementation of the CMatroxApplication class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4995)
#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialMachine.h"
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

CMatroxApplication::CMatroxApplication()
{
	milApp = 0;
	m_lCompteurType = 0;
	m_iNbGroupe = 0;
	m_iDernierGroupe = 0;

	m_lAVIXSize = 180;
	m_lAVIYSize = 180;
}

CMatroxApplication::~CMatroxApplication()
{
}
#pragma warning(disable : 4995)
BOOL CMatroxApplication::LireFromBase(CADODatabase *daoBase, CString sMachine)
{
	m_StrMachine = sMachine;

	//CDaoRecordset m_Recordset (&theBase.dao_Base);
	CADORecordset m_Recordset(daoBase);

	if (ACBaseStrRsOpen(m_StrMachine,"Machine","V_Application",&m_Recordset) )
	{
		m_StrApplication = ACBaseGetStr("Application",&m_Recordset);
		m_StrInitFlag = ACBaseGetStr("InitFlag",&m_Recordset);

		m_lCompteurType = ACBaseGetLong("CompteurType",&m_Recordset);
		m_lCompteurIndex = ACBaseGetLong("CompteurIndex",&m_Recordset);

		m_dMoyenneInspection = ACBaseGetDouble("MoyInspection",&m_Recordset);

		m_Recordset.Close();

		return TRUE;
	}
	else
	{
		sErrMsg = "ERREUR Application.Lire \nApplication inexistante";
		return FALSE;
	}
}
#pragma warning(default : 4995)
BOOL CMatroxApplication::Charger()
{
	MappAlloc(M_DEFAULT, &milApp);
	return TRUE;
}

BOOL CMatroxApplication::Decharger()
{
	if(milApp > 0)
	{
		MappFree(milApp);
		milApp = 0;
	}

	return TRUE;
}

void CMatroxApplication::Initialiser()
{
}
void CMatroxApplication::GererArret()
{
	//TODO CAM2
	char sFichierDestination[255];
	char sFichier[255];
	theCam.m_iModeInspection = CAM_MODE_ARRET;
	MbufExportSequence(theCam.m_sAVIFile,M_AVI_DIB ,M_NULL,M_NULL,M_NULL,M_CLOSE);

	strcpy_s(sFichierDestination,(LPCTSTR)theConfig.str_RepertoireBase);
	sprintf_s(sFichier,"\\images\\%d.avi",theCam.lCleCoupe);
	strcat_s(sFichierDestination,sFichier);
	/*((CDialMachine*)MainDiag)->AfficherChaineListe(theCam.m_sAVIFile);
	((CDialMachine*)MainDiag)->AfficherChaineListe(sFichierDestination);*/

	CopyFile(theCam.m_sAVIFile,sFichierDestination,FALSE);

}

void CMatroxApplication::GererMarche(long lCleCoupe, char *sArticle, long lCleOF)
{
	theCam.sArticle = sArticle;
	theCam.lCleCoupe = lCleCoupe;
	theCam.lNumeroImage = 0;

	theCam.Options_Sauvegarde = false;
	theCam.Options_Overlay = false;

	//Gestion Lateral
	this->GererLateral = false;

	theCam.lCleOF = lCleOF;
	theCam.AfficherData();

	char sFichier[100];
	//V16AVI
	strcpy_s(theCam.m_sAVIFile,(LPCTSTR)theConfig.str_RepertoireApp);
	sprintf_s(sFichier,"\\images\\%d.avi",lCleCoupe);
	strcat_s(theCam.m_sAVIFile,sFichier);
	MbufExportSequence(theCam.m_sAVIFile,M_AVI_DIB ,M_NULL,M_NULL,M_NULL,M_OPEN);
	theCam.m_lAVIImage = 0;

	//Mode Synchro
	theCam.iSnap=1;	
	theCam.iModeAcquisition = 0;
	theCam.Param_Camera.iMode = 1;
	theCam.AppliquerParametres();

	theCam.m_iModeInspection = CAM_MODE_INSPECTION;
	theCamUV.m_iModeInspection = CAM_MODE_INSPECTION;

}

#pragma warning(default : 4995)