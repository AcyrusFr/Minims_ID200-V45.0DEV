// DialValidation.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialValidation.h"


// Boîte de dialogue CDialValidation

IMPLEMENT_DYNAMIC(CDialValidation, CAcyDial)

CDialValidation::CDialValidation(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialValidation::IDD, pParent)
	//,Menu_Of(_T("Menu_Of"))
	, Menu_Of(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Of"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{

}

CDialValidation::~CDialValidation()
{
}

void CDialValidation::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICMENU_VALIDOF, Menu_Of);
}


BEGIN_MESSAGE_MAP(CDialValidation, CAcyDial)
	ON_BN_CLICKED(IDC_BUTTON1, &CDialValidation::OnBnClickedButton1)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialValidation

void CDialValidation::OnBnClickedButton1()
{
	
}

BOOL CDialValidation::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initilisation Menu
	Menu_Of.bVertical = true;

	Menu_Of.strProfil = theApp.CurrentUser.strProfil;
	Menu_Of.SendMessage(WM_CREATE,NULL,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
BOOL CDialValidation::OnCommand(WPARAM wParam, LPARAM lParam)
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