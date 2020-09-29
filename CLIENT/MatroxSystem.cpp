// MatroxSystem.cpp: implementation of the CMatroxSystem class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4995)

#include "stdafx.h"
#include "BaseFenetre.h"
#include "General.h"

#include "AC_Base.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatroxSystem::CMatroxSystem()
{
	milSys = 0;
	m_lNumCarte = 0;
	m_sDescripteur = M_SYSTEM_DEFAULT;
}

CMatroxSystem::~CMatroxSystem()
{

}
#pragma warning(disable : 4995)
BOOL CMatroxSystem::LireFromBase(CADODatabase *daoBase, CString sApplication)
{
	COleVariant var;

	CADORecordset m_Recordset (daoBase);

	CString strNumero;
	strNumero.Format("%ld",m_iNumero);

	CString strCritere=_T("");
	strCritere = "Numero = " + strNumero + " AND Application = '" + sApplication  + "'";

	if(ACBaseReqRsOpen(strCritere,"[V_Systeme]",&m_Recordset))
	{
		m_lNumCarte = ACBaseGetLong("NumCarte",&m_Recordset);
		m_sDescripteur = ACBaseGetStr("Descripteur",&m_Recordset);

		m_Recordset.Close();
		return TRUE;
	}
	else
	{
		sErrMsg = "ERREUR Systeme.Lire \nSysteme inexistant";
		return FALSE;

	}
	
}
#pragma warning(default : 4995)
BOOL CMatroxSystem::Charger()
{
	//char str[10];
	//InitializeCriticalSection(&m_cs);
	if( m_sDescripteur == "SETUP")
	{
//#ifdef MIL10
		MsysAlloc(M_SYSTEM_DEFAULT,m_lNumCarte,M_COMPLETE, &milSys);
//#else
//		MsysAlloc(M_SYSTEM_SETUP,m_lNumCarte,M_COMPLETE, &milSys);
//#endif
		//sprintf(str,"Systeme = %ld",milSys);
		//AfxMessageBox(str,MB_OK|MB_ICONSTOP);
	}
	else
	{
		MsysAlloc(M_SYSTEM_HOST,m_lNumCarte,M_COMPLETE, &milSys);
	}
	return TRUE;
}

BOOL CMatroxSystem::Decharger()
{
	if(milSys > 0)	
	{
		MsysFree(milSys);
		milSys = 0;
	}

	return TRUE;
}

#pragma warning(default : 4995)