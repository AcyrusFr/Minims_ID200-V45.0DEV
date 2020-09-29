#pragma once
#include "staticmenu.h"

#include "AcyDial.h"
// Boîte de dialogue CDialValidation

class CDialValidation : public CAcyDial
{
	DECLARE_DYNAMIC(CDialValidation)

public:
	CDialValidation(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialValidation();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALVALIDATIONOF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CStaticMenu Menu_Of;
	virtual BOOL OnInitDialog();
	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
