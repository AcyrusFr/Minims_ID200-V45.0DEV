// splitfrm.cpp : implementation file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "BaseFenetre.h"
#include "splitfrm.h"

#include "FormBase.h"
#include "FormStatus.h"
#include "FormMenuMachine.h"



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitFrame

IMPLEMENT_DYNCREATE(CSplitFrame, CFrameWnd)

CSplitFrame::CSplitFrame()
{
	pCurrentView = NULL;
	pPrevView = NULL;
}

CSplitFrame::~CSplitFrame()
{
}

BOOL CSplitFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	CRect ThisRect;

	this->GetWindowRect(&ThisRect);
	

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_OUTILSVUES))
	//{
	//	TRACE0("Impossible de créer toolbar\n");
	//	return -1;      // échec de la création
	//}

	////supprimez ces trois lignes si vous ne souhaitez pas que la barre d'outils soit ancrable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);


	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_OUTILSVUES))
	//	{
	//		TRACE0("Failed to create toolbar\n");
	//		return -1;      // fail to create
	//	}

	//m_wndToolBar.InitSizes(CSize(73,30));

	//m_wndToolBar.SetWindowText(_T("MainTools"));
	//m_wndToolBar.UpdateSizes();
	//m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	//CRect rectToolBar;
	//m_wndToolBar.GetWindowRect(&rectToolBar);
	
	//OffsetToolbar = 0;//rectToolBar.Height();

	this->EnableScrollBar(SB_BOTH,ESB_ENABLE_BOTH);  

	 m_wndSplitter.SetTracking(false);// pas de tracking.
	// add the first splitter pane - which is a nested splitter with 2 rows
	if (!m_wndSplitter.CreateStatic(this,3,1,SWP_NOMOVE|WS_CHILD | WS_VISIBLE))
	{
		TRACE("Failed to create nested splitter\n");
		return FALSE;
	}
	else
	{
		CRect cr;
		GetWindowRect(&cr);
		int Sizey = 0;

		Sizey = cr.Height() - SIZE_STATUSVIEW - SIZE_MENUVIEW - 30;
		if(Sizey < 0) Sizey = 0;
		m_wndSplitter.SetRowInfo( 0,SIZE_MENUVIEW, SIZE_MENUVIEW);
		m_wndSplitter.SetRowInfo( 1,Sizey, 0);
		m_wndSplitter.SetRowInfo( 2,SIZE_STATUSVIEW, SIZE_STATUSVIEW);
	}

	// add the first splitter pane - acocmd in column 0
	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CFormMenuMachine), CSize(ThisRect.Width(), SIZE_MENUVIEW), pContext))
	{
		TRACE("Failed to create first pane\n");
		return FALSE;
	}
	if (!m_wndSplitter.CreateView(1, 0,
		RUNTIME_CLASS(CFormBase), CSize(ThisRect.Width(), ThisRect.Height() - SIZE_STATUSVIEW - SIZE_MENUVIEW), pContext))
	{
		TRACE("Failed to create first pane\n");
		return FALSE;
	}
	if (!m_wndSplitter.CreateView(2, 0,
		RUNTIME_CLASS(CFormStatus), CSize(ThisRect.Width(), SIZE_STATUSVIEW), pContext))
	{
		TRACE("Failed to create first pane\n");
		return FALSE;
	} 
	else
	{
		m_wndSplitter.RecalcLayout();
	}

//	MDIMaximize();
	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSplitFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CSplitFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SYSCOMMAND()
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitFrame message handlers

BOOL CSplitFrame::OnEraseBkgnd(CDC* pDC)
{
	return CFrameWnd::OnEraseBkgnd(pDC);
}



void CSplitFrame::OnDestroy()
{
	
}




void CSplitFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	CRect cr;
    GetWindowRect(&cr);
	int Sizey = 0;
	//ScreenToClient(&cr);

	Sizey = cr.Height() - SIZE_STATUSVIEW - SIZE_MENUVIEW - 100;
	if(Sizey < 0) Sizey = 0;

    if (  m_wndSplitter.IsWindowVisible() )
    {
		m_wndSplitter.GetPane(1,0)->GetWindowRect(&rect);
	
		if(::IsWindow(pCurrentView->GetSafeHwnd()))
		{
			if(!pCurrentView->IsDetached)
			{
				pCurrentView->ResetScrolling();
				pCurrentView->SetWindowPos(NULL,rect.left,rect.top ,rect.Width(),rect.Height() ,SWP_NOMOVE  |SWP_NOACTIVATE);
				pCurrentView->ShowWindow(SW_SHOW);
				AfxGetMainWnd()->SetForegroundWindow();
			}

			if((pPrevView != NULL)&&(!pPrevView->IsDetached))
			{
				pPrevView->ResetScrolling();
				pPrevView->SetWindowPos(NULL,rect.left,rect.top ,rect.Width(),rect.Height() ,SWP_SHOWWINDOW |SWP_FRAMECHANGED);
			}
   
		}

		m_wndSplitter.SetRowInfo( 1,Sizey, 0);
        m_wndSplitter.RecalcLayout();
    }

}

BOOL CSplitFrame::ReplaceView(int row, int col,CAcyDial * pView)
{
	

	m_wndSplitter.GetPane(row,col)->GetWindowRect(&rect);

	if(!pView->IsDetached)
	{
		pPrevView = NULL;

		HideAll();
		//pView->ResetScrolling();

		pView->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height() ,SWP_NOACTIVATE);
		pView->ShowWindow(SW_SHOW);
	}
	else
	{
		pView->SetForegroundWindow();
		pPrevView = pCurrentView;
	}
	
	pCurrentView = pView;

	/*((CFormStatus*)theApp.pFormStatus)->str_status.Empty();
	((CFormStatus*)theApp.pFormStatus)->UpdateData(FALSE);*/
   
   return TRUE;
}
    

void CSplitFrame::SetCamView()
{
	// Afficher Fenetre Cameras
	ReplaceView(1,0,theApp.CamView);
}

void CSplitFrame::SetDefautPisteView()
{
	//V29 Afficher Fenetre defauts par piste
	ReplaceView(1,0,theApp.DefautPisteView);
}

void CSplitFrame::SetParamView()
{
	// Afficher Fenetre Paramètres
	ReplaceView(1,0,theApp.ParamView);
}


void CSplitFrame::OnViewMachine()
{
	if(theApp.DialMachineEnCours == NULL) return;
	
	//save fenetre précédente
	theApp.DialMachinePrev = theApp.DialMachineEnCours;

	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.DialMachineEnCours);
}
void CSplitFrame::OnViewAlarmes()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.AlarmesView);
}
void CSplitFrame::SetVisionView()
{
	// Afficher Fenetre Traitements
	ReplaceView(1,0,theApp.VisionView);
}
bool CSplitFrame::DetachCurrentView()
{
	//Detacher la Dialog En cours de l'application 
	if(!pCurrentView->IsDetached)
	{
		//Detachement
		pCurrentView->IsDetached = true;

		//Deplacement pour montrer le detachement
		m_wndSplitter.GetPane(1,0)->GetWindowRect(&rect);

		//Affichage de la bordure
		//pCurrentView->ModifyStyle(WS_SYSMENU,WS_CAPTION |WS_THICKFRAME |WS_OVERLAPPED,SWP_FRAMECHANGED);
		pCurrentView->ModifyStyle(WS_SYSMENU,WS_VSCROLL|WS_HSCROLL|WS_CAPTION |WS_THICKFRAME |WS_OVERLAPPED,SWP_FRAMECHANGED);
		pCurrentView->SetWindowPos(NULL,rect.left + 50,rect.top + 50,rect.Width() ,rect.Height() ,SWP_FRAMECHANGED);

		return true;
	}
	else
		return false;


}
bool CSplitFrame::AttachCurrentView()
{
	//Re-attacher la Dialog En cours de l'application 
	
	if(!pCurrentView->IsDetached)
	{
		return false;
	}
	else	
	{
		//Attachement
		pCurrentView->IsDetached = false;

		pCurrentView->ResetScrolling();
		
		//Deplacement pour montrer le detachement
		m_wndSplitter.GetPane(1,0)->GetWindowRect(&rect);

		//Affichage de la bordure
		//pCurrentView->ModifyStyle(WS_CAPTION |WS_THICKFRAME |WS_OVERLAPPED,0 ,SWP_FRAMECHANGED);
		pCurrentView->ModifyStyle(WS_VSCROLL|WS_HSCROLL|WS_CAPTION |WS_THICKFRAME |WS_OVERLAPPED,0 ,SWP_FRAMECHANGED);
		pCurrentView->SetWindowPos(NULL,rect.left ,rect.top ,rect.Width() ,rect.Height() ,SWP_FRAMECHANGED);
		
		return true;
	}


}
//void CSplitFrame::OnViewNew()
//{
//	// Afficher Fenetre Traitements
//	AfxMessageBox("New bouton ToolBar");
//	
//}
void CSplitFrame::HideAll()
{
	for(int i = 0; i < theApp.iNbDialog;i++)
	{
		if(!theApp.TabDial[i]->IsDetached)
			theApp.TabDial[i]->ShowWindow(SW_HIDE);
	}
}

void CSplitFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);

	// On Move
	CRect cr;
    GetWindowRect(&cr);
	int Sizey = 0;
	//ScreenToClient(&cr);

	Sizey = cr.Height() - SIZE_STATUSVIEW - SIZE_MENUVIEW - 100;
	if(Sizey < 0) Sizey = 0;

    if (  m_wndSplitter.IsWindowVisible() )
    {
		m_wndSplitter.GetPane(1,0)->GetWindowRect(&rect);
	
		if(::IsWindow(pCurrentView->GetSafeHwnd()))
		{
			if(!pCurrentView->IsDetached)
			{
				pCurrentView->ResetScrolling();
				pCurrentView->SetWindowPos(NULL,rect.left,rect.top ,rect.Width(),rect.Height() ,SWP_SHOWWINDOW |SWP_FRAMECHANGED);
				this->SetForegroundWindow();
			}

			if((pPrevView != NULL)&&(!pPrevView->IsDetached))
			{
				pPrevView->ResetScrolling();
				pPrevView->SetWindowPos(NULL,rect.left,rect.top ,rect.Width(),rect.Height() ,SWP_SHOWWINDOW |SWP_FRAMECHANGED);
			}
   
		}

		m_wndSplitter.SetRowInfo( 1,Sizey, 0);
        m_wndSplitter.RecalcLayout();
    }

}

void CSplitFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID==SC_CLOSE)
	{
		if(theApp.Process.iEtat != ETAT_PROCESS_INITAPP)
		{
			if(theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION)
			{
	     
				return ; // pas de fermeture
			}
		}
	}

	CFrameWnd::OnSysCommand(nID, lParam);
}

void CSplitFrame::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	if(strFonction == "AFF_CURRENTMACHINEVIEW")
	{
		OnViewMachine();
	}
	else if(strFonction == "AFF_MACHINEVIEW")
	{
		theApp.DialMachineEnCours = theApp.MachineView;
		OnViewMachine();
	}
	else if(strFonction == "AFF_CAMVIEW")
	{
		SetCamView();
	}
	else if(strFonction == "AFF_PARAMVIEW")
	{
		SetParamView();
	}
	else if(strFonction == "AFF_VISIONVIEW")
	{
		//V29 SetVisionView();
		SetDefautPisteView();
	}
	else if(strFonction == "AFF_VISIONDATA")
	{
		SetVisionView();
	}
	else if(strFonction == "AFF_ALARMESVIEW")
	{
		OnViewAlarmes();
	}
	else if(strFonction == "AFF_ROBOTSVIEW")
	{
		SetRobotView();
	}
	else if(strFonction == "AFF_PESONSVIEW")
	{
		//on interdit l'accés à la fenetre peson 
		//si on est en validation de fin de lot
		if(theApp.Process.iEtat!= ETAT_PROCESS_VALIDATIONFINLOT)
		{
			SetPesonView();
		}
	}
	else if(strFonction == "AFF_OPCVIEW")
	{
		SetOPCView();
	}
	else if(strFonction == "AFF_CHARGEMENTVIEW")
	{
		SetChargementView();
	}
	else if(strFonction == "AFF_INITIALISATIONVIEW")
	{
		SetInitialisationView();
	}
	else if(strFonction == "AFF_VALIDATIONVIEW")
	{
		SetValidationView();
	}
	else if(strFonction == "AFF_DMXVIEW")
	{
		SetDmxView();
	}
	else if(strFonction == "AFF_LOGIN")
	{
		theApp.OnLogin();
	}
	else if(strFonction == "AFF_UEYE_VIEW")
	{
		SetUeyeView();
	}
	else if(strFonction == "AFF_PESEE_VIEW")
	{
		SetPeseeView();
	}
	else if(strFonction == "AFF_ETIQ_VIEW")
	{
		SetEtiqView();
	}
	else if(strFonction == "AFF_DETACH_CURRENT_VIEW")
	{
		if(!DetachCurrentView())
		{
			AttachCurrentView();
		}
	}
}

void CSplitFrame::SetDmxView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.DMXView);
}
void CSplitFrame::SetChargementView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.ChargementView);
}
void CSplitFrame::SetValidLotView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.ValidLotView);
}
void CSplitFrame::SetInitialisationView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.InitialisationView);
}
void CSplitFrame::SetValidationView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.ValidationView);
}
void CSplitFrame::SetRobotView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.RobotsView);
}
void CSplitFrame::SetPesonView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.PesonsView);
}
void CSplitFrame::SetOPCView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.OPCView);
}
void CSplitFrame::SetUeyeView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.UeyeView);
}
void CSplitFrame::SetPeseeView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.PeseeView);
}
void CSplitFrame::SetEtiqView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.EtiqView);
}
void CSplitFrame::SetMixupView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1,0,theApp.DialMixup);
}
void CSplitFrame::SetFormulaireView()
{
	// Afficher Fenetre Alarmes
	ReplaceView(1, 0, theApp.DialFormulaire);
}
void CSplitFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);

	if (bActive == TRUE)
	{
		if (!theApp.GetMainWnd()->IsZoomed())
		{
			theApp.GetMainWnd()->ShowWindow(SW_RESTORE);
		}
	}
}
