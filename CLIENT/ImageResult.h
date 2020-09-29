// ImageResult.h: interface for the CImageResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGERESULT_H__06B0DF04_CFF7_48A6_93A2_38FDD7E9F8FC__INCLUDED_)
#define AFX_IMAGERESULT_H__06B0DF04_CFF7_48A6_93A2_38FDD7E9F8FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mil.h>


class CImageResult  
{
public:
	long* lVals;
	void Decharger();
	long lType;
	MIL_ID Charger(MIL_ID SystemId, long nbEntry, long TypeResult);
	MIL_ID milSys;
	MIL_ID milResult;
	long lX;
	CImageResult();
	virtual ~CImageResult();

};

#endif // !defined(AFX_IMAGERESULT_H__06B0DF04_CFF7_48A6_93A2_38FDD7E9F8FC__INCLUDED_)
