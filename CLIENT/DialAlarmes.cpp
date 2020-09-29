// DialAlarmes.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialAlarmes.h"


// Boîte de dialogue CDialAlarmes

IMPLEMENT_DYNAMIC(CDialAlarmes, CAcyDial)

CDialAlarmes::CDialAlarmes(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialAlarmes::IDD, pParent)
	//,Menu_Alarmes("Menu_Alarmes")
	, Menu_Alarmes(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Alarmes"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
}

CDialAlarmes::~CDialAlarmes()
{
}

void CDialAlarmes::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, grilleAlarmes);
	DDX_Control(pDX, IDC_EVENTSLIST, m_EventsList);
	DDX_Control(pDX, IDC_DEFAUTENCOURS, ListeDefEnCours);
	DDX_Control(pDX, IDC_STATICMENU_ALARMES, Menu_Alarmes);
	DDX_Control(pDX, IDC_LIST1, Liste_Alertes);
}


BEGIN_MESSAGE_MAP(CDialAlarmes, CAcyDial)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialAlarmes

BOOL CDialAlarmes::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	GetWindowRect(m_rect);
	m_nScrollPos = 0;

	int NbGroupes = NBGROUPES;

	// Initialisation de la grille pour affichage des Alarmes
	grilleAlarmes.SetDefCellHeight(25);//hauteur des cellules
	grilleAlarmes.SetDefCellWidth(330);//largeur des cellules
	grilleAlarmes.SetColumnCount(2);
	grilleAlarmes.SetRowCount(NbGroupes+1);//+1 pour légende
	grilleAlarmes.SetFixedRowCount(1);//légende fixe
	grilleAlarmes.SetFixedColumnCount(1);//Premiére colonne fixe
	grilleAlarmes.ExpandColumnsToFit(FALSE);
	grilleAlarmes.SetListMode(TRUE);
	grilleAlarmes.SetSingleRowSelection(TRUE);
	grilleAlarmes.SetSingleColSelection(TRUE);
	grilleAlarmes.SetFixedColumnSelection(FALSE);
	grilleAlarmes.SetFixedRowSelection(FALSE);

	grilleAlarmes.SetFixedBkColor(RGB(0,46,184));
	grilleAlarmes.SetFixedTextColor(RGB(255,255,255));

	//légende
	grilleAlarmes.SetItemText(0,0,_T("Groupe Alarme"));
	grilleAlarmes.SetItemText(0,1,_T("Nb Defauts"));

	//remplissage des intitulés
	grilleAlarmes.Refresh();

	Menu_Alarmes.bVertical = true;

	Menu_Alarmes.strProfil = theApp.CurrentUser.strProfil;
	Menu_Alarmes.SendMessage(WM_CREATE,NULL,NULL);


	//Changement de la police de caractère
	CFont NewFont;	
	NewFont.CreateFont(
				   20,                        // nHeight
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
				   "Arial");                 // lpszFacename
	
	/*ListeDefEnCours.SetCustomFont(&NewFont);
	Liste_Alertes.SetCustomFont(&NewFont);

	ListeDefEnCours.SetCustomColor(RGB(255,0,0));
	Liste_Alertes.SetCustomColor(RGB(0,0,255));*/


	ListeDefEnCours.Initialise(&NewFont,1000);
	Liste_Alertes.Initialise(&NewFont, 1000);
	m_EventsList.Initialise(&NewFont, 1000);

	ListeDefEnCours.SetCustomColor(RGB(255, 0, 0));
	Liste_Alertes.SetCustomColor(RGB(0, 0, 255));


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

int CDialAlarmes::UpdateAlarme(void)
{

	CString strTemp;
	//rafraichissement Affichage des alarmes dans la grille
	for(int i = 0; i < NBGROUPES;i++)
	{
		//remplissage des intitulés
		grilleAlarmes.SetItemText(i + 1,0,theApp.m_GestAlarmes.GrpAlarmes[i].strLibelle);

		if(theApp.m_GestAlarmes.GrpAlarmes[i].NbDefauts != 0)
		{
			CGridCellBase* cell = grilleAlarmes.GetCell(i + 1,1);
			cell->SetBackClr(RGB(255,0,0));
		}
		else
		{
			CGridCellBase* cell = grilleAlarmes.GetCell(i + 1,1);
			cell->SetBackClr(RGB(255,255,255));
		}

		//V 40 Affichage AlarmeVal
		//V 38 Affichage AlarmeVal pour groupe OPC
		if (i == GROUPE_OPC)
		{
			strTemp.Format(_T("%d [%d]"), theApp.m_GestAlarmes.GrpAlarmes[i].NbDefauts, theApp.m_GestAlarmes.Alarme[iIndex_ALARME_OPC].NewValue);
		}
		else
		{
			strTemp.Format("%d", theApp.m_GestAlarmes.GrpAlarmes[i].NbDefauts);
		}
		
		grilleAlarmes.SetItemText(i + 1,1,strTemp);
	}

	grilleAlarmes.Refresh();

	return 0;
}

void CDialAlarmes::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{


	CAcyDial::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDialAlarmes::OnSize(UINT nType, int cx, int cy)
{
	CAcyDial::OnSize(nType, cx, cy);

}
void CDialAlarmes::AfficherChaineListe(CString sChaine)
{
	if(::IsWindow(m_EventsList.GetSafeHwnd()))//0.99
	{
		m_EventsList.InsertString(0,sChaine);
		//m_EventsList.SendNotifyMessage(ACY_LB_INSERTSTRING,0,(LPARAM)sChaine);
	}
}
void CDialAlarmes::AfficherDefautEnCours(CString sChaine)
{
	if(::IsWindow(ListeDefEnCours.GetSafeHwnd()))//0.99
	{
		ListeDefEnCours.InsertString(0,sChaine);
		//ListeDefEnCours.SendNotifyMessage(ACY_LB_INSERTSTRING,0,(LPARAM)sChaine);
	}
}
void CDialAlarmes::ResetDefautEnCours()
{
	if(::IsWindow(ListeDefEnCours.GetSafeHwnd()))//0.99
	{
		ListeDefEnCours.ResetContent();
		Liste_Alertes.ResetContent();
	//	ListeDefEnCours.SendNotifyMessage(ACY_LB_RESETCONTENT,0,0);
	}
}


BOOL CDialAlarmes::OnCommand(WPARAM wParam, LPARAM lParam)
{
	

	return CAcyDial::OnCommand(wParam, lParam);
}