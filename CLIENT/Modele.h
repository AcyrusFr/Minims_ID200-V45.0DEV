#pragma once

#include "Image.h"

#define MODELE_MAXNB	800L
#define COLORTRACAGE	RGB(255,255,255)

class CModele
{
public:
	COLORREF ColorTracage;
	CModele(void);
	~CModele(void);
	bool Charger(void);
	bool Trouver(void);
	bool SetRoi(double X,double Y,double dX,double dY);
	bool CenterRoi(double X,double Y);
	bool CenterRoi(double X,double Y,bool Redim);
	bool VerifierRoi();
	bool RedimRoi();
	bool SetControl(MIL_INT ControlType,double ControlValue);
	bool SetControl(MIL_INT index,MIL_INT ControlType,double ControlValue);
	bool GetControl(MIL_INT Index,MIL_INT ControlType,long * Value);
	bool DrawBox(MIL_ID Image,MIL_INT Index = M_ALL , COLORREF Color = COLORTRACAGE);
	bool DrawTitre(MIL_ID Image, COLORREF Color = COLORTRACAGE);
	bool DrawEdges(MIL_ID Image,MIL_INT Index = M_ALL , COLORREF Color = COLORTRACAGE);
	bool DrawROI(MIL_ID Image, COLORREF Color = COLORTRACAGE);
	bool DrawPosition(MIL_ID Image,MIL_INT Index = M_ALL , COLORREF Color = COLORTRACAGE);
	bool Decharger();
	void Calibrer();

	void ModifyParamsContext();
	void SaveToFile();

	void GetInteractiveResults();

	double	Model_dX;
	double	Model_dY;
	double	Model_CentreX;
	double	Model_CentreY;

	double Model_Angle;

	double	ResImgX[MODELE_MAXNB]; 
	double	ResImgY[MODELE_MAXNB]; 
	double	ResRoiX[MODELE_MAXNB]; 
	double	ResRoiY[MODELE_MAXNB]; 
	double	ResAngle[MODELE_MAXNB]; 
	double	ResScale[MODELE_MAXNB]; 
	double	ResScore[MODELE_MAXNB]; 
	bool	IsCalculable;

	CMImage	*mImg;
	int		iChild;
	long	RoiX;
	long	RoiY;
	long	RoiXSize;
	long	RoiYSize;
	long	RoiRedimX;
	long	RoiRedimY;
	long	RoiRedimXSize;
	long	RoiRedimYSize;

	MIL_ID	*MF_System;
	MIL_ID	SearchContext;
	MIL_ID	Result;
	CString	strFile;
	long	MF_NbRes;

	MIL_TEXT_PTR	mTitre;
	bool CreateFromImage(MIL_ID Image,double OffsetX ,double OffsetY,double ModelCY ,double ModelCX);
	bool AddFromImage(MIL_ID Image,double OffsetX ,double OffsetY,double ModelCY ,double ModelCX);
	bool CreateFromDXF(CString strFichierDXF,double dPxScale = 0,MIL_ID Calibration = 0);

	long lIsTimeout;
};
