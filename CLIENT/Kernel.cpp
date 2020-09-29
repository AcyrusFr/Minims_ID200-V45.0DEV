// Kernel.cpp: implementation of the CKernel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseFenetre.h"
#include "Kernel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKernel::CKernel()
{
	lX = 0;
	lY = 0;
	milKernel = 0;
}

CKernel::~CKernel()
{

}

void CKernel::Charger(MIL_ID SystemId, long SizeX, long SizeY)
{
	milSys = SystemId;
	lX = SizeX;
	lY = SizeY;
	Diviseur = lX*lY;

	unsigned char KernelData[400];
	for (long i=0;i<lX*lY;i++)
	{
		KernelData[i] = 1;
	}

	MbufAlloc2d(milSys,lX,lY,8L+M_UNSIGNED,M_KERNEL,&milKernel);
	MbufPut(milKernel,KernelData);
	MbufControlNeighborhood(milKernel,M_NORMALIZATION_FACTOR,Diviseur);
	//MbufControlNeighborhood(milKernel,M_OFFSET_CENTER_X,0);
	//MbufControlNeighborhood(milKernel,M_OFFSET_CENTER_Y,0);
	//MbufControlNeighborhood(milKernel,M_OVERSCAN,M_DISABLE);
}

void CKernel::Decharger()
{
	MbufFree(milKernel);
	milKernel = 0;

}
