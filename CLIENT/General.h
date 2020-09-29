#include "math.h"

#include "Base.h"
#include "Config.h"
//Parametres
#include "JeuParametres.h"

#include "MatroxApplication.h"
#include "MatroxSystem.h"
#include "MatroxCamera.h"
#include "MatroxCameraUV.h"
#include "MatroxCameraUeye.h"

#include "RecetteVision.h"

//Timers
#define DELAI_TIMER_PROCESS	20
#define DELAI_TIMER_AFFICHAGE	200

#define ID_TIMER_ALARME	3001
#define ID_TIMER_LOGOUT	3002
#define ID_TIMER_PROCESS 3003
#define ID_TIMER_ALIVE 3004
#define ID_TIMER_PULSEVISION 3005
#define ID_TIMER_AFFICHAGE	3006
#define ID_TIMER_MAJ_ENCOURSMACHINE	3007//V2.50
#define ID_TIMER_CALCUL_CADENCE			3008//V 44.8
//V 45.0
#define DELAI_CALCUL_CADENCE			1000 * 10


#ifdef DEBUG//V 44.8
#define CODE_UTILISATEUR_DEFAUT	_T("ACYRUS")
#define PROFIL_UTILISATEUR_DEFAUT _T("MANAGER")
#else
#define CODE_UTILISATEUR_DEFAUT	_T("AUCUN")
#define PROFIL_UTILISATEUR_DEFAUT _T("AUCUN")
#endif // DEBUG



//Formulaires MessagesBox ACYRUS
#define F_MSGBOX_AVERT_OK			_T("MSGBOX_AVERT_OK")
#define F_MSGBOX_QUEST_YN			_T("MSGBOX_AVERT_YN")
#define F_MSGBOX_ERREUR_OK			_T("MSGBOX_ERREUR_OK")

#define	MSG_F_MSGBOX_OK_TITRE_ERREUR	_T("Erreur !")
#define	MSG_F_MSGBOX_OK_TITRE			_T("Avertissement !")
#define	MSG_F_MSGBOX_QUEST_TITRE		_T("Question ?")
#define	MSG_F_MSGBOX_OKCANCEL_TITRE		_T("Avertissement !")


extern void*	MainDiag;
extern HWND		MainHandle;

extern CConfig			theConfig;
extern CBase			theBase;

//Jeux de paramétres
extern CJeuParametres JParamGeneral;//Table Parametre
extern CJeuParametres JParamVisionGeneral;//Table Parametre
extern CJeuParametres JParamDetection;//Aucune sauvegarde

//Paramétres
extern CParametreLong *		pParamLgPulseVision;
extern CParametreLong *		pParamAnglePriseMin;
extern CParametreLong *		pParamAnglePriseMax;
extern CParametreString *	ParamCommandeTestCamera;
extern CParametreString *	pParamDossierExport;
extern CParametreString *	pParamDirBackup;
extern CParametreString *	ParamStrFormat_CSVFile;		//V36
extern CParametreString *	ParamStrSACHET_Fichier;
extern CParametreString *	pParamNomBase;
extern CParametreString	*	ParamStrNomServeurOpc;
extern CParametreLong *		pParamDetailFASTEC;			//V35
extern CParametreLong *		pParamImpressionType;		//V16 Impression
extern CParametreString	*	pParamImpressionImprimante;	//V16 Impression
extern CParametreString *	ParamStrDossierSHOPORD;
extern CParametreString *	ParamStrNomFichierSHOPORD_ACYRUS;
extern CParametreString *	ParamStrIPDMX1;
extern CParametreString *	ParamStrIPDMX2;
extern CParametreString *	ParamStrIPDMX3;
extern CParametreLong *		pParamNbJoursFichiersBDD;
extern CParametreLong *		pParamNbJoursPurgeDossierCSV;
extern CParametreLong *		pParamTempsLogout;
extern CParametreLong *		pParamNbCyclePulse;
//extern CParametreLong *		pParamNbTentativesLogin;

extern CParametreLong * pParamOffsetPosX;
extern CParametreLong * pParamOffsetPosY;
extern CParametreLong * pParamOffsetPosZ;

//BM
extern CParametreLong * pParamVisionAuxIORobot1;	//#2
extern CParametreLong * pParamVisionAuxIORobot2;	//#2
extern CParametreLong * pParamVisionAuxIORobot3;	//#2

extern CParametreLong * pParamVisionDXZoom;
extern CParametreLong * pParamVisionShowPiste;
extern CParametreLong * pParamVisionDiviseur;

extern CParametreLong * pParamVisionPiste1G;
extern CParametreLong * pParamVisionPiste1D;
extern CParametreLong * pParamVisionPiste2G;
extern CParametreLong * pParamVisionPiste2D;
extern CParametreLong * pParamVisionPiste3G;
extern CParametreLong * pParamVisionPiste3D;
extern CParametreLong * pParamVisionCtrlTimeOut;
extern CParametreLong * pParamVisionValeurTimeOut;
extern CParametreLong * pParamVisionImageTimeOut;
extern CParametreLong * pParamVisionOffsetRobotXBlister;
extern CParametreLong * pParamVisionOffsetRobotXSachet;
extern CParametreLong * pParamVisionRotation;

extern CParametreLong * pParamVisionPiste1G_UV;
extern CParametreLong * pParamVisionPiste1D_UV;
extern CParametreLong * pParamVisionPiste2G_UV;
extern CParametreLong * pParamVisionPiste2D_UV;
extern CParametreLong * pParamVisionPiste3G_UV;
extern CParametreLong * pParamVisionPiste3D_UV;
extern CParametreLong * pParamVisionDistance_UV;
extern CParametreDouble * pParamVisionLigne_ms;
extern CParametreDouble * pParamVisionTemps_ms;
extern CParametreLong * pParamVisionSelection;
extern CParametreLong * pParamVisionTracking;

extern CParametreLong * pParamVisionSauvegarde;

extern CParametreDouble * pParamVisionFrequenceBlister;	//V28 : Contrôle vitesse
extern CParametreDouble * pParamVisionFrequenceSachet;	//V28 : Contrôle vitesse
extern CParametreDouble * pParamVisionFrequenceRapport;	//V28 : Contrôle vitesse

extern CRecetteVision m_RecetteVision;

//PAramètre Port Serie Camera
extern CParametreString * pParamPariteCOM;
extern CParametreLong * pParamVitesseCOM;
extern CParametreLong * pParamBitsDataCOM;
extern CParametreLong * pParamBitsStopCOM;

extern CParametreDouble * pParamPoidsEtalon1;
extern CParametreDouble * pParamPoidsEtalon2;
extern CParametreDouble * pParamPoidsMinEtalon1;
extern CParametreDouble * pParamPoidsMinEtalon2;
extern CParametreDouble * pParamPoidsMaxEtalon1;
extern CParametreDouble * pParamPoidsMaxEtalon2;

extern CParametreDouble * pParamSachetNom;
extern CParametreDouble * pParamSachetMin;
extern CParametreDouble * pParamSachetMax;
extern CParametreDouble * pParamBlisterNom;
extern CParametreDouble * pParamBlisterMin;
extern CParametreDouble * pParamBlisterMax;
extern CParametreDouble * pParamSachet4mlNom;
extern CParametreDouble * pParamSachet4mlMin;
extern CParametreDouble * pParamSachet4mlMax;

//V37 extern CParametreLong * pParamForcageInhibitionEtiquette;
//V 44.0 extern CParametreLong * pParamLectureBoite;	//V37

//V 44.0
#define ETICONCEPT	1// XML ou ETICONCEPT
#ifdef XML
extern CParametreString * pParamRepFichierEtiXml;
extern CParametreString * pParamNomFichierEtiXml;
#endif

#ifdef ETICONCEPT
//BDD chargement Etiquette (Eticoncept)
extern CParametreString * pParamRepBaseEticoncept;
extern CParametreString * pParamNomBaseEticoncept;
extern CParametreString * pParamProviderBaseEticoncept;
extern CParametreString * pParamNomTableEticoncept;
extern CParametreString * pParamNomChampMachine;
extern CParametreString * pParamNomMachineBL;
//Fin BDD chargement Etiquette (Eticoncept)
#endif
extern CParametreString * pParamFormatWolke;	//V37
extern CParametreString * pParamTexteWolke;	//V37
extern CParametreString * pParamTitreColonneWolke;	//V37


extern CParametreLong * pParamForcagePreFAT;

//Vision
//MIL
extern MIL_ID	MilApplication;
extern MIL_ID	MilSystem;

extern CMatroxApplication Application;
extern CMatroxSystem Systeme[4];			//SYSTEM_HOST : de [3] à [4] pour systeme HOST
extern CMatroxCamera theCam;

extern CMatroxCameraUV theCamUV;

extern CMatroxCameraUeye uEye[2];
extern CJeuParametres JParamuEye[2];
//Paramètres uEye
extern CParametreLong	* UEye_ImgRotation[2];
extern CParametreDouble * UEye_Temps_Expo[2];
extern CParametreLong	* UEye_Source_Trigger[2];
extern CParametreLong	* UEye_Delai_Trigger[2];

#define ID_CAM_UEYE1	0
#define ID_CAM_UEYE2	1

#define CAM_MODE_ARRET			0
#define CAM_MODE_CALIBRATION	1
#define CAM_MODE_INSPECTION		3

#define PROCESS_TYPE			0		//0=Thread, 1=Processing

//Definitions messages
#define WCMDE_INSPECTION_RESULT	WM_USER + 2
#define WCMDE_MESSAGELISTE		WM_USER + 1
#define WCMDE_MAJMENU		WM_USER + 3
#define WM_PESON_DEFAUT_POIDSMIN	WM_USER + 4
#define WM_PESON_DEFAUT_POIDSMAX	WM_USER + 5
//V 44.7 #define WM_MAJ_COMPTEURS	WM_USER+6
#define WM_PESON_MAJMESURE	WM_USER+7
#define WM_MAJ_ETAT	WM_USER+8
#define WM_MAJ_PREVDIALOG WM_USER+9
//#define ACY_LB_INSERTSTRING WM_USER+10 LisBoxNotify.h
//#define ACY_LB_RESETCONTENT WM_USER+11 LisBoxNotify.h
#define WM_MAJ_VIEWMACHINE WM_USER+12

extern CParametreDouble * pParamToleranceEtuiBlister;
extern CParametreDouble * pParamToleranceEtuiSachet;
extern CParametreDouble * pParamToleranceEtuiSachet4ml;

//V 39 DMX
extern CParametreLong	* pParamPortEcouteLecteurDMX;

//V 40 ActiveDir
#define MODE_BDD	0
#define MODE_AD	1

extern CParametreLong *		pParamAD_ModeLoginUser;/*MODE_BDD:0 ; MODE_AD:1 ;*/
extern CParametreString *	pParamAD_LDAPHost;
extern CParametreString *	pParamAD_LDAPDomaine;
extern CParametreString *	pParamAD_IPServeur;
extern CParametreString *	pParamAD_MachineName;
extern CParametreString *	pParamAD_EnteteGroupesAcyrus;

//V 43-ALL Histo. Tapis
extern CParametreDouble * pParamTapis_DistanceCam_Robot1;
extern CParametreDouble * pParamTapis_DistanceCam_Robot2;
extern CParametreDouble * pParamTapis_DistanceCam_Robot3;

//V 44.7
extern CParametreDouble * pParamCadence_MinMachine;
extern CParametreLong * pParamModeAffichageResolution;
extern CParametreDouble * pParamCadence_MinRobot;
extern CParametreDouble * pParamCadence_Periode;
extern CParametreDouble * pParamCadence_Timeout;