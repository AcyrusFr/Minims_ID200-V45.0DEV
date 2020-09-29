#pragma once
#include "afxwin.h"



// Mode formulaire CFormBase

class CFormBase : public CFormView
{
	DECLARE_DYNCREATE(CFormBase)

protected:
	CFormBase();           // constructeur protégé utilisé par la création dynamique
	virtual ~CFormBase();

public:
	enum { IDD = IDD_FORMBASE };
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListBox ListeInitialisation;
	virtual void OnInitialUpdate();
};


