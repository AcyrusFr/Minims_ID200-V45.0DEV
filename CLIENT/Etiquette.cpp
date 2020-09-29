#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialEtiquette.h"
#include "Etiquette.h"
#include "General.h"
#include "AC_Base.h"

#include "Acy_Xml\tinyxml.h"//V 44.0

CEtiquette::CEtiquette(void)
{
	Etiq_strCode = _T("");
	Etiq_ID = 0;
	Etiq_iNbExtract = 0;
	Etiq_bTest = false;

	Etiq_bChargementOK = false;
	Etiq_strEtat = _T("");
	for(int i=0;i<NB_CHAMP_MAX;i++)
	{
		ETC_strChampBrut[i] = _T("");
		ETC_strChampFormat[i] = _T("");
		ETC_strChampTitre[i] = _T("");
		ETC_SrcData[i] = 0; //V44.5
	}

	for (int i = 0; i < NB_EXTRACTIONS; i++)
	{
		ZeroMemory(&RoiExtract[i],sizeof(PARAM_ROI));
		ZeroMemory(&RoiExtractBase[i],sizeof(PARAM_ROI));
		ZeroMemory(&RoiExtractRotation[i],sizeof(PARAM_ROI));
		ZeroMemory(&RoiResult[i],sizeof(PARAM_ROI));
		ZeroMemory(&PtCentreResult[i],sizeof(STRUCT_POINT));
	}
	iForcageResult = -1;
}

CEtiquette::~CEtiquette(void)
{
}

bool CEtiquette::LireEntete(bool bAuto)
{
	//Chargement extractions depuis format etiquette
	Etiq_ID = 0;
	Etiq_iNbExtract = 0;

	CString strCritere = _T("");
	CADORecordset rsEtiquette (&theBase.dao_Base);
	strCritere.Format(_T("Code = '%s'"),Etiq_strCode);
	if(!ACBaseReqRsOpen(strCritere,"[Etiquette]",&rsEtiquette))
	{
		rsEtiquette.Close();
		Etiq_strEtat.Format(_T("%s : Nouveau Format étiquette : '%s'"),Etiq_strTitre,Etiq_strCode);
		if(bAuto)
		{
			//Ajout de l'entete
			CString strRequete;
			strRequete.Format( _T("INSERT INTO Etiquette ([Code]) VALUES ('%s')"),Etiq_strCode); 
			try
			{
				theBase.dao_Base.Execute(strRequete);
			}
			catch (CException* e)
			{
				e;//GCA AVERT
				Etiq_strEtat.Format("%s E1.1: Ajout Entete etiquette : %s",Etiq_strTitre,Etiq_strCode);
				return false;
			}

			//Essai de relecture
			if(!ACBaseReqRsOpen(strCritere,"[Etiquette]",&rsEtiquette))
			{
				Etiq_strEtat.Format("%s E1.2: Nouveau format non créé : %s",Etiq_strTitre,Etiq_strCode);
				rsEtiquette.Close();
				return false;
			}
		}
		else
		{
			Etiq_strEtat.Format("%s E1.3: lecture entete : %s",Etiq_strTitre,Etiq_strCode);
			rsEtiquette.Close();
			return false;
		}
	}

	Etiq_ID = ACBaseGetLong("ID",&rsEtiquette);
	rsEtiquette.Close();
	return true;
}


bool CEtiquette::Extractions_Charger()
{
	//Reset des extractions
	for (int i = 0; i < NB_EXTRACTIONS; i++)
	{
		Extract[i].Decharger(); //ETIQ a faire : RAZ des variables dans Decharger()

		Extract[i].Extract_bIsOK = true;//V 44.1
	}	

	Etiq_iNbExtract = 0;

	CString strCritere = _T("");
	CADORecordset rsExtractions (&theBase.dao_Base);
	strCritere.Format(_T("IDEtiquette = '%ld' AND Actif = 0"),Etiq_ID);
	if(!ACBaseReqRsOpen(strCritere,"[EtiqDetail]",&rsExtractions))
	{
		Etiq_strEtat = Etiq_strTitre + _T(" E2.1: Format étiquette non initialisée");
		if (rsExtractions.IsOpen())
		{
			rsExtractions.Close();
		}
		return false;
	}

	int iIndex = 0;
	PARAM_ROI RoiTemp = {0};
	int iTpExtract = 0;
	CString strETC_Champ = _T("");
	CString strETC_ChampSup = _T("");
	CString strETC_Data = _T("");
	CString strETC_DataSup = _T("");
	CString strFicModele = _T("");
	CString strLibelle = _T("");
	long lIDTemp = 0;
	long lAngleRotation = 0;		//V48
	long lIsRepere = 0;				//V49
	long lCentreX = 0;				//V49
	long lCentreY = 0;				//V49
	long lContrainte = 0;			//V50
	long lVal1 = 0;					//V21

	rsExtractions.MoveFirst();
	do
	{
		iTpExtract = 0;
		ZeroMemory(&RoiTemp,sizeof(PARAM_ROI));

		//Récupération des infos
		strLibelle = ACBaseGetStr("Libelle",&rsExtractions);
		if(strLibelle.GetLength() == 0 ) strLibelle = _T("");
		strETC_Champ = ACBaseGetStr("ETC_Champ",&rsExtractions);
		if(strETC_Champ.GetLength() == 0 ) strETC_Champ = _T("");
		strETC_Data = ACBaseGetStr("ETC_Data",&rsExtractions);
		if(strETC_Data.GetLength() == 0 ) strETC_Data = _T("");
		strETC_ChampSup = ACBaseGetStr("ETC_ChampSup",&rsExtractions);
		if(strETC_ChampSup.GetLength() == 0 ) strETC_ChampSup = _T("");
		strETC_DataSup = ACBaseGetStr("ETC_DataSup",&rsExtractions);
		if(strETC_DataSup.GetLength() == 0 ) strETC_DataSup = _T("");
		RoiTemp.X1 = ACBaseGetInt("RoiX",&rsExtractions);
		RoiTemp.Y1 = ACBaseGetInt("RoiY",&rsExtractions);
		RoiTemp.dX = ACBaseGetInt("RoiDX",&rsExtractions);
		RoiTemp.dY = ACBaseGetInt("RoiDY",&rsExtractions);
		iTpExtract = ACBaseGetInt("TpExtract",&rsExtractions);
		strFicModele = ACBaseGetStr("FicModele",&rsExtractions);
		if(strFicModele.GetLength() == 0 ) strFicModele = _T("");
		lAngleRotation = ACBaseGetInt("Orientation",&rsExtractions);	//V48
		lIsRepere = ACBaseGetInt("IsRepere",&rsExtractions);			//V49
		lCentreX = ACBaseGetInt("CentreX",&rsExtractions);				//V50
		lCentreY = ACBaseGetInt("CentreY",&rsExtractions);				//V50
		lContrainte = ACBaseGetInt("Contrainte",&rsExtractions);		//V50
		lVal1 = ACBaseGetLong("lVal1",&rsExtractions);					//V21


		//On ne garde que le nom du fichier et on se remet dans les repertoires de l'application
		int iIndex = strFicModele.ReverseFind(_T('\\'));
		if (iIndex > 0)
		{
			CString strDir = _T("");
			strDir = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\");

			CString strTemp = strFicModele.Right((strFicModele.GetLength() -(iIndex + 1)));
			strFicModele = strDir + strTemp;
		}
		else
		{
			//uniquement le nom du fichier
			CString strDir = _T("");
			strDir = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\");

			CString strTemp = strFicModele;
			strFicModele = strDir + strTemp;
		}

		lIDTemp = ACBaseGetLong("ID",&rsExtractions);
				
		int iIndexExtraction = Etiq_iNbExtract;
		Etiq_iNbExtract ++;

		//re-chargement des Images et Extractions
		memcpy_s(&RoiExtractBase[iIndexExtraction],sizeof(PARAM_ROI),&RoiTemp,sizeof(PARAM_ROI));

		imgTrait_roiExtract[iIndexExtraction].sLibelle.Format(_T("imgTrait_roi[%d]"),iIndexExtraction);
		imgTrait_roiExtract[iIndexExtraction].Decharger();
		imgTrait_roiExtract[iIndexExtraction].Charger(&imgTrait);
		if(!imgTrait_roiExtract[iIndexExtraction].SetROI(&RoiExtractBase[iIndexExtraction]))
		{
			AfxMessageBox("Zone de lecture incorrecte");

			//V 44.1
			Extract[iIndexExtraction].Extract_bIsOK = false;
			Extract[iIndexExtraction].strResult.Format(_T("ERREUR Position %s Non Définie"), Extract[iIndexExtraction].strLibelle);
			//return false;		//TODO : retourner ou afficher une erreur
		}

		imgVisu_roiExtract[iIndexExtraction].Decharger();
		imgVisu_roiExtract[iIndexExtraction].sLibelle.Format(_T("imgVisu_roi[%d]"),iIndexExtraction);
		imgVisu_roiExtract[iIndexExtraction].Charger(imgVisu);
		imgVisu_roiExtract[iIndexExtraction].SetROI(&RoiExtractBase[iIndexExtraction]);

		//V48
		Extract[iIndexExtraction].lOrientation = lAngleRotation;
		switch(Extract[iIndexExtraction].lOrientation)
		{
		case 90:
			RoiExtractRotation[iIndexExtraction].X1 = 0;
			RoiExtractRotation[iIndexExtraction].Y1 = 0;
			RoiExtractRotation[iIndexExtraction].dX = RoiTemp.dY;
			RoiExtractRotation[iIndexExtraction].dY = RoiTemp.dX;
			//Affectation de l'image de calcul
			Extract[iIndexExtraction].img = &imgRotation_roiExtract[iIndexExtraction];
			break;

		case -90:
			RoiExtractRotation[iIndexExtraction].X1 = 0;
			RoiExtractRotation[iIndexExtraction].Y1 = 0;
			RoiExtractRotation[iIndexExtraction].dX = RoiTemp.dY;
			RoiExtractRotation[iIndexExtraction].dY = RoiTemp.dX;
			//Affectation de l'image de calcul
			Extract[iIndexExtraction].img = &imgRotation_roiExtract[iIndexExtraction];
			break;

		case 180:
			RoiExtractRotation[iIndexExtraction].X1 = 0;
			RoiExtractRotation[iIndexExtraction].Y1 = 0;
			RoiExtractRotation[iIndexExtraction].dX = RoiTemp.dX;
			RoiExtractRotation[iIndexExtraction].dY = RoiTemp.dY;
			//Affectation de l'image de calcul
			Extract[iIndexExtraction].img = &imgRotation_roiExtract[iIndexExtraction];
			break;

		default:	//V16 : si le parametre est mal défini, reste horizontal
			RoiExtractRotation[iIndexExtraction].X1 =RoiTemp.X1;
			RoiExtractRotation[iIndexExtraction].Y1 = RoiTemp.Y1;
			RoiExtractRotation[iIndexExtraction].dX = RoiTemp.dX;
			RoiExtractRotation[iIndexExtraction].dY = RoiTemp.dY;
			//Affectation de l'image de calcul
			Extract[iIndexExtraction].img = &imgTrait_roiExtract[iIndexExtraction];
			break;
		}

		imgRotation_roiExtract[iIndexExtraction].Decharger();
		imgRotation_roiExtract[iIndexExtraction].sLibelle.Format(_T("imgRotation_roi[%d]"),iIndexExtraction);
		imgRotation_roiExtract[iIndexExtraction].Charger(&imgRotation);
		imgRotation_roiExtract[iIndexExtraction].SetROI(&RoiExtractRotation[iIndexExtraction]);

		Extract[iIndexExtraction].lIsRepere = lIsRepere;
		Extract[iIndexExtraction].PtCentreBase.Xpix = (double)lCentreX;
		Extract[iIndexExtraction].PtCentreBase.Ypix = (double)lCentreY;
		
		Extract[iIndexExtraction].lContrainte = lContrainte;
		Extract[iIndexExtraction].IDDetail = lIDTemp;

		Extract[iIndexExtraction].lVal1 = lVal1;

		Extract[iIndexExtraction].iType = iTpExtract;
		Extract[iIndexExtraction].iNumExtract = iIndex;

		Extract[iIndexExtraction].bVisualisation = true;
		Extract[iIndexExtraction].ColorVisu = COLOR_VERT;
		//Affectation de l'image de visualisation
		Extract[iIndexExtraction].imgVisu = &imgVisu_roiExtract[iIndexExtraction];
			
		Extract[iIndexExtraction].strFicModele = strFicModele;
		Extract[iIndexExtraction].strChamp = strETC_Champ;
		Extract[iIndexExtraction].strChampSup = strETC_ChampSup;
		Extract[iIndexExtraction].strCodeALire = strETC_Data ;//V49 TODO + strETC_DataSup;
		Extract[iIndexExtraction].strLibelle = strLibelle;

		//V 44.0
		if (!Extract[iIndexExtraction].Charger())
		{
			//V 44.1
			Extract[iIndexExtraction].Extract_bIsOK = false;
			Extract[iIndexExtraction].strResult.Format(_T("ERREUR FICHIER %s (%s)"), Extract[iIndexExtraction].strLibelle, Extract[iIndexExtraction].strFicModele);

			Etiq_strEtat = Etiq_strTitre + _T(" E2.2: Chargement extraction incorrecte (verifier la présence des fichiers modèles)");
		}

		//Application des contraintes pour l'OCR
		if(Extract[iIndexExtraction].iType == TYPE_EXTRACT_OCR)
		{
			//V49 + V50
			char charListe[2] = {0};
			ZeroMemory(&charListe,2* sizeof(char));
			//RAZ des contraintes
			MstrControl(Extract[iIndexExtraction].Reader.SearchContext,M_STRING_INDEX(M_ALL),M_STRING_SIZE_MIN,M_DEFAULT);
			MstrControl(Extract[iIndexExtraction].Reader.SearchContext,M_STRING_INDEX(M_ALL),M_STRING_SIZE_MAX,M_DEFAULT);
			for(int i=0;i<255;i++)
			{
				MstrSetConstraint(Extract[iIndexExtraction].Reader.SearchContext, M_STRING_INDEX(M_ALL), i,  M_DEFAULT, M_NULL );
			}

			//Ajout des conraintes spécifiques
			if(Extract[iIndexExtraction].lContrainte == 1)
			{
				for(int i=0;i<Extract[iIndexExtraction].strCodeALire.GetLength();i++)
				{
					charListe[0] = Extract[iIndexExtraction].strCodeALire.GetAt(i);
					MstrSetConstraint(Extract[iIndexExtraction].Reader.SearchContext, M_STRING_INDEX(M_ALL), i,  M_ANY, charListe );
				}
				//Definition de la taille du code à lire
				MstrControl(Extract[iIndexExtraction].Reader.SearchContext,M_STRING_INDEX(M_ALL),M_STRING_SIZE_MIN,Extract[iIndexExtraction].strCodeALire.GetLength());
				MstrControl(Extract[iIndexExtraction].Reader.SearchContext,M_STRING_INDEX(M_ALL),M_STRING_SIZE_MAX,Extract[iIndexExtraction].strCodeALire.GetLength());
			}
		}
		//V21 : Cas particulier des blobs
		if(Extract[iIndexExtraction].iType == TYPE_EXTRACT_BLOB)
		{
			Extract[iIndexExtraction].img = &imgRotation_roiExtract[iIndexExtraction];
		}
		//V21 : Cas particulier des stat
		if(Extract[iIndexExtraction].iType == TYPE_EXTRACT_STAT)
		{
			Extract[iIndexExtraction].img = &imgRotation_roiExtract[iIndexExtraction];
		}

		Extract[iIndexExtraction].PreProcess();

		rsExtractions.MoveNext();
	}
	while(!rsExtractions.IsEOF());

	if (rsExtractions.IsOpen())
	{
		rsExtractions.Close();
	}

	Etiq_strEtat.Format("%s: Etiquette chargée = %s",Etiq_strTitre,Etiq_strCode);
	Etiq_bChargementOK = true;
	return true;
}

void CEtiquette::Extractions_Draw(CGraphListe * pGraph)
{
	pGraph->Clear();
	pGraph->Objet_ResetAll();

	PARAM_ROI * pRoi = NULL;

	for (int i = 0; i < Etiq_iNbExtract; i++)
	{
		pRoi = &RoiExtractBase[i];

		if ((pRoi->dX == 0) ||
			(pRoi->dY == 0))
		{
			//extraction créée "nouvellement"
			pRoi->X1 = 100;
			pRoi->Y1 = 100;
			pRoi->dX = 100;
			pRoi->dY = 100;
		}


		CString strLibelleZone = _T("Zone ") + Extract[i].strLibelle + _T(" |");
		pGraph->AddRectAngle(pRoi->X1
			, pRoi->Y1
			, pRoi->dX
			, pRoi->dY
			, 0//Angle
			, M_PIXEL
			, Extract[i].ColorVisu
			, false//Filled
			, strLibelleZone);

		MIL_INT LastLabel = (MIL_INT)pGraph->GetLastLabel();

		switch (Extract[i].iType)
		{
		case TYPE_EXTRACT_MODELE:
			pGraph->Objet_SetMenuFromLabel(LastLabel, MENU_EXTRACTION_MODELES);
			break;

		case TYPE_EXTRACT_OCR:
		case TYPE_EXTRACT_DMX:
		case TYPE_EXTRACT_CODEBARRE:
		case TYPE_EXTRACT_BLOB:
		case TYPE_EXTRACT_STAT:
		//case TYPE_EXTRACT_STAT_COLOR:
		case TYPE_EXTRACT_UNDEF:
		default:
			pGraph->Objet_SetMenuFromLabel(LastLabel, MENU_EXTRACTION);
			break;
		}


		pGraph->SetSelectableLabel(LastLabel);
		pGraph->SetNOTRotatableLabel(LastLabel);

		Extract[i].iLabelTracage = LastLabel;
	}
}

bool CEtiquette::ETC_ChargerData(CString CodeOF)
{
#ifdef XML
	for (int i = 0;i<NB_CHAMP_MAX;i++)
	{
		ETC_strChampBrut[i] = _T("");
		ETC_strChampFormat[i] = _T("");
		ETC_strChampTitre[i] = _T("");
	}

	//V 44.0 préparation gestion fichier .xml
	CString StrFichierXml = _T("");
	StrFichierXml.Format(_T("%s\\%s"), pParamRepFichierEtiXml->value, pParamNomFichierEtiXml->value);

	TiXmlDocument * Acydoc;
	Acydoc = new TiXmlDocument(StrFichierXml);
	bool loadOkay = Acydoc->LoadFile(TIXML_ENCODING_UTF8);

	if (loadOkay == false)
	{
		Etiq_strEtat = Etiq_strTitre + _T(" : E0.1 Erreur ouverture fichier .xml");
		if (Acydoc != NULL)
		{
			delete(Acydoc);
		}
		return false;
	}

	//le fichier est présent et les données chargées
	
	//Récupération de la liste des champs à lire dans la table EtiqChamp
	CString strEtiqChamp = _T("");
	CADORecordset rsEtiqChamp(&theBase.dao_Base);
	if (!ACBaseReqRsOpen(_T(""), "[EtiqChamp]", &rsEtiqChamp))
	{
		Etiq_strEtat = Etiq_strTitre + _T(" : E0.2 Chargement de la liste des champs Etifront");
		if (Acydoc != NULL)
		{
			delete(Acydoc);
		}
		return false;
	}
	rsEtiqChamp.MoveFirst();
	int iIndex = 0;
	do
	{
		//Récupération des infos
		iIndex = ACBaseGetInt("Numero", &rsEtiqChamp);
		ETC_strChampTitre[iIndex] = ACBaseGetStr("Libelle", &rsEtiqChamp);
		rsEtiqChamp.MoveNext();
	} while (!rsEtiqChamp.IsEOF());

	if (rsEtiqChamp.IsOpen())
	{
		rsEtiqChamp.Close();
	}
	//FIN Récupération de la liste des champs à lire dans la table EtiqChamp

	//Vérification de l'OF affecté à la machine
	CString strODF_Code = _T("");
	TiXmlElement* ElementCode = NULL;
	ElementCode = ACYXXml_GetElementIfExist(Acydoc, _T("ODF_Code"));

	if (ElementCode == NULL)
	{
		Etiq_strEtat.Format(_T("%s: E0.3 Champ ODF_Code introuvable dans le fichier: %s"), Etiq_strTitre, StrFichierXml);
		if (Acydoc != NULL)
		{
			delete(Acydoc);
		}
		return false;
	}

	strODF_Code = ACYXXml_GetValueAsSTR(ElementCode);
	if (strODF_Code != CodeOF)
	{
		Etiq_strEtat.Format(_T("%s: E0.4 Champ ODF_Code (%s) non conforme dans le fichier: %s"), Etiq_strTitre, strODF_Code, StrFichierXml);
		if (Acydoc != NULL)
		{
			delete(Acydoc);
		}
		return false;
	}
	//Récupération du fichier .lab
	TiXmlElement* ElementLab = NULL;
	ElementLab = ACYXXml_GetElementIfExist(Acydoc, _T("GabaritCode"));

	if (ElementLab == NULL)
	{
		Etiq_strEtat.Format(_T("%s: E0.5 Champ GabaritCode introuvable dans le fichier: %s"), Etiq_strTitre, StrFichierXml);
		if (Acydoc != NULL)
		{
			delete(Acydoc);
		}
		return false;
	}

	Etiq_strCode = ACYXXml_GetValueAsSTR(ElementLab);
	if (Etiq_strCode == _T(""))
	{
		Etiq_strEtat.Format(_T("%s: E0.6 Champ GabaritCode (%s) non conforme dans le fichier: %s"), Etiq_strTitre, strODF_Code, StrFichierXml);
		if (Acydoc != NULL)
		{
			delete(Acydoc);
		}
		return false;
	}

	Etiq_strCode += _T(".lab");

	CString strTemp = _T("");
	//Récupération de la valeur des champs
	TiXmlElement* ElementChamp = NULL;
	
	for (int i = 0; i < NB_CHAMP_MAX; i++)
	{
		if (ETC_strChampTitre[i] != _T(""))
		{
			ElementChamp = ACYXXml_GetElementIfExist(Acydoc, ETC_strChampTitre[i]);
			if (ElementChamp == NULL)
			{
				//Champ inexistant
				ETC_strChampBrut[i] = _T("");
				continue;
			}

			ETC_strChampBrut[i] = ACYXXml_GetValueAsSTR(ElementChamp);
		}
	}

	//Traitement des données
	CString strRecherche = _T("");
	for (int i = 0; i < NB_CHAMP_MAX; i++)
	{
		if (ETC_strChampTitre[i] != _T(""))
		{
			//Champ fixe
			ETC_strChampFormat[i] = ETC_strChampBrut[i];

			//Suppression systématiques des espaces et caractères spéciaux pour ocr/reader matrox
			ETC_strChampFormat[i].Remove(_T(' '));
			ETC_strChampFormat[i].Remove(_T('.'));
			ETC_strChampFormat[i].Remove(_T(':'));
			ETC_strChampFormat[i].Remove(_T(','));
			ETC_strChampFormat[i].Remove(_T(';'));
			ETC_strChampFormat[i].Remove(_T('%'));
			ETC_strChampFormat[i].Remove(_T('-'));
			ETC_strChampFormat[i].Remove(_T('_'));
			ETC_strChampFormat[i].Remove(_T('/'));
			ETC_strChampFormat[i].Remove(_T('°'));
		}
	}


	if (Acydoc != NULL)
	{
		delete(Acydoc);
	}

return true;
#endif	

#ifdef ETICONCEPT
	//Récupération de l'étiquette liée à l'OF
	CString strTableMask = _T("MASK");
	CBase BaseEticoncept;

	for(int i=0;i<NB_CHAMP_MAX;i++)
	{
		ETC_strChampBrut[i] = _T("");
		ETC_strChampFormat[i] = _T("");
		ETC_strChampTitre[i] = _T("");
		ETC_SrcData[i] = 0;
	}

	//Récupération de la liste des champs à lire dans la table EtiqChamp
	CString strEtiqChamp = _T("");
	CADORecordset rsEtiqChamp (&theBase.dao_Base);
	if(!ACBaseReqRsOpen(_T(""),"[EtiqChamp]",&rsEtiqChamp))
	{
		Etiq_strEtat = Etiq_strTitre + _T(" : E0.1 Chargement de la liste des champs Etifront");
		return false;
	}
	rsEtiqChamp.MoveFirst();
	int iIndex = 0;
	do
	{
		//Récupération des infos
		iIndex = ACBaseGetInt("Numero",&rsEtiqChamp);
		ETC_strChampTitre[iIndex] = ACBaseGetStr("Libelle",&rsEtiqChamp);

		//V44.5 Test sur la source de données
		if(ETC_strChampTitre[iIndex].Left(4) == _T("ART_"))
		{
			ETC_SrcData[iIndex] = SRC_ETIFRONT;
		}
		else if(ETC_strChampTitre[iIndex].Left(4) == _T("MEN_"))
		{
			ETC_SrcData[iIndex] = SRC_ETIFRONT;
		}
		else if (ETC_strChampTitre[iIndex].Left(9) == _T("ITEM_MET_"))
		{
			ETC_SrcData[iIndex] = SRC_ITEM_MET;
		}
		else
		{
			ETC_SrcData[iIndex] = SRC_AUCUN;
		}



		rsEtiqChamp.MoveNext();
	}
	while(!rsEtiqChamp.IsEOF());

	if (rsEtiqChamp.IsOpen())
	{
		rsEtiqChamp.Close();
	}

	if(!BaseEticoncept.Ouvrir(pParamRepBaseEticoncept->value,pParamNomBaseEticoncept->value,pParamProviderBaseEticoncept->value))
	{
		Etiq_strEtat = Etiq_strTitre + _T(" : E0.2 Erreur ouverture base Eticoncept");
		return false;
	}

	//Fonction de verification d'un OF en cours
	CADORecordset rsMeltingPot (&BaseEticoncept.dao_Base);
	CString strCritere = _T("");

	//Recherche de l'affectation de la machine
	if(pParamNomMachineBL->value != "*")
	{
		strCritere.Format(_T("%s = '%s'"),pParamNomChampMachine->value,pParamNomMachineBL->value);
		if(!ACBaseReqRsOpen(strCritere,pParamNomTableEticoncept->value,&rsMeltingPot))
		{
			if(rsMeltingPot.m_strLastError = _T(""))
			{
				Etiq_strEtat.Format(_T("%s : E0.3 Machine %s non affectée dans Eticoncept"),Etiq_strTitre,pParamNomMachineBL->value);
			}
			else
			{
				Etiq_strEtat = Etiq_strTitre + " : E0.3 " + rsMeltingPot.m_strLastError;
			}
			rsMeltingPot.Close();
			BaseEticoncept.Fermer();
			return false;
		}
	}
	else
	{
		strCritere.Format(_T("ODF_Code = '%s'"),CodeOF);
		if(!ACBaseReqRsOpen(strCritere,pParamNomTableEticoncept->value,&rsMeltingPot))
		{
			if(rsMeltingPot.m_strLastError = _T(""))
			{
				Etiq_strEtat.Format(_T("%s : E0.3 SIMULATION / OF %s non déclaré"),Etiq_strTitre,CodeOF);
			}
			else
			{
				Etiq_strEtat= Etiq_strTitre + " : E0.3 " + rsMeltingPot.m_strLastError;
			}
			rsMeltingPot.Close();
			BaseEticoncept.Fermer();
			return false;
		}
	}
	//Verification du nombre d'enregistrement
	if(rsMeltingPot.GetRecordCount() > 1)
	{
		Etiq_strEtat.Format(_T("%s: E0.4 Plusieurs OF declarés pour la machine %s "),Etiq_strTitre,pParamNomMachineBL->value);
		rsMeltingPot.Close();
		BaseEticoncept.Fermer();
		return false;
	}

	//Vérification de l'OF affecté à la machine
	CString strODF_Code = _T("");
	strODF_Code = ACBaseGetStr("ODF_Code",&rsMeltingPot);
	if(strODF_Code != CodeOF)
	{
		Etiq_strEtat.Format(_T("%s: E0.5 OF Eticoncept non conforme : %s"),Etiq_strTitre,strODF_Code);
		rsMeltingPot.Close();
		BaseEticoncept.Fermer();
		return false;
	}

	//Récupération du Fichier Etiquette (.lab)
	CString strTAR_Code = _T("");
	CString strTLI_Code = _T("");
	CADORecordset rsMask (&BaseEticoncept.dao_Base);

	strTLI_Code = ACBaseGetStr("TLI_Code",&rsMeltingPot);
	strTAR_Code = ACBaseGetStr("TAR_Code",&rsMeltingPot);

	CString strCritereMask = _T("");
	strCritereMask.Format(_T("TLI_Code = '%s' AND TAR_Code = '%s' AND MSK_Rank = '1'"),strTLI_Code,strTAR_Code);
	if(!ACBaseReqRsOpen(strCritereMask,strTableMask,&rsMask))
	{
		Etiq_strEtat = Etiq_strTitre + _T(" : E0.6 Fichier LAB non défini");
		rsMeltingPot.Close();
		BaseEticoncept.Fermer();
		return false;
	}
	Etiq_strCode = ACBaseGetStr("MSK_Code",&rsMask);
	rsMask.Close();

	CString strTemp = _T("");
	//Récupération de la valeur des champs
	for (int i = 0; i < NB_CHAMP_MAX; i++)
	{
		//V44.5 if(ETC_strChampTitre[i] != _T(""))
		if (ETC_SrcData[i] == SRC_ETIFRONT)
		{
			ETC_strChampBrut[i] = ACBaseGetStr(ETC_strChampTitre[i],&rsMeltingPot);
		}
	}

	//Traitement des données

	//V44.5 : Extraction dans Etifront (SRC_ETIFRONT)
	CString strRecherche = _T("");
	for (int i = 0; i < NB_CHAMP_MAX; i++)
	{
		//V44.5 if(ETC_strChampTitre[i] != _T(""))
		if (ETC_SrcData[i] == SRC_ETIFRONT)
		{
			strTemp = ETC_strChampBrut[i].Left(4);
			if (strTemp.Left(2) == _T("{#"))
			{
				//Champ variable
				//Si le champ commence par DT => à rechercher dans ODF_XML
				if (strTemp == _T("{#DT"))
				{
					strRecherche = ETC_strChampBrut[i];
					strRecherche.Remove(_T('{'));
					strRecherche.Remove(_T('#'));
					strRecherche.Remove(_T('}'));

					CString strXml = _T("");
					CString strFind = _T("");
					int iLenght = 0;
					iLenght = strRecherche.GetLength() - (strRecherche.Find(_T("_")) + 1);

					strXml = ACBaseGetStr("ODF_XML",&rsMeltingPot);
					//Exemple Format Xml
					//<DTPEREMPTION_MMYYYY>102015</DTPEREMPTION_MMYYYY>
					strFind.Format(_T("</%s>"),strRecherche);
					int index = strXml.Find(strFind);
					if(index > 0)
					{
						ETC_strChampFormat[i] = strXml.Mid((index - iLenght),iLenght);
					}
				}
				//Sinon => rechercher le champ lui-même
				else
				{
					strRecherche = ETC_strChampBrut[i];
					strRecherche.Remove(_T('{'));
					strRecherche.Remove(_T('#'));
					strRecherche.Remove(_T('}'));
					ETC_strChampFormat[i] = ACBaseGetStr(strRecherche,&rsMeltingPot);
				}
			}
			else
			{
				//Champ fixe
				ETC_strChampFormat[i] = ETC_strChampBrut[i];
			}
		}

		//Suppression systématiques des espaces
		ETC_strChampFormat[i].Remove(_T(' '));
		ETC_strChampFormat[i].Remove(_T('.'));
		ETC_strChampFormat[i].Remove(_T(':'));
		ETC_strChampFormat[i].Remove(_T(','));
		ETC_strChampFormat[i].Remove(_T(';'));
		ETC_strChampFormat[i].Remove(_T('%'));
		ETC_strChampFormat[i].Remove(_T('-'));
		ETC_strChampFormat[i].Remove(_T('_'));
		ETC_strChampFormat[i].Remove(_T('/'));
		ETC_strChampFormat[i].Remove(_T('°'));
	}

	//V44.5 : Extraction dans ITEM_MET (SRC_ITEM_MET)
	for (int i = 0; i < NB_CHAMP_MAX; i++)
	{
		if (ETC_SrcData[i] == SRC_ITEM_MET)
		{
			ETC_strChampBrut[i] = theApp.m_Recette.ArtEtiquette.CodeDMX;
			ETC_strChampFormat[i] = theApp.m_Recette.ArtEtiquette.CodeDMX;
		}
	}

	if(rsMeltingPot.IsOpen())
		rsMeltingPot.Close();

	BaseEticoncept.Fermer();
	return true;
#endif
}

bool CEtiquette::ETC_AffecterDataTo_EtiqDetail()
{
	CString strRequete=_T("");
	for(int iChamp=0;iChamp<NB_CHAMP_MAX;iChamp++)
	{
		if(ETC_strChampTitre[iChamp]!="")	//V18 : On ne change pas la valeur pour les champs sans nom lié à Eticoncept
		{
			strRequete.Format( _T("UPDATE EtiqDetail SET [ETC_Data] = '%s' WHERE [IDEtiquette] = %ld AND [ETC_Champ] = '%s'"),
				ETC_strChampFormat[iChamp],
				this->Etiq_ID,
				ETC_strChampTitre[iChamp]);
			try
			{
				theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
			}
			catch (CException* e)
			{
				TCHAR   szCause[255];
				e->GetErrorMessage(szCause,255);
				e->Delete();
				//strErreur = "ERREUR UPDATE CRecette::Table Lot\r\nDescription :\n";
				//strErreur = strErreur + szCause;
				////Affichage journal événements
				//theApp.SetEventsJournalStr(strErreur);
			}
		}
	}
	for(int iChamp=0;iChamp<NB_CHAMP_MAX;iChamp++)
	{
		if(ETC_strChampTitre[iChamp]!="")	//V18 : On ne change pas la valeur pour les champs sans nom lié à Eticoncept
		{
			strRequete.Format( _T("UPDATE EtiqDetail SET [ETC_DataSup] = '%s' WHERE [IDEtiquette] = %ld AND [ETC_ChampSup] = '%s'"),
				ETC_strChampFormat[iChamp],
				this->Etiq_ID,
				ETC_strChampTitre[iChamp]);
			try
			{
				theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
			}
			catch (CException* e)
			{
				TCHAR   szCause[255];
				e->GetErrorMessage(szCause,255);
				e->Delete();
				//strErreur = "ERREUR UPDATE CRecette::Table Lot\r\nDescription :\n";
				//strErreur = strErreur + szCause;
				////Affichage journal événements
				//theApp.SetEventsJournalStr(strErreur);
			}
		}
	}

	return true;
}

bool CEtiquette::UpdateROI(void)
{
	//Maj de la BDD avec les infos en cours
	CString strCritere2 = _T("");
	CADORecordset rsDetail (&theBase.dao_Base);

	for (int i = 0; i < this->Etiq_iNbExtract; i++)
	{
		strCritere2.Format(_T("ID = '%ld'"),Extract[i].IDDetail);
		if(ACBaseReqRsOpen(strCritere2,"[EtiqDetail]",&rsDetail))
		{
			rsDetail.Edit();
			PARAM_ROI RoiTemp = {0};

			memcpy_s(&RoiTemp,sizeof(PARAM_ROI),&RoiExtractBase[i],sizeof(PARAM_ROI));

			//enregistrement des infos
			ACBaseSetInt(RoiTemp.X1,"RoiX",&rsDetail);
			ACBaseSetInt(RoiTemp.Y1,"RoiY",&rsDetail);
			ACBaseSetInt(RoiTemp.dX,"RoiDX",&rsDetail);
			ACBaseSetInt(RoiTemp.dY,"RoiDY",&rsDetail);

			rsDetail.Update();
			rsDetail.Close();
		}
	}
	return true;
}

bool CEtiquette::Apprentissage(int iExtract)
{
	bool bEligible = false;
	switch(Extract[iExtract].iType)
	{
		case TYPE_EXTRACT_BLOB:
		case TYPE_EXTRACT_STAT:
			bEligible = true;
			break;
	}
	
	if(!bEligible) return false;
	MbufClear(imgRotation.milBuf,0x00);
	Vision_DoExtractionSingle(iExtract);
	
	//Maj de la BDD pour le champs lVal1
	CString strCritere2 = _T("");
	CADORecordset rsDetail (&theBase.dao_Base);

	Extract[iExtract].lVal1 = Extract[iExtract].lResult;
	strCritere2.Format(_T("ID = '%ld'"),Extract[iExtract].IDDetail);
	if(ACBaseReqRsOpen(strCritere2,"[EtiqDetail]",&rsDetail))
	{
		rsDetail.Edit();

		//enregistrement des infos
		ACBaseSetLong(Extract[iExtract].lResult,"lVal1",&rsDetail);

		rsDetail.Update();
		rsDetail.Close();
	}
	return true;
}

bool CEtiquette::UpdateReference(void)
{
	//Maj du centre de la zone de référence
	CString strCritere2 = _T("");
	CADORecordset rsDetail (&theBase.dao_Base);

	//V30 : réapprentisasge des zones de STAT
	for (int i = 0; i < this->Etiq_iNbExtract; i++)
	{
		Apprentissage(i);
	}

	for (int i = 0; i < this->Etiq_iNbExtract; i++)
	{
		strCritere2.Format(_T("ID = '%ld'"),Extract[i].IDDetail);
		if(ACBaseReqRsOpen(strCritere2,"[EtiqDetail]",&rsDetail))
		{
			rsDetail.Edit();
			//enregistrement des positions
			ACBaseSetInt((int)(PtCentreResult[i].Xpix),"CentreX",&rsDetail);
			ACBaseSetInt((int)(PtCentreResult[i].Ypix),"CentreY",&rsDetail);

			rsDetail.Update();
			rsDetail.Close();
		}
	}

	//Recentrage de la zone de lecture
	PARAM_ROI RoiTemp = {0};
	for (int i = 0; i < this->Etiq_iNbExtract; i++)
	{
		if(Extract[i].lIsRepere == 0)
		{
			RoiExtractBase[i].X1 = (long)PtCentreResult[i].Xpix - RoiExtractBase[i].dX/2;
			RoiExtractBase[i].Y1 = (long)PtCentreResult[i].Ypix - RoiExtractBase[i].dY/2;
		}
	}
	UpdateROI();

	return true;
}

void CEtiquette::InitOF(void)
{
//Eticoncept
	for(int i=0;i<NB_CHAMP_MAX;i++)
	{
		ETC_strChampBrut[i] = _T("");
		ETC_strChampFormat[i] = _T("");
		ETC_strChampTitre[i] = _T("");
	}

//Extractions
	Etiq_bChargementOK = false;
	Etiq_strEtat = Etiq_strTitre + _T(" : Chargement OF");
	Etiq_iNbExtract = 0;
	Etiq_ID = 0;

	bResAcquisition_OK = FALSE;
	bResAcquisition_NOK = FALSE;
	bMixup = FALSE;
}

bool CEtiquette::Vision_Charger(MIL_ID idSystem)
{
	CString sTemp;

	milSys = idSystem;
	lX = 1600;		//V42 TODO a parametrer
	lY = 2400;		//V42 TODO a parametrer

	//Initialisation des extractions
	imgTrait.sLibelle.Format(_T("Etiq Traitement"));
	imgTrait.Charger(milSys,lX,lY,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	//Initialisation des extractions
	//V48 : image rotation carrée, au maximum de X ou Y
	long MaxSize = lX;
	if(lY>lX)	MaxSize = lY;
	imgRotation.sLibelle.Format(_T("Etiq Traitement Rotation"));
	imgRotation.Charger(milSys,MaxSize,MaxSize,1,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	return true;
}

void CEtiquette::Vision_DoExtraction()
{
	CString strForListe=_T("");

	iResultatEtiquette = DEFAUT_ETIQ_AUCUN;

	//Copie de l'image source 
	MbufCopy(imgSource->milBuf,imgTrait.milBuf);
	MbufClear(imgRotation.milBuf,0x00);

	strForListe = Etiq_strTitre +  _T(" -----------------------------------------------");
	((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strForListe);

	DecalageX = 0;
	DecalageY = 0;

	//Recherche de l'extraction référence
	for (int i = 0; i < Etiq_iNbExtract; i++)
	{
		if(Extract[i].lIsRepere == 1)
		{
			if(Vision_DoExtractionSingle(i))
			{
				DecalageX = (long)(PtCentreResult[i].Xpix - Extract[i].PtCentreBase.Xpix);
				DecalageY = (long)(PtCentreResult[i].Ypix - Extract[i].PtCentreBase.Ypix);
			}
			((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,Extract[i].strResult);	//V30
			break;
		}
	}

	//Boucle sur les extractions
	if(iResultatEtiquette == DEFAUT_ETIQ_AUCUN)		//V54 : On ne fait pas le reste si la référence n'est pas trouvée
	{
		for (int i = 0; i < Etiq_iNbExtract; i++)
		{
			if(Extract[i].lIsRepere == 0)
			{
				Vision_DoExtractionSingle(i);
				((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,Extract[i].strResult);	//V30
			}
		}
	}

	//Affichage en cas de test
	if(Etiq_bTest)
	{
		for (int i = 0; i < Etiq_iNbExtract; i++)
		{
			TracerCroix(imgVisu->milBuf,&PtCentreResult[i],10,COLOR_ROUGE);
			TracerROI(imgVisu,&RoiExtract[i],COLOR_CYAN);
			TracerROI(imgVisu,&RoiExtractBase[i],COLOR_BLEU);
		}
	}
	Etiq_bTest = false;
}

bool CEtiquette::Vision_DoExtractionSingle(int iNumExtract)
{
	bool bResult;

	Extract[iNumExtract].strResult=_T("");
	RoiExtract[iNumExtract].X1 = RoiExtractBase[iNumExtract].X1 + DecalageX;
	RoiExtract[iNumExtract].Y1 = RoiExtractBase[iNumExtract].Y1 + DecalageY;
	RoiExtract[iNumExtract].dX = RoiExtractBase[iNumExtract].dX;
	RoiExtract[iNumExtract].dY = RoiExtractBase[iNumExtract].dY;

	if(!imgTrait_roiExtract[iNumExtract].SetROI(&RoiExtract[iNumExtract]))
	{
		iResultatEtiquette = DEFAUT_ETIQ_EXTRACT;
		Extract[iNumExtract].strResult.Format(_T("ERRPOS %s"),Extract[iNumExtract].strLibelle);
		return false;
	}
	if(!imgVisu_roiExtract[iNumExtract].SetROI(&RoiExtract[iNumExtract]))
	{
		iResultatEtiquette = DEFAUT_ETIQ_EXTRACT;
		Extract[iNumExtract].strResult.Format(_T("ERRPOS %s"),Extract[iNumExtract].strLibelle);
		return false;
	}

	switch(Extract[iNumExtract].lOrientation)
	{
	case 90:
		MimRotate(imgTrait_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf,Extract[iNumExtract].lOrientation,RoiExtract[iNumExtract].dX/2,RoiExtract[iNumExtract].dY/2,RoiExtractRotation[iNumExtract].dX/2,RoiExtractRotation[iNumExtract].dY/2,M_DEFAULT);			
		break;
	case -90:
		MimRotate(imgTrait_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf,Extract[iNumExtract].lOrientation,RoiExtract[iNumExtract].dX/2,RoiExtract[iNumExtract].dY/2,RoiExtractRotation[iNumExtract].dX/2,RoiExtractRotation[iNumExtract].dY/2,M_DEFAULT);			
		break;
	case 180:
		MimRotate(imgTrait_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf,Extract[iNumExtract].lOrientation,RoiExtract[iNumExtract].dX/2,RoiExtract[iNumExtract].dY/2,RoiExtractRotation[iNumExtract].dX/2,RoiExtractRotation[iNumExtract].dY/2,M_DEFAULT);			
		break;
	default:
		//Rien a faire			
		break;
	}

	//V21 : Prétraitement Blobs
	if(Extract[iNumExtract].iType == TYPE_EXTRACT_BLOB)
	{
		//Traitement des images
		MbufCopy(imgTrait_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf);
		MimConvolve(imgRotation_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf,M_SOBEL);
		MimBinarize(imgRotation_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf,M_GREATER,50,M_NULL);
	}
	//V21 : Prétraitement Stat
	if(Extract[iNumExtract].iType == TYPE_EXTRACT_STAT)
	{
		//Traitement des images
		MbufCopy(imgTrait_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf);
		MimConvolve(imgRotation_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf,M_SOBEL);
		MimBinarize(imgRotation_roiExtract[iNumExtract].milBuf,imgRotation_roiExtract[iNumExtract].milBuf,M_GREATER,50,M_NULL);
	}


	if (Extract[iNumExtract].strCodeALire != _T(""))	//V44.5
	{
		MappControl(M_ERROR, M_PRINT_DISABLE);
		bResult = Extract[iNumExtract].Lire();
		MappControl(M_ERROR, M_PRINT_ENABLE);
	}
	else
	{
		bResult = true;
	}

	//Gestion du résultat
	switch (Extract[iNumExtract].iType)
	{
		case TYPE_EXTRACT_OCR:
			if(bResult)
			{
				if (Extract[iNumExtract].Reader.strLue[0].Find(Extract[iNumExtract].strCodeALire ,0) >= 0)
				{
					Extract[iNumExtract].Reader.DrawStringContour(Extract[iNumExtract].imgVisu,COLOR_VERT);
					Extract[iNumExtract].strResult.Format(_T("               %s : (lu %s)"),Extract[iNumExtract].strLibelle,Extract[iNumExtract].Reader.strLue[0]);
				}
				else
				{
					bResult = false;
					Extract[iNumExtract].Reader.DrawStringContour(Extract[iNumExtract].imgVisu,COLOR_ORANGE);
					Extract[iNumExtract].strResult.Format(_T("ERREUR %s : (lu %s)"),Extract[iNumExtract].strLibelle,Extract[iNumExtract].Reader.strLue[0]);
				}

				//Position du résultat
				Extract[iNumExtract].Reader.Get_ResultPositions();
				switch(Extract[iNumExtract].lOrientation)
				{
				case -90:
					RoiResult[iNumExtract].X1 = Extract[iNumExtract].Reader.resultBox.Y1;
					RoiResult[iNumExtract].Y1 = RoiExtractRotation[iNumExtract].dX-Extract[iNumExtract].Reader.resultBox.X2;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Reader.resultBox.dY;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Reader.resultBox.dX;

					PtCentreResult[iNumExtract].Xpix = Extract[iNumExtract].Reader.resultCentre.Ypix;
					PtCentreResult[iNumExtract].Ypix = RoiExtractRotation[iNumExtract].dX-Extract[iNumExtract].Reader.resultCentre.Xpix;;
					break;

				case 90:
					RoiResult[iNumExtract].Y1 =Extract[iNumExtract].Reader.resultBox.X1;
					RoiResult[iNumExtract].X1 = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Reader.resultBox.Y2;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Reader.resultBox.dY;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Reader.resultBox.dX;

					PtCentreResult[iNumExtract].Xpix = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Reader.resultCentre.Ypix;
					PtCentreResult[iNumExtract].Ypix = Extract[iNumExtract].Reader.resultCentre.Xpix;
					break;

				case 180:
					RoiResult[iNumExtract].X1 = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Reader.resultBox.X2;
					RoiResult[iNumExtract].Y1 = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Reader.resultBox.Y2;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Reader.resultBox.dX;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Reader.resultBox.dY;

					PtCentreResult[iNumExtract].Xpix = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Reader.resultCentre.Xpix;
					PtCentreResult[iNumExtract].Ypix = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Reader.resultCentre.Ypix;;
					break;

				default:
					RoiResult[iNumExtract].X1 = Extract[iNumExtract].Reader.resultBox.X1;
					RoiResult[iNumExtract].Y1 = Extract[iNumExtract].Reader.resultBox.Y1;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Reader.resultBox.dX;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Reader.resultBox.dY;

					PtCentreResult[iNumExtract].Xpix = Extract[iNumExtract].Reader.resultCentre.Xpix;
					PtCentreResult[iNumExtract].Ypix = Extract[iNumExtract].Reader.resultCentre.Ypix;;
					break;
				}

				RoiResult[iNumExtract].X1 = RoiResult[iNumExtract].X1 + RoiExtract[iNumExtract].X1;
				RoiResult[iNumExtract].Y1 = RoiResult[iNumExtract].Y1 + RoiExtract[iNumExtract].Y1;
				PtCentreResult[iNumExtract].Xpix = PtCentreResult[iNumExtract].Xpix + RoiExtract[iNumExtract].X1;
				PtCentreResult[iNumExtract].Ypix = PtCentreResult[iNumExtract].Ypix + RoiExtract[iNumExtract].Y1;
			}
			else
			{
				TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_ROUGE);
				Extract[iNumExtract].strResult.Format(_T("ERREUR %s : PAS DE LECTURE"),Extract[iNumExtract].strLibelle);
			}
			if(!bResult)
			{
				iResultatEtiquette = DEFAUT_ETIQ_EXTRACT;
			}
			break;

		case TYPE_EXTRACT_CODEBARRE:
			if(bResult)
			{
				if (Extract[iNumExtract].strResult.Find(Extract[iNumExtract].strCodeALire ,0) >= 0)
				{
					TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_VERT);
					Extract[iNumExtract].strResult.Format(_T("               %s : (lu %s)"),Extract[iNumExtract].strLibelle,Extract[iNumExtract].strResult);
				}
				else
				{
					bResult = false;
					TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_ORANGE);
					MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
					MgraText(M_DEFAULT,Extract[iNumExtract].imgVisu->milBuf,0,0,mText);
					Extract[iNumExtract].strResult.Format(_T("ERREUR %s : (lu %s)"),Extract[iNumExtract].strLibelle,Extract[iNumExtract].strResult);
				}

				//Position du résultat
				Extract[iNumExtract].Code.Get_ResultPositions();
				//V49
				switch(Extract[iNumExtract].lOrientation)
				{
				case -90:
					RoiResult[iNumExtract].X1 = Extract[iNumExtract].Code.resultBox.Y1;
					RoiResult[iNumExtract].Y1 = RoiExtractRotation[iNumExtract].dX-Extract[iNumExtract].Code.resultBox.X2;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dY;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dX;

					PtCentreResult[iNumExtract].Xpix = Extract[iNumExtract].Code.resultCentre.Ypix;
					PtCentreResult[iNumExtract].Ypix = RoiExtractRotation[iNumExtract].dX-Extract[iNumExtract].Code.resultCentre.Xpix;;
					break;

				case 90:
					RoiResult[iNumExtract].Y1 =Extract[iNumExtract].Code.resultBox.X1;
					RoiResult[iNumExtract].X1 = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultBox.Y2;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dY;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dX;

					PtCentreResult[iNumExtract].Xpix = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultCentre.Ypix;
					PtCentreResult[iNumExtract].Ypix = Extract[iNumExtract].Code.resultCentre.Xpix;
					break;

				case 180:
					RoiResult[iNumExtract].X1 = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Code.resultBox.X2;
					RoiResult[iNumExtract].Y1 = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultBox.Y2;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dX;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dY;

					PtCentreResult[iNumExtract].Xpix = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Code.resultCentre.Xpix;
					PtCentreResult[iNumExtract].Ypix = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultCentre.Ypix;;
					break;

				default:
					RoiResult[iNumExtract].X1 = Extract[iNumExtract].Code.resultBox.X1;
					RoiResult[iNumExtract].Y1 = Extract[iNumExtract].Code.resultBox.Y1;
					RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dX;
					RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dY;

					PtCentreResult[iNumExtract].Xpix = Extract[iNumExtract].Code.resultCentre.Xpix;
					PtCentreResult[iNumExtract].Ypix = Extract[iNumExtract].Code.resultCentre.Ypix;;
					break;
				}

				RoiResult[iNumExtract].X1 = RoiResult[iNumExtract].X1 + RoiExtract[iNumExtract].X1;
				RoiResult[iNumExtract].Y1 = RoiResult[iNumExtract].Y1 + RoiExtract[iNumExtract].Y1;
				PtCentreResult[iNumExtract].Xpix = PtCentreResult[iNumExtract].Xpix + RoiExtract[iNumExtract].X1;
				PtCentreResult[iNumExtract].Ypix = PtCentreResult[iNumExtract].Ypix + RoiExtract[iNumExtract].Y1;
			}
			else
			{
				TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_ROUGE);
				MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
				MgraText(M_DEFAULT,Extract[iNumExtract].imgVisu->milBuf,0,0,mText);
				Extract[iNumExtract].strResult.Format(_T("ERREUR %s : PAS DE LECTURE"),Extract[iNumExtract].strLibelle);
			}
			if(!bResult)
			{
				iResultatEtiquette = DEFAUT_ETIQ_EXTRACT;
			}
			break;

		case TYPE_EXTRACT_DMX:	//V44.5 : Ajout de cette extraction
			if (bResult)
			{
				if (Extract[iNumExtract].strResult.Find(Extract[iNumExtract].strCodeALire, 0) >= 0)
				{
					TracerROI(imgVisu, &RoiExtract[iNumExtract], COLOR_VERT);
					Extract[iNumExtract].strResult.Format(_T("               %s : (lu %s)"), Extract[iNumExtract].strLibelle, Extract[iNumExtract].strResult);
				}
				else
				{
					bResult = false;
					TracerROI(imgVisu, &RoiExtract[iNumExtract], COLOR_ORANGE);
					MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
					MgraText(M_DEFAULT, Extract[iNumExtract].imgVisu->milBuf, 0, 0, mText);
					Extract[iNumExtract].strResult.Format(_T("ERREUR %s : (lu %s)"), Extract[iNumExtract].strLibelle, Extract[iNumExtract].strResult);
				}

				//Position du résultat
				//V45.5 : Test si quelque chose à lire
				if (Extract[iNumExtract].strCodeALire != _T(""))	//V44.5
				{
					Extract[iNumExtract].Code.Get_ResultPositions();
					switch (Extract[iNumExtract].lOrientation)
					{
					case -90:
						RoiResult[iNumExtract].X1 = Extract[iNumExtract].Code.resultBox.Y1;
						RoiResult[iNumExtract].Y1 = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Code.resultBox.X2;
						RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dY;
						RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dX;

						PtCentreResult[iNumExtract].Xpix = Extract[iNumExtract].Code.resultCentre.Ypix;
						PtCentreResult[iNumExtract].Ypix = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Code.resultCentre.Xpix;;
						break;

					case 90:
						RoiResult[iNumExtract].Y1 = Extract[iNumExtract].Code.resultBox.X1;
						RoiResult[iNumExtract].X1 = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultBox.Y2;
						RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dY;
						RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dX;

						PtCentreResult[iNumExtract].Xpix = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultCentre.Ypix;
						PtCentreResult[iNumExtract].Ypix = Extract[iNumExtract].Code.resultCentre.Xpix;
						break;

					case 180:
						RoiResult[iNumExtract].X1 = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Code.resultBox.X2;
						RoiResult[iNumExtract].Y1 = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultBox.Y2;
						RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dX;
						RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dY;

						PtCentreResult[iNumExtract].Xpix = RoiExtractRotation[iNumExtract].dX - Extract[iNumExtract].Code.resultCentre.Xpix;
						PtCentreResult[iNumExtract].Ypix = RoiExtractRotation[iNumExtract].dY - Extract[iNumExtract].Code.resultCentre.Ypix;;
						break;

					default:
						RoiResult[iNumExtract].X1 = Extract[iNumExtract].Code.resultBox.X1;
						RoiResult[iNumExtract].Y1 = Extract[iNumExtract].Code.resultBox.Y1;
						RoiResult[iNumExtract].dX = Extract[iNumExtract].Code.resultBox.dX;
						RoiResult[iNumExtract].dY = Extract[iNumExtract].Code.resultBox.dY;

						PtCentreResult[iNumExtract].Xpix = Extract[iNumExtract].Code.resultCentre.Xpix;
						PtCentreResult[iNumExtract].Ypix = Extract[iNumExtract].Code.resultCentre.Ypix;;
						break;
					}

					RoiResult[iNumExtract].X1 = RoiResult[iNumExtract].X1 + RoiExtract[iNumExtract].X1;
					RoiResult[iNumExtract].Y1 = RoiResult[iNumExtract].Y1 + RoiExtract[iNumExtract].Y1;
					PtCentreResult[iNumExtract].Xpix = PtCentreResult[iNumExtract].Xpix + RoiExtract[iNumExtract].X1;
					PtCentreResult[iNumExtract].Ypix = PtCentreResult[iNumExtract].Ypix + RoiExtract[iNumExtract].Y1;
				}
				else
				{
					RoiResult[iNumExtract].X1 = RoiExtract[iNumExtract].X1;
					RoiResult[iNumExtract].Y1 = RoiExtract[iNumExtract].Y1;
					RoiResult[iNumExtract].dX = RoiExtract[iNumExtract].dX;
					RoiResult[iNumExtract].dY = RoiExtract[iNumExtract].dY;
					PtCentreResult[iNumExtract].Xpix = RoiExtract[iNumExtract].X1 + RoiExtract[iNumExtract].dX/2;
					PtCentreResult[iNumExtract].Ypix =RoiExtract[iNumExtract].Y1 + RoiExtract[iNumExtract].dY/2;
				}
			}
			else
			{
				TracerROI(imgVisu, &RoiExtract[iNumExtract], COLOR_ROUGE);
				MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
				MgraText(M_DEFAULT, Extract[iNumExtract].imgVisu->milBuf, 0, 0, mText);
				Extract[iNumExtract].strResult.Format(_T("ERREUR %s : PAS DE LECTURE"), Extract[iNumExtract].strLibelle);
			}
			if (!bResult)
			{
				iResultatEtiquette = DEFAUT_ETIQ_EXTRACT;
			}
			break;

		case TYPE_EXTRACT_BLOB:	//V18
			if(bResult)
			{
				long lSeuil = 0;
				lSeuil = atoi(Extract[iNumExtract].strCodeALire);
				long lPourcentage = 0;
				if(Extract[iNumExtract].lVal1>0)
				{
					lPourcentage = Extract[iNumExtract].lResult * 100 / Extract[iNumExtract].lVal1;
				}
				else
				{
					lPourcentage = -1;	//Générer une erreur
				}
				if (lPourcentage > lSeuil)
				{
					TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_VERT);
					Extract[iNumExtract].strResult.Format(_T("               %s : (Resultat %ld = %ld/%ld)"),Extract[iNumExtract].strLibelle,lPourcentage,Extract[iNumExtract].lResult,Extract[iNumExtract].lVal1);
				}
				else
				{
					bResult = false;
					TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_ORANGE);
					MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
					MgraText(M_DEFAULT,Extract[iNumExtract].imgVisu->milBuf,0,0,mText);
					Extract[iNumExtract].strResult.Format(_T("ERREUR %s : (Resultat %ld = %ld/%ld)"),Extract[iNumExtract].strLibelle,lPourcentage,Extract[iNumExtract].lResult,Extract[iNumExtract].lVal1);
				}
				PtCentreResult[iNumExtract].Xpix = RoiExtract[iNumExtract].X1 + RoiExtract[iNumExtract].dX / 2;
				PtCentreResult[iNumExtract].Ypix = RoiExtract[iNumExtract].Y1 + RoiExtract[iNumExtract].dY / 2;
			}
			else
			{
				bResult = false;
				TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_ROUGE);
				MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
				MgraText(M_DEFAULT,Extract[iNumExtract].imgVisu->milBuf,0,0,mText);
				Extract[iNumExtract].strResult.Format(_T("ERREUR %s : PAS DE LECTURE"),Extract[iNumExtract].strLibelle);
			}
			if(!bResult)
			{
				iResultatEtiquette = DEFAUT_ETIQ_EXTRACT;
			}
			break;

		case TYPE_EXTRACT_STAT:	//V18
			if(bResult)
			{
				long lSeuil = 0;
				lSeuil = atoi(Extract[iNumExtract].strCodeALire);
				long lPourcentage = 0;
				if(Extract[iNumExtract].lVal1>0)
				{
					lPourcentage = Extract[iNumExtract].lResult * 100 / Extract[iNumExtract].lVal1;
				}
				else
				{
					lPourcentage = -1;	//Générer une erreur
				}
				if (lPourcentage > lSeuil)
				{
					TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_VERT);
					Extract[iNumExtract].strResult.Format(_T("               %s : (Resultat %ld = %ld/%ld)"),Extract[iNumExtract].strLibelle,lPourcentage,Extract[iNumExtract].lResult,Extract[iNumExtract].lVal1);
				}
				else
				{
					bResult = false;
					TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_ORANGE);
					MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
					MgraText(M_DEFAULT,Extract[iNumExtract].imgVisu->milBuf,0,0,mText);
					Extract[iNumExtract].strResult.Format(_T("ERREUR %s : (Resultat %ld = %ld/%ld)"),Extract[iNumExtract].strLibelle,lPourcentage,Extract[iNumExtract].lResult,Extract[iNumExtract].lVal1);
				}
				PtCentreResult[iNumExtract].Xpix = RoiExtract[iNumExtract].X1 + RoiExtract[iNumExtract].dX / 2;
				PtCentreResult[iNumExtract].Ypix = RoiExtract[iNumExtract].Y1 + RoiExtract[iNumExtract].dY / 2;
			}
			else
			{
				bResult = false;
				TracerROI(imgVisu,&RoiExtract[iNumExtract],COLOR_ROUGE);
				MIL_TEXT_PTR mText = Extract[iNumExtract].strLibelle.GetBuffer(Extract[iNumExtract].strLibelle.GetLength());
				MgraText(M_DEFAULT,Extract[iNumExtract].imgVisu->milBuf,0,0,mText);
				Extract[iNumExtract].strResult.Format(_T("ERREUR %s : PAS DE LECTURE"),Extract[iNumExtract].strLibelle);
			}
			if(!bResult)
			{
				iResultatEtiquette = DEFAUT_ETIQ_EXTRACT;
			}
			break;
	}

	return bResult;
}

void CEtiquette::Vision_Decharger()
{

	for (int i = 0; i < NB_EXTRACTIONS; i++)
	{
		Extract[i].Decharger();
		
		imgVisu_roiExtract[i].Decharger();
		imgTrait_roiExtract[i].Decharger();
		imgRotation_roiExtract[i].Decharger();		//V25

	}

	imgTrait.Decharger();
	imgRotation.Decharger();
}
