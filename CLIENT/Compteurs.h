#pragma once
#include "ClientOPC.h"

class CCompteurs
{
public:
	CCompteurs(void);
	~CCompteurs(void);
	
	//Communication
	CClientOPC* m_OPCClient_R;// Read

	BOOL RBit1[8];
	BOOL RBit2[8];

	//Compteurs
	int NbFormes[3];
	int NbDMXNonLu[3];
	int NbDMXLu[3];

	int NbBoitesFermees;
	int NbEtiqCollees;
	int NbEtiqNonVues;

	int NbEtuisEjectesMauvais;
	int NbEtuisEjectesBon;

	//Robots
	int NbRebusVision[3];
	int NbRebusPoids[3];
	int NbElementsBoite[3];
	int NbEtuis[3];

	//Robot M10
	int EtuisVidesDistri;
	int EtuisPleinsPris;

	int NbMinimsEtuis;//doit etre egal à Recette::NbPieceOK
	int NbMinimsRebusPoids;//Doit etre egal à Recette::NbRebusPoids
	int NbMinimsRebusVision;//Doit etre egal à Recette::NbRebusVision
	int NbMinimsPrisRobot;//Doit etre egal à Recette::NbPieceOK + Recette::NbPIeceNOK
	
	int TotalNoRAZ;

	int EncoursEtui[3];

	int iNbEtuisNCPoids;
	int iNbEtuisNCEtiquette;

	static bool GererCycle(int iPAram);

};
