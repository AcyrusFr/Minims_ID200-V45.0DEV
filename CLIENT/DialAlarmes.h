#pragma once
#include "gridctrl.h"
#include "afxwin.h"
#include "staticmenu.h"
#include "ACYListBox.h"

#include "AcyDial.h"//Scroll + Detachement
// Boîte de dialogue CDialAlarmes

class CDialAlarmes : public CAcyDial
{
	DECLARE_DYNAMIC(CDialAlarmes)

public:
	CDialAlarmes(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialAlarmes();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALALARMES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	int m_nCurHeight;
	int m_nScrollPos;
	CRect m_rect;

	CGridCtrl grilleAlarmes;
	virtual BOOL OnInitDialog();
	int UpdateAlarme(void);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CACYListBox m_EventsList;
	void AfficherChaineListe(CString sChaine);
	CACYListBox ListeDefEnCours;
	void AfficherDefautEnCours(CString sChaine);
	void ResetDefautEnCours();
	CStaticMenu Menu_Alarmes;
	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	CACYListBox Liste_Alertes;
};
