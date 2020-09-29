// Base.cpp: implementation of the CBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4996)
#pragma warning(disable:4995)
#include "stdafx.h"
#include "Base.h"
#include "DiagListe.h"
#include "afxwin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase::CBase()
{
}

CBase::~CBase()
{
}

BOOL CBase::Ouvrir(CString strRepertoire,CString strNomFichier,CString strProvider)
{
	sNomFichier = strNomFichier;
	sRepertoire = strRepertoire;
	sProvider = strProvider;
	if (sProvider=="DSN")
		sBase = "ODBC;DSN=" + sNomFichier;	
	else if(sProvider=="SQL")
		sBase = sNomFichier;
	else if (sProvider=="MDB")
#ifdef WIN64
		sBase = _T("Provider=Microsoft Office 12.0 Access Database Engine OLE DB Provider;Data Source=") + sRepertoire + _T("\\") + sNomFichier;
#else
		sBase = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=") + sRepertoire + _T("\\") + sNomFichier;
#endif
	else sBase = sRepertoire + "\\" + sNomFichier;

	dao_Base.SetConnectionString(sBase);

	if(dao_Base.Open(_T(sBase)))
	{
		return TRUE;
	}
	else
	{
		strErreur = "ERREUR CBase::Ouvrir \nDescription :\n" + dao_Base.GetErrorDescription();
		return FALSE;
	}
	
	return TRUE;
}

BOOL CBase::Fermer()
{
	dao_Base.Close();
	return TRUE;
}

#pragma warning(disable:4995)

CString CBase::ChoisirStrRS(CString sChamp, CString sTable)
{
	CDiagListe sDialog;

	sDialog.pAdoDatabase = &dao_Base;
	sDialog.sChamp = sChamp;
	sDialog.sTable = sTable;
	sDialog.sCritere = "";
	sDialog.sResult = "";
	sDialog.DoModal();

	return sDialog.sResult;
}
CString CBase::ChoisirStrRS(CString sChamp, CString sTable,CString sCritere)
{
	CDiagListe sDialog;

	sDialog.pAdoDatabase = &dao_Base;
	sDialog.sChamp = sChamp;
	sDialog.sTable = sTable;
	sDialog.sCritere = sCritere;
	sDialog.sResult = "";
	sDialog.DoModal();

	return sDialog.sResult;
}

#pragma warning(default:4996)
#pragma warning(default:4995)