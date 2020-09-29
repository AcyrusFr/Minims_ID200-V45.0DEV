// DialSelectPrint.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialSelectPrint.h"
#include "DiagListe.h"


// Boîte de dialogue CDialSelectPrint

IMPLEMENT_DYNAMIC(CDialSelectPrint, CDialog)

CDialSelectPrint::CDialSelectPrint(CWnd* pParent /*=NULL*/)
	: CDialog(CDialSelectPrint::IDD, pParent)
{
	strFrameOF.Empty();
	iExport = IMPRESSION;
	bAskFile = false; //PAr défaut
}

CDialSelectPrint::~CDialSelectPrint()
{
}

void CDialSelectPrint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialSelectPrint, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDialSelectPrint::OnBnClickedCancel)
	ON_BN_CLICKED(ID_BUTSELECTION, &CDialSelectPrint::OnBnClickedButselection)
	ON_BN_CLICKED(ID_BUTOFENCOURS, &CDialSelectPrint::OnBnClickedButofencours)
	ON_BN_CLICKED(IDC_RADIO_EXP1, &CDialSelectPrint::OnBnClickedRadioExp1)
	ON_BN_CLICKED(IDC_RADIO_EXP2, &CDialSelectPrint::OnBnClickedRadioExp2)
	ON_BN_CLICKED(IDC_RADIO_EXP3, &CDialSelectPrint::OnBnClickedRadioExp3)
	ON_BN_CLICKED(IDC_RADIO_EXP4, &CDialSelectPrint::OnBnClickedRadioExp4)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialSelectPrint

void CDialSelectPrint::OnBnClickedCancel()
{
	// Annulation Impression
	OnCancel();
}

void CDialSelectPrint::OnBnClickedButselection()
{
	//Ouverture fenetre selection OF à imprimer
	CDiagListe sDialog;

	sDialog.pAdoDatabase = &theBase.dao_Base;
	sDialog.sChamp = "Numero";
	sDialog.sTable = "Coupes";
	sDialog.sCritere = "";
	sDialog.sResult = "";
	sDialog.DoModal();

	if(sDialog.sResult != "" )
	{	
		strFrameOF = sDialog.sResult;
		OnOK();
	}
	else
	{
		OnCancel();
	}

	
}

void CDialSelectPrint::OnBnClickedButofencours()
{
	// Ouverture de l'OF en cours
	if(strOfEnCours != "")
		strFrameOF = strOfEnCours;
	else
		strFrameOF = "%";

	OnOK();
}


void CDialSelectPrint::OnBnClickedRadioExp1()
{
	// Impression
	iExport = IMPRESSION;
	bAskFile = false;
}

void CDialSelectPrint::OnBnClickedRadioExp2()
{
	//Export Pdf
	iExport = PDF;
	bAskFile = true;
}

void CDialSelectPrint::OnBnClickedRadioExp3()
{
	//Export Excel
	iExport = EXCEL;
	bAskFile = true;
}

BOOL CDialSelectPrint::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IMpression par défaut
	CheckRadioButton(IDC_RADIO_EXP1,IDC_RADIO_EXP3,IDC_RADIO_EXP1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialSelectPrint::OnBnClickedRadioExp4()
{
	//¨Previsu
	iExport = PREVISU;
	bAskFile = false;
}
