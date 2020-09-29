#pragma once
#include "Vision.h"
#include "Extraction.h"
#include "GraphListe.h"

#define NB_CHAMP_MAX		50
#define NB_EXTRACTIONS		20

#define	SRC_AUCUN		0		//V44.5
#define	SRC_ETIFRONT	1		//V44.5
#define	SRC_ITEM_MET	2		//V44.5

#define MENU_EXTRACTION			_T("Sauvegarder Position;ETIQ_SAVEPOS_EXTRACT|Parametres;ETIQ_PARAM_EXTRACT|")
#define MENU_EXTRACTION_MODELES	_T("Sauvegarder Position;ETIQ_SAVEPOS_EXTRACT|Parametres;ETIQ_PARAM_EXTRACT|Creer Modele;ETIQ_EXTRACT_MODELE|")

typedef struct _STRUCT_EXTRACTION
{
	long ID;
	long IDEtiquette;
	CString ETC_Champ;
	CString ETC_ChampSup;
	long RoiX;
	long RoiY;
	long RoiDX;
	long RoiDY;
	long TypeExtract;
	CString FicModele;
	long Orientation;
}
STRUCT_EXTRACTION;

class CEtiquette
{
public:
	CEtiquette(void);
	~CEtiquette(void);

//Gestion de l'étiquette
	//Variables générales propres à l'étiquette
	BOOL		Etiq_bChargementOK;
	CString		Etiq_strEtat;
	CString		Etiq_strCode;
	long		Etiq_ID;
	int			Etiq_iNbExtract;
	bool		Etiq_bTest;
	CString		Etiq_strTitre;

//Gestion des extractions
	//Variables
	STRUCT_EXTRACTION DataExtraction[NB_EXTRACTIONS];
	CExtraction  Extract[NB_EXTRACTIONS];
	//Fonctions
	bool LireEntete(bool bAuto);
	bool Extractions_Charger();
	void Extractions_Draw(CGraphListe * pGraph);

//Gestion Eticoncept
	//Variables
	CString		ETC_strChampBrut[NB_CHAMP_MAX];
	CString		ETC_strChampFormat[NB_CHAMP_MAX];
	CString		ETC_strChampTitre[NB_CHAMP_MAX];
	long		ETC_SrcData[NB_CHAMP_MAX];			//V44.5
	//Fonctions
	bool		ETC_ChargerData(CString CodeOF);
	bool		ETC_AffecterDataTo_EtiqDetail();

//Communication machine
	BOOL *pBitInhibImpression;		//V37TODO : à gérer
	BOOL *pBitInhibLecture;				//V 44.0BOOL bInhibLecture;				
	BOOL bResAcquisition_OK;
	BOOL bResAcquisition_NOK;
	BOOL bMixup;
	int iResultatEtiquette;

	int iForcageResult;

	bool UpdateROI(void);
	bool UpdateReference();
	void InitOF(void);
	bool Apprentissage(int iExtract);

	//V42 : Vision / Images
	long lX;
	long lY;
	bool Vision_Charger(MIL_ID idSystem);
	void Vision_DoExtraction();
	bool Vision_DoExtractionSingle(int iNumExtract);
	MIL_ID	milSys;

	CMImage *imgVisu;	
	CMImage imgVisu_roiExtract[NB_EXTRACTIONS];

	CMImage *imgSource;	

	CMImage imgTrait;
	CMImage imgRotation;
	CMImage imgTrait_roiExtract[NB_EXTRACTIONS];
	CMImage imgRotation_roiExtract[NB_EXTRACTIONS];

	PARAM_ROI RoiExtract[NB_EXTRACTIONS];
	PARAM_ROI RoiExtractRotation[NB_EXTRACTIONS];
	PARAM_ROI RoiExtractBase[NB_EXTRACTIONS];

	PARAM_ROI RoiResult[NB_EXTRACTIONS];
	STRUCT_POINT PtCentreResult[NB_EXTRACTIONS];
	long DecalageX;
	long DecalageY;

	void Vision_Decharger();
};


