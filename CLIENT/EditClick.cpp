// EditClick.cpp�: fichier d'impl�mentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "EditClick.h"

#include "DiagPadNum.h"
#include "DiagPadAlpha.h"


// CEditClick

IMPLEMENT_DYNAMIC(CEditClick, CEdit)

CEditClick::CEditClick()
{
	bAlphaNum  = false;
}

CEditClick::~CEditClick()
{
}


BEGIN_MESSAGE_MAP(CEditClick, CEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// Gestionnaires de messages de CEditClick





void CEditClick::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(bAlphaNum)
	{
		// Ouverture de la fen�tre de saisie AlphaNum�rique
		CDiagPadAlpha FrmSaisie;
		
		FrmSaisie.strTitre = _T("Numero d'OF");
		FrmSaisie.strInfos = _T("Numero d'OF");

		if(FrmSaisie.DoModal() == IDOK)
		{
			SetWindowText(FrmSaisie.strSaisie);
		}

	}
	else
	{
		// Ouverture de la fen�tre de saisie Num�rique
		CDiagPadNum FrmSaisie;
		
		FrmSaisie.strTitre = _T("Numero d'OF");
		FrmSaisie.strInfos = _T("Numero d'OF");

		if(FrmSaisie.DoModal() == IDOK)
		{
			SetWindowText(FrmSaisie.strSaisie);
		}
	}


	CEdit::OnLButtonDblClk(nFlags, point);
}
