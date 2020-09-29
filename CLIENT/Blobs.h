#pragma once

#include "Image.h"

#define SIZE_CONTOUR_DEFAUT	2000

class CBlobs
{
public:
	CBlobs(void);
	~CBlobs(void);

	//Variables
	MIL_ID	*MF_System;
	MIL_ID FeatureList;
	MIL_ID BlobResult;

	//Variables Contours
	bool bTakeContour;
	long * Index;
	double * X;
	double * Y;
	long CurrentSize;
	long MaxCount;

	//Variables resultats
	double * Label;
	double * Area;
	double * Box_XMin;
	double * Box_XMax;
	double * Box_YMin;
	double * Box_YMax;
	double * NumberHoles;
	double * Centre_X;
	double * Centre_Y;
	double * Default_Value;

	//Fonctions
	bool Charger(long lMaxCount,bool bTakeContour);
	bool Decharger(void);
	int Calculer(MIL_ID Image);
	MIL_INT NumberFinded;
	void SetMaxCount(long lMaxCount);

	int GetNumber(void);
	long GetContourNbPts(MIL_INT Label);
	void GetContour(MIL_INT Label,STRUCT_POINT * Pts);
	int Selectionner(MIL_INT Operation, MIL_INT Feature, MIL_INT Condition, double CondLow, double CondHigh);

	bool GetResult(MIL_INT Result);
	void DrawBlobs(MIL_ID Image, COLORREF Color);
	void DrawHoles(MIL_ID Image, COLORREF Color);
	void DrawBlobs(MIL_INT Label,MIL_ID Image, COLORREF Color);
	void DrawHoles(MIL_INT Label,MIL_ID Image, COLORREF Color);
	void DrawContour(MIL_ID Image, COLORREF Color);
	
	void GetInteractiveResults();
};
