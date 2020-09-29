// DialAffGridRecette.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DialAffGridRecette.h"


// Boîte de dialogue CDialAffGridRecette

IMPLEMENT_DYNAMIC(CDialAffGridRecette, CDialog)

CDialAffGridRecette::CDialAffGridRecette(CWnd* pParent /*=NULL*/,CRecette * c_pRecette /*= NULL*/)
	: CDialog(CDialAffGridRecette::IDD, pParent)
{
	pRecette = c_pRecette;
}

CDialAffGridRecette::~CDialAffGridRecette()
{
}

void CDialAffGridRecette::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, GrilleRecetteAff);
}


BEGIN_MESSAGE_MAP(CDialAffGridRecette, CDialog)

END_MESSAGE_MAP()


// Gestionnaires de messages de CDialAffGridRecette

BOOL CDialAffGridRecette::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Initialisation de la grille
	GrilleRecetteAff.SetDefCellHeight(25);//hauteur des cellules
	GrilleRecetteAff.SetColumnCount(2);
	GrilleRecetteAff.SetFixedRowCount(1);//légende fixe
	GrilleRecetteAff.SetRowCount(26+1);	//#2 Emballage : de 23 à 26
	GrilleRecetteAff.SetColumnWidth(0,150);
	GrilleRecetteAff.ExpandLastColumn();
	GrilleRecetteAff.SetColumnResize(FALSE);
	GrilleRecetteAff.SetListMode(TRUE);
	GrilleRecetteAff.SetSingleRowSelection(TRUE);
	GrilleRecetteAff.SetSingleColSelection(TRUE);
	GrilleRecetteAff.SetFixedColumnSelection(FALSE);
	GrilleRecetteAff.SetFixedRowSelection(FALSE);
	GrilleRecetteAff.SetFixedBkColor(RGB(0,46,184));
	GrilleRecetteAff.SetFixedTextColor(RGB(255,255,255));
	GrilleRecetteAff.SetEditable(FALSE);

	// Changement Police des 4 premiéres lignes
	const LOGFONT * cellLogFont = NULL;
	CFont NewFont;

	NewFont.CreateFont(
	   14,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_BOLD,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   DEFAULT_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   "Arial");                 // lpszFacename

	NewFont.GetLogFont(&myLogFont);

	for(int i = 1; i < 5; i++)
	{
		GrilleRecetteAff.SetItemFont(i,0,&myLogFont);
		GrilleRecetteAff.SetItemFont(i,1,&myLogFont);
		
		GrilleRecetteAff.SetItemFgColour(i,0,RGB(0,0,255));
		GrilleRecetteAff.SetItemFgColour(i,1,RGB(0,0,255));
	}

	//légende
	CGridCellBase* cellAff = NULL;
	
	cellAff = GrilleRecetteAff.GetCell(0,0);
	cellAff->SetText(_T("Libelle"));
	cellAff = GrilleRecetteAff.GetCell(0,1);
	cellAff->SetText(_T("Valeur"));

	//Colonne Libelle
	GrilleRecetteAff.SetItemText(1,0,_T("Description"));
	GrilleRecetteAff.SetItemText(2,0,_T("OF"));
	GrilleRecetteAff.SetItemText(3,0,_T("Article"));
	GrilleRecetteAff.SetItemText(4,0,_T("Lot"));
	GrilleRecetteAff.SetItemText(5,0,_T("Type de produit"));
	GrilleRecetteAff.SetItemText(6,0,_T("Description ETUI"));
	GrilleRecetteAff.SetItemText(7,0,_T("Code ETUI"));
	GrilleRecetteAff.SetItemText(8,0,_T("Format Etiquette"));
	GrilleRecetteAff.SetItemText(9,0,_T("Description Notice 1"));
	GrilleRecetteAff.SetItemText(10,0,_T("Code Notice 1"));
	GrilleRecetteAff.SetItemText(11,0,_T("Format Notice 1"));
	GrilleRecetteAff.SetItemText(12,0,_T("Description Notice 2"));
	GrilleRecetteAff.SetItemText(13,0,_T("Code Notice 2"));
	GrilleRecetteAff.SetItemText(14,0,_T("Format Notice 2"));
	GrilleRecetteAff.SetItemText(15,0,_T("Quantité"));
	GrilleRecetteAff.SetItemText(16,0,_T("Peremption (Mois)"));
	GrilleRecetteAff.SetItemText(17,0,_T("Source Chargement"));
	GrilleRecetteAff.SetItemText(18,0,_T("Reprise"));
	GrilleRecetteAff.SetItemText(19,0,_T("Statut"));
	GrilleRecetteAff.SetItemText(20,0,_T("Couleur"));
	//0.99
	GrilleRecetteAff.SetItemText(21,0,_T("DMX Vision"));
	GrilleRecetteAff.SetItemText(22,0,_T("Minims"));
	GrilleRecetteAff.SetItemText(23,0,_T("Tol. poids Etui"));
	//#2 Emballage
	GrilleRecetteAff.SetItemText(24,0,_T("Code Emballage"));
	GrilleRecetteAff.SetItemText(25,0,_T("Description Emballage"));
	GrilleRecetteAff.SetItemText(26,0,_T("CB Emballage"));

	InitialiserGrilleRecette();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialAffGridRecette::InitialiserGrilleRecette(void)
{
	if(pRecette == NULL) return;

	//Initialisation de la grille pour affichage des paramètres/Etats
	//Colonne  Valeur
	CString strTemp = "";
	GrilleRecetteAff.SetItemText(1,1,pRecette->Article.Description);

	strTemp = pRecette->OfNum;
	GrilleRecetteAff.SetItemText(2,1,strTemp);

	GrilleRecetteAff.SetItemText(3,1,pRecette->Article.Code);
	GrilleRecetteAff.SetItemText(4,1,pRecette->strLot);


	if(pRecette->Article.Type == RECETTE_TYPEBLISTER)
	{
		strTemp = _T("BLISTER");
	}
	else
	{
		strTemp = _T("SACHET");
	}
	GrilleRecetteAff.SetItemText(5,1,strTemp);
	GrilleRecetteAff.SetItemText(6,1,pRecette->ArtEtui.Description);
	strTemp.Format(_T("%s"),pRecette->ArtEtui.Code);
	GrilleRecetteAff.SetItemText(7,1,strTemp);
	strTemp.Format(_T("%d"),pRecette->ArtEtiquette.iParam1);
	GrilleRecetteAff.SetItemText(8,1,strTemp);
	GrilleRecetteAff.SetItemText(9,1,pRecette->ArtNotice1.Description);
	strTemp.Format(_T("%s"),pRecette->ArtNotice1.Code);
	GrilleRecetteAff.SetItemText(10,1,strTemp);
	strTemp.Format(_T("%d"),pRecette->ArtNotice1.iParam1);
	GrilleRecetteAff.SetItemText(11,1,strTemp);
	GrilleRecetteAff.SetItemText(12,1,pRecette->ArtNotice2.Description);
	strTemp.Format(_T("%s"),pRecette->ArtNotice2.Code);
	GrilleRecetteAff.SetItemText(13,1,strTemp);
	strTemp.Format(_T("%d"),pRecette->ArtNotice2.iParam1);
	GrilleRecetteAff.SetItemText(14,1,strTemp);
	strTemp.Format(_T("%d"),pRecette->Quantite);
	GrilleRecetteAff.SetItemText(15,1,strTemp);
	strTemp.Format(_T("%d"),pRecette->PeremptionMois);
	GrilleRecetteAff.SetItemText(16,1,strTemp);
	strTemp.Format(_T("%d"),pRecette->SourceData);
	GrilleRecetteAff.SetItemText(17,1,strTemp);

	if(pRecette->Reprise)
		strTemp = _T("TRUE");
	else
		strTemp = _T("FALSE");

	GrilleRecetteAff.SetItemText(18,1,strTemp);

	strTemp.Format(_T("%d"),pRecette->iStatut);
	GrilleRecetteAff.SetItemText(19,1,strTemp);

	strTemp.Format(_T("%d"),pRecette->ArtMinims.iParam2);
	GrilleRecetteAff.SetItemText(20,1,strTemp);

	strTemp.Format(_T("%s"),pRecette->CodeDMXLot);
	GrilleRecetteAff.SetItemText(21,1,strTemp);

	GrilleRecetteAff.SetItemText(22,1,pRecette->ArtMinims.Code);
	
	strTemp.Format(_T("%4.2f"),(double)(pRecette->TolEtuiPlein_cg / 100.));
	GrilleRecetteAff.SetItemText(23,1,strTemp);

	//#2 Emballage
	GrilleRecetteAff.SetItemText(24,1,pRecette->ArtEmballage.Code);
	GrilleRecetteAff.SetItemText(25,1,pRecette->ArtEmballage.Description);
	GrilleRecetteAff.SetItemText(26,1,pRecette->ArtEmballage.CodeDMX);

	GrilleRecetteAff.Refresh();

}



