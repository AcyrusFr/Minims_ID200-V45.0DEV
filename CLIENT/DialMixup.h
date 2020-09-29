#pragma once
#include "staticpolice.h"
#include "staticmenu.h"
#include "afxwin.h"
//#include "ListBoxNotify.h"
#include "ACYListBox.h"
#include "AcyDial.h"

// Bo�te de dialogue CDialMixup

class CDialMixup : public CAcyDial
{
	DECLARE_DYNAMIC(CDialMixup)

public:
	CDialMixup(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialMixup();

// Donn�es de bo�te de dialogue
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
