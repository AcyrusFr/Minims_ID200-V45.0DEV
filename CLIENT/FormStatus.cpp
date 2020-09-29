// FormStatus.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "FormStatus.h"
#include "splitfrm.h"


// CFormStatus

IMPLEMENT_DYNCREATE(CFormStatus, CFormView)

CFormStatus::CFormStatus()
	: CFormView(CFormStatus::IDD)
	, str_status(_T(""))
	//,MenuStatus("MenuCommande")
	, MenuStatus(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("MenuCommande"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	bIsAlarmeVisible = false;
}

CFormStatus::~CFormStatus()
{
}

void CFormStatus::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, str_status);
	DDX_Control(pDX, IDC_BUTALARME, butAlarme);
	DDX_Control(pDX, IDC_STATICMENU_STATUS, MenuStatus);
}

BEGIN_MESSAGE_MAP(CFormStatus, CFormView)
	ON_BN_CLICKED(IDC_BUTALARME, &CFormStatus::OnBnClickedButalarme)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// Diagnostics de CFormStatus

#ifdef _DEBUG
void CFormStatus::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormStatus::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// Gestionnaires de messages de CFormStatus

void CFormStatus::OnBnClickedButalarme()
{
	//flag fenêtre alarme
	if(theApp.AlarmesView->IsWindowVisible())
		bIsAlarmeVisible =true;
	else 
		bIsAlarmeVisible = false;

	//arret clignotement
	butAlarme.bAlarme = false;

	if(!bIsAlarmeVisible)
	{
		//Affichage de la fenetre des alarmes
		((CSplitFrame * )AfxGetMainWnd())->OnViewAlarmes();
	}
	else
	{
		//affichage fenêtre principale
		((CSplitFrame * )AfxGetMainWnd())->OnViewMachine();
	}

}

void CFormStatus::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	theApp.pFormStatus = this;

	//Initialisation du bouton alarme
	butAlarme.Init();
}

void CFormStatus::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == ID_TIMER_ALARME)
	{
		theApp.GererAlarmes();//rafraichissement des alarmes
	}

	CFormView::OnTimer(nIDEvent);
}

void CFormStatus::OnDestroy()
{
	CFormView::OnDestroy();

	//Arret timer rafraichissement des alarmes
	KillTimer(TimerAlarme);
}
