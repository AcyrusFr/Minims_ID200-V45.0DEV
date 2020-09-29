#include "StdAfx.h"
#include "StringReader.h"

CStringReader::CStringReader(void)
{
	for(int i = 0; i < NB_MAX_STR_READER; i++)
	{
		strLue[i] = _T("");
	}
}

CStringReader::~CStringReader(void)
{
}

bool CStringReader::Charger(void)
{
	if(SearchContext>0)	MstrFree(SearchContext);	
	SearchContext = 0;

	if(Result>0)	MstrFree(Result);	
	Result = 0;

	SearchContext = MstrRestore(strFile.GetBuffer(),*System,M_DEFAULT,M_NULL);
	strFile.ReleaseBuffer();

	MstrAllocResult(*System, M_DEFAULT, &Result);
	return true;

}

void CStringReader::PreProcess(void)
{
	long lIsToProcess = 0;
	MstrInquire(SearchContext,M_CONTEXT,M_PREPROCESSED + M_TYPE_LONG,&lIsToProcess); 

	if(lIsToProcess == 0)
	{
		MstrPreprocess(SearchContext, M_DEFAULT);
	}
	else
	{
		MstrPreprocess(SearchContext, M_RESET);//unpreprocess
		MstrPreprocess(SearchContext, M_DEFAULT);//preprocess
	}
}
void CStringReader::Decharger()
{
	if(SearchContext>0)	MstrFree(SearchContext);	
	SearchContext = 0;

	if(Result>0)	MstrFree(Result);	
	Result = 0;
}
bool CStringReader::Trouver(void)
{
	lNbRes  = 0L;

	//Recherche du modèle
	//gestion erreur matrox
	MappControl(M_ERROR,M_PRINT_DISABLE);
	MstrRead(SearchContext, mImg->milBuf, Result );
	if(MappGetError(M_CURRENT, M_NULL))
	{
		MappControl(M_ERROR,M_PRINT_ENABLE);
		return false;
	}

	MappControl(M_ERROR,M_PRINT_ENABLE);

	//Analyse du résultat
	long lIsTimeout = M_FALSE;
	MstrGetResult(Result, M_GENERAL, M_TIMEOUT_END  + M_TYPE_LONG, &lIsTimeout);
	if(lIsTimeout == M_TRUE)
		return false;

	//Analyse du résultat
	MstrGetResult(Result, M_GENERAL, M_STRING_NUMBER + M_TYPE_LONG, &lNbRes);
	if(lNbRes <= 0)
		return false;

	if(lNbRes >= NB_MAX_STR_READER)
		return false;
	
	for(int i = 0; i < NB_MAX_STR_READER; i++)
	{
		strLue[i] = _T("");
	}

	MIL_TEXT_CHAR String[60];
	for(int i = 0; i < lNbRes; i++)
	{
		MstrGetResult(Result, i, M_STRING+M_TYPE_TEXT_CHAR, String);
		strLue[i] = String;
	}
	return true;

}

void CStringReader::DrawStringContour(CMImage * Img,COLORREF Color,MIL_ID Index /*= M_ALL*/)
{
	if(lNbRes <= 0)
		return;

	MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MstrDraw(M_DEFAULT,Result, Img->milBuf,M_DRAW_STRING_CONTOUR, Index,M_NULL,M_DEFAULT );
}

bool CStringReader::ModifyParamsContext()
{
	MstrControl(SearchContext,M_CONTEXT, M_INTERACTIVE, M_DEFAULT); 
	return true;
}

bool CStringReader::SaveToFile(void)
{
	
	MstrSave( strFile.GetBuffer(),SearchContext,M_DEFAULT);
	strFile.ReleaseBuffer();
	return true;
}

void CStringReader::GetResult(MIL_INT Index,MIL_INT TypeResult,double * dVal)
{
	if(dVal == NULL) return;

	MstrGetResult(Result,Index, TypeResult + M_TYPE_DOUBLE, dVal);
}

bool CStringReader::Get_ResultPositions()
{
	double dTemp;
	MstrGetResult(Result, 0, M_STRING_BOX_UL_X+M_TYPE_DOUBLE, &dTemp);
	resultBox.X1 = (long)dTemp;
	MstrGetResult(Result, 0, M_STRING_BOX_UL_Y+M_TYPE_DOUBLE, &dTemp);
	resultBox.Y1 = (long)dTemp;
	MstrGetResult(Result, 0, M_STRING_BOX_BR_X+M_TYPE_DOUBLE, &dTemp);
	resultBox.X2 = (long)dTemp;
	MstrGetResult(Result, 0, M_STRING_BOX_BR_Y+M_TYPE_DOUBLE, &dTemp);
	resultBox.Y2 = (long)dTemp;

	resultBox.dX = resultBox.X2 - resultBox.X1;
	resultBox.dY = resultBox.Y2 - resultBox.Y1;

	resultCentre.Xpix = (resultBox.X1+resultBox.X2)/2;
	resultCentre.Ypix = (resultBox.Y1+resultBox.Y2)/2;
	return true;
}
