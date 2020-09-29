// ImageResult.cpp: implementation of the CImageResult class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageResult.h"
#include <malloc.h>
//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif
//
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageResult::CImageResult()
{
	milSys = 0;
	milResult = 0;
}

CImageResult::~CImageResult()
{

}

MIL_ID CImageResult::Charger(MIL_ID SystemId, long nbEntry, long TypeResult)
{
	milSys = SystemId;
	lX = nbEntry;
	lType = TypeResult;

	milResult = MimAllocResult(milSys,lX,lType,M_NULL);
	lVals = (long*) calloc(lX,sizeof(long));
	return milResult;
}

void CImageResult::Decharger()
{
	if(milResult>0)
	{
		free(lVals);
		MimFree(milResult);
	}
	milResult = 0;
}
