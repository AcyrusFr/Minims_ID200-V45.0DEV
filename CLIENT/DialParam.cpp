// DialParam.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialParam.h"
#include "AC_Util.h"


// Boîte de dialogue CDialParam

IMPLEMENT_DYNAMIC(CDialParam, CAcyDial)

CDialParam::CDialParam(CWnd* pParent /*=NULL*/,CJeuParametres* pJParamParent /*=NULL*/)
	: CAcyDial(CDialParam::IDD, pParent)
	//,Menu_Params(_T("Menu_Params"))
	, Menu_Params(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Params"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	CurrentJParam = pJParamParent;
	GrilleParam.sLibelle = _T("Params_APP");
}

CDialParam::~CDialParam()
{
}

void CDialParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, GrilleParam);
	DDX_Control(pDX, IDC_STATICMENU_PARAMS, Menu_Params);
}


BEGIN_MESSAGE_MAP(CDialParam, CAcyDial)
	ON_NOTIFY(GVN_COLUMNCLICK,IDC_GRILLE,OnGridColClick)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialParam
void CDialParam::SetDimGridToReg(CConfig * RegConfig, CGridCtrl * pGrid)
{

	CString strFormat, strTemp;

	//Initialisation chaine Format 
	for (int i = 0; i < pGrid->GetColumnCount();i++)
	{
		strTemp.Format(_T("%d/"), pGrid->GetColumnWidth(i));
		strFormat += strTemp;
	}

	//Sauvegarde de la cle Format
	RegConfig->SetStringValue(_T("DimGrilles"), pGrid->sLibelle, strFormat);

}
void CDialParam::GetDimGridToReg(CConfig * RegConfig, CGridCtrl * pGrid)
{
	CString strFormat, strTemp;

	//Initialisation Format par defaut
	for (int i = 0; i < pGrid->GetColumnCount();i++)
	{
		strTemp.Format(_T("%d/"), pGrid->GetColumnWidth(i));
		strFormat += strTemp;
	}

	//Recuperation de la cle Format
	strFormat = RegConfig->GetStringValue(_T("DimGrilles"), pGrid->sLibelle, strFormat);

	//Resize des colonnes
	int index = 0;
	int TailleColonne = 0;
	int iColonne = 0;

	do
	{
		strTemp = strFormat.Tokenize(_T("/"), index);
		if (index < 0) break;
#ifdef _UNICODE
		TailleColonne = _wtoi(strTemp);
#else
		TailleColonne = atoi(strTemp);
#endif
		if (iColonne >= pGrid->GetColumnCount())
			break;

		pGrid->SetColumnWidth(iColonne, TailleColonne);
		iColonne++;

	} while (index > 0);

	pGrid->Refresh();
}
void CDialParam::OnGridColClick(NMHDR* Nmhdr,LRESULT* lRes)
{
	SetDimGridToReg(&theConfig,&GrilleParam);
}
void CDialParam::OnBnClickedButEdit()
{
	// Edition Parametre En cours
	bool bParamModif = false;
	bParamModif = CurrentJParam->EditSelectedParam();
	CurrentJParam->SauvegarderParametres();
	CurrentJParam->RefreshGrille();

	//Traca Evenement Modif valeur Paramétre
	if (bParamModif)
	{
		int IndexParam = CurrentJParam->LastUpdateIndex;
		CString strLibelle,strCom;

		strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_MODIF_PARAM_F),CurrentJParam->Parametres[IndexParam]->label);
			
		switch(CurrentJParam->Parametres[IndexParam]->type)
		{
			case TYPEDOUBLE:
				strCom.Format(_T("Avant: %2.3f | Après: %2.3f"),CurrentJParam->Parametres[IndexParam]->dVal_Prev,((CParametreDouble*)CurrentJParam->Parametres[IndexParam])->value);
				break;
			case TYPESTRING:
				strCom.Format(_T("Avant: %s | Après: %s"),CurrentJParam->Parametres[IndexParam]->str_Prev,((CParametreString*)CurrentJParam->Parametres[IndexParam])->value);
				break;
			case TYPELONG:
				strCom.Format(_T("Avant: %ld | Après: %ld"),CurrentJParam->Parametres[IndexParam]->lVal_Prev,((CParametreLong*)CurrentJParam->Parametres[IndexParam])->value);
				break;

			default:
				break;
		}
			
		theApp.theJournal.Evenement(CurrentJParam->TypeTraca,strLibelle,strCom,theApp.CurrentUser.strCode);
	}
			
}

void CDialParam::OnBnClickedRadGeneral()
{
	// Jeu Paramétre General
	CurrentJParam->pGrille = NULL;
	JParamGeneral.pGrille = &GrilleParam;
	CurrentJParam= &JParamGeneral;
	JParamGeneral.RefreshGrille();

	GetDimGridToReg(&theConfig,&GrilleParam);
}

void CDialParam::OnBnClickedRadVision()
{
	// Jeu Paramétre Vision
	CurrentJParam->pGrille = NULL;

	JParamVisionGeneral.pGrille = &GrilleParam;
	CurrentJParam= &JParamVisionGeneral;
	JParamVisionGeneral.RefreshGrille();

	GetDimGridToReg(&theConfig,&GrilleParam);
}

void CDialParam::OnBnClickedRadCam1()
{
	// Jeu Paramétre CAM 1
	CurrentJParam->pGrille = NULL;
	JParamuEye[ID_CAM_UEYE1].pGrille = &GrilleParam;
	CurrentJParam= &JParamuEye[ID_CAM_UEYE1];
	JParamuEye[ID_CAM_UEYE1].RefreshGrille();

	GetDimGridToReg(&theConfig,&GrilleParam);
}

void CDialParam::OnBnClickedRadCam2()
{
	// Jeu Paramétre CAM 2
	CurrentJParam->pGrille = NULL;
	JParamuEye[ID_CAM_UEYE2].pGrille = &GrilleParam;
	CurrentJParam= &JParamuEye[ID_CAM_UEYE2];
	JParamuEye[ID_CAM_UEYE2].RefreshGrille();

	GetDimGridToReg(&theConfig,&GrilleParam);
}

BOOL CDialParam::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Jeu de parametre par défaut

	JParamGeneral.pGrille = &GrilleParam;
	CurrentJParam = &JParamGeneral;
	JParamGeneral.RefreshGrille();

	GetDimGridToReg(&theConfig,&GrilleParam);
	
	//Initialisation du menu
	Menu_Params.bVertical = true;
	Menu_Params.strProfil = theApp.CurrentUser.strProfil;
	Menu_Params.SendMessage(WM_CREATE,NULL,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

BOOL CDialParam::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// Interception de la touche ENTREE et ECHAP

	return CAcyDial::OnCommand(wParam, lParam);
}

void  CDialParam::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{

	if(strFonction == "PARAM_SELECTGENERAL")
	{
		OnBnClickedRadGeneral();
	}
	else if(strFonction == "PARAM_SELECTVISION")
	{
		OnBnClickedRadVision();
	}
	else if(strFonction == "PARAM_SELECTCAM1")
	{
		OnBnClickedRadCam1();
	}
	else if(strFonction == "PARAM_SELECTCAM2")
	{
		OnBnClickedRadCam2();
	}
	else if(strFonction == "PARAM_EDIT")
	{
		OnBnClickedButEdit();
	}
	
}