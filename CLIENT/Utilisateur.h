#pragma once

#define UTILISATEUR_NIVEAU_AUCUN	0
#define UTILISATEUR_NIVEAU_OPERATEUR	1
#define UTILISATEUR_NIVEAU_TECHNICIEN	2
#define UTILISATEUR_NIVEAU_MANAGER	3

class CUtilisateur
{
public:
	CUtilisateur(void);
	~CUtilisateur(void);
	
	CString strCode;
	CString strProfil;
	CString strLibelle;

	CString strPassword;
	bool bIsActif;
	short NbErrLogin;
	short NiveauOperateur;//V 39 int to short
	int NiveauBDD;
	BOOL bMaintenance;

	//BDD
	long IdBase;


	bool IsActif(void);
	bool Lire_Base(CString strUserCode);
	bool Ecrire_Base(void);
	int Deconnecter(void);
	bool VerifierMotDePasse(CString strPassword);
	bool IsManager(void);
};
