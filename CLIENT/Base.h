// Base.h: interface for the CBase class.
//
//////////////////////////////////////////////////////////////////////
//desactivation des avertissements deprecated function
#pragma warning (disable: 4995)

#if !defined(AFX_BASE_H__5383B166_CD2E_40EA_8988_28657CB5CD8E__INCLUDED_)
#define AFX_BASE_H__5383B166_CD2E_40EA_8988_28657CB5CD8E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ado2.h"

class CBase  
{
public:
	CString ChoisirStrRS(CString sChamp, CString sTable);
	CString ChoisirStrRS(CString sChamp, CString sTable,CString sCritere);

// Variables externes
	CADODatabase dao_Base;

	CString sNomFichier;
	CString sRepertoire;
	CString strErreur;
	CString sProvider;

	CString sMachine;
	CString sApplication;

// Fonctions externes
	BOOL Ouvrir(CString strRepertoire,CString strNomFichier, CString sProvider);
	BOOL Fermer();

	CBase();
	virtual ~CBase();

private:
	CString sBase;
};

#endif // !defined(AFX_BASE_H__5383B166_CD2E_40EA_8988_28657CB5CD8E__INCLUDED_)
//activation des avertissements deprecated function
#pragma warning (default: 4995)