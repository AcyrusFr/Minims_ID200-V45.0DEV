#include "StdAfx.h"
#include "RecetteVision.h"
#include "BaseFenetre.h"

#include "DialVision.h"
#include "AC_Base.h"

CRecetteVision::CRecetteVision(void)
{
	
	JeuDetection = &JParamDetection;

	//Initialisation des paramétres
	JeuDetection->AjouterParametreLong("Pretraitement",0,0,1);
	JeuDetection->AjouterParametreLong("Seuil",40,0,255);
	JeuDetection->AjouterParametreLong("SeuilProj",10,0,255);

	JeuDetection->AjouterParametreLong("LongueurMax",0,0,200);
	JeuDetection->AjouterParametreLong("SurfaceMin",0,0,100000);
	JeuDetection->AjouterParametreLong("SurfaceMax",0,0,100000);

	JeuDetection->AjouterParametreLong("CodeType",0,0,2);	
	JeuDetection->AjouterParametreLong("CodeOffsetX",0,-100,100);
	JeuDetection->AjouterParametreLong("CodeOffsetY",0,-100,100);
	JeuDetection->AjouterParametreLong("CodeSizeX",0,0,100);
	JeuDetection->AjouterParametreLong("CodeSizeY",0,0,100);
	JeuDetection->AjouterParametreLong("CodeTimeOut",50,0,100);
	JeuDetection->AjouterParametreLong("CodeCellX",10,0,20);	
	JeuDetection->AjouterParametreLong("CodeCellY",10,0,20);
	JeuDetection->AjouterParametreLong("CodeDoubleTest",0,0,1);
	JeuDetection->AjouterParametreLong("CodeScoreMin",100,0,100);

	JeuDetection->AjouterParametreLong("BlobMin",0,0,100000);
	JeuDetection->AjouterParametreLong("BlobMax",100000,0,100000);

	JeuDetection->AjouterParametreLong("UV_Src1",0,0,2);	//V2.30
	JeuDetection->AjouterParametreLong("UV_Src2",2,0,2);	//V2.30
	JeuDetection->AjouterParametreLong("UV_Seuil",40,0,255);
	JeuDetection->AjouterParametreLong("UV_SurfaceMin",40,0,1000);

	JeuDetection->AjouterParametreLong("Tache_Seuil",10,0,255);
	JeuDetection->AjouterParametreLong("Tache_Seuil1",10,0,255);
	JeuDetection->AjouterParametreLong("Tache_Seuil2",10,0,255);
	JeuDetection->AjouterParametreLong("Tache_Seuil3",10,0,255);
	JeuDetection->AjouterParametreLong("Tache_SurfMin",10,0,1000);
	JeuDetection->AjouterParametreLong("Tache_Plan",2,0,2);
	JeuDetection->AjouterParametreLong("Tache_Morpho",1,0,3);

	//V2.44 : Ajout des contrôles de mesure des minims
	JeuDetection->AjouterParametreLong("LargMin",0,0,200);
	JeuDetection->AjouterParametreLong("LargMax",0,0,200);
	JeuDetection->AjouterParametreLong("LongMin",0,0,200);
	JeuDetection->AjouterParametreLong("LongMax",0,0,200);
	JeuDetection->AjouterParametreLong("RepositionMax",0,0,10);

	//V2.55 : Distance min entre les minims pour soufflage
	JeuDetection->AjouterParametreLong("DistMinSoufflage",0,0,100);
	JeuDetection->AjouterParametreLong("AngleMaxSoufflage",0,0,180);

	//#2 CB_
	JeuDetection->AjouterParametreLong("CBPositionnement",50,0,200);
	JeuDetection->AjouterParametreLong("CBLargeur",30,0,200);

	//V17 : Gestion du gain
	JeuDetection->AjouterParametreLong("CamGain",4096,0,30000);

	pLong_Pretraitement = (CParametreLong * ) JeuDetection->GetParam("Pretraitement");
	pLong_Seuil = (CParametreLong * ) JeuDetection->GetParam("Seuil");
	pLong_SeuilProj = (CParametreLong * ) JeuDetection->GetParam("SeuilProj");

	pLong_LongueurMax = (CParametreLong * ) JeuDetection->GetParam("LongueurMax");
	pLong_SurfaceMin = (CParametreLong * ) JeuDetection->GetParam("SurfaceMin");
	pLong_SurfaceMax = (CParametreLong * ) JeuDetection->GetParam("SurfaceMax");

	pLong_CodeType = (CParametreLong * ) JeuDetection->GetParam("CodeType");
	pLong_CodeOffsetX = (CParametreLong * ) JeuDetection->GetParam("CodeOffsetX");
	pLong_CodeOffsetY = (CParametreLong * ) JeuDetection->GetParam("CodeOffsetY");
	pLong_CodeSizeX = (CParametreLong * ) JeuDetection->GetParam("CodeSizeX");
	pLong_CodeSizeY = (CParametreLong * ) JeuDetection->GetParam("CodeSizeY");
	pLong_CodeTimeOut = (CParametreLong * ) JeuDetection->GetParam("CodeTimeOut");
	pLong_CodeCellX = (CParametreLong * ) JeuDetection->GetParam("CodeCellX");
	pLong_CodeCellY = (CParametreLong * ) JeuDetection->GetParam("CodeCellY");
	pLong_CodeDoubleTest = (CParametreLong * ) JeuDetection->GetParam("CodeDoubleTest");
	pLong_CodeScoreMin = (CParametreLong * ) JeuDetection->GetParam("CodeScoreMin");

	pLong_BlobMin = (CParametreLong * ) JeuDetection->GetParam("BlobMin");
	pLong_BlobMax = (CParametreLong * ) JeuDetection->GetParam("BlobMax");

	pLong_UVSrc1 = (CParametreLong * ) JeuDetection->GetParam("UV_Src1");	//V2.30
	pLong_UVSrc2 = (CParametreLong * ) JeuDetection->GetParam("UV_Src2");	//V2.30
	pLong_UVSeuil = (CParametreLong * ) JeuDetection->GetParam("UV_Seuil");
	pLong_UVSurfMin = (CParametreLong * ) JeuDetection->GetParam("UV_SurfaceMin");

	pLong_TacheSeuil = (CParametreLong * ) JeuDetection->GetParam("Tache_Seuil");
	//V2.56 : Paramétrage des taches en fonction de la couleur
	pLong_TacheSeuil1 = (CParametreLong * ) JeuDetection->GetParam("Tache_Seuil1");
	pLong_TacheSeuil2 = (CParametreLong * ) JeuDetection->GetParam("Tache_Seuil2");
	pLong_TacheSeuil3 = (CParametreLong * ) JeuDetection->GetParam("Tache_Seuil3");
	pLong_TacheSurfMin = (CParametreLong * ) JeuDetection->GetParam("Tache_SurfMin");
	pLong_TachePlan = (CParametreLong * ) JeuDetection->GetParam("Tache_Plan");
	pLong_TacheMorpho = (CParametreLong * ) JeuDetection->GetParam("Tache_Morpho");	//V2.53

	//V2.44
	pLong_LargMin = (CParametreLong * ) JeuDetection->GetParam("LargMin");
	pLong_LargMax = (CParametreLong * ) JeuDetection->GetParam("LargMax");
	pLong_LongMin = (CParametreLong * ) JeuDetection->GetParam("LongMin");
	pLong_LongMax = (CParametreLong * ) JeuDetection->GetParam("LongMax");
	pLong_RepositionnementMax = (CParametreLong * ) JeuDetection->GetParam("RepositionMax");

	//V2.55 : Distance min entre les minims pour soufflage
	pLong_DistMinSoufflage = (CParametreLong * ) JeuDetection->GetParam("DistMinSoufflage");
	pLong_AngleMaxSoufflage = (CParametreLong * ) JeuDetection->GetParam("AngleMaxSoufflage");

	//#2 CB_
	pLong_CBPositionnement = (CParametreLong * ) JeuDetection->GetParam("CBPositionnement");
	pLong_CBLargeur = (CParametreLong * ) JeuDetection->GetParam("CBLargeur");

	//V17 : Gestion du gain
	pLong_CamGain = (CParametreLong * ) JeuDetection->GetParam("CamGain");

	//Initialisation par défaut
	Code_RoiSizeXpix = 10;
	Code_RoiSizeYpix = 10;

	bResetCountDefVision = false;//V 43-ALL
}

CRecetteVision::~CRecetteVision(void)
{
}

bool CRecetteVision::Charger(int iTypeRecette)
{
	//Chargement du jeu de paramètre en fonction du type de la recette
	
	//Recuperation valeur BDD

	#pragma warning( disable: 4995)
	CADORecordset rsRecVision (&theBase.dao_Base);
#pragma warning( default: 4995)
	CString strTemp,strCritere;
	
	strTemp.Format("%d",iTypeRecette);
	strCritere = "[Type] = " + strTemp + " AND [Machine] = '" + theConfig.str_Machine  + "'";

	if(ACBaseReqRsOpen(strCritere,"[ParamVision]",&rsRecVision))
	{

		pLong_Pretraitement->value = ACBaseGetLong("Pretraitement",&rsRecVision);
		pLong_SeuilProj->value = ACBaseGetLong("SeuilProj",&rsRecVision);
		pLong_Seuil->value = ACBaseGetLong("Seuil",&rsRecVision);
		pLong_LongueurMax->value = ACBaseGetLong("LongueurMax",&rsRecVision);
		pLong_SurfaceMin->value = ACBaseGetLong("SurfaceMin",&rsRecVision);
		pLong_SurfaceMax->value = ACBaseGetLong("SurfaceMax",&rsRecVision);
		pLong_CodeType->value = ACBaseGetLong("CodeType",&rsRecVision);
		pLong_CodeOffsetX->value = ACBaseGetLong("CodeOffsetX",&rsRecVision);
		pLong_CodeOffsetY->value = ACBaseGetLong("CodeOffsetY",&rsRecVision);
		pLong_CodeSizeX->value = ACBaseGetLong("CodeSizeX",&rsRecVision);
		pLong_CodeSizeY->value = ACBaseGetLong("CodeSizeY",&rsRecVision);
		pLong_CodeTimeOut->value = ACBaseGetLong("CodeTimeOut",&rsRecVision);
		pLong_CodeCellX->value = ACBaseGetLong("CodeCellX",&rsRecVision);
		pLong_CodeCellY->value = ACBaseGetLong("CodeCellY",&rsRecVision);
		pLong_CodeDoubleTest->value = ACBaseGetLong("CodeDoubleTest",&rsRecVision);
		pLong_CodeScoreMin->value = ACBaseGetLong("CodeScoreMin",&rsRecVision);

		pLong_BlobMin->value = ACBaseGetLong("BlobMin",&rsRecVision);
		pLong_BlobMax->value = ACBaseGetLong("BlobMax",&rsRecVision);

		pLong_UVSrc1->value = ACBaseGetLong("UVSrc1",&rsRecVision);
		pLong_UVSrc2->value = ACBaseGetLong("UVSrc2",&rsRecVision);
		pLong_UVSeuil->value = ACBaseGetLong("UVSeuil",&rsRecVision);
		pLong_UVSurfMin->value = ACBaseGetLong("UVSurfMin",&rsRecVision);

		pLong_TacheSeuil->value = ACBaseGetLong("TacheSeuil",&rsRecVision);
		//V2.56 : Paramétrage des taches en fonction de la couleur
		pLong_TacheSeuil1->value = ACBaseGetLong("TacheSeuil1",&rsRecVision);
		pLong_TacheSeuil2->value = ACBaseGetLong("TacheSeuil2",&rsRecVision);
		pLong_TacheSeuil3->value = ACBaseGetLong("TacheSeuil3",&rsRecVision);
		pLong_TacheSurfMin->value = ACBaseGetLong("TacheSurfMin",&rsRecVision);
		pLong_TachePlan->value = ACBaseGetLong("TachePlan",&rsRecVision);
		pLong_TacheMorpho->value = ACBaseGetLong("TacheMorpho",&rsRecVision);	//V2.53

		pLong_LargMin->value = ACBaseGetLong("LargMin",&rsRecVision);
		pLong_LargMax->value = ACBaseGetLong("LargMax",&rsRecVision);
		pLong_LongMin->value = ACBaseGetLong("LongMin",&rsRecVision);
		pLong_LongMax->value = ACBaseGetLong("LongMax",&rsRecVision);
		pLong_RepositionnementMax->value = ACBaseGetLong("RepositionMax",&rsRecVision);

		//V2.55 : Distance Min Soufflage
		pLong_DistMinSoufflage->value = ACBaseGetLong("DistMinSoufflage",&rsRecVision);
		pLong_AngleMaxSoufflage->value = ACBaseGetLong("AngleMaxSoufflage",&rsRecVision);

		//#2 CB_
		pLong_CBPositionnement->value = ACBaseGetLong("CBPositionnement",&rsRecVision);
		pLong_CBLargeur->value = ACBaseGetLong("CBLargeur",&rsRecVision);

		//V17 : Gestion du gain
		pLong_CamGain->value = ACBaseGetLong("CamGain",&rsRecVision);

		Type = iTypeRecette;
		bChargerVision = true;
		bResetCountDefVision = false;//V 43-ALL
		rsRecVision.Close();
	}
	else
	{
		//Erreur OF inexistant
		strErreur = "Erreur Chargement RecetteVision:  Type inexistant (aucun enregistrement)";
		return false;
	}
	
	((CDialVision * )theApp.VisionView)->MAJGrilleParam();
	
	return true;
}

bool CRecetteVision::Sauvegarder(void)
{
	//Sauvegarde vers BDD

	#pragma warning( disable: 4995)
	CADORecordset rsRecVision (&theBase.dao_Base);
#pragma warning( default: 4995)
	CString strTemp,strCritere;
	
	strTemp.Format("%d",Type);
	strCritere = "[Type] = " + strTemp + " AND [Machine] = '" + theConfig.str_Machine  + "'";

	if(ACBaseReqRsOpen(strCritere,"[ParamVision]",&rsRecVision))
	{
		rsRecVision.Edit();

		ACBaseSetLong(pLong_Pretraitement->value,"Pretraitement",&rsRecVision);
		ACBaseSetLong(pLong_SeuilProj->value,"SeuilProj",&rsRecVision);
		ACBaseSetLong(pLong_Seuil->value,"Seuil",&rsRecVision);
		ACBaseSetLong(pLong_LongueurMax->value,	"LongueurMax",&rsRecVision);
		ACBaseSetLong(pLong_SurfaceMin->value,"SurfaceMin",&rsRecVision);
		ACBaseSetLong(pLong_SurfaceMax->value,"SurfaceMax",&rsRecVision);
		ACBaseSetLong(pLong_CodeType->value,"CodeType",&rsRecVision);
		ACBaseSetLong(pLong_CodeOffsetX->value,	"CodeOffsetX",&rsRecVision);
		ACBaseSetLong(pLong_CodeOffsetY->value,	"CodeOffsetY",&rsRecVision);
		ACBaseSetLong(pLong_CodeSizeX->value,"CodeSizeX",&rsRecVision);
		ACBaseSetLong(pLong_CodeSizeY->value,"CodeSizeY",&rsRecVision);
		ACBaseSetLong(pLong_CodeTimeOut->value,	"CodeTimeOut",&rsRecVision);
		ACBaseSetLong(pLong_CodeCellX->value,"CodeCellX",&rsRecVision);
		ACBaseSetLong(pLong_CodeCellY->value,"CodeCellY",&rsRecVision);
		ACBaseSetLong(pLong_CodeDoubleTest->value,"CodeDoubleTest",&rsRecVision);
		ACBaseSetLong(pLong_CodeScoreMin->value,"CodeScoreMin",&rsRecVision);

		ACBaseSetLong(pLong_BlobMin->value,"BlobMin",&rsRecVision);
		ACBaseSetLong(pLong_BlobMax->value,"BlobMax",&rsRecVision);

		ACBaseSetLong(pLong_UVSrc1->value,"UVSrc1",&rsRecVision);
		ACBaseSetLong(pLong_UVSrc2->value,"UVSrc2",&rsRecVision);
		ACBaseSetLong(pLong_UVSeuil->value,"UVSeuil",&rsRecVision);
		ACBaseSetLong(pLong_UVSurfMin->value,"UVSurfMin",&rsRecVision);

		ACBaseSetLong(pLong_TacheSeuil->value,"TacheSeuil",&rsRecVision);
		//V2.56 : Paramétrage des taches en fonction de la couleur
		ACBaseSetLong(pLong_TacheSeuil1->value,"TacheSeuil1",&rsRecVision);
		ACBaseSetLong(pLong_TacheSeuil2->value,"TacheSeuil2",&rsRecVision);
		ACBaseSetLong(pLong_TacheSeuil3->value,"TacheSeuil3",&rsRecVision);
		ACBaseSetLong(pLong_TacheSurfMin->value,"TacheSurfMin",&rsRecVision);
		ACBaseSetLong(pLong_TachePlan->value,"TachePlan",&rsRecVision);
		ACBaseSetLong(pLong_TacheMorpho->value,"TacheMorpho",&rsRecVision);	//V2.53

		//V2.44
		ACBaseSetLong(pLong_LargMin->value,"LargMin",&rsRecVision);
		ACBaseSetLong(pLong_LargMax->value,"LargMax",&rsRecVision);
		ACBaseSetLong(pLong_LongMin->value,"LongMin",&rsRecVision);
		ACBaseSetLong(pLong_LongMax->value,"LongMax",&rsRecVision);
		ACBaseSetLong(pLong_RepositionnementMax->value,"RepositionMax",&rsRecVision);

		//V2.55 : Distance Min Soufflage
		ACBaseSetLong(pLong_DistMinSoufflage->value,"DistMinSoufflage",&rsRecVision);
		ACBaseSetLong(pLong_AngleMaxSoufflage->value,"AngleMaxSoufflage",&rsRecVision);

		//#2 CB_
		ACBaseSetLong(pLong_CBPositionnement->value,"CBPositionnement",&rsRecVision);
		ACBaseSetLong(pLong_CBLargeur->value,"CBLargeur",&rsRecVision);

		//V17 : Gestion du gain
		ACBaseSetLong(pLong_CamGain->value,"CamGain",&rsRecVision);

		bChargerVision = true;
		
		rsRecVision.Update();
		rsRecVision.Close();
	}
	else
	{
		//Erreur OF inexistant
		strErreur = "Erreur Sauvegarde RecetteVision:  Type inexistant (aucun enregistrement)";
		return false;
	}

	return true;
}
