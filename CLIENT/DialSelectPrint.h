#pragma once

#define IMPRESSION 1
#define PDF 2
#define EXCEL 3
#define PREVISU 4

// Boîte de dialogue CDialSelectPrint

class CDialSelectPrint : public CDialog
{
	DECLARE_DYNAMIC(CDialSelectPrint)

public:
	CDialSelectPrint(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialSelectPrint();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALSELECTPRINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel2();
public:
	afx_msg void OnBnClickedCancel4();
public:
	afx_msg void OnBnClickedCancel3();
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnBnClickedButselection();
public:
	afx_msg void OnBnClickedButofencours();
public:
	int iExport;
	bool bAskFile;

	CString strFrameOF;
	CString strOfEnCours;
	afx_msg void OnBnClickedRadioExp1();
	afx_msg void OnBnClickedRadioExp2();
	afx_msg void OnBnClickedRadioExp3();
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedRadioExp4();
};
