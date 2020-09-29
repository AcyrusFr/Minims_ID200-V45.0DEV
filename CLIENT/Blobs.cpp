//V44.5 :	Mise à jour des fonctions avec MIL_COMPILE_VERSION_HEX == 0x1020
//			Suppression des anciens commentaires

#include "StdAfx.h"
#include "Blobs.h"

CBlobs::CBlobs(void)
{
	NumberFinded = 0;
	MF_System = M_NULL;
	FeatureList = M_NULL;
	BlobResult = M_NULL;

	Index = NULL;
	X = NULL;
	Y = NULL;
	Label = NULL;
	Area = NULL;
	Default_Value = NULL;
	Box_XMin = NULL;
	Box_XMax = NULL;
	Box_YMin = NULL;
	Box_YMax = NULL;
	NumberHoles = NULL;
	Centre_X = NULL;
	Centre_Y = NULL;
	CurrentSize = 0;
	MaxCount = 10 ;

	bTakeContour = false;
}

CBlobs::~CBlobs(void)
{
}

bool CBlobs::Charger(long lMaxCount,bool bTake_Contour)
{
	bTakeContour = bTake_Contour;
	MaxCount = lMaxCount;

	Decharger();
	//V44.5 
	MblobAlloc(*MF_System, M_DEFAULT, M_DEFAULT, &FeatureList);
	if (bTakeContour)
	{
		MblobControl(FeatureList, M_ALL_FEATURES, M_ENABLE);
	}
	else
	{
		MblobControl(FeatureList, M_ALL_FEATURES, M_ENABLE);
		MblobControl(FeatureList, M_CHAINS, M_DISABLE);
	}
	MblobAllocResult(*MF_System, M_DEFAULT, M_DEFAULT, &BlobResult);

	if(bTakeContour)
	{
		MblobControl(BlobResult,M_SAVE_RUNS,M_ENABLE);
		Index = (long*) calloc(SIZE_CONTOUR_DEFAUT * lMaxCount,sizeof(long));
		X = (double*) calloc(SIZE_CONTOUR_DEFAUT * lMaxCount,sizeof(double));
		Y = (double*) calloc(SIZE_CONTOUR_DEFAUT * lMaxCount,sizeof(double));
	}

	//Allocations resultats par défaut
	Label = (double*) calloc(lMaxCount,sizeof(double));
	Area = (double*) calloc(lMaxCount,sizeof(double));
	Default_Value = (double*) calloc(lMaxCount,sizeof(double));

	Box_XMin = (double*) calloc(lMaxCount,sizeof(double));
	Box_XMax = (double*) calloc(lMaxCount,sizeof(double));
	Box_YMin = (double*) calloc(lMaxCount,sizeof(double));
	Box_YMax = (double*) calloc(lMaxCount,sizeof(double));

	NumberHoles = (double*) calloc(lMaxCount,sizeof(double));

	Centre_X = (double*) calloc(lMaxCount,sizeof(double));
	Centre_Y = (double*) calloc(lMaxCount,sizeof(double));

	return true;
}

bool CBlobs::Decharger(void)
{
	
	if(FeatureList > 0) MblobFree( FeatureList); 
	if(BlobResult > 0) MblobFree( BlobResult); 

	if(bTakeContour)
	{	
		free(Index);
		free(X);
		free(Y);
	}

	if(Label != NULL) free(Label);
	if(Area != NULL) free(Area);
	if(Default_Value != NULL) free(Default_Value);
	if(Box_XMin != NULL) free(Box_XMin);
	if(Box_XMax != NULL) free(Box_XMax);
	if(Box_YMin != NULL) free(Box_YMin);
	if(Box_YMax != NULL) free(Box_YMax);
	if(NumberHoles != NULL) free(NumberHoles);
	if(Centre_X != NULL) free(Centre_X);
	if(Centre_Y != NULL) free(Centre_Y);

	return true;
}
int CBlobs::GetNumber(void)
{
	MblobGetResult(BlobResult, M_NUMBER + M_TYPE_MIL_INT, &NumberFinded);
	return (int) NumberFinded;
}

int CBlobs::Calculer(MIL_ID Image)
{
	MblobCalculate(FeatureList,Image, M_NULL, BlobResult);
	GetNumber();
	return (int) NumberFinded;
}
int CBlobs::Selectionner(MIL_INT Operation, MIL_INT Feature, MIL_INT Condition, double CondLow, double CondHigh)
{
	MblobSelect(BlobResult,Operation,Feature,Condition,CondLow,CondHigh);
	GetNumber();
	return (int) NumberFinded;
}

bool CBlobs::GetResult(MIL_INT Result)
{
	if((NumberFinded < 0 )|| (NumberFinded > MaxCount ))
		return false;

	double * pResults = NULL;
	switch(Result)
	{
	case M_AREA:
		pResults = Area;
		break;

	case M_BOX_X_MAX:
		pResults = Box_XMax;
		break;

	case M_BOX_X_MIN:
		pResults = Box_XMin;
		break;

	case  M_BOX_Y_MAX:
		pResults = Box_YMax;
		break;

	case  M_BOX_Y_MIN:
		pResults = Box_YMin;
		break;

	case M_NUMBER_OF_HOLES :
		pResults = NumberHoles;
		break;

	case M_LABEL_VALUE :
		pResults = Label;
		break;	

	case M_CENTER_OF_GRAVITY_X :
		pResults = Centre_X;
		break;

	case M_CENTER_OF_GRAVITY_Y :
		pResults = Centre_Y;
		break;	

	default:
		//Aire par défaut
		pResults = Default_Value;
		break;

	}

	MblobGetResult(BlobResult, Result + M_TYPE_MIL_DOUBLE ,pResults);
	
	return true;
}

void CBlobs::GetInteractiveResults()
{
	MblobGetResult(BlobResult,  M_INTERACTIVE, M_NULL);
}

void CBlobs::DrawContour(MIL_ID Image, COLORREF Color)
{
	MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MblobDraw(M_DEFAULT,BlobResult, Image,M_DRAW_BLOBS_CONTOUR, M_INCLUDED_BLOBS , M_DEFAULT);
}
void CBlobs::DrawBlobs(MIL_ID Image, COLORREF Color)
{
	MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MblobDraw(M_DEFAULT,BlobResult, Image,M_DRAW_BLOBS, M_INCLUDED_BLOBS , M_DEFAULT);
}
void CBlobs::DrawHoles(MIL_ID Image, COLORREF Color)
{
	MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MblobDraw(M_DEFAULT,BlobResult, Image,M_DRAW_HOLES, M_INCLUDED_BLOBS , M_DEFAULT);
}
void CBlobs::DrawBlobs(MIL_INT Label,MIL_ID Image, COLORREF Color)
{
	MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MblobDraw(M_DEFAULT,BlobResult, Image,M_DRAW_BLOBS, Label , M_DEFAULT);
}
void CBlobs::DrawHoles(MIL_INT Label,MIL_ID Image, COLORREF Color)
{
	MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MblobDraw(M_DEFAULT,BlobResult, Image,M_DRAW_HOLES, Label , M_DEFAULT);
}

long CBlobs::GetContourNbPts(MIL_INT Label)
{
	if(!bTakeContour)
		return 0;

	long lChainedPx = 0;
	long lNbPtsContour = 0;
	MblobGetResultSingle(BlobResult,Label,M_NUMBER_OF_CHAINED_PIXELS + M_TYPE_LONG ,&lChainedPx);
	
	if((lChainedPx > SIZE_CONTOUR_DEFAUT)||(CurrentSize < lChainedPx))
	{
		free(Index);
		free(X);
		free(Y);

		Index = (long*) calloc(lChainedPx,sizeof(long));
		X = (double*) calloc(lChainedPx,sizeof(double));
		Y = (double*) calloc(lChainedPx,sizeof(double));

		CurrentSize = lChainedPx;
	}

	MblobGetResultSingle(BlobResult,Label,M_CHAIN_INDEX + M_TYPE_LONG ,Index);

	for(int i = 0; i < lChainedPx;i ++)
	{
		if(Index[i] > 1)
		{
			lNbPtsContour = i;
			break;
		}
		else
		{
			lNbPtsContour = i;
		}
	}

	return lNbPtsContour;
}

void CBlobs::GetContour(MIL_INT Label,STRUCT_POINT * Pts)
{
	if(!bTakeContour)
		return ;

	/*AVERTISSEMENT DOC MIL
	Results are only available in pixel units.*/

	if(Pts == NULL) return;

	long lChainedPx = 0;
	long lNbPtsContour = 0;

	MblobGetResultSingle(BlobResult,Label,M_NUMBER_OF_CHAINED_PIXELS + M_TYPE_LONG ,&lChainedPx);

	//reallocation si on depasse la taille par défaut
	if((lChainedPx > SIZE_CONTOUR_DEFAUT)||(CurrentSize < lChainedPx))
	{
		free(Index);
		free(X);
		free(Y);

		Index = (long*) calloc(lChainedPx,sizeof(long));
		X = (double*) calloc(lChainedPx,sizeof(double));
		Y = (double*) calloc(lChainedPx,sizeof(double));
		
		CurrentSize = lChainedPx;

	}

	MblobGetResultSingle(BlobResult,Label,M_CHAIN_INDEX + M_TYPE_LONG ,Index);
	MblobGetResultSingle(BlobResult,Label, M_CHAIN_X + M_TYPE_DOUBLE ,X);
	MblobGetResultSingle(BlobResult,Label, M_CHAIN_Y + M_TYPE_DOUBLE ,Y);
	
	//Contour du Blob uniquement
	for(int i = 0; i < lChainedPx;i ++)
	{
		if(Index[i] > 1)
		{
			lNbPtsContour = i;
			break;
		}
		else
		{
			lNbPtsContour = i;
		}

		Pts[i].Xpix = X[i];
		Pts[i].Ypix = Y[i];
	}

}
void CBlobs::SetMaxCount(long lMaxCount)
{
	MaxCount = lMaxCount;

	if(bTakeContour)
	{	
		free(Index);
		free(X);
		free(Y);
	}
	if(Label != NULL) free(Label);
	if(Area != NULL) free(Area);
	if(Default_Value != NULL) free(Default_Value);
	if(Box_XMin != NULL) free(Box_XMin);
	if(Box_XMax != NULL) free(Box_XMax);
	if(Box_YMin != NULL) free(Box_YMin);
	if(Box_YMax != NULL) free(Box_YMax);
	if(NumberHoles != NULL) free(NumberHoles);
	if(Centre_X != NULL) free(Centre_X);
	if(Centre_Y != NULL) free(Centre_Y);

	if(bTakeContour)
	{		
		Index = (long*) calloc(SIZE_CONTOUR_DEFAUT * lMaxCount,sizeof(long));
		X = (double*) calloc(SIZE_CONTOUR_DEFAUT * lMaxCount,sizeof(double));
		Y = (double*) calloc(SIZE_CONTOUR_DEFAUT * lMaxCount,sizeof(double));
	}

	//Allocations resultats par défaut
	Label = (double*) calloc(lMaxCount,sizeof(double));
	Area = (double*) calloc(lMaxCount,sizeof(double));
	Default_Value = (double*) calloc(lMaxCount,sizeof(double));

	Box_XMin = (double*) calloc(lMaxCount,sizeof(double));
	Box_XMax = (double*) calloc(lMaxCount,sizeof(double));
	Box_YMin = (double*) calloc(lMaxCount,sizeof(double));
	Box_YMax = (double*) calloc(lMaxCount,sizeof(double));

	NumberHoles = (double*) calloc(lMaxCount,sizeof(double));
	Centre_X = (double*) calloc(lMaxCount,sizeof(double));
	Centre_Y = (double*) calloc(lMaxCount,sizeof(double));

}