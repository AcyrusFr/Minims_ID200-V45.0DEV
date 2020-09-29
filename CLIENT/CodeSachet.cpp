#include "StdAfx.h"
#include "CodeSachet.h"

CCodeSachet::CCodeSachet(void)
{
	sCodeLu = _T("");
	sErreur = _T("");
	bLectureOK = false;	//V34ALL
	iCodeReponse = -10;	//V34ALL
}

CCodeSachet::~CCodeSachet(void)
{
}

void CCodeSachet::RAZ(void)
{
	sCodeLu = _T("NC");
	sErreur = _T("Non Définie");
	bLectureOK = false;	//V34ALL
	iCodeReponse = -10;	//V34ALL
}
