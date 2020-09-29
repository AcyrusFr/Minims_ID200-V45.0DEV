// MatroxSystem.h: interface for the CMatroxSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATROXSYSTEM_H__752AB167_0884_4BFE_B56D_3446313388B0__INCLUDED_)
#define AFX_MATROXSYSTEM_H__752AB167_0884_4BFE_B56D_3446313388B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include"Image.h"

#pragma warning(disable:4996)
#pragma warning(disable:4995)

class CMatroxSystem  
{
public:
	int			m_iNumero;
	MIL_ID		milSys;
	long		m_lNumCarte;
	CString		m_sDescripteur;
	CString		sErrMsg;
	
	BOOL LireFromBase(CADODatabase *daoBase, CString sApplication);
	BOOL Charger();
	BOOL Decharger();
	CMatroxSystem();
	virtual ~CMatroxSystem();

};

#endif // !defined(AFX_MATROXSYSTEM_H__752AB167_0884_4BFE_B56D_3446313388B0__INCLUDED_)
