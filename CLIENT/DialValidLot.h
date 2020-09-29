#pragma once
#include "gridctrl.h"
#include "staticmenu.h"
#include "staticpolice.h"
#include "AcyDial.h"

#define ID_TIMER_VALIDPESONS	1000

// Boîte de dialogue CDialValidLot

class CDialValidLot : public CAcyDial
{
	DECLARE_DYNAMIC(CDialValidLot)

public:
	CDialValidLot(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialValidLot();

// Données de boîte de dialogue
	enum { IDD = IDD_DIAL_VALIDATIONLOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CClientOPC* m_ClientOPC;
	CRecette * Recette;
	UINT_PTR TimerLecture;

	bool bAllPesons;
	int iPesonSelected;
	bool bDebutLot;

	CGridCtrl GrilleValidLot;
	virtual BOOL OnInitDialog();
	int UpdatePesonWindow(void);
	int ResetGrille(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int StartLecture(void);
	int StopLecture(void);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CString strNumLot;
	CStaticMenu Menu_ValidLotUnlock;
	CStaticMenu Menu_ValidLot;
	CStaticPolice static_Lot;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
