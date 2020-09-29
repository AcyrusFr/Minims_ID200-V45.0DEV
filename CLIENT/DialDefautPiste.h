#pragma once
#include "AcyDial.h"
#include "staticmenu.h"
#include "afxwin.h"
#include "staticpolice.h"

// Boîte de dialogue CDialDefautPiste

class CDialDefautPiste : public CAcyDial
{
	DECLARE_DYNAMIC(CDialDefautPiste)

public:
	CDialDefautPiste(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialDefautPiste();

// Données de boîte de dialogue
	enum { IDD = IDD_DIAL_DEFAUTPISTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	CStaticMenu Menu_Defaut;
	void ExecuterFonctionExt(CString strFonction,void * BoutonParent);
	void Calcul_ValScroll(void);
	void MAJ_Scrollbars(void);
	void UpdateScrollBarsPositions();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic Image;

	CMImage * imgParent;
	CRect ImgRect;

	int Hauteur;
	int Largeur;

	int iScrollH;
	int iScrollV;
	int iScrollHMax;
	int iScrollVMax;
	double dZoomFactorH;
	double dZoomFactorV;
	CRect WinRect;
	HWND WindowsView;
	bool bState_Gauche;
	bool bState_Droite;
	bool bState_Haut;
	bool bState_Bas;

	CScrollBar SB_Vert;
	CScrollBar SB_Horz;
	CStaticPolice Text_Piste[3];

	//V 44.8 petite resolution
	CStaticPolice Staticcadence_Vues;
	CStaticPolice Staticcadence_Pris;
	CStaticPolice Staticcadence_Ratio;

	CStaticPolice Textcadence_Vues[3];
	CStaticPolice Textcadence_Pris[3];
	CStaticPolice Textcadence_Ratio[3];
};
