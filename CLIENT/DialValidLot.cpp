// DialValidLot.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialValidLot.h"


// Boîte de dialogue CDialValidLot

IMPLEMENT_DYNAMIC(CDialValidLot, CAcyDial)

CDialValidLot::CDialValidLot(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialValidLot::IDD, pParent)
	, strNumLot(_T(""))
	//,Menu_ValidLotUnlock("Menu_ValidLotUnlock")
	//,Menu_ValidLot("Menu_ValidLot")
	, Menu_ValidLotUnlock(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_ValidLotUnlock"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
	, Menu_ValidLot(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_ValidLot"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	bAllPesons = true;
	bDebutLot = true;
	iPesonSelected = -1;

	m_ClientOPC = NULL;
	Recette = NULL;
}

CDialValidLot::~CDialValidLot()
{
}

void CDialValidLot::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, GrilleValidLot);
	DDX_Text(pDX, IDC_STATIC_NUMLOT, strNumLot);
	DDX_Control(pDX, IDC_STATICMENU_VALIDLOT, Menu_ValidLotUnlock);
	DDX_Control(pDX, IDC_STATICMENU_VALIDLOT2, Menu_ValidLot);
	DDX_Control(pDX, IDC_STATIC_NUMLOT, static_Lot);
}


BEGIN_MESSAGE_MAP(CDialValidLot, CAcyDial)

	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialValidLot

BOOL CDialValidLot::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	CString strTemp = _T("");
	static_Lot.GetWindowText(strTemp);
	static_Lot.strText = strTemp;

	static_Lot.iSizePolice = 15;
	static_Lot.bSouligne = TRUE;

	//Initialisation de la grille
	GrilleValidLot.SetDefCellHeight(25);//hauteur des cellules
	GrilleValidLot.SetColumnCount(5);
	GrilleValidLot.SetFixedRowCount(1);//légende fixe
	GrilleValidLot.SetRowCount(13);
	GrilleValidLot.SetColumnResize(FALSE);
	GrilleValidLot.ExpandColumnsToFit(TRUE);
	GrilleValidLot.ExpandRowsToFit(TRUE);
	GrilleValidLot.SetListMode(TRUE);
	GrilleValidLot.SetSingleRowSelection(TRUE);
	GrilleValidLot.SetSingleColSelection(TRUE);
	GrilleValidLot.SetFixedColumnSelection(FALSE);
	GrilleValidLot.SetFixedRowSelection(FALSE);
	//légende
	GrilleValidLot.SetFixedBkColor(RGB(0,46,184));
	GrilleValidLot.SetFixedTextColor(RGB(255,255,255));
	GrilleValidLot.SetItemText(0,0,_T("Peson"));
	GrilleValidLot.SetItemText(0,1,_T("Nominal"));
	GrilleValidLot.SetItemText(0,2,_T("Minimal"));
	GrilleValidLot.SetItemText(0,3,_T("Maximal"));
	GrilleValidLot.SetItemText(0,4,_T("Valeur"));

	UpdatePesonWindow();

	Menu_ValidLotUnlock.strProfil = theApp.CurrentUser.strProfil;
	Menu_ValidLotUnlock.SendMessage(WM_CREATE,NULL,NULL);

	Menu_ValidLot.bVertical = true;
	Menu_ValidLot.strProfil = theApp.CurrentUser.strProfil;
	Menu_ValidLot.SendMessage(WM_CREATE,NULL,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

int CDialValidLot::UpdatePesonWindow(void)
{
	CString strTemp = "";
	CGridCellBase * cell = NULL;

	if(Recette == NULL) return 0;

	if(bDebutLot) Menu_ValidLotUnlock.ShowWindow(SW_HIDE);
	else Menu_ValidLotUnlock.ShowWindow(SW_SHOW);

	strNumLot = _T("Lot N° : ") + Recette->strLot;
	UpdateData(FALSE);

	if(!bAllPesons)
	{
		if((iPesonSelected < 0) && (iPesonSelected > 5)) return 0;
		
		ResetGrille();
		
		strTemp.Format(_T("Peson %d Etalon1"),theApp.Pesons[iPesonSelected].iNumPesonBL);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 1 ,0,strTemp);
		strTemp.Format(_T("%2.3f g"),pParamPoidsEtalon1->value);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 1 ,1,strTemp);
		strTemp.Format(_T("%2.3f g"),pParamPoidsMinEtalon1->value);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 1 ,2,strTemp);
		strTemp.Format(_T("%2.3f g"),pParamPoidsMaxEtalon1->value);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 1 ,3,strTemp);

		strTemp.Format(_T("%2.3f g"),theApp.Pesons[iPesonSelected].PoidsEtalon1/1000.);
		cell = GrilleValidLot.GetCell(iPesonSelected*2 + 1 ,4);
		cell->SetText(strTemp);

		//changement de couleur de la cellule
		if(theApp.Pesons[iPesonSelected].PesonMode == PESON_MODEOFF)
		{
			//Gris
			cell->SetBackClr(RGB(236,233,216));
		}
		else
		{
			//changement de couleur de la cellule
			if(theApp.Pesons[iPesonSelected].StatutEtalon1 == PESON_STATUTETALON_NOK)
			{
				//Rouge
				cell->SetBackClr(RGB(255,0,0));
			}
			else
			{
				//Vert
				cell->SetBackClr(RGB(0,255,0));
			}
		}

		strTemp.Format(_T("Peson %d Etalon2"),theApp.Pesons[iPesonSelected].iNumPesonBL);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 2 ,0,strTemp);
		//BMPeson
		//strTemp.Format(_T("%2.3f g"),Recette->PoidsNomEtalon[1]);
		strTemp.Format(_T("%2.3f g"),pParamPoidsEtalon2->value);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 2 ,1,strTemp);
		//BMPeson
		//strTemp.Format(_T("%2.3f g"),Recette->PoidsMinEtalon[1]);
		strTemp.Format(_T("%2.3f g"),pParamPoidsMinEtalon2->value);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 2 ,2,strTemp);
		//BMPeson
		//strTemp.Format(_T("%2.3f g"),Recette->PoidsMaxEtalon[1]);
		strTemp.Format(_T("%2.3f g"),pParamPoidsMaxEtalon2->value);
		GrilleValidLot.SetItemText(iPesonSelected*2 + 2 ,3,strTemp);

		strTemp.Format(_T("%2.3f g"),theApp.Pesons[iPesonSelected].PoidsEtalon2/1000.);
		cell = GrilleValidLot.GetCell(iPesonSelected*2 + 2 ,4);
		cell->SetText(strTemp);

		if(theApp.Pesons[iPesonSelected].PesonMode == PESON_MODEOFF)
		{
			//Gris
			cell->SetBackClr(RGB(236,233,216));
		}
		else
		{
			//changement de couleur de la cellule
			if(theApp.Pesons[iPesonSelected].StatutEtalon1 == PESON_STATUTETALON_NOK)
			{
				//Rouge
				cell->SetBackClr(RGB(255,0,0));
			}
			else
			{
				//Vert
				cell->SetBackClr(RGB(0,255,0));
			}
		}
		
	}
	else
	{
		for(int i = 0; i < 6;i++)
		{
			strTemp.Format(_T("Peson %d Etalon1"),theApp.Pesons[i].iNumPesonBL);
			GrilleValidLot.SetItemText(i*2 + 1 ,0,strTemp);
			strTemp.Format(_T("%2.3f g"),pParamPoidsEtalon1->value);
			GrilleValidLot.SetItemText(i*2 + 1 ,1,strTemp);
			strTemp.Format(_T("%2.3f g"),pParamPoidsMinEtalon1->value);
			GrilleValidLot.SetItemText(i*2 + 1 ,2,strTemp);
			strTemp.Format(_T("%2.3f g"),pParamPoidsMaxEtalon1->value);
			GrilleValidLot.SetItemText(i*2 + 1 ,3,strTemp);

			strTemp.Format(_T("%2.3f g"),theApp.Pesons[i].PoidsEtalon1/1000.);
			cell = GrilleValidLot.GetCell(i*2 + 1 ,4);
			cell->SetText(strTemp);

			if(theApp.Pesons[i].PesonMode == PESON_MODEOFF)
			{
				//Gris
				cell->SetBackClr(RGB(236,233,216));
			}
			else
			{
				//changement de couleur de la cellule
				if(theApp.Pesons[i].StatutEtalon1 == PESON_STATUTETALON_NOK)
				{
					//Rouge
					cell->SetBackClr(RGB(255,0,0));
				}
				else
				{
					//Vert
					cell->SetBackClr(RGB(0,255,0));
				}
			}

			strTemp.Format(_T("Peson %d Etalon2"),theApp.Pesons[i].iNumPesonBL);
			GrilleValidLot.SetItemText(i*2 + 2 ,0,strTemp);
			strTemp.Format(_T("%2.3f g"),pParamPoidsEtalon2->value);
			GrilleValidLot.SetItemText(i*2 + 2 ,1,strTemp);
			strTemp.Format(_T("%2.3f g"),pParamPoidsMinEtalon2->value);
			GrilleValidLot.SetItemText(i*2 + 2 ,2,strTemp);
			strTemp.Format(_T("%2.3f g"),pParamPoidsMaxEtalon2->value);
			GrilleValidLot.SetItemText(i*2 + 2 ,3,strTemp);

			strTemp.Format(_T("%2.3f g"),theApp.Pesons[i].PoidsEtalon2/1000.);
			cell = GrilleValidLot.GetCell(i*2 + 2 ,4);
			cell->SetText(strTemp);

			//changement de couleur de la cellule
			if(theApp.Pesons[i].PesonMode == PESON_MODEOFF)
			{
				//Gris
				cell->SetBackClr(RGB(236,233,216));
			}
			else
			{
				//changement de couleur de la cellule
				if(theApp.Pesons[i].StatutEtalon2 == PESON_STATUTETALON_NOK)
				{
					//Rouge
					cell->SetBackClr(RGB(255,0,0));
				}
				else
				{
					//Vert
					cell->SetBackClr(RGB(0,255,0));
				}
			}
		}
	}

	GrilleValidLot.Refresh();
	return 0;
}

int CDialValidLot::ResetGrille(void)
{

	for(int i = 1; i < GrilleValidLot.GetRowCount(); i++)
	{
		for(int y = 0; y < GrilleValidLot.GetColumnCount(); y++)
		{
			GrilleValidLot.SetItemText(i,y,_T(""));
			GrilleValidLot.SetItemBkColour(i,y,RGB(255,255,255));
		}

	}
	return 0;
}

void CDialValidLot::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
		case ID_TIMER_VALIDPESONS:
			UpdatePesonWindow();
		break;

		default:
			break;
	}

	CAcyDial::OnTimer(nIDEvent);
}

int CDialValidLot::StartLecture(void)
{
	//Lancement timer pour lecture priodique des valeurs
	TimerLecture = SetTimer(ID_TIMER_VALIDPESONS,200,NULL);

	return 0;
}

int CDialValidLot::StopLecture(void)
{
	KillTimer(TimerLecture);
	return 0;
}

void CDialValidLot::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CAcyDial::OnShowWindow(bShow, nStatus);

	// Gestion du timer d'affichage 
	if(bShow)
		StartLecture();
	else
		StopLecture();
}
BOOL CDialValidLot::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// Interception de la touche ENTREE et ECHAP GCA 1.14

	CWnd * pWnd = GetFocus();
	switch(wParam)
	{
	case IDOK:
		if(pWnd!=GetDlgItem(IDOK))
			return FALSE;
		break;
	case IDCANCEL:
		if(pWnd!=GetDlgItem(IDCANCEL))
			return FALSE;
		break;
	}


	return CAcyDial::OnCommand(wParam, lParam);
}