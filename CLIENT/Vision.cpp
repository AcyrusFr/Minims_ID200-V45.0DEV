//BMx
// Vision.cpp : implementation file
//
/*

V2.40
Lecture Datamatrix en premier

V2.41
Contrôle de temps seulement sur TempsCalcul pour les Pulses Robots
Inhibition des contrôles sur les deux premières images après démarrage tapis (Cause erreur tache UV)

V2.42
Ajout de la fonction vidage2 (Vidage de la distribution)
	=>Ajout d'un bouton dans le menu DialMachine

V2.43
Reprise complète du chargement CSV : voir "CDialChargement::CSV_Charger(void)"

V2.44
Vérification de la taille du minimu après rotation
Vérification du recentrage (Ajout d'une limite)
Controle Instance Unique au démarrage
Ajout Etat chargement Fichier CSV dans DialChargement
RAZ bit recette chargée à l'initialisation pour RAZ compteurs
suppression champ compteurs dans Recette::Ajouter => TESTE OK

V2.50
DialogBox de fin d'OF
Confirmation de solde d'OF par mot de passe
TIMER de mise à jour des encours machine
Mise à jour de l'utilisateur de fin de coupe
Enregistrement des images couleur pour les défauts tache
Enregistrement des images taches UV
Mise a jout table EnCoursMachine en cas d'annulation InitialisationOF ou PesonsDébut
Vérification de la valididté de l'OF en début de production

V2.51
Enregistrement des images couleur pour les défauts ModèlesNC
Enregistrement des images couleur pour les défauts Datamatrix
Nettoyage des fonctions inutiles
TachesUV : Détection avec les plans suivants RED+GREEN-BLUE
Taches : Diminution de la ROI de detection après convolution pour ne pas détecter les abhérations sur les bords

V2.52
Gestion des compteurs pour OPC
Ajout des DB302.20 à DB302.28

V2.53
Ajout Morphologie pour les taches + Champ TacheMorpho dans ParamVision
Ajout de la possibilité de lancer le mode VIDAGE2 Hors Production
CRecette::SavePoidsEtalonFin, Remplacement PoidsEtalonSaved->PoidsEtalonSavedFin

V2.54
Ajout des DB302.28.5 à DB302.28.7
Ajout des DB302.29.0 à DB302.29.7
Base : Modification Alarme.Cle=225 : Code 212 à 222 car doublon de code
	   Ajout des code 223 à 233

V2.55
Vision.cpp Limitation de la distance entre les Minims pour soufflage
Vision.cpp Limitation de l'angle de soufflage
CBaseFenetreApp::SetEncoursMachine() : (FASTEC) Envoi seulement des valeurs en cours, sans les valeurs d'origine

V2.56
Ajout des seuils spécifiques par couleur

V2.57
Correction Pb mémoire (Classe CImageButtonWith26Style)

AFAIRE
Enregistrement des images UV par ROI
Gestion affichage en différé
Limiter les alarmes si dépassement de tableau. Actuellement s'il y a plus de lignes dans la base que la taille des tableaux la machine plante

AVIEW
Mettre les données dans l'ordre dans les rapports
*/


#pragma warning(disable:4996)
#include "stdafx.h"

#include "BaseFenetre.h"
#include "General.h"
#include "AC_Base.h"
#include "Image.h"
#include "ImageResult.h"
#include "DialCameras.h"
#include "DialMachine.h"
#include "DialEtiquette.h"
#include "Modele.h"
#include "Code.h"
#include "CodePiste.h"
#include "Math.h"
#include "AC_Util.h"
#include <sys/timeb.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//V 44.8#define USEMIL		1

//V 44.8 CMImage * Images[IMG_NBR_MAX] = {0};
CMImage ImageRoiEtiquette[MAX_CAM_UEYE];

double CalculerDistanceMM(STRUCT_POINT *PT1, STRUCT_POINT *PT2)
{
	return sqrt(pow(PT1->Xmm-PT2->Xmm,2) + pow(PT1->Ymm-PT2->Ymm,2));
}
double CalculerDistancePix(STRUCT_POINT *PT1, STRUCT_POINT *PT2)
{
	return sqrt(pow(PT1->Xpix-PT2->Xpix,2) + pow(PT1->Ypix-PT2->Ypix,2));
}


void CalculerRotation(STRUCT_POINT *Src, STRUCT_POINT *Dst,double dAngle)
{
	//Position du point après rotation de l'outil
	//x = x'cos(a) - y'sin(a)
	//y = x'sin(a) + y'cos(a)
	Dst->Xmm = Src->Xmm*cos(DegToRad(dAngle)) - Src->Ymm*sin(DegToRad(dAngle));
	Dst->Ymm = Src->Xmm*sin(DegToRad(dAngle)) + Src->Ymm*cos(DegToRad(dAngle));
}

int TacheCouleur_Extraire(CMatroxCamera *Cam, CPiste *pPiste, int iPiste)
{
	//V2.10 : Recherche des taches couleur
	//Valeurs retournées :	0 = Pas de tache
	//						1 = Présence tache
	//						2 = Erreur analyse

	PARAM_ROI RoiTemp;
	MIL_INT TotalBlobsCouleur = 0;
	CString StringTemp;
	CString strNomFichierImg;
	char sFichier[100];

	if(m_RecetteVision.pLong_TacheSeuil->value == 0)	return 0;

	//Positionnement de la zone de recherche
	RoiTemp.X1 = (long)(Cam->RoiModele.X1 * Cam->dZoom);
	RoiTemp.X2 = (long)(Cam->RoiModele.X2 * Cam->dZoom);
	RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
	RoiTemp.Y1 = (long)(Cam->RoiModele.Y1 * Cam->dZoom);
	RoiTemp.Y2 = (long)(Cam->RoiModele.Y2 * Cam->dZoom);
	RoiTemp.dY = RoiTemp.Y2 - RoiTemp.Y1;

	if(!pPiste->roiTacheSrc.SetROI(&RoiTemp))
	{
		//Erreur de positionnement
		StringTemp.Format("Robot %d : Erreur TacheSRC",iPiste);
		theApp.SetStringListVision(StringTemp);
		return 2;
	}
	
	if(!pPiste->roiTacheDst.SetROI(&RoiTemp))
	{
		//Erreur de positionnement
		StringTemp.Format("Robot %d : Erreur TacheDST",iPiste);
		theApp.SetStringListVision(StringTemp);
		return 2;
	}

	MimArith(pPiste->roiTacheSrc.milBuf,pPiste->roiTacheDst.milBuf,pPiste->roiTacheDst.milBuf,M_SUB+M_SATURATION);
	//V2.53 : Annulation convolution, remplacement par Morphologie
	//MimConvolve(pPiste->roiTacheDst.milBuf,pPiste->roiTacheDst.milBuf,M_SMOOTH);
	//V2.53 : Ajout morphologie
	if(m_RecetteVision.pLong_TacheMorpho->value > 0 )
	{
		MimErode(pPiste->roiTacheDst.milBuf,pPiste->roiTacheDst.milBuf,m_RecetteVision.pLong_TacheMorpho->value,M_GRAYSCALE);
		MimDilate(pPiste->roiTacheDst.milBuf,pPiste->roiTacheDst.milBuf,m_RecetteVision.pLong_TacheMorpho->value,M_GRAYSCALE);
	}

	//V2.51 : Diminution de la zone pour inhiber les pertes de convolution
	RoiTemp.X1 = RoiTemp.X1 + 5;
	RoiTemp.X2 = RoiTemp.X2 - 5;
	RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
	RoiTemp.Y1 = RoiTemp.Y1 + 5;
	RoiTemp.Y2 = RoiTemp.Y2 - 5;
	RoiTemp.dY = RoiTemp.Y2 - RoiTemp.Y1;
	if(!pPiste->roiTacheDst.SetROI(&RoiTemp))
	{
		//Erreur de positionnement
		StringTemp.Format("Robot %d : Erreur TacheDST",iPiste);
		theApp.SetStringListVision(StringTemp);
		return 2;
	}

	if(m_RecetteVision.pLong_TacheSeuil->value > 0)
		MimBinarize(pPiste->roiTacheDst.milBuf,pPiste->roiTacheDst.milBuf,M_GREATER,m_RecetteVision.pLong_TacheSeuil->value,M_NULL);

	Cam->CouleurBlob.Calculer(pPiste->roiTacheDst.milBuf); 
	TotalBlobsCouleur = Cam->CouleurBlob.Selectionner(M_EXCLUDE, M_AREA, M_LESS_OR_EQUAL, m_RecetteVision.pLong_TacheSurfMin->value, M_NULL); 

	if(TotalBlobsCouleur > 0)
	{
		//Présence Tache
		if(pParamVisionSauvegarde->value == 1)
		{
			strNomFichierImg.Format("%s\\TacheCouleur %d_%ld.bmp",theApp.m_Recette.RepertoireImages,iPiste,Cam->lNumeroImage);
			sprintf(sFichier,strNomFichierImg);
			if(pPiste->roiSaveToFile.SetROI(&RoiTemp))
			{
				MbufExport(sFichier,M_BMP,pPiste->roiSaveToFile.milBuf);
			}
		}
		return 1;
	}
	return 0;
}

int RepositionnerCentre(CMatroxCamera *Cam, CPiste *pPiste, int iPiste, CModele *Modele)
{
	double dAngle=0.;
	PARAM_ROI RoiTemp;
	MIL_INT TotalBlobsRotation = 0;
	int iBlobSelectionRotation = 0;
	int iBlob=0;
	double dSurfTemp=0;
	CString StringTemp;
	STRUCT_POINT PtBlobOrigine;
	STRUCT_POINT PtBlobCentre;
	STRUCT_POINT PtBlobDestination;
	STRUCT_POINT PtHG;
	STRUCT_POINT PtBD;

	/*dAngle = pPiste->PtModele.Zpix;*/
	dAngle = pPiste->PtModele.Tmm;
	if((dAngle>0) && (dAngle< 90))	
	{
		dAngle = 90 - dAngle;
	}
	else
	{
		if((dAngle> 90) && (dAngle< 180))
		{
			dAngle = 90 - dAngle;
		}
		else
		{
			if((dAngle> 180) && (dAngle< 270))
			{
				dAngle = 270 - dAngle;
			}
			else
			{
				dAngle = 270 - dAngle;
			}
		}
	}
	MbufClear(Cam->imgRotationZoom.milBuf,0x00);
	
	RoiTemp.X1 = Cam->imgRotationZoom.Roi.dX/2 - (long)Modele->ResImgX[0];
	RoiTemp.dX = Cam->RoiModele.dX;
	RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
	RoiTemp.Y1 = Cam->imgRotationZoom.Roi.dY/2 - (long)Modele->ResImgY[0];
	RoiTemp.dY = Cam->RoiModele.dY;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;

	//Copie de la zone au centre de l'image
	if(!Cam->roiRotation.SetROI(&RoiTemp))
	{
		//Erreur de positionnement
		return 3;
	}
	MbufCopy(pPiste->roiModele.milBuf,Cam->roiRotation.milBuf);
	MimRotate(Cam->imgRotationZoom.milBuf,Cam->imgRotationZoom.milBuf,dAngle,150,150,150,150,M_DEFAULT);
	
	Cam->RotationBlob.Calculer(Cam->imgRotationZoom.milBuf); 
	TotalBlobsRotation = Cam->RotationBlob.Selectionner(M_EXCLUDE, M_AREA, M_LESS_OR_EQUAL, 500, M_NULL); 

	if((TotalBlobsRotation>0) && (TotalBlobsRotation<10))
	{
		Cam->RotationBlob.GetResult(M_AREA);
		Cam->RotationBlob.GetResult(M_BOX_X_MIN);
		Cam->RotationBlob.GetResult(M_BOX_X_MAX);
		Cam->RotationBlob.GetResult(M_BOX_Y_MIN);
		Cam->RotationBlob.GetResult(M_BOX_Y_MAX);

		dSurfTemp = 0;
		for(iBlob=0; iBlob < TotalBlobsRotation; iBlob++)
		{
			if(Cam->RotationBlob.Area[iBlob] > dSurfTemp)
			{
				dSurfTemp = Cam->RotationBlob.Area[iBlob];
				iBlobSelectionRotation = iBlob;
			}
		}

		PtBlobOrigine.Xpix = Cam->imgRotationZoom.Roi.dX/2;
		PtBlobOrigine.Ypix = Cam->imgRotationZoom.Roi.dY/2;
		pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&PtBlobOrigine);

		PtBlobCentre.Xpix = (Cam->RotationBlob.Box_XMax[iBlobSelectionRotation] + Cam->RotationBlob.Box_XMin[iBlobSelectionRotation])/2.0;
		PtBlobCentre.Ypix = (Cam->RotationBlob.Box_YMax[iBlobSelectionRotation] + Cam->RotationBlob.Box_YMin[iBlobSelectionRotation])/2.0;
		pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&PtBlobCentre);

		PtBlobCentre.Xmm = PtBlobCentre.Xmm - PtBlobOrigine.Xmm;
		PtBlobCentre.Ymm = PtBlobCentre.Ymm - PtBlobOrigine.Ymm;

		CalculerRotation(&PtBlobCentre,&PtBlobDestination,-dAngle);

		//V2.44 : Contrôle des dimensions
		//Angle HautGauche
		PtHG.Xpix = Cam->RotationBlob.Box_XMin[iBlobSelectionRotation];
		PtHG.Ypix = Cam->RotationBlob.Box_YMin[iBlobSelectionRotation];
		//Angle BasDroite
		PtBD.Xpix = Cam->RotationBlob.Box_XMax[iBlobSelectionRotation];
		PtBD.Ypix = Cam->RotationBlob.Box_YMax[iBlobSelectionRotation];
		//Contrôle largeur et hauteur
		pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&PtHG);
		pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&PtBD);

		pPiste->LargeurMinim = abs(PtBD.Ymm - PtHG.Ymm);
		pPiste->LongueurMinim = abs(PtBD.Xmm - PtHG.Xmm);
		pPiste->LongueurRepositionnement = Dist_PointPoint(0.,0.,PtBlobCentre.Xmm,PtBlobCentre.Ymm);

		if(m_RecetteVision.pLong_RepositionnementMax->value > 0)
		{
			if(pPiste->LongueurRepositionnement > (double)m_RecetteVision.pLong_RepositionnementMax->value)
				return 2;	//Trop de décalage entre les points
		}
		if(m_RecetteVision.pLong_LargMin->value > 0)
		{
			if(pPiste->LargeurMinim < (double)m_RecetteVision.pLong_LargMin->value)
				return 10;	//Trop étroit
		}
		if(m_RecetteVision.pLong_LargMax->value > 0)
		{
			if(pPiste->LargeurMinim > (double)m_RecetteVision.pLong_LargMax->value)
				return 11;	//Trop large
		}
		if(m_RecetteVision.pLong_LongMin->value > 0)
		{
			if(pPiste->LongueurMinim < (double)m_RecetteVision.pLong_LongMin->value)
				return 20;	//Trop court
		}
		if(m_RecetteVision.pLong_LongMax->value > 0)
		{
			if(pPiste->LongueurMinim > (double)m_RecetteVision.pLong_LongMax->value)
				return 21;	//Trop long
		}

		//Repositionnement du centre
		pPiste->PtModele.Xmm = pPiste->PtModele.Xmm + PtBlobDestination.Xmm;
		pPiste->PtModele.Ymm = pPiste->PtModele.Ymm + PtBlobDestination.Ymm;
		pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToPix(&pPiste->PtModele);
	}
	else
	{
		//Pas de Blob => Pas normal on arrête
		return 1;
	}
	return 0;
}

int Code2of5_Extraire(CMatroxCamera *Cam, CPiste *pPiste)
{
	PARAM_ROI RoiSachet;
	int iNbLectureOK = 0;

	for(int i=0;i<CB_NBZONES;i++)
	{
		pPiste->CodeSachet[i].RAZ();
	}

	//Centre de la piste
	STRUCT_POINT PtCentrePiste;
	PtCentrePiste.Xpix = (pPiste->roiDetection[0].Roi.X1 + pPiste->roiDetection[0].Roi.X2)/2;
	PtCentrePiste.Ypix = (pPiste->roiDetection[0].Roi.Y1 + pPiste->roiDetection[0].Roi.Y2)/2;
	pPiste->roiDetection[0].Calibration->ToMM(&PtCentrePiste);

	//Position des bords en fonction du repositionnement
	STRUCT_POINT PtHG;
	PtHG.Xmm = PtCentrePiste.Xmm + pPiste->LongueurMinim/2.0;
	PtHG.Ymm = PtCentrePiste.Ymm + pPiste->LargeurMinim/2.0;
	pPiste->roiDetection[0].Calibration->ToPix(&PtHG);
	STRUCT_POINT PtBD;
	PtBD.Xmm = PtCentrePiste.Xmm - pPiste->LongueurMinim/2.0;
	PtBD.Ymm = PtCentrePiste.Ymm - pPiste->LargeurMinim/2.0;
	pPiste->roiDetection[0].Calibration->ToPix(&PtBD);
	long lLargeurPix = (long)(PtBD.Xpix - PtHG.Xpix);
	long lLongueurPix = (long)(PtBD.Ypix - PtHG.Ypix);

	STRUCT_POINT PtCentreSachetRotation;
	PtCentreSachetRotation.Xpix =  Cam->RoiSachetVertical.X1 + Cam->RoiSachetVertical.dX/2;
	PtCentreSachetRotation.Ypix =  Cam->RoiSachetVertical.Y1 + Cam->RoiSachetVertical.dY/2;

	RoiSachet.X1 = (long)PtCentreSachetRotation.Xpix - lLargeurPix/2;
	RoiSachet.X2 = RoiSachet.X1 + lLargeurPix;
	RoiSachet.dX = RoiSachet.X2 - RoiSachet.X1;
	RoiSachet.Y1 = (long)PtCentreSachetRotation.Ypix - lLongueurPix/2;
	RoiSachet.Y2 = RoiSachet.Y1 + lLongueurPix;
	RoiSachet.dY = RoiSachet.Y2 - RoiSachet.Y1;

	//Préparation
	pPiste->CodeSachet[0].mCodeProj.Libelle = "G";
	pPiste->CodeSachet[0].RAZ();
	pPiste->CodeSachet[0].pmRoi_Lecture.X1 = RoiSachet.X1 + m_RecetteVision.pLong_CBPositionnement->value;
	pPiste->CodeSachet[0].pmRoi_Lecture.X2 = pPiste->CodeSachet[0].pmRoi_Lecture.X1 + m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[0].pmRoi_Lecture.dX = pPiste->CodeSachet[0].pmRoi_Lecture.X2 - pPiste->CodeSachet[0].pmRoi_Lecture.X1;
	pPiste->CodeSachet[0].pmRoi_Lecture.Y1 = RoiSachet.Y1 + 50;
	pPiste->CodeSachet[0].pmRoi_Lecture.Y2 = RoiSachet.Y2 - 50;
	pPiste->CodeSachet[0].pmRoi_Lecture.dY = pPiste->CodeSachet[0].pmRoi_Lecture.Y2 - pPiste->CodeSachet[0].pmRoi_Lecture.Y1;

	pPiste->CodeSachet[1].mCodeProj.Libelle = "D";
	pPiste->CodeSachet[1].RAZ();
	pPiste->CodeSachet[1].pmRoi_Lecture.X2 = RoiSachet.X2 - m_RecetteVision.pLong_CBPositionnement->value;
	pPiste->CodeSachet[1].pmRoi_Lecture.X1 = pPiste->CodeSachet[1].pmRoi_Lecture.X2 - m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[1].pmRoi_Lecture.dX = pPiste->CodeSachet[1].pmRoi_Lecture.X2 - pPiste->CodeSachet[1].pmRoi_Lecture.X1;
	pPiste->CodeSachet[1].pmRoi_Lecture.Y1 = RoiSachet.Y1 + 50;
	pPiste->CodeSachet[1].pmRoi_Lecture.Y2 = RoiSachet.Y2 - 50;
	pPiste->CodeSachet[1].pmRoi_Lecture.dY = pPiste->CodeSachet[1].pmRoi_Lecture.Y2 - pPiste->CodeSachet[1].pmRoi_Lecture.Y1;

	//V32ALL
	pPiste->CodeSachet[2].mCodeProj.Libelle = "G2";
	pPiste->CodeSachet[2].RAZ();
	pPiste->CodeSachet[2].pmRoi_Lecture.X1 = RoiSachet.X1 + m_RecetteVision.pLong_CBPositionnement->value + m_RecetteVision.pLong_CBLargeur->value/2;
	pPiste->CodeSachet[2].pmRoi_Lecture.X2 = pPiste->CodeSachet[2].pmRoi_Lecture.X1 + m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[2].pmRoi_Lecture.dX = pPiste->CodeSachet[2].pmRoi_Lecture.X2 - pPiste->CodeSachet[2].pmRoi_Lecture.X1;
	pPiste->CodeSachet[2].pmRoi_Lecture.Y1 = RoiSachet.Y1 + 50;
	pPiste->CodeSachet[2].pmRoi_Lecture.Y2 = RoiSachet.Y2 - 50;
	pPiste->CodeSachet[2].pmRoi_Lecture.dY = pPiste->CodeSachet[2].pmRoi_Lecture.Y2 - pPiste->CodeSachet[2].pmRoi_Lecture.Y1;

	//V32ALL
	pPiste->CodeSachet[3].mCodeProj.Libelle = "D2";
	pPiste->CodeSachet[3].RAZ();
	pPiste->CodeSachet[3].pmRoi_Lecture.X2 = RoiSachet.X2 - m_RecetteVision.pLong_CBPositionnement->value - m_RecetteVision.pLong_CBLargeur->value/2;
	pPiste->CodeSachet[3].pmRoi_Lecture.X1 = pPiste->CodeSachet[3].pmRoi_Lecture.X2 - m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[3].pmRoi_Lecture.dX = pPiste->CodeSachet[3].pmRoi_Lecture.X2 - pPiste->CodeSachet[3].pmRoi_Lecture.X1;
	pPiste->CodeSachet[3].pmRoi_Lecture.Y1 = RoiSachet.Y1 + 50;
	pPiste->CodeSachet[3].pmRoi_Lecture.Y2 = RoiSachet.Y2 - 50;
	pPiste->CodeSachet[3].pmRoi_Lecture.dY = pPiste->CodeSachet[3].pmRoi_Lecture.Y2 - pPiste->CodeSachet[3].pmRoi_Lecture.Y1;

	//V33ALL
	pPiste->CodeSachet[4].mCodeProj.Libelle = "G3";
	pPiste->CodeSachet[4].RAZ();
	pPiste->CodeSachet[4].pmRoi_Lecture.X1 = RoiSachet.X1 + m_RecetteVision.pLong_CBPositionnement->value + m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[4].pmRoi_Lecture.X2 = pPiste->CodeSachet[4].pmRoi_Lecture.X1 + m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[4].pmRoi_Lecture.dX = pPiste->CodeSachet[4].pmRoi_Lecture.X2 - pPiste->CodeSachet[4].pmRoi_Lecture.X1;
	pPiste->CodeSachet[4].pmRoi_Lecture.Y1 = RoiSachet.Y1 + 50;
	pPiste->CodeSachet[4].pmRoi_Lecture.Y2 = RoiSachet.Y2 - 50;
	pPiste->CodeSachet[4].pmRoi_Lecture.dY = pPiste->CodeSachet[4].pmRoi_Lecture.Y2 - pPiste->CodeSachet[4].pmRoi_Lecture.Y1;

	//V33ALL
	pPiste->CodeSachet[5].mCodeProj.Libelle = "D3";
	pPiste->CodeSachet[5].RAZ();
	pPiste->CodeSachet[5].pmRoi_Lecture.X2 = RoiSachet.X2 - m_RecetteVision.pLong_CBPositionnement->value - m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[5].pmRoi_Lecture.X1 = pPiste->CodeSachet[5].pmRoi_Lecture.X2 - m_RecetteVision.pLong_CBLargeur->value;
	pPiste->CodeSachet[5].pmRoi_Lecture.dX = pPiste->CodeSachet[5].pmRoi_Lecture.X2 - pPiste->CodeSachet[5].pmRoi_Lecture.X1;
	pPiste->CodeSachet[5].pmRoi_Lecture.Y1 = RoiSachet.Y1 + 50;
	pPiste->CodeSachet[5].pmRoi_Lecture.Y2 = RoiSachet.Y2 - 50;
	pPiste->CodeSachet[5].pmRoi_Lecture.dY = pPiste->CodeSachet[5].pmRoi_Lecture.Y2 - pPiste->CodeSachet[5].pmRoi_Lecture.Y1;

	CCodeSachet *CodeTemp;
	for(int i=0; i<CB_NBZONES; i++)
	{
		CodeTemp = &pPiste->CodeSachet[i];

		if(!CodeTemp->roiLecture.SetROI(&CodeTemp->pmRoi_Lecture))
		{
			CodeTemp->sErreur = _T("Roi incorrecte");
			CodeTemp->iCodeReponse = -5;
			continue;
		}

		MimConvolve(CodeTemp->roiLecture.milBuf,CodeTemp->roiLecture.milBuf,M_SMOOTH);
		MimProject(CodeTemp->roiLecture.milBuf,CodeTemp->mImgRes->milResult,M_90_DEGREE );
		MimGetResult(CodeTemp->mImgRes->milResult,M_VALUE + M_TYPE_LONG,CodeTemp->mImgRes->lVals);

		CodeTemp->iCodeReponse = CodeTemp->mCodeProj.Extraire(CodeTemp->mImgRes->lVals,CodeTemp->pmRoi_Lecture.dY,CodeTemp->pmRoi_Lecture.dX);
		if(CodeTemp->iCodeReponse > 0)
		{
			CodeTemp->sCodeLu.Format("%s", CodeTemp->mCodeProj.sCode);
			CodeTemp->bLectureOK = true;
			iNbLectureOK++;
		}
	}

	if(iNbLectureOK > 0)
	{
		for(int i=0; i<CB_NBZONES; i++)
		{
			CodeTemp = &pPiste->CodeSachet[i];
			if(CodeTemp->bLectureOK)
			{
				MgraColor(M_DEFAULT,0xFF);
				MgraRect(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posdebut,CodeTemp->pmRoi_Lecture.X2,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posfin);
				MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,CodeTemp->sCodeLu.GetBuffer(CodeTemp->sCodeLu.GetLength()));
			}
		}
	}


	//for(int i=0; i<CB_NBZONES; i++)
	//{
	//	if(bAffiche[i])
	//	{
	//		CodeTemp = &pPiste->CodeSachet[i];
	//		if(CodeOK == i)
	//		{
	//			MgraColor(M_DEFAULT,0xFF);
	//			MgraRect(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posdebut,CodeTemp->pmRoi_Lecture.X2,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posfin);
	//			MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,CodeTemp->sCodeLu.GetBuffer(CodeTemp->sCodeLu.GetLength()));
	//		}
	//		else
	//		{
	//			MgraColor(M_DEFAULT,0xFF);
	//			MgraRect(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,CodeTemp->pmRoi_Lecture.X2,CodeTemp->pmRoi_Lecture.Y2);
	//			MgraLine(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posdebut,CodeTemp->pmRoi_Lecture.X2,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posdebut);
	//			MgraLine(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posfin,CodeTemp->pmRoi_Lecture.X2,CodeTemp->pmRoi_Lecture.Y1 + CodeTemp->mCodeProj.posfin);
	//			switch(iRepCB[i])
	//			{
	//				case 0:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,CodeTemp->sCodeLu.GetBuffer(CodeTemp->sCodeLu.GetLength()));
	//					break;
	//				case -1:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,"Manque barres");
	//					break;
	//				case -2:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,"Trop de barres");
	//					break;
	//				case -3:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,"Nbr incorrect");
	//					break;
	//				case -4:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,"Manque Debut-Fin");
	//					break;
	//				case -5:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,"ROI incorrecte");
	//					break;
	//				case -6:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,"Err identification");
	//					break;
	//				default:
	//					MgraText(M_DEFAULT,Cam->imgSachetVertical.milBuf,CodeTemp->pmRoi_Lecture.X1,CodeTemp->pmRoi_Lecture.Y1,"Erreur non geree");
	//					break;
	//			}
	//		}
	//	}
	//}

	return iNbLectureOK;
}

int DataMatrix_Extraire(CMatroxCamera *Cam, CPiste *pPiste)
{
	//Recherche du datamatrix
	//Rotation du point datamatrix en fonction de l'orientation du Minim(mm)
	pPiste->PtDatamatrix.Zmm  = pPiste->PtModele.Zmm;
	CalculerRotation(&pPiste->PtDatamatrixOrigine,&pPiste->PtDatamatrix,pPiste->PtDatamatrix.Zmm);
	//Repositionnement (mm)
	pPiste->PtDatamatrix.Xmm = pPiste->PtModele.Xmm + pPiste->PtDatamatrix.Xmm;
	pPiste->PtDatamatrix.Ymm = pPiste->PtModele.Ymm + pPiste->PtDatamatrix.Ymm;
	//Conversion en pixels
	pPiste->roiDetection[pPiste->IndexLastRoi].Calibration->ToPix(&pPiste->PtDatamatrix);

	pPiste->proiDMX1.X1 = (long)pPiste->PtDatamatrix.Xpix-m_RecetteVision.Code_RoiSizeXpix/2 + pPiste->roiDetection[pPiste->IndexLastRoi].Roi.X1;
	pPiste->proiDMX1.dX = m_RecetteVision.Code_RoiSizeXpix;
	pPiste->proiDMX1.X2 = pPiste->proiDMX1.X1 + pPiste->proiDMX1.dX;
	pPiste->proiDMX1.Y1 = (long)pPiste->PtDatamatrix.Ypix-m_RecetteVision.Code_RoiSizeYpix/2 + pPiste->roiDetection[pPiste->IndexLastRoi].Roi.Y1;
	pPiste->proiDMX1.dY = m_RecetteVision.Code_RoiSizeYpix;
	pPiste->proiDMX1.Y2 = pPiste->proiDMX1.Y1 + pPiste->proiDMX1.dY;
	pPiste->roiCode.SetROI(&pPiste->proiDMX1);

	//pPiste->CodeObj.SetControl(M_SEARCH_ANGLE, pPiste->PtModele.Zpix); 
	pPiste->CodeObj.Trouver();

	if(pPiste->CodeObj.bResult)
		return 1;

	if(m_RecetteVision.pLong_CodeDoubleTest->value == 0)
	{
		return 1;
	}

	//DoubleTest
	//Recherche du datamatrix
	//Rotation du point datamatrix en fonction de l'orientation du Minim(mm)
	STRUCT_POINT OrigineInverse;
	OrigineInverse.Xmm = -pPiste->PtDatamatrixOrigine.Xmm;
	OrigineInverse.Ymm = -pPiste->PtDatamatrixOrigine.Ymm;

	pPiste->PtDatamatrix.Zmm  = pPiste->PtModele.Zmm;
	CalculerRotation(&OrigineInverse,&pPiste->PtDatamatrix,pPiste->PtDatamatrix.Zmm);
	//Repositionnement (mm)
	pPiste->PtDatamatrix.Xmm = pPiste->PtModele.Xmm + pPiste->PtDatamatrix.Xmm;
	pPiste->PtDatamatrix.Ymm = pPiste->PtModele.Ymm + pPiste->PtDatamatrix.Ymm;
	//Conversion en pixels
	pPiste->roiDetection[pPiste->IndexLastRoi].Calibration->ToPix(&pPiste->PtDatamatrix);

	pPiste->proiDMX2.X1 = (long)pPiste->PtDatamatrix.Xpix-m_RecetteVision.Code_RoiSizeXpix/2 + pPiste->roiDetection[pPiste->IndexLastRoi].Roi.X1;
	pPiste->proiDMX2.dX = m_RecetteVision.Code_RoiSizeXpix;
	pPiste->proiDMX2.X2 = pPiste->proiDMX2.X1 + pPiste->proiDMX2.dX;
	pPiste->proiDMX2.Y1 = (long)pPiste->PtDatamatrix.Ypix-m_RecetteVision.Code_RoiSizeYpix/2 + pPiste->roiDetection[pPiste->IndexLastRoi].Roi.Y1;
	pPiste->proiDMX2.dY = m_RecetteVision.Code_RoiSizeYpix;
	pPiste->proiDMX2.Y2 = pPiste->proiDMX2.Y1 + pPiste->proiDMX2.dY;
	pPiste->roiCode.SetROI(&pPiste->proiDMX2);

	//pPiste->CodeObj.SetControl(M_SEARCH_ANGLE, pPiste->PtModele.Zpix); 
	pPiste->CodeObj.Trouver();
	return 2;
}

int Rotation_Sachet(CMatroxCamera *Cam, CPiste *pPiste)
{
	//Valeurs de retour :
	//0 = Rotation OK
	//1 = Rotation NOK

	STRUCT_POINT PtCentre;
	PARAM_ROI RoiTemp;
	double dAngle;

	//Repositionnement de la zone
	PtCentre.Xmm = pPiste->PtModele.Xmm;
	PtCentre.Ymm = pPiste->PtModele.Ymm;
	//Conversion en pixels
	pPiste->roiDetection[pPiste->IndexLastRoi].Calibration->ToPix(&PtCentre);

	RoiTemp.X1 = (long)PtCentre.Xpix-400 + pPiste->roiDetection[pPiste->IndexLastRoi].Roi.X1;
	RoiTemp.dX = 800;
	RoiTemp.X2 = RoiTemp.X1 + RoiTemp.dX;
	RoiTemp.Y1 = (long)PtCentre.Ypix-400 + pPiste->roiDetection[pPiste->IndexLastRoi].Roi.Y1;
	RoiTemp.dY = 800;
	RoiTemp.Y2 = RoiTemp.Y1 + RoiTemp.dY;
	if(!pPiste->roiSachetRotationSrc.SetROI(&RoiTemp))
		return 2;

	Cam->RoiSachetVertical.X1 = pPiste->lNumPiste*800;
	Cam->RoiSachetVertical.dX = 800;
	Cam->RoiSachetVertical.X2 = Cam->RoiSachetVertical.X1 + Cam->RoiSachetVertical.dX;
	Cam->RoiSachetVertical.Y1 = 0;
	Cam->RoiSachetVertical.dY = 800;
	Cam->RoiSachetVertical.Y2 = Cam->RoiSachetVertical.Y1 + Cam->RoiSachetVertical.dY;
	if(!pPiste->roiSachetRotationDst.SetROI(&Cam->RoiSachetVertical))
		return 3;

	//Calcul de l'angle
	//dAngle = pPiste->PtModele.Zpix;
	dAngle = pPiste->PtModele.Tmm;
	if((dAngle>0) && (dAngle< 90))	
	{
		dAngle = 90-dAngle;
	}
	else
	{
		if((dAngle> 90) && (dAngle< 180))
		{
			dAngle = 90 - dAngle;
		}
		else
		{
			if((dAngle> 180) && (dAngle< 270))
			{
				dAngle = 270 - dAngle;
			}
			else
			{
				dAngle = 270 - dAngle;
			}
		}
	}

	MimRotate(pPiste->roiSachetRotationSrc.milBuf,pPiste->roiSachetRotationDst.milBuf,dAngle,400,400,400,400,M_DEFAULT);

	return 0;
}

unsigned long MFTYPE Acquisition(void *TParam)
{
	CMatroxCamera *Cam = (CMatroxCamera *) TParam;
	double OffsetRobotX = 0;
	
	PARAM_ROI RoiTemp;

	CString StringTemp;
	char sFichier[100];
	PARAM_ROI RoiBlob;
	long OffsetModeleYpix;
	long OffsetModeleXpix;
	//V2.10
	int iColor = M_RED;
	long lTemp;
	
	int IndexGrab = 0;
	int IndexLoad = 0;

	int i,iResultRotation,iResult2of5;
	int iPiste;
	CPiste* pPiste;

	MIL_INT TotalBlobsInitial = 0;
	int iBlobSelectionInitial = 0;

	long TotalBlobsCouleur = 0;
	int iBlobSelectionCouleur = 0;
	int ResultTache;
	int ResultRepositionnement;

	double dSurfTemp = 0;
	int iBlob = 0;

	int iDilate = 2;
	double dAngle = 0.;

	long HistoMin=30,HistoMax=100;

	long LastTick = 0;
	long DiffTick = 0;

	//TODOBM : A faire au chargement de la recette
	STRUCT_POINT Pt1;
	STRUCT_POINT Pt2;

	//Synchro UV
	long lPosY;
	long ImageUV;
	long LigneUV;
	int IndexUV;

	bool bSetTimerPulse = false;
	CString strNomFichierImg;

	Pt1.Xmm = 0;
	Pt1.Ymm = 0;
	Cam->Piste[0].roiDetectionZoom[0].Calibration->ToPix(&Pt1);
	Pt1.Xpix = Pt1.Xpix + Cam->Piste[0].PtDatamatrixOrigine.Xpix;
	Pt1.Ypix = Pt1.Ypix + Cam->Piste[0].PtDatamatrixOrigine.Ypix;	
	Cam->Piste[0].roiDetectionZoom[0].Calibration->ToMM(&Pt1);
	Cam->Piste[0].PtDatamatrixOrigine.Xmm = Pt1.Xmm;
	Cam->Piste[0].PtDatamatrixOrigine.Ymm = Pt1.Ymm;

	Pt1.Xmm = 0;
	Pt1.Ymm = 0;
	Cam->Piste[1].roiDetectionZoom[0].Calibration->ToPix(&Pt1);
	Pt1.Xpix = Pt1.Xpix + Cam->Piste[1].PtDatamatrixOrigine.Xpix;
	Pt1.Ypix = Pt1.Ypix + Cam->Piste[1].PtDatamatrixOrigine.Ypix;	
	Cam->Piste[1].roiDetectionZoom[0].Calibration->ToMM(&Pt1);
	Cam->Piste[1].PtDatamatrixOrigine.Xmm = Pt1.Xmm;
	Cam->Piste[1].PtDatamatrixOrigine.Ymm = Pt1.Ymm;

	Pt1.Xmm = 0;
	Pt1.Ymm = 0;
	Cam->Piste[2].roiDetectionZoom[0].Calibration->ToPix(&Pt1);
	Pt1.Xpix = Pt1.Xpix + Cam->Piste[2].PtDatamatrixOrigine.Xpix;
	Pt1.Ypix = Pt1.Ypix + Cam->Piste[2].PtDatamatrixOrigine.Ypix;	
	Cam->Piste[2].roiDetectionZoom[0].Calibration->ToMM(&Pt1);
	Cam->Piste[2].PtDatamatrixOrigine.Xmm = Pt1.Xmm;
	Cam->Piste[2].PtDatamatrixOrigine.Ymm = Pt1.Ymm;

	Cam->Piste[0].lNextTop = 0;
	Cam->Piste[1].lNextTop = 0;
	Cam->Piste[2].lNextTop = 0;

	//V2.53 : Résolution Zoom
	Pt1.Xmm = 0;
	Pt1.Ymm = 0;
	Cam->Piste[1].roiDetectionZoom[0].Calibration->ToPix(&Pt1);
	Pt1.Ypix = 0;
	Cam->Piste[1].roiDetectionZoom[0].Calibration->ToMM(&Pt1);

	Pt2.Xpix = Pt1.Xpix;
	Pt2.Ypix = Cam->lYZoom;
	Cam->Piste[1].roiDetectionZoom[0].Calibration->ToMM(&Pt2);

	Cam->dPixelSizeYZoom = abs(Pt1.Xmm-Pt2.Xmm) / Cam->lYZoom;

	//V34
	theApp.CB_NbCodes = 35;

	theApp.CB_ListeCodes[0]	= "097121";
	theApp.CB_ListeCodes[1]	= "097122";
	theApp.CB_ListeCodes[2]	= "097134";
	theApp.CB_ListeCodes[3]	= "097154";
	theApp.CB_ListeCodes[4]	= "097157";
	theApp.CB_ListeCodes[5]	= "097158";
	theApp.CB_ListeCodes[6]	= "097162";
	theApp.CB_ListeCodes[7]	= "097163";
	theApp.CB_ListeCodes[8]	= "097165";
	theApp.CB_ListeCodes[9]	= "097166";
	theApp.CB_ListeCodes[10] = "097167";
	theApp.CB_ListeCodes[11] = "097168";
	theApp.CB_ListeCodes[12] = "097173";
	theApp.CB_ListeCodes[13] = "097174";
	theApp.CB_ListeCodes[14] = "097175";
	theApp.CB_ListeCodes[15] = "097176";
	theApp.CB_ListeCodes[16] = "097177";
	theApp.CB_ListeCodes[17] = "097178";
	theApp.CB_ListeCodes[18] = "097184";
	theApp.CB_ListeCodes[19] = "097185";
	theApp.CB_ListeCodes[20] = "097186";
	theApp.CB_ListeCodes[21] = "097187";
	theApp.CB_ListeCodes[22] = "097188";
	theApp.CB_ListeCodes[23] = "097189";
	theApp.CB_ListeCodes[24] = "097190";
	theApp.CB_ListeCodes[25] = "097191";
	theApp.CB_ListeCodes[26] = "097194";
	theApp.CB_ListeCodes[27] = "097195";
	theApp.CB_ListeCodes[28] = "097196";
	theApp.CB_ListeCodes[29] = "097197";
	theApp.CB_ListeCodes[30] = "097198";
	theApp.CB_ListeCodes[31] = "097199";
	theApp.CB_ListeCodes[32] = "097204";
	theApp.CB_ListeCodes[33] = "097205";
	theApp.CB_ListeCodes[34] = "097208";


	if(Cam->milDig>0)
	{
		MdigControl(Cam->milDig,M_GRAB_MODE,M_ASYNCHRONOUS); 
		MdigControl(Cam->milDig,M_ROTARY_ENCODER_POSITION_TRIGGER,pParamVisionDiviseur->value);
		MdigControl(Cam->milDig,M_ROTARY_ENCODER_POSITION,0);
	}
	 
	while ( Cam->Param_Acquisition.iMode )
	{
		MappTimer(M_TIMER_READ+M_GLOBAL, &Cam->Date_Calcul);
		Cam->Temps_Calcul = Cam->Date_Calcul - Cam->Date_Image;
		if(Cam->iModeAcquisition == 1)
		{
			do
			{
				Sleep(100);
			}
			while(Cam->iSnap<1);
			Cam->iSnap=0;

			if (Cam->milDig!=0)
			{
				MdigGrab(Cam->milDig, Cam->imgGrab[IndexGrab].milBuf);
			}
		}
		else
		{
			if (Cam->milDig!=0)
			{
				MdigGrab(Cam->milDig, Cam->imgGrab[IndexGrab].milBuf);
			}
			Cam->iSnap=0;
		}
		IndexLoad = (IndexGrab - 1 + Cam->Acq_Nb) % Cam->Acq_Nb;
		IndexGrab = (IndexGrab + 1) % Cam->Acq_Nb;
		
		//Vérification des temps
		double DateNew = 0;
		MappTimer(M_TIMER_READ+M_GLOBAL, &DateNew);
		Cam->Temps_Image = DateNew - Cam->Date_Image;
		Cam->Date_Image = DateNew;

		//V23 MappTimer(M_TIMER_READ+M_GLOBAL, &Cam->Temps_Image);
		//V23 RAZ Timer
		//V23 MappTimer(M_TIMER_RESET+M_GLOBAL, M_NULL);

		//V2.40 : Contrôle du temps max en fonction seulement du temps de calcul
		if(Cam->Temps_Calcul*1000 < pParamVisionValeurTimeOut->value)
		{
			//#2 Activation des TOPs seulement si le temps de calcul est inférieur au seuil (+/- 160 ms)
			if(Cam->milDigs>0)	//#2 : test sur milDigs | #1:MilDigs = Port1; #2:milDigs = Port0
			{
				bSetTimerPulse = false;

				for (int i = 0; i < 3; i++)
				{
					if(Cam->Piste[i].lNextTop == Cam->lNumeroImage)
					{
						MdigControl(Cam->milDigs,M_USER_BIT_STATE + Cam->milUSERBIT[i] + M_DIG_DISPATCH_IMMEDIATE,M_ON);	//#12
						bSetTimerPulse = true;
						Cam->Piste[i].m_Robot->NumTop ++;
					}
				}
				

				//V1.22 : Regroupement de la gestion du pulse pour tous les robots
				if(bSetTimerPulse)
				{
					theApp.TimerVision = SetTimer(theApp.MachineView->GetSafeHwnd(),ID_TIMER_PULSEVISION,pParamLgPulseVision->value,NULL);
				}
				//V23-
				//DiffTick = theApp.m_Recette.m_OPCClient->TickOPC-LastTick;
				//StringTemp.Format("Temps = %ld %ld %ld",(long)(Cam->Temps_Image*1000),(long)(Cam->Temps_Calcul*1000),DiffTick);
				//theApp.SetStringListVisionTemps(StringTemp);
			}
		}
		//V28 : Contrôle vitesse
		double TempsImage_ms = Cam->Temps_Image*1000.0;
		double TempsMin_ms = 0;
		switch(theApp.m_Recette.ArtMinims.iParam1)//theApp.m_Recette.Article.Type
		{
			case RECETTE_TYPEBLISTER:
				TempsMin_ms = pParamVisionFrequenceRapport->value / pParamVisionFrequenceBlister->value;
				break;

			default:
				TempsMin_ms = pParamVisionFrequenceRapport->value / pParamVisionFrequenceSachet->value;
				break;
		}
		if(TempsImage_ms < TempsMin_ms)
		{
			theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue = 1;
		}
		else
		{
			theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue = 0;
		}

		//V23+
		DiffTick = theApp.m_Recette.m_OPCClient->TickOPC-LastTick;
		StringTemp.Format("Temps = %ld %ld %ld %ld",(long)(Cam->Temps_Image*1000),(long)(Cam->Temps_Calcul*1000),DiffTick,(long)(TempsMin_ms));
		theApp.SetStringListVisionTemps(StringTemp);



		if(Cam->Simulation_Enable)
		{
			Cam->Simulation_IndexLoad = (Cam->Simulation_IndexLoad+1)%Cam->Simulation_NbRoi;
			MbufCopy(Cam->Simulation_Roi[Cam->Simulation_IndexLoad].milBuf,Cam->imgGrab[IndexLoad].milBuf);
		}

		//V43-ALL
		if (m_RecetteVision.bResetCountDefVision)
		{
			m_RecetteVision.bResetCountDefVision = false;
			for (i = 0;i < NBPISTE;i++)
			{
				Cam->Piste[i].lCountDefVision_Surface = 0;
				Cam->Piste[i].lCountDefVision_Inverse = 0;
				Cam->Piste[i].lCountDefVision_Modele = 0;
				Cam->Piste[i].lCountDefVision_Dimensions = 0;
				Cam->Piste[i].lCountDefVision_Codes = 0;
				Cam->Piste[i].lCountDefVision_TachesUV = 0;
				Cam->Piste[i].lCountDefVision_TachesCouleur = 0;
				Cam->Piste[i].lCountDefVision_AnglePrise = 0;
				//Cam->Piste[i].lCountVision_OK = 0;//V 44.8
			}
		}

		if(m_RecetteVision.bChargerVision)
		{
			m_RecetteVision.bChargerVision = false;
			Pt1.Xmm = 0;
			Pt1.Ymm = 0;
			Pt2.Xmm = m_RecetteVision.pLong_CodeSizeX->value;
			Pt2.Ymm = m_RecetteVision.pLong_CodeSizeY->value;
			Cam->Piste[1].roiDetection[0].Calibration->ToPix(&Pt1);
			Cam->Piste[1].roiDetection[0].Calibration->ToPix(&Pt2);
			m_RecetteVision.Code_RoiSizeXpix =(long) abs(Pt1.Xpix - Pt2.Xpix);
			m_RecetteVision.Code_RoiSizeYpix =(long) abs(Pt1.Ypix - Pt2.Ypix);

			for(i=0;i<NBPISTE;i++)
			{
				//Rechargement des modèles d'objet (Mise à jour de l'angle)
				Cam->ModeleBlister[i].Charger();
				Cam->ModeleSachet[i].Charger();

				//Mise à jour paramètres Datamatrix
				Cam->Piste[i].CodeObj.Type = m_RecetteVision.pLong_CodeType->value;
				Cam->Piste[i].PtDatamatrixOrigine.Xmm = m_RecetteVision.pLong_CodeOffsetX->value;
				Cam->Piste[i].PtDatamatrixOrigine.Ymm = m_RecetteVision.pLong_CodeOffsetY->value;
				Cam->Piste[i].CodeObj.ChargerFromFile();
			}
		}

		Cam->lNumeroImage ++;

		//V2.40 : Vérification du temps image pour détection des arrêts du tapis
		if(Cam->Temps_Image*1000 > pParamVisionImageTimeOut->value)
		{
			if(Cam->milDig>0)
			{
				//Détection arrêt tapis => Annulation des traitements sur les deux prochaines images
				Cam->lNumeroImageDernierArret = Cam->lNumeroImage;
				StringTemp.Format("ARRET TAPIS = %ld",(long)(Cam->Temps_Image*1000));
				theApp.SetStringListVisionTemps(StringTemp);
			}
		}

		switch(Cam->m_iModeInspection)
		{
			case CAM_MODE_ARRET:
				//sprintf_s(sText,"MODE ARRET");
				break;

			case CAM_MODE_INSPECTION:
				if(theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM1].NewValue == 1)
				{
					if(theConfig.str_Machine.Right(5) != "SIMUL")
					{
						StringTemp.Format("ALARME VITESSE");
						theApp.SetStringListVision(StringTemp);
						break;
					}
				}

				//Selection des couleurs, pleine résolution
				MbufCopyColor(Cam->imgGrab[IndexLoad].milBuf,Cam->imgSelectionColor.milBuf,iColor);
				//ZOOM pour Tracking
				MimResize(Cam->imgSelectionColor.milBuf,Cam->imgGrabZoom.milBuf,1/Cam->dZoom,1/Cam->dZoom,M_DEFAULT);

				//PRETRAITEMENT
				if(m_RecetteVision.pLong_Pretraitement->value == 1)
				{
					MimConvolve(Cam->imgGrabZoom.milBuf,Cam->imgGrabZoom.milBuf,M_SOBEL);
				}

				//TRACKING
				for(iPiste=0;iPiste<NBPISTE;iPiste++)
				{
					pPiste = &Cam->Piste[iPiste];
					if(m_RecetteVision.pLong_Seuil->value == 0)
					{
						MbufCopy(pPiste->roiGrabZoom.milBuf,pPiste->roiBinZoom.milBuf);
					}
					else
					{
						MimBinarize(pPiste->roiGrabZoom.milBuf,pPiste->roiBinZoom.milBuf,M_GREATER,m_RecetteVision.pLong_Seuil->value,M_NULL);
					}
					MimProject(pPiste->roiBinZoom.milBuf,Cam->imgRes[iPiste].milResult,M_90_DEGREE );
					MimGetResult(Cam->imgRes[iPiste].milResult,M_VALUE + M_TYPE_LONG,Cam->imgRes[iPiste].lVals);

					pPiste->Saturation = false;		//V1.22
					pPiste->DeuxiemeMinim = false;		//V1.22
					if(pPiste->TrackEnCours)
					{
						pPiste->IndexEcritureReel ++;
						pPiste->IndexEcriture = pPiste->IndexEcritureReel;
						if(pPiste->IndexEcriture > (NBCUMUL/2 - 1))
						{
							pPiste->IndexEcriture = NBCUMUL/2 - 1;
							pPiste->Saturation = true;
						}

						MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrack[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,iColor);
						MbufCopy(pPiste->roiBinZoom.milBuf,pPiste->roiTrackZoom[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf);
						//V2.10 : récupération plan couleur pour les taches
						if(m_RecetteVision.pLong_TachePlan->value == 0)
							MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackTache[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,M_RED);
						if(m_RecetteVision.pLong_TachePlan->value == 1)
							MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackTache[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,M_GREEN);
						if(m_RecetteVision.pLong_TachePlan->value == 2)
							MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackTache[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,M_BLUE);
						//V2.50 : Traking de l'image couleur pour la sauvegarde des défauts
						MbufCopy(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackCouleur[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf);
					}

					pPiste->AnalyserMinim = false;
					pPiste->AfficherDefaut = false;		//V30
					for(i=0;i<Cam->lYZoom;i++)
					{
						if(!pPiste->TrackEnCours)
						{
							if(Cam->imgRes[iPiste].lVals[i] > (m_RecetteVision.pLong_SeuilProj->value*255))
							{
								//Début Minim
								pPiste->IndexEcritureReel = 0;
								pPiste->IndexEcriture = 0;
								pPiste->TrackEnCours = true;
								MbufClear(pPiste->roiDetectionZoom[pPiste->IndexRoi].milBuf,0x00);	//A SUPPRIMER
								MbufClear(pPiste->roiDetection[pPiste->IndexRoi].milBuf,0x00);		//A SUPPRIMER
								MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrack[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,iColor);
								MbufCopy(pPiste->roiBinZoom.milBuf,pPiste->roiTrackZoom[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf);
								//V2.10 : récupération couleur a soustraire
								if(m_RecetteVision.pLong_TachePlan->value == 0)
									MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackTache[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,M_RED);
								if(m_RecetteVision.pLong_TachePlan->value == 1)
									MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackTache[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,M_GREEN);
								if(m_RecetteVision.pLong_TachePlan->value == 2)
									MbufCopyColor(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackTache[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf,M_BLUE);
								//V2.50 : Traking de l'image couleur pour la sauvegarde des défauts
								MbufCopy(pPiste->roiGrab[IndexLoad].milBuf,pPiste->roiTrackCouleur[pPiste->IndexEcriture + pPiste->IndexRoi*NBCUMUL/2].milBuf);
								
								pPiste->CumulBin = Cam->imgRes[iPiste].lVals[i]/255;
								pPiste->PositionDebut = i;
								pPiste->NumeroImageDebut[pPiste->IndexRoi] = Cam->lNumeroImage;
								//V2.53
								pPiste->MetrageDebut = (Cam->lNumeroImage*Cam->lYZoom + i)*Cam->dPixelSizeYZoom;
							}
						}
						else
						{
							pPiste->CumulBin += Cam->imgRes[iPiste].lVals[i]/255;
							if(Cam->imgRes[iPiste].lVals[i] < (m_RecetteVision.pLong_SeuilProj->value*255))
							{
								//Fin Minim
								pPiste->PositionFin = pPiste->IndexEcritureReel*Cam->lYZoom + i;	//Donne la longueur de l'objet
								Pt1.Xpix = 0;
								Pt1.Ypix = pPiste->PositionDebut;
								Pt2.Xpix = 0;
								Pt2.Ypix = pPiste->PositionFin;
								pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&Pt1);
								pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&Pt2);

								//V1.22 : Prise avec chevauchement
								if(pPiste->DeuxiemeMinim)
								{
									//On a deux minims dans la même image => on ne traite pas le deuxième
									pPiste->TrackEnCours = false;
								}
								else
								{
									if(pPiste->Saturation)
									{
										MgraText(M_DEFAULT,pPiste->roiDetectionZoom[pPiste->IndexRoi].milBuf,0,0,"GUIRLANDE");
										pPiste->IndexRoi = 1 - pPiste->IndexRoi;
										pPiste->TrackEnCours = false;
										StringTemp.Format("Robot %d : Guirlande",iPiste);
										theApp.SetStringListVision(StringTemp);
									}
									else
									{
										pPiste->DeuxiemeMinim = true;
										if(abs(Pt1.Xmm-Pt2.Xmm) > m_RecetteVision.pLong_LongueurMax->value)
											pPiste->MinimToutSeul = false;
										else
											pPiste->MinimToutSeul = true;

										pPiste->Result_CumulProj = pPiste->CumulBin;
										pPiste->Result_PositionDebut = pPiste->PositionDebut;
										pPiste->Result_PositionFin = pPiste->PositionFin;
										pPiste->Result_IndexEcriture = pPiste->IndexEcriture;

										pPiste->IndexLastRoi = pPiste->IndexRoi;
										pPiste->AnalyserMinim = true;

										pPiste->IndexRoi = 1 - pPiste->IndexRoi;
										pPiste->TrackEnCours = false;

										//V2.55 : Distance depuis le dernier Minim
										pPiste->DistanceDepuisDernier = pPiste->MetrageDebut - pPiste->MetrageFin;
									}
								}
								//V2.53
								pPiste->MetrageFin = (Cam->lNumeroImage*Cam->lYZoom + i)*Cam->dPixelSizeYZoom;
							}
						}
					}

					pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
					pPiste->Result_iDefaut = DEFAUT_AUCUN;

					//V2.41 : Annulation des contrôles sur les premières images après arrêt
					if(Cam->lNumeroImage < (Cam->lNumeroImageDernierArret+3))
					{
						pPiste->AnalyserMinim = false;

						StringTemp.Format("Attente redemarrage %ld",Cam->lNumeroImageDernierArret);
						theApp.SetStringListVision(StringTemp);
					}

					if(pPiste->AnalyserMinim)
					{
						switch(theApp.m_Recette.ArtMinims.iParam1)//theApp.m_Recette.Article.Type
						{
							case RECETTE_TYPEBLISTER:
								OffsetRobotX = (double)pParamVisionOffsetRobotXBlister->value;

								//IDENTIFICATION PAR LES BLOBS CAS BLISTER
								//POSITIONNEMENT DE LA ZONE DE RECHERCHE DU MODELE
								RoiBlob.X1 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X1;
								RoiBlob.X2 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X2;
								RoiBlob.dX = RoiBlob.X2 - RoiBlob.X1;
								RoiBlob.Y1 = pPiste->Result_PositionDebut-10;
								RoiBlob.Y2 = pPiste->Result_PositionFin+10;
								if(RoiBlob.Y1<0)	RoiBlob.Y1 = 0;
								OffsetModeleXpix = RoiBlob.X1;
								OffsetModeleYpix = RoiBlob.Y1;
								if(RoiBlob.Y2>Cam->lYZoomCumul/2)	RoiBlob.Y2 = Cam->lYZoomCumul/2;
								RoiBlob.Y1 += pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1;
								RoiBlob.Y2 += pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1;
								RoiBlob.dY = RoiBlob.Y2 - RoiBlob.Y1;
								if(pPiste->roiBlob.SetROI(&RoiBlob))
								{
									//Recherche des blobs
									Cam->InitialBlob.Calculer(pPiste->roiBlob.milBuf); 
									TotalBlobsInitial = Cam->InitialBlob.Selectionner( M_EXCLUDE, M_AREA, M_LESS_OR_EQUAL, 500, M_NULL); 

									//Sélection du plus grand blob
									iBlobSelectionInitial = 0;
									dSurfTemp = 0;
									if(TotalBlobsInitial > 0)
									{
										if(TotalBlobsInitial < NB_MAX_BLOBS_INITIAL)
										{
											Cam->InitialBlob.GetResult(M_AREA);
											Cam->InitialBlob.GetResult(M_BOX_X_MIN);
											Cam->InitialBlob.GetResult(M_BOX_X_MAX);
											Cam->InitialBlob.GetResult(M_BOX_Y_MIN);
											Cam->InitialBlob.GetResult(M_BOX_Y_MAX);
											Cam->InitialBlob.GetResult(M_NUMBER_OF_HOLES);

											for(iBlob=0; iBlob < TotalBlobsInitial; iBlob++)
											{
												if(Cam->InitialBlob.Area[iBlob] > dSurfTemp)
												{
													dSurfTemp = Cam->InitialBlob.Area[iBlob];
													iBlobSelectionInitial = iBlob;
												}
											}
										}
										else
										{
											//Trop de blobs trouvés supérieurs à 500 points, on passe cette image
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											StringTemp.Format("Robot %d : Saturation BlobInitial",iPiste);
											theApp.SetStringListVision(StringTemp);
										}
									}
									else
									{
										//Aucun blob trouvé supérieur à 500 points, on passe cette image
										pPiste->AnalyserMinim = false;
										pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
										pPiste->Result_iDefaut = DEFAUT_AUCUN;
									}
								}
								else
								{
									//Erreur de positionnement
									pPiste->AnalyserMinim = false;
									pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
									pPiste->Result_iDefaut = DEFAUT_AUCUN;
									StringTemp.Format("Robot %d : Erreur BlobInitial",iPiste);
									theApp.SetStringListVision(StringTemp);
								}

								//Vérification que la surface du blob (Pour différencier d'un sachet)
								if(pPiste->AnalyserMinim)
								{
									if(Cam->InitialBlob.Area[iBlobSelectionInitial] > m_RecetteVision.pLong_BlobMax->value)
									{
										//Supérieure à surface MAX =>	NE PAS CONSIDERER
										pPiste->AnalyserMinim = false;
										pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
										pPiste->Result_iDefaut = DEFAUT_AUCUN;

										pPiste->lCountDefVision_Surface ++;//V 43-ALL 
										
										StringTemp.Format("Robot %d : Surface max = %ld",iPiste,(long)(Cam->InitialBlob.Area[iBlobSelectionInitial]));
										theApp.SetStringListVision(StringTemp);
									}
									if(Cam->InitialBlob.Area[iBlobSelectionInitial] < m_RecetteVision.pLong_BlobMin->value)
									{
										//Inférieure à surface MIN =>	NE PAS CONSIDERER
										pPiste->AnalyserMinim = false;
										pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
										pPiste->Result_iDefaut = DEFAUT_AUCUN;

										pPiste->lCountDefVision_Surface++;//V 43-ALL 
										StringTemp.Format("Robot %d : Surface min = %ld",iPiste,(long)(Cam->InitialBlob.Area[iBlobSelectionInitial]));
										theApp.SetStringListVision(StringTemp);
									}
								}

								//Vérification de la face par comptage des trous
								if(pPiste->AnalyserMinim)
								{
									if(Cam->InitialBlob.NumberHoles[iBlobSelectionInitial] < 40)	//TODO : à parametrer
									{
										//Pas assez de trous => Face non imprimée
										pPiste->AnalyserMinim = false;
										pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
										pPiste->Result_iDefaut = DEFAUT_AUCUN;
										pPiste->lCountDefVision_Inverse++;//V 43-ALL 

										StringTemp.Format("Robot %d : Inverse %ld",iPiste,(long)(Cam->InitialBlob.NumberHoles[iBlobSelectionInitial]));
										theApp.SetStringListVision(StringTemp);
									}
								}

								//RECHERCHE DU MODELE
								if(pPiste->AnalyserMinim)
								{
									//V1.22 Affectation de la roi selon le blob sélectionné
									Cam->RoiModele.X1 = RoiBlob.X1 + (long)(Cam->InitialBlob.Box_XMin[iBlobSelectionInitial]) - 10;
									if(Cam->RoiModele.X1<0)	Cam->RoiModele.X1 = 0;
									Cam->RoiModele.X2 = RoiBlob.X1 + (long)(Cam->InitialBlob.Box_XMax[iBlobSelectionInitial]) + 10;
									if(Cam->RoiModele.X2>pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X2)	Cam->RoiModele.X2 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X2;
									Cam->RoiModele.dX = Cam->RoiModele.X2 - Cam->RoiModele.X1;
									Cam->RoiModele.Y1 = RoiBlob.Y1 + (long)(Cam->InitialBlob.Box_YMin[iBlobSelectionInitial]) - 10;
									if(Cam->RoiModele.Y1<pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1)	Cam->RoiModele.Y1 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1;
									Cam->RoiModele.Y2 = RoiBlob.Y1 + (long)(Cam->InitialBlob.Box_YMax[iBlobSelectionInitial]) + 10;
									if(Cam->RoiModele.Y2>pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y2)	Cam->RoiModele.Y2 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y2;
									Cam->RoiModele.dY = Cam->RoiModele.Y2 - Cam->RoiModele.Y1;

									OffsetModeleXpix = Cam->RoiModele.X1 - RoiBlob.X1;
									OffsetModeleYpix = Cam->RoiModele.Y1 % pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.dY;

									if(pPiste->roiModele.SetROI(&Cam->RoiModele))
									{
										Cam->ModeleBlister[iPiste].mImg = &pPiste->roiModele;
										if(Cam->ModeleBlister[iPiste].Trouver())
										{
											//Point central de l'objet
											pPiste->PtModele.Xpix = Cam->ModeleBlister[iPiste].ResImgX[0] + OffsetModeleXpix;
											pPiste->PtModele.Ypix = Cam->ModeleBlister[iPiste].ResImgY[0] + OffsetModeleYpix;
											pPiste->PtModele.Tmm = Cam->ModeleBlister[iPiste].ResAngle[0];//Zpix
											pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&pPiste->PtModele);
											pPiste->PtModele.Zmm = pPiste->PtModele.Tmm - 90;
										}
										else
										{	
											//Modele non trouvé
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;

											pPiste->lCountDefVision_Modele++;
											
											//V17 : Images inutiles
											//if(pParamVisionSauvegarde->value == 1)
											//{
											//	strNomFichierImg.Format("%s\\%s\\CodeNC\\ModeleNC%d_%ld.bmp",theConfig.str_RepertoireData,NOM_DOSSIER_IMAGES,iPiste,Cam->lNumeroImage);
											//	sprintf(sFichier,strNomFichierImg);
											//	//MbufExport(sFichier,M_BMP,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf);
											//	//V2.51 : Enregistrement image couleur
											//	RoiTemp.X1 = (long)( Cam->RoiModele.X1*Cam->dZoom);
											//	RoiTemp.X2 = (long)( Cam->RoiModele.X2*Cam->dZoom);
											//	RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
											//	RoiTemp.Y1 = (long)( Cam->RoiModele.Y1*Cam->dZoom);
											//	RoiTemp.Y2 = (long)( Cam->RoiModele.Y2*Cam->dZoom);
											//	RoiTemp.dY = RoiTemp.Y2 - RoiTemp.Y1;
											//	if(pPiste->roiSaveToFile.SetROI(&RoiTemp))	
											//	{
											//		MbufExport(sFichier,M_BMP,pPiste->roiSaveToFile.milBuf);
											//	}
											//}
											StringTemp.Format("Robot %d : Modele NC",iPiste);
											theApp.SetStringListVision(StringTemp);
										}
									}
									else
									{
										//Erreur de positionnement
										pPiste->AnalyserMinim = false;
										pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
										pPiste->Result_iDefaut = DEFAUT_AUCUN;
										pPiste->lCountDefVision_Modele++;

										StringTemp.Format("Robot %d : Erreur Modele",iPiste);
										theApp.SetStringListVision(StringTemp);
									}
								}

								//V2.44 : Contrôle des dimensions après rotation
								if(pPiste->AnalyserMinim)
								{
									if(pParamVisionRotation->value == 1)
									{
										ResultRepositionnement = RepositionnerCentre(Cam,pPiste,iPiste,&Cam->ModeleBlister[iPiste]);
										switch(ResultRepositionnement)
										{
											case 0:	//Repositionnement OK
												//Rien a faire
												break;
											case 1:	//Blob rotation non trouvé
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Rotation sans blob",iPiste);
												theApp.SetStringListVision(StringTemp);
												break;

											case 2:	//Distance trop grande de repositionnement
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Repositionnement MAX = %f",iPiste,(float)pPiste->LongueurRepositionnement);
												theApp.SetStringListVision(StringTemp);
												break;

											case 3:	//Erreur de positionnement de la ROI rotation
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Rotation erreur",iPiste);
												theApp.SetStringListVision(StringTemp);
												break;

											case 10:	//LargeurMIN
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Largeur MIN = %f",iPiste,(float)pPiste->LargeurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											case 11:	//LargeurMAX
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Largeur MAX = %f",iPiste,(float)pPiste->LargeurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											case 20:	//LongueurMIN
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Longueur MIN = %f",iPiste,(float)pPiste->LongueurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											case 21:	//LongueurMAX
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Longueur MAX = %f",iPiste,(float)pPiste->LongueurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											default:	//Autre erreur
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;
												StringTemp.Format("Robot %d : Erreur Repositionnement",iPiste);
												theApp.SetStringListVision(StringTemp);
												break;
										}
									}
								}

								//Datamatrix
								if(pPiste->AnalyserMinim)
								{
									if(pPiste->CodeObj.Type == 1)
									{
										int iRet = DataMatrix_Extraire(Cam,pPiste);
										switch(pPiste->CodeObj.BarcodeStatus)
										{
											//V 44.3
											//M_10_0
#if (MIL_COMPILE_VERSION_HEX == 0x1000)
										case M_STATUS_OK:
#else
										case M_STATUS_READ_OK:
#endif
												//AVEC VERIFICATION DU SCORE SEULEMENT SUR MIXUP
												//Comparaison Code Lu/Code Recette
												if(strcmp(pPiste->CodeObj.BarcodeString,theApp.m_Recette.CodeDMXLot) == 0)
												{
													//Code conforme
													//V2.40 : Rien à faire
													//pPiste->Result_iDecision = ROBOT_POSITION_OK;
													//pPiste->Result_iDefaut = DEFAUT_AUCUN;
													//StringTemp.Format("Robot %d : Prise",iPiste);
												}
												else
												{
													//Code non conforme => vérification du score pour MIXUP
													if((pPiste->CodeObj.Score >= (m_RecetteVision.pLong_CodeScoreMin->value/100.0)) && (pPiste->CodeObj.NbErreurs == 0))
													{
														//Taux de lecture suffisant pour etre sur du résultat
														//=>MIXUP
														pPiste->AnalyserMinim = false;
														pPiste->Result_iDecision = ROBOT_POSITION_MELANGE;
														pPiste->Result_iDefaut = DEFAUT_DMX;
														pPiste->lCountDefVision_Codes++;
														StringTemp.Format("Robot %d : Mélange",iPiste);
														theApp.SetStringListVision(StringTemp);
														//V30
														pPiste->AfficherDefaut = true;
														StringTemp= _T("Mélange");
														theApp.SetStringDefautPiste(StringTemp,iPiste);

														MgraText(M_DEFAULT,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf,0,0,pPiste->CodeObj.BarcodeString);

														Cam->bSaveOne = true;
														if(pParamVisionSauvegarde->value == 1)
														{
															strNomFichierImg.Format("%s\\MIXUP %d_%ld.bmp",theApp.m_Recette.RepertoireImages,iPiste,Cam->lNumeroImage);
															sprintf(sFichier,strNomFichierImg);

															MbufExport(sFichier,M_BMP,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf);
														}
													}
													else
													{
														//Taux de lecture insuffisant pour etre sur du résultat
														//=>On laisse passer le blister pour une deuxième lecture
														pPiste->AnalyserMinim = false;
														pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
														pPiste->Result_iDefaut = DEFAUT_AUCUN;
														StringTemp.Format("Robot %d : Code Incertain Sc=%4.2f Err=%ld",iPiste,(float)pPiste->CodeObj.Score,pPiste->CodeObj.NbErreurs);
														theApp.SetStringListVision(StringTemp);
														StringTemp.Format("Code Incertain Sc=%4.2f Err=%ld",(float)pPiste->CodeObj.Score,pPiste->CodeObj.NbErreurs);
														if(pParamVisionSauvegarde->value == 1)
														{
															strNomFichierImg.Format("%s\\DatamatrixIncertain %d_%ld.bmp",theApp.m_Recette.RepertoireImages,iPiste,Cam->lNumeroImage);
															sprintf(sFichier,strNomFichierImg);
															MbufExport(sFichier,M_BMP,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf);
														}
													}
												}
												break;
												
											default:
												//DataMatrix non lu
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_REBUT;
												pPiste->Result_iDefaut = DEFAUT_DMX;
												pPiste->lCountDefVision_Codes++;

												StringTemp.Format("Robot %d : Ejection Code",iPiste);
												theApp.SetStringListVision(StringTemp);
												//V30
												pPiste->AfficherDefaut = true;
												StringTemp = _T("DataMatrix");
												theApp.SetStringDefautPiste(StringTemp,iPiste);

												//V30 : traçage position en rouge
												RoiTemp.X1 = pPiste->proiDMX1.X1;// - pPiste->proiMinim.X1;
												RoiTemp.X2 = pPiste->proiDMX1.X2;// - pPiste->proiMinim.X1;
												RoiTemp.Y1 = pPiste->proiDMX1.Y1;// - pPiste->proiMinim.Y1;
												RoiTemp.Y2 = pPiste->proiDMX1.Y2;// - pPiste->proiMinim.Y1;
												//MgraRect(M_DEFAULT,pPiste->roiResultatDst.milBuf,RoiTemp.X1,RoiTemp.Y1,RoiTemp.X2,RoiTemp.Y2);
												MgraColor(M_DEFAULT,M_RGB888(255,0,0));
												MgraRect(M_DEFAULT,Cam->imgTrackCouleur.milBuf,RoiTemp.X1,RoiTemp.Y1,RoiTemp.X2,RoiTemp.Y2);
												RoiTemp.X1++;// - pPiste->proiMinim.X1;
												RoiTemp.X2--;// - pPiste->proiMinim.X1;
												RoiTemp.Y1++;// - pPiste->proiMinim.Y1;
												RoiTemp.Y2--;// - pPiste->proiMinim.Y1;
												MgraRect(M_DEFAULT,Cam->imgTrackCouleur.milBuf,RoiTemp.X1,RoiTemp.Y1,RoiTemp.X2,RoiTemp.Y2);
												if(iRet == 2)
												{
													//Affichage deuxième zone de lecture
													RoiTemp.X1 = pPiste->proiDMX2.X1;// - pPiste->proiMinim.X1;
													RoiTemp.X2 = pPiste->proiDMX2.X2;// - pPiste->proiMinim.X1;
													RoiTemp.Y1 = pPiste->proiDMX2.Y1;// - pPiste->proiMinim.Y1;
													RoiTemp.Y2 = pPiste->proiDMX2.Y2;// - pPiste->proiMinim.Y1;
													//MgraRect(M_DEFAULT,pPiste->roiResultatDst.milBuf,RoiTemp.X1,RoiTemp.Y1,RoiTemp.X2,RoiTemp.Y2);
													MgraColor(M_DEFAULT,M_RGB888(255,0,0));
													MgraRect(M_DEFAULT,Cam->imgTrackCouleur.milBuf,RoiTemp.X1,RoiTemp.Y1,RoiTemp.X2,RoiTemp.Y2);
													RoiTemp.X1++;// - pPiste->proiMinim.X1;
													RoiTemp.X2--;// - pPiste->proiMinim.X1;
													RoiTemp.Y1++;// - pPiste->proiMinim.Y1;
													RoiTemp.Y2--;// - pPiste->proiMinim.Y1;
													MgraRect(M_DEFAULT,Cam->imgTrackCouleur.milBuf,RoiTemp.X1,RoiTemp.Y1,RoiTemp.X2,RoiTemp.Y2);
												}

												if(pParamVisionSauvegarde->value == 1)
												{
													CString strNomFichierImg;
													strNomFichierImg.Format("%s\\Datamatrix %d_%ld.bmp",theApp.m_Recette.RepertoireImages,iPiste,Cam->lNumeroImage);
													sprintf(sFichier,strNomFichierImg);
													//V2.51 : Sauvegarde image couleur
													//MbufExport(sFichier,M_BMP,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf);
													RoiTemp.X1 = (long)( Cam->RoiModele.X1*Cam->dZoom);
													RoiTemp.X2 = (long)( Cam->RoiModele.X2*Cam->dZoom);
													RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
													RoiTemp.Y1 = (long)( Cam->RoiModele.Y1*Cam->dZoom);
													RoiTemp.Y2 = (long)( Cam->RoiModele.Y2*Cam->dZoom);
													RoiTemp.dY = RoiTemp.Y2 - RoiTemp.Y1;
													if(pPiste->roiSaveToFile.SetROI(&RoiTemp))	
													{
														MbufExport(sFichier,M_BMP,pPiste->roiSaveToFile.milBuf);
													}
												}
												break;
										}//pPiste->CodeObj.BarcodeStatus
									}//Type code
								}//Datamatrix

								//Vérification des taches UV
								//V1.22 : AFAIRE = Vérifier le repositionnement
								if(pPiste->AnalyserMinim)
								{
									if(pParamVisionTracking->value == 0)
									{
										//Position dans l'image pleine résolution
										Pt1.Xpix = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.dX / 2;
										Pt1.Ypix = Cam->InitialBlob.Box_YMin[iBlobSelectionInitial];
										Pt2.Xpix = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.dX / 2;
										Pt2.Ypix = Cam->InitialBlob.Box_YMax[iBlobSelectionInitial];

										pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&Pt1);
										pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&Pt2);

										pPiste->roiDetection[pPiste->IndexLastRoi].Calibration->ToPix(&Pt1);
										pPiste->roiDetection[pPiste->IndexLastRoi].Calibration->ToPix(&Pt2);

										for(i=(int)Pt1.Ypix;i<(int)Pt2.Ypix;i++)
										{
											//Récupération du numéro d'image impacté
											lPosY = (long)i;
											ImageUV = (long)floor(lPosY/(double)Cam->lY);
											LigneUV = lPosY - ImageUV*Cam->lY;

											ImageUV = pPiste->NumeroImageDebut[pPiste->IndexLastRoi] + ImageUV;
											IndexUV = ImageUV%10;

											if(pPiste->PresenceTache[IndexUV][LigneUV]>0)
											{
												//Tache UV
												//V2.40
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_REBUT;
												pPiste->Result_iDefaut = DEFAUT_TACHEUV;
												pPiste->lCountDefVision_TachesUV++;

												StringTemp.Format("Robot %d : Tache UV",iPiste);
												theApp.SetStringListVision(StringTemp);
												//V30
												pPiste->AfficherDefaut = true;
												StringTemp = _T("Tache UV");
												theApp.SetStringDefautPiste(StringTemp,iPiste);

												break;
											}
										}
									}
									else
									{
										//Position dans l'image pleine résolution
										Pt1.Xpix = (long)( Cam->RoiModele.X1*Cam->dZoom);
										Pt2.Xpix = (long)( Cam->RoiModele.X2*Cam->dZoom);
										Pt1.Ypix = (long)( Cam->RoiModele.Y1*Cam->dZoom) - pPiste->roiDetection[pPiste->IndexLastRoi].Roi.Y1;
										Pt2.Ypix = (long)( Cam->RoiModele.Y2*Cam->dZoom) - pPiste->roiDetection[pPiste->IndexLastRoi].Roi.Y1;

										MgraRectFill(M_DEFAULT,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf,0,Pt1.Ypix,10,Pt2.Ypix);

										double MetrageImage = Cam->Date_Image*1000;
										MetrageImage = MetrageImage - Cam->lY * pParamVisionLigne_ms->value * pPiste->Result_IndexEcriture;
										double MetrageDebutMinim = 0;
										double MetrageFinMinim = 0;
										MetrageDebutMinim = MetrageImage + Pt1.Ypix * pParamVisionLigne_ms->value;
										MetrageFinMinim = MetrageImage + Pt2.Ypix * pParamVisionLigne_ms->value;
										bool bDetecte = false;
										bool bDraw = false;
										long Y1 = 0;
										long Y2 = 0;
										if(pParamVisionSelection->value == 1)
										{
											StringTemp.Format("Minim debut %ld",(long)MetrageDebutMinim);
											theApp.SetStringListVision(StringTemp);
											StringTemp.Format("Minim fin   %ld",(long)MetrageFinMinim);
											theApp.SetStringListVision(StringTemp);
										}
										for(i=0;i<TABTACHE;i++)
										{
											bDraw = false;
											if((theCam.Piste[iPiste].MetrageTache[i].MetrageDebut)>(MetrageDebutMinim) && (theCam.Piste[iPiste].MetrageTache[i].MetrageDebut)<(MetrageFinMinim))
											{
												bDetecte = true;
												bDraw = true;
											}
											if((theCam.Piste[iPiste].MetrageTache[i].MetrageFin)>(MetrageDebutMinim) && (theCam.Piste[iPiste].MetrageTache[i].MetrageFin)<(MetrageFinMinim))
											{
												bDetecte = true;
												bDraw = true;
											}
											if(bDraw)
											{
												Y1 =(long)( (theCam.Piste[iPiste].MetrageTache[i].MetrageDebut - MetrageImage) / pParamVisionLigne_ms->value);
												Y2 =(long)( (theCam.Piste[iPiste].MetrageTache[i].MetrageFin - MetrageImage) / pParamVisionLigne_ms->value);
												MgraRectFill(M_DEFAULT,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf,50,Y1,100,Y2);
											}
										}
										if(bDetecte)
										{
											//Tache UV
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_REBUT;
											pPiste->Result_iDefaut = DEFAUT_TACHEUV;
											pPiste->lCountDefVision_TachesUV++;

											StringTemp.Format("Robot %d : Tache UV",iPiste);
											theApp.SetStringListVision(StringTemp);
											//V30
											pPiste->AfficherDefaut = true;
											StringTemp = _T("Tache UV");
											theApp.SetStringDefautPiste(StringTemp,iPiste);
										}
									}
								}//Vérification des taches UV

								//V2.10 : Vérification des taches couleur par comparaison Rouge - Vert (Päramétré)
								if(pPiste->AnalyserMinim)
								{
									ResultTache = TacheCouleur_Extraire(Cam,pPiste,iPiste);
									switch(ResultTache)
									{
										case 0:		//Pas de tache
											//Rien a faire
											break;
										case 1:		//Présence tache
											//V2.40
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_REBUT;
											pPiste->Result_iDefaut = DEFAUT_TACHE;
											pPiste->lCountDefVision_TachesCouleur++;

											StringTemp.Format("Robot %d : Tache Couleur",iPiste);
											theApp.SetStringListVision(StringTemp);
											//V30
											pPiste->AfficherDefaut = true;
											StringTemp = _T("Tache COULEUR");
											theApp.SetStringDefautPiste(StringTemp,iPiste);
											break;

										case 2:		//Erreur détection tache
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											break;
									}
								}//Vérification des taches couleur


								//V2.40
								//Désision finale pour les robots
								if(pPiste->AnalyserMinim)
								{
									//Aucun problème de contrôle (Erreur ou défaut)
									//=>On prend la pièce
									pPiste->Result_iDecision = ROBOT_POSITION_OK;
									pPiste->Result_iDefaut = DEFAUT_AUCUN;
									StringTemp.Format("Robot %d : Prise",iPiste);
									theApp.SetStringListVision(StringTemp);

									if(Cam->imgTrackZoom.milDisp>0)
									{
										Cam->ModeleBlister[iPiste].ColorTracage = 0;
										Cam->ModeleBlister[iPiste].DrawEdges(Cam->ModeleBlister[iPiste].mImg->milBuf,M_ALL,Cam->ModeleBlister[iPiste].ColorTracage);
										MgraColor(M_DEFAULT,0xFF);
										Cam->ModeleBlister[iPiste].DrawROI(Cam->ModeleBlister[iPiste].mImg->milBuf);
									}
								}
								else
								{
									//Limitation du soufflage des rebuts si les minims se chevauche
									if(pPiste->Result_iDecision == ROBOT_POSITION_REBUT)
									{
										if(!pPiste->MinimToutSeul)
										{
											//Les minims se chevauchent => on annule le soufflage
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											StringTemp.Format("Robot %d : Soufflage chevauche",iPiste);
											theApp.SetStringListVision(StringTemp);
										}
									}
								}	

								//Affichage des défauts par piste
								if(pPiste->AfficherDefaut)
								{
									MbufClear(pPiste->roiResultatDst.milBuf,0x00);
									pPiste->proiMinim.X1 = (long)( Cam->RoiModele.X1*Cam->dZoom);
									pPiste->proiMinim.X2 = (long)( Cam->RoiModele.X2*Cam->dZoom);
									pPiste->proiMinim.dX = pPiste->proiMinim.X2 - pPiste->proiMinim.X1;
									pPiste->proiMinim.Y1 = (long)( Cam->RoiModele.Y1*Cam->dZoom);
									pPiste->proiMinim.Y2 = (long)( Cam->RoiModele.Y2*Cam->dZoom);
									pPiste->proiMinim.dY = pPiste->proiMinim.Y2 - pPiste->proiMinim.Y1;
									if(pPiste->roiResultatSrc.SetROI(&pPiste->proiMinim))
									{
										MbufCopyColor(pPiste->roiResultatSrc.milBuf,pPiste->roiResultatDst.milBuf,M_ALL_BAND);
									}
								}

								break;
							
							case RECETTE_TYPESACHET:
								OffsetRobotX = (double)pParamVisionOffsetRobotXSachet->value;

								//IDENTIFICATION PAR LES BLOBS CAS SACHET
								//POSITIONNEMENT DE LA ZONE DE RECHERCHE DU MODELE
								RoiBlob.X1 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X1;
								RoiBlob.X2 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X2;
								RoiBlob.dX = RoiBlob.X2 - RoiBlob.X1;
								RoiBlob.Y1 = pPiste->Result_PositionDebut-10;
								RoiBlob.Y2 = pPiste->Result_PositionFin+10;
								if(RoiBlob.Y1<0)	RoiBlob.Y1 = 0;
								OffsetModeleXpix = RoiBlob.X1;
								OffsetModeleYpix = RoiBlob.Y1;
								if(RoiBlob.Y2>Cam->lYZoomCumul/2)	RoiBlob.Y2 = Cam->lYZoomCumul/2;
								RoiBlob.Y1 += pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1;
								RoiBlob.Y2 += pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1;
								RoiBlob.dY = RoiBlob.Y2 - RoiBlob.Y1;
								
								if(pPiste->roiBlob.SetROI(&RoiBlob))
								{
									//Recherche des blobs
									Cam->InitialBlob.Calculer(pPiste->roiBlob.milBuf); 
									TotalBlobsInitial = Cam->InitialBlob.Selectionner( M_EXCLUDE, M_AREA, M_LESS_OR_EQUAL, 500, M_NULL); 

									//Sélection du plus grand blob
									iBlobSelectionInitial = 0;
									dSurfTemp = 0;
									if(TotalBlobsInitial > 0)
									{
										if(TotalBlobsInitial < NB_MAX_BLOBS_INITIAL)
										{
											Cam->InitialBlob.GetResult(M_AREA); 
											Cam->InitialBlob.GetResult(M_BOX_X_MIN); 
											Cam->InitialBlob.GetResult(M_BOX_X_MAX); 
											Cam->InitialBlob.GetResult(M_BOX_Y_MIN); 
											Cam->InitialBlob.GetResult(M_BOX_Y_MAX); 
											Cam->InitialBlob.GetResult(M_NUMBER_OF_HOLES); 

											for(iBlob=0; iBlob < TotalBlobsInitial; iBlob++)
											{
												if(Cam->InitialBlob.Area[iBlob] > dSurfTemp)
												{
													dSurfTemp = Cam->InitialBlob.Area[iBlob];
													iBlobSelectionInitial = iBlob;
												}
											}
										}
										else
										{
											//Trop de blobs trouvés supérieurs à 500 points, on passe cette image
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											StringTemp.Format("Robot %d : Saturation BlobInitial",iPiste);
											theApp.SetStringListVision(StringTemp);
										}
									}
									else
									{
										//Aucun blob trouvé supérieur à 500 points, on passe cette image
										pPiste->AnalyserMinim = false;
										pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
										pPiste->Result_iDefaut = DEFAUT_AUCUN;
									}
								}
								else
								{
									//Erreur de positionnement
									pPiste->AnalyserMinim = false;
									pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
									pPiste->Result_iDefaut = DEFAUT_AUCUN;
									StringTemp.Format("Robot %d : Erreur ROI BlobInitial",iPiste);
									theApp.SetStringListVision(StringTemp);
								}

								//Vérification de la surface du blob (Pour différencier d'un blister)
								if(pPiste->AnalyserMinim)
								{
									if(m_RecetteVision.pLong_BlobMax->value > 0)
									{
										if(Cam->InitialBlob.Area[iBlobSelectionInitial] > m_RecetteVision.pLong_BlobMax->value)
										{
											//Supérieure à surface MAX =>	NE PAS CONSIDERER
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											pPiste->lCountDefVision_Surface++;//V 43-ALL 

											StringTemp.Format("Robot %d : Surface max = %ld",iPiste,(long)(Cam->InitialBlob.Area[iBlobSelectionInitial]));
											theApp.SetStringListVision(StringTemp);
										}
									}
									if(m_RecetteVision.pLong_BlobMin->value > 0)
									{
										if(Cam->InitialBlob.Area[iBlobSelectionInitial] < m_RecetteVision.pLong_BlobMin->value)
										{
											//Inférieure à surface MIN =>	NE PAS CONSIDERER
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											pPiste->lCountDefVision_Surface++;//V 43-ALL 

											StringTemp.Format("Robot %d : Surface min = %ld",iPiste,(long)(Cam->InitialBlob.Area[iBlobSelectionInitial]));
											theApp.SetStringListVision(StringTemp);
										}
									}
								}

								//Recherche du modele
								if(pPiste->AnalyserMinim)
								{
									//Affectation de la roi selon le blob sélectionné
									Cam->RoiModele.X1 = RoiBlob.X1 + (long)(Cam->InitialBlob.Box_XMin[iBlobSelectionInitial]) - 10;
									if(Cam->RoiModele.X1<0)	Cam->RoiModele.X1 = 0;
									Cam->RoiModele.X2 = RoiBlob.X1 + (long)(Cam->InitialBlob.Box_XMax[iBlobSelectionInitial]) + 10;
									if(Cam->RoiModele.X2>pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X2)	Cam->RoiModele.X2 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.X2;
									Cam->RoiModele.dX = Cam->RoiModele.X2 - Cam->RoiModele.X1;
									Cam->RoiModele.Y1 = RoiBlob.Y1 + (long)(Cam->InitialBlob.Box_YMin[iBlobSelectionInitial]) - 10;
									if(Cam->RoiModele.Y1<pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1)	Cam->RoiModele.Y1 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y1;
									Cam->RoiModele.Y2 = RoiBlob.Y1 + (long)(Cam->InitialBlob.Box_YMax[iBlobSelectionInitial]) + 10;
									if(Cam->RoiModele.Y2>pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y2)	Cam->RoiModele.Y2 = pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.Y2;
									Cam->RoiModele.dY = Cam->RoiModele.Y2 - Cam->RoiModele.Y1;

									OffsetModeleXpix = Cam->RoiModele.X1 - RoiBlob.X1;
									OffsetModeleYpix = Cam->RoiModele.Y1 % pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Roi.dY;

									if(pPiste->roiModele.SetROI(&Cam->RoiModele))
									{
										Cam->ModeleSachet[iPiste].mImg = &pPiste->roiModele;
										if(Cam->ModeleSachet[iPiste].Trouver())
										{
											//Point central de l'objet
											pPiste->PtModele.Xpix = Cam->ModeleSachet[iPiste].ResImgX[0] + OffsetModeleXpix;
											pPiste->PtModele.Ypix = Cam->ModeleSachet[iPiste].ResImgY[0] + OffsetModeleYpix;
											pPiste->PtModele.Tmm = Cam->ModeleSachet[iPiste].ResAngle[0];
											pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&pPiste->PtModele);
											pPiste->PtModele.Zmm = pPiste->PtModele.Tmm - 90;
										}
										else
										{	
											//Modele non trouvé
											pPiste->AnalyserMinim = false;
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											pPiste->lCountDefVision_Modele++;

											StringTemp.Format("Robot %d : Modele NC",iPiste);
											theApp.SetStringListVision(StringTemp);
										}
									}
									else
									{
										//Erreur de positionnement
										pPiste->AnalyserMinim = false;
										pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
										pPiste->Result_iDefaut = DEFAUT_AUCUN;
										pPiste->lCountDefVision_Modele++;

										StringTemp.Format("Robot %d : Erreur Modele",iPiste);
										theApp.SetStringListVision(StringTemp);
									}
								}

								if(pPiste->AnalyserMinim)
								{
									if(pParamVisionRotation->value == 1)
									{
										ResultRepositionnement = RepositionnerCentre(Cam,pPiste,iPiste,&Cam->ModeleSachet[iPiste]);
										switch(ResultRepositionnement)
										{
											case 0:	//Repositionnement OK
												//Rien a faire
												break;
											case 1:	//
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Rotation sans blob",iPiste);
												theApp.SetStringListVision(StringTemp);
												break;

											case 2:	//Distance trop grande de repositionnement
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Repositionnement MAX = %f",iPiste,(float)pPiste->LongueurRepositionnement);
												theApp.SetStringListVision(StringTemp);
												break;

											case 3:	//Erreur de positionnement de la ROI rotation
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Rotation erreur",iPiste);
												theApp.SetStringListVision(StringTemp);
												break;

											case 10:	//LargeurMIN
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Largeur MIN = %f",iPiste,(float)pPiste->LargeurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											case 11:	//LargeurMAX
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Largeur MAX = %f",iPiste,(float)pPiste->LargeurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											case 20:	//LongueurMIN
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Longueur MIN = %f",iPiste,(float)pPiste->LongueurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											case 21:	//LongueurMAX
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Longueur MAX = %f",iPiste,(float)pPiste->LongueurMinim);
												theApp.SetStringListVision(StringTemp);
												break;

											default:	//Autre erreur
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Erreur Repositionnement",iPiste);
												theApp.SetStringListVision(StringTemp);
												break;
										}
									}
								}

								//V2.57 : Rotation de l'objet pour Lecture CodeBarre
								if(pPiste->AnalyserMinim)
								{
									if((m_RecetteVision.pLong_CodeType->value == 2) && (theApp.m_Recette.ArtEmballage.CodeDMX != ""))
									{
										iResultRotation = Rotation_Sachet(Cam,pPiste);
										switch(iResultRotation)
										{
											case 0:			//0 = Rotation OK
												//Rien à faire
												break;

											default:	//Erreur de positionnement des zones après rotation => Ne pas prendre le Minim
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
												pPiste->Result_iDefaut = DEFAUT_AUCUN;
												pPiste->lCountDefVision_Dimensions++;

												StringTemp.Format("Robot %d : Erreur Rotation %d",iPiste,iResultRotation);
												theApp.SetStringListVision(StringTemp);
												break;
										}
									}
								}

								//V2.57 : Extraction code 2of5
								if(pPiste->AnalyserMinim)
								{
									if((m_RecetteVision.pLong_CodeType->value == 2) && (theApp.m_Recette.ArtEmballage.CodeDMX != ""))
									{
										iResult2of5 = Code2of5_Extraire(Cam,pPiste);
										int ModeleNum = atoi(theApp.m_Recette.ArtEmballage.CodeDMX);
										int ResNum = 0;
										bool Vrai_MIXUP = false;
										bool CodeOK = false;
										pPiste->CodeSachetResult = "NC";

										//Vérification si au moins une lecture conforme
										for(int iCB = 0; iCB<CB_NBZONES; iCB++)
										{
											if(pPiste->CodeSachet[iCB].bLectureOK)
											{
												ResNum = atoi(pPiste->CodeSachet[iCB].sCodeLu);
												if(ResNum == ModeleNum)
												{
													CodeOK = true;
													pPiste->CodeSachetResult = pPiste->CodeSachet[iCB].sCodeLu;
												}
											}
										}

										//if(pPiste->CodeSachetResult == theApp.m_Recette.ArtEmballage.CodeDMX)
										//V30 : Comparaison de la valeur numérique
										if(CodeOK)
										{
											//Code conforme, rien à faire
										}
										else
										{
											//Code non conforme => vérification de la réponse
											//Suppression des lectures avec moins de 6 caractères
											int ResLongueur = 0;
											for(int iCB = 0; iCB<CB_NBZONES; iCB++)
											{
												if(pPiste->CodeSachet[iCB].bLectureOK)
												{
													ResLongueur = pPiste->CodeSachet[iCB].sCodeLu.GetLength();
													if(ResLongueur != 6)
													{
														pPiste->CodeSachet[iCB].bLectureOK = false;
													}
												}
											}
											//Suppression des lectures ne commençant pas par '097xxx'
											for(int iCB = 0; iCB<CB_NBZONES; iCB++)
											{
												if(pPiste->CodeSachet[iCB].bLectureOK)
												{
													ResNum = atoi(pPiste->CodeSachet[iCB].sCodeLu);
													if(ResNum > 97999)		pPiste->CodeSachet[iCB].bLectureOK = false;
													if(ResNum < 97000)		pPiste->CodeSachet[iCB].bLectureOK = false;
												}
											}

											//Vérification du nombre restant
											iResult2of5 = 0;
											for(int iCB = 0; iCB<CB_NBZONES; iCB++)
											{
												if(pPiste->CodeSachet[iCB].bLectureOK)
												{
													iResult2of5++;
												}
											}

											//Test MIXUP si reste au moins deux réponses
											if(iResult2of5 >1)
											{
												for(int iCode = 0; iCode<theApp.CB_NbCodes; iCode++)
												{
													theApp.CB_Iterations[iCode] = 0;
													for(int iCB = 0; iCB<CB_NBZONES; iCB++)
													{
														if(pPiste->CodeSachet[iCB].bLectureOK)
														{
															if(pPiste->CodeSachet[iCB].sCodeLu == theApp.CB_ListeCodes[iCode])
															{
																theApp.CB_Iterations[iCode] ++;
															}
														}
													}
												}

												for(int iCode = 0; iCode<theApp.CB_NbCodes; iCode++)
												{
													if(theApp.CB_Iterations[iCode]>0)
													{
														Vrai_MIXUP = true;
														pPiste->CodeSachetResult = theApp.CB_ListeCodes[iCode];
													}
												}
											}

											if(Vrai_MIXUP)
											{
												//Réponse OK
												//=>MIXUP
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_MELANGE;
												pPiste->Result_iDefaut = DEFAUT_DMX;
												pPiste->lCountDefVision_Codes++;
												StringTemp.Format("Robot %d : Mélange ",iPiste);
												StringTemp = StringTemp + pPiste->CodeSachetResult;
												theApp.SetStringListVision(StringTemp);

												MgraText(M_DEFAULT,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf,0,0,pPiste->CodeSachetResult.GetBuffer(pPiste->CodeSachetResult.GetLength()));

												Cam->bSaveOne = true;
												if(pParamVisionSauvegarde->value == 1)
												{
													strNomFichierImg.Format("%s\\MIXUP %d_%ld.bmp",theApp.m_Recette.RepertoireImages,iPiste,Cam->lNumeroImage);
													sprintf(sFichier,strNomFichierImg);
													MbufExport(sFichier,M_BMP,pPiste->roiDetection[pPiste->IndexLastRoi].milBuf);
												}
											}
											else
											{
												//Lecture non conforme
												pPiste->AnalyserMinim = false;
												pPiste->Result_iDecision = ROBOT_POSITION_REBUT;
												pPiste->Result_iDefaut = DEFAUT_CB;
												pPiste->lCountDefVision_Codes++;

												//StringTemp.Format("Robot %d : ErrCB %s",iPiste,pPiste->CodeSachet.sCode);
												StringTemp.Format("Robot %d : CB ",iPiste);
												StringTemp = StringTemp + pPiste->CodeSachetResult;
												theApp.SetStringListVision(StringTemp);
												
												pPiste->AfficherDefaut = true;
												StringTemp= _T("Code barre");
												theApp.SetStringDefautPiste(StringTemp,iPiste);

												if(pParamVisionSauvegarde->value == 1)
												{
													strNomFichierImg.Format("%s\\Code2of5 %d_%ld.bmp",theApp.m_Recette.RepertoireImages,iPiste,Cam->lNumeroImage);
													sprintf(sFichier,strNomFichierImg);
													RoiTemp.X1 = (long)( Cam->RoiModele.X1*Cam->dZoom);
													RoiTemp.X2 = (long)( Cam->RoiModele.X2*Cam->dZoom);
													RoiTemp.dX = RoiTemp.X2 - RoiTemp.X1;
													RoiTemp.Y1 = (long)( Cam->RoiModele.Y1*Cam->dZoom);
													RoiTemp.Y2 = (long)( Cam->RoiModele.Y2*Cam->dZoom);
													RoiTemp.dY = RoiTemp.Y2 - RoiTemp.Y1;
													if(pPiste->roiSaveToFile.SetROI(&RoiTemp))	
													{
														MbufExport(sFichier,M_BMP,pPiste->roiSaveToFile.milBuf);
													}

												}
											}
										}
									}
								}

								//V2.44
								//Désision finale pour les robots
								if(pPiste->AnalyserMinim)
								{
									//Aucun problème de contrôle (Erreur ou défaut)
									//=>On prend la pièce
									pPiste->Result_iDecision = ROBOT_POSITION_OK;
									pPiste->Result_iDefaut = DEFAUT_AUCUN;
									StringTemp.Format("Robot %d : Prise",iPiste);
									theApp.SetStringListVision(StringTemp);

									if(Cam->imgTrackZoom.milDisp>0)
									{
										Cam->ModeleSachet[iPiste].ColorTracage = 0;
										Cam->ModeleSachet[iPiste].DrawEdges(Cam->ModeleSachet[iPiste].mImg->milBuf,M_ALL,Cam->ModeleSachet[iPiste].ColorTracage);
										MgraColor(M_DEFAULT,0xFF);
										Cam->ModeleSachet[iPiste].DrawROI(Cam->ModeleSachet[iPiste].mImg->milBuf);
									}
								}
								else
								{
									//Limitation du soufflage des rebuts si les minims se chevauche
									if(pPiste->Result_iDecision == ROBOT_POSITION_REBUT)
									{
										if(!pPiste->MinimToutSeul)
										{
											//Les minims se chevauchent => on annule le soufflage
											pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
											pPiste->Result_iDefaut = DEFAUT_AUCUN;
											StringTemp.Format("Robot %d : Annulation soufflage",iPiste);
											theApp.SetStringListVision(StringTemp);
										}
									}
								}									
								if(pPiste->AfficherDefaut)
								{
									MbufClear(pPiste->roiResultatDst.milBuf,0x00);
									pPiste->proiMinim.X1 = (long)( Cam->RoiModele.X1*Cam->dZoom);
									pPiste->proiMinim.X2 = (long)( Cam->RoiModele.X2*Cam->dZoom);
									pPiste->proiMinim.dX = pPiste->proiMinim.X2 - pPiste->proiMinim.X1;
									pPiste->proiMinim.Y1 = (long)( Cam->RoiModele.Y1*Cam->dZoom);
									pPiste->proiMinim.Y2 = (long)( Cam->RoiModele.Y2*Cam->dZoom);
									pPiste->proiMinim.dY = pPiste->proiMinim.Y2 - pPiste->proiMinim.Y1;

									MbufCopy(pPiste->roiSachetRotationDst.milBuf,pPiste->roiResultatDst.milBuf);
								}
								break;

						}//TYPE_RECETTE
					}//pPiste->AnalyserMinim

					if(pPiste->Result_iDecision != ROBOT_POSITION_RIEN)
					{
						//Point robot en fonction de la position dans l'image
						pPiste->PtRobot.Xpix = pPiste->PtModele.Xpix;
						pPiste->PtRobot.Ypix = pPiste->PtModele.Ypix;
						pPiste->PtRobot.Tmm = pPiste->PtModele.Tmm;
						pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&pPiste->PtRobot);
						Pt1.Xpix = pPiste->PtRobot.Xpix;
						Pt1.Ypix = (pPiste->Result_IndexEcriture+1) * Cam->lYZoom;
						pPiste->roiDetectionZoom[pPiste->IndexLastRoi].Calibration->ToMM(&Pt1);

						//Repositionnement longitudinal en fonction de l'image en cours
						pPiste->PtRobot.Xmm = pPiste->PtRobot.Xmm - Pt1.Xmm;
						pPiste->PtRobot.Xmm = pPiste->PtRobot.Xmm  + OffsetRobotX;

						pPiste->PtRobot.Zmm = pPiste->PtRobot.Tmm - 90;
						if(pPiste->PtRobot.Zmm > 180)
						{
							pPiste->PtRobot.Zmm = -360 + pPiste->PtRobot.Zmm;
						}
						if(pPiste->PtRobot.Zmm > 90)
						{
							pPiste->PtRobot.Zmm =  pPiste->PtRobot.Zmm - 180;
						}
						if(pPiste->PtRobot.Zmm < -90)
						{
							pPiste->PtRobot.Zmm =  pPiste->PtRobot.Zmm + 180;
						}

						if(pPiste->PtRobot.Zmm > pParamAnglePriseMax->value)
						{
							pPiste->PtRobot.Zmm =  pPiste->PtRobot.Zmm - 180;
						}

						//Limitation de l'angle de prise par les robots
						if(pPiste->Result_iDecision == ROBOT_POSITION_OK)
						{
							if((pPiste->PtRobot.Zmm > pParamAnglePriseMax->value) || (pPiste->PtRobot.Zmm < pParamAnglePriseMin->value))
							{
								pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
								pPiste->lCountDefVision_AnglePrise++;

								StringTemp.Format("Robot %d : Dépassement angle prise ANGLE = %f", iPiste, (float)pPiste->PtRobot.Zmm);//V 43-ALL Affichage valeur Angle
								theApp.SetStringListVision(StringTemp);
							}
							else
							{
								pPiste->lCountVision_OK++; //V 44.8
							}
						}

						//V2.55 : Limitation de la distance entre les Minims pour soufflage
						if(pPiste->Result_iDecision == ROBOT_POSITION_REBUT)
						{
							if(pPiste->DistanceDepuisDernier < m_RecetteVision.pLong_DistMinSoufflage->value)
							{
								//Les minims sont trop près => on annule le soufflage
								pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
								StringTemp.Format("Robot %d : Soufflage distance",iPiste);
								theApp.SetStringListVision(StringTemp);
							}
						}

						//V2.55 : Limitation de l'angle de soufflage
						if(pPiste->Result_iDecision == ROBOT_POSITION_REBUT)
						{
							if(abs(pPiste->PtRobot.Zmm) > m_RecetteVision.pLong_AngleMaxSoufflage->value)
							{
								//Les minims sont trop en biais => on annule le soufflage
								pPiste->Result_iDecision = ROBOT_POSITION_RIEN;
								StringTemp.Format("Robot %d : Soufflage angle",iPiste);
								theApp.SetStringListVision(StringTemp);
							}
						}

						if(pPiste->Result_iDecision != ROBOT_POSITION_RIEN)
						{
							//Envoyer coordonnées
							if(pPiste->m_Robot->bPosSimul == true)
							{
								pPiste->m_Robot->SetPosition(pPiste->m_Robot->PosStatutSimul,(int)(pPiste->PtRobot.Xmm*10.),(int)(pPiste->PtRobot.Ymm*10.),(int)(pPiste->PtRobot.Zmm*10.));
							}
							else
							{
								//pPiste->m_Robot->SetPosition(pPiste->Result_iDecision,(int)(pPiste->PtRobot.Xmm*10.),(int)(pPiste->PtRobot.Ymm*10.),(int)(pPiste->PtRobot.Zmm*10.));
								//V35 : Envoi du détail des défauts
								int iDecision = pPiste->Result_iDecision;
								if(pParamDetailFASTEC->value == 1)
								{
									if(pPiste->Result_iDecision == ROBOT_POSITION_REBUT)
									{
										iDecision = 10 + pPiste->Result_iDefaut;
										//10 = 10 + DEFAUT_AUCUN
										//11 = 10 + DEFAUT_DMX
										//12 = 10 + DEFAUT_TACHE
										//13 = 10 + DEFAUT_CB
										//14 = 10 + DEFAUT_TACHEUV
									}
								}
								pPiste->m_Robot->SetPosition(iDecision,(int)(pPiste->PtRobot.Xmm*10.),(int)(pPiste->PtRobot.Ymm*10.),(int)(pPiste->PtRobot.Zmm*10.));
							}
							pPiste->lNextTop = Cam->lNumeroImage;

							//Mise à jour des compteurs vision
							switch(theApp.Process.iEtat)
							{
								case ETAT_PROCESS_PRODUCTIONDEMARRAGE:
								case ETAT_PROCESS_PRODUCTIONMARCHE:
								case ETAT_PROCESS_ARRETENCOURS:
								case ETAT_PROCESS_VIDAGEMARCHE:
									switch(pPiste->Result_iDefaut)
									{
										case DEFAUT_AUCUN:
											theApp.m_Recette.NbVisionOK[1] = theApp.m_Recette.NbVisionOK[1] + 1;
											break;
										case DEFAUT_DMX:
											theApp.m_Recette.NbVisionDMX[1] = theApp.m_Recette.NbVisionDMX[1] + 1;
											break;
										case DEFAUT_TACHE:
											theApp.m_Recette.NbVisionTache[1] = theApp.m_Recette.NbVisionTache[1] + 1;
											break;
										case DEFAUT_TACHEUV:
											theApp.m_Recette.NbVisionTacheUV[1] = theApp.m_Recette.NbVisionTacheUV[1] + 1;
											break;
										case DEFAUT_CB:
											theApp.m_Recette.NbVisionCB[1] = theApp.m_Recette.NbVisionCB[1] + 1;
											break;
									}//pPiste->Result_iDefaut
									break;
							}//theApp.Process.iEtat
						}//pPiste->Result_iDecision != ROBOT_POSITION_RIEN -> APRES LES DERNIERS CONTROLES (Angle, Soufflage...)
					}// pPiste->Result_iDecision != ROBOT_POSITION_RIEN
				}//Boucle sur les pistes
				break;
		}//Cam->m_iModeInspection

		LastTick = theApp.m_Recette.m_OPCClient->TickOPC;
		
		//V 43-ALL scrolling permanent pour voir l'état du tapis en cours (ce qui a été pris et pas encore...)
		MbufCopy(Cam->ROIScroll[0].milBuf,Cam->imgScrollTemp.milBuf);
		MbufCopy(Cam->imgScrollTemp.milBuf,Cam->ROIScroll[1].milBuf);
		MbufCopy(Cam->imgGrabZoom.milBuf,Cam->ROIScroll[2].milBuf);

		//Affichage
		//Scrolling
		if (Cam->imgScroll.milDisp>0)
		{
			//Affichage des Minims sélectionnés
			for(iPiste=0;iPiste<NBPISTE;iPiste++)
			{
				MgraColor(M_DEFAULT,0xFF);
				if(Cam->Piste[iPiste].Result_iDecision != ROBOT_POSITION_RIEN)
				{
					lTemp = Cam->imgScroll.Roi.dY - ((Cam->Piste[iPiste].Result_IndexEcriture+1) * Cam->lYZoom - (long)Cam->Piste[iPiste].PtRobot.Ypix);
					//V 43-ALL MgraArcFill(M_DEFAULT,Cam->imgScroll.milBuf,(long)Cam->Piste[iPiste].PtRobot.Xpix + Cam->Piste[iPiste].roiBlob.Roi.X1,lTemp,10,10,0,360);
					MgraArc(M_DEFAULT, Cam->imgScroll.milBuf, (long)Cam->Piste[iPiste].PtRobot.Xpix + Cam->Piste[iPiste].roiBlob.Roi.X1, lTemp, 10, 10, 0, 360);
				}
			}
		}

		//Positionnement des pistes
		if(Cam->imgGrabVisu.milDisp > 0)
		{
			MbufCopy(Cam->imgGrab[IndexLoad].milBuf,Cam->imgGrabVisu.milBuf);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste1G->value,0,pParamVisionPiste1G->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste1D->value,0,pParamVisionPiste1D->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste2G->value,0,pParamVisionPiste2G->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste2D->value,0,pParamVisionPiste2D->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste3G->value,0,pParamVisionPiste3G->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste3D->value,0,pParamVisionPiste3D->value,Cam->lY);
		}

		if(Cam->imgRotationZoom.milDisp > 0)
		{
			MgraLine(M_DEFAULT,Cam->imgRotationZoom.milBuf,Cam->imgRotationZoom.Roi.dX/2,0,Cam->imgRotationZoom.Roi.dX/2,Cam->imgRotationZoom.Roi.Y2);
			MgraLine(M_DEFAULT,Cam->imgRotationZoom.milBuf,0,Cam->imgRotationZoom.Roi.dY/2,Cam->imgRotationZoom.Roi.X2,Cam->imgRotationZoom.Roi.dY/2);
		}


		if((Cam->bSave)||(Cam->bSaveOne))
		{
			for(iPiste=0;iPiste<3;iPiste++)
			{
				if(Cam->Piste[iPiste].Result_iDecision != ROBOT_POSITION_RIEN)
				{
					//V17 : Voir intéret

					//strNomFichierImg.Format("%s\\%s\\Track%d_%ld.bmp",theConfig.str_RepertoireData,NOM_DOSSIER_IMAGES,iPiste,Cam->lNumeroImage);
					//sprintf(sFichier,strNomFichierImg);
					//MbufExport(sFichier,M_BMP,Cam->Piste[iPiste].roiDetection[Cam->Piste[iPiste].IndexLastRoi].milBuf);
					//
					//strNomFichierImg.Format("%s\\%s\\TrackZoom%d_%ld.bmp",theConfig.str_RepertoireData,NOM_DOSSIER_IMAGES,iPiste,Cam->lNumeroImage);
					//sprintf(sFichier,strNomFichierImg);
					//MbufExport(sFichier,M_BMP,Cam->imgTrackZoom.milBuf);
				}
			}
			Cam->bSaveOne = false;
		}

		//Fenetre de visualisation camera
		if(Cam->m_Handle)
		{
			SendMessage(Cam->m_Handle,WM_COMMAND,10,0);
		}

		if(Cam->m_iType == 1)	//Cas fichier
			Sleep(10);
	}

	return(1L);
}


unsigned long MFTYPE AcquisitionUV(void *TParam)
{
	CMatroxCameraUV *Cam = (CMatroxCameraUV *) TParam;
	CString StringTemp;
	MIL_INT TotalBlobs = 0;	

	long Position;
	long Destination;
	long NumeroImageMaitre;
	long LigneImageMaitre;
	long HauteurImageMaitre;
	long IndexTableau;

	int IndexEcriture = 0;

	int nFlipFlop = 0;
	int i,iBlob;
	int iPiste;
	int iPisteDef = 0;

	long lTemp;
	bool bDefaut;
	char sFichier[100];
	long lNumeroFichier = 0;
	CString strNomFichierImg;

	HauteurImageMaitre = theCam.lY;

	Cam->lNumeroImageMaitreRef = 0;

	if(Cam->milDig>0)
	{
		MdigControl(Cam->milDig,M_GRAB_MODE,M_ASYNCHRONOUS); 
		MdigControl(Cam->milDig,M_ROTARY_ENCODER_POSITION_TRIGGER,pParamVisionDiviseur->value);
		MdigControl(Cam->milDig,M_ROTARY_ENCODER_POSITION,0);
	}
	 
	while ( Cam->Param_Acquisition.iMode )
	{
		if(Cam->iModeAcquisition == 1)
		{
			do
			{
				Sleep(100);
			}
			while(Cam->iSnap<1);
			Cam->iSnap=0;

			if (Cam->milDig!=0)
			{
				MdigGrab(Cam->milDig, Cam->imgGrab[nFlipFlop].milBuf);
			}
		}
		else
		{
			if (Cam->milDig!=0)
			{
				MdigGrab(Cam->milDig, Cam->imgGrab[nFlipFlop].milBuf);
			}
			Cam->iSnap=0;
		}

		nFlipFlop = 1-nFlipFlop;

		//V23 Vérification des temps
		MappTimer(M_TIMER_READ+M_GLOBAL, &Cam->Date_ImageUV);
		if(pParamVisionSelection->value == 1)
		{
			StringTemp.Format("TempsUV = %ld",(long)(Cam->Date_ImageUV*1000));
			theApp.SetStringListVisionTemps(StringTemp);
		}

		Cam->lNumeroImage ++;
		if(pParamVisionTracking->value == 0)
		{
			if(*Cam->lNumeroImageMaitre != Cam->lNumeroImageMaitreRef)
			{
				Cam->lNumeroImageMaitreRef = *Cam->lNumeroImageMaitre;
				Cam->lNumeroImage = 0;
				lTemp = (Cam->lNumeroImageMaitreRef+5)%10;
				for(i=0;i<HauteurImageMaitre;i++)
				{
					theCam.Piste[0].PresenceTache[lTemp][i] = 0;
					theCam.Piste[1].PresenceTache[lTemp][i] = 0;
					theCam.Piste[2].PresenceTache[lTemp][i] = 0;
				}
			}
		}

		switch(Cam->m_iModeInspection)
		{
			case CAM_MODE_ARRET:
				break;

			case CAM_MODE_INSPECTION:
				if(m_RecetteVision.pLong_UVSeuil->value > 0)
				{
					//V2.30
					//Selection d'un plan pour binarisation
					if(m_RecetteVision.pLong_UVSrc1->value == m_RecetteVision.pLong_UVSrc2->value)
					{
						if(m_RecetteVision.pLong_UVSrc1->value == 0)
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgDiff.milBuf,M_RED);
						if(m_RecetteVision.pLong_UVSrc1->value == 1)
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgDiff.milBuf,M_GREEN);
						if(m_RecetteVision.pLong_UVSrc1->value == 2)
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgDiff.milBuf,M_BLUE);
					}
					else
					{
						if(m_RecetteVision.pLong_UVSrc1->value == 0)	//0+1
						{
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrc1.milBuf,M_RED);
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrcPlus.milBuf,M_GREEN);
						}
						if(m_RecetteVision.pLong_UVSrc1->value == 1)
						{
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrc1.milBuf,M_GREEN);
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrcPlus.milBuf,M_BLUE);
						}
						if(m_RecetteVision.pLong_UVSrc1->value == 2)
						{
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrc1.milBuf,M_BLUE);
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrcPlus.milBuf,M_RED);
						}

						if(m_RecetteVision.pLong_UVSrc2->value == 0)
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrc2.milBuf,M_RED);
						if(m_RecetteVision.pLong_UVSrc2->value == 1)
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrc2.milBuf,M_GREEN);
						if(m_RecetteVision.pLong_UVSrc2->value == 2)
							MbufCopyColor(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgSrc2.milBuf,M_BLUE);

						MimArith(Cam->imgSrc1.milBuf,Cam->imgSrcPlus.milBuf,Cam->imgDiff.milBuf,M_ADD+M_SATURATION);
						MimArith(Cam->imgDiff.milBuf,Cam->imgSrc2.milBuf,Cam->imgDiff.milBuf,M_SUB+M_SATURATION);
					}

					MimConvolve(Cam->imgDiff.milBuf,Cam->imgDiff.milBuf,M_SMOOTH);
					MimBinarize(Cam->imgDiff.milBuf,Cam->imgBin.milBuf,M_GREATER,m_RecetteVision.pLong_UVSeuil->value,M_NULL);
					MimDilate(Cam->imgBin.milBuf,Cam->imgBin.milBuf,1,M_BINARY);

					//Recherche des défauts
					for(iPiste=0;iPiste<NBPISTE;iPiste++)
					{
						bDefaut = false;
						//Recherche des blobs
						Cam->BlobUV.Calculer(Cam->roiBin[iPiste].milBuf); 
						TotalBlobs = Cam->BlobUV.Selectionner( M_EXCLUDE, M_AREA, M_LESS, m_RecetteVision.pLong_UVSurfMin->value, M_NULL); 

						//Vérification que la surface n'exède pas la surface max d'un blister (Toute face)
						//Présence de tâche
						if(TotalBlobs>=500)	
						{
							bDefaut = true;
							TotalBlobs = 0;
							iPisteDef = iPiste;
							//V2.51 : Marquage de toute la zone
							if(pParamVisionTracking->value == 0)
							{
								for(i=0;i<Cam->lY;i++)
								{
									//Position par rapport à l'origine
									Position = Cam->lNumeroImage * Cam->lY + i;
									Destination = Position + pParamVisionDistance_UV->value;
									NumeroImageMaitre = (long)(floor(Destination/(double)HauteurImageMaitre));
									LigneImageMaitre = Destination - NumeroImageMaitre*HauteurImageMaitre;

									NumeroImageMaitre = Cam->lNumeroImageMaitreRef + NumeroImageMaitre;
									IndexTableau = NumeroImageMaitre%10;
									theCam.Piste[iPiste].PresenceTache[IndexTableau][LigneImageMaitre] = 1;
								}
							}
							else
							{
								theCam.Piste[iPiste].MetrageTache[IndexEcriture].MetrageDebut = Cam->Date_ImageUV*1000;
								theCam.Piste[iPiste].MetrageTache[IndexEcriture].MetrageFin = Cam->Date_ImageUV*1000 + Cam->lY * pParamVisionLigne_ms->value;
								IndexEcriture = (IndexEcriture+1)%TABTACHE;
							}
						}
						
						if(TotalBlobs>0)
						{
							bDefaut = true;
							iPisteDef = iPiste;
							Cam->BlobUV.GetResult(M_BOX_Y_MIN);
							Cam->BlobUV.GetResult(M_BOX_Y_MAX);
							Cam->BlobUV.GetResult(M_BOX_X_MIN);
							Cam->BlobUV.GetResult(M_BOX_X_MAX);
							if(Cam->imgBin.milDisp>0)
							{
								for(iBlob=0; iBlob < TotalBlobs; iBlob++)
								{
									MgraRect(M_DEFAULT,Cam->imgBin.milBuf,
										Cam->BlobUV.Box_XMin[iBlob] + Cam->roiBin[iPiste].Roi.X1,
										Cam->BlobUV.Box_YMin[iBlob],
										Cam->BlobUV.Box_XMax[iBlob] + Cam->roiBin[iPiste].Roi.X1,
										Cam->BlobUV.Box_YMax[iBlob]);
								}
							}
							if(pParamVisionSelection->value == 1)
							{
								StringTemp.Format("UV %d",iPiste);
								theApp.SetStringListVision(StringTemp);
							}
						}
						if(pParamVisionTracking->value == 0)
						{
							for(iBlob=0; iBlob < TotalBlobs; iBlob++)
							{
								for(i= (int)Cam->BlobUV.Box_YMin[iBlob];i < (int)Cam->BlobUV.Box_YMax[iBlob];i++)
								{
									//Position par rapport à l'origine
									Position = Cam->lNumeroImage * Cam->lY + i;
									Destination = Position + pParamVisionDistance_UV->value;
									NumeroImageMaitre = (long)(floor(Destination/(double)HauteurImageMaitre));
									LigneImageMaitre = Destination - NumeroImageMaitre*HauteurImageMaitre;

									NumeroImageMaitre = Cam->lNumeroImageMaitreRef + NumeroImageMaitre;
									IndexTableau = NumeroImageMaitre%10;
									theCam.Piste[iPiste].PresenceTache[IndexTableau][LigneImageMaitre] = 1;
								}
							}
						}
						else
						{
							if(bDefaut)
							{
								double lLigneDebut = (double)Cam->lY;
								double lLigneFin = 0;
								for(iBlob=0; iBlob < TotalBlobs; iBlob++)
								{
									if(Cam->BlobUV.Box_YMin[iBlob]<lLigneDebut)	lLigneDebut = Cam->BlobUV.Box_YMin[iBlob];
									if(Cam->BlobUV.Box_YMax[iBlob]>lLigneFin)	lLigneFin = Cam->BlobUV.Box_YMax[iBlob];
								}
								if(lLigneDebut < lLigneFin)
								{
									theCam.Piste[iPiste].MetrageTache[IndexEcriture].MetrageDebut = Cam->Date_ImageUV*1000 + lLigneDebut*pParamVisionLigne_ms->value + pParamVisionTemps_ms->value;
									theCam.Piste[iPiste].MetrageTache[IndexEcriture].MetrageFin = Cam->Date_ImageUV*1000 + lLigneFin*pParamVisionLigne_ms->value + pParamVisionTemps_ms->value;;
									StringTemp.Format("UV debut %ld",(long)theCam.Piste[iPiste].MetrageTache[IndexEcriture].MetrageDebut);
									theApp.SetStringListVision(StringTemp);
									StringTemp.Format("UV fin   %ld",(long)theCam.Piste[iPiste].MetrageTache[IndexEcriture].MetrageFin);
									theApp.SetStringListVision(StringTemp);
									IndexEcriture = (IndexEcriture+1)%TABTACHE;
								}
							}
						}

						//V21 : enregistrement par piste
						if(bDefaut)
						{
							if(pParamVisionSauvegarde->value == 1)
							{
								lNumeroFichier++;
								strNomFichierImg.Format("%s\\TacheUV %d_%ld.bmp",theApp.m_Recette.RepertoireImages,iPisteDef,lNumeroFichier);
								sprintf(sFichier,strNomFichierImg);
								MbufExport(sFichier,M_BMP,Cam->roiSrc[nFlipFlop][iPisteDef].milBuf);
							}
						}
					}
				}
				break;
		}
		
		

		//Positionnement des pistes
		if(Cam->imgGrabVisu.milDisp > 0)
		{
			MbufCopy(Cam->imgGrab[nFlipFlop].milBuf,Cam->imgGrabVisu.milBuf);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste1G_UV->value,0,pParamVisionPiste1G_UV->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste1D_UV->value,0,pParamVisionPiste1D_UV->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste2G_UV->value,0,pParamVisionPiste2G_UV->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste2D_UV->value,0,pParamVisionPiste2D_UV->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste3G_UV->value,0,pParamVisionPiste3G_UV->value,Cam->lY);
			MgraLine(M_DEFAULT,Cam->imgGrabVisu.milBuf,pParamVisionPiste3D_UV->value,0,pParamVisionPiste3D_UV->value,Cam->lY);
		}

		//if((Cam->bSave)||(Cam->bSaveOne))
		//{
		//	for(iPiste=0;iPiste<3;iPiste++)
		//	{
		//		if(Cam->Piste[iPiste].Result_iDecision != ROBOT_POSITION_RIEN)
		//		{
		//			sprintf(sFichier,"C:\\ACYBL\\Images\\Track%d_%ld.bmp",iPiste,Cam->lNumeroImage);
		//			MbufExport(sFichier,M_BMP,Cam->Piste[iPiste].roiDetection[Cam->Piste[iPiste].IndexLastRoi].milBuf);
		//			//sprintf(sFichier,"C:\\ACYBL\\Images\\TrackZoom%ld.bmp",Cam->lNumeroImage);
		//			//MbufExport(sFichier,M_BMP,Cam->imgTrackZoom.milBuf);
		//		}
		//	}
		//	Cam->bSaveOne = false;
		//}

		if(Cam->m_iType == 1)	//Cas fichier
			Sleep(10);
	}

	return(1L);
}

void TracerCroix(MIL_ID Image,STRUCT_POINT * PtCentre/*Px*/,int iLargeur,COLORREF Color)
{
	int iLargeurCroix = iLargeur;

	STRUCT_POINT PtTemp1,PtTemp2;

	PtTemp1.Xpix = PtCentre->Xpix - iLargeurCroix;
	PtTemp1.Ypix = PtCentre->Ypix ;
	PtTemp2.Xpix = PtCentre->Xpix + iLargeurCroix;
	PtTemp2.Ypix = PtCentre->Ypix ;

	MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MgraLine(M_DEFAULT,Image,PtTemp1.Xpix,PtTemp1.Ypix,PtTemp2.Xpix,PtTemp2.Ypix);

	PtTemp1.Xpix = PtCentre->Xpix ;
	PtTemp1.Ypix = PtCentre->Ypix - iLargeurCroix ;
	PtTemp2.Xpix = PtCentre->Xpix ;
	PtTemp2.Ypix = PtCentre->Ypix + iLargeurCroix;

	MgraLine(M_DEFAULT,Image,PtTemp1.Xpix,PtTemp1.Ypix,PtTemp2.Xpix,PtTemp2.Ypix);

}

unsigned long MFTYPE AcquisitionUeye(void *TParam)
{
	CMatroxCameraUeye *Cam = (CMatroxCameraUeye *) TParam;

	//boucle acquisition
	while ( Cam->Param_Acquisition.iMode )
	{
		//if(Cam->Temps_Calcul != NULL) MappTimer(M_TIMER_READ+M_GLOBAL,(MIL_DOUBLE *)( &Cam->Temps_Calcul->value));
		
		if(Cam->iModeAcquisition == 1)
		{
			do
			{
				Sleep(10);
			}
			while(Cam->iSnap<1);
		}
	
		if (Cam->iSnap == 2)
		{
			//copie de m_pcImageMemory[id_Cam] -> ImgSrc->milBuf
			if(Cam->m_nBitsPerPixel > 8)
			{
				MbufPutColor2d(Cam->imgGrabuEye.milBuf ,M_PACKED + M_BGR24 ,M_ALL_BANDS,0,0,Cam->lX,Cam->lY, Cam->m_pcImageMemory);
			}
			else
			{
				//MbufPutColor2d(Cam->imgGrab.milBuf ,M_PLANAR ,M_ALL_BANDS,0,0,Cam->lX,Cam->lY, Cam->m_pcImageMemory);
				MbufPut2d(Cam->imgGrabuEye.milBuf, 0, 0, Cam->lX, Cam->lY, Cam->m_pcImageMemory);
			}
		}

		Cam->iSnap=0;
		
		//Vérification des temps
		//if(Cam->Temps_Image != NULL) MappTimer(M_TIMER_READ+M_GLOBAL, &Cam->Temps_Image->value);

		if(Cam->bFromFile)
		{
			if(Cam->iIndexSeqFile < 0)
			{
				if (Cam->imgGrabuEye.milBuf > 0)
				{
					MbufLoad(Cam->m_sFichier,Cam->imgGrabuEye.milBuf);
				}
			}
			else
			{
				Cam->m_sFichier = Cam->StrFileSeq.GetAt(Cam->iIndexSeqFile);
				if (Cam->imgGrabuEye.milBuf > 0)
				{
					MbufLoad(Cam->m_sFichier,Cam->imgGrabuEye.milBuf);
				}

				Cam->iIndexSeqFile ++;
				if(Cam->iIndexSeqFile == Cam->StrFileSeq.GetCount())
				{
					Cam->iIndexSeqFile = 0;
				}
			}
		}

		//RAZ Timer
		//MappTimer(M_TIMER_RESET+M_GLOBAL, M_NULL);	!!! Utilisé dans le tracking

		Cam->lNumeroImage ++;

		switch(Cam->m_iModeInspection)
		{
			case CAM_MODE_ARRET:
				break;

			case CAM_MODE_INSPECTION:
				theApp.VisionBoite.Vision_OnSnapEye(Cam->m_iNumuEye);
				break;

		}

		if((Cam->bSave)||(Cam->bSaveOne))
		{
			CString strNomFichierImg;
			strNomFichierImg.Format("%s\\%s\\Etiquette_%ld_%d.bmp",theConfig.str_RepertoireData,NOM_DOSSIER_IMAGES,Cam->lNumeroImage,Cam->m_iNumuEye);
			if (Cam->imgGrabuEye.milBuf > 0)
			{
				MbufExport(strNomFichierImg.GetBuffer(),M_BMP,Cam->imgGrabuEye.milBuf);
				strNomFichierImg.ReleaseBuffer();
			}
			
			Cam->bSaveOne = false;
		}

		if(Cam->m_iType == 1)//Cas uEye non connectée => fichier
			Sleep(100);
		if((Cam->m_iType == 3)&&(Cam->m_hCam == 0))//Cas uEye non connectée => fichier
			Sleep(100);
	}

	return(1L);
}

void TracerROI(CMImage * Img,PARAM_ROI	* Roi,COLORREF Color)
{
	if(Img->milBuf > 0)
	{
		MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
		STRUCT_POINT Pt1 = {0},Pt2 = {0};

		Pt1.Xpix = Roi->X1;
		Pt1.Ypix = Roi->Y1;

		Pt2.Xpix = Roi->X1 + Roi->dX;
		Pt2.Ypix = Roi->Y1;

		MgraLine(M_DEFAULT,Img->milBuf,Pt1.Xpix,Pt1.Ypix,Pt2.Xpix,Pt2.Ypix);

		Pt1.Xpix = Roi->X1 + Roi->dX;
		Pt1.Ypix = Roi->Y1 + Roi->dY;

		MgraLine(M_DEFAULT,Img->milBuf,Pt1.Xpix,Pt1.Ypix,Pt2.Xpix,Pt2.Ypix);

		Pt2.Xpix = Roi->X1 ;
		Pt2.Ypix = Roi->Y1 + Roi->dY;

		MgraLine(M_DEFAULT,Img->milBuf,Pt1.Xpix,Pt1.Ypix,Pt2.Xpix,Pt2.Ypix);

		Pt1.Xpix = Roi->X1;
		Pt1.Ypix = Roi->Y1;

		MgraLine(M_DEFAULT,Img->milBuf,Pt1.Xpix,Pt1.Ypix,Pt2.Xpix,Pt2.Ypix);
	}

}

void TracerLigne(CMImage * Img,int iPosition, bool bHrz,COLORREF Color)
{
	if(Img->milBuf > 0)
	{
		MgraColor(M_DEFAULT,M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
		STRUCT_POINT Pt1 = {0},Pt2 = {0};

		if (bHrz)
		{
			Pt1.Xpix = Img->Roi.X1;
			Pt1.Ypix = iPosition;

			Pt2.Xpix = Img->Roi.X1 + Img->Roi.dX;
			Pt2.Ypix = iPosition;
		}
		else
		{
			Pt1.Xpix = iPosition;
			Pt1.Ypix = Img->Roi.Y1;
			
			Pt2.Xpix = iPosition;
			Pt2.Ypix = Img->Roi.Y1 + Img->Roi.dY;
		}

		MgraLine(M_DEFAULT,Img->milBuf,Pt1.Xpix,Pt1.Ypix,Pt2.Xpix,Pt2.Ypix);
	}

}

//V 43-ALL
void Vision_HistoriqueTapis_Afficher()
{
	if (theCam.imgScroll.IsFenetreOpen == true)
	{
		theCam.imgScroll.FenetreFermer();
		return;
	}

	theCam.imgScroll.FenetreOuvrir();

	if (theCam.imgScroll.milDisp > 0)
	{
		//Affichage de la position robot de chaque piste
		for (int iPiste = 0;iPiste<NBPISTE;iPiste++)
		{
			STRUCT_POINT Pt1 = { 0 }, Pt2 = { 0 };
			switch (iPiste)
			{
			case 0:
			default:
				Pt1.Xpix = (double)pParamVisionPiste1G->value / theCam.dZoom;
				Pt2.Xpix = (double)pParamVisionPiste1D->value / theCam.dZoom;

				Pt1.Xmm += pParamTapis_DistanceCam_Robot1->value * 1000;
				Pt2.Xmm += pParamTapis_DistanceCam_Robot1->value * 1000;

				Pt1.Ypix = ((theCam.imgScroll.Roi.dY - 1) - ((pParamTapis_DistanceCam_Robot1->value * 1000) / theCam.dPixelSizeY) / theCam.dZoom);
				Pt2.Ypix = ((theCam.imgScroll.Roi.dY - 1) - ((pParamTapis_DistanceCam_Robot1->value * 1000) / theCam.dPixelSizeY) / theCam.dZoom);

				break;

			case 1:
				Pt1.Xpix = (double)pParamVisionPiste2G->value / theCam.dZoom;
				Pt2.Xpix = (double)pParamVisionPiste2D->value / theCam.dZoom;

				Pt1.Xmm += pParamTapis_DistanceCam_Robot2->value * 1000;
				Pt2.Xmm += pParamTapis_DistanceCam_Robot2->value * 1000;

				Pt1.Ypix = ((theCam.imgScroll.Roi.dY - 1) - ((pParamTapis_DistanceCam_Robot2->value * 1000) / theCam.dPixelSizeY) / theCam.dZoom);
				Pt2.Ypix = ((theCam.imgScroll.Roi.dY - 1) - ((pParamTapis_DistanceCam_Robot2->value * 1000) / theCam.dPixelSizeY) / theCam.dZoom);
				
				break;

			case 2:
				Pt1.Xpix = (double)pParamVisionPiste3G->value / theCam.dZoom;
				Pt2.Xpix = (double)pParamVisionPiste3D->value / theCam.dZoom;

				Pt1.Xmm += pParamTapis_DistanceCam_Robot3->value * 1000;
				Pt2.Xmm += pParamTapis_DistanceCam_Robot3->value * 1000;

				Pt1.Ypix = ((theCam.imgScroll.Roi.dY - 1) -((pParamTapis_DistanceCam_Robot3->value * 1000) / theCam.dPixelSizeY) / theCam.dZoom);
				Pt2.Ypix = ((theCam.imgScroll.Roi.dY - 1) - ((pParamTapis_DistanceCam_Robot3->value * 1000) / theCam.dPixelSizeY) / theCam.dZoom);
				break;
			}

			/*Pt1.Xpix = Pt1.Xpix / theCam.dZoom;
			Pt1.Ypix = Pt1.Ypix / theCam.dZoom;
			Pt2.Xpix = Pt2.Xpix / theCam.dZoom;
			Pt2.Ypix = Pt2.Ypix / theCam.dZoom;*/

			MgraColor(M_DEFAULT, 0xFF);
			MgraLine(M_DEFAULT, theCam.imgScroll.milBuf,Pt1.Xpix, Pt1.Ypix,Pt2.Xpix, Pt2.Ypix);
			
			/*if (Cam->Piste[iPiste].Result_iDecision != ROBOT_POSITION_RIEN)
			{
				lTemp = Cam->imgScroll.Roi.dY - ((Cam->Piste[iPiste].Result_IndexEcriture + 1) * Cam->lYZoom - (long)Cam->Piste[iPiste].PtRobot.Ypix);
				MgraArcFill(M_DEFAULT, Cam->imgScroll.milBuf, (long)Cam->Piste[iPiste].PtRobot.Xpix + Cam->Piste[iPiste].roiBlob.Roi.X1, lTemp, 10, 10, 0, 360);
			}*/
		}
	}
}

//V 43-ALL
void Vision_HistoriqueTapis_Exporter()
{
	//L'image doit etre affichée pour etre exportée (sinon il n'y a pas les tracages dessus)
	if (theCam.imgScroll.milDisp > 0)
	{
		Vision_HistoriqueTapis_Afficher();
	}

	//Selection du fichier pour export
	CFileDialog* file_dlg = new CFileDialog(FALSE);
	file_dlg->m_ofn.lpstrTitle = _T("Sauvegarde Fichier Historique Tapis");
	file_dlg->m_ofn.lpstrFilter = _T("Fichiers Images (*.bmp)\0*.bmp\0Tous les Fichier (*.*)\0*.*\0\0");

	INT_PTR resultdlg = file_dlg->DoModal();

	if (resultdlg != IDOK)
	{
		delete (file_dlg);
		return;
	}
	
	CString strFileToCopy = file_dlg->GetPathName();

	if (strFileToCopy.Right(4) != _T(".bmp"))
	{
		strFileToCopy.Append(_T(".bmp"));
	}

	MbufExport(strFileToCopy,M_BMP, theCam.imgScroll.milBuf);

}

bool ModeleExtraction_Creer(PARAM_ROI * RoiModele, CString strNomFichierModele)
{
	CModele NewModele;
	CMImage ImageNewModeleGrey;

	NewModele.MF_System = &Systeme[0].milSys;

	ImageNewModeleGrey.sLibelle = _T("Image Creation modele");
	ImageNewModeleGrey.Charger(Systeme[0].milSys, RoiModele->dX, RoiModele->dY, 1, 8 + M_UNSIGNED, M_IMAGE + M_PROC + M_DISP);

	MbufCopyColor2d(((CDialEtiquette *)theApp.EtiqView)->imgVisuSelectRoi.milBuf, ImageNewModeleGrey.milBuf,
		M_LUMINANCE, RoiModele->X1, RoiModele->Y1, M_ALL_BANDS,
		0,
		0,
		RoiModele->dX,
		RoiModele->dY);

	NewModele.CreateFromImage(ImageNewModeleGrey.milBuf, 0, 0, ImageNewModeleGrey.Roi.dX, ImageNewModeleGrey.Roi.dY);

	NewModele.strFile = strNomFichierModele;
	NewModele.SaveToFile();

	NewModele.Decharger();
	ImageNewModeleGrey.Decharger();
	return true;
}

