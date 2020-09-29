#pragma once
#include "ClientOPC.h"
#include "gridctrl.h"
//#include "voyant.h"
#include "..\\COMMUN\VOYANT\Voyant.h"

#include "AcyDial.h"
#define TIMER_AFF	1000
// Boîte de dialogue CDialGrpOPC

class CDialGrpOPC : public CAcyDial
{
	DECLARE_DYNAMIC(CDialGrpOPC)

public:
	CDialGrpOPC(CWnd* pParent = NULL, CClientOPC* OPCGroupe = NULL);   // constructeur standard
	virtual ~CDialGrpOPC();
	CClientOPC * Groupe;
// Données de boîte de dialogue
	enum { IDD = IDD_DIAL_OPC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CGridCtrl Grille;
	UINT_PTR TimerAff;
	void InitialiserGrille(void);

	afx_msg void OnBnClickedButsetval();
	afx_msg void OnBnClickedButgetval();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int StartLecture(void);
	int StopLecture(void);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ResetGrille(void);
	afx_msg void OnBnClickedRadAuto();
	afx_msg void OnBnClickedRadManu();
	afx_msg void OnBnClickedRadManuraf();
	afx_msg void OnBnClickedRadAutoraf();
	afx_msg void OnBnClickedButresetopccom();
	afx_msg void OnBnClickedRadprocessw();
	afx_msg void OnBnClickedRadrobotread1();
	afx_msg void OnBnClickedRadrobotwrite1();
	afx_msg void OnBnClickedRadrobotread2();
	afx_msg void OnBnClickedRadrobotwrite2();
	afx_msg void OnBnClickedRadrobotread3();
	afx_msg void OnBnClickedRadrobotwrite3();
	afx_msg void OnBnClickedRadprocessw2();
	afx_msg void OnBnClickedRadprocessw3();
public:
	afx_msg void OnBnClickedRadrobotread4();
	CVoyant voy_Auto;
	CVoyant voy_Vidage;
	CVoyant voy_Maint;
	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	
public:
	afx_msg void OnBnClickedRadrobotwrite4();
	afx_msg void OnBnClickedRadrobotwrite7();
	afx_msg void OnBnClickedRadrobotwrite5();
	afx_msg void OnBnClickedRadrobotwrite6();
};
