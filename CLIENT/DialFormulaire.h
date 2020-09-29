#pragma once
#include "afxwin.h"
#include "AcyDial.h"//Scroll + Detachement

//DT
using namespace AcyCL;
//DT

// Bo�te de dialogue CDialFormulaire

class CDialFormulaire : public CAcyDial
{
	DECLARE_DYNAMIC(CDialFormulaire)

public:
	CDialFormulaire(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialFormulaire();
	virtual BOOL OnInitDialog();
//	LRESULT OnMAJViewUTILISATEUR(WPARAM wParam, LPARAM lParam);

// Donn�es de bo�te de dialogue
	enum { IDD = IDD_DIALUTILISATEUR };


	//DT
	CWinFormsControl<AcyCL::uCtlForm> m_ctrlFormulaire;
	//DT


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
