#pragma once
#include "buttonalarme.h"
#include "staticmenu.h"



// Mode formulaire CFormStatus

class CFormStatus : public CFormView
{
	DECLARE_DYNCREATE(CFormStatus)

protected:
	CFormStatus();           // constructeur protégé utilisé par la création dynamique
	virtual ~CFormStatus();

public:
	enum { IDD = IDD_FORMSTATUS };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	bool bIsAlarmeVisible;

	DECLARE_MESSAGE_MAP()
public:
	CString str_status;
	UINT_PTR TimerAlarme;
	CButtonAlarme butAlarme;
	afx_msg void OnBnClickedButalarme();
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	CStaticMenu MenuStatus;
};


