#pragma once

#include "AcyDial.h"//Scroll + Detachement
#include "staticmenu.h"
#include "afxwin.h"
#include "Vision.h"
#include "afxcmn.h"
#include "staticpolice.h"
#include "afxwinforms.h"
#include "ACYListBox.h"
#include "StaticVisuImg_V2.h"

#define ZOOM_FACTOR_MIN	0.125 // en vrai on peut descendre à 0.03
#define ZOOM_FACTOR_MAX	16

#define SIZE_CROIX_TRACAGE	20

#define COLOR_TRACAGE_1	RGB(255,0,0)
#define COLOR_TRACAGE_2	RGB(0,255,0)
#define COLOR_TRACAGE_3	RGB(0,0,255)

#define DEPLACEMENT_ORIGINE	5
#define DEPLACEMENT_ROI	5

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
	/*CStaticMenu Menu_ModifImg;

	CStatic Image;

	bool bCapturing;
	bool imgAffichee;*/
	CStaticVisuImg ImageVisu;

	CMImage * imgParent;
	CMImage imgVisuSelectRoi;

	int iIndexExtract;
	CExtraction * CurrentExtract;
	/*STRUCT_POINT CurrentPt;

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
	HWND WindowsView;*/

	//CString strInfos;
	//int iCellule;

	//bool bState_Gauche;
	//bool bState_Droite;
	//bool bState_Haut;
	//bool bState_Bas;

	////Boutons
	//void Roi_Reset(void);
	//void Roi_ZoomP(void);
	//void Roi_ZoomM(void);
	//Fin Boutons

	int GetClikZone(CPoint point);
	void SetImage(void);
	void RefreshImage(void);
	void SetImageCellule();
	
	/*afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void TracerPoint(void);
	void DrawCurrentPoint(void);*/
	/*void OnGridColClick(NMHDR* Nmhdr,LRESULT* lRes);
	void OnExtractChanged(NMHDR* Nmhdr,LRESULT* lRes);*/

	/*void Stretch(void);
	void MAJ_Scrollbars(void);
	void Calcul_ValScroll(void);
	void StretchVrt(void);
	void StretchHrz(void);
	void UpdateScrollBarsPositions();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	CScrollBar SB_Vert;
	CScrollBar SB_Horz;*/

	/*int iIndexVerticeClic;
	void GererClicDown(STRUCT_POINT CurrentPt);
	void GererClicUp(STRUCT_POINT CurrentPt);
	void GererRClicDown(STRUCT_POINT CurrentPt);
	void GererRClicUp(STRUCT_POINT CurrentPt);
	void GererClicMove(STRUCT_POINT CurrentPt);

	void SetScrollBarsToCentre(void);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);*/

	afx_msg void OnDestroy();
	int ExecuterFonctionExt(CString strFonction,void * BoutonParent);
	bool ChargerFormulaire(CString sFormat);

	int iExtract;

	CWinFormsControl<AcyCL::uCtlForm> form_Extract;
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	CACYListBox m_ListeResult;
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
