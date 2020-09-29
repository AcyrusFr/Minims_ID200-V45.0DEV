#pragma once

//#include "BaseFenetre.h"


#include "resource.h"
#include "gridctrl.h"
#include "Recette.h"



// Boîte de dialogue CDialAffGridRecette

class CDialAffGridRecette : public CDialog
{
	DECLARE_DYNAMIC(CDialAffGridRecette)

public:
	CDialAffGridRecette(CWnd* pParent = NULL,CRecette * c_pRecette = NULL);   // constructeur standard
	virtual ~CDialAffGridRecette();

// Données de boîte de dialogue
	enum { IDD = IDD_DIAL_AFFRECETTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()

public:
	CRecette * pRecette;
	CGridCtrl GrilleRecetteAff;
	LOGFONT myLogFont;
	void InitialiserGrilleRecette(void);
	virtual BOOL OnInitDialog();
};
