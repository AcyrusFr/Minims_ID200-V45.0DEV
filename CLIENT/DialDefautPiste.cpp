// DialDefautPiste.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialDefautPiste.h"


// Boîte de dialogue CDialDefautPiste

IMPLEMENT_DYNAMIC(CDialDefautPiste, CAcyDial)

CDialDefautPiste::CDialDefautPiste(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialDefautPiste::IDD, pParent)
//,Menu_Defaut("Menu_Defaut")
, Menu_Defaut(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Defaut"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	iScrollH = 0;
	iScrollV = 0;
	iScrollHMax = 0;
	iScrollVMax = 0;

	Hauteur = 0;
	Largeur = 0;

	dZoomFactorH = 1;
	dZoomFactorV = 1;

	WindowsView = NULL;
	imgParent = NULL;
}

CDialDefautPiste::~CDialDefautPiste()
{
}

void CDialDefautPiste::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICMENUDEFAUT, Menu_Defaut);
	DDX_Control(pDX, IDC_STATIC_IMGDEF, Image);
	DDX_Control(pDX, IDC_SCROLLBAR2, SB_Vert);
	DDX_Control(pDX, IDC_SCROLLBAR1, SB_Horz);
	DDX_Control(pDX, IDC_TEXT_PISTE0, Text_Piste[0]);
	DDX_Control(pDX, IDC_TEXT_PISTE1, Text_Piste[1]);
	DDX_Control(pDX, IDC_TEXT_PISTE2, Text_Piste[2]);

	//V 44.8 petite resolution
	DDX_Control(pDX, IDC_STATIC_CAD_P1_VUS2, Textcadence_Vues[0]);
	DDX_Control(pDX, IDC_STATIC_CAD_P2_VUS2, Textcadence_Vues[1]);
	DDX_Control(pDX, IDC_STATIC_CAD_P3_VUS2, Textcadence_Vues[2]);
	DDX_Control(pDX, IDC_STATIC_CAD_P1_PRIS2, Textcadence_Pris[0]);
	DDX_Control(pDX, IDC_STATIC_CAD_P2_PRIS2, Textcadence_Pris[1]);
	DDX_Control(pDX, IDC_STATIC_CAD_P3_PRIS2, Textcadence_Pris[2]);
	DDX_Control(pDX, IDC_STATIC_CAD_P1_RATIO2, Textcadence_Ratio[0]);
	DDX_Control(pDX, IDC_STATIC_CAD_P2_RATIO2, Textcadence_Ratio[1]);
	DDX_Control(pDX, IDC_STATIC_CAD_P3_RATIO2, Textcadence_Ratio[2]);

	DDX_Control(pDX, IDC_STATIC_CAD_P1_VUS3, Staticcadence_Vues);
	DDX_Control(pDX, IDC_STATIC_CAD_P1_PRIS3, Staticcadence_Pris);
	DDX_Control(pDX, IDC_STATIC_CAD_P1_RATIO3, Staticcadence_Ratio);
	
}


BEGIN_MESSAGE_MAP(CDialDefautPiste, CAcyDial)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialDefautPiste


BOOL CDialDefautPiste::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	for (int i = 0; i < 3; i++)
	{
		Text_Piste[i].strText.Format("Piste %d",i+1);
		Text_Piste[i].bSautDeLigne = true;
	//	Text_Piste[i].bCenter = true;
		Text_Piste[i].bCentreTexte = true;
		Text_Piste[i].CouleurText = RGB(255,0,0);
		Text_Piste[i].iSizePolice = 20;
		Text_Piste[i].Invalidate();
	}

	GetDlgItem(IDC_STATIC_CAD_P1_RATIO2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CAD_P2_RATIO2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CAD_P3_RATIO2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CAD_P1_RATIO3)->ShowWindow(SW_HIDE);
	//V 44.8 petite resolution
	switch (pParamModeAffichageResolution->value)
	{
	case 0:// petite resolution : affichage sur dialogue vision
	default:

		Staticcadence_Vues.strText = _T("Tapis (/min)");
		Staticcadence_Pris.strText = _T("Robot (/min)");
		Staticcadence_Ratio.strText = _T("Ratio (%)");

		Staticcadence_Vues.iSizePolice = 15;
		Staticcadence_Pris.iSizePolice = 15;
		Staticcadence_Ratio.iSizePolice = 15;

		Staticcadence_Vues.bSouligne = false;
		Staticcadence_Pris.bSouligne = false;
		Staticcadence_Ratio.bSouligne = false;

		for (int i = 0; i < 3; i++)
		{
			Textcadence_Vues[i].strText = _T("0");
			Textcadence_Pris[i].strText = _T("0");
			Textcadence_Ratio[i].strText = _T("0");
		
			Textcadence_Vues[i].iSizePolice = 20;
			Textcadence_Pris[i].iSizePolice = 20;
			Textcadence_Ratio[i].iSizePolice = 20;
	
			Textcadence_Vues[i].bSouligne = false;
			Textcadence_Pris[i].bSouligne = false;

			Textcadence_Vues[i].CouleurText = RGB(0, 0, 255);
			Textcadence_Pris[i].CouleurText = RGB(0, 0, 255);
			Textcadence_Ratio[i].CouleurText = RGB(0, 0, 255);
		}

		//refresh
		for (int i = 0; i < 3; i++)
		{
			Textcadence_Vues[i].Invalidate();
			Textcadence_Pris[i].Invalidate();
			Textcadence_Ratio[i].Invalidate();
		}

		Staticcadence_Vues.Invalidate();
		Staticcadence_Pris.Invalidate();
		Staticcadence_Ratio.Invalidate();

		//affichage des champs de la dialogue defaut piste
		GetDlgItem(IDC_STATIC_CAD_P1_VUS2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAD_P2_VUS2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAD_P3_VUS2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAD_P1_PRIS2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAD_P2_PRIS2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAD_P3_PRIS2)->ShowWindow(SW_SHOW);
		/*GetDlgItem(IDC_STATIC_CAD_P1_RATIO2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAD_P2_RATIO2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_CAD_P3_RATIO2)->ShowWindow(SW_SHOW);*/
		break;

	case 1:// Grande resolution : affichage sur dialogue Machine
		GetDlgItem(IDC_STATIC_CAD_P1_VUS2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P2_VUS2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P3_VUS2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P1_PRIS2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P2_PRIS2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P3_PRIS2)->ShowWindow(SW_HIDE);
		/*GetDlgItem(IDC_STATIC_CAD_P1_RATIO2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P2_RATIO2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P3_RATIO2)->ShowWindow(SW_HIDE);*/

		GetDlgItem(IDC_STATIC_CAD_P1_VUS3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CAD_P1_PRIS3)->ShowWindow(SW_HIDE);
		/*GetDlgItem(IDC_STATIC_CAD_P1_RATIO3)->ShowWindow(SW_HIDE);*/
		break;
	}

	//Construction du Menu Dynamique
	Menu_Defaut.bVertical = false;
	Menu_Defaut.strProfil = theApp.CurrentUser.strProfil;
	Menu_Defaut.SendMessage(WM_CREATE,NULL,NULL);	

	//Récupération taille affichage image
	WindowsView = NULL;
	WindowsView = Image.GetSafeHwnd();
	Image.GetWindowRect(&WinRect);

	ScreenToClient(&WinRect);
	
	Hauteur = WinRect.Height();
	Largeur = WinRect.Width();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}


BOOL CDialDefautPiste::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	return CAcyDial::OnCommand(wParam, lParam);
}

void CDialDefautPiste::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	//Recuperation pointeur sur Menu Parent
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;
	if(strFonction == "DEFAUT_1")
	{
		int i = 0;
	}
	else if(strFonction == "DEFAUT_2")
	{
		int i = 0;
	}
}

void CDialDefautPiste::MAJ_Scrollbars(void)
{
	Calcul_ValScroll();

	//MAJ Des Scollbars en fonction du zoom et de la position

	SB_Horz.SetScrollRange(0,iScrollHMax,FALSE);
	SB_Vert.SetScrollRange(0,iScrollVMax,FALSE);

	SB_Horz.SetScrollPos(iScrollH,TRUE);
	SB_Vert.SetScrollPos(iScrollV,TRUE);

	if(imgParent == NULL) return;

	if((!bState_Haut)||(!bState_Bas))
	{
		SB_Vert.EnableWindow(FALSE);
	}
	else
	{
		double dRapport = Hauteur / (imgParent->Roi.dY * dZoomFactorV);

		SCROLLINFO InfoVrt;
		SB_Vert.GetScrollInfo(&InfoVrt,SIF_ALL);
		InfoVrt.nPage = (UINT)( (InfoVrt.nMax -InfoVrt.nMin) * dRapport);
		InfoVrt.nMax = InfoVrt.nMax + InfoVrt.nPage;
		SB_Vert.SetScrollInfo(&InfoVrt,TRUE);

		SB_Vert.EnableWindow(TRUE);
	}

	if((!bState_Droite)||(!bState_Gauche))
	{
		SB_Horz.EnableWindow(FALSE);
	}
	else
	{
		double dRapport = Largeur / (imgParent->Roi.dX * dZoomFactorH);

		SCROLLINFO InfoHrz;
		SB_Horz.GetScrollInfo(&InfoHrz,SIF_ALL);
		InfoHrz.nPage = (UINT)( (InfoHrz.nMax -InfoHrz.nMin) * dRapport);
		InfoHrz.nMax = InfoHrz.nMax + InfoHrz.nPage;
		SB_Horz.SetScrollInfo(&InfoHrz,TRUE);

		SB_Horz.EnableWindow(TRUE);
	}
}

void CDialDefautPiste::Calcul_ValScroll(void)
{
	//Calcul des valeurs de scrolling 
	WindowsView = Image.GetSafeHwnd();
	Image.GetWindowRect(&WinRect);

	ScreenToClient(&WinRect);
	
	Hauteur = WinRect.Height();
	Largeur = WinRect.Width();

	bState_Gauche = false;
	bState_Droite = false;

	if(imgParent == NULL) return;

	if(imgParent->Roi.dX * dZoomFactorH <= Largeur)
	{
	//	Largeur = imgParent->Roi.dX;
		iScrollHMax = 0;
	}
	else
	{
		iScrollHMax = (int)( imgParent->Roi.dX * dZoomFactorH - Largeur);
		bState_Gauche = true;
		bState_Droite = true;
		bState_Haut = false;
		bState_Bas = false;
	}

	if(imgParent->Roi.dY * dZoomFactorV <= Hauteur)
	{
		//Hauteur = imgParent->Roi.dY;
		iScrollVMax = 0;
	}
	else
	{
		iScrollVMax = (int)(imgParent->Roi.dY * dZoomFactorV - Hauteur);
		bState_Haut = true;
		bState_Bas = true;
	}

}

void CDialDefautPiste::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//Scrolling fenêtre si c'est la scrollbar Windows
	if(pScrollBar == NULL)
	{
		CAcyDial::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	//Scrollbar verticale
	if((nSBCode == SB_THUMBPOSITION)||(nSBCode == SB_THUMBTRACK))
	{
		if(nPos != iScrollV)
		{
			iScrollV = nPos;
			MdispPan(imgParent->milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;
			SB_Vert.SetScrollPos(iScrollV,TRUE);
		}
	}
	else
	{
		CAcyDial::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}


void CDialDefautPiste::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//Scrolling fenêtre si c'est la scrollbar Windows
	if(pScrollBar == NULL)
	{
		CAcyDial::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	//Scrollbar Horizontale
	if((nSBCode == SB_THUMBPOSITION)||(nSBCode == SB_THUMBTRACK))
	{
		if(nPos != iScrollH)
		{
			iScrollH = nPos;
			MdispPan(imgParent->milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;
			SB_Horz.SetScrollPos(iScrollH,TRUE);
		}
	}
	else
	{
		CAcyDial::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}

void CDialDefautPiste::UpdateScrollBarsPositions()
{
	if(SB_Vert.IsWindowEnabled()) SB_Vert.SetScrollPos(iScrollV,TRUE);
	if(SB_Horz.IsWindowEnabled()) SB_Horz.SetScrollPos(iScrollH,TRUE);
}

