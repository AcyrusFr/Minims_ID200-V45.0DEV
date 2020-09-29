// DialMixup.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialMixup.h"


// Boîte de dialogue CDialMixup

IMPLEMENT_DYNAMIC(CDialMixup, CAcyDial)

CDialMixup::CDialMixup(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialMixup::IDD, pParent)
	//,Menu_Mixup("Menu_Mixup")
	, Menu_Mixup(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Mixup"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{

}

CDialMixup::~CDialMixup()
{
}

void CDialMixup::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICTEXTMIXUP, staticTitre);
	DDX_Control(pDX, IDC_STATICMENU_MIXUP, Menu_Mixup);
	DDX_Control(pDX, IDC_DEFAUTENCOURS, ListCurrentDef);
}


BEGIN_MESSAGE_MAP(CDialMixup, CAcyDial)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialMixup

BOOL CDialMixup::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation 
	staticTitre.CouleurText = RGB(255,0,0);

	CString strTemp = _T("MIXUP");
	staticTitre.strText = strTemp;

	staticTitre.iSizePolice = 30;
	staticTitre.Invalidate();

	Menu_Mixup.strProfil = theApp.CurrentUser.strProfil;
	Menu_Mixup.SendMessage(WM_CREATE,NULL,NULL);

	//ListCurrentDef.SendMessage(ACY_LB_INIT,0,0);

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

	ListCurrentDef.Initialise(&NewFont, 1000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

BOOL CDialMixup::OnCommand(WPARAM wParam, LPARAM lParam)
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