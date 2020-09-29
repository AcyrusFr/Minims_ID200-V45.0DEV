#include "StdAfx.h"
#include "Defaut.h"

#include "BaseFenetre.h"
#include"AC_Base.h"

CDefaut::CDefaut(void)
{
	Position = -1;
	Type= -1;
	ID= -1;
	IDCoupe= -1;

	Code= _T("");
}

CDefaut::~CDefaut(void)//V 45.0
{
}

int CDefaut::Ajouter()
{

	if(ID == -1) return 0;//Defaut Non chargé

	//Ajout d'un defaut dans la BDD
	#pragma warning (disable: 4995)
	CString strCom = _T("");
	CString strRequete=_T("");
	
	strRequete.Format( _T("INSERT INTO DefCoupes ([IDDefaut],[IDCoupe],[Metrage],[Position],[Valeur],[CleImg]) VALUES(%d,%d,%ld,%ld,%2.3f,%ld)"),ID,IDCoupe,Metrage,Position,Valeur,CleImg); 
	try
	{
		theBase.dao_Base.Execute(strRequete);
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = "ERREUR AJOUT CRecette::Table Coupe\r\nDescription :\n";
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}


	
	return 1;
}

int CDefaut::Charger(CString strCode)
{
	#pragma warning (disable: 4995)
	CADORecordset rsDefaut (&theBase.dao_Base);

	CString strRequete = _T("SELECT * FROM Defauts WHERE [Code] = '")+ strCode + _T("'");
	try
	{
		rsDefaut.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
		
		if (rsDefaut.IsOpen())
		{
			if (rsDefaut.GetRecordCount() > 0 )	
			{

				//V 40 ID = ACBaseGetInt(_T("ID"),&rsDefaut);
				ID = ACBaseGetLong(_T("ID"), &rsDefaut);

				Libelle = ACBaseGetStr(_T("Libelle"),&rsDefaut);
				Categorie = ACBaseGetStr(_T("Categorie"),&rsDefaut);
				//Visible et Origine A VOIR
			}
		}
	
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Chargement Table Defaut\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}

	//V 45.0 correction
	if (rsDefaut.IsOpen())
	{
		rsDefaut.Close();
	}

#pragma warning (default: 4995)
	return 0;
}
