// config.h: interface for the Cconfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__6DC6D2F8_6AD3_47B6_A639_D7D545452C4C__INCLUDED_)
#define AFX_CONFIG_H__6DC6D2F8_6AD3_47B6_A639_D7D545452C4C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CConfig  
{
public:
	CString str_Registre;
	BOOL Lire();
	CString str_Client;
	CString str_Machine;

	CString str_RepertoireBase;
	CString str_RepertoireApp;
	CString str_RepertoireData;
	CString str_NomBase;
	CString str_Provider;
	CString strConnexionAView;
	
	CConfig();
	virtual ~CConfig();
	CString GetStringValue(CString sCle, CString sNom, CString sDefault );
	long GetLongValue(CString sCle, CString sNom, CString sDefault );
	long GetIntValue(CString sCle, CString sNom, CString sDefault );
	double GetDoubleValue(CString sCle, CString sNom, CString sDefault );

	long SetLongValue(CString sCle, CString sNom, long lValue );
	double SetDoubleValue(CString sCle, CString sNom, double lValue );
	CString SetStringValue(CString sCle, CString sNom,CString strValue );
};

#endif // !defined(AFX_CONFIG_H__6DC6D2F8_6AD3_47B6_A639_D7D545452C4C__INCLUDED_)
