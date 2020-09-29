#include "StdAfx.h"
#include "Piste.h"

CPiste::CPiste(void)
{
	XMin = 0;
	XMax = 0;
	XMinZoom = 0;
	XMaxZoom = 0;

	IndexEcriture = 0;
	IndexEcritureReel = 0;
	TrackEnCours = false;
	AnalyserMinim = false;
	AfficherDefaut = false;	//V30

	NumeroImageDebut[0] = 0;
	NumeroImageDebut[1] = 0;

	//V2.44
	LargeurMinim = 0.0;
	LongueurMinim = 0.0;
	LongueurRepositionnement = 0.0;

	//V2.53
	MetrageDebut = 0;
	MetrageFin = 0;

	//V2.55
	DistanceDepuisDernier = 1000;

	//V23
	for(int i=0;i<TABTACHE;i++)
	{
		MetrageTache[i].MetrageDebut = 0;
		MetrageTache[i].MetrageFin = 0;
		MetrageTache[i].XpixDebut = 0;
		MetrageTache[i].XpixFin = 0;
	}
}

CPiste::~CPiste(void)
{


}
