#pragma once
#include "gridctrl.h"
#include "afxwin.h"
//#include "voyant.h"
#include "..\\COMMUN\VOYANT\Voyant.h"
#include "staticmenu.h"
#include "staticpolice.h"

//#include "ListBoxNotify.h"
//#include "CustomListBox.h"
#include "ACYListBox.h"

#include "AcyDial.h"

// Boîte de dialogue CDialMachine

class CDialMachine : public CAcyDial
{
	DECLARE_DYNAMIC(CDialMachine)

public:
	CDialMachine(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialMachine();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALMACHINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl GrilleEtats;
	virtual BOOL OnInitDialog();
	int MAJGrilleComptage(void);
	CVoyant voy_robot1;
	CVoyant voy_robot2;
	CVoyant voy_robot3;
	CVoyant voy_Opc;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//CStaticMenu m_Menu;
	CVoyant voy_peson1;
	CVoyant voy_peson2;
	CVoyant voy_peson3;
	CVoyant voy_peson4;
	CVoyant voy_peson5;
	CVoyant voy_peson6;

	CVoyant voy_DMX1;
	CVoyant voy_DMX2;
	CVoyant voy_DMX3;
	CVoyant voy_Cam1;
	CVoyant voy_camUV;

	CStaticMenu MenuDialMachine;

	LRESULT OnMAJCompteurs(WPARAM wParam, LPARAM lParam);
	LRESULT OnMAJEtat(WPARAM wParam, LPARAM lParam);
	LRESULT OnMAJPrevDialog(WPARAM wParam, LPARAM lParam);
	LRESULT OnMAJViewMachine(WPARAM wParam, LPARAM lParam);
	void InitialiserTextes(void);
	CStaticPolice TextDescription;
	CStaticPolice TextOf;
	CStaticPolice TextArticle;
	CStaticPolice TextLot;
	CStaticPolice static_description;
	CStaticPolice static_of;
	CStaticPolice static_article;
	CStaticPolice static_lot;
	CStaticPolice TextCadence;
	CStaticPolice static_cadence;

	//V 44.8 grande Resolution
	CStaticPolice static_Cadence_Vues;
	CStaticPolice static_Cadence_Pris;
	CStaticPolice static_Cadence_Ratio;
	CStaticPolice static_Cadence_Piste[3];
	CStaticPolice Textcadence_Vues[3];
	CStaticPolice Textcadence_Pris[3];
	CStaticPolice Textcadence_Ratio[3];

	void SetStateButProcess(int iState);
	CACYListBox ListDefMachine;
	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	CACYListBox Liste_Alertes;
	//V37 CVoyant voy_Cam3;
	//V37 CVoyant voy_Cam4;
	CVoyant voy_pesage;

	//V 44.0 CVoyant voy_CamBoite;			//V37
	CVoyant voy_ImpWolke;			//V37
	CVoyant voy_LectWolke;			//V37
	CVoyant voy_ImpScellement;		//V37
	CVoyant voy_LectScellement;		//V37
};
