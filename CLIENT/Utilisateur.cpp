#include "StdAfx.h"
#include "Utilisateur.h"
#include "BaseFenetre.h"
#include "AC_Base.h"

CUtilisateur::CUtilisateur(void)
{
	strCode = "";
	strPassword = "";
	bIsActif = false;
	NbErrLogin = 0;
	strLibelle = "";
	IdBase = 0;

	NiveauOperateur = UTILISATEUR_NIVEAU_AUCUN;
	bMaintenance = FALSE;
	NiveauBDD = 0;

}

CUtilisateur::~CUtilisateur(void)
{
}

bool CUtilisateur::IsActif(void)
{
	return bIsActif;
}

bool CUtilisateur::Lire_Base(CString strUserCode)
{
#pragma warning (disable: 4995)
	CADORecordset rsUser (&theBase.dao_Base);
	CString strvalue = "";

	if (ACBaseStrRsOpen(strUserCode,"Code","Utilisateurs",&rsUser))	
	{
	
		strCode = ACBaseGetStr("Code",&rsUser);
		strPassword = ACBaseGetStr("Password",&rsUser);
		strLibelle = ACBaseGetStr("Libelle",&rsUser);
		strProfil = ACBaseGetStr("Profil",&rsUser);
		NiveauBDD = ACBaseGetInt("Niveau",&rsUser);
		
		int iTemp = ACBaseGetInt("Actif",&rsUser);
		iTemp == 0 ? bIsActif = true: bIsActif = false; 

		NbErrLogin = (short) ACBaseGetInt("ErrLogin",&rsUser);

		rsUser.Close();
	
	}

	//Recherche Niveau + maintenance dans table profil
	if(rsUser.IsOpen()) rsUser.Close();

	if (ACBaseStrRsOpen(strProfil,"Libelle","Profils",&rsUser))	
	{
		int iTemp = ACBaseGetInt("Maintenance",&rsUser);
		iTemp == 1 ? bMaintenance = TRUE: bMaintenance = FALSE; 
		NiveauOperateur = (short) ACBaseGetInt("Niveau",&rsUser);
		rsUser.Close();
		return true;
	}

	return false;

#pragma warning (default: 4995)
}

bool CUtilisateur::Ecrire_Base(void)
{
	#pragma warning (disable: 4995)
	CADORecordset rsUser (&theBase.dao_Base);
	CString strvalue = "";

	if (ACBaseStrRsOpen(strCode,"Code","Utilisateurs",&rsUser))	
	{
	
		rsUser.Edit();

		ACBaseSetStr(strPassword,"Password",&rsUser);
		ACBaseSetStr(strProfil,"Profil",&rsUser);
		ACBaseSetStr(strLibelle,"Libelle",&rsUser);

		int iTemp = 0;
		bIsActif == true ? iTemp = 0 : iTemp = 1; 
		ACBaseSetInt(iTemp,"Actif",&rsUser);
		ACBaseSetInt(NbErrLogin,"ErrLogin",&rsUser);

		rsUser.Update();

		rsUser.Close();
		return true;
	}
			

	return false;

#pragma warning (default: 4995)
}

int CUtilisateur::Deconnecter(void)
{
	//deconnexion de l'utilisateur en cours
//V 41 Ecrasement données BDD Utilisateurs	Ecrire_Base();

	if(strCode != CODE_UTILISATEUR_DEFAUT ) theApp.theJournal.Clore(IdBase);
	return 0;
}

bool CUtilisateur::VerifierMotDePasse(CString strPassWord)
{
	if(this->strPassword == strPassWord)
		return true;
	else
		return false;
}



bool CUtilisateur::IsManager(void)
{
	if(NiveauOperateur == UTILISATEUR_NIVEAU_MANAGER)
		return true;
	else
	return false;
}
