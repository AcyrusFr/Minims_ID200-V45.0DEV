#pragma once

#define DEFAUT_TYPE_PESONMIN	0
#define DEFAUT_TYPE_PESONMAX	1
#define DEFAUT_TYPE_VISION	2


class CDefaut
{
public:
	CDefaut(void);
	~CDefaut(void);//V 45.0

	//Variables
	long Position;
	long Metrage;
	int Type;
	int ID;
	int IDCoupe;

	double Valeur;

	long CleImg;

	CString Code;
	CString Libelle;
	CString Categorie;

	CString strErreur;

	//Fonctions
	int Ajouter();
	int Charger(CString strCode);
};
