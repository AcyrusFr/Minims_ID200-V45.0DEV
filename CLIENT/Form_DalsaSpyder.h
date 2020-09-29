#pragma once
#include "DalsaSpyder3CL.h"
#include "gridctrl.h"

#include "Acydial.h"
#include "resource.h"

#define NBPARAMAFFDALSA	48 //47 params + legénde

// Boîte de dialogue CForm_DalsaSpyder

class CForm_DalsaSpyder : public CAcyDial
{
	DECLARE_DYNAMIC(CForm_DalsaSpyder)

public:
	CForm_DalsaSpyder(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CForm_DalsaSpyder();

// Données de boîte de dialogue
	enum { IDD = IDD_FORMDALSASPYDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CString str_InfosCom;
	CString str_InfosReg;
	CDalsaSpyder3CL* Cam;
	CGridCtrl grille ;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButWritecmd();
	afx_msg void OnBnClickedButOpendoc();
	CString str_commande;
	CString str_recv;
	afx_msg void OnBnClickedButgetvalue();
	afx_msg void OnBnClickedButsetvalue();
	LRESULT AfficherRep(WPARAM Param1, LPARAM Param2);
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam ); 
	afx_msg void OnBnClickedRadioCam1();
	afx_msg void OnBnClickedRadioCam2();
	int RefreshWindow(void);
};
