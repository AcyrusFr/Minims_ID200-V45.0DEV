// DialEtiquette.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialEtiquette.h"


// Boîte de dialogue CDialEtiquette

IMPLEMENT_DYNAMIC(CDialEtiquette, CAcyDial)

CDialEtiquette::CDialEtiquette(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialEtiquette::IDD, pParent)
	,Menu_Etiquette("Menu_Etiquette")
{

}

CDialEtiquette::~CDialEtiquette()
{
}

void CDialEtiquette::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MENU_ETIQUETTE, Menu_Etiquette);
	DDX_Control(pDX, IDC_PICTURE, static_image);
}


BEGIN_MESSAGE_MAP(CDialEtiquette, CAcyDial)

END_MESSAGE_MAP()


// Gestionnaires de messages de CDialEtiquette


BOOL CDialEtiquette::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation du menu
	Menu_Etiquette.bVertical = true;
	Menu_Etiquette.strProfil = theApp.CurrentUser.strProfil;
	Menu_Etiquette.SendMessage(WM_CREATE,NULL,NULL);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}


BOOL CDialEtiquette::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

	return CAcyDial::OnCommand(wParam, lParam);
}
