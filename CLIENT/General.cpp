//BMx

#include "stdafx.h"
#include "General.h"

#include "RecetteVision.h"

void*	MainDiag;
HWND		MainHandle;

CConfig			theConfig;
CBase			theBase;

//Jeux de paramétres
CJeuParametres JParamGeneral;//Table Parametre
CJeuParametres JParamVisionGeneral;//Table Parametre
CJeuParametres JParamDetection;//Aucune sauvegarde




//Paramètres
CParametreLong * pParamLgPulseVision;

CParametreLong * pParamAnglePriseMin;
CParametreLong * pParamAnglePriseMax;

CParametreString * ParamCommandeTestCamera;
CParametreString * pParamDossierExport;
CParametreString * pParamDirBackup;
CParametreString * ParamStrFormat_CSVFile;
CParametreString * ParamStrSACHET_Fichier;		//V36
CParametreString * ParamStrNomServeurOpc;
CParametreLong *   pParamDetailFASTEC;			//V35
CParametreLong   * pParamImpressionType;		//V16 Impression
CParametreString * pParamImpressionImprimante;	//V16 Impression
CParametreString * pParamNomBase;
CParametreString * ParamStrDossierSHOPORD;
CParametreString * ParamStrNomFichierSHOPORD_ACYRUS;
CParametreString * ParamStrIPDMX1;
CParametreString * ParamStrIPDMX2;
CParametreString * ParamStrIPDMX3;
CParametreLong * pParamNbJoursFichiersBDD;
CParametreLong * pParamNbJoursPurgeDossierCSV;
//CParametreLong * ParamLongTest;
CParametreLong * pParamTempsLogout;
CParametreLong * pParamNbCyclePulse;
//CParametreLong * pParamNbTentativesLogin;//V 41-AD plus de gestion du nombre d'erreurs saisie mot de passe

CParametreLong * pParamOffsetPosX;
CParametreLong * pParamOffsetPosY;
CParametreLong * pParamOffsetPosZ;

CParametreLong * pParamVisionAuxIORobot1;	//#2
CParametreLong * pParamVisionAuxIORobot2;	//#2
CParametreLong * pParamVisionAuxIORobot3;	//#2

CParametreLong * pParamVisionDXZoom;
CParametreLong * pParamVisionShowPiste;
CParametreLong * pParamVisionDiviseur;

CParametreLong * pParamVisionPiste1G;
CParametreLong * pParamVisionPiste1D;
CParametreLong * pParamVisionPiste2G;
CParametreLong * pParamVisionPiste2D;
CParametreLong * pParamVisionPiste3G;
CParametreLong * pParamVisionPiste3D;
CParametreLong * pParamVisionCtrlTimeOut;
CParametreLong * pParamVisionValeurTimeOut;
CParametreLong * pParamVisionImageTimeOut;
CParametreLong * pParamVisionOffsetRobotXBlister;
CParametreLong * pParamVisionOffsetRobotXSachet;
CParametreLong * pParamVisionRotation;
CParametreLong * pParamVisionPiste1G_UV;
CParametreLong * pParamVisionPiste1D_UV;
CParametreLong * pParamVisionPiste2G_UV;
CParametreLong * pParamVisionPiste2D_UV;
CParametreLong * pParamVisionPiste3G_UV;
CParametreLong * pParamVisionPiste3D_UV;
CParametreLong * pParamVisionDistance_UV;
CParametreDouble * pParamVisionLigne_ms;
CParametreDouble * pParamVisionTemps_ms;
CParametreLong * pParamVisionSelection;
CParametreLong * pParamVisionTracking;
CParametreLong * pParamVisionSauvegarde;
CParametreDouble * pParamVisionFrequenceBlister;	//V28 : Contrôle vitesse
CParametreDouble * pParamVisionFrequenceSachet;		//V28 : Contrôle vitesse
CParametreDouble * pParamVisionFrequenceRapport;	//V28 : Contrôle vitesse

//Paramètres Port Serie Camera
CParametreString * pParamPariteCOM;
CParametreLong * pParamVitesseCOM;
CParametreLong * pParamBitsDataCOM;
CParametreLong * pParamBitsStopCOM;

CParametreDouble * pParamPoidsEtalon1;
CParametreDouble * pParamPoidsEtalon2;
CParametreDouble * pParamPoidsMinEtalon1;
CParametreDouble * pParamPoidsMinEtalon2;
CParametreDouble * pParamPoidsMaxEtalon1;
CParametreDouble * pParamPoidsMaxEtalon2;

CParametreDouble * pParamSachetNom;
CParametreDouble * pParamSachetMin;
CParametreDouble * pParamSachetMax;
CParametreDouble * pParamBlisterNom;
CParametreDouble * pParamBlisterMin;
CParametreDouble * pParamBlisterMax;
CParametreDouble * pParamSachet4mlNom;
CParametreDouble * pParamSachet4mlMin;
CParametreDouble * pParamSachet4mlMax;

//V37 CParametreLong * pParamForcageInhibitionEtiquette;
//V 44.0 CParametreLong * pParamLectureBoite;

#ifdef XML
CParametreString * pParamRepFichierEtiXml;
CParametreString * pParamNomFichierEtiXml;
#endif

#ifdef ETICONCEPT
//BDD chargement Etiquette (Eticoncept)
CParametreString * pParamRepBaseEticoncept;
CParametreString * pParamNomBaseEticoncept;
CParametreString * pParamProviderBaseEticoncept;
CParametreString * pParamNomTableEticoncept;
CParametreString * pParamNomChampMachine;
CParametreString * pParamNomMachineBL;
//Fin BDD chargement Etiquette (Eticoncept)
#endif
CParametreString * pParamFormatWolke;	//V37
CParametreString * pParamTexteWolke;	//V37
CParametreString * pParamTitreColonneWolke;	//V37

CParametreLong * pParamForcagePreFAT;

//Recette Vision
CRecetteVision m_RecetteVision;

//Vision MIL
MIL_ID	MilApplication;
MIL_ID	MilSystem;

CMatroxApplication Application;
CMatroxSystem Systeme[4];			//SYSTEM_HOST : de [3] à [4] pour systeme HOST
CMatroxCamera theCam;

CMatroxCameraUV theCamUV;//CAM2

CMatroxCameraUeye uEye[2];
CJeuParametres JParamuEye[2];
CParametreLong		* UEye_ImgRotation[2];
CParametreDouble	* UEye_Temps_Expo[2];
CParametreLong		* UEye_Source_Trigger[2];
CParametreLong		* UEye_Delai_Trigger[2];

CParametreDouble * pParamToleranceEtuiBlister;
CParametreDouble * pParamToleranceEtuiSachet;
CParametreDouble * pParamToleranceEtuiSachet4ml;

//V 39 DMX
CParametreLong	* pParamPortEcouteLecteurDMX;

//V 40 ActiveDir
CParametreLong *		pParamAD_ModeLoginUser;
CParametreString *	pParamAD_LDAPHost;
CParametreString *	pParamAD_LDAPDomaine;
CParametreString *	pParamAD_IPServeur;
CParametreString *	pParamAD_MachineName;
CParametreString *	pParamAD_EnteteGroupesAcyrus;

//V 43-ALL Histo. Tapis
CParametreDouble * pParamTapis_DistanceCam_Robot1;
CParametreDouble * pParamTapis_DistanceCam_Robot2;
CParametreDouble * pParamTapis_DistanceCam_Robot3;
//V 44.7
CParametreDouble * pParamCadence_MinMachine;
CParametreLong * pParamModeAffichageResolution;
CParametreDouble * pParamCadence_MinRobot;
CParametreDouble * pParamCadence_Periode;
CParametreDouble * pParamCadence_Timeout;
