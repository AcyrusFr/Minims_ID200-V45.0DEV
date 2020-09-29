// BaseFenetre.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "splitfrm.h"

#include "BaseFenetreDoc.h"
#include "BaseFenetreView.h"

#include "Form_DalsaSpyder.h"
#include "FormStatus.h"
#include "FormBase.h"
#include "FormMenuMachine.h"
#include "DialParam.h"
#include "DialAlarmes.h"
#include "DialVision.h"
#include "DialRobot.h"
#include "DialPesons.h"
#include "DialValidLot.h"
#include "DialMachine.h"
#include "DialGrpOPC.h"
#include "DialChargement.h"
#include "DialMixup.h"
#include "DialInitialisation.h"
#include "DialValidation.h"
#include "DialDMX.h"
//#include "DialAffGridRecette.h"
#include "DialCameras.h"
#include "DialPesee.h"
#include "DialEtiquette.h"
#include "DialDefautPiste.h"

#include "DiagPadAlpha.h"
#include "DiagPadNum.h"
//#include "DialSelectPrint.h"
#include "DiagListeReq.h"
#include "AC_Base.h"
#include "DialDefautPiste.h"	//V30
#include "AC_Util.h"//V 40 

#include "DialFormulaire.h" //V 45.0 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

// Implémentation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// Commande App pour exécuter la boîte de dialogue
void CBaseFenetreApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
// CBaseFenetreApp

BEGIN_MESSAGE_MAP(CBaseFenetreApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CBaseFenetreApp::OnAppAbout)
	ON_COMMAND(ID_LOGIN, &CBaseFenetreApp::OnLogin)
	ON_COMMAND(ID_APP_EXIT, &CBaseFenetreApp::OnExit)
END_MESSAGE_MAP()


// construction CBaseFenetreApp
int ExecuterFonctionExtGeneral(CString strFonction, void * BoutonParent)
{
	return theApp.ExecuterFonctionExt(strFonction, BoutonParent);
}

CBaseFenetreApp::CBaseFenetreApp()
:m_Mutex(FALSE,"ACYRUS_VISION"),
DalsaCamera1(true),
DalsaCamera2(true)
{

	// Placez toutes les initialisations significatives dans InitInstance
	CamAffEnCours = 1;
	pFormMenu = NULL;
	pFormStatus = NULL;
	CamView = NULL;
	ParamView = NULL;
	AlarmesView = NULL;
	VisionView = NULL;
	RobotsView = NULL;
	TimerLogout = NULL;
	TimerProcess = NULL;

	for(int i = 0; i < 14; i++)
	{
		iAffVoyant[i] = 0;
	}

	Pesons[0].iNumPeson = 1;
	Pesons[1].iNumPeson = 2;
	Pesons[2].iNumPeson = 3;
	Pesons[3].iNumPeson = 4;
	Pesons[4].iNumPeson = 5;
	Pesons[5].iNumPeson = 6;

	Pesons[0].iNumPesonBL = 11;
	Pesons[1].iNumPesonBL = 12;
	Pesons[2].iNumPesonBL = 21;
	Pesons[3].iNumPesonBL = 22;
	Pesons[4].iNumPesonBL = 31;
	Pesons[5].iNumPesonBL = 32;

	PesonEtui.iNumPeson = 7;
	PesonEtui.iNumPesonBL = 41;

	m_DMXEtui.iNumDMX = 1;
	m_DMXNotice1.iNumDMX = 2;
	m_DMXNotice2.iNumDMX = 3;
	
	//session = NULL;

	bDoModeDegrade = false;//V 40

	iModeCSDesign = 0;
}


// Seul et unique objet CBaseFenetreApp

CBaseFenetreApp theApp;


// initialisation de CBaseFenetreApp

BOOL CBaseFenetreApp::InitInstance()
{
	// InitCommonControlsEx() est requis sur Windows XP si le manifeste de l'application
	// spécifie l'utilisation de ComCtl32.dll version 6 ou ultérieure pour activer les
	// styles visuels.  Dans le cas contraire, la création de fenêtres échouera.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// À définir pour inclure toutes les classes de contrôles communs à utiliser
	// dans votre application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialiser les bibliothèques OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	//on verifie que se soit la seule execution de l'application//V2.44
	BOOL Bres = m_Mutex.Lock(0);
	
	Bres == 1 ? m_bLock = true: m_bLock = false;
	if(!m_bLock)
    {
		AfxMessageBox(_T("Application en cours d'éxécution, Arrêter le programme en cours ou Attendez la fin de son éxécution"));
		PostQuitMessage(0);
        return(0);
    }
	// Initialisation standard
	// Si vous n'utilisez pas ces fonctionnalités et que vous souhaitez réduire la taille
	// de votre exécutable final, vous devez supprimer ci-dessous
	// les routines d'initialisation spécifiques dont vous n'avez pas besoin.
	// Changez la clé de Registre sous laquelle nos paramètres sont enregistrés
	// modifiez cette chaîne avec des informations appropriées,
	// telles que le nom de votre société ou organisation
	//SetRegistryKey(_T("Applications locales générées par AppWizard"));
	//LoadStdProfileSettings(0);  // Charge les options de fichier INI standard (y compris les derniers fichiers utilisés)

	//#2 session = new CInternetSession();
	                    
	// Inscrire les modèles de document de l'application. Ces modèles
	//  lient les documents, fenêtres frame et vues entre eux
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBaseFenetreDoc),
		RUNTIME_CLASS(CSplitFrame),       // fenêtre frame SDI principale
		RUNTIME_CLASS(CBaseFenetreView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	//Initialisation des parametres
	//V18
	//InitJeuParam();
	//InitJeuParamVision();

	//Initialisation Composants COM
	HRESULT hr = CoInitialize(NULL);
	if(hr == S_OK)
		return FALSE;

	
	// Analyser la ligne de commande pour les commandes shell standard, DDE, ouverture de fichiers
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Commandes de dispatch spécifiées sur la ligne de commande. Retournent FALSE si
	// l'application a été lancée avec /RegServer, /Register, /Unregserver ou /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	//V 44.8
	iModeCSDesign = 0;
	CString strCommandeLancement = CString(m_lpCmdLine);

	if (strCommandeLancement.Find(_T("-DESIGN")) != -1)
		iModeCSDesign = 1;
		
	// La seule fenêtre a été initialisée et peut donc être affichée et mise à jour
	m_pMainWnd->ShowWindow(SW_HIDE);
	m_pMainWnd->UpdateWindow();

	Process.iEtat = ETAT_PROCESS_INITAPP;

	StartTimerProcess();
	

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	
	return TRUE;
}


int CBaseFenetreApp::InitJeuParamVision(void)
{

	//Initialisation des jeux de parametres
	JParamVisionGeneral.strTitre = _T("Paramètres Vision");
	JParamVisionGeneral.TypeTraca = JOURNAL_TYPE_JPARAM_VISION;
	
	if (theConfig.str_Machine.Find(_T("#2")) >= 0)//V 30-ALL
	{
		JParamVisionGeneral.AjouterParametreLong(_T("TOP Synchro AUX_IO Robot 1"),9,0,31);	//#2
		JParamVisionGeneral.AjouterParametreLong(_T("TOP Synchro AUX_IO Robot 2"),2,0,31);	//#2
		JParamVisionGeneral.AjouterParametreLong(_T("TOP Synchro AUX_IO Robot 3"),3,0,31);	//#2
	}
	else
	{
		JParamVisionGeneral.AjouterParametreLong(_T("TOP Synchro AUX_IO Robot 1"),8,0,31);	//#1
		JParamVisionGeneral.AjouterParametreLong(_T("TOP Synchro AUX_IO Robot 2"),9,0,31);	//#1
		JParamVisionGeneral.AjouterParametreLong(_T("TOP Synchro AUX_IO Robot 3"),2,0,31);	//#1
	}
	
	JParamVisionGeneral.AjouterParametreLong("Vision_DXZoom",600,0,2000);
	JParamVisionGeneral.AjouterParametreLong("Vision_ShowPiste",0,0,1);
	JParamVisionGeneral.AjouterParametreLong("Vision_Diviseur",15,0,30);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste1G",0,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste1D",460,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste2G",602,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste3G",1405,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste2D",1260,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste3D",2048,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_CtrlTimeOut",1,0,1);
	JParamVisionGeneral.AjouterParametreLong("Vision_ValeurTimeOut",10,-1,500);
	JParamVisionGeneral.AjouterParametreLong("Vision_ImageTimeOut",450,-1,1000);
	JParamVisionGeneral.AjouterParametreLong("Vision_OffsetRobotXBlister",0,-100,100);
	JParamVisionGeneral.AjouterParametreLong("Vision_OffsetRobotXSachet",0,-100,100);
	JParamVisionGeneral.AjouterParametreLong("Vision_Rotation",0,0,1);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste1G_UV",0,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste1D_UV",200,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste2G_UV",300,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste3G_UV",600,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste2D_UV",800,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Piste3D_UV",1000,0,4096);
	JParamVisionGeneral.AjouterParametreLong("Vision_Distance_UV",2000,0,100000);
	JParamVisionGeneral.AjouterParametreDouble("Vision_Ligne_ms",0.3,0,1000);
	JParamVisionGeneral.AjouterParametreDouble("Vision_Temps_ms",500,0,1000);
	JParamVisionGeneral.AjouterParametreLong("Vision_Selection",0,0,1);
	JParamVisionGeneral.AjouterParametreLong("Vision_Tracking",0,0,1);
	JParamVisionGeneral.AjouterParametreLong("Vision_Sauvegarde",0,0,1);
	JParamVisionGeneral.AjouterParametreDouble("Vision_FrequenceBlister",55,0,200);					//V28 : Contrôle vitesse
	JParamVisionGeneral.AjouterParametreDouble("Vision_FrequenceSachet",55,0,200);					//V28 : Contrôle vitesse
	JParamVisionGeneral.AjouterParametreDouble("Vision_Rapport Hz / Temps Image",12980,0,20000);	//V28 : Contrôle vitesse


	//V 43-ALL Histo. Tapis
	pParamTapis_DistanceCam_Robot1 = JParamVisionGeneral.AjouterDirectParametreDouble(_T("Tapis Distance Camera Robot1 (m)"),0.370,0,5.0);
	pParamTapis_DistanceCam_Robot2 = JParamVisionGeneral.AjouterDirectParametreDouble(_T("Tapis Distance Camera Robot2 (m)"),1.050,0,5.0);
	pParamTapis_DistanceCam_Robot3 = JParamVisionGeneral.AjouterDirectParametreDouble(_T("Tapis Distance Camera Robot3 (m)"),1.730,0,5.0);

	JParamVisionGeneral.ChargerFromBase("Parametre");//Chargement des valeurs dans la BDD
	
	pParamVisionAuxIORobot1 = (CParametreLong * ) JParamVisionGeneral.GetParam(_T("TOP Synchro AUX_IO Robot 1"));	//#2 //V 30-ALL
	//pParamVisionAuxIORobot1->SelectionChamp = _T("2/P0_TTL_AUX_IO0 - M_USER_BIT2 (DB9-1);3/P0_TTL_AUX_IO1 - M_USER_BIT3(DB44-13);4/TTL_AUX_IO0 - M_USER_BIT4(DB44-43);5/TTL_AUX_IO1 - M_USER_BIT5(DB44-15);");
	//pParamVisionAuxIORobot1->iSelection = SAISIE_LISTEPREDEF;
	//pParamVisionAuxIORobot1->InitSelectionChamp();
	pParamVisionAuxIORobot2 = (CParametreLong * ) JParamVisionGeneral.GetParam(_T("TOP Synchro AUX_IO Robot 2"));	//#2 //V 30-ALL
	//pParamVisionAuxIORobot2->SelectionChamp = _T("2/P0_TTL_AUX_IO0 - M_USER_BIT2 (DB9-1);3/P0_TTL_AUX_IO1 - M_USER_BIT3(DB44-13);4/TTL_AUX_IO0 - M_USER_BIT4(DB44-43);5/TTL_AUX_IO1 - M_USER_BIT5(DB44-15);");
	//pParamVisionAuxIORobot2->iSelection = SAISIE_LISTEPREDEF;
	//pParamVisionAuxIORobot2->InitSelectionChamp();
	pParamVisionAuxIORobot3 = (CParametreLong * ) JParamVisionGeneral.GetParam(_T("TOP Synchro AUX_IO Robot 3"));	//#2 //V 30-ALL
	/*pParamVisionAuxIORobot3->SelectionChamp = _T("2/P0_TTL_AUX_IO0 - M_USER_BIT2 (DB9-1);3/P0_TTL_AUX_IO1 - M_USER_BIT3(DB44-13);4/TTL_AUX_IO0 - M_USER_BIT4(DB44-43);5/TTL_AUX_IO1 - M_USER_BIT5(DB44-15);");
	pParamVisionAuxIORobot3->iSelection = SAISIE_LISTEPREDEF;
	pParamVisionAuxIORobot3->InitSelectionChamp();*/


	pParamVisionDXZoom = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_DXZoom");
	pParamVisionShowPiste = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_ShowPiste");
	pParamVisionDiviseur = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Diviseur");
	pParamVisionPiste1G = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste1G");
	pParamVisionPiste1D = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste1D");
	pParamVisionPiste2G = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste2G");
	pParamVisionPiste2D = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste2D");
	pParamVisionPiste3G = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste3G");
	pParamVisionPiste3D = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste3D");
	pParamVisionCtrlTimeOut = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_CtrlTimeOut");
	pParamVisionValeurTimeOut = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_ValeurTimeOut");
	pParamVisionImageTimeOut = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_ImageTimeOut");
	pParamVisionOffsetRobotXBlister = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_OffsetRobotXBlister");
	pParamVisionOffsetRobotXSachet  = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_OffsetRobotXSachet");
	pParamVisionRotation  = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Rotation");

	pParamVisionPiste1G_UV = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste1G_UV");
	pParamVisionPiste1D_UV = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste1D_UV");
	pParamVisionPiste2G_UV = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste2G_UV");
	pParamVisionPiste2D_UV = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste2D_UV");
	pParamVisionPiste3G_UV = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste3G_UV");
	pParamVisionPiste3D_UV = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Piste3D_UV");
	pParamVisionDistance_UV = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Distance_UV");
	pParamVisionLigne_ms = (CParametreDouble * ) JParamVisionGeneral.GetParam("Vision_Ligne_ms");
	pParamVisionTemps_ms = (CParametreDouble * ) JParamVisionGeneral.GetParam("Vision_Temps_ms");
	pParamVisionSelection = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Selection");
	pParamVisionTracking = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Tracking");

	pParamVisionSauvegarde = (CParametreLong * ) JParamVisionGeneral.GetParam("Vision_Sauvegarde");

	pParamVisionFrequenceBlister = (CParametreDouble * ) JParamVisionGeneral.GetParam("Vision_FrequenceBlister");				//V28
	pParamVisionFrequenceSachet = (CParametreDouble * ) JParamVisionGeneral.GetParam("Vision_FrequenceSachet");					//V28
	pParamVisionFrequenceRapport = (CParametreDouble * ) JParamVisionGeneral.GetParam("Vision_Rapport Hz / Temps Image");		//V28
	return 0;
}
void CBaseFenetreApp::InitJeuParam()
{
	//Initialisation des jeux de parametres
	JParamGeneral.strTitre = _T("Paramètres Généraux");
	JParamGeneral.TypeTraca = JOURNAL_TYPE_JPARAM_GENERAL;

	JParamGeneral.AjouterParametreLong("Temps_Logout_Auto",10,1,100);
//	JParamGeneral.AjouterParametreLong("NbTentatives_Login",4,1,10);
	JParamGeneral.AjouterParametreString("Nom_BaseSQL","acyrus_bl");	//V2.58
	JParamGeneral.AjouterParametreString("Nom_Serveur_OPC","DllTestSvr");
	JParamGeneral.AjouterParametreLong("OPC : Détail défauts FASTEC",FALSE,FALSE,TRUE);		///35
	JParamGeneral.AjouterParametreLong("Fin OF : Type impression",0,0,2);				//V16
	JParamGeneral.AjouterParametreString("Fin OF : Imprimante par défaut","Default");	//V16
	JParamGeneral.AjouterParametreString("Commande_Test_Camera","gsl");//Cmd Camera Get Status LED
	JParamGeneral.AjouterParametreString("Nom_Dossier_Backup","C:\\ACYBL_Backup");
	JParamGeneral.AjouterParametreString("Nom_Dossier_Export","");//Valeur par défaut vide pour ne pas faire l'export par défaut
	JParamGeneral.AjouterParametreLong("Nb_Cycle_Pulse",1,1,100);
	JParamGeneral.AjouterParametreString("Nom_Dossier_SHOPORD","C:\\ACYBL_Base\\ShopOrder");
	JParamGeneral.AjouterParametreString("Nom_FichierCSV_ACYRUS","SHOPORD_ACYRUS.csv");
	JParamGeneral.AjouterParametreString("Str_Format_Remplacement_CSV","\"/|,/;");
	JParamGeneral.AjouterParametreString("SACHETS (liste) : fichier .csv","");	//V36

	//Paramétre Camera (Port Serie)
	JParamGeneral.AjouterParametreLong("Vitesse_COM",9600,0,115000);
	JParamGeneral.AjouterParametreString("Parite_COM","N");
	JParamGeneral.AjouterParametreLong("Nb_BitsData_COM",8,7,8);
	JParamGeneral.AjouterParametreLong("Nb_BitsStop_COM",1,1,2);

	//Paramétre Poids Etalon 1/2
	JParamGeneral.AjouterParametreDouble("Poids_Etalon_1",1.,0.,100.);
	JParamGeneral.AjouterParametreDouble("Poids_Etalon_2",3.,0.,100.);
	JParamGeneral.AjouterParametreDouble("Poids_Min_Etalon_1",0.9,0.,100.);
	JParamGeneral.AjouterParametreDouble("Poids_Min_Etalon_2",2.9,0.,100.);
	JParamGeneral.AjouterParametreDouble("Poids_Max_Etalon_1",1.1,0.,100.);
	JParamGeneral.AjouterParametreDouble("Poids_Max_Etalon_2",3.1,0.,100.);

	//Adressses IP Lecteur DMX
	JParamGeneral.AjouterParametreString("IP_LecteurDMX_Etui","192.168.10.31");
	JParamGeneral.AjouterParametreString("IP_LecteurDMX_Notice1","192.168.10.32");
	JParamGeneral.AjouterParametreString("IP_LecteurDMX_Notice2","192.168.10.33");
	JParamGeneral.AjouterParametreLong("Port Ecoute Socket", 10000, 100, 65535);

	//Paramétre Poids Min/Max
	JParamGeneral.AjouterParametreDouble("Sachet_Nom",1.6,0.,10.);
	JParamGeneral.AjouterParametreDouble("Sachet_Min",1.4,0.,10.);
	JParamGeneral.AjouterParametreDouble("Sachet_Max",1.8,0.,10.);
	JParamGeneral.AjouterParametreDouble("Blister_Nom",1.8,0.,10.);
	JParamGeneral.AjouterParametreDouble("Blister_Min",1.6,0.,10.);
	JParamGeneral.AjouterParametreDouble("Blister_Max",2,0.,10.);
	JParamGeneral.AjouterParametreDouble("Sachet4ml_Nom",1.8,0.,10.);
	JParamGeneral.AjouterParametreDouble("Sachet4ml_Min",1.6,0.,10.);
	JParamGeneral.AjouterParametreDouble("Sachet4ml_Max",2,0.,10.);

	JParamGeneral.AjouterParametreLong("offset_Pos_X",0,-10000,10000);
	JParamGeneral.AjouterParametreLong("offset_Pos_Y",0,-10000,10000);
	JParamGeneral.AjouterParametreLong("offset_Pos_Z",0,-360,360);

	JParamGeneral.AjouterParametreLong("Angle_Prise_Min",-360,-360,360);
	JParamGeneral.AjouterParametreLong("Angle_Prise_Max",360,-360,360);

	JParamGeneral.AjouterParametreLong("Lg_Pulse_Vision_ms",120,0,400);
	JParamGeneral.AjouterParametreLong("Nb_Jours_Fichiers_BDD",15,1,365);
	JParamGeneral.AjouterParametreLong("Nb_Jours_Fichiers_CSV",7,1,365);
	JParamGeneral.AjouterParametreDouble("Tolérance Etui Blister (g)",1,0,50);
	JParamGeneral.AjouterParametreDouble("Tolérance Etui Sachet (g)",1,0,50);
	JParamGeneral.AjouterParametreDouble("Tolérance Etui Sachet 4ml(g)",1,0,50);

#ifdef XML
	pParamRepFichierEtiXml = JParamGeneral.AjouterDirectParametreString("Rep_Fichier_Etiq_Xml", theConfig.str_RepertoireBase);
	pParamNomFichierEtiXml = JParamGeneral.AjouterDirectParametreString("Nom_Fichier_Etiq_Xml", theConfig.str_Machine + _T(".xml"));
#endif

#ifdef ETICONCEPT
	//Données pour Eticoncept et Etiquettes
	JParamGeneral.AjouterParametreString("Rep_Base_BL","\\DEV-PORT-06");
	JParamGeneral.AjouterParametreString("Nom_Base_BL","Provider=SQLOLEDB;Data Source=DEV-PORT-06\\SQLEXP2008R2;Initial Catalog=acyrus_test;User Id=sa;Password=acyrus;Persist Security Info=true;");
	JParamGeneral.AjouterParametreString("Provider_Base_BL","SQL");
	JParamGeneral.AjouterParametreString("Nom_Table_BL","MeltingPot");
	JParamGeneral.AjouterParametreString("Nom_Champ_Machine_BL","ODF_StartedOnLine");
	JParamGeneral.AjouterParametreString("Nom_Machine_BL","LINE_ID1");
#endif
	//JParamGeneral.AjouterParametreLong("Forcage_Inhibition_CTRL_Etiquette",TRUE,FALSE,TRUE);
	//V 44.0 JParamGeneral.AjouterParametreLong("Lecture Boites",TRUE,FALSE,TRUE);		//V37
	JParamGeneral.AjouterParametreString("Format Etiquette WOLKE","WOLKE");		//V37
	JParamGeneral.AjouterParametreString("Titre Colonne ITEM_MET WOLKE","Sec Ref");	//V37
	JParamGeneral.AjouterParametreString("Texte ITEM_MET WOLKE","AA");		//V37

	//V16 : Forcage PreFAT
	JParamGeneral.AjouterParametreLong("Forcage pour PreFAT",0,0,1);

	JParamGeneral.AjouterTitre(_T("Active Directory"));
	JParamGeneral.AjouterParametreLong(_T("Mode Authentification Utilisateurs"), 0, 0, 10);

	JParamGeneral.AjouterParametreString(_T("LDAP Host"), _T("blemea.bausch.com"));//BLEMEA.BAUSCH.COM
	JParamGeneral.AjouterParametreString(_T("LDAP Domaine"), _T("dc=blemea,dc=bausch,dc=com"));//BLEMEA.BAUSCH.COM

	JParamGeneral.AjouterParametreString(_T("IP serveur (test mode degrade)"), _T("172.18.1.30"));
	JParamGeneral.AjouterParametreString(_T("Nom Machine sur Annuaire"), theConfig.str_Machine);
	JParamGeneral.AjouterParametreString(_T("Entete groupes ACYRUS"), _T("ABS_ACYRUS"));

	//V 44.7
	JParamGeneral.AjouterTitre(_T("Cadence"));
	pParamCadence_MinMachine = JParamGeneral.AjouterDirectParametreDouble(_T("Seuil Min Alarme"), 8, 0, 50);
	pParamCadence_MinRobot = JParamGeneral.AjouterDirectParametreDouble(_T("Seuil Min Robot Alarme"), 50, 0, 200);
	pParamCadence_Periode = JParamGeneral.AjouterDirectParametreDouble(_T("Delai calcul (min)"), 10, 0, 30);
	pParamCadence_Timeout = JParamGeneral.AjouterDirectParametreDouble(_T("Timeout Acquit (min)"), 10, 0, 60);

	JParamGeneral.AjouterTitre(_T("Affichage"));
	pParamModeAffichageResolution = JParamGeneral.AjouterDirectParametreLong(_T("Resolution Affichage"), 0, 0, 50);
	pParamModeAffichageResolution->SelectionChamp = _T("0/0-Classique;1/1-Grande (1920x1080);");
	pParamModeAffichageResolution->iSelection = SAISIE_LISTEPREDEF;
	pParamModeAffichageResolution->InitSelectionChamp();
	/*
	CHARGEMENT BASE DE DONNEES
	*/


	JParamGeneral.ChargerFromBase("Parametre");//Chargement des valeurs dans la BDD
	
	/*
	RECUPERATION DES POINTEURS PARAMS APPLI
	*/
	pParamAnglePriseMin = (CParametreLong * ) JParamGeneral.GetParam("Angle_Prise_Min");
	pParamAnglePriseMax = (CParametreLong * ) JParamGeneral.GetParam("Angle_Prise_Max");
	pParamLgPulseVision = (CParametreLong * ) JParamGeneral.GetParam("Lg_Pulse_Vision_ms");

	ParamCommandeTestCamera = (CParametreString * ) JParamGeneral.GetParam("Commande_Test_Camera");
	pParamDossierExport = (CParametreString * ) JParamGeneral.GetParam("Nom_Dossier_Export");
	pParamDirBackup = (CParametreString * ) JParamGeneral.GetParam("Nom_Dossier_Backup");
	pParamNomBase = (CParametreString * ) JParamGeneral.GetParam("Nom_BaseSQL");			//V2.58
	ParamStrNomServeurOpc = (CParametreString * ) JParamGeneral.GetParam("Nom_Serveur_OPC");
	pParamDetailFASTEC = (CParametreLong * ) JParamGeneral.GetParam("OPC : Détail défauts FASTEC");		//V35
	pParamDetailFASTEC->iAffichage = AFF_BOOL;

	//V16 : gestion impression fin d'OF
	pParamImpressionType = (CParametreLong * ) JParamGeneral.GetParam("Fin OF : Type impression");
	pParamImpressionType->SelectionChamp = _T("0/0-Pas d'impression;1/1-Papier;2/2-PDF;");
	pParamImpressionType->iSelection = SAISIE_LISTEPREDEF;
	pParamImpressionType->InitSelectionChamp();
	pParamImpressionImprimante = (CParametreString * ) JParamGeneral.GetParam("Fin OF : Imprimante par défaut");

	pParamNbCyclePulse = (CParametreLong * ) JParamGeneral.GetParam("Nb_Cycle_Pulse");
	pParamTempsLogout = (CParametreLong * ) JParamGeneral.GetParam("Temps_Logout_Auto");
//	pParamNbTentativesLogin = (CParametreLong * ) JParamGeneral.GetParam("NbTentatives_Login");
	ParamStrDossierSHOPORD = (CParametreString * ) JParamGeneral.GetParam("Nom_Dossier_SHOPORD");
	ParamStrNomFichierSHOPORD_ACYRUS = (CParametreString * ) JParamGeneral.GetParam("Nom_FichierCSV_ACYRUS");
	pParamNbJoursFichiersBDD = (CParametreLong * ) JParamGeneral.GetParam("Nb_Jours_Fichiers_BDD");
	pParamNbJoursPurgeDossierCSV = (CParametreLong * ) JParamGeneral.GetParam("Nb_Jours_Fichiers_CSV");
	ParamStrFormat_CSVFile = (CParametreString * ) JParamGeneral.GetParam("Str_Format_Remplacement_CSV");

	ParamStrSACHET_Fichier = (CParametreString * ) JParamGeneral.GetParam("SACHETS (liste) : fichier .csv");	//V36

	pParamPariteCOM = (CParametreString * ) JParamGeneral.GetParam("Parite_COM");
	pParamVitesseCOM = (CParametreLong * ) JParamGeneral.GetParam("Vitesse_COM");
	pParamBitsDataCOM = (CParametreLong * ) JParamGeneral.GetParam("Nb_BitsData_COM");
	pParamBitsStopCOM = (CParametreLong * ) JParamGeneral.GetParam("Nb_BitsStop_COM");

	pParamPoidsEtalon1= (CParametreDouble * ) JParamGeneral.GetParam("Poids_Etalon_1");
	pParamPoidsEtalon2 = (CParametreDouble * ) JParamGeneral.GetParam("Poids_Etalon_2");
	pParamPoidsMinEtalon1 = (CParametreDouble * )JParamGeneral.GetParam("Poids_Min_Etalon_1");
	pParamPoidsMinEtalon2 = (CParametreDouble * ) JParamGeneral.GetParam("Poids_Min_Etalon_2");
	pParamPoidsMaxEtalon1 = (CParametreDouble * ) JParamGeneral.GetParam("Poids_Max_Etalon_1");
	pParamPoidsMaxEtalon2 = (CParametreDouble * ) JParamGeneral.GetParam("Poids_Max_Etalon_2");

	pParamSachetNom  = (CParametreDouble * ) JParamGeneral.GetParam("Sachet_Nom");
	pParamSachetMin  = (CParametreDouble * ) JParamGeneral.GetParam("Sachet_Min");
	pParamSachetMax  = (CParametreDouble * ) JParamGeneral.GetParam("Sachet_Max");
	pParamBlisterNom = (CParametreDouble * ) JParamGeneral.GetParam("Blister_Nom");
	pParamBlisterMin = (CParametreDouble * ) JParamGeneral.GetParam("Blister_Min");
	pParamBlisterMax = (CParametreDouble * ) JParamGeneral.GetParam("Blister_Max");
	pParamSachet4mlNom = (CParametreDouble * ) JParamGeneral.GetParam("Sachet4ml_Nom");
	pParamSachet4mlMin = (CParametreDouble * ) JParamGeneral.GetParam("Sachet4ml_Min");
	pParamSachet4mlMax = (CParametreDouble * ) JParamGeneral.GetParam("Sachet4ml_Max");

	ParamStrIPDMX1 = (CParametreString * ) JParamGeneral.GetParam("IP_LecteurDMX_Etui");
	ParamStrIPDMX2 = (CParametreString * ) JParamGeneral.GetParam("IP_LecteurDMX_Notice1");
	ParamStrIPDMX3 = (CParametreString * ) JParamGeneral.GetParam("IP_LecteurDMX_Notice2");
	pParamPortEcouteLecteurDMX = (CParametreLong *)JParamGeneral.GetParam("Port Ecoute Socket");

	pParamOffsetPosX = (CParametreLong * ) JParamGeneral.GetParam("offset_Pos_X");
	pParamOffsetPosY = (CParametreLong * ) JParamGeneral.GetParam("offset_Pos_Y");
	pParamOffsetPosZ = (CParametreLong * ) JParamGeneral.GetParam("offset_Pos_Z");

	pParamToleranceEtuiBlister = (CParametreDouble * ) JParamGeneral.GetParam("Tolérance Etui Blister (g)");
	pParamToleranceEtuiSachet = (CParametreDouble * ) JParamGeneral.GetParam("Tolérance Etui Sachet (g)");
	pParamToleranceEtuiSachet4ml = (CParametreDouble *)JParamGeneral.GetParam("Tolérance Etui Sachet 4ml(g)");

#ifdef ETICONCEPT
	pParamRepBaseEticoncept = (CParametreString * )  JParamGeneral.GetParam("Rep_Base_BL");
	pParamNomBaseEticoncept = (CParametreString * )  JParamGeneral.GetParam("Nom_Base_BL");
	pParamProviderBaseEticoncept = (CParametreString * )  JParamGeneral.GetParam("Provider_Base_BL");
	pParamNomTableEticoncept = (CParametreString * )  JParamGeneral.GetParam("Nom_Table_BL");
	pParamNomChampMachine = (CParametreString * )  JParamGeneral.GetParam("Nom_Champ_Machine_BL");
	pParamNomMachineBL = (CParametreString * )  JParamGeneral.GetParam("Nom_Machine_BL");
#endif
	//pParamForcageInhibitionEtiquette = (CParametreLong * ) JParamGeneral.GetParam("Forcage_Inhibition_CTRL_Etiquette"); //V37
	//pParamForcageInhibitionEtiquette->iAffichage = AFF_BOOL;//V37

	//V37

	//V 44.0 //pParamLectureBoite = (CParametreLong * ) JParamGeneral.GetParam("Lecture Boites");				//V37
	//pParamLectureBoite->iAffichage = AFF_BOOL;														//V37

	pParamFormatWolke = (CParametreString * )  JParamGeneral.GetParam("Format Etiquette WOLKE");	//V37
	pParamTitreColonneWolke = (CParametreString * ) JParamGeneral.GetParam("Titre Colonne ITEM_MET WOLKE");		//V37
	pParamTexteWolke = (CParametreString * )  JParamGeneral.GetParam("Texte ITEM_MET WOLKE");		//V37

	pParamForcagePreFAT = (CParametreLong * ) JParamGeneral.GetParam("Forcage pour PreFAT");


	//V 40 ActiveDir
	pParamAD_ModeLoginUser = (CParametreLong *)JParamGeneral.GetParam(_T("Mode Authentification Utilisateurs"));
	pParamAD_ModeLoginUser->SelectionChamp = _T("0/0-Base de données Locale;1/1-Active Directory;");
	pParamAD_ModeLoginUser->iSelection = SAISIE_LISTEPREDEF;
	pParamAD_ModeLoginUser->InitSelectionChamp();

	pParamAD_LDAPHost = (CParametreString *)JParamGeneral.GetParam(_T("LDAP Host"));
	pParamAD_LDAPDomaine = (CParametreString *)JParamGeneral.GetParam(_T("LDAP Domaine"));
	pParamAD_IPServeur = (CParametreString *)JParamGeneral.GetParam(_T("IP serveur (test mode degrade)"));
	pParamAD_MachineName = (CParametreString *)JParamGeneral.GetParam(_T("Nom Machine sur Annuaire"));
	pParamAD_EnteteGroupesAcyrus = (CParametreString *)JParamGeneral.GetParam(_T("Entete groupes ACYRUS"));
}

void CBaseFenetreApp::InitParamuEyes(void)
{
	for(int iCam=0; iCam<2;iCam++)
	{
		//Initialisation du jeu de parametres de la camera
		JParamuEye[iCam].strTitre.Format( _T("Paramètres Camera %d"),uEye[iCam].m_iNumero);
		JParamuEye[iCam].TypeTraca = JOURNAL_TYPE_JPARAM_GENERAL;

		JParamuEye[iCam].AjouterParametreLong("Image rotation (Degré)",0,-360,360);
		JParamuEye[iCam].AjouterParametreDouble("Temps Exposition (ms)",20,0,MAXLONG);
		JParamuEye[iCam].AjouterParametreLong("Source Trigger",0,0,4200);
		JParamuEye[iCam].AjouterParametreLong("Delai Trigger (µs)",200000,0,MAXLONG);

		CString strTable = _T("");
		strTable.Format(_T("ParamCam%d"),uEye[iCam].m_iNumero);
		JParamuEye[iCam].ChargerFromBase(strTable);
	
		UEye_ImgRotation[iCam] = (CParametreLong * ) JParamuEye[iCam].GetParam("Image rotation (Degré)");
		UEye_Temps_Expo[iCam] = (CParametreDouble * ) JParamuEye[iCam].GetParam("Temps Exposition (ms)");
		UEye_Source_Trigger[iCam] = (CParametreLong * ) JParamuEye[iCam].GetParam("Source Trigger");
		UEye_Source_Trigger[iCam]->SelectionChamp = _T("4098/LowHigh;4097/HighLow;4104/BitSnap;");
		UEye_Source_Trigger[iCam]->iSelection = SAISIE_LISTEPREDEF;
		UEye_Source_Trigger[iCam]->InitSelectionChamp();
		UEye_Delai_Trigger[iCam] = (CParametreLong * ) JParamuEye[iCam].GetParam("Delai Trigger (µs)");
	}
}


void CBaseFenetreApp::InitFenetres()
{
	//Initialisation des fenetres d'affichages
	
	MachineView = new CDialMachine(AfxGetMainWnd());
	MachineView->Create(CDialMachine::IDD,AfxGetMainWnd());
	MachineView->UpdateWindow();
	MachineView->ShowWindow(SW_HIDE);//Fenetre par défaut
	
	TabDial[iNbDialog] = MachineView;
	iNbDialog ++;

	CamView = new CForm_DalsaSpyder(AfxGetMainWnd());
	CamView->Create(CForm_DalsaSpyder::IDD,AfxGetMainWnd());
	CamView->UpdateWindow();
	CamView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = CamView;
	iNbDialog ++;

	DalsaCamera1.frame = CamView;
	DalsaCamera2.frame = CamView;
	
	ParamView = new CDialParam(AfxGetMainWnd(),&JParamGeneral);
	ParamView->Create(CDialParam::IDD,AfxGetMainWnd());
	ParamView->UpdateWindow();
	ParamView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = ParamView;
	iNbDialog ++;

	VisionView = new CDialVision(AfxGetMainWnd());
	VisionView->Create(CDialVision::IDD,AfxGetMainWnd());
	VisionView->UpdateWindow();
	VisionView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = VisionView;
	iNbDialog ++;

	InitialisationView = new CDialInitialisation(AfxGetMainWnd());
	InitialisationView->Create(CDialInitialisation::IDD,AfxGetMainWnd());
	InitialisationView->UpdateWindow();
	InitialisationView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = InitialisationView;
	iNbDialog ++;

	DialMixup= new CDialMixup(AfxGetMainWnd());
	DialMixup->Create(CDialMixup::IDD,AfxGetMainWnd());
	DialMixup->UpdateWindow();
	DialMixup->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = DialMixup;
	iNbDialog ++;

	ValidationView = new CDialValidation(AfxGetMainWnd());
	ValidationView->Create(CDialValidation::IDD,AfxGetMainWnd());
	ValidationView->UpdateWindow();
	ValidationView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = ValidationView;
	iNbDialog ++;

	ValidLotView= new CDialValidLot(AfxGetMainWnd());
	ValidLotView->Create(CDialValidLot::IDD,AfxGetMainWnd());
	ValidLotView->UpdateWindow();
	ValidLotView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = ValidLotView;
	iNbDialog ++;

	RobotsView = new CDialRobot(AfxGetMainWnd());
	RobotsView->Create(CDialRobot::IDD,AfxGetMainWnd());
	RobotsView->UpdateWindow();
	RobotsView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = RobotsView;
	iNbDialog ++;

//	PesonsView = new CDialPesons(AfxGetMainWnd(),&theApp.Pesons[0]);
	PesonsView = new CDialPesons(AfxGetMainWnd(), &theApp.Pesons[0]);
	PesonsView->Create(CDialPesons::IDD,AfxGetMainWnd());
	PesonsView->UpdateWindow();
	PesonsView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = PesonsView;
	iNbDialog ++;

	AlarmesView = new CDialAlarmes(AfxGetMainWnd());
	AlarmesView->Create(CDialAlarmes::IDD,AfxGetMainWnd());
	AlarmesView->UpdateWindow();
	AlarmesView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = AlarmesView;
	iNbDialog ++;

	ChargementView = new CDialChargement(AfxGetMainWnd());
	ChargementView->Create(CDialChargement::IDD,AfxGetMainWnd());
	ChargementView->UpdateWindow();
	ChargementView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = ChargementView;
	iNbDialog ++;

	OPCView	 = new CDialGrpOPC(AfxGetMainWnd(),m_Recette.m_OPCClient);//Groupe Robot par défaut
	OPCView->Create(CDialGrpOPC::IDD,AfxGetMainWnd());
	OPCView->UpdateWindow();
	OPCView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = OPCView;
	iNbDialog ++;

	DMXView  = new CDialDMX(AfxGetMainWnd());
	DMXView->Create(CDialDMX::IDD,AfxGetMainWnd());
	DMXView->UpdateWindow();
	DMXView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = DMXView;
	iNbDialog ++;

	UeyeView  = new CDialCameras(AfxGetMainWnd());
	UeyeView->Create(CDialCameras::IDD,AfxGetMainWnd());
	UeyeView->UpdateWindow();
	UeyeView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = UeyeView;
	iNbDialog ++;

	PeseeView  = new CDialPesee(AfxGetMainWnd());
	PeseeView->Create(CDialPesee::IDD,AfxGetMainWnd());
	PeseeView->UpdateWindow();
	PeseeView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = PeseeView;
	iNbDialog ++;
	
	EtiqView  = new CDialEtiquette(AfxGetMainWnd());
	EtiqView->Create(CDialEtiquette::IDD,AfxGetMainWnd());
	EtiqView->UpdateWindow();
	EtiqView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = EtiqView;
	iNbDialog ++;
	
	DefautPisteView  = new CDialDefautPiste(AfxGetMainWnd());
	DefautPisteView->Create(CDialDefautPiste::IDD,AfxGetMainWnd());
	DefautPisteView->UpdateWindow();
	DefautPisteView->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = DefautPisteView;
	iNbDialog ++;

	//V 45.0
	DialFormulaire = new CDialFormulaire(AfxGetMainWnd());
	DialFormulaire->Create(CDialFormulaire::IDD, AfxGetMainWnd());
	DialFormulaire->UpdateWindow();
	DialFormulaire->ShowWindow(SW_HIDE);
	TabDial[iNbDialog] = DialFormulaire;
	iNbDialog++;

	//initialisation des menus
	((CFormMenuMachine*)pFormMenu)->MenuMachine.strProfil = theApp.CurrentUser.strProfil;
	((CFormMenuMachine*)pFormMenu)->MenuMachine.SendMessage(WM_CREATE,NULL,NULL);

	((CFormStatus*)pFormStatus)->MenuStatus.strProfil = theApp.Process.sEtat[theApp.Process.iEtat];
	((CFormStatus*)pFormStatus)->MenuStatus.SendMessage(WM_CREATE,0,0);
}

// gestionnaires de messages pour CBaseFenetreApp


int CBaseFenetreApp::ExitInstance()
{
	//Enregistrement logout quand on quitte l'application
	CurrentUser.Deconnecter();

	//Arret thread OPC
	StopThreadOPC();

	if (DalsaCamera1.m_hCom != NULL)
	{
		CloseHandle(DalsaCamera1.m_hCom);
		DalsaCamera1.m_hCom = NULL;
	}
	
	if (DalsaCamera2.m_hCom != NULL)
	{
		CloseHandle(DalsaCamera2.m_hCom);
		DalsaCamera2.m_hCom = NULL;
	}

	//Arret groupes defauts
	for(int i = 0; i < 3; i++)
	{
		m_GroupeDefauts[i].EndGroupe();
	}

	theCam.m_iModeInspection = CAM_MODE_ARRET;
	theCam.m_bIsOK = TRUE;
	theCam.iSnap = 1;

	theCam.Stop();
	theCam.Decharger();

	//CAM2
	theCamUV.m_iModeInspection = CAM_MODE_ARRET;
	theCamUV.m_bIsOK = TRUE;
	theCamUV.iSnap = 1;

	theCamUV.Stop();
	theCamUV.Decharger();

	((CDialEtiquette*)theApp.EtiqView)->ImageVisu.DetachGraphicListe();//V 44.8
	((CDialEtiquette*)theApp.EtiqView)->ImageVisu.Decharger();

	//V 44.8
	EtiqScellement.Vision_Decharger();
	EtiqWolke.Vision_Decharger();
	VisionBoite.Vision_Decharger();

	uEye[ID_CAM_UEYE1].m_iModeInspection = CAM_MODE_ARRET;
	uEye[ID_CAM_UEYE1].m_bIsOK = TRUE;
	uEye[ID_CAM_UEYE1].iSnap = 1;

	uEye[ID_CAM_UEYE2].m_iModeInspection = CAM_MODE_ARRET;
	uEye[ID_CAM_UEYE2].m_bIsOK = TRUE;
	uEye[ID_CAM_UEYE2].iSnap = 1;

	uEye[ID_CAM_UEYE2].Stop();
	uEye[ID_CAM_UEYE1].Stop();
	
	uEye[ID_CAM_UEYE2].Decharger();
	uEye[ID_CAM_UEYE1].Decharger();

	Systeme[0].Decharger();
	Systeme[1].Decharger();
	Systeme[2].Decharger();//UNUSED

	//theCam.imgWnd.Decharger();
	Systeme[3].Decharger();

	//MappControl(M_ERROR,M_PRINT_DISABLE);
	Application.Decharger();
	
	//GCA V1.18 
	//Ajout evenement journal (type INFO) Arret PC
	theJournal.Evenement(JOURNAL_TYPEINFO,_T("Arret App"),_T(""),_T(""));

	//fermeture Base
	theBase.Fermer();

	//Sauvegarde des parametres
	JParamGeneral.SauvegarderParametres();

	// Fermeture/destruction des fenetres
	for(int i = 0; i < iNbDialog;i++)
	{
		delete(TabDial[i]);
		TabDial[i] = NULL;
	}
	//#3 DMX
	m_DMXEtui.ClientSocket.pExecuteOnDeconnect = NULL;
	m_DMXEtui.ClientSocket.pExecuteOnValueReceive = NULL;
	m_DMXNotice1.ClientSocket.pExecuteOnDeconnect = NULL;
	m_DMXNotice1.ClientSocket.pExecuteOnValueReceive = NULL;
	m_DMXNotice2.ClientSocket.pExecuteOnDeconnect = NULL;
	m_DMXNotice2.ClientSocket.pExecuteOnValueReceive = NULL;
	Process.ServeurSocket.pExecuteOnConnect = NULL;

	//SockV3
	m_DMXNotice2.ClientSocket.pExecuteOnArretThread = NULL;
	m_DMXEtui.ClientSocket.pExecuteOnArretThread = NULL;
	m_DMXNotice1.ClientSocket.pExecuteOnArretThread = NULL;

	m_DMXEtui.ListeAffichage = NULL;
	m_DMXNotice1.ListeAffichage = NULL;
	m_DMXNotice2.ListeAffichage = NULL;

	m_DMXEtui.ClientSocket.Deconnecter();
	m_DMXNotice1.ClientSocket.Deconnecter();
	m_DMXNotice2.ClientSocket.Deconnecter();
	Process.ServeurSocket.Deconnecter();
	
	CoUninitialize();

	// Relache le mutex pour execution unique
	if(m_bLock) m_Mutex.Unlock();//V2.44

	return CWinApp::ExitInstance();
}

int CBaseFenetreApp::GererAlarmes(void)
{
	m_GestAlarmes.GererAlarmes();
	GererVoyants();
	return 0;
}

int CBaseFenetreApp::SetButAlarme(bool bCligno)
{
	//Bouton alarme de la fenetre statut
	if(IsWindow(pFormStatus->GetSafeHwnd()))
	{
		((CFormStatus * )pFormStatus)->butAlarme.bAlarme = bCligno;

		if(bCligno)((CFormStatus * )pFormStatus)->butAlarme.Invalidate();
	}
	return 0;
}
void CBaseFenetreApp::ChargerUtilisateur(CString strNom)
{
	CurrentUser.Deconnecter();//MAJ journal Evenement LogOut

	CurrentUser.strCode = strNom;
	CurrentUser.Lire_Base(CurrentUser.strCode);

	if(strNom != CODE_UTILISATEUR_DEFAUT ) CurrentUser.IdBase = theApp.theJournal.Ajouter(JOURNAL_TYPELOGIN,CurrentUser.strLibelle,"Login");
	
	MAJMenu();

	//MAJ Valeur Process/MAchine
	if(CurrentUser.bMaintenance)
	{
		*Process.pBitMaintenance = TRUE;
	}
	else
	{
		*Process.pBitMaintenance = FALSE;
	}
}
void CBaseFenetreApp::ChargerUtilisateurDefaut()
{
	CurrentUser.Deconnecter();//MAJ journal Evenement LogOut

	CurrentUser.strCode = CODE_UTILISATEUR_DEFAUT;
	CurrentUser.strProfil = PROFIL_UTILISATEUR_DEFAUT;
	CurrentUser.strLibelle = CurrentUser.strCode;
	//CurrentUser.Lire_Base(CurrentUser.strCode);
	MAJMenu();

	*Process.pBitMaintenance = FALSE;
}

bool CBaseFenetreApp::AuthenticateADUser(CString strUser, CString strMotDePasse, CString& strLibelleUser, CStringArray& strListeGroupes, CStringArray& strListeProfils)
{
	/*
	RECHERCHE DU NOM COMPLET UTILISATEUR A PARTIR DU LOGIN SAISI
	*/
	CString str_UserNew = _T("");
	CString strFilter = _T("(&(objectClass=user)(objectCategory=person)(sAMAccountName=%s))");
	CString strChampRetour = _T("distinguishedName");

	//valeurs serveur ACYRUS
	if (pParamAD_LDAPHost->value == _T("192.168.0.39"))
	{
		strFilter = _T("(&(objectClass=posixAccount)(objectClass=person)(cn=%s))");
		strChampRetour = _T("cn");
	}

	str_UserNew = _T("");
	if (ACYActiveDir_GetUserDistinguishedName_LDAP(
		pParamAD_LDAPHost->value
		, pParamAD_LDAPDomaine->value
		, strUser
		, str_UserNew
		, strFilter
		, strChampRetour)
		== false)
	{

		if (ACYActiveDir_GetNumLastError() == ACYAD_CONNEXION_FAILED)
		{
			//Connexion au serveur echouée, on fait les test pour savoir si on passe en mode dégradé
			CString strIpTemp = _T("");
			strIpTemp = pParamAD_IPServeur->value;
			if (!Ping(strIpTemp))
			{

				if (AfxMessageBox(_T("Annuaire indisponible (pas de réseau B&L)\nUtiliser le mode dégradé local ?"), MB_YESNO, MB_ICONQUESTION) == IDYES)
				{
					bDoModeDegrade = true;
					return false;
				}
			}
		}
		//Utilisateur inconnu
		return false;
	}

	/*
	VERIFICATION DES INFORMATIONS DE CONNEXION
	*/

	//valeurs serveur ACYRUS
	if (pParamAD_LDAPHost->value == _T("192.168.0.39"))
	{
		CString strTemp = str_UserNew;
		str_UserNew.Format(_T("uid=%s,cn=users,%s"), strTemp, pParamAD_LDAPDomaine->value);
	}


	if (ACYActiveDir_LoginLDAP(pParamAD_LDAPDomaine->value, str_UserNew, strMotDePasse, pParamAD_LDAPHost->value) == false)
	{
		//Utilisateur inconnu / Informations d'identification incorrectes
		return false;
	}

	//Pas besoin de le garder en mémoire... strPassword = strMotDePasse;
	CString strTemp = str_UserNew;

	//Format CN=Jouve\, Jessie,OU=Users,OU=ABS,OU=FRA,DC=blemea,DC=bausch,DC=com
	//recherche de la deuxieme virgule dans la chaine
	int iIndex = strTemp.Find(_T("="));

	if (iIndex < 0)
	{
		strLibelleUser = str_UserNew;
	}
	int iIndex2 = strTemp.Find(_T(","), iIndex);

	if (iIndex2 < 0)
	{
		strLibelleUser = str_UserNew;
	}

	iIndex2 = strTemp.Find(_T(","), (iIndex2 + 1));

	if (iIndex2 < 0)
	{
		strLibelleUser = str_UserNew;
	}

	strLibelleUser = strTemp.Mid((iIndex + 1), ((iIndex2 - iIndex) - 1));
	//strLibelleUser.Replace(_T("\\,\r\n"), _T(" "));
	//V 43-ALL
	strLibelleUser.Remove(_T('\\'));
	strLibelleUser.Remove(_T(','));
	strLibelleUser.Remove(_T('\r'));
	strLibelleUser.Remove(_T('\n'));

	/*
	VERIFICATION DES GROUPES/MACHINES/PROFILS
	*/
	strListeGroupes.RemoveAll();
	strListeProfils.RemoveAll();

	ACYActiveDir_GetUserGroups(strListeGroupes);

	if (strListeGroupes.GetCount() == 0)
	{
		AfxMessageBox(_T("L'utilisateur n'appartient à aucun groupes !"));
		return false;
	}
	else
	{

		//CString strMessage = _T("");
		//strMessage.Format(_T("%d groupes detectés, login OK"),(int)strListeGroupes.GetCount());
		//AfxMessageBox(strMessage);//Debug LDAP


		CString strEnteteGrps = pParamAD_EnteteGroupesAcyrus->value;
		CString strMachineEnCours = pParamAD_MachineName->value;

		strEnteteGrps.MakeUpper();
		strMachineEnCours.MakeUpper();

		CString strGroupe = _T("");
		for (INT_PTR j = 0; j < strListeGroupes.GetCount(); j++)
		{
			strGroupe = strListeGroupes.GetAt(j);
			strGroupe.MakeUpper();

			CString strMachineTemp = _T("");
			CString strProfilTemp = _T("");

			int iIndexStart = 0;
			int iIndexMachine = 0;
			int iIndexProfil = 0;

			//on ne prend en charge que les groupes commencant par ABS_ACYRUS_
			if (strGroupe.Left(strEnteteGrps.GetLength()) != strEnteteGrps)
			{
				continue;
			}
			iIndexStart = strEnteteGrps.GetLength();
			if (iIndexStart != 0)
			{
				iIndexMachine = strGroupe.Find(_T("_"), iIndexStart);
				if (iIndexMachine < iIndexStart)
				{
					//Pblm de formatage du profil
					continue;
				}
			}
			else
			{
				//Pas d'entête defini poru les groupes
				iIndexMachine = -1;
			}

			iIndexProfil = strGroupe.Find(_T("_"), (iIndexMachine + 1));

			if (iIndexProfil <= iIndexMachine)
			{
				//Pblm de formatage du profil
				continue;
			}

			strMachineTemp = strGroupe.Mid((iIndexMachine + 1), (iIndexProfil - iIndexMachine - 1));
			strProfilTemp = strGroupe.Mid((iIndexProfil + 1), (strGroupe.GetLength() - iIndexProfil - 1));

			strMachineTemp.MakeUpper();
			strProfilTemp.MakeUpper();

			if (strMachineEnCours == strMachineTemp)
			{
				//	AfxMessageBox(strProfilTemp);//Debug LDAP

				strListeProfils.SetAtGrow(strListeProfils.GetCount(), strProfilTemp);
			}
		}

		if (strListeProfils.GetCount() == 0)
		{
			//Erreur de lecture des machines/profils dans les groupes reçus
			AfxMessageBox(_T("LogonUser ECHEC: Le système n'a detecté aucun groupes pour la machine/profil en cours"));
			return false;
		}

		//utilisateur OK
		//mot de passe ok
		//au moins un profil declaré sur cette machine
		return true;
	}

	return false;
}

bool CBaseFenetreApp::Login_IsDevelopperUser(CString strUtilisateur,CString strMotDePasse)
{
	CString strUserDev = strUtilisateur.MakeUpper();
	if (strUserDev == _T("ACYDEV"))
	{
		if (strMotDePasse == ACYDEV_GetPassword())
		{
			/*
			CHARGEMENT DES INFORMATIONS APPLICATION EN FONCTION DU PROFIL
			*/
			CurrentUser.IdBase = theApp.theJournal.Ajouter(JOURNAL_TYPELOGIN, CurrentUser.strLibelle, _T("Login"));

			CurrentUser.strCode = _T("ACYRUS");
			CurrentUser.strProfil = _T("MANAGER");
			CurrentUser.strLibelle = _T("ACYRUS");
			CurrentUser.bIsActif = true;
			CurrentUser.NiveauOperateur = UTILISATEUR_NIVEAU_MANAGER;
			CurrentUser.bMaintenance = false;

			//MAJ Valeur Process/MAchine
			*Process.pBitMaintenance = FALSE;

			MAJMenu();
			return true;
		}
	}

	return false;
}

bool CBaseFenetreApp::Login_DoSaisieIdentifiants(CString& strLogin, CString& strMdp)
{
	CDiagPadAlpha frame, framePass;

	frame.bPassword = false;
	frame.strSaisie = _T("");
	if (bDoModeDegrade == true)
	{
		frame.bMajusculeForced = true;
		frame.strTitre = _T("Login Utilisateur LOCAL (Ancien)");
		frame.strInfos = _T("Saisir votre Login Utilisateur LOCAL (Ancien)");
	}
	else
	{
		frame.bMajusculeForced = false;
		frame.strTitre = _T("Login Utilisateur");
		frame.strInfos = theApp.TableMessage.GetStr(MESSAGE_USER_INFOS_LOGIN);
	}

	frame.strSelectionTable = _T("");
	frame.strSelectionChamp = _T("");

	if (frame.DoModal() != IDOK)return false;
	if (frame.strSaisie == _T(""))return false;

	strLogin = frame.strSaisie;

	//demande du mot de passe //V 40 deplacer ici, avant toute autre vérification
	framePass.bPassword = true;
	framePass.strSaisie = _T("");

	if (bDoModeDegrade == true)
	{
		framePass.bMajusculeForced = true;
		framePass.strTitre = _T("Mot de Passe Utilisateur LOCAL (Ancien)");
		framePass.strInfos = _T("Saisir votre mot de passe LOCAL (Ancien)");
	}
	else
	{
		framePass.bMajusculeForced = false;
		framePass.strTitre = _T("Mot de Passe Utilisateur");
		framePass.strInfos = _T("Saisir votre mot de passe");
	}

	framePass.strSelectionTable = _T("");
	framePass.strSelectionChamp = _T("");

	if (framePass.DoModal() != IDOK) return false;
	if (framePass.strSaisie == _T("")) return false;

	strMdp = framePass.strSaisie;

	return true;
}

void CBaseFenetreApp::OnLogin()
{
	// Login
	CString strUtilisateur,strMotDePasse,strTemp;
	CUtilisateur UserEnCours;

	ChargerUtilisateurDefaut(); //V 40 ChargerUtilisateur(CODE_UTILISATEUR_DEFAUT);

	if (!Login_DoSaisieIdentifiants(strUtilisateur, strMotDePasse))
	{
		//Echap ou saisie Nulle sur une des deux fenetres (Login/mdp)
		return;
	}
	

	//V 43.2 Login user Acyrus, pour évité de chercher quelqu'un avec les droits qu'on veut...
	if (Login_IsDevelopperUser(strUtilisateur, strMotDePasse) == true)
	{
		return;
	}
		

	if (!Login_DoVerification(strUtilisateur, strMotDePasse,&UserEnCours))
	{
		//Echap ou saisie Nulle sur une des deux fenetres (Login/mdp)
		return;
	}

	/*
	CHARGEMENT DES INFORMATIONS APPLICATION EN FONCTION DU PROFIL
	*/	
	if ((pParamAD_ModeLoginUser->value == MODE_BDD)||(bDoModeDegrade == true))
	{
		ChargerUtilisateur(UserEnCours.strCode);
	}
	else
	{
		//mode ActiveDirectory
		CurrentUser.strCode = UserEnCours.strCode;//V 43-ALL
		CurrentUser.strProfil = UserEnCours.strProfil;
		CurrentUser.strLibelle = UserEnCours.strLibelle;
		CurrentUser.bIsActif = UserEnCours.bIsActif;
		CurrentUser.NiveauOperateur = UserEnCours.NiveauOperateur;
		CurrentUser.bMaintenance = UserEnCours.bMaintenance;

		CurrentUser.IdBase = theApp.theJournal.Ajouter(JOURNAL_TYPELOGIN, CurrentUser.strLibelle, _T("Login"));

		//MAJ Valeur Process/Machine
		if (CurrentUser.bMaintenance)
		{
			*Process.pBitMaintenance = TRUE;
		}
		else
		{
			*Process.pBitMaintenance = FALSE;
		}
		MAJMenu();
	}

	//declenchement du timer de logout Auto
	if (TimerLogout != NULL) KillTimer(MachineView->GetSafeHwnd(), TimerLogout);
	TimerLogout = SetTimer(MachineView->GetSafeHwnd(), ID_TIMER_LOGOUT, (pParamTempsLogout->value * 60000), NULL);

}

int CBaseFenetreApp::SetInfosString(CString strInfos)
{

	strInfos += _T("\r\n");

	((CFormStatus*)pFormStatus)->str_status.Empty();

	((CFormStatus*)pFormStatus)->str_status  =  (strInfos + ((CFormStatus*)pFormStatus)->str_status);
	
	((CFormStatus*)pFormStatus)->UpdateData(FALSE);

	return 0;
}

int CBaseFenetreApp::MAJMenu(void)
{
	//MAJ texte MenuMachine

	((CFormMenuMachine*)pFormMenu)->staticUtilisateur.strText = CurrentUser.strLibelle;
	((CFormMenuMachine*)pFormMenu)->staticUtilisateur.Invalidate();

	((CFormMenuMachine*)pFormMenu)->MenuMachine.strProfil = CurrentUser.strProfil;
	((CFormMenuMachine*)pFormMenu)->MenuMachine.MajMenu(NULL,NULL);

	//Menu Peson
	((CDialPesons * )PesonsView)->MenuDiagPeson.strProfil = CurrentUser.strProfil;
	((CDialPesons * )PesonsView)->MenuDiagPeson.MajMenu(NULL,NULL);

	//Menu Robot
	((CDialRobot * )RobotsView)->Menu_robot.strProfil = CurrentUser.strProfil;
	((CDialRobot * )RobotsView)->Menu_robot.MajMenu(NULL,NULL);

	//Menu Dialog MAchine
	((CDialMachine * )MachineView)->MenuDialMachine.strProfil = CurrentUser.strProfil;
	((CDialMachine * )MachineView)->MenuDialMachine.MajMenu(NULL,NULL);
	((CDialMachine * )MachineView)->TextDescription.Invalidate();
	((CDialMachine * )MachineView)->TextOf.Invalidate();
	((CDialMachine * )MachineView)->TextArticle.Invalidate();
	((CDialMachine * )MachineView)->TextLot.Invalidate();
	((CDialMachine * )MachineView)->static_description.Invalidate();
	((CDialMachine * )MachineView)->static_of.Invalidate();
	((CDialMachine * )MachineView)->static_article.Invalidate();
	((CDialMachine * )MachineView)->static_lot.Invalidate();
	((CDialMachine * )MachineView)->static_cadence.Invalidate();

	//Menu Chargement
	((CDialChargement * )ChargementView)->Menu_Chargement.strProfil = CurrentUser.strProfil;
	((CDialChargement * )ChargementView)->Menu_Chargement.MajMenu(NULL,NULL);

	//Menu Vision
	((CDialVision * )VisionView)->Menu_Vision.strProfil = CurrentUser.strProfil;
	((CDialVision * )VisionView)->Menu_Vision.MajMenu(NULL,NULL);

	//Menu Lot
	((CDialValidLot * )ValidLotView)->Menu_ValidLotUnlock.strProfil = CurrentUser.strProfil;
	((CDialValidLot * )ValidLotView)->Menu_ValidLotUnlock.MajMenu(NULL,NULL);

	((CDialValidLot * )ValidLotView)->Menu_ValidLot.strProfil = CurrentUser.strProfil;
	((CDialValidLot * )ValidLotView)->Menu_ValidLot.MajMenu(NULL,NULL);

	((CDialInitialisation* )InitialisationView)->MenuInit.strProfil = CurrentUser.strProfil;
	((CDialInitialisation* )InitialisationView)->MenuInit.MajMenu(NULL,NULL);

	((CDialValidLot * )ValidLotView)->static_Lot.Invalidate();
	((CDialInitialisation* )InitialisationView)->static_init.Invalidate();

	//Menu DMX
	((CDialDMX * )DMXView)->Menu_DMX.strProfil = CurrentUser.strProfil;
	((CDialDMX * )DMXView)->Menu_DMX.MajMenu(NULL,NULL);

	//Menu Mixup
	((CDialMixup * )DialMixup)->Menu_Mixup.strProfil = CurrentUser.strProfil;
	((CDialMixup * )DialMixup)->Menu_Mixup.MajMenu(NULL,NULL);

	//Menu Alarmes
	((CDialAlarmes * )AlarmesView)->Menu_Alarmes.strProfil = CurrentUser.strProfil;
	((CDialAlarmes * )AlarmesView)->Menu_Alarmes.MajMenu(NULL,NULL);

	((CDialCameras*)UeyeView)->Menu_Camera.strProfil = CurrentUser.strProfil;
	((CDialCameras* )UeyeView)->Menu_Camera.MajMenu(NULL,NULL);

	((CDialPesee*)PeseeView)->Menu_PesonEtui.strProfil = CurrentUser.strProfil;
	((CDialPesee* )PeseeView)->Menu_PesonEtui.MajMenu(NULL,NULL);

	((CDialParam*)ParamView)->Menu_Params.strProfil = CurrentUser.strProfil;
	((CDialParam* )ParamView)->Menu_Params.MajMenu(NULL,NULL);

	//V29
	((CDialDefautPiste*)DefautPisteView)->Menu_Defaut.strProfil = CurrentUser.strProfil;
	((CDialDefautPiste* )DefautPisteView)->Menu_Defaut.MajMenu(NULL,NULL);

	return 0;
}

int CBaseFenetreApp::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{

	//On remet la tempo du timer de logout Auto
	if(TimerLogout!=NULL) KillTimer(MachineView->GetSafeHwnd(),TimerLogout);
	TimerLogout = SetTimer(MachineView->GetSafeHwnd(),ID_TIMER_LOGOUT,(pParamTempsLogout->value * 60000),NULL);

	//Recuperation pointeur sur Menu Parent
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	//Gestion des entêtes Fonctions EXT
	CString strEntete = _T("");
	int index = 0;
	strEntete = strFonction.Tokenize(_T("_"),index);

	//FONCTIONS AFFICHAGE des fenêtres
	if(strEntete == _T("AFF"))
	{
		((CSplitFrame *)(AfxGetMainWnd()))->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	//FONCTIONS VISION
	else if (strEntete == _T("VISION"))
	{
		((CDialVision * )theApp.VisionView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	else if (strEntete == _T("DEFAUT"))	//V29
	{
		((CDialDefautPiste * )theApp.DefautPisteView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	/*___________________________
		FONCTIONS CAMERA UEYE
	___________________________*/
	else if (strEntete == _T("CAM"))
	{
		((CDialCameras*)UeyeView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	//FONCTIONS PESONS de la Fenetre Pesons
	else if (strEntete == _T("PESON"))
	{
		((CDialPesons*)PesonsView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	//FONCTIONS PESON ETUI de la fentre DialPesee
	else if (strEntete == _T("PESETUI"))
	{
		((CDialPesee*)PeseeView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	//FONCTIONS DMX
	else if (strEntete == _T("DMX"))
	{
		((CDialDMX*)DMXView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	//FONCTIONS ROBOT
	else if(strFonction == _T("ROBOT_DESACTIVER"))
	{

		if(((CDialRobot * )RobotsView)->bAllSelected == true)
		{
			m_Robot1.inhiber(NULL,ROBOT_INHIBER_FORCEOFF);
			m_Robot2.inhiber(NULL,ROBOT_INHIBER_FORCEOFF);
			m_Robot3.inhiber(NULL,ROBOT_INHIBER_FORCEOFF);
		}
		else if(((CDialRobot * )RobotsView)->m_robot != NULL)
		{	
			((CDialRobot * )RobotsView)->m_robot->inhiber(BoutonParent,ROBOT_INHIBER_NORMAL);
		}

	}
	else if(strFonction == _T("ROBOT_ACTIVER"))
	{

		if(((CDialRobot * )RobotsView)->bAllSelected == true)
		{
			m_Robot1.inhiber(NULL,ROBOT_INHIBER_FORCEON);
			m_Robot2.inhiber(NULL,ROBOT_INHIBER_FORCEON);
			m_Robot3.inhiber(NULL,ROBOT_INHIBER_FORCEON);
		}
		else if(((CDialRobot * )RobotsView)->m_robot != NULL)
		{	
			((CDialRobot * )RobotsView)->m_robot->inhiber(BoutonParent,ROBOT_INHIBER_NORMAL);
		}

	}
	else if(strFonction == _T("ROBOT_PARKING"))
	{
		if((theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONARRET)&&
			(theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION))
		{
			theApp.SetInfosString(theApp.TableMessage.GetStr(MESSAGE_HORSOUARRETPRODUCTION_NOK));
			return -1;
		}

		if(((CDialRobot * )RobotsView)->bAllSelected == true)
		{
			*m_Robot1.pBitParking = TRUE;
			*m_Robot2.pBitParking = TRUE;
			*m_Robot3.pBitParking = TRUE;
		}
		else if(((CDialRobot * )RobotsView)->m_robot != NULL)
		{
			*((CDialRobot * )RobotsView)->m_robot->pBitParking = TRUE;
		}

	}
	//COMMANDES CHARGEMENT OF
	else if(strFonction == _T("CHARGEMENT_LISTOF"))
	{
		((CDialChargement*)ChargementView)->OnBnClickedButListe();
	}
	else if(strFonction == _T("CHARGEMENT_CODEBARRE"))//GCA V1.17
	{
		((CDialChargement*)ChargementView)->OnChargementCodeBarre();
	}
	else if(strFonction == _T("CHARGEMENT_NEW"))
	{
		//((CDialChargement*)ChargementView)->Bouton_RecetteNew();	//V2.50
	}
	//Validation Fin lot
	else if(strFonction == _T("VALIDFINLOT_DEBLOCAGE"))
	{
		bool IsToUnlock = false;
		// Verifier que les pesons hors tolerance soient inhibes
		for(int i = 0; i < 6; i++)
		{
			if( ((Pesons[i].StatutEtalon1 == PESON_STATUTETALON_NOK)||
				(Pesons[i].StatutEtalon2 == PESON_STATUTETALON_NOK))
				&& (Pesons[i].PesonMode != PESON_MODEOFF))
			{
				//besoin deblocage
				IsToUnlock = true;
			}
		}

		if(IsToUnlock)
		{
			//Demande Mot de passe Manager
			if(DemandeLoginManager(m_Recette.UserForcage,m_Recette.DateForcage))
			{
				theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE, _T("Forcage Validation Fin Lot"),m_Recette.DateForcage,m_Recette.UserForcage);
				m_Recette.IsFinLot_Forced = true;
			}
		}
		else
		{
			//si on a cliquer sur le bouton Unlock mais que les valeurs sont bonnes
			Process.Gerer(CMD_VALIDER);//Cycle Normal
		}

	}
	//COMMANDES PROCESS
	else if (strEntete == _T("PROCESS"))
	{
		theApp.Process.ExecuterFonctionExt(strFonction,BoutonParent);
	}
	/*______________________________________________________
		FENETRE PARAMETRES
	______________________________________________________*/
	else if (strEntete == _T("PARAM"))
	{
		((CDialParam*)ParamView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	
	//FONCTIONS ETIQUETTE V3.4
	else if ((strEntete == _T("ETIQ"))||(strEntete == _T("ETIQUETTE")))
	{
		((CDialEtiquette*)EtiqView)->ExecuterFonctionExt(strFonction,BoutonParent);
	}
	
	//Cas par défaut
	else 
	{
		//Fonction par défaut / ERREUR
	}
	return 0;
}

void CBaseFenetreApp::GererVoyants(void)
{
	//Gestion voyant 
	
	if(m_GestAlarmes.GrpAlarmes[GROUPE_ROBOT1].NbDefauts > 0) 
	{	
		iAffVoyant[INDEX_VOY_ROBOT1] = 1;
	}
	else 
	{	
		iAffVoyant[INDEX_VOY_ROBOT1] = 0;
	}

	if(m_GestAlarmes.GrpAlarmes[GROUPE_ROBOT2].NbDefauts > 0) 
	{	
		iAffVoyant[INDEX_VOY_ROBOT2] = 1;
	}
	else 
	{	
		iAffVoyant[INDEX_VOY_ROBOT2] = 0;
	}

	if(m_GestAlarmes.GrpAlarmes[GROUPE_ROBOT3].NbDefauts > 0) 
	{	
		iAffVoyant[INDEX_VOY_ROBOT3] = 1;
	}
	else 
	{	
		iAffVoyant[INDEX_VOY_ROBOT3] = 0;
	}
	
	if(m_GestAlarmes.GrpAlarmes[GROUPE_OPC].NbDefauts != 0) 
	{	
		iAffVoyant[INDEX_VOY_OPC] = 1;
	}
	else
	{
		iAffVoyant[INDEX_VOY_OPC] = 0;
	}

	if(m_GestAlarmes.GrpAlarmes[GROUPE_CAM1].NbDefauts != 0) 
		iAffVoyant[INDEX_VOY_CAM1] = 1;
	else 
		iAffVoyant[INDEX_VOY_CAM1] = 0;

	if(m_GestAlarmes.GrpAlarmes[GROUPE_CAMUV].NbDefauts != 0) 
		iAffVoyant[INDEX_VOY_CAMUV] = 1;
	else 
		iAffVoyant[INDEX_VOY_CAMUV] = 0;

	//V37TODO : A vérifier
	//if(*VisionBoite.pBitInhibition == TRUE)
	//	iAffVoyant[INDEX_VOY_CAM3] = -1;
	//else if(m_GestAlarmes.GrpAlarmes[GROUPE_CAM_UEYE1].NbDefauts != 0) 
	//	iAffVoyant[INDEX_VOY_CAM3] = 1;
	//else
	//	iAffVoyant[INDEX_VOY_CAM3] = 0;
	//
	//if(*VisionBoite.pBitInhibition == TRUE)
	//	iAffVoyant[INDEX_VOY_CAM4] = -1;
	//else if(m_GestAlarmes.GrpAlarmes[GROUPE_CAM_UEYE2].NbDefauts != 0) 
	//	iAffVoyant[INDEX_VOY_CAM4] = 1;
	//else
	//	iAffVoyant[INDEX_VOY_CAM4] = 0;
	//Voyant poste lecture boite

	//V 44.0 

	/*if(pParamLectureBoite->value == FALSE)
		*VisionBoite.pBitInhibitionLectureBoite = TRUE;
	else
		*VisionBoite.pBitInhibitionLectureBoite = FALSE;

	if(*VisionBoite.pBitInhibitionLectureBoite == TRUE)
	{
		iAffVoyant[INDEX_VOY_CAMBOITE] = -1;
	}
	else if(m_GestAlarmes.GrpAlarmes[GROUPE_CAM_UEYE1].NbDefauts != 0) 
		iAffVoyant[INDEX_VOY_CAMBOITE] = 1;
	else if(m_GestAlarmes.GrpAlarmes[GROUPE_CAM_UEYE2].NbDefauts != 0) 
		iAffVoyant[INDEX_VOY_CAMBOITE] = 1;
	else
		iAffVoyant[INDEX_VOY_CAMBOITE] = 0;*/

	//V37TODO : A vérifier
	//Voyant Impression Wolke
	if(*EtiqWolke.pBitInhibImpression == TRUE)
		iAffVoyant[INDEX_VOY_IMPWOLKE] = -1;
	else
		iAffVoyant[INDEX_VOY_IMPWOLKE] = 0;

	//Voyant Impression Scellement
	if(*EtiqScellement.pBitInhibImpression == TRUE)
		iAffVoyant[INDEX_VOY_IMPCELLEMENT] = -1;
	else
		iAffVoyant[INDEX_VOY_IMPCELLEMENT] = 0;

	//Voyant Lecture Wolke
	if(*EtiqWolke.pBitInhibLecture == TRUE)
		iAffVoyant[INDEX_VOY_LECTWOLKE] = -1;
	else
		iAffVoyant[INDEX_VOY_LECTWOLKE] = 0;

	//Voyant Lecture Scellement
	if (*EtiqScellement.pBitInhibLecture == TRUE)
		iAffVoyant[INDEX_VOY_LECTCELLEMENT] = -1;
	else
		iAffVoyant[INDEX_VOY_LECTCELLEMENT] = 0;
		
	//V 44.0 if (*VisionBoite.pBitInhibitionLectureBoite == TRUE)
	/*if (*EtiqScellement.pBitInhibLecture == TRUE)
	{
		iAffVoyant[INDEX_VOY_CAMBOITE] = -1;
	}
	else if (m_GestAlarmes.GrpAlarmes[GROUPE_CAM_UEYE1].NbDefauts != 0)
		iAffVoyant[INDEX_VOY_CAMBOITE] = 1;
	else if (m_GestAlarmes.GrpAlarmes[GROUPE_CAM_UEYE2].NbDefauts != 0)
		iAffVoyant[INDEX_VOY_CAMBOITE] = 1;
	else
		iAffVoyant[INDEX_VOY_CAMBOITE] = 0;*/

	if(m_GestAlarmes.GrpAlarmes[GROUPE_PESAGE].NbDefauts != 0) 
		iAffVoyant[INDEX_VOY_PESAGE] = 1;
	else 
		iAffVoyant[INDEX_VOY_PESAGE] = 0;

	//liaison Voyants/Variables

	if(m_Robot1.RobotMode==ROBOT_MODEOFF) iAffVoyant[INDEX_VOY_ROBOT1] = -1;
	if(m_Robot2.RobotMode==ROBOT_MODEOFF) iAffVoyant[INDEX_VOY_ROBOT2] = -1;
	if(m_Robot3.RobotMode==ROBOT_MODEOFF) iAffVoyant[INDEX_VOY_ROBOT3] = -1;

	if(m_Robot1.RobotMode==ROBOT_MODEON) iAffVoyant[INDEX_VOY_ROBOT1] = 0;
	if(m_Robot2.RobotMode==ROBOT_MODEON) iAffVoyant[INDEX_VOY_ROBOT2] = 0;
	if(m_Robot3.RobotMode==ROBOT_MODEON) iAffVoyant[INDEX_VOY_ROBOT3] = 0;

	if((m_Robot1.RobotMode==ROBOT_MODEPARKING)||(m_Robot1.RobotMode==ROBOT_MODEVIDAGE)) iAffVoyant[INDEX_VOY_ROBOT1] = 2;
	if((m_Robot2.RobotMode==ROBOT_MODEPARKING)||(m_Robot2.RobotMode==ROBOT_MODEVIDAGE)) iAffVoyant[INDEX_VOY_ROBOT2] = 2;
	if((m_Robot3.RobotMode==ROBOT_MODEPARKING)||(m_Robot3.RobotMode==ROBOT_MODEVIDAGE)) iAffVoyant[INDEX_VOY_ROBOT3] = 2;

	if(Pesons[0].PesonMode==PESON_MODEOFF) iAffVoyant[INDEX_VOY_PESON1] = -1;
	if(Pesons[1].PesonMode==PESON_MODEOFF) iAffVoyant[INDEX_VOY_PESON2] = -1;
	if(Pesons[2].PesonMode==PESON_MODEOFF) iAffVoyant[INDEX_VOY_PESON3] = -1;
	if(Pesons[3].PesonMode==PESON_MODEOFF) iAffVoyant[INDEX_VOY_PESON4] = -1;
	if(Pesons[4].PesonMode==PESON_MODEOFF) iAffVoyant[INDEX_VOY_PESON5] = -1;
	if(Pesons[5].PesonMode==PESON_MODEOFF) iAffVoyant[INDEX_VOY_PESON6] = -1;

	if(Pesons[0].PesonMode==PESON_MODEON) iAffVoyant[INDEX_VOY_PESON1] = 0;
	if(Pesons[1].PesonMode==PESON_MODEON) iAffVoyant[INDEX_VOY_PESON2] = 0;
	if(Pesons[2].PesonMode==PESON_MODEON) iAffVoyant[INDEX_VOY_PESON3] = 0;
	if(Pesons[3].PesonMode==PESON_MODEON) iAffVoyant[INDEX_VOY_PESON4] = 0;
	if(Pesons[4].PesonMode==PESON_MODEON) iAffVoyant[INDEX_VOY_PESON5] = 0;
	if(Pesons[5].PesonMode==PESON_MODEON) iAffVoyant[INDEX_VOY_PESON6] = 0;

	if(Pesons[0].PesonMode==PESON_MODEVERIFDYNAMIQUE) iAffVoyant[INDEX_VOY_PESON1] = 2;
	if(Pesons[1].PesonMode==PESON_MODEVERIFDYNAMIQUE) iAffVoyant[INDEX_VOY_PESON2] = 2;
	if(Pesons[2].PesonMode==PESON_MODEVERIFDYNAMIQUE) iAffVoyant[INDEX_VOY_PESON3] = 2;
	if(Pesons[3].PesonMode==PESON_MODEVERIFDYNAMIQUE) iAffVoyant[INDEX_VOY_PESON4] = 2;
	if(Pesons[4].PesonMode==PESON_MODEVERIFDYNAMIQUE) iAffVoyant[INDEX_VOY_PESON5] = 2;
	if(Pesons[5].PesonMode==PESON_MODEVERIFDYNAMIQUE) iAffVoyant[INDEX_VOY_PESON6] = 2;
	

	if(*m_DMXEtui.pBitInhibition == TRUE)iAffVoyant[INDEX_VOY_DMXETUI] = -1;
	else if(*m_DMXEtui.pBitEtatConnexion == DMX_DISCONNECTED)iAffVoyant[INDEX_VOY_DMXETUI] = 1;
	else iAffVoyant[INDEX_VOY_DMXETUI] = 0;
	
	if(*m_DMXNotice1.pBitInhibition == TRUE)iAffVoyant[INDEX_VOY_DMXNOTICE1] = -1;
	else if(*m_DMXNotice1.pBitEtatConnexion == DMX_DISCONNECTED)iAffVoyant[INDEX_VOY_DMXNOTICE1] = 1;
	else iAffVoyant[INDEX_VOY_DMXNOTICE1] = 0;

	if(*m_DMXNotice2.pBitInhibition == TRUE)iAffVoyant[INDEX_VOY_DMXNOTICE2] = -1;
	else if(*m_DMXNotice2.pBitEtatConnexion == DMX_DISCONNECTED)iAffVoyant[INDEX_VOY_DMXNOTICE2] = 1;
	else iAffVoyant[INDEX_VOY_DMXNOTICE2] = 0;

	if(PesonEtui.PesonMode==PESON_MODEOFF)iAffVoyant[INDEX_VOY_PESAGE] = -1;
	if(PesonEtui.PesonMode==PESON_MODEON)iAffVoyant[INDEX_VOY_PESAGE] = 0;

	//Voyants cycle Automate
	iAffVoyant[INDEX_VOY_MAINT] = -1;
	iAffVoyant[INDEX_VOY_VIDAGE] = -1;
	iAffVoyant[INDEX_VOY_AUTO] = -1;

	if((Process.pBitAuto != NULL)
		&&(Process.pBitMaintenance != NULL)
		&&(Process.pBitVidage != NULL))
	{
		if(*Process.pBitAuto == TRUE )
		{
			iAffVoyant[INDEX_VOY_AUTO] = 0;
		}
		else if(*Process.pBitMaintenance == TRUE )
		{
			iAffVoyant[INDEX_VOY_MAINT] = 0;
		}
		else if(*Process.pBitVidage == TRUE )
		{
			iAffVoyant[INDEX_VOY_VIDAGE] = 0;
		}
	}
	if(IsWindowVisible(OPCView->GetSafeHwnd()))
	{
		((CDialGrpOPC *) OPCView)->voy_Auto.Invalidate();
		((CDialGrpOPC *) OPCView)->voy_Vidage.Invalidate();
		((CDialGrpOPC *) OPCView)->voy_Maint.Invalidate();
	}

	if(IsWindow(MachineView->GetSafeHwnd()))
	{
		
		((CDialMachine *) MachineView)->voy_robot1.Invalidate();
		((CDialMachine *) MachineView)->voy_robot2.Invalidate();
		((CDialMachine *) MachineView)->voy_robot3.Invalidate();
		((CDialMachine *) MachineView)->voy_Opc.Invalidate();
		((CDialMachine *) MachineView)->voy_peson1.Invalidate();
		((CDialMachine *) MachineView)->voy_peson2.Invalidate();
		((CDialMachine *) MachineView)->voy_peson3.Invalidate();
		((CDialMachine *) MachineView)->voy_peson4.Invalidate();
		((CDialMachine *) MachineView)->voy_peson5.Invalidate();
		((CDialMachine *) MachineView)->voy_peson6.Invalidate();
		((CDialMachine *) MachineView)->voy_DMX1.Invalidate();
		((CDialMachine *) MachineView)->voy_DMX2.Invalidate();
		((CDialMachine *) MachineView)->voy_DMX3.Invalidate();
		((CDialMachine *) MachineView)->voy_Cam1.Invalidate();
		((CDialMachine *) MachineView)->voy_camUV.Invalidate();
		((CDialMachine *) MachineView)->voy_pesage.Invalidate();

		//V37 ((CDialMachine *) MachineView)->voy_Cam3.Invalidate();
		//V37 ((CDialMachine *) MachineView)->voy_Cam4.Invalidate();
		//V44.0 ((CDialMachine *) MachineView)->voy_CamBoite.Invalidate();		//V37
		((CDialMachine *) MachineView)->voy_ImpWolke.Invalidate();		//V37
		((CDialMachine *) MachineView)->voy_ImpScellement.Invalidate();	//V37
		((CDialMachine *) MachineView)->voy_LectWolke.Invalidate();		//V37
		((CDialMachine *) MachineView)->voy_LectScellement.Invalidate();	//V37
		
		
		if(IsWindowVisible(MachineView->GetSafeHwnd()))
		{
			((CDialMachine *) MachineView)->voy_robot1.UpdateWindow();
			((CDialMachine *) MachineView)->voy_robot2.UpdateWindow();
			((CDialMachine *) MachineView)->voy_robot3.UpdateWindow();
			((CDialMachine *) MachineView)->voy_Opc.UpdateWindow();
			((CDialMachine *) MachineView)->voy_peson1.UpdateWindow();
			((CDialMachine *) MachineView)->voy_peson2.UpdateWindow();
			((CDialMachine *) MachineView)->voy_peson3.UpdateWindow();
			((CDialMachine *) MachineView)->voy_peson4.UpdateWindow();
			((CDialMachine *) MachineView)->voy_peson5.UpdateWindow();
			((CDialMachine *) MachineView)->voy_peson6.UpdateWindow();
			((CDialMachine *) MachineView)->voy_DMX1.UpdateWindow();
			((CDialMachine *) MachineView)->voy_DMX2.UpdateWindow();
			((CDialMachine *) MachineView)->voy_DMX3.UpdateWindow();
			((CDialMachine *) MachineView)->voy_Cam1.UpdateWindow();
			((CDialMachine *) MachineView)->voy_camUV.UpdateWindow();
			((CDialMachine *) MachineView)->voy_pesage.UpdateWindow();

			//V37 ((CDialMachine *) MachineView)->voy_Cam3.UpdateWindow();
			//V37 ((CDialMachine *) MachineView)->voy_Cam4.UpdateWindow();
			//V44.0 ((CDialMachine *) MachineView)->voy_CamBoite.UpdateWindow();		//V37
			((CDialMachine *) MachineView)->voy_ImpWolke.UpdateWindow();		//V37
			((CDialMachine *) MachineView)->voy_ImpScellement.UpdateWindow();	//V37
			((CDialMachine *) MachineView)->voy_LectWolke.UpdateWindow();		//V37
			((CDialMachine *) MachineView)->voy_LectScellement.UpdateWindow();	//V37
		}

	}	
}		
int CBaseFenetreApp::MAJMenuEtat(void)
{

	//Gestion specifique pour le menu Status en fonction du process et pas du profil
	((CFormMenuMachine*)pFormMenu)->staticUtilisateur.strText = CurrentUser.strLibelle;
	((CFormMenuMachine*)pFormMenu)->staticUtilisateur.Invalidate();

	((CFormMenuMachine*)pFormMenu)->StaticInfos.strText = Process.sEtat[Process.iEtat];
	((CFormMenuMachine*)pFormMenu)->StaticInfos.Invalidate();

	((CFormStatus*)pFormStatus)->MenuStatus.strProfil = theApp.Process.sEtat[theApp.Process.iEtat];
	((CFormStatus*)pFormStatus)->MenuStatus.MajMenu(NULL,NULL);

	// on empeche la fermeture de l'application pendant la production
	CMenu *pSysMenu =AfxGetMainWnd()->GetSystemMenu(FALSE);
	if(pSysMenu != NULL)
	{
		if(theApp.Process.iEtat == ETAT_PROCESS_HORSPRODUCTION)
		{
			pSysMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND  |MF_ENABLED   );
		}
		else
		{
			pSysMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND  |MF_GRAYED   );
		}
	}
	

	return 0;
}

UINT CBaseFenetreApp::CommOPC(LPVOID pParam)
{

	CClientOPC* m_Grp = (CClientOPC*) pParam;
	
	clock_t  Time = 0,PrevTime = 0;
	CString strTime;

	do
	{
		Sleep(10);//Sleep(5) pas de différence dans une boucle vide tm = ~15ms

		//Calcul Temps de cycle OPC A DECOMMENTER
		/*Time = GetTickCount();
		imSeconde = (double)(Time - PrevTime)  ;
		PrevTime = Time;
		strTime.Format(_T("OPC : %4.2f mS \n"),imSeconde);
		::OutputDebugString(strTime);*/
		//Fin Calcul Temps de cycle OPC 
		m_Grp[GROUPE_RECETTE].TickOPC++;


		//OPC Gerer Gestion des données
		m_Grp[GROUPE_RECETTE].GererCycle(0);
		m_Grp[GROUPE_PROCESS_R].GererCycle(0);
		m_Grp[GROUPE_ROBOT2_TR].GererCycle(2);
		m_Grp[GROUPE_ROBOT3_TR].GererCycle(3);
		m_Grp[GROUPE_ROBOT1_TR].GererCycle(1);
		m_Grp[GROUPE_PESAGE_R].GererCycle(0);

		// LEcture/Ecriture OPC 
		m_Grp[GROUPE_PROCESS_R].ReadItemsGroup();
		m_Grp[GROUPE_ROBOT1_TR].ReadItemsGroup();
		m_Grp[GROUPE_ROBOT2_TR].ReadItemsGroup();
		m_Grp[GROUPE_ROBOT3_TR].ReadItemsGroup();
		m_Grp[GROUPE_COMPTEURS].ReadItemsGroup();

		//V32
		m_Grp[GROUPE_PESAGE_R].ReadItemsGroup();

		m_Grp[GROUPE_PROCESS_R].OPCToData();
		m_Grp[GROUPE_ROBOT1_TR].OPCToData();
		m_Grp[GROUPE_ROBOT2_TR].OPCToData();
		m_Grp[GROUPE_ROBOT3_TR].OPCToData();
		m_Grp[GROUPE_COMPTEURS].OPCToData();

		//V32
		m_Grp[GROUPE_PESAGE_R].OPCToData();
		
		//Inversion des mots spécifiques
		theApp.PesonEtui.LastMesure = (theApp.PesonEtui.iVal211_12 * 256)  + theApp.PesonEtui.iVal211_13; 
		theApp.PesonEtui.StabilisedValue = (theApp.PesonEtui.iVal211_26 * 256)  + theApp.PesonEtui.iVal211_27; 

		theApp.PesonEtui.iPoidsMoyenMinims = (theApp.PesonEtui.iVal110_10 * 256)  + theApp.PesonEtui.iVal110_11; 
		theApp.PesonEtui.iTareEtuiVide = (theApp.PesonEtui.iVal110_12 * 256)  + theApp.PesonEtui.iVal110_13; 
		theApp.PesonEtui.iTareEtuiPlein = (theApp.PesonEtui.iVal110_14 * 256)  + theApp.PesonEtui.iVal110_15; 

		//V16 : Cadence
		theApp.Process.dCadence = (theApp.Process.iVal110_8 * 256 + theApp.Process.iVal110_9) / 10.0;

		m_Grp[GROUPE_RECETTE].DataToOPC();
		m_Grp[GROUPE_PROCESS_W].DataToOPC();
		m_Grp[GROUPE_ROBOT1_TW].DataToOPC();
		m_Grp[GROUPE_ROBOT2_TW].DataToOPC();
		m_Grp[GROUPE_ROBOT3_TW].DataToOPC();

		m_Grp[GROUPE_PROCESS_W].WriteItemsGroup();
		m_Grp[GROUPE_RECETTE].WriteItemsGroup();
		m_Grp[GROUPE_ROBOT1_TW].WriteItemsGroup();
		m_Grp[GROUPE_ROBOT2_TW].WriteItemsGroup();
		m_Grp[GROUPE_ROBOT3_TW].WriteItemsGroup();	

		theApp.Process.bThreadStopped = false;
	}
	while(!theApp.Process.bStopThread);

	m_Grp[GROUPE_RECETTE].EndGroupe();
	m_Grp[GROUPE_PROCESS_R].EndGroupe();
	m_Grp[GROUPE_PROCESS_W].EndGroupe();
	m_Grp[GROUPE_ROBOT1_TR].EndGroupe();
	m_Grp[GROUPE_ROBOT1_TW].EndGroupe();
	m_Grp[GROUPE_ROBOT2_TW].EndGroupe();
	m_Grp[GROUPE_ROBOT2_TR].EndGroupe();
	m_Grp[GROUPE_ROBOT3_TW].EndGroupe();
	m_Grp[GROUPE_ROBOT3_TR].EndGroupe();
	m_Grp[GROUPE_COMPTEURS].EndGroupe();//GCA1412
	//V32
	m_Grp[GROUPE_PESAGE_R].EndGroupe();

	theApp.Process.bThreadStopped = true;

	TRACE("Thread OPC STOP\n");
	AfxEndThread(100);
	return 0;
}

int CBaseFenetreApp::StartThreadOPC(void)
{
	theApp.Process.bStopThread = false;
	TRACE("Thread OPC START\n");
	m_Thread = AfxBeginThread(CommOPC, &m_Groupes);
	
	if(m_Thread == NULL)
		return 0;
	else
		return 1;
}

int CBaseFenetreApp::StopThreadOPC(void)
{
	theApp.Process.bStopThread = true;

	while (!theApp.Process.bThreadStopped)
	{
		Sleep(10);
	}

	return 0;
}



int CBaseFenetreApp::SetEventsJournalStr(CString strMessage)
{
	if(AlarmesView > 0)
	{
		((CDialAlarmes*)AlarmesView)->AfficherChaineListe(strMessage.GetBuffer());
		strMessage.ReleaseBuffer();
	}
	else
	{
		return -1;
	}

	return 0;
}
int CBaseFenetreApp::ResetDefautsEnCours()
{
	if(AlarmesView > 0)
	{
		((CDialAlarmes*)AlarmesView)->ResetDefautEnCours();


		//Fenetre Machine
		if(IsWindow(((CDialMachine*)MachineView)->ListDefMachine.GetSafeHwnd()))
		{
			((CDialMachine*)MachineView)->ListDefMachine.ResetContent();
			((CDialMachine*)MachineView)->Liste_Alertes.ResetContent();
		}
		
	}
	else
	{
		return -1;
	}

	if(DialMixup > 0)
	{
		if(IsWindow(((CDialMixup*)DialMixup)->ListCurrentDef.GetSafeHwnd()))
		{
			if(IsWindowVisible(((CDialMixup*)DialMixup)->ListCurrentDef.GetSafeHwnd()))
			{
				((CDialMixup*)DialMixup)->ListCurrentDef.ResetContent();
			}
		}
	}
	else
	{
		return -1;
	}

	return 0;
}
int CBaseFenetreApp::SetDefautsEnCours(CString strMessage)
{

	if(AlarmesView > 0)
	{
		((CDialAlarmes*)AlarmesView)->AfficherDefautEnCours(strMessage);

		if(IsWindow(((CDialMachine*)MachineView)->ListDefMachine.GetSafeHwnd()))
		{
			//((CDialMachine*)MachineView)->ListDefMachine.InsertString(0,chTemp);
			((CDialMachine*)MachineView)->ListDefMachine.AddString(strMessage);
		}
	}
	else
	{
		return -1;
	}

	if(DialMixup > 0)
	{
		if(IsWindow(((CDialMixup*)DialMixup)->ListCurrentDef.GetSafeHwnd()))
		{
			if(IsWindowVisible(((CDialMixup*)DialMixup)->ListCurrentDef.GetSafeHwnd()))
			{
				((CDialMixup*)DialMixup)->ListCurrentDef.InsertString(0, strMessage);
			}
		}
	}
	else
	{
		return -1;
	}

	return 0;
}
int CBaseFenetreApp::SetAlertesEnCours(CString strMessage)
{

	if(AlarmesView > 0)
	{
		if(::IsWindow(((CDialAlarmes*)AlarmesView)->Liste_Alertes.GetSafeHwnd()))//0.99
		{
			((CDialAlarmes*)AlarmesView)->Liste_Alertes.AddString(strMessage);
		}

		if(IsWindow(((CDialMachine*)MachineView)->Liste_Alertes.GetSafeHwnd()))
		{
			//((CDialMachine*)MachineView)->Liste_Alertes.InsertString(0,chTemp);
			((CDialMachine*)MachineView)->Liste_Alertes.AddString(strMessage);
		}
	}
	else
	{
		return -1;
	}

	if(DialMixup > 0)
	{
		if(IsWindow(((CDialMixup*)DialMixup)->ListCurrentDef.GetSafeHwnd()))
		{
			if(IsWindowVisible(((CDialMixup*)DialMixup)->ListCurrentDef.GetSafeHwnd()))
			{
				((CDialMixup*)DialMixup)->ListCurrentDef.InsertString(0, strMessage);
			}
		}
	}
	else
	{
		return -1;
	}

	return 0;
}
int CBaseFenetreApp::GererSynchro(void)
{
	if(Process.pBitDeVie  == NULL)
		return 1;

	if(	*Process.pBitDeVie == TRUE)
	{
		*Process.pBitDeVie = FALSE;
	}
	else
	{
		*Process.pBitDeVie = TRUE;
	}

	return 0;
}
afx_msg void CBaseFenetreApp::OnExit()
{
	//Verification Cycle En cours /arret App dans Splitfrm::OnsysCommand
	AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND,SC_CLOSE,0);
}

//V 45.0 DEV
/*	Uniformisation des fonctions de login 
	(Login/DemandeLoginManager/DemandeLogin)

*/
bool CBaseFenetreApp::Login_DoVerification(CString strLogin, CString strMdp, CUtilisateur * UtilisateurEnCours)
{
	//V 40  Login via LDAP sur annuaire Active Directory
	if ((pParamAD_ModeLoginUser->value == MODE_AD) &&
		(bDoModeDegrade == false))
	{
		CStringArray Liste_Groupes;
		CStringArray Profils;

		if (AuthenticateADUser(
			strLogin
			, strMdp
			, UtilisateurEnCours->strLibelle
			, Liste_Groupes
			, Profils))
		{
			//au moins un profil connu sur cette machine
			if (Profils.GetCount() == 1)
			{
				CADORecordset rsUser(&theBase.dao_Base);

				if (ACBaseStrRsOpen(Profils.GetAt(0), _T("Libelle_AD"), _T("Profils"), &rsUser))
				{
					int iTemp = ACBaseGetInt(_T("Maintenance"), &rsUser);
					iTemp == 1 ? UtilisateurEnCours->bMaintenance = TRUE : UtilisateurEnCours->bMaintenance = FALSE;
					UtilisateurEnCours->NiveauOperateur = (short)ACBaseGetInt(_T("Niveau"), &rsUser);
					UtilisateurEnCours->strProfil = ACBaseGetStr(_T("Libelle"), &rsUser);

					rsUser.Close();
				}
				else
				{
					if (rsUser.IsOpen())
					{
						rsUser.Close();
					}

					AfxMessageBox(_T("LogonUser ECHEC: Le profil est inconnu sur cette machine/application (Profil_AD a ajouté dans Profils) !"));
					return false;
				}

				UtilisateurEnCours->strCode = strLogin;
				///*
				//CHARGEMENT DES INFORMATIONS APPLICATION EN FONCTION DU PROFIL
				//*/
				//CurrentUser.IdBase = theApp.theJournal.Ajouter(JOURNAL_TYPELOGIN, CurrentUser.strLibelle, _T("Login"));

				//CurrentUser.strCode = UtilisateurEnCours->strCode = strUtilisateur;//V 43-ALL
				//CurrentUser.strProfil = UtilisateurEnCours->strProfil;
				//CurrentUser.strLibelle = UtilisateurEnCours->strLibelle;
				//CurrentUser.bIsActif = UtilisateurEnCours->bIsActif;
				//CurrentUser.NiveauOperateur = UtilisateurEnCours->NiveauOperateur;
				//CurrentUser.bMaintenance = UtilisateurEnCours->bMaintenance;

				////MAJ Valeur Process/MAchine
				//if (CurrentUser.bMaintenance)
				//{
				//	*Process.pBitMaintenance = TRUE;
				//}
				//else
				//{
				//	*Process.pBitMaintenance = FALSE;
				//}
				//MAJMenu();
				//return;
			}
			else
			{	// (iNBProfilsOKsurMachineEnCours > 1)
				//L'utilisateur choisi avec quel profil il se connecte
				CParametreLong ParamSaisie;
				CString strTemp = _T("");
				for (INT_PTR j = 0; j < Profils.GetCount(); j++)
				{
					if (Profils.GetAt(j) != _T(""))
					{
						strTemp.Format(_T("%d/%d-%s;"), j, j, Profils.GetAt(j));
						ParamSaisie.SelectionChamp += strTemp;
					}
				}

				ParamSaisie.ParentParam = NULL;
				ParamSaisie.value_min = 0;
				ParamSaisie.value_max = 5;
				ParamSaisie.iSelection = SAISIE_LISTEPREDEF;
				ParamSaisie.InitSelectionChamp();

				if (!ParamSaisie.Editer())
				{
					return false;
				}

				CADORecordset rsUser(&theBase.dao_Base);

				if (ACBaseStrRsOpen(Profils.GetAt(ParamSaisie.value), _T("Libelle_AD"), _T("Profils"), &rsUser))
				{
					int iTemp = ACBaseGetInt(_T("Maintenance"), &rsUser);
					iTemp == 1 ? UtilisateurEnCours->bMaintenance = TRUE : UtilisateurEnCours->bMaintenance = FALSE;
					UtilisateurEnCours->NiveauOperateur = (short)ACBaseGetInt(_T("Niveau"), &rsUser);
					UtilisateurEnCours->strProfil = ACBaseGetStr(_T("Libelle"), &rsUser);

					rsUser.Close();
				}
				else
				{
					if (rsUser.IsOpen())
					{
						rsUser.Close();
					}

					AfxMessageBox(_T("LogonUser ECHEC: Le profil est inconnu sur cette machine/application (Profil_AD a ajouté dans Profils) !"));
					return false;
				}
			}
			UtilisateurEnCours->strCode = strLogin;
			return true;
		}
		else
		{
			return false;
		}
	}

	//RAZ mode dégradé
	bDoModeDegrade = false;

	//Verification du login dans la base
	if (UtilisateurEnCours->Lire_Base(strLogin))
	{
		if (UtilisateurEnCours->IsActif())
		{
			if (UtilisateurEnCours->VerifierMotDePasse(strMdp))
			{
				//Mot de passe OK
				//Mise A Zero du Nb Erreurs login
				UtilisateurEnCours->NbErrLogin = 0;
				UtilisateurEnCours->Ecrire_Base();

				return true;
			}
			else
			{
				//Mot de passe NOK
				//UtilisateurEnCours->NbErrLogin ++;
				CString strTemp = _T("");
				strTemp.Format(theApp.TableMessage.GetStr(MESSAGE_USER_INFOS_PASSWORDNOK), (int)UtilisateurEnCours->NbErrLogin);
				theApp.theJournal.Evenement(JOURNAL_TYPELOGIN, UtilisateurEnCours->strLibelle, strTemp, UtilisateurEnCours->strCode);
			}
		}
		else
		{
			// Log Tentative utilisateur Desactivé
			CString strCom;
			strCom.Format(theApp.TableMessage.GetStr(MESSAGE_USER_INFOS_DESACTIVATE));
			theApp.theJournal.Evenement(JOURNAL_TYPELOGIN, UtilisateurEnCours->strLibelle, strCom, UtilisateurEnCours->strCode);

			SetInfosString(UtilisateurEnCours->strCode + _T("\r\n") + theApp.TableMessage.GetStr(MESSAGE_USER_INFOS_DESACTIVATE));
		}
	}
	else
	{
		// Log Tentative utilisateur inconnu
		UtilisateurEnCours->strCode = strLogin;
		CString strCom;
		strCom.Format(theApp.TableMessage.GetStr(MESSAGE_USER_INFOS_INCONNU));
		theApp.theJournal.Evenement(JOURNAL_TYPELOGIN, UtilisateurEnCours->strLibelle, strCom, UtilisateurEnCours->strCode);

		SetInfosString(UtilisateurEnCours->strCode + _T("\r\n") + theApp.TableMessage.GetStr(MESSAGE_USER_INFOS_INCONNU));
	}

	return false;
}

bool CBaseFenetreApp::DemandeLoginManager(CString &strUser,CString &strDate)
{
	//Demande du login
	CString strUtilisateur, strMotDePasse, strTemp;
	CUtilisateur UserManager;
	bool bReturn = false;

	if (!Login_DoSaisieIdentifiants(strUtilisateur, strMotDePasse))
	{
		//Echap ou saisie Nulle sur une des deux fenetres (Login/mdp)
		return false;
	}
	
	//V 43.2 Login user Acyrus, pour évité de chercher quelqu'un avec les droits qu'on veut...
	if (Login_IsDevelopperUser(strUtilisateur, strMotDePasse) == true)
	{
		strUser = _T("ACYRUS");

		//Date
		CTime time;
		tm t1;
		int iJour, iMois, iAnnee;
		int iHeure, iMinute, iSeconde;

		//recupération de la date et l'heure 
		time = CTime::GetCurrentTime();
		t1 = *(time.GetLocalTm(&t1));

		iJour = t1.tm_mday;
		iMois = (t1.tm_mon + 1);//+1-> janvier =0
		iAnnee = (t1.tm_year + 1900);

		iHeure = t1.tm_hour;
		iMinute = t1.tm_min;
		iSeconde = t1.tm_sec;

		//Format: 26/11/2012 18:25:36
		strDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"), iJour, iMois, iAnnee, iHeure, iMinute, iSeconde);

		return true;
	}

	if (!Login_DoVerification(strUtilisateur, strMotDePasse, &UserManager))
	{
		//Echap ou saisie Nulle sur une des deux fenetres (Login/mdp)
		return false;
	}

	//verification Type MANAGER
	if(UserManager.IsManager())
	{
		bReturn =  true;
	}
	else
	{
		AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_USER_INFOS_NOTMANAGER),MB_OK|MB_ICONWARNING);
		bReturn =  false;
	}

	//Date
	CTime time;
	tm t1; 
	int iJour,iMois,iAnnee;
	int iHeure,iMinute,iSeconde;

	//recupération de la date et l'heure 
	time= CTime::GetCurrentTime();
	t1 = *(time.GetLocalTm( &t1 ));

	iJour = t1.tm_mday;
	iMois = (t1.tm_mon+1);//+1-> janvier =0
	iAnnee = (t1.tm_year+1900);

	iHeure = t1.tm_hour;
	iMinute = t1.tm_min;
	iSeconde = t1.tm_sec;
	
	//Format: 26/11/2012 18:25:36
	strDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),iJour,iMois,iAnnee,iHeure,iMinute,iSeconde);

	return bReturn;
}

bool CBaseFenetreApp::DemandeLogin(CString &strUser,CString &strDate)
{
	//Demande du login
	CString strUtilisateur, strMotDePasse, strTemp;
	CUtilisateur User;
	bool bReturn = false;

	if (!Login_DoSaisieIdentifiants(strUtilisateur, strMotDePasse))
	{
		//Echap ou saisie Nulle sur une des deux fenetres (Login/mdp)
		return false;
	}

	//V 43.2 Login user Acyrus, pour évité de chercher quelqu'un avec les droits qu'on veut...
	if (Login_IsDevelopperUser(strUtilisateur, strMotDePasse) == true)
	{
		strUser = _T("ACYRUS");

		//Date
		CTime time;
		tm t1;
		int iJour, iMois, iAnnee;
		int iHeure, iMinute, iSeconde;

		//recupération de la date et l'heure 
		time = CTime::GetCurrentTime();
		t1 = *(time.GetLocalTm(&t1));

		iJour = t1.tm_mday;
		iMois = (t1.tm_mon + 1);//+1-> janvier =0
		iAnnee = (t1.tm_year + 1900);

		iHeure = t1.tm_hour;
		iMinute = t1.tm_min;
		iSeconde = t1.tm_sec;

		//Format: 26/11/2012 18:25:36
		strDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"), iJour, iMois, iAnnee, iHeure, iMinute, iSeconde);

		return true;
	}

	if (!Login_DoVerification(strUtilisateur, strMotDePasse, &User))
	{
		//Echap ou saisie Nulle sur une des deux fenetres (Login/mdp)
		return false;
	}

	//Date
	CTime time;
	tm t1; 
	int iJour,iMois,iAnnee;
	int iHeure,iMinute,iSeconde;

	//recupération de la date et l'heure 
	time= CTime::GetCurrentTime();
	t1 = *(time.GetLocalTm( &t1 ));

	iJour = t1.tm_mday;
	iMois = (t1.tm_mon+1);//+1-> janvier =0
	iAnnee = (t1.tm_year+1900);

	iHeure = t1.tm_hour;
	iMinute = t1.tm_min;
	iSeconde = t1.tm_sec;
	
	//Format: 26/11/2012 18:25:36
	strDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),iJour,iMois,iAnnee,iHeure,iMinute,iSeconde);

	return bReturn;
}


int CBaseFenetreApp::SetStringListVision(CString strInfos)
{
	if(IsWindow(VisionView->GetSafeHwnd()))
	{
		((CDialVision*)VisionView)->m_ListVision.InsertString(0,strInfos);
	}

	return 0;
}

int CBaseFenetreApp::SetStringListVisionTemps(CString strInfos)
{
	if(IsWindow(VisionView->GetSafeHwnd()))
	{
		((CDialVision*)VisionView)->m_ListVisionTemps.InsertString(0,strInfos);
	}

	return 0;
}

int CBaseFenetreApp::SetStringDefautPiste(CString strInfos, int iPiste)	//V30
{
	if(IsWindow(VisionView->GetSafeHwnd()))
	{
		((CDialDefautPiste*)DefautPisteView)->Text_Piste[iPiste].strText = strInfos;
		((CDialDefautPiste*)DefautPisteView)->Text_Piste[iPiste].Invalidate();
	}

	return 0;
}

int CBaseFenetreApp::ClearListVision()		//BM+
{
	((CDialVision*)VisionView)->m_ListVisionTemps.ResetContent();
	((CDialVision*)VisionView)->m_ListVision.ResetContent();
	
	return 0;
}

int CBaseFenetreApp::BackupBase(void)
{
	//Sauvegarde Fichiers Importants
	CString strDir = pParamDirBackup->value + _T("\\") + NOM_DOSSIER_BASE;
	
	if(!PathFileExists(strDir))
	{
		CreateDirectory(strDir, NULL);
	}

	CString strDirSrc;
	CString strDirDest;

	//Construction Nom fichier sauvegarde
	CString Localpath = theConfig.str_RepertoireBase + _T("\\");
	CString path = pParamDirBackup->value + _T("\\") + NOM_DOSSIER_BASE;

	CString strFileName;
	CString strLocalFileName = Localpath + _T("LastSave.bak");
	CTime time;
	tm t1; 
	int iJour,iMois,iAnnee;
	int iHeure,iMinute,iSeconde;

	//recupération de la date et l'heure 
	time= CTime::GetCurrentTime();
	t1 = *(time.GetLocalTm( &t1 ));

	iJour = t1.tm_mday;
	iMois = (t1.tm_mon+1);//+1-> janvier =0
	iAnnee = (t1.tm_year+1900);

	iHeure = t1.tm_hour;
	iMinute = t1.tm_min;
	iSeconde = t1.tm_sec;

#pragma warning (disable: 4129)
	//Format: 26/11/2012 18:25:36/*SET FICHIER=%REPERTOIRE%\BDD_BL_%J%_%M%_%A%_A_%H%_%MN%_%S%.bak*/
	strFileName.Format(_T("%s\BDD_BL_%d_%d_%d_A_%d_%d_%d.bak"),path,iJour,iMois,iAnnee,iHeure,iMinute,iSeconde);
#pragma warning (default: 4129)

	//Executer Procédure stockée de sauvegarde BDD 
	/*PROCEDURE [dbo].[Save_BDD] 
	@FileName varchar(100) */
	CString strRequete;
	try
	{
		//strRequete.Format( _T("EXECUTE [acyrus_bauschlomb].[dbo].[Save_BDD] @FileName = '%s' "),strFileName); 
		//strRequete.Format( _T("EXECUTE [%s].[dbo].[Save_BDD] @FileName = '%s' "),pParamNomBase->value,strFileName);	//V2.58
		strRequete.Format( _T("EXECUTE [%s].[dbo].[Save_BDD] @FileName = '%s' "),pParamNomBase->value,strLocalFileName);	//V2.58
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges|dbSQLPassThrough*/);	
	}
	catch (CException* e)
	{
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause,255);
			e->Delete();

	}
	
	//Copie du fichier a distance
	CopyFile(strLocalFileName,strFileName,FALSE);
	
	return 1;
}

bool CBaseFenetreApp::PurgeDirNbJours(int NbJours, CString str)
{
	CString StrRepertoire = str;
	CString StrTemp;
	CTime time; 
	int numFiles = 0;
	int numFilesToDelete = 0;
	CStringArray ListeFilesToDelete;

	//recupération de la date et l'heure 
	time= CTime::GetCurrentTime();
	

	//Lister Fichiers du repertoire
	 WIN32_FIND_DATA fData;
	  HANDLE          hfind;
	  FILETIME        ft;
	
	  StrTemp = StrRepertoire;
	  StrTemp += _T("*.*");

	  hfind = FindFirstFile(StrTemp, &fData);
	  if(hfind != INVALID_HANDLE_VALUE)
	  {
		do
		{
		  if(!(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		  { 
			//Date de modification
			FileTimeToLocalFileTime(&(fData.ftLastWriteTime), &ft);
			CTime TimeFile(ft);
	        
			//Comparaison Numero du jour
		   CTimeSpan Span = ( time - TimeFile);
		   long AgeFichier =(long) Span.GetDays();
		   if(AgeFichier > NbJours)
		   {

					ListeFilesToDelete.SetAtGrow(numFilesToDelete,fData.cFileName);
					numFilesToDelete++;
			}			
	       	numFiles++;
		  }
		}
		while (FindNextFile(hfind, &fData));
		FindClose(hfind);
	  }
	  else
	  {
		return false;
	  }

	  //Suppression des fichiers concernés

	  for( int i = 0; i< numFilesToDelete;i++)
	  {
		  StrTemp = StrRepertoire;
		  StrTemp  += ListeFilesToDelete.GetAt(i);
		  if(DeleteFile(StrTemp)==0)
		  {
			int NumErreur = GetLastError();
		  }
	  }

	  return true;

}

bool CBaseFenetreApp::CopyDossier(CString strSrc, CString strDest)
{
	char *Src;
	char *Dest;
	CStringA strSrcA,strDestA;

	//Formatage des noms des dossiers pour la copie
	strSrcA = strSrc;
	strDestA = strDest;
	int IndexFinSrc = strSrcA.GetLength();
	int IndexFinDest = strDestA.GetLength();

	Src = (char *) calloc(IndexFinSrc + 4,sizeof(char));
	Dest = (char *) calloc(IndexFinDest + 2,sizeof(char));

	strcpy_s(Src,IndexFinSrc + 4,strSrcA.GetBuffer());
	strcpy_s(Dest,IndexFinDest + 2,strDestA.GetBuffer());
	strSrcA.ReleaseBuffer();
	strDestA.ReleaseBuffer();

	Src[IndexFinSrc] = '\\';
	Src[IndexFinSrc + 1] = '*';
	Src[IndexFinSrc + 2] = '\0';
	Src[IndexFinSrc + 3] = '\0';

	Dest[IndexFinDest+1] = '\0';

	LPCSTR source = Src;
	LPCSTR dest = Dest;
	
	//Préparation de la	 copie du dossier
	SHFILEOPSTRUCT structure_copie;

	structure_copie.hwnd=NULL;
	structure_copie.wFunc=FO_COPY;
	structure_copie.pFrom= source;
	structure_copie.pTo= dest;
	structure_copie.fFlags=FOF_SIMPLEPROGRESS|FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION ;
	structure_copie.fAnyOperationsAborted=true;
	structure_copie.hNameMappings=NULL;
	structure_copie.lpszProgressTitle="Sauvegarde en cours";
 
	//lance la copie des fichiers
	if(SHFileOperation( &structure_copie)==0)
	{
		free(Src);
		free(Dest);
		return true;
	}


	free(Src);
	free(Dest);
	return false;
}
bool CBaseFenetreApp::SetEncoursMachine()
{
	//Enregistrement dans la table EnCours pour Echange avec serveur
	//V2.55 : Envoi seulement des valeurs en cours, sans les valeurs d'origine
	CADORecordset rsEnCours (&theBase.dao_Base);
	
	CString strTemp,strErreur;
	long lTemp = 0L;

	CString strRequete;

	lTemp = m_Recette.NbBoiteOK[1];
	strRequete.Format(_T("UPDATE EnCoursMachine SET [Numerique1]= %ld"),lTemp);//V2.52 

	lTemp = m_Recette.NbBoiteNOK[1];
	strTemp.Format(_T(",[Numerique2]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbBoitePrelev[1];
	strTemp.Format(_T(",[Numerique3]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbPieceOK[1];
	strTemp.Format(_T(",[Numerique4]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbRebutVision[1];
	strTemp.Format(_T(",[Numerique5]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbRebutPoids[1];
	strTemp.Format(_T(",[Numerique6]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbVisionOK[1];
	strTemp.Format(_T(",[Numerique7]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbVisionDMX[1];
	strTemp.Format(_T(",[Numerique8]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbVisionTache[1];
	strTemp.Format(_T(",[Numerique9]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbVisionTacheUV[1];
	strTemp.Format(_T(",[Numerique10]= %ld"),lTemp) ;//V2.52 
	strRequete += strTemp;
	lTemp = m_Recette.NbVisionCB[1];
	strTemp.Format(_T(",[Numerique11]= %ld"),lTemp) ;//V2.52 

	strRequete += strTemp;
	strRequete += _T(" WHERE [Machine]='") + theConfig.str_Machine + _T("'");

	try
	{
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Ecriture Table EnCoursMachine \r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return false;
	}

	return true;
}

void CBaseFenetreApp::StartTimerProcess()
{
	//Lancement Timer Initialisation 
	CWnd* wnd=0;
	CWnd* wnd_splitter=0;
	CMDIChildWnd * child=NULL;
	HWND HWNDTimer = NULL;

	wnd = AfxGetApp()->GetMainWnd();

	if(wnd->IsKindOf(RUNTIME_CLASS(CSplitFrame)))
	{	
		wnd_splitter=((CSplitFrame*)wnd)->m_wndSplitter.GetPane(1,0);

		if(wnd_splitter->IsKindOf(RUNTIME_CLASS(CFormBase)))
		{
			//export des données dans le fichier via la fonction de CForm2
			HWNDTimer = ((CFormBase*)wnd_splitter)->GetSafeHwnd();
		}	
	}

	TimerProcess = SetTimer(HWNDTimer,ID_TIMER_PROCESS,50,NULL);
}

