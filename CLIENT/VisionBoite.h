#pragma once
#include "Vision.h"

class CVisionBoite
{
public:
	CVisionBoite(void);
	~CVisionBoite(void);

//Communication machine
//V 44.0 	BOOL * pBitInhibitionLectureBoite;
	BOOL * pBitResAcquisition_OK;
	BOOL * pBitResAcquisition_NOK;
	BOOL * pBitMixup;
	BOOL * pBitSnap;
	BOOL  oldBitSnap;
	CString strEtat;

	//V42 : Vision / Images
	bool bSnapOK[2];
	bool bSnapAttente;
	long lX;
	long lY;
	MIL_ID	milSys;

	CMImage imgVisu;
	//V37 CMImage imgVisu_roiExtract[NB_EXTRACTIONS];

	CMImage imgSource;
	CMImage imgSource_roiCam[2];

	CGraphListe Graph_Visu;

	//V37 CMImage imgTrait;
	//V37 CMImage imgRotation;
	//V37 CMImage imgTrait_roiExtract[NB_EXTRACTIONS];
	//V37 CMImage imgRotation_roiExtract[NB_EXTRACTIONS];

	//V37 PARAM_ROI RoiExtract[NB_EXTRACTIONS];
	//V37 PARAM_ROI RoiExtractRotation[NB_EXTRACTIONS];
	//V37 PARAM_ROI RoiExtractBase[NB_EXTRACTIONS];

	//V37 PARAM_ROI RoiResult[NB_EXTRACTIONS];
	//V37 STRUCT_POINT PtCentreResult[NB_EXTRACTIONS];
	//V37 long DecalageX;
	//V37 long DecalageY;

	bool Vision_Charger(MIL_ID idSystem);
	void Vision_Decharger();

	void Vision_OnSnapEye(int iNumCam);
//	void Vision_DoExtraction();
//	bool Vision_DoExtractionSingle(int iNumExtract);
	void ChargerImageTest(CString sFile);
	void InitOF(void);
	void Process_BitSnap();
};


