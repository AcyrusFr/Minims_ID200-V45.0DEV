// MatroxCameraUV.h: interface for the CMatroxCameraUV class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_MATROXCAMERA_H__C11C1398_9F52_4577_988E_0E04963293E9__INCLUDED_)
//#define AFX_MATROXCAMERA_H__C11C1398_9F52_4577_988E_0E04963293E9__INCLUDED_

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
#include "Blobs.h"

#define NB_BLOBS_UV_MAX	1000

class CMatroxCameraUV  
{
public:
	CMatroxCameraUV();
	virtual ~CMatroxCameraUV();

	BOOL LireFromBase(CADODatabase *daoBase, CString sApplication, CString sDossier);
	BOOL Charger(MIL_ID idSystem);
	BOOL Decharger();

	void Fenetre_Ouvrir();
	void Fenetre_Fermer();
	bool IsFenetreOpened;

	void Start();
	void Stop();

	CString		sErrMsg;

	long lX;
	long lY;

	char m_sAVIFile[150];
	long m_lAVIImage;

	bool Options_Sauvegarde;
	bool Options_Overlay;
	
	CBlobs BlobUV;
	
public:
	bool bSave;
	bool bSaveOne;
	int iModeAcquisition;
	int iSnap;

	bool Simulation_Enable;
	int Simulation_IndexLoad;
	int Simulation_NbRoi;
	CMImage Simulation_Image;
	CMImage Simulation_Roi[20];


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

	CMImage imgGrab[2];
	CMImage roiSrc[2][3];
	CMImage imgSrc1;	//V2.30
	CMImage imgSrc2;	//V2.30
	CMImage imgDiff;	//V2.30
	CMImage imgSrcPlus;	//V2.51
	CMImage imgBin;
	CMImage roiBin[3];

	CMImage imgGrabVisu;

	//CMImage imgWnd;

	CImageResult	imgRes[10];
	CKernel	ker[10];
	CLut	lut[10];

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

//Parametres camera
	double	dPixelSizeX;
	double	dPixelSizeY;
	double	m_dOrigine;

//Reultats inspection
	long	lNumeroImage;
	long	*lNumeroImageMaitre;
	long	lNumeroImageMaitreRef;

public:
	void* m_pDiag;


//V23
	double Date_ImageUV;
};

//#endif // !defined(AFX_MATROXCAMERA_H__C11C1398_9F52_4577_988E_0E04963293E9__INCLUDED_)
