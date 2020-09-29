#pragma once

#include "AcyDial.h"
#include "staticmenu.h"
#include "afxwin.h"
#include "gridctrl.h"
#include "ACYListBox.h"
// Boîte de dialogue CDialPesee

class CDialPesee : public CAcyDial
{
	DECLARE_DYNAMIC(CDialPesee)

public:
	CDialPesee(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialPesee();

// Données de boîte de dialogue
	enum { IDD = IDD_DIAL_PESEE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CStaticMenu Menu_PesonEtui;
	CACYListBox m_ListeHisto;
	CGridCtrl GrilleEtats;


	virtual BOOL OnInitDialog();
	void ExecuterFonctionExt(CString strFonction,void * BoutonParent);
	void AfficherParamGrille(void);
	int AfficherHistoMesures(void);
	LRESULT OnMAJMesure(WPARAM Param1, LPARAM Param2);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
