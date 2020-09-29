// Lut.cpp: implementation of the CLut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseFenetre.h"
#include "Lut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLut::CLut()
{
	lX = 0;
	milLut = 0;
	lMin = 0;
	lMax = 256;
}

CLut::~CLut()
{

}

void CLut::Charger(MIL_ID SystemId, long SizeX, long Type)
{
	milSys = SystemId;
	lX = SizeX;
	lType = Type;
	MbufAlloc1d(milSys,lX,8L+M_UNSIGNED,M_LUT ,&milLut);
}

void CLut::Decharger()
{
	MbufFree(milLut);
	milLut = 0;
}

void CLut::Creer(int iType)
{
	unsigned char lTab[256];
	long i;

	if(iType == 0)
	{
		for (i=0; i<lMin; i++)
		{
			lTab[i]=(unsigned char)lMin;
		}
		for (i=lMax; i<lX; i++)
		{
			lTab[i]=255;
		}
		for (i=lMin; i<lMax; i++)
		{
			//lTab[i]= (unsigned char) ((lX-1) * (i - lMin) / (lMax - lMin));
			lTab[i]=(unsigned char) i;
		}
	}
	else
	{
		for (i=0; i<256; i++)
		{
			lTab[i]=0;
		}
		lTab[1] = 255;
		lTab[2] = 255;
		lTab[3] = 255;
	}

	MbufPut1d(milLut,0,lX,lTab);
}
