#pragma once
#include "gridctrl.h"
#include "JeuParametres.h"
#include "staticmenu.h"

#include "AcyDial.h"//Scroll + Detachement

// Boîte de dialogue CDialParam

class CDialParam : public CAcyDial
{
	DECLARE_DYNAMIC(CDialParam)

public:
	CDialParam(CWnd* pParent = NULL,CJeuParametres* pJParamParent =NULL);   // constructeur standard
	virtual ~CDialParam();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALOGPARAM1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()

	
public:
	//Variables
	CJeuParametres * CurrentJParam;
	CGridCtrl GrilleParam;

	//Fonctions
	void OnBnClickedButEdit();
	void OnBnClickedRadGeneral();
	void OnBnClickedRadVision();
	void OnBnClickedRadCam1();
	void OnBnClickedRadCam2();
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	CStaticMenu Menu_Params;
	void SetDimGridToReg(CConfig * RegConfig, CGridCtrl * pGrid);
	void GetDimGridToReg(CConfig * RegConfig, CGridCtrl * pGrid);
	void OnGridColClick(NMHDR* Nmhdr,LRESULT* lRes);
	void  ExecuterFonctionExt(CString strFonction,void * BoutonParent);
};
#pragma once
