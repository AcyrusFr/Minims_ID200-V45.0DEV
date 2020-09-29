//BMx
#include "stdafx.h"
#include "Extraction.h"
#include "AC_Base.h"
#include "General.h"

CExtraction::CExtraction()
{ 
	img = NULL;

	iNumExtract = -1;
	iType = TYPE_EXTRACT_UNDEF;
	strFicModele = _T("");
	strCodeALire = _T("");
	strLibelle = _T("");
	strResult = _T("");

	bVisualisation = false;
	ColorVisu = COLOR_BLANC;
	imgVisu = NULL;

	lIsRepere = 0;
	PtCentreBase.Xpix = 0;
	PtCentreBase.Ypix = 0;
	lContrainte = 0;
	lVal1 = 0;
	lResult = 0;	//V21

	Extract_bIsOK = false;//V 44.1
}


CExtraction::~CExtraction(void)
{
	//Au cas où on aurait oublié le delete...
	Reader.Decharger();
	Modele.Decharger();
	Code.Decharger();
}


bool CExtraction::Lire()
{
	int iNnBlob = 0;
	MIL_INT ValTemp = 0;

	strResult = _T("");
	if (img == NULL)return true;
	if (iType == TYPE_EXTRACT_UNDEF) return true;

	COLORREF LocalColor = COLOR_BLEU;

	bool bReturn = false;
	switch (iType)
	{
	case TYPE_EXTRACT_OCR:
		Reader.mImg = img;
		bReturn = Reader.Trouver();
		if (bReturn)
		{
			strResult = Reader.strLue[0];
			LocalColor = COLOR_VERT;
		}
		break;

	case TYPE_EXTRACT_MODELE:
		Modele.mImg = img;
		bReturn = Modele.Trouver();
		break;

	case TYPE_EXTRACT_DMX:
	case TYPE_EXTRACT_CODEBARRE:
		Code.mImg = img;
		bReturn = Code.Trouver();

		if (bReturn)
		{
			strResult = CString(Code.BarcodeString);
		}
		break;

	case TYPE_EXTRACT_BLOB:
		lResult = BlobEtiq.Calculer(img->milBuf);
		lResult = BlobEtiq.Selectionner(M_EXCLUDE, M_AREA, M_LESS_OR_EQUAL, 50, M_NULL);
		bReturn = true;
		break;

	case TYPE_EXTRACT_STAT:
		MimStat(img->milBuf,StatImp.milResult,M_SUM,M_NULL,M_NULL,M_NULL);
		MimGetResult(StatImp.milResult, M_SUM+M_TYPE_MIL_INT, &ValTemp);
		lResult = (long)(ValTemp / 256);
		bReturn = true;
		break;

	case TYPE_EXTRACT_UNDEF:
	default:
		break;
	}

	return bReturn;
}

bool CExtraction::Charger(void)
{
	//Récupération pointeur sur image
	if (img == NULL)return false;
	
	bool bReturn = false;

	//Chargement Vision de l'extraction
	switch (iType)
	{
	case TYPE_EXTRACT_OCR:
		Reader.System = &img->milSys;
		Reader.strFile = strFicModele;

		//V 44.0
		if (!TrouverFichier(Reader.strFile))//V 44.1 Correction
		{
			break;
		}

		bReturn = Reader.Charger();
		break;

	case TYPE_EXTRACT_MODELE:
		Modele.MF_System = &img->milSys;
		Modele.strFile = strFicModele;

		//V 44.0
		if (!TrouverFichier(Modele.strFile))//V 44.1 Correction
		{
			break;
		}
		bReturn = Modele.Charger();
		break;

	case TYPE_EXTRACT_DMX:
	case TYPE_EXTRACT_CODEBARRE:
		Code.MF_System = &img->milSys;
		Code.MF_File = strFicModele;
		//V 44.0
		if (!TrouverFichier(Code.MF_File))//V 44.1 Correction
		{
			break;
		}
		bReturn = Code.ChargerFromFile();
		break;

	case TYPE_EXTRACT_BLOB:
		BlobEtiq.MF_System =  &img->milSys;
		BlobEtiq.Charger(5000,false);
		bReturn = true;//V 44.1 
		break;

	case TYPE_EXTRACT_STAT:
		StatImp.Decharger();
		StatImp.Charger(img->milSys,1,M_STAT_LIST);	//Moyenne image
		bReturn = true;//V 44.1 
		break;

	default:
		break;
	}

	return bReturn;//V 44.1 
}

bool CExtraction::PreProcess(void)
{
	//Récupération pointeur sur image
	if (img == NULL)return false;
	
	bool bReturn = false;

	//Chargement Vision de l'extraction
	switch (iType)
	{
	case TYPE_EXTRACT_OCR:
		Reader.PreProcess();
		break;

	case TYPE_EXTRACT_MODELE:
		break;

	case TYPE_EXTRACT_DMX:
	case TYPE_EXTRACT_CODEBARRE:
		break;

	case TYPE_EXTRACT_BLOB:
		break;

	case TYPE_EXTRACT_STAT:
		break;

	case TYPE_EXTRACT_UNDEF:
	default:
		break;
	}
	return bReturn;
}


bool CExtraction::Decharger(void)
{
	switch (iType)
	{
	case TYPE_EXTRACT_OCR:
		Reader.Decharger();
		break;

	case TYPE_EXTRACT_MODELE:
		Modele.Decharger();
		break;

	case TYPE_EXTRACT_DMX:
	case TYPE_EXTRACT_CODEBARRE:

		Code.Decharger();
		break;

	case TYPE_EXTRACT_BLOB:
		BlobEtiq.Decharger();	//V25
		break;

	case TYPE_EXTRACT_STAT:
		StatImp.Decharger();//V 44.0 MsysFree
		break;

	case TYPE_EXTRACT_UNDEF:
	default:
		break;
	}
	return true;
}

void CExtraction::OpenInteractiveWindow(void)
{
	switch (iType)
	{
	case TYPE_EXTRACT_OCR:
		Reader.ModifyParamsContext();
		break;

	case TYPE_EXTRACT_MODELE:
		Modele.ModifyParamsContext();
		break;

	case TYPE_EXTRACT_DMX:
	case TYPE_EXTRACT_CODEBARRE:
		Code.ModifyParamsCode();
		break;

	case TYPE_EXTRACT_BLOB:
		break;

	case TYPE_EXTRACT_STAT:
		break;

	case TYPE_EXTRACT_UNDEF:
	default:
		break;
	}
}
bool CExtraction::UpdateFichier(void)
{
	//Maj de la BDD avec les infos en cours
	CString strCritere2 = _T("");
	CADORecordset rsDetail(&theBase.dao_Base);

	strCritere2.Format(_T("ID = '%ld'"), IDDetail);
	if (ACBaseReqRsOpen(strCritere2, _T("[EtiqDetail]"), &rsDetail))
	{
		rsDetail.Edit();

		ACBaseSetStr(strFicModele, _T("FicModele"), &rsDetail);

		rsDetail.Update();
		rsDetail.Close();
	}

	return true;
}
