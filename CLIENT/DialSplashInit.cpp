// DialSplashInit.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialSplashInit.h"


// Boîte de dialogue CDialSplashInit

IMPLEMENT_DYNAMIC(CDialSplashInit, CDialog)

CDialSplashInit::CDialSplashInit(CWnd* pParent /*=NULL*/)
	: CDialog(CDialSplashInit::IDD, pParent)
	, strInitInfos(_T(""))
{

}

CDialSplashInit::~CDialSplashInit()
{
}

void CDialSplashInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, strInitInfos);
}


BEGIN_MESSAGE_MAP(CDialSplashInit, CDialog)

END_MESSAGE_MAP()


// Gestionnaires de messages de CDialSplashInit

BOOL CDialSplashInit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// cacher le curseur clignotant
	GetDlgItem(IDC_EDIT1)->SetFocus();
	GetDlgItem(IDC_EDIT1)->HideCaret();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

BOOL CDialSplashInit::DestroyWindow()
{
	// remettre curseur (au cas ou )
	GetDlgItem(IDC_EDIT1)->SetFocus();
	GetDlgItem(IDC_EDIT1)->ShowCaret();

	return CDialog::DestroyWindow();
}
