// FormMenuMachine.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "FormMenuMachine.h"


// CFormMenuMachine

IMPLEMENT_DYNCREATE(CFormMenuMachine, CFormView)

CFormMenuMachine::CFormMenuMachine()
	: CFormView(CFormMenuMachine::IDD)
	//,MenuMachine("Menu_Machine")//Nom du menu machine 
	, MenuMachine(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Machine"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
	, strEtatMachine(_T("HORS PRODUCTION"))
{

}

CFormMenuMachine::~CFormMenuMachine()
{
}

void CFormMenuMachine::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MENUMACHINE, MenuMachine);
	DDX_Text(pDX, IDC_TEXT_ETATMACHINE, strEtatMachine);
	DDX_Control(pDX, IDC_TEXT_ETATMACHINE, StaticInfos);
	DDX_Control(pDX, IDC_TEXT_UTILISATEUR, staticUtilisateur);
}

BEGIN_MESSAGE_MAP(CFormMenuMachine, CFormView)
END_MESSAGE_MAP()


// Diagnostics de CFormMenuMachine

#ifdef _DEBUG
void CFormMenuMachine::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormMenuMachine::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// Gestionnaires de messages de CFormMenuMachine

void CFormMenuMachine::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	
	theApp.pFormMenu = this;

	staticUtilisateur.strText = theApp.CurrentUser.strLibelle;
	staticUtilisateur.iSizePolice = 20;

	StaticInfos.iSizePolice = 15;
	StaticInfos.bSautDeLigne = true;
}
