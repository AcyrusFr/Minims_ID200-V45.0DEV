// DialInitialisation.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialInitialisation.h"


// Boîte de dialogue CDialInitialisation

IMPLEMENT_DYNAMIC(CDialInitialisation, CAcyDial)

CDialInitialisation::CDialInitialisation(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialInitialisation::IDD, pParent)
	//,MenuInit(_T("Menu_Init"))
	, MenuInit(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Init"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	//TimerDmx = NULL;
	TimerTimeout = NULL;
	//Dmx = NULL;
	bTimeOut = false;
	bTimeOutHappened = false;
}

CDialInitialisation::~CDialInitialisation()
{
}

void CDialInitialisation::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, ListeInit);
	DDX_Control(pDX, IDC_STATICMENU_DIALINIT, MenuInit);
	DDX_Control(pDX, IDC_STATIC_TITRE, static_init);
}


BEGIN_MESSAGE_MAP(CDialInitialisation, CAcyDial)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialInitialisation
int CDialInitialisation::SetStringListe(CString strToAdd)
{
	//ListeInit.SendNotifyMessage(ACY_LB_INSERTSTRING,0,(LPARAM)(LPTSTR)strToAdd.GetBuffer());
	ListeInit.InsertString(0,strToAdd);
//	strToAdd.ReleaseBuffer();
	UpdateWindow();
	return 0;
}

int CDialInitialisation::ResetListe(void)
{
	//ListeInit.SendNotifyMessage(ACY_LB_RESETCONTENT,0,0);
	ListeInit.ResetContent();
	return 0;
}

BOOL CDialInitialisation::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	CString strTemp = _T("");
	static_init.GetWindowText(strTemp);
	static_init.strText = strTemp;
	static_init.iSizePolice = 15;
	static_init.bSouligne = true;

	// Initialisation du menu
	MenuInit.bVertical = true;
	MenuInit.strProfil = theApp.CurrentUser.strProfil;
	MenuInit.SendMessage(WM_CREATE,NULL,NULL);	
	
	//V 44.7 ListeInit.SendMessage(ACY_LB_INIT,0,0);
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

	ListeInit.Initialise(&NewFont, 1000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialInitialisation::OnTimer(UINT_PTR nIDEvent)
{

	if(nIDEvent == ID_TIMER_TIMEOUT)
	{
		bTimeOut = true;
		//if(Dmx != NULL)	Dmx->Alarme[DMXALM_TIMEOUT]->NewValue = ALARME_ON;

		if(bTimeOutHappened == false) bTimeOutHappened = true;

		StopTimerTimeout();
	}

	CAcyDial::OnTimer(nIDEvent);
}

void CDialInitialisation::StartTimerTimeout(void)
{
	bTimeOut = false;
	TimerTimeout = this->SetTimer(ID_TIMER_TIMEOUT,5000,NULL);
}

void CDialInitialisation::StopTimerTimeout(void)
{
	KillTimer(TimerTimeout);
}