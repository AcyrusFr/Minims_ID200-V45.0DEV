#pragma once
#include "ClientOPC.h"

#include "ProcessDmx.h"

#define ETAT_PROCESS_HORSPRODUCTION	0
#define ETAT_PROCESS_CHARGEMENTOF	1
#define ETAT_PROCESS_VALIDATIONFINLOT	2
#define ETAT_PROCESS_INITIALISATIONMACHINE	3
#define ETAT_PROCESS_VALIDATIONDEBUTLOT	4
#define ETAT_PROCESS_VALIDATIONDEBUTOF	5
#define ETAT_PROCESS_PRODUCTIONARRET	6
#define ETAT_PROCESS_PRODUCTIONDEMARRAGE	7
#define ETAT_PROCESS_PRODUCTIONMARCHE	8
#define ETAT_PROCESS_VALIDATIONFINOF	9
#define ETAT_PROCESS_MIXUP	10	
#define ETAT_PROCESS_VERIFDYNAMIQUE	11
#define ETAT_PROCESS_INITAPP	12
#define ETAT_PROCESS_ARRETENCOURS	13
#define ETAT_PROCESS_VIDAGEARRET	14
#define ETAT_PROCESS_VIDAGEMARCHE	15
#define ETAT_PROCESS_VERIFETALONNAGE	16
#define ETAT_PROCESS_VIDAGE2	17
#define ETAT_PROCESS_ETALONNAGE_ETUI_ARRET	18
#define ETAT_PROCESS_ETALONNAGE_ETUI_MARCHE	19

#define CMD_VALIDER	1
#define CMD_ANNULER	2
#define CMD_DEPARTCYCLE	3
#define CMD_ARRETCYCLE	4

#define ETATMACHINE_HORSCYCLE	0
#define ETATMACHINE_INITIALISATION	1			//OF chargé, Initialisation en cours
#define ETATMACHINE_CYCLEAUTO	2				//Production
#define ETATMACHINE_CYCLEAUTOVIDAGE	3
#define ETATMACHINE_CYCLEAUTODECHARGEMENT	4
#define ETATMACHINE_CYCLEMAINTENANCE	5
#define ETATMACHINE_CYCLEAUTOETUI	6

#define INDEX_BIT_STARTCYCLE_W	0
#define INDEX_BIT_STOPCYCLE_W	1
#define INDEX_BIT_VIDAGE_W	2
#define INDEX_BIT_MAINTENANCE_W	3
#define INDEX_BIT_AUTO_W	4
#define INDEX_BIT_VIDAGE2_W	7

#define INDEX_BIT_STATUSARRET_R	0
#define INDEX_BIT_STATUSINIT_R	1
#define INDEX_BIT_STATUSPROD_R	2
#define INDEX_BIT_STATUSARRETENCOURS_R	3
#define INDEX_BIT_STATUSARRETFINCYCLE_R	4
#define INDEX_BIT_STATUSPLATEAUVIDE_R	5

#define PROCESS_NBCYCLE_BITARRETINIT	10//5 suffisait pas : si au bout de N cycles de lecture on a toujours le bit arret la machine n'a pas démarrée

//Process.RBit2
#define INDEX_BIT_DEFAUTETUYEUSE_R	0
#define INDEX_BIT_DEFAUTROBOTS_R	1
#define INDEX_BIT_DEFAUTETUYEUSE_AFCY_R	2
#define INDEX_BIT_DEFAUTROBOTS_AFCY_R	3
#define INDEX_BIT_DEFAUTROBOTS_MIXUPVISION_R	4
#define INDEX_BIT_DEFAUTROBOTS_SACHETVIDE_R	5
#define INDEX_BIT_DEFAUTROBOTS_MIXUPETUYEUSE_R	6

//GCA 3.7 bits du Process.WBit1
#define INDEX_BIT_MODES_CYCLE_ETALON_PESEE_W	0
#define INDEX_BIT_ETIQ_RESULT_OK_W	1
#define INDEX_BIT_ETIQ_RESULT_NOK_W	2
#define INDEX_BIT_ETIQ_MIXUP_W	3

//V 43.3
//V 44.0
#define NB_MOTS_DB30X	50
//#define NB_MOTS_DB301	5
//#define NB_MOTS_DB302	34//V2.52 : de 20 à 29 pour intégration des nouveaux messages
//							//V2.54 : de 29 à 30 pour intégration des nouveaux messages
//							//GCA 3.7 ajout du bit 30
//							//V21 : de 31 à 32 pour intégration des nouveaux messages
//							//V 39 de 32 à 33
//							//V 42 de 33 à 34
//#define NB_MOTS_DB303	18

#define NB_VAL_CADENCE_MAX	500

typedef struct _PARAM_CADENCE
{
	double dTab_Cadence[NB_VAL_CADENCE_MAX];
	int iModulo_Cadence;
	double dMoy_Cadence;
	ULONGLONG ulTickTimeoutAcquit;
}
PARAM_CADENCE;

class CProcess
{
public:
	CProcess(void);
	~CProcess(void);

public:

	//Alarmes Machine
	ALARME * Alarme;
	ALARME * AlarmeAlertes;

	CClientOPC* m_OPCClient_R;// Read
	CClientOPC* m_OPCClient_W;//Write

	//Gestion Défauts
	CClientOPC * m_OPCClient_Ai;
	CClientOPC * m_OPCClient_AFCy;
	CClientOPC * m_OPCClient_Alertes;

	BOOL Def301[NB_MOTS_DB30X][8];//V 44.0
	BOOL Def302[NB_MOTS_DB30X][8];//V 44.0	
	BOOL Def303[NB_MOTS_DB30X][8];//V 44.0

	int Def301_AdresseMax;//V 44.0
	int Def302_AdresseMax;//V 44.0
	int Def303_AdresseMax;//V 44.0

	int iNbDefautsMachine;

	//Attributs
	int iOldEtat;
	int iEtat;
	int iCmdAuto;
	int iCmdInitOK;
	CString sEtat[20];//Nb Etats Process

	//#3 DMX
	//int iCurentDmx ;
	//CProcessDMX * pDMX[3];
	CServSocketDMX ServeurSocket;

	//Variable Arret thread:
	bool bStopThread;
	bool bThreadStopped;
	
	//Varibales Cycle Machine
	int Step;
	int EtatMachine;
	int FonctStartCycle;

	CString strErreur;

	//Bits MIXUP
	BOOL * pBitMixup[6];

	//Autres

	BOOL WBit0[8];
	BOOL WBit1[8];//GCA 3.7 
	BOOL WBit2[8];
	BOOL WBit3[8];
	BOOL WBit4[8];
	BOOL WBit45[8];
	BOOL WBit5[8];
	BOOL WBit6[8];
	BOOL WBit7[8];
	BOOL WBit8[8];
	BOOL WBit9[8];	//14.0 = Eclairage Supérieur
					//14.1 = Eclairage latéral
					//14.2 = Eclairage UV

	BOOL RBit1[8];//Divers
	BOOL RBit2[8];//Defauts
	BOOL RBit3[8];//Status
	BOOL RBit4[8];

	BYTE iVal110_8;		//V16 Cadence
	BYTE iVal110_9;		//V16 Cadence
	double dCadence;	//V16 Cadence

	//Pointeurs Bits Modes
	BOOL * pBitMaintenance;
	BOOL * pBitAuto;
	BOOL * pBitVidage;
	BOOL * pBitVidage2;		//V2.42
	BOOL * pBitAcquitDefaut;
	BOOL * pBitMachineVide;
	BOOL * pBitDeVie;
	BOOL * pBitEtalonnageEtui;
	BOOL * pBitEclairageSup;//#1 fictif
	BOOL * pBitEclairageLat;//#1 fictif
	BOOL * pBitEclairageUV;//#1 fictif
	BOOL * pBitAutorisationDebutOF;	//#1 fictif //V19

	BOOL ROctetForcage[8];//V 30-ALL
	BOOL WOctetFictif[8];//V 30-ALL

	//V39 #3 DMX
	BYTE iValResult_DmxEtui;	//DB 101.15
	BYTE iValResult_DmxNotice1;	//DB 101.16
	BYTE iValResult_DmxNotice2;	//DB 101.17

	BOOL BitUndef[8];//V44.0 Bits non liès sur l'automate (pour avoir des pointeurs non NULL)

	//index OPC
	int IndexStartCycle;

	int iCyclePulse;

	int iNbCycleInit;

	//V 44.7 gestion alarme cadence faible
	int iCadence_NbValCalcul;
	int iModeCadence;//0 : attente demarrage; 1: demarrage OK, Mode controle; 2: Alarme active, attente acquittement OU cadence > seuil 
	ULONGLONG ulTickDebutControle;

	double dTab_CadenceMachine[NB_VAL_CADENCE_MAX];
	int iModulo_CadenceMachine;
	int iNbMoy_CadenceMachine;
	double dMoy_CadenceMachine;

	//V 45.0
	PARAM_CADENCE CadencePrises[3];
	PARAM_CADENCE CadenceTapis[3];

	//Fonctions

	int Gerer(int iCommande);
	static bool GererCycle(int);
	int GererDefautsMachine(void);
	int GererAlarme(void);
	int GererEtat_HorsProduction(int iCommande);
	int GererEtat_ChargementOf(int iCommande);
	int GererEtat_ValidationFinLot(int iCommande,int iCmdAuto);
	
	int GererEtat_Initialisation(int iCommande);
	int GererEtat_InitialisationAuto(int iCmd);

	int GererEtat_ValidationDebutLot(int iCommande,int iCmdAuto);
	int GererEtat_ValidationDebutOf(int iCommande,int iCmdAuto);
	int GererEtat_ProductionArret(int iCommande,int iCmdAuto);
	int GererEtat_Demarrage(int iCommande,int iCmdAuto);
	int GererEtat_ProductionMarche(int iCommande,int iCmdAuto);
	int GererEtat_ValidationFinOf(int iCommande,int iCmdAuto);
	int GererEtat_Mixup(int iCommande,int iCmdAuto );
	int GererEtat_VerifDynamique(int iCommande,int iCmdAuto);
	int GererEtat_InitApp();
	int GererEtat_ArretEnCours(int iCommande,int iCmdAuto );
	int GererEtat_VidageArret(int iCommande,int iCmdAuto );
	int GererEtat_VidageMarche(int iCommande,int iCmdAuto );
	int GererEtat_VerifEtalonnage(int iCommande,int iCmdAuto );
	int GererEtat_Vidage2(int iCommande,int iCmdAuto );
	int GererEtat_EtalonnageEtui_Arret(int iCommande,int iCmdAuto);
	int GererEtat_EtalonnageEtui_Marche(int iCommande,int iCmdAuto);
	
	bool Autoriser_ValidInit(void);
	void Gerer_Shutdown(void);
    void Gerer_ImpressionAutoFinOF(CString sOF);
	void Gerer_Impression(CString sOF, long lOption);
	void ExecuterFonctionExt(CString strFonction,void * BoutonParent);
	void Defauts_Acquitement();

	void Get_MaxAdresses_DB30X();
	//V 44.0 

	//V 44.7
	void Cadence_CalculerMoy();
	//V 44.8
	void Cadence_CalculerRobots();
	void Cadence_AcquitAlarmes();
	void Cadence_Gerer();
	void Cadence_Reset();
	//V 44.9
};
