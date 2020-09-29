// DialPesee.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialPesee.h"
#include "afxdialogex.h"


// Boîte de dialogue CDialPesee

IMPLEMENT_DYNAMIC(CDialPesee, CAcyDial)

CDialPesee::CDialPesee(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialPesee::IDD, pParent)
	//,Menu_PesonEtui("Menu_PesonEtui")
	, Menu_PesonEtui(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_PesonEtui"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{

}

CDialPesee::~CDialPesee()
{
}

void CDialPesee::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MENU_PESON, Menu_PesonEtui);
	DDX_Control(pDX, IDC_LIST2, m_ListeHisto);
	DDX_Control(pDX, IDC_GRILLEETATS, GrilleEtats);
}


BEGIN_MESSAGE_MAP(CDialPesee, CAcyDial)
	ON_MESSAGE(WM_PESON_MAJMESURE,OnMAJMesure)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialPesee


BOOL CDialPesee::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation de la grille des Etats
	GrilleEtats.SetDefCellHeight(25);//hauteur des cellules
	GrilleEtats.SetColumnCount(2);
	GrilleEtats.SetFixedRowCount(1);//légende fixe
	GrilleEtats.SetRowCount(13);
	GrilleEtats.ExpandColumnsToFit(TRUE);
	GrilleEtats.SetListMode(TRUE);
	GrilleEtats.SetSingleRowSelection(TRUE);
	GrilleEtats.SetSingleColSelection(TRUE);
	GrilleEtats.SetFixedColumnSelection(FALSE);
	GrilleEtats.SetFixedRowSelection(FALSE);
	//légende
	CGridCellBase* cellule;
	GrilleEtats.SetFixedBkColor(RGB(0,46,184));
	GrilleEtats.SetFixedTextColor(RGB(255,255,255));
	cellule = GrilleEtats.GetCell(0,0);
	cellule->SetText(_T("Paramétre"));
	cellule = GrilleEtats.GetCell(0,1);
	cellule->SetText(_T("Etat"));

	//Légende
	GrilleEtats.SetItemText(1,0,_T("Etat"));
	GrilleEtats.SetItemText(2,0,_T("Derniére Mesure (cg)"));
	GrilleEtats.SetItemText(3,0,_T("Statut Derniére Mesure"));
	GrilleEtats.SetItemText(4,0,_T("Valeur dynamique stabilisée (cg)"));
	GrilleEtats.SetItemText(5,0,_T("Poids Etalon 1"));
	GrilleEtats.SetItemText(6,0,_T("Statut Etalon 1")); //0->lu par vision; 1->pesage OK; 2-> Pesage NOK (à archiver)
	GrilleEtats.SetItemText(7,0,_T("Poids Etalon 2"));
	GrilleEtats.SetItemText(8,0,_T("Statut Etalon 2"));
	GrilleEtats.SetItemText(9,0,_T("Peson OFF"));
	GrilleEtats.SetItemText(10,0,_T("Poids Moyen Minims (cg)"));
	GrilleEtats.SetItemText(11,0,_T("Tare Etui Vide (cg)"));
	GrilleEtats.SetItemText(12,0,_T("Tare Etui Plein (cg)"));

	CFont NewFont;
	NewFont.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_SEMIBOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename

	m_ListeHisto.Initialise(&NewFont, 1000);

	//Construction du Menu Dynamique
	Menu_PesonEtui.bVertical = true;

	Menu_PesonEtui.strProfil = theApp.CurrentUser.strProfil;
	Menu_PesonEtui.SendMessage(WM_CREATE,NULL,NULL);

	this->SetTimer(ID_TIMER_AFFICHAGE,DELAI_TIMER_AFFICHAGE,NULL);

	AfficherHistoMesures();	//V41

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void  CDialPesee::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	if(strFonction == "PESETUI_DESACTIVER")
	{
		theApp.PesonEtui.inhiber(BoutonParent,PESON_INHIBER_NORMAL);
	}
	else if(strFonction == "PESETUI_ACTIVER")
	{
		theApp.PesonEtui.inhiber(BoutonParent,PESON_INHIBER_NORMAL);
	}
	else if(strFonction == "PESETUI_TARE")
	{
		if((theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONARRET)&&
			(theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION))
		{
			theApp.SetInfosString(theApp.TableMessage.GetStr(MESSAGE_HORSOUARRETPRODUCTION_NOK));
			return ;
		}

		theApp.PesonEtui.Do_Tare();
	}
}

void CDialPesee::AfficherParamGrille(void)
{
	CString strTemp = "";
	
	strTemp.Format(_T("%d"),theApp.PesonEtui.iEtat);
	GrilleEtats.SetItemText(1,1,strTemp);

	strTemp.Format(_T("%d"),theApp.PesonEtui.LastMesure);
	GrilleEtats.SetItemText(2,1,strTemp);

	strTemp.Format(_T("%d"),theApp.PesonEtui.StatutLastMesure);
	GrilleEtats.SetItemText(3,1,strTemp);

	strTemp.Format(_T("%d"),theApp.PesonEtui.StabilisedValue);
	GrilleEtats.SetItemText(4,1,strTemp);

	strTemp.Format(_T("%d"),theApp.PesonEtui.PoidsEtalon1);
	GrilleEtats.SetItemText(5,1,strTemp);

	strTemp.Format(_T("%d"),theApp.PesonEtui.StatutEtalon1);
	GrilleEtats.SetItemText(6,1,strTemp); //0->lu par vision; 1->pesage OK; 2-> Pesage NOK (à archiver)

	strTemp.Format(_T("%d"),theApp.PesonEtui.PoidsEtalon2);
	GrilleEtats.SetItemText(7,1,strTemp);
	
	strTemp.Format(_T("%d"),theApp.PesonEtui.StatutEtalon2);
	GrilleEtats.SetItemText(8,1,strTemp);

	strTemp.Format(_T("%d"),theApp.PesonEtui.PesonMode);
	GrilleEtats.SetItemText(9,1,strTemp);

	//Changement couleur Cellule en fonction de l'Etat du peson
	CGridCellBase * cell = NULL;
	cell = GrilleEtats.GetCell(9,1);
	if(theApp.PesonEtui.PesonMode == PESON_MODEOFF)
	{
		//Gris
		cell->SetBackClr(RGB(236,233,216));
	}
	else
	{
		//Vert
		cell->SetBackClr(RGB(0,255,0));
	}
	
	strTemp.Format(_T("%4.2f"),(double)(theApp.PesonEtui.iPoidsMoyenMinims / 100.));
	GrilleEtats.SetItemText(10,1,strTemp);

	strTemp.Format(_T("%4.2f"),(double)(theApp.PesonEtui.iTareEtuiVide / 100.));
	GrilleEtats.SetItemText(11,1,strTemp);

	strTemp.Format(_T("%4.2f"),(double)(theApp.PesonEtui.iTareEtuiPlein / 100.));
	GrilleEtats.SetItemText(12,1,strTemp);
	
	GrilleEtats.Refresh();
}

void CDialPesee::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == ID_TIMER_AFFICHAGE)
	{
		if(IsWindowVisible())
		{
			AfficherParamGrille();

			//MAJ tableau Mesures pour peson Etui
			if(theApp.PesonEtui.indexHisto != theApp.PesonEtui.indexAffichage)
			{
				::SendMessage(theApp.PeseeView->GetSafeHwnd(),WM_PESON_MAJMESURE,theApp.PesonEtui.iNumPeson,0);
				theApp.PesonEtui.indexAffichage = theApp.PesonEtui.indexHisto;
			}
		}
	}

	CAcyDial::OnTimer(nIDEvent);
}

int CDialPesee::AfficherHistoMesures(void)
{
	CString strTemp = "";
	int index = 0;

	m_ListeHisto.ResetContent();

	for(int i = 0; i < PESON_NB_MESURES_HISTO ;i++) //Peson->indexHisto + 1 => Valeur la + ancienne
	{
		index = (i + (theApp.PesonEtui.indexHisto) + PESON_NB_MESURES_HISTO) % PESON_NB_MESURES_HISTO;

		strTemp.Format(_T("Mesure %d = %2.3f"),index,theApp.PesonEtui.TabMesures[index]/100.);	
		m_ListeHisto.InsertString(0, strTemp);
	}

	return 0;
}

LRESULT CDialPesee::OnMAJMesure(WPARAM Param1, LPARAM Param2)
{
	
	AfficherHistoMesures();
	return 0;
}