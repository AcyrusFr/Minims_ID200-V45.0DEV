#include "StdAfx.h"
#include "Journal.h"
#include "BaseFenetre.h"

#include"AC_Base.h"

CJournal::CJournal(void)
{
	pBase = NULL;
	strTable = _T("Journal");
}

CJournal::~CJournal(void)
{
}

int CJournal::Ajouter(int Type, CString strLibelle, CString Commentaire)
{
	if (pBase == NULL)
	{
		return 0;
	}
	//Ouverture Evenements/alarme
	CString strCom = Commentaire;
	CString strRequete=_T("");
	
	strRequete.Format( _T("INSERT INTO %s([DateDebut],[Machine],[Utilisateur],[Type],[IDOF],[IncDebut],[Libelle],[Commentaire]) VALUES(GetDate(),'%s','%s',%d,%ld,%ld,'%s','%s')"),strTable,theConfig.str_Machine,theApp.CurrentUser.strCode,Type,theApp.m_Recette.ID,theApp.m_Recette.NbBoiteOK[1],strLibelle,strCom); 
	try
	{
		pBase->Execute(strRequete);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		sErreur = "ERREUR Ecriture Journal::Ajouter\r\nDescription :\n";
		sErreur = sErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(sErreur);
		return 0;
	}


	CADORecordset rsJournal (pBase);
	long lCleID = 0;
	strRequete.Format(_T("SELECT  MAX(ID) AS IDMax FROM %s;"),strTable);
	try
	{
		rsJournal.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
		
		if (rsJournal.IsOpen())
		{
			lCleID = ACBaseGetLong(_T("IDMax"),&rsJournal);
		}

		rsJournal.Close();
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		sErreur = _T("ERREUR Ecriture Journal::SelectIDENTITY\r\nDescription :\n");
		sErreur = sErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(sErreur);
		return 0;
	}

	return lCleID;

	return 0;
}

int CJournal::Clore(long CleID)
{
	if (pBase == NULL)
	{
		return 0;
	}

	//Fermeture Evenement/alarme

	if(CleID == 0) return 0;

	CString strRequete=_T("");
	
	strRequete.Format( _T("UPDATE %s SET DateFin = GetDate(),IncFin = %ld WHERE ID = %ld"),strTable,theApp.m_Recette.NbBoiteOK[1],CleID); 
	try
	{
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		sErreur = _T("ERREUR Ecriture Journal::Ajouter\r\nDescription :\n");
		sErreur = sErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(sErreur);
		return 0;
	}
	return 1;

}
int CJournal::Evenement(int Type, CString strLibelle, CString Commentaire,CString strUser)
{
	if (pBase == NULL)
	{
		return 0;
	}

	//V 43-ALL
	if (strLibelle.GetLength() > 50)
	{
		strLibelle = strLibelle.Left(50);
	}

	//Cloture Immédiate
	//Ouverture Evenements/alarme
	CString strCom = Commentaire;
	CString strRequete=_T("");
	
	strRequete.Format( _T("INSERT INTO %s([DateDebut],[DateFin],[Machine],[Utilisateur],[Type],[IDLot],[IDOF],[IncDebut],[IncFin],[Libelle],[Commentaire]) VALUES(GetDate(),GetDate(),'%s','%s',%d,%ld,%ld,%d,%d,'%s','%s')"),strTable,theConfig.str_Machine,strUser,Type,theApp.m_Recette.IDLot,theApp.m_Recette.ID,theApp.m_Recette.NbBoiteOK[1],theApp.m_Recette.NbEtuiOK[1],strLibelle,strCom); 
	try
	{
		pBase->Execute(strRequete);
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		sErreur = _T("ERREUR Ecriture Journal::Evenement\r\nDescription :\n");
		sErreur = sErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(sErreur);
		return 0;
	}

	return 0;
}
