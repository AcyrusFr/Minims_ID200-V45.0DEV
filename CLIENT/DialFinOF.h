#pragma once
#include "staticmenu.h"


// Boîte de dialogue CDialFinOF

class CDialFinOF : public CDialog
{
	DECLARE_DYNAMIC(CDialFinOF)

public:
	CDialFinOF(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialFinOF();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedYes();
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedAbort();
	int ResultModal;
};
