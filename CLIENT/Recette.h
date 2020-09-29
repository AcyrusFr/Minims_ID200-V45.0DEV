#pragma once
#pragma warning (disable: 4995)

#include "ClientOPC.h"
#include "GestAlarmes.h"
#include "Article.h"

#define MAX_CAR_DMX	100 //Nbre de caractères maximum dans le code DMX

#define RECETTE_TYPESACHET	2
#define RECETTE_TYPEBLISTER	1
#define RECETTE_TYPESACHET_4ML	4
#define RECETTE_TYPEETUI	3//pour enregistrement Table Articles
#define RECETTE_TYPENOTICE	4
#define RECETTE_TYPEMINIMS	5
#define RECETTE_TYPEETIQ	6
//V44.6 #define RECETTE_TYPEETIQBOITE	7 //GCA V3.4
#define RECETTE_TYPEEMBALLAGE	8 //#2 Emballage

#define RECETTE_FORMAT_NO	0
#define RECETTE_FORMAT_1	1
#define RECETTE_FORMAT_2	2

#define RECETTE_INDEXBIT_RECETTECHARGEE	2
#define RECETTE_INDEXBIT_RAZETALONS	0

#define RECETTE_SOURCE_CSV	0
#define RECETTE_SOURCE_CSVOLD	1
#define RECETTE_SOURCE_MANU	2
#define RECETTE_SOURCE_BASE	3

#define RECETTE_STATUT_NO	0
#define RECETTE_STATUT_CHARGEE	1
#define RECETTE_STATUT_DEBUTLOT	2
#define RECETTE_STATUT_DEBUTOF	3
#define RECETTE_STATUT_PRODUCTION	4
#define RECETTE_STATUT_FINOF	5
#define RECETTE_STATUT_FINLOT	6
#define RECETTE_STATUT_TERMINEE	7		

#define RECETTE_COULEUR_NO	0


class CRecette
{
public:
	CRecette(void);
	~CRecette(void);

	//Variables Recette
	CClientOPC* m_OPCClient;
	//Alarme
	ALARME * Alarme;
	CString strErreur;

	CArticle Article;
	CArticle ArtMinims;
	CArticle ArtNotice1;
	CArticle ArtNotice2;
	CArticle ArtEtui;	
	CArticle ArtEtiquette;//GCA V1.17

	//V44.6 CArticle ArtEtiqBoite;//GCA V3.4
	CArticle ArtEmballage;		//#2 Emballage

	CString  strLot;
	//Datamatrix
	char CodeDMXLot[MAX_CAR_DMX];//Datamatrix Vision
	
	bool IsCSV_OK;
	bool IsOldCSV_OK;
	bool IsLoaded;

	bool IsFinLot_Forced;

	CString  OfNum;
	int Quantite;
	int QuantiteParBoite;

	int Peremption;
	int PeremptionMois;

	//Recette
	float PoidsNominal;
	float PoidsMini;
	float PoidsMaxi;

	int PoidsNominal_mg;
	int PoidsMini_mg;
	int PoidsMaxi_mg;

	int TolEtuiPlein_cg;//"Tolérance négative d'un étui plein" en cg

	int PoidsEtalonSaved[6][2];
	int PoidsEtalonSavedFin[6][2];

	//Bits Contrôles Automate
	BOOL WBit1[8];
	BOOL WBit2[8];
	BOOL * pBitRecetteChargee;
	BOOL * pBitRazEtalons;

	//Pulse
	int iCyclePulse;

	short GestionNotice1;
	short GestionNotice2;
	
	//Compteurs
	long NbBoiteOK[2];		//V2.52 EnCoursMachine.Numerique1
	long NbBoiteNOK[2];		//V2.52 EnCoursMachine.Numerique2
	long NbBoitePrelev[2];	//V2.52 EnCoursMachine.Numerique3

	long NbEtuiOK[2];
	long NbEtuiNOK[2];
	//#3 DMX UNUSED long NbEtuiPleinNOK[2]; : TO REMOVE

	long NbNotice1OK[2];
	long NbNotice1NOK[2];
	long NbNotice2OK[2];
	long NbNotice2NOK[2];	

	long NbPieceOK[2];		//V2.52 EnCoursMachine.Numerique4
	long NbPieceNOK[2];	

	long NbRebutVision[2];	//V2.52 EnCoursMachine.Numerique5
	long NbRebutPoids[2];	//V2.52 EnCoursMachine.Numerique6

	long NbVisionOK[2];		//V2.52 EnCoursMachine.Numerique7
	long NbVisionDMX[2];	//V2.52 EnCoursMachine.Numerique8
	long NbVisionTache[2];	//V2.52 EnCoursMachine.Numerique9
	long NbVisionTacheUV[2];//V2.52 EnCoursMachine.Numerique10
	long NbVisionCB[2];		//V2.52 EnCoursMachine.Numerique11 + V16

	//Process
	int SourceData;
	bool Reprise;
	int iStatut;
	CString UserForcage;
	CString DateForcage;//Format: 26/11/2012 18:25:36
	CString RepertoireImages;//#2

	//V2.50
	CString UserFin;

	//Fonctions
	bool ChargerFromCSVAttente(CString l_OF_Number);
	bool Copy(CRecette * Recette_Dest);
	int Reset(void);
	int SavePoidsEtalon(void);
	int SavePoidsEtalonFin(void);
	int SavePoidsEtalonPeson(int iPeson);

	//Table Coupes
	long ID;
	long IDArticle;
	long IDLot;

	int Ajouter(void);
	int Update(void);
	static bool GererCycle(int);
	//bool VerifierAutorisationListe();	//V2.51 : Suppression de la fonction
	int AjouterArticle(void);
	bool ReCharger(CString strOf);
	int AjouterLot(void);
	int UpdateLot(void);

	//GCA V1.17 Table EnCoursMachine
	bool bIsEnCours;
	bool IsRecetteEnCours();
	void SetRecetteEnCours();
	void EndRecetteEnCours();
	void SetUtilisateurFin();	//V2.50
};
