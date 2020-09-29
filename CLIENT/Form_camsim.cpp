// Form_camsim.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "Form_camsim.h"
#include "General.h"

// Boîte de dialogue CForm_camsim

IMPLEMENT_DYNAMIC(CForm_camsim, CDialog)

CForm_camsim::CForm_camsim(CWnd* pParent /*=NULL*/)
	: CDialog(CForm_camsim::IDD, pParent)
{
	Cam = NULL; 
}

CForm_camsim::~CForm_camsim()
{
}

void CForm_camsim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CForm_camsim, CDialog)
	ON_MESSAGE(WM_MESSFORMCAM, &CForm_camsim::AfficherRep)
END_MESSAGE_MAP()


// Gestionnaires de messages de CForm_camsim
LRESULT CForm_camsim::AfficherRep(WPARAM Param1, LPARAM Param2)
{
		char CharRecv= (char) Param1;
		
		if(Cam != NULL)
			Cam->AfficheRep(CharRecv);
		return 0;
}
