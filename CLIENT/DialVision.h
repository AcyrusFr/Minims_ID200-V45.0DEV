#pragma once
#include "gridctrl.h"
#include "afxwin.h"
#include "staticmenu.h"
#include "ACYListBox.h"

#include "StaticPolice.h"
// Boîte de dialogue CDialVision
#include "AcyDial.h"

class CDialVision : public CAcyDial
{
	DECLARE_DYNAMIC(CDialVision)

public:
	CDialVision(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialVision();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALVISION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	int m_nCurHeight;
	int m_nScrollPos;
	CRect m_rect;

	CJeuParametres * pParam;

	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButsnap();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedImageTest();
	afx_msg void OnBnClickedButfreerun();
	afx_msg void OnBnClickedButimages();
	afx_msg void OnBnClickedButSyncon();
	afx_msg void OnBnClickedButEclairage();
	afx_msg void OnBnClickedButCompteur();
	afx_msg void OnBnClickedButRecharger();
	afx_msg void OnBnClickedRadPos();
	afx_msg void OnBnClickedRadPos2();
	afx_msg void OnBnClickedRadPos3();
	afx_msg void OnBnClickedRadPos4();
	CGridCtrl GrilleVision;
	int MAJGrilleParam(void);
	afx_msg void OnBnClickedButeditparam();
	afx_msg void OnBnClickedButsaveparam();
	CACYListBox m_ListVision;
	CACYListBox m_ListVisionTemps;
public:
	CStaticMenu Menu_Vision;
public:
	afx_msg void OnBnClickedRadPos5();
	void  ExecuterFonctionExt(CString strFonction,void * BoutonParent);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	CGridCtrl Grille_CatDefauts;
	void MAJ_GrilleCount();
	afx_msg void OnBnClickedButeditparam2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	CStaticPolice TextCadence;//V 43.2
};
