#pragma once
#include "resource.h"
#include "gridctrl.h" //#include "..\\..\\MFC GRID\\gridctrl.h"
#include "JeuParametres.h"
#include "afxcmn.h"

#include "AcyDial.h"//Scroll + Detachement
// Boîte de dialogue CDialogParam

#define COEF_DOUBLE	100

class CDialogParam : public CAcyDial
{
	DECLARE_DYNAMIC(CDialogParam)

public:
	CDialogParam(CWnd* pParent = NULL,CJeuParametres * parentParam= NULL);   // constructeur standard
	virtual ~CDialogParam();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALOGPARAM };

protected:
	CJeuParametres * ParentParam;
	CParametre* ParamsInit[NB_PARAM_MAX];
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	
	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl Grille;
	virtual BOOL OnInitDialog();
	void InitialiserGrille(void);
	afx_msg void OnDestroy();
	void OnSelChanged(NMHDR* Nmhdr,LRESULT* lRes);
	CString strLabelSelec;
	CSliderCtrl SliderValue;
	CString strValueMin;
	CString strValueMax;
	int iSlider;
	afx_msg void OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButSave();
	afx_msg void OnNMReleasedcaptureSlider(NMHDR *pNMHDR, LRESULT *pResult);
	int iValSlider;
public:
	afx_msg void OnBnClickedOk();
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedButPlus();
	afx_msg void OnBnClickedButMoins();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButEdit();
	afx_msg void OnBnClickedRadGeneral();
	afx_msg void OnBnClickedRadVision();
	void SaveInitValues(void);
	void DeleteInitValues(void);
};
