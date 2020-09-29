#pragma once
#include "Image.h"
#define NB_MAX_STR_READER	10

class CStringReader
{
public:
	CStringReader(void);
	~CStringReader(void);

	MIL_ID	*System;
	MIL_ID	SearchContext;
	MIL_ID	Result;
	CString	strFile;
	long	lNbRes;
	CMImage	*mImg;
	CString strLue[NB_MAX_STR_READER];

	bool Charger(void);
	void Decharger();
	bool Trouver(void);
	void PreProcess(void);
	void DrawStringContour(CMImage * Img,COLORREF Color,MIL_ID Index = M_ALL);

	bool ModifyParamsContext();
	bool SaveToFile(void);
	void GetResult(MIL_INT Index,MIL_INT TypeResult,double * dVal);

	bool Get_ResultPositions();
	PARAM_ROI resultBox;
	STRUCT_POINT resultCentre;
};
