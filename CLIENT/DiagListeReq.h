#pragma once


// Boîte de dialogue CDiagListeReq

class CDiagListeReq : public CDialog
{
	DECLARE_DYNAMIC(CDiagListeReq)

public:
	CDiagListeReq(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDiagListeReq();

// Données de boîte de dialogue
	enum { IDD = IDD_LISTEAPP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CString strRequete;
	CString sResult;
	CString sChamp;
	CListBox m_Liste;
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkList1();
};
