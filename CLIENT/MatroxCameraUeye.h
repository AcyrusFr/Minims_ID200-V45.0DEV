//BMx
// MatroxCamera.h: interface for the CMatroxCameraUeye class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4996)
#pragma warning(disable:4995)

#include "Image.h"
#include "Base.h"
#include "JeuParametres.h"
#include "ueye.h"

#define COEF_TPSEXPO	1000

typedef struct _RESULT_CTRL_ETIQUETTE
{
	int iTypeDefaut;//Resultat Général

	int iTypeDefCam1;
	int iTypeDefCam2;

	long lNumImgCam1;
	long lNumImgCam2;

	CString strCodeLu;

}RESULT_CTRL_ETIQUETTE;

typedef struct _THPARAM_ACQUISITION
{
	int		iMode;
	MIL_ID	ThreadId;
}
THPARAM_ACQUISITION;

typedef struct _PARAM_CAMERA_UEYE
{
	int  iFormat;
	int  iMode;

	int iGain;
	int iExpo;

	double	Temps_Expo;
	long	Source_Trigger;
	long	Delai_Trigger;
	long	Trigger_TimeOut;
	long	Flash;
}
PARAM_CAMERA_UEYE;


class CMatroxCameraUeye  
{
public:
	CMatroxCameraUeye();
	virtual ~CMatroxCameraUeye();

	//long Diviseur;
	CString		sErrMsg;

	long lX;
	long lY;

	//variables uEye
	HIDS	m_hCam;	        // handle to camera
	HWND	m_hWndDisplay;	    // handle to diplay window
	INT		m_nColorMode;		// Y8/RGB16/RGB24/REG32
	INT		m_nBitsPerPixel;	// number of bits needed store one pixel
	INT		m_nPitch;					 // pitch of image lines
	INT		m_nBytesPerPixel;	// number of bytes needed store one pixel
	INT		m_nImageSize;		// size of image in bytes

	// memory buffers
	INT		m_lMemoryId;			// camera memory - buffer ID
	char*	m_pcImageMemory;		// camera memory - pointer to buffer
	BOOL m_bLive;
	
	bool Save;
	bool SaveOne;
	// memory needed for live display while using DIB
	SENSORINFO m_sInfo;			// sensor information struct

	//Fin variables uEye

public:
	void* m_pDiag;
	bool bSave;
	bool bSaveOne;
	int iModeAcquisition;
	int iSnap;
	bool bFromFile;

	int iIndexSeqFile;
	CStringArray StrFileSeq;

	//Entete
	BOOL		m_bIsOK;
	int			m_iNumero;
	int			m_iNumuEye;
	int			iAdresseuEye;
	CString		m_sLibelle;
	int			m_iType;			//0=Mire, 1=Fichier, 2=Camera
	CString		m_sFichier;
	CString		m_sRepertoire;
	long		m_lNumSys;
	long		m_lNumPort;

	//Paramètres camera
	PARAM_CAMERA_UEYE Param_Camera;

	//bool bChargerVision;

	//Définition des objets
	MIL_ID	milSys;

	//Images normales
	CMImage imgGrabuEye;
	//N.U CMImage imgRoiGrab;
	
	//ACQUISITION
	THPARAM_ACQUISITION	Param_Acquisition;
public:

	HWND m_Handle;

	int m_iGroupeIndex;
	int m_iModeInspection;
	int m_iNumeroGroupe;
	void AfficherData();

//Parametres camera
	double	dPixelSizeX;
	double	dPixelSizeY;
	double	m_dOrigine;

//Reultats inspection
	long	lNumeroImage;

public:

	BOOL LireFromBase(CADODatabase *daoBase, CString sApplication, CString sDossier);
	BOOL Charger(MIL_ID idSystem);
	BOOL Decharger();

	void Fenetre_Ouvrir();
	void Fenetre_Fermer();
	bool IsFenetreOpened;

	void Start();
	void Stop();

	void AppliquerParametres();
	void Snap();
	void Freerun();
	void SetTriggerSource(int iSource);
	void SetTriggerDelai(int iValµs);
	void SetTempsExpo(double dVal_ms);
	void SetTriggerTimeOut(int iVal);
	void SetFlash(int iVal);
};


