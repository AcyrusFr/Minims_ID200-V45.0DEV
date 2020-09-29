#pragma once
#include "JeuParametres.h"

class CRecetteVision
{
public:
	CRecetteVision(void);
	~CRecetteVision(void);

	CJeuParametres * JeuDetection;
	CString strErreur;
	int Type;//1 Blister ; 2 sachet ;
	bool bChargerVision;

	//Paramétres
	CParametreLong * pLong_Pretraitement;	//0=Aucun,1=Sobel
	CParametreLong * pLong_Seuil;			//Seuil de binarisation pour tracking
	CParametreLong * pLong_SeuilProj;		//Seuil de détection pour tracking

	CParametreLong * pLong_LongueurMax;		//Longueur max de l'objet (Pour tri des pièces qui se touchent)
	CParametreLong * pLong_SurfaceMin;		//Surface minimum de l'objet(Pour sélection face blister)
	CParametreLong * pLong_SurfaceMax;		//Surface maximum de l'objet(Pour tri des pièces qui se touchent)

	CParametreLong * pLong_CodeType;		//0=Aucun; 1=Datamatrix; 2=PharmaCode
	CParametreLong * pLong_CodeOffsetX;		//Offset X en mm
	CParametreLong * pLong_CodeOffsetY;		//Offset Y en mm
	CParametreLong * pLong_CodeSizeX;		//Largeur X en mm
	CParametreLong * pLong_CodeSizeY;		//Longueur Y en mm
	CParametreLong * pLong_CodeTimeOut;		//Temps max autorisé pour extraction (ms)
	CParametreLong * pLong_CodeCellX;		//Nombre de cellules en X
	CParametreLong * pLong_CodeCellY;		//Nombre de cellules en Y
	CParametreLong * pLong_CodeDoubleTest;	//0=NON, 1=Essayer une deuxieme position
	CParametreLong * pLong_CodeScoreMin;	//Minimum de score acceptable
	long Code_RoiSizeXpix;
	long Code_RoiSizeYpix;

	CParametreLong * pLong_BlobMin;			//Seuil min de la surface de l'objet
	CParametreLong * pLong_BlobMax;			//Seuil max de la surface de l'objet

	CParametreLong * pLong_UVSrc1;			//V2.30 Plan couleur Src1
	CParametreLong * pLong_UVSrc2;			//V2.30 Plan couleur Src2
	CParametreLong * pLong_UVSeuil;			//Seuil de binarisation pour détection de taches
	CParametreLong * pLong_UVSurfMin;		//Surface minimum des blobs

	CParametreLong * pLong_TacheSeuil;		//Seuil de binarisation pour détection de taches
	//V2.56 : Paramétrage des taches en fonction de la couleur
	CParametreLong * pLong_TacheSeuil1;		//Seuil de binarisation pour détection de taches
	CParametreLong * pLong_TacheSeuil2;		//Seuil de binarisation pour détection de taches
	CParametreLong * pLong_TacheSeuil3;		//Seuil de binarisation pour détection de taches
	CParametreLong * pLong_TacheSurfMin;	//Surface minimum des blobs
	CParametreLong * pLong_TachePlan;		//Plan couleur à soustraire
	CParametreLong * pLong_TacheMorpho;		//Erosion / Dilatation	//V2.53

	//V2.44
	CParametreLong * pLong_LargMin;			//Largeur min de l'objet
	CParametreLong * pLong_LargMax;			//Largeur max de l'objet
	CParametreLong * pLong_LongMin;			//Longueur max de l'objet
	CParametreLong * pLong_LongMax;			//Longueur min de l'objet
	CParametreLong * pLong_RepositionnementMax;			//Distance max de repositionnement

	//V2.55
	CParametreLong * pLong_DistMinSoufflage; //Distance min entre deux minims pour soufflage
	CParametreLong * pLong_AngleMaxSoufflage; //Distance min entre deux minims pour soufflage

	//#2 CB_ gestion code barre
	CParametreLong * pLong_CBPositionnement;
	CParametreLong * pLong_CBLargeur;

	//V17 : Gestion du gain par recette
	CParametreLong * pLong_CamGain;

	//V 43-ALL
	bool bResetCountDefVision;

	bool Charger(int iTypeRecette);
	bool Sauvegarder(void);
};
