
#include "stdafx.h"
#include "Compteurs.h"

CCompteurs::CCompteurs(void)
{
	for(int i = 0; i < 3 ; i++)
	{
		NbFormes[i] = 0;
		NbDMXNonLu[i] = 0;
		NbDMXLu[i] = 0;

		NbRebusVision[i] = 0;
		NbRebusPoids[i] = 0;
		NbElementsBoite[i] = 0;
		NbEtuis[i] = 0;

		EncoursEtui[i] = 0;
	}

	NbBoitesFermees= 0;
	NbEtiqCollees= 0;
	NbEtiqNonVues= 0;

	NbEtuisEjectesMauvais= 0;
	NbEtuisEjectesBon= 0;
	//Robot M10
	EtuisVidesDistri= 0;
	EtuisPleinsPris= 0;

	NbMinimsEtuis= 0;//doit etre egal à Recette::NbPieceOK
	NbMinimsRebusPoids= 0;//Doit etre egal à Recette::NbRebusPoids
	NbMinimsRebusVision= 0;//Doit etre egal à Recette::NbRebusVision
	NbMinimsPrisRobot= 0;//Doit etre egal à Recette::NbPieceOK + Recette::NbPIeceNOK
	
	TotalNoRAZ= 0;
}
CCompteurs::~CCompteurs(void)
{

}

bool CCompteurs::GererCycle(int iParam)
{
	return false;
}