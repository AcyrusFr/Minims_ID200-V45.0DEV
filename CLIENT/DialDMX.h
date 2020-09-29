#pragma once
#include "ProcessDMX.h"
#include "gridctrl.h"
#include "staticmenu.h"


#define TIMER_AFF_DMX	1000
// Boîte de dialogue CDialDMX
#include "AcyDial.h"
#include "afxwin.h"
#include "ACYListBox.h"

class CDialDMX : public CAcyDial
{
	DECLARE_DYNAMIC(CDialDMX)

public:
	CDialDMX(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialDMX();

// Données de boîte de dialogue
	UINT_PTR TimerLecture;
	CProcessDMX * m_ProcessDMX;

	int UpdateDMXWindow(void);
	CGridCtrl m_GrilleDMX;

	CStaticMenu Menu_DMX;

	CACYListBox ListeDMX;
	CACYListBox ListeDMX2;
	CACYListBox ListeDMX3;

	enum { IDD = IDD_DIALDMX};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:

	
	
	afx_msg void OnBnClickedRaddmx1();
	afx_msg void OnBnClickedRaddmx2();
	afx_msg void OnBnClickedRaddmx3();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int StartLecture(void);
	int StopLecture(void);
	void  ExecuterFonctionExt(CString strFonction,void * BoutonParent);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	
};
