//BMx

#pragma once
#include "Image.h"
#include "MatroxCameraUeye.h"

//V 43 -ALL #define IMG_NBR_MAX 300
//V 44.8 #define IMG_NBR_MAX 1500
#define MAX_CAM_UEYE	2

#define	DEFAUT_AUCUN	0
#define	DEFAUT_DMX		1
#define	DEFAUT_TACHE	2
#define	DEFAUT_CB		3
#define	DEFAUT_TACHEUV	4


#define	DEFAUT_ETIQ_AUCUN	0
#define	DEFAUT_ETIQ_EXTRACT		1
#define	DEFAUT_ETIQ_MIXUP		2

//couleurs
#define COLOR_VERT	RGB(0,255,0)
#define COLOR_ROUGE	RGB(255,0,0)
#define COLOR_JAUNE	RGB(250,230,0)
#define COLOR_GRIS	RGB(236,233,216)
#define COLOR_ROSE	RGB(255,0,255)
#define COLOR_ORANGE	RGB(255,128,0)
#define COLOR_CYAN	RGB(0,255,255)
#define COLOR_BLEU	RGB(0,0,255)
#define COLOR_BLANC	RGB(255,255,255)
#define COLOR_NOIR	RGB(0,0,0)

//V34ALL
#define CB_NBZONES	6	//V34ALL

typedef struct _PARAM_CAMERA
{
	int  iFormat;
	int  iMode;
	int  iGain;
	int  iTempsExpo;
	long lNumSerial;
	int  iTempsExpoInit;
	int  iGainInit;
}
PARAM_CAMERA;

unsigned long MFTYPE Acquisition(void *TParam);
unsigned long MFTYPE AcquisitionUV(void *TParam);
unsigned long MFTYPE AcquisitionUeye(void *TParam);

void TracerCroix(MIL_ID Image,STRUCT_POINT * PtCentre/*Px*/,int iLargeur,COLORREF Color);
void TracerROI(CMImage * Img,PARAM_ROI	* Roi,COLORREF Color);
void TracerLigne(CMImage * Img,int iPosition, bool bHrz,COLORREF Color);

//extern CMImage * Images[IMG_NBR_MAX];
extern CMImage ImageRoiEtiquette[MAX_CAM_UEYE];

//V 43-ALL
void Vision_HistoriqueTapis_Afficher();
//V 43-ALL
void Vision_HistoriqueTapis_Exporter();

//V 44.8
bool ModeleExtraction_Creer(PARAM_ROI * RoiModele, CString strNomFichierModele);

