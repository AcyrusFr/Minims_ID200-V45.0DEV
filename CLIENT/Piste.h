#pragma once

#include "Vision.h" 
#include "Robot.h"
//#include "Calibration.h"
#include "CodePiste.h"
#include "CodeSachet.h"
//V 43.2 #define NBCUMUL	100//V43-ALL #define NBCUMUL	8
#define NBCUMUL	8//V 44.0 #define NBCUMUL	100
#define NBCUMUL_SCROLL	50//V 44.1 
//V23
#define TABTACHE	50
#ifndef _STRUCT_METRAGE
typedef struct _STRUCT_METRAGE
{
	double MetrageDebut;
	double MetrageFin;
	long XpixDebut;
	long XpixFin;
}
STRUCT_METRAGE;
#endif

class CPiste
{
public:
	CPiste(void);
	~CPiste(void);

	long lNumPiste;

	long XMin;
	long XMax;
	long XMinZoom;
	long XMaxZoom;

	long IndexRoi;			//Zone du haut ou zone du bas
	long IndexLastRoi;		//Derniere zone sur laquelle faire le traitement
	long IndexEcriture;
	long IndexEcritureReel;
	bool Saturation;
	bool DeuxiemeMinim;
	bool MinimToutSeul;
	bool TrackEnCours;
	bool AnalyserMinim;
	bool AfficherDefaut;	//V30
	long NumeroImageDebut[2];
	
	long CumulBin;
	long PositionDebut;
	long PositionFin;

	double MetrageFin;
	double MetrageDebut;
	double DistanceDepuisDernier;

	long Result_CumulProj;
	long Result_PositionDebut;
	long Result_PositionFin;
	long Result_IndexEcriture;

	int Result_iDecision;
	int Result_iDefaut;
	long lNextTop;

	STRUCT_POINT PtRobot;
	STRUCT_POINT PtModele;
	STRUCT_POINT PtDatamatrixOrigine;
	STRUCT_POINT PtDatamatrixZoom;
	STRUCT_POINT PtDatamatrix;

	CMImage roiGrab[NBCUMUL];
	CMImage roiGrabZoom;
	CMImage roiBinZoom;
	CMImage roiTrack[NBCUMUL];
	CMImage roiTrackTache[NBCUMUL];
	CMImage roiTrackCouleur[NBCUMUL];	//V2.50
	CMImage roiTrackZoom[NBCUMUL];

	CMImage roiDetectionZoom[2];
	CMImage roiModele;
	CMImage roiBlob;
	CMImage roiDetection[2];
	CMImage roiCode;

	CMImage roiTacheSrc;
	CMImage roiTacheDst;
	CMImage roiSaveToFile;

	CMImage roiSachetRotationSrc;
	CMImage roiSachetRotationDst;
	//CMImage roiSachet2of5Proj;

	CMImage roiResultatSrc;
	CMImage roiResultatDst;

	PARAM_ROI	proiMinim;
	PARAM_ROI	proiDMX1;
	PARAM_ROI	proiDMX2;

	//Robot Associé
	CRobot * m_Robot;

	//Calibrations associées
	//CCalibration *CalibZoom;
	//CCalibration *Calib;
	CCodePiste	CodeObj;
	CCodeSachet CodeSachet[6];	//V33ALL [4] //V32ALL [2]
	CString CodeSachetResult;

	long	PresenceTache[10][800];
	STRUCT_METRAGE	MetrageTache[TABTACHE];

	//V2.44
	double LargeurMinim;
	double LongueurMinim;
	double LongueurRepositionnement;

	//V 43-ALL Stats defauts vision par piste pour aider aux réglages de la distribution
	long lCountDefVision_Surface;
	long lCountDefVision_Inverse;
	long lCountDefVision_Modele;
	long lCountDefVision_Dimensions;
	long lCountDefVision_Codes;
	long lCountDefVision_TachesUV;
	long lCountDefVision_TachesCouleur;
	long lCountDefVision_AnglePrise;

	//V 44.8
	long lCountVision_OK;
};
