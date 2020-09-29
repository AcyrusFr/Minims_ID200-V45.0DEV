#pragma once
#include "AcyDial.h"
#include "staticmenu.h"
#include "afxwin.h"

// Boîte de dialogue CDialEtiquette

class CDialEtiquette : public CAcyDial
{
	DECLARE_DYNAMIC(CDialEtiquette)

public:
	CDialEtiquette(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialEtiquette();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALETIQUETTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	CStaticMenu Menu_Etiquette;
	CStatic static_image;
};
