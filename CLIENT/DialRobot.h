#pragma once
#include "Robot.h"
#include "gridctrl.h"
#include "staticmenu.h"
#include "afxwin.h"
#include "ACYListBox.h"
#include "AcyDial.h"

#define TIMER_AFF_ROBOTS	1000
// Boîte de dialogue CDialRobot

class CDialRobot : public CAcyDial
{
	DECLARE_DYNAMIC(CDialRobot)

public:
	CDialRobot(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialRobot();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALROBOT };

	CRobot* m_robot;
	bool bAllSelected;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:

	CGridCtrl GrilleEtats;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioRobot1();
	afx_msg void OnBnClickedRadioRobot2();
	afx_msg void OnBnClickedRadioRobot3();
	afx_msg void OnBnClickedRadioRobotall();
	int UpdateFenetre(void);
	CStaticMenu Menu_robot;
	
	UINT_PTR TimerLecture;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int StartLecture(void);
	int StopLecture(void);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	CACYListBox ListHistoRobot;

	void MAJHistoPositions(void);
	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
