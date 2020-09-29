// BaseFenetre.h : fichier d'en-tête principal pour l'application BaseFenetre
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux

#include "General.h"
#include "Process.h"
// CBaseFenetreApp:
// Consultez BaseFenetre.cpp pour l'implémentation de cette classe
//
#include "Robot.h"
#include "GestAlarmes.h"
#include "ProcessDMX.h"
#include "Recette.h"
#include "Peson.h"
#include "PesonEtui.h"
#include "DalsaSpyder3CL.h"
#include "Journal.h"
#include "MyStrTable.h"
#include "Compteurs.h"//GCA 1412
#include "Etiquette.h"
#include "VisionBoite.h"
#include "afxmt.h"//include Mutex 2.44
//#include "gridctrl.h"

#include "Utilisateur.h"
#include "AcyDial.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib,"..\\..\\..\\..\\..\\CLASSES\\ACYActiveDirectory\\ACY_Login_LDAP.lib")// ou Propriétés du projet-> editeur de liens -> entrée -> dependances supplémentaires : ACY_Login_LDAP.lib
#include "ACYActiveDirectory\ACYActiveDirectory.h"


using namespace AcyCL;


#define NB_MAX_DIALOG	21//V 45.0 old = 20
//Defineitions Noms DOSSIERS
#define NOM_DOSSIER_RAPPORTS	_T("Reports\\")
#define NOM_DOSSIER_ICONES	_T("Icones\\")
#define NOM_DOSSIER_BASE	_T("Base\\")
#define NOM_DOSSIER_IMAGES	_T("Images")

//DEFINITIONS FICHIER CSV
#define TOKEN_FORMAT_CSVFILE	"|"
#define SEP_FORMAT_CSVFILE	"/"

//DEFINITIONS INDEX VOYANTS
#define INDEX_VOY_ROBOT1	0
#define INDEX_VOY_ROBOT2	1
#define INDEX_VOY_ROBOT3	2
#define INDEX_VOY_OPC	3
#define INDEX_VOY_PESON1	4
#define INDEX_VOY_PESON2	5
#define INDEX_VOY_PESON3	6
#define INDEX_VOY_PESON4	7
#define INDEX_VOY_PESON5	8
#define INDEX_VOY_PESON6	9
#define INDEX_VOY_DMXETUI	10
#define INDEX_VOY_DMXNOTICE1	11
#define INDEX_VOY_DMXNOTICE2	12
#define INDEX_VOY_CAM1	13
#define INDEX_VOY_AUTO	14
#define INDEX_VOY_VIDAGE	15
#define INDEX_VOY_MAINT	16
#define INDEX_VOY_CAMUV	17
//#define INDEX_VOY_CAM3	18
//#define INDEX_VOY_CAM4	19
//#define INDEX_VOY_PESAGE	20
//#define INDEX_VOY_WOLKE	21
//#define INDEX_VOY_ETIQUETTE	22
#define INDEX_VOY_PESAGE	18			//V37
//V44.0 #define INDEX_VOY_CAMBOITE	19			//V37
//V44.0 #define INDEX_VOY_IMPWOLKE	20			//V37
//V44.0 #define INDEX_VOY_IMPCELLEMENT	21		//V37
//V44.0 #define INDEX_VOY_LECTWOLKE	22			//V37
//V44.0 #define INDEX_VOY_LECTCELLEMENT	23		//V37
#define INDEX_VOY_IMPWOLKE	19		
#define INDEX_VOY_IMPCELLEMENT	20
#define INDEX_VOY_LECTWOLKE	21		
#define INDEX_VOY_LECTCELLEMENT	22


//Definitions GRoupes
#define GROUPE_RECETTE	0

#define GROUPE_PROCESS_R	1
#define GROUPE_PROCESS_W	2

#define GROUPE_ROBOT1_TW	3
#define GROUPE_ROBOT1_TR	4

#define GROUPE_ROBOT2_TW	5
#define GROUPE_ROBOT2_TR	6

#define GROUPE_ROBOT3_TW	7
#define GROUPE_ROBOT3_TR	8

#define GROUPE_COMPTEURS	9//GCA 1412

#define GROUPE_PESAGE_R	10//GCA 3.7


#define GROUPE_DEFAUTS_AIMMEDIAT	0
#define GROUPE_DEFAUTS_AFCYCLE	1
#define GROUPE_DEFAUTS_ALERTES	2

int ExecuterFonctionExtGeneral(CString strFonction, void * BoutonParent);
class CBaseFenetreApp : public CWinApp
{
public:
	CBaseFenetreApp();
	//variables execution unique
	CMutex     m_Mutex;//V2.44
    bool       m_bLock;


	//Fenêtres
	CFormView * pFormMenu;
	CFormView * pFormStatus;
	CAcyDial * MachineView;
	CAcyDial * CamView;
	CAcyDial * ChargementView;
	CAcyDial * ValidLotView;
	CAcyDial * ParamView;
	CAcyDial * VisionView;
	CAcyDial * InitialisationView;
	CAcyDial * ValidationView;
	CAcyDial * AlarmesView;
	CAcyDial * RobotsView;
	CAcyDial * PesonsView;
	CAcyDial * OPCView;
	CAcyDial * DMXView;

	CAcyDial * UeyeView;
	CAcyDial * PeseeView;
	CAcyDial * EtiqView;
	CAcyDial * DefautPisteView;

	CAcyDial * DialMixup;

	CAcyDial * DialFormulaire;//V 45.0

	CAcyDial* DialMachineEnCours;
	CAcyDial* DialMachinePrev;

	int CamAffEnCours;

	//Groupes OPC
	bool bStopThread;
	CClientOPC m_Groupes[11];
	CClientOPC m_GroupeDefauts[3];

	//ROBOTS
	CRobot m_Robot1;
	CRobot m_Robot2;
	CRobot m_Robot3;

	//Pesons
	CPeson Pesons[6];
	
	//ALARMES
	CGestAlarmes	m_GestAlarmes;

	//DMX
	//#3 DMX CInternetSession* session;
	CProcessDMX	m_DMXEtui;
	CProcessDMX	m_DMXNotice1;
	CProcessDMX	m_DMXNotice2;

	//Process
	CProcess Process;
	UINT_PTR TimerProcess;
	UINT_PTR TimerAlive;//Timer 1sec
	UINT_PTR TimerVision;//Timer 100 ms (pulses vision)
	UINT_PTR TimerEnCoursMachine;//V2.50

	//Recette
	CRecette m_Recette;
	CRecette m_RecetteOld;
	CRecette m_RecetteNew;

	//Utilisateur
	CUtilisateur CurrentUser;
	UINT_PTR TimerLogout;

	bool bDoModeDegrade;//V 40

	//Cameras
	CDalsaSpyder3CL	DalsaCamera1;
	CDalsaSpyder3CL	DalsaCamera2;
	
	//Jeux de parametres
	void InitJeuParam();
	void InitParamuEyes(void);
	//Initialisation Affichage
	void InitFenetres();

	//Alarmes
	//int iAffVoyant[21];//18 voyants + 3 voyant OPC
	//V 44.0 int iAffVoyant[24];//18 voyants + 3 voyant OPC + 2 voyant Wolke + 2 voyant Etiquette
	int iAffVoyant[23];//17 voyants + 3 voyant OPC + 2 voyant Wolke + 2 voyant Etiquette

	//Journal Evenements
	CJournal theJournal;
	//String Table ACYRUS
	CMyStrTable TableMessage;

	//GCA 1412
	//Compteurs
	CCompteurs Compteurs;

	CPesonEtui PesonEtui;
	CEtiquette EtiqScellement;	//V37
	CEtiquette EtiqWolke;		//V37
	CEtiquette *EtiqTest;		//V37
	CVisionBoite VisionBoite;	//V37

	//V34
	CString CB_ListeCodes[100];
	int CB_Iterations[100];
	int CB_NbCodes;

	//tableau dialog pour Hide/dechargement Auto
	int iNbDialog;
	CAcyDial * TabDial[NB_MAX_DIALOG];

	//Formulaires
	int iModeCSDesign;

// Substitutions
public:
	virtual BOOL InitInstance();
// Implémentation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
public:
	int GererAlarmes(void);
	int SetButAlarme(bool bCligno);
	afx_msg void OnLogin();
	afx_msg void OnExit();
	int SetInfosString(CString strInfos);
	int MAJMenu(void);
	int ExecuterFonctionExt(CString strFonction,void * BoutonParent);
	void GererVoyants(void);
	int MAJMenuEtat(void);
	//Thread Lecture OPC
	CWinThread*	m_Thread;
	static UINT CommOPC(LPVOID pParam);
	int StartThreadOPC(void);
	int StopThreadOPC(void);

	int SetEventsJournalStr(CString strMessage);
	int SetDefautsEnCours(CString strMessage);
	int SetAlertesEnCours(CString strMessage);
	int ResetDefautsEnCours();
	int GererSynchro(void);
	void ChargerUtilisateur(CString strNom);
	void ChargerUtilisateurDefaut();
	bool AuthenticateADUser(CString strUser, CString strMotDePasse, CString & strLibelleUser, CStringArray & strListeGroupes, CStringArray & strListeProfils);
	
	
	int InitJeuParamVision(void);
	int SetStringListVision(CString strInfos);
	int SetStringListVisionTemps(CString strInfos);
	int SetStringDefautPiste(CString strInfos, int iPiste);	//V30
	int ClearListVision();
	int BackupBase(void);
	bool PurgeDirNbJours(int NbJours, CString str);
	bool CopyDossier(CString strSrc, CString strDest);

	bool SetEncoursMachine();

	void StartTimerProcess();	//V18

	//V45.0 uniformisation des fopnctions login
	bool Login_DoVerification(CString strLogin, CString strMdp, CUtilisateur * UtilisateurEnCours);

	bool DemandeLoginManager(CString &strUser,CString &strDate);
	bool DemandeLogin(CString &strUser,CString &strDate);
	bool Login_IsDevelopperUser(CString strUtilisateur, CString strMotDePasse);
	bool Login_DoSaisieIdentifiants(CString & strLogin, CString & strMdp);
};

extern CBaseFenetreApp theApp;