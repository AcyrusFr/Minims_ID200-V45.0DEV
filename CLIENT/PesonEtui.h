#pragma once
#include "peson.h"
class CPesonEtui :
	public CPeson
{
public:
	CPesonEtui(void);
	~CPesonEtui(void);

	BOOL UndefBits[8];
	BOOL * pBitFinEtalonnageOK;

	int iPoidsMoyenMinims;//en cg
	int iTareEtuiVide;//en cg
	int iTareEtuiPlein;//en cg
	BYTE iVal110_10;
	BYTE iVal110_11;

	BYTE iVal110_12;
	BYTE iVal110_13;

	BYTE iVal110_14;
	BYTE iVal110_15;

	//LastMesure
	BYTE iVal211_12;
	BYTE iVal211_13;

	//StabilisedValue
	BYTE iVal211_26;
	BYTE iVal211_27;

	void InitOF();
	static bool GererCycle(int iParam);

	int oldLastMesure;//Gestion changement de valeur
	bool GererProduction(void);
};

