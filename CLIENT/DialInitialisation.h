#pragma once
#include "afxwin.h"
#include "staticmenu.h"
#include "ProcessDmx.h"
#include "staticpolice.h"
//V 44.7 #include "ListBoxNotify.h"
#include "ACYListBox.h"

#include "AcyDial.h"

//#define ID_TIMER_SENDDMX	1000
#define ID_TIMER_TIMEOUT	1001

// Boîte de dialogue CDialInitialisation

class CDialInitialisation : public CAcyDial
{
	DECLARE_DYNAMIC(CDialInitialisation)

public:
	CDialInitialisation(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialInitialisation();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALINITIALISATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	//UINT_PTR TimerDmx;
	//CProcessDMX * Dmx;

	UINT_PTR TimerTimeout;
	bool bTimeOut;
	bool bTimeOutHappened;

	CACYListBox ListeInit;
	int SetStringListe(CString strToAdd);
	int ResetListe(void);
	CStaticMenu MenuInit;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//void StartTimerDmx(void);
	//void StopTimerDmx(void);
	void StartTimerTimeout(void);
	void StopTimerTimeout(void);
	CStaticPolice static_init;
};
