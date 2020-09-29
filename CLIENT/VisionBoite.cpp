#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialEtiquette.h"
#include "General.h"
#include "AC_Base.h"
#include "VisionBoite.h"


CVisionBoite::CVisionBoite(void)
{
	oldBitSnap = 0;
	bSnapOK[0] = false;
	bSnapOK[1] = false;
	bSnapAttente = false;
}

CVisionBoite::~CVisionBoite(void)
{
}

void CVisionBoite::InitOF(void)
{
//Machine
	//V37
	//if (pParamForcageInhibitionEtiquette->value == TRUE)
	//{
	//	*pBitInhibitionLectureBoite = TRUE;
	//}
	//else
	//{
	//	*pBitInhibitionLectureBoite = FALSE;
	//}

	//V 44.0 
	/*if (pParamLectureBoite->value == TRUE)
	{
		*pBitInhibitionLectureBoite = FALSE;
	}
	else
	{
		*pBitInhibitionLectureBoite = TRUE;
	}*/

	*pBitResAcquisition_OK = FALSE;
	*pBitResAcquisition_NOK = FALSE;
	*pBitMixup = FALSE;

//Caméras
	bSnapOK[0] = false;
	bSnapOK[1] = false;
	bSnapAttente = false;
}

void CVisionBoite::ChargerImageTest(CString sFile)
{
	MIL_TEXT_PTR mFile = NULL;
	mFile = sFile.GetBuffer(sFile.GetLength());
	MbufLoad(mFile,imgSource.milBuf);
	//V37 : Affichage
	MbufCopy(imgSource.milBuf,imgVisu.milBuf);
	if (((CDialEtiquette *)theApp.EtiqView)->IsWindowVisible())
	{
		((CDialEtiquette *)theApp.EtiqView)->RefreshImage();
	}
}

bool CVisionBoite::Vision_Charger(MIL_ID idSystem)
{
	CString sTemp;

	milSys = idSystem;
	//lX = 1600;		//V42 TODO a parametrer
	//lY = 2400;		//V42 TODO a parametrer
	lX = uEye[1].lX;		//V42 TODO a parametrer
	lY = uEye[1].lY * 2;		//V42 TODO a parametrer

	imgSource.sLibelle.Format(_T("Etiq Source"));
	imgSource.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
	
	PARAM_ROI roiTemp;
	roiTemp.X1 = 0;
	roiTemp.Y1 = 0;
	roiTemp.dX = lX;
	roiTemp.dY = lY/2;
	imgSource_roiCam[0].sLibelle = _T("Etiq imgSource_roiCam 0");
	imgSource_roiCam[0].Charger(&imgSource);
	imgSource_roiCam[0].SetROI(&roiTemp);

	roiTemp.X1 = 0;
	roiTemp.Y1 = lY/2;
	roiTemp.dX = lX;
	roiTemp.dY = lY/2;
	imgSource_roiCam[1].sLibelle = _T("Etiq imgSource_roiCam 1");
	imgSource_roiCam[1].Charger(&imgSource);
	imgSource_roiCam[1].SetROI(&roiTemp);

	imgVisu.sLibelle.Format(_T("Etiq Visu"));
	imgVisu.Charger(milSys,lX,lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//V37
	////Initialisation des extractions
	//imgTrait.sLibelle.Format(_T("Etiq Traitement"));
	//imgTrait.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//V37
	////Initialisation des extractions
	////V48 : image rotation carrée, au maximum de X ou Y
	//long MaxSize = lX;
	//if(lY>lX)	MaxSize = lY;
	//imgRotation.sLibelle.Format(_T("Etiq Traitement Rotation"));
	//imgRotation.Charger(milSys,MaxSize,MaxSize,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//V 44.8
	Graph_Visu.MF_System = &milSys;
	Graph_Visu.IRepere = M_PIXEL;//V 2.3
	Graph_Visu.Charger();


	return true;
}

void CVisionBoite::Vision_OnSnapEye(int iNumCam)
{
	if(UEye_ImgRotation[iNumCam]->value == 0)
	{
		MbufCopy(uEye[iNumCam].imgGrabuEye.milBuf, imgSource_roiCam[iNumCam].milBuf);
	}
	else
	{
		MimRotate(uEye[iNumCam].imgGrabuEye.milBuf,imgSource_roiCam[iNumCam].milBuf,(double)UEye_ImgRotation[iNumCam]->value,uEye[iNumCam].imgGrabuEye.Roi.dX/2,uEye[iNumCam].imgGrabuEye.Roi.dY/2,imgSource_roiCam[iNumCam].Roi.dX/2,imgSource_roiCam[iNumCam].Roi.dY/2,M_DEFAULT);
	}
	CString strForListe=_T("");
	strForListe.Format("Reception image camera %d",iNumCam);
	((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strForListe);

	if(bSnapAttente)
	{
		bSnapOK[iNumCam] = true;
		
		if(bSnapOK[0] && bSnapOK[1])
		{
			bSnapAttente = false;

			//V37 Vision_DoExtraction();
			MbufCopy(imgSource.milBuf,imgVisu.milBuf);

			//V37 Execution des traitements sur les deux extractions
			theApp.EtiqScellement.iResultatEtiquette = DEFAUT_ETIQ_AUCUN;
			//V 44.0 if(!theApp.EtiqScellement.bInhibLecture)
			if (!*theApp.EtiqScellement.pBitInhibLecture)
				theApp.EtiqScellement.Vision_DoExtraction();
			
			theApp.EtiqWolke.iResultatEtiquette = DEFAUT_ETIQ_AUCUN;
			//V 44.0 if(!theApp.EtiqWolke.bInhibLecture)
			if (!*theApp.EtiqWolke.pBitInhibLecture)
				theApp.EtiqWolke.Vision_DoExtraction();

			//V37 : Affichage
			if (((CDialEtiquette *)theApp.EtiqView)->IsWindowVisible())
			{
				((CDialEtiquette *)theApp.EtiqView)->RefreshImage();
			}

			//V37TODO : !!! Gérer le cumul des résultats !!!
			//Forcage Sorties pour test avec Automate
			if(theApp.EtiqScellement.iForcageResult < 0)
			{
				if((theApp.EtiqScellement.iResultatEtiquette == DEFAUT_ETIQ_AUCUN) && (theApp.EtiqWolke.iResultatEtiquette == DEFAUT_ETIQ_AUCUN))
				{
					*pBitResAcquisition_OK = TRUE;
					*pBitResAcquisition_NOK = FALSE;
					*pBitMixup = FALSE;
				}
				else if((theApp.EtiqScellement.iResultatEtiquette == DEFAUT_ETIQ_MIXUP) || (theApp.EtiqWolke.iResultatEtiquette == DEFAUT_ETIQ_MIXUP))
				{
					*pBitResAcquisition_OK = FALSE;
					*pBitResAcquisition_NOK = FALSE;
					*pBitMixup = TRUE;
				}
				else
				{
					*pBitResAcquisition_OK = FALSE;
					*pBitResAcquisition_NOK = TRUE;
					*pBitMixup = FALSE;
				}

				//switch(theApp.EtiqScellement.iResultatEtiquette)
				//{
				//	case DEFAUT_ETIQ_AUCUN:
				//		*pBitResAcquisition_OK = TRUE;
				//		*pBitResAcquisition_NOK = FALSE;
				//		*pBitMixup = FALSE;
				//		break;

				//	case DEFAUT_ETIQ_EXTRACT:
				//		*pBitResAcquisition_OK = FALSE;
				//		*pBitResAcquisition_NOK = TRUE;
				//		*pBitMixup = FALSE;
				//		break;

				//	case DEFAUT_ETIQ_MIXUP:
				//		*pBitResAcquisition_OK = FALSE;
				//		*pBitResAcquisition_NOK = FALSE;
				//		*pBitMixup = TRUE;
				//		break;
				//}
			}
			else
			{
				switch(theApp.EtiqScellement.iForcageResult)
				{
					case 0:
						*pBitResAcquisition_OK = TRUE;
						*pBitResAcquisition_NOK = FALSE;
						*pBitMixup = FALSE;
						break;

					case 1:
						*pBitResAcquisition_OK = FALSE;
						*pBitResAcquisition_NOK = TRUE;
						*pBitMixup = FALSE;
						break;

					case 2:
						*pBitResAcquisition_OK = FALSE;
						*pBitResAcquisition_NOK = FALSE;
						*pBitMixup = TRUE;
						break;
				}
			}

			//V37 Enregistrement des images
			if(theConfig.str_Machine.Right(5) != "SIMUL")
			{
				if(*pBitResAcquisition_OK != TRUE)
				{
					CString strNomFichierImg;
					strNomFichierImg.Format("%s\\Etiquette %ld.bmp",theApp.m_Recette.RepertoireImages,uEye[0].lNumeroImage);
					MbufExport(strNomFichierImg.GetBuffer(),M_BMP,imgSource.milBuf);
					strNomFichierImg.ReleaseBuffer();
				}
			}

		}
	}
}

void CVisionBoite::Process_BitSnap()
{
	//Inhibition gérée coté automate
	bSnapAttente = true;

	//RAZ des resultats
	*pBitResAcquisition_OK = FALSE;
	*pBitResAcquisition_NOK = FALSE;
	*pBitMixup = FALSE;

	bSnapOK[0] = false;
	bSnapOK[1] = false;

	//V52
	CString strForListe=_T("");
	((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strForListe);
	strForListe=_T("ATTENTE Image....(Bit Snap)");
	((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strForListe);

	for (int iCam = 0; iCam < 2; iCam++)
	{
		if(UEye_Source_Trigger[iCam]->value == IS_SET_TRIGGER_SOFTWARE)
		{
			uEye[iCam].Snap();
		}
	}
}

void CVisionBoite::Vision_Decharger()
{
	//V37
	//for (int i = 0; i < NB_EXTRACTIONS; i++)
	//{
	//	imgVisu_roiExtract[i].Decharger();
	//	imgTrait_roiExtract[i].Decharger();
	//	imgRotation_roiExtract[i].Decharger();		//V25
	//}
	imgVisu.Decharger();
	
	imgSource_roiCam[0].Decharger();
	imgSource_roiCam[1].Decharger();
	imgSource.Decharger();

	//V 44.8
	Graph_Visu.Decharger();
	
	//V37
	//imgTrait.Decharger();
	//imgRotation.Decharger();
}
