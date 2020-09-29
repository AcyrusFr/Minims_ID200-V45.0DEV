#pragma once
#include "staticpolice.h"
#include "staticmenu.h"
#include "afxwin.h"
//#include "ListBoxNotify.h"
#include "ACYListBox.h"
#include "AcyDial.h"

// Boîte de dialogue CDialMixup

class CDialMixup : public CAcyDial
{
	DECLARE_DYNAMIC(CDialMixup)

public:
	CDialMixup(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialMixup();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALMIXUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CStaticPolice staticTitre;
	virtual BOOL OnInitDialog();
	CStaticMenu Menu_Mixup;
	CACYListBox ListCurrentDef;
	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
