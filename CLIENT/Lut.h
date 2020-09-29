// Lut.h: interface for the CLut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LUT_H__DC532A2C_CDF0_4F1F_8312_4C90B0AD4846__INCLUDED_)
#define AFX_LUT_H__DC532A2C_CDF0_4F1F_8312_4C90B0AD4846__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLut  
{
public:
	long lX;
	long lType;
	long lMax;
	long lMin;
	MIL_ID milSys;
	MIL_ID milLut;

	CLut();
	virtual ~CLut();
	void Charger(MIL_ID SystemId, long SizeX, long Type);
	void Decharger();
	void Creer(int iType);
};

#endif // !defined(AFX_LUT_H__DC532A2C_CDF0_4F1F_8312_4C90B0AD4846__INCLUDED_)
