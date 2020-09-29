// Kernel.h: interface for the CKernel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KERNEL_H__CB2A94BB_E536_419C_9F10_0BE65EA0A0D9__INCLUDED_)
#define AFX_KERNEL_H__CB2A94BB_E536_419C_9F10_0BE65EA0A0D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKernel  
{
public:
	void Decharger();
	void Charger(MIL_ID SystemId, long SizeX, long SizeY);
	long Diviseur;
	long lY;
	long lX;
	MIL_ID	milSys;
	MIL_ID	milKernel;

	CKernel();
	virtual ~CKernel();

};

#endif // !defined(AFX_KERNEL_H__CB2A94BB_E536_419C_9F10_0BE65EA0A0D9__INCLUDED_)
