#pragma once

#define CB_ORIENTATION_GD 0
#define CB_ORIENTATION_DG 1

class CCodeBarreProj  
{
 public:
	CCodeBarreProj(void);
	virtual ~CCodeBarreProj(void);
	int Extraire(long* pData,long lNbData, long lDiviseur);
	int Lire_Code2of5();
	void RechercheLargeurBarre(int iDebut, int iFin);
	void CreerMotif();
	long ExtraireMotif(int iDebut, int iFin, int iPas);
	
	CString Libelle; 

	int iOrientation;
	int iSeuilBarre;
	int iSeuilBarreNoire;
	int iSeuilBarreBlanche;

	int iNbBarre;

	char sCode[100];
	char motif[100];
	long posdebut;
	long posfin;

	bool bNbrExact;

	long pLissage[1000];
	long pDiff[1000];
	int barre[1000];
};
