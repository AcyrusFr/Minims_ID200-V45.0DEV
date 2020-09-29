// DiagSaisie.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DiagSaisie.h"


// Boîte de dialogue CDiagSaisie

IMPLEMENT_DYNAMIC(CDiagSaisie, CDialog)

CDiagSaisie::CDiagSaisie(CWnd* pParent /*=NULL*/)
	: CDialog(CDiagSaisie::IDD, pParent)
	, strSaisie(_T(""))
	, strInfos(_T(""))
{
	bPassword = false;
	strTitre = "";

}

CDiagSaisie::~CDiagSaisie()
{
}

void CDiagSaisie::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIAGSAISIE, strSaisie);
	DDX_Text(pDX, IDC_STATICINFOS, strInfos);
}


BEGIN_MESSAGE_MAP(CDiagSaisie, CDialog)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_0, &CDiagSaisie::OnBnClickedButPavnum0)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_PT, &CDiagSaisie::OnBnClickedButPavnumPt)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_RET, &CDiagSaisie::OnBnClickedButPavnumRet)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_1, &CDiagSaisie::OnBnClickedButPavnum1)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_2, &CDiagSaisie::OnBnClickedButPavnum2)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_3, &CDiagSaisie::OnBnClickedButPavnum3)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_4, &CDiagSaisie::OnBnClickedButPavnum4)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_5, &CDiagSaisie::OnBnClickedButPavnum5)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_6, &CDiagSaisie::OnBnClickedButPavnum6)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_7, &CDiagSaisie::OnBnClickedButPavnum7)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_8, &CDiagSaisie::OnBnClickedButPavnum8)
	ON_BN_CLICKED(IDC_BUT_PAVNUM_9, &CDiagSaisie::OnBnClickedButPavnum9)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDiagSaisie

void CDiagSaisie::OnBnClickedButPavnum7()
{
	// Bouton 7 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD7;
    key.wScan = ::VkKeyScan(VK_NUMPAD7);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum8()
{
	// Bouton 8 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD8;
    key.wScan = ::VkKeyScan(VK_NUMPAD8);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum9()
{
	// Bouton 9 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD9;
    key.wScan = ::VkKeyScan(VK_NUMPAD9);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum4()
{
	// Bouton 4 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD4;
    key.wScan = ::VkKeyScan(VK_NUMPAD4);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum5()
{
	// Bouton 5 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD5;
    key.wScan = ::VkKeyScan(VK_NUMPAD5);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum6()
{
	// Bouton 6 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD6;
    key.wScan = ::VkKeyScan(VK_NUMPAD6);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum1()
{
	// Bouton 1 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD1;
    key.wScan = ::VkKeyScan(VK_NUMPAD1);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum2()
{
	// Bouton 2 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD2;
    key.wScan = ::VkKeyScan(VK_NUMPAD2);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum3()
{
	// Bouton 3 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD3;
    key.wScan = ::VkKeyScan(VK_NUMPAD3);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnum0()
{
	// Bouton 0 du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_NUMPAD0;
    key.wScan = ::VkKeyScan(VK_NUMPAD0);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnumPt()
{
	// Bouton . du pavé Numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_DECIMAL;
    key.wScan = ::VkKeyScan(VK_DECIMAL);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}

void CDiagSaisie::OnBnClickedButPavnumRet()
{
	// Bouton Retour du pavé numérique
	this->GetDlgItem(IDC_EDIT_DIAGSAISIE)->SetFocus();

	INPUT keyInput;
	keyInput.type = INPUT_KEYBOARD;
	
	KEYBDINPUT key;
	key.wVk = VK_BACK;
    key.wScan = ::VkKeyScan(VK_BACK);
       
    key.dwFlags = 0;

    keyInput.ki = key;
    ::SendInput(1,&keyInput,sizeof(INPUT));
}


BOOL CDiagSaisie::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Initialisation du titre de la fenétre
	if(strTitre!="") this->SetWindowText(strTitre);
	else this->SetWindowText("Fenetre de saisie");

	if(bPassword)
	{
		CEdit* pEdit = (CEdit*) this->GetDlgItem(IDC_EDIT_DIAGSAISIE);
		pEdit->SetPasswordChar( '#' );
		pEdit->ModifyStyle( 0, ES_PASSWORD );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
