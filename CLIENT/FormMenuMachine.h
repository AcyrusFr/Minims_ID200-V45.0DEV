#pragma once
#include "staticmenu.h"
#include "StaticPolice.h"



// Mode formulaire CFormMenuMachine

class CFormMenuMachine : public CFormView
{
	DECLARE_DYNCREATE(CFormMenuMachine)

protected:
	CFormMenuMachine();           // constructeur protégé utilisé par la création dynamique
	virtual ~CFormMenuMachine();

public:
	enum { IDD = IDD_FORMMENUMACHINE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CStaticMenu MenuMachine;
	virtual void OnInitialUpdate();
	CString strEtatMachine;
	CStaticPolice StaticInfos;
	CStaticPolice staticUtilisateur;
};


