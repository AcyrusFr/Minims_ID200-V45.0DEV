#pragma once
//#include "Image.h"
#include "StringReader.h"
#include "Modele.h"
#include "Code.h"
#include "Blobs.h"
#include "ImageResult.h"

#define TYPE_EXTRACT_UNDEF		0
#define TYPE_EXTRACT_OCR		1
#define TYPE_EXTRACT_MODELE		2
#define TYPE_EXTRACT_DMX		3	//DMX
#define TYPE_EXTRACT_CODEBARRE	4	//Code barre
#define TYPE_EXTRACT_BLOB		5	//V18
#define TYPE_EXTRACT_STAT		6	//V21

#define NB_TYPES_EXTRACTIONS	7	//V21

class CExtraction
{
public:
	CExtraction();
	~CExtraction(void);

	//BDD
	long IDDetail;
	//Paramètres extraction
	CMImage * img;

	int iNumExtract;
	int iType;
	CString strType;

	CString strFicModele;
	CString strChamp;
	CString strChampSup;
	CString strCodeALire;
	CString strLibelle;
	long lOrientation;

	long lIsRepere;
	STRUCT_POINT PtCentreBase;
	long lContrainte;
	long lVal1;	//V21

	//int iResult;
	bool Extract_bIsOK;//V 44.1
	CString strResult;
	long lResult;	//V21

	//Visualisation Vision
	bool bVisualisation;
	COLORREF ColorVisu;
	CMImage * imgVisu;

	MIL_INT iLabelTracage;

	//Vision
	CStringReader Reader;
	CModele Modele;
	CCode Code;
	CBlobs BlobEtiq;
	CImageResult StatImp;
	

	bool Lire();
	bool Charger(void);
	bool PreProcess(void);
	bool Decharger(void);
	void OpenInteractiveWindow(void);
	bool UpdateFichier(void);
};

