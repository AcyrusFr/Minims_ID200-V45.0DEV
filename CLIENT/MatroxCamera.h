// MatroxCamera.h: interface for the CMatroxCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATROXCAMERA_H__C11C1398_9F52_4577_988E_0E04963293E9__INCLUDED_)
#define AFX_MATROXCAMERA_H__C11C1398_9F52_4577_988E_0E04963293E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4996)
#pragma warning(disable:4995)


#include "Image.h"
#include "ImageResult.h"
#include "Kernel.h"
#include "Lut.h"
#include "Piste.h"
#include "Modele.h"
#include "CodePiste.h"
#include "Blobs.h"


#define NB_MAX_BLOBS_INITIAL	1000
#define NB_MAX_BLOBS_COULEUR	1000
#define NB_MAX_BLOBS_ROTATION	1000


#define CAM_MAXZONE		10

//#2
//#define	IO_SYNCHRO_0	2
//#define	IO_SYNCHRO_1	3
//#define	IO_SYNCHRO_2	4

#define NBPISTE	3
//V43-ALL #define NBCUMUL	8

class CMatroxCamera  
{
public:
	CMatroxCamera();
	virtual ~CMatroxCamera();

	BOOL LireFromBase(CADODatabase *daoBase, CString sApplication, CString sDossier);
	BOOL Charger(MIL_ID idSystem);
	BOOL Decharger();

	void Fenetre_Ouvrir();
	void Fenetre_Fermer();
	bool IsFenetreOpened;

	void Start();
	void Stop();

	//long Diviseur;
	CString		sErrMsg;

	int CycleTestIO;

	long lX;
	long lY;
	long Acq_Nb;
	long lYCumul;

	double dZoom;
	long lXZoom;
	long lYZoom;
	long lYZoomCumul;

	char m_sAVIFile[150];
	long m_lAVIImage;

	//Pièce en cours de contrôle
	long lCleCoupe;
	long lCleOF;
	CString sArticle;
	
	bool Options_Sauvegarde;
	bool Options_Overlay;
	
	CBlobs InitialBlob;
	CBlobs CouleurBlob;
	CBlobs RotationBlob;

public:
	bool bSave;
	bool bSaveOne;
	int iModeAcquisition;
	int iSnap;

	double Temps_Image;
	double Temps_Calcul;
	double Date_Image;
	double Date_Calcul;

	bool Simulation_Enable;
	int Simulation_IndexLoad;
	int Simulation_NbRoi;
	CMImage Simulation_Image;
	CMImage Simulation_Roi[20];
	CMImage Simulation_RoiPiste;


	void AppliquerParametres();

	//Entete
	BOOL		m_bIsOK;
	int			m_iNumero;
	CString		m_sLibelle;
	int			m_iType;			//0=Mire, 1=Fichier, 2=Camera
	CString		m_sFichier;
	CString		m_sRepertoire;
	long		m_lNumSys;
	long		m_lNumPort;

	//Paramètres camera
	PARAM_CAMERA Param_Camera;

	//Définition des objets
	MIL_ID	milSys;
	MIL_ID	milDig;
	MIL_ID	milDigs;

	MIL_INT64	milAUXIO[3];//V 30-ALL
	MIL_INT64	milUSERBIT[3];//V 30-ALL

	CMImage imgGrab[NBCUMUL];
	CMImage imgGrabVisu;
	CMImage imgGrabZoom;
	CMImage imgSelectionColor;

	CMImage imgScroll;
	CMImage ROIScroll[3];
	CMImage imgScrollTemp;

	CMImage imgProj;
	CMImage imgTrack;
	CMImage imgTrackTache;
	CMImage imgTrackCouleur;		//V2.50
	CMImage imgTrackZoom;
	CMImage imgRotationZoom;
	CMImage roiRotation;

	CMImage imgSachetVertical;
	CMImage imgResultat;

	//CMImage imgWnd;

	CImageResult	imgRes[10];
	CKernel	ker[10];
	CLut	lut[10];
	CPiste Piste[3];

	CModele ModeleBlister[3];
	CModele ModeleSachet[3];

	PARAM_ROI RoiModele;
	PARAM_ROI RoiSachetVertical;	//V2.57


	//ACQUISITION
	THPARAM_ACQUISITION	Param_Acquisition;

	

public:
	int m_iStatus;

	long m_lAVIYSize;
	long m_lAVIXSize;
	BOOL m_bSauveImg;
	HWND m_Handle;

	int m_iGroupeIndex;
	int m_iModeInspection;
	int m_iNumeroGroupe;
	void AfficherData();
	int		m_iMode;		//0=STOP, 1=GRAB, 2=PAS A PAS

//V2.53
	double	dPixelSizeYZoom;	//Coef pour métrage



//Parametres camera
	double	dPixelSizeX;
	double	dPixelSizeY;
	double	m_dOrigine;

//Reultats inspection
	long	lNumeroImage;
	long	lNumeroImageDernierArret;

public:
	void* m_pDiag;
};

#endif // !defined(AFX_MATROXCAMERA_H__C11C1398_9F52_4577_988E_0E04963293E9__INCLUDED_)
