#pragma once
#include "Defaut.h"


#define PESON_NB_MESURES_HISTO	30

#define PESON_MODEON	0
#define PESON_MODEOFF	-1
#define PESON_MODEVERIFDYNAMIQUE	1

#define PESON_MESURE_OK	1
#define PESON_MESURE_NOK	2

#define PESON_STATUTETALON_NOK	0
#define PESON_STATUTETALON_OK	1

#define PESON_INHIBER_NORMAL	0
#define PESON_INHIBER_FORCEON	1
#define PESON_INHIBER_FORCEOFF	2

class CPeson
{
public:
	CPeson(void);
	~CPeson(void);
	
	//UNUSED void * CurrentRecette;
	int iEtat;
	
	int iNumPeson;
	int iNumPesonBL;

	int LastMesure;//en cg
	int StatutLastMesure;//0->Rien; 1->pesage OK; 2-> Pesage NOK (à archiver)
	
	int StabilisedValue;//en cg

	int PoidsEtalon1;
	int PoidsEtalon2;

	short StatutEtalon1; // 0=>Poids Etalon NOK 1 => Poids Etalon OK
	short StatutEtalon2;

	short PesonMode;//0->Normal -1-> PesonOFF 1-> Verif.Dynamique

	int Acquittement;
	BOOL * pBitInhibition;
	BOOL * pBitAcquittement;
	BOOL * pBitEtalon1OK;
	BOOL * pBitEtalon2OK;
	BOOL * pBitDoTare;

	BOOL * pBitVerifDyna;

	//Tableau des n derniéres mesures glissantes
	int indexHisto;
	int TabMesures[PESON_NB_MESURES_HISTO];

	int indexAffichage;//GCA V1.18

	//Index des valeurs OPC
	int IndexStatutLastPoids;
	int IndexDemandeTare;
	int iCyclePulse;

	//Nombre de mesures pour la verification dynamique
	int NbMesuresVerifDyna;
	CString RefEchantillonsVerifDyna;
	CString RefProcedureVerifDyna;
	double PoidsNomVerifDyna;

	//Defaut
	CDefaut DefautPoidsMin;
	CDefaut DefautPoidsMax;

	//V 44.7 comptage des produits pour cadence par robot
	long lNbMesuresEffectuees;

	int Do_Tare(void);
	int Do_VerifDynamique(void);
	int GererMesure();
	int inhiber(void * BoutonParent,int Mode);
	int GererAlarme(void);
	bool GererValidation(void);
	bool GererProduction(void);
	bool GererVerifDyna(void);
	int InitOF(void);
};
