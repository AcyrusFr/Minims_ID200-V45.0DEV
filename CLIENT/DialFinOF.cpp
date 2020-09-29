// DialFinOF.cpp�: fichier d'impl�mentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialFinOF.h"


// Bo�te de dialogue CDialFinOF

IMPLEMENT_DYNAMIC(CDialFinOF, CDialog)

CDialFinOF::CDialFinOF(CWnd* pParent /*=NULL*/)
	: CDialog(CDialFinOF::IDD, pParent)
{
	ResultModal = IDABORT;
}

CDialFinOF::~CDialFinOF()
{
}

void CDialFinOF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialFinOF, CDialog)
	ON_BN_CLICKED(IDYES, &CDialFinOF::OnBnClickedYes)
	ON_BN_CLICKED(IDNO, &CDialFinOF::OnBnClickedNo)
	ON_BN_CLICKED(IDABORT, &CDialFinOF::OnBnClickedAbort)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialFinOF

BOOL CDialFinOF::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION�: les pages de propri�t�s OCX devraient retourner FALSE
}


void CDialFinOF::OnBnClickedYes()
{
	// TODO Solder OF

	ResultModal = IDYES;
	CDialog::OnOK();
}

void CDialFinOF::OnBnClickedNo()
{
	//  TODO Arr�t syst�me
	ResultModal = IDNO;
	CDialog::OnOK();
}

void CDialFinOF::OnBnClickedAbort()
{
	//Annulation
	ResultModal = IDABORT;
	CDialog::OnOK();
}
