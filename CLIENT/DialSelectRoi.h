#pragma once
#include "afxwin.h"
#include "resource.h"

#include "ImageButtonWithStyle.h"

#define ZOOM_FACTOR_MIN	0.125 // en vrai on peut descendre à 0.03
#define ZOOM_FACTOR_MAX	16
#define STEP_SCROLL_PIX	15
#define SIZE_CROIX_TRACAGE	20
#define COLOR_TRACAGE_1	RGB(255,0,0)
#define COLOR_TRACAGE_2	RGB(0,255,0)

#define NB_BOUTONS	11

// Boîte de dialogue CDialSelectRoi

class CDialSelectRoi : public CDialog
{
	DECLARE_DYNAMIC(CDialSelectRoi)

public:
	CDialSelectRoi(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialSelectRoi();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALSELECTROI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	bool bRoiDone;
	CMImage * imgParent;
	CMImage imgVisuSelectRoi;
	PARAM_ROI SelectedRoi;

	HWND WindowsView;
	CRect WinRect;
	int Hauteur;
	int Largeur;

	CStatic Image;
	CRect ImgRect;
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	bool bCapturing;
	afx_msg void OnBnClickedButreset();
	int GetClikZone(CPoint point);
	CString strInfos;

	int iScrollH;
	int iScrollV;
	int iScrollHMax;
	int iScrollVMax;
	double dZoomFactorH;
	double dZoomFactorV;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButhaut();
	afx_msg void OnBnClickedButbas();
	afx_msg void OnBnClickedButdroite();
	afx_msg void OnBnClickedButgauche();
	void TracerRoi();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButzoomplus();
	afx_msg void OnBnClickedButzoommoins();
	
	CImageButtonWithStyle but_Haut;
	CImageButtonWithStyle but_Bas;
	CImageButtonWithStyle but_Gauche;
	CImageButtonWithStyle but_Droite;
	CImageButtonWithStyle but_zoomplus;
	CImageButtonWithStyle but_zoommoins;
	HBITMAP m_h_bitmap[NB_BOUTONS];

	CImageButtonWithStyle butOk;
	CImageButtonWithStyle butCancel;
	afx_msg void OnBnClickedButstretch();
	CScrollBar SB_Horz;
	CScrollBar SB_Vert;

	void MAJ_Scrollbars();
	void UpdateScrollBarsPositions();
	CImageButtonWithStyle but_stretch;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButstretchvrt();
	afx_msg void OnBnClickedButstretchhrz();
	CImageButtonWithStyle but_StretchHrz;
	CImageButtonWithStyle but_StretchVrt;
};
