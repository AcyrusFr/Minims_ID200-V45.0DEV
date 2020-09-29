#pragma once
#include "gridctrl.h"
#include "afxwin.h"
#include "staticmenu.h"
//#include "ListBoxNotify.h"
#include "ACYListBox.h"
#include "AcyDial.h"

#define TIMER_AFF_PESONS	1000
// Boîte de dialogue CDialPesons

class CDialPesons : public CAcyDial
{
	DECLARE_DYNAMIC(CDialPesons)

public:
	CDialPesons(CWnd* pParent = NULL, CPeson * Peson = NULL);   // constructeur standard
	virtual ~CDialPesons();

// Données de boîte de dialogue
	enum { IDD = IDD_DIAL_PESONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CPeson * Peson;
	CGridCtrl GrilleEtats;

	UINT_PTR TimerLecture;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioPeson1();
	afx_msg void OnBnClickedRadioPeson2();
	afx_msg void OnBnClickedRadioPeson3();
	afx_msg void OnBnClickedRadioPeson4();
	afx_msg void OnBnClickedRadioPeson5();
	afx_msg void OnBnClickedRadioPeson6();
	int AfficherHistoMesures(void);
	int UpdateDialPeson(void);
	afx_msg void OnBnClickedRadioPesonall();
	bool bSelectAllPesons;
	CACYListBox m_ListeHisto;
	int AfficherParamGrille(void);

	afx_msg void OnBnClickedButSavemesuresdyna();
	CStaticMenu MenuDiagPeson;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int StartLecture(void);
	int StopLecture(void);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	LRESULT OnMAJMesure(WPARAM Param1, LPARAM Param2);
	LRESULT OnDefautPoidsMin(WPARAM Param1, LPARAM Param2);
	LRESULT OnDefautPoidsMax(WPARAM Param1, LPARAM Param2);
	
	void  ExecuterFonctionExt(CString strFonction,void * BoutonParent);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
