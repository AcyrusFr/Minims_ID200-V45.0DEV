// FormBase.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "FormBase.h"
#include "splitfrm.h"
#include "DialMachine.h"

// CFormBase

IMPLEMENT_DYNCREATE(CFormBase, CFormView)

CFormBase::CFormBase()
	: CFormView(CFormBase::IDD)
{

}

CFormBase::~CFormBase()
{
}

void CFormBase::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, ListeInitialisation);
}

BEGIN_MESSAGE_MAP(CFormBase, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Diagnostics de CFormBase

#ifdef _DEBUG
void CFormBase::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormBase::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// Gestionnaires de messages de CFormBase

void CFormBase::OnTimer(UINT_PTR nIDEvent)
{
	// Timer Initialisation
	switch(nIDEvent)
	{
		case ID_TIMER_PROCESS:
			KillTimer(ID_TIMER_PROCESS);

			theApp.Process.Gerer(0);
			//V 44.7 optimisation : deja fait dans le timer d'affichage de la dialogue
			//if(theApp.Process.iEtat != ETAT_PROCESS_INITAPP)	//V18
			//{
			//	((CDialMachine * )theApp.MachineView)->MAJGrilleComptage();
			//}

			theApp.TimerProcess = this->SetTimer(ID_TIMER_PROCESS,DELAI_TIMER_PROCESS,NULL);
			break;

	}

	CFormView::OnTimer(nIDEvent);
}

void CFormBase::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// Initialisation de la Liste

	//Changement de la police de caractère
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
				   "Arial");                 // lpszFacename

	ListeInitialisation.SetFont(&NewFont,TRUE);
}
