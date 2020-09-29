#pragma once

#pragma warning(disable:4995)

#include "Recette.h"
#include "afxwin.h"
#include "staticmenu.h"
#include "gridctrl.h"
//#include "ListBoxNotify.h"
#include "ACYListBox.h"

#include "AcyDial.h"
// Bo�te de dialogue CDialChargement

class CDialChargement : public CAcyDial
{
	DECLARE_DYNAMIC(CDialChargement)

public:
	CDialChargement(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialChargement();

// Donn�es de bo�te de dialogue
	enum { IDD = IDD_DIALCHARGEMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CRecette * Recette;
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButListe();
	int MAJRecetteWindows(void);
//	CListBoxNotify m_ListEvents;
	CACYListBox m_ListEvents;
	CStaticMenu Menu_Chargement;
	int UnlockOFLoad(void);
	//bool VerifCSVFiles(void);	//V2.51 : Suppression de la proc�dure
	//int Bouton_RecetteNew(void);	//V2.50 : Suppression de la proc�dure
	//void EditerNewRecetteFormulaire(void);	//V2.50 : Suppression de la proc�dure
	bool CSV_Charger();
	bool OF_Charger(CString sNumOF);
	void OnChargementCodeBarre(void);
	CGridCtrl GrilleRecetteCharg;
	LOGFONT myLogFont; //Police grille
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	bool CSVListeCodesSachets_Charger();	//V36
	CString CSVListeCodesSachets_Erreur;	//V36

	bool ITEM_MET_Copier();				//V37
	CString ITEM_MET_Copier_Erreur;		//V37
	int ITEM_MET_CodeBoite_Extract(CString sItemNumber);	//V37
	CString ITEM_MET_CodeBoite_Erreur;	//V37

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
