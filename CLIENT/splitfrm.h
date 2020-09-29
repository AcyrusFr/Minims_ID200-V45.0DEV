// splitfrm.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


/////////////////////////////////////////////////////////////////////////////
// CSplitFrame frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "AcyDial.h"
#include "MySplitterWnd.h"

#define SIZE_STATUSVIEW	55
#define SIZE_MENUVIEW	65

class CSplitFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CSplitFrame)
protected:
	CSplitFrame();          // protected constructor used by dynamic creation

// Attributes
public:
	
	CMySplitterWnd    m_wndSplitter;
	//CDynToolBar    m_wndToolBar;

	CRect rect;//Rectangle ZoneSplitter  Lgn 1 Col 0

	CAcyDial* pCurrentView;
	CAcyDial* pPrevView;
public:
	virtual ~CSplitFrame();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	// Generated message map functions
	//{{AFX_MSG(CSplitFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	BOOL ReplaceView(int row, int col,CAcyDial * pViewClass);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	 void SetParamView();
	 void SetVisionView();
	
public:
	 void OnViewMachine();
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	void HideAll(void);

public:
	 void OnViewAlarmes();
	 void SetCamView();
	 void SetDefautPisteView();	//V29
	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	//Detachement Fenêtre en cours
	bool DetachCurrentView();
	bool AttachCurrentView();

	void ExecuterFonctionExt(CString strFonction,void * BoutonParent);

	void SetDmxView();
	void SetChargementView();
	void SetValidLotView();
	void SetInitialisationView();
	void SetValidationView();
	void SetRobotView();
	void SetPesonView();
	void SetOPCView();
	void SetUeyeView();
	void SetPeseeView();
	void SetEtiqView();
	void SetMixupView();
	void SetFormulaireView();
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
};

/////////////////////////////////////////////////////////////////////////////
