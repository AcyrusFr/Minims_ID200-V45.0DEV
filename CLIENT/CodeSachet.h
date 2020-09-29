#pragma once

#include "CodeBarreProj.h"
#include "Image.h"
#include "ImageResult.h"

class CCodeSachet
{
public:
	CCodeSachet(void);
	~CCodeSachet(void);
	void RAZ(void);

public:
	CCodeBarreProj mCodeProj;
	CString sCodeLu;
	bool bLectureOK;	//V34ALL
	int iCodeReponse;	//V34ALL
	CString sErreur;
	PARAM_ROI pmRoi_Lecture;
	CMImage roiLecture;
	CImageResult * mImgRes;
};
