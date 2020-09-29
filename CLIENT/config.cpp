// config.cpp: implementation of the Cconfig class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "config.h"
#include "key.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{
}

CConfig::~CConfig()
{

}

BOOL CConfig::Lire()
{
	CKey m_Key;
	CString m_String;

	//Recherche des registres de l'application ASPECT
	m_String = "Software\\VB and VBA program settings\\CQDT\\AppVision";

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return FALSE;
		}
	}

	str_Registre.Empty();
	str_Registre = "Software\\VB and VBA program settings\\CQDT";

	str_Machine = GetStringValue("AppVision","Nom","Vision");
	str_Client = GetStringValue("AppVision","Client","Acyrus");
	m_Key.Close();

	//Mémorisation du chemin d'accès
	str_Registre.Empty();
	str_Registre = "Software\\VB and VBA program settings";
	str_Registre = str_Registre + "\\CQDT-" + str_Client + "-" + str_Machine;
	
	m_String.Empty();
	m_String = str_Registre + "\\Configuration";

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return FALSE;
		}
	}

	str_RepertoireApp = GetStringValue("Configuration","CheminApp","C:\\ACYBL_App");
	str_RepertoireData = GetStringValue("Configuration","CheminData","C:\\ACYBL_Data");
	str_RepertoireBase = GetStringValue("Configuration","CheminBase","C:\\ACYBL_Base");
	str_Provider = GetStringValue("Configuration","Provider","DSN");
	str_NomBase = GetStringValue("Configuration","Base","ACY_BAUCH;UID=sa;PWD=acyrus;");

#pragma warning (disable: 4129)
	strConnexionAView = GetStringValue("Configuration","AView","Data Source=PCBL01\sqlexpress;Initial Catalog=acyrus_bauschlomb;User Id=USER;Password=user;Persist Security Info=true;");
#pragma warning (default: 4129)

	m_Key.Close();


	return TRUE;
}

CString CConfig::GetStringValue(CString sCle, CString sNom, CString sDefault )
{
	CKey m_Key;
	CString m_String;
	CString sReponse;

	//Ouverture de l'emplacement
	m_String.Empty();
	m_String = str_Registre + "\\" + sCle;

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return "ERREUR";
		}
	}
	if (!m_Key.GetStringValue(sReponse,sNom))	
	{
		m_Key.SetStringValue(sDefault,sNom);
		sReponse = sDefault;
	}
	m_Key.Close();
	
	return sReponse;
}

long CConfig::GetLongValue(CString sCle, CString sNom, CString sDefault )
{
	CKey m_Key;
	CString m_String;
	CString sReponse;

	//Ouverture de l'emplacement
	m_String.Empty();
	m_String = str_Registre + "\\" + sCle;

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return -1;
		}
	}
	if (!m_Key.GetStringValue(sReponse,sNom))	
	{
		m_Key.SetStringValue(sDefault,sNom);
		sReponse = sDefault;
	}
	m_Key.Close();
	
#ifdef _UNICODE
	return 	(long)(_wtoi(sReponse));
#else
	return 	(long)(atoi(sReponse));
#endif
}

long CConfig::GetIntValue(CString sCle, CString sNom, CString sDefault )
{
	CKey m_Key;
	CString m_String;
	CString sReponse;

	//Ouverture de l'emplacement
	m_String.Empty();
	m_String = str_Registre + "\\" + sCle;

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return -1;
		}
	}
	if (!m_Key.GetStringValue(sReponse,sNom))	
	{
		m_Key.SetStringValue(sDefault,sNom);
		sReponse = sDefault;
	}
	m_Key.Close();
	
#ifdef _UNICODE
	return 	_wtoi(sReponse);
#else
	return 	atoi(sReponse);
#endif
}
double CConfig::GetDoubleValue(CString sCle, CString sNom, CString sDefault )
{
	CKey m_Key;
	CString m_String;
	CString sReponse;

	//Ouverture de l'emplacement
	m_String.Empty();
	m_String = str_Registre + "\\" + sCle;

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return -1;
		}
	}
	if (!m_Key.GetStringValue(sReponse,sNom))	
	{
		m_Key.SetStringValue(sDefault,sNom);
		sReponse = sDefault;
	}
	m_Key.Close();
	
#ifdef _UNICODE
	return 	_wtoi(sReponse);
#else
	return 	atoi(sReponse);
#endif
}

long CConfig::SetLongValue(CString sCle, CString sNom, long lValue )
{
	CKey m_Key;
	CString m_String;
	CString sValue;

	//Ouverture de l'emplacement
	m_String.Empty();
	m_String = str_Registre + "\\" + sCle;

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return -1;
		}
	}
#ifdef _UNICODE
	sValue.Format(L"%ld",lValue);
#else
	sValue.Format("%ld",lValue);
#endif
	m_Key.SetStringValue(sValue,sNom);
	
	return 0;
}

double CConfig::SetDoubleValue(CString sCle, CString sNom, double lValue )
{
	CKey m_Key;
	CString m_String;
	CString sValue;

	//Ouverture de l'emplacement
	m_String.Empty();
	m_String = str_Registre + "\\" + sCle;

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return -1;
		}
	}
#ifdef _UNICODE
	sValue.Format(L"%f",lValue);
#else
	sValue.Format("%f",lValue);
#endif
	
	m_Key.SetStringValue(sValue,sNom);
	
	return 0;
}
CString CConfig::SetStringValue(CString sCle, CString sNom,CString strValue )
{
	CKey m_Key;
	CString m_String;
	CString sValue;

	//Ouverture de l'emplacement
	m_String.Empty();
	m_String = str_Registre + "\\" + sCle;

	if(!m_Key.Open(HKEY_CURRENT_USER,m_String))
	{
		if(!m_Key.Create(HKEY_CURRENT_USER,m_String))
		{
			m_Key.Close();
			return "";
		}
	}

	sValue = strValue;
	
	m_Key.SetStringValue(sValue,sNom);
	
	return sValue;
}