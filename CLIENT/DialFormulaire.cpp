// DialUtilisateur.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialFormulaire.h"
#include "AcyDial.h"
#include "FormStatus.h"

// Boîte de dialogue CDialFormulaire

IMPLEMENT_DYNAMIC(CDialFormulaire, CAcyDial)

CDialFormulaire::CDialFormulaire(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialFormulaire::IDD, pParent)
{
}

CDialFormulaire::~CDialFormulaire()
{
}

void CDialFormulaire::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_ManagedControl(pDX, IDC_STATIC_FUTILISATEUR, m_ctrlFormulaire);
}


BEGIN_MESSAGE_MAP(CDialFormulaire, CAcyDial)
	//ON_MESSAGE(WM_MAJ_VIEWUTILISATEUR,OnMAJViewUTILISATEUR)
END_MESSAGE_MAP()

BOOL CDialFormulaire::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	//V2.2 : Ajouter f_module et f_rech correspondant
	System::String^ sConnect = gcnew System::String (theConfig.str_NomBase);
	System::String^ sRepertoire = gcnew System::String (theConfig.str_RepertoireBase);
	m_ctrlFormulaire->Init(sConnect,sRepertoire,"","");
	m_ctrlFormulaire->hwnd = (System::IntPtr) this->GetSafeHwnd();
	m_ctrlFormulaire->OpenForm("Coupes", "C_ENT", "Production",false, false, false, false, true, true, true, false);
	m_ctrlFormulaire->SetFormMenu(false,false,true);
    m_ctrlFormulaire->cls->Frm_Lire();
    m_ctrlFormulaire->cls->Data_SetIndexChamp();
    m_ctrlFormulaire->cls->Frm_SetSMenu();
    m_ctrlFormulaire->cls->Frm_SetForm(m_ctrlFormulaire->pObjet, false);

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CDialFormulaire::OnCommand(WPARAM wParam, LPARAM lParam)
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

	return CDialog::OnCommand(wParam, lParam);
}

//
//LRESULT CDialFormulaire::OnMAJViewUTILISATEUR(WPARAM wParam, LPARAM lParam)
//{
//	theApp.ExecuterFonctionExt(_T("AFF_UTILISATEURVIEW"),NULL);
//	return 0L;
//}
