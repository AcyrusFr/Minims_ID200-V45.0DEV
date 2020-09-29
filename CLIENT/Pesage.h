#pragma once

#include "ClientOPC.h"

class CPesage
{
public:
	CPesage(void);
	~CPesage(void);

	//Communication 
	BOOL * pBitInhibition;
	BOOL * pBitDoTare;
	BOOL * pBitCycleEtalonEtui;

	static bool GererCycle(int iParam);

	//V32 A voir idem pesons
	int StatutLastMesure;//0->Rien; 1->pesage OK; 2-> Pesage NOK (à archiver)
	int StabilisedValue;//mg

};

