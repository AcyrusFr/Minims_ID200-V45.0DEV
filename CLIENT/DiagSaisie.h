#pragma once


// Boîte de dialogue CDiagSaisie

class CDiagSaisie : public CDialog
{
	DECLARE_DYNAMIC(CDiagSaisie)

public:
	CDiagSaisie(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDiagSaisie();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALLOG_SAISIE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	bool bPassword;
	CString strSaisie;
	CString strInfos;
	CString strTitre;
	afx_msg void OnBnClickedButPavnum0();
	afx_msg void OnBnClickedButPavnumPt();
	afx_msg void OnBnClickedButPavnumRet();
	afx_msg void OnBnClickedButPavnum1();
	afx_msg void OnBnClickedButPavnum2();
	afx_msg void OnBnClickedButPavnum3();
	afx_msg void OnBnClickedButPavnum4();
	afx_msg void OnBnClickedButPavnum5();
	afx_msg void OnBnClickedButPavnum6();
	afx_msg void OnBnClickedButPavnum7();
	afx_msg void OnBnClickedButPavnum8();
	afx_msg void OnBnClickedButPavnum9();
	virtual BOOL OnInitDialog();
};
