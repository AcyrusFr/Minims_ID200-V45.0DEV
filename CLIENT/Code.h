#pragma once

#include "Image.h"

#define CODE_MAXNB	40

class CCode
{
public:
	COLORREF ColorTracage;
	CCode(void);
	~CCode(void);
	bool Charger(void);
	void Decharger(void);
	bool ChargerFromFile(void);
	bool Trouver(void);
	bool SetRoi(double X,double Y,double dX,double dY);
	bool CenterRoi(double X,double Y);
	bool CenterRoi(double X,double Y,bool Redim);
	bool VerifierRoi();
	bool RedimRoi();
	bool SetControl(long ControlType,double ControlValue);
	bool DrawBox();
	bool DrawTitre();
	bool DrawEdges();
	bool DrawROI();
	bool DrawPosition();

	bool DrawBox(MIL_ID Image);
	bool DrawEdges(MIL_ID Image);
	bool DrawPosition(MIL_ID Image);

	bool ModifyParamsCode();
	bool SaveToFile(void);

	bool Get_ResultPositions();

	bool bResult;
	double	Code_dX;
	double	Code_dY;
	double	Code_PosX;
	double	Code_PosY;
	int		Type;					//0=Aucun; 1=Datamatrix; 2=PharmaCode

	double	ResImgX[CODE_MAXNB]; 
	double	ResImgY[CODE_MAXNB]; 
	double	ResRoiX[CODE_MAXNB]; 
	double	ResRoiY[CODE_MAXNB]; 
	double	ResAngle[CODE_MAXNB]; 
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
	//MIL_ID	Result;
	MIL_ID	Context;
	MIL_ID	CodeResult;
	MIL_INT BarcodeStatus;
	double Score;
	long NbErreurs;
	MIL_TEXT_CHAR BarcodeString[60];


	CString	MF_File;
	long	MF_NbRes;

	MIL_TEXT_PTR	mTitre;

	PARAM_ROI resultBox;
	STRUCT_POINT resultCentre;
};
