// MatroxApplication.h: interface for the CMatroxApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATROXAPPLICATION_H__6DE7F825_27B5_48BC_BAFF_15BEEF5341CB__INCLUDED_)
#define AFX_MATROXAPPLICATION_H__6DE7F825_27B5_48BC_BAFF_15BEEF5341CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4996)
#pragma warning(disable:4995)


class CMatroxApplication  
{
public:
	CMatroxApplication();
	virtual ~CMatroxApplication();
	BOOL LireFromBase(CADODatabase *daoBase, CString strMachine);
	BOOL Charger();
	BOOL Decharger();

	MIL_ID		milApp;
	CString		m_StrMachine;
	CString		m_StrApplication;
	CString		m_StrInitFlag;

	CString		sErrMsg;

//A TRIER 

public:
	int m_iTestLecture;
	bool GererFantome;
	bool GererDefaut;
	bool GererAvance;
	bool GererLanguette;
	bool GererLateral;
	bool GererBlocageArret;
	bool GererReglage;
	long ReglageIncrement;
	char m_sChaine[200];
	BOOL m_bPurge;

	void Initialiser();
	long m_lAVIYSize;
	long m_lAVIXSize;
	int m_iDernierGroupe;
	double m_dMoyenneInspection;
	void GererMarche(long lCleCoupe, char *sArticle, long lCleOF);
	void GererArret();
	int m_iNbGroupe;

	long	m_lCompteurType;	//0=SIMUL,	1=CAMERA,	2=INPUT
	long	m_lCompteurIndex;	//INUTIL,	N° CAMERA	N°INPUT

};

#endif // !defined(AFX_MATROXAPPLICATION_H__6DE7F825_27B5_48BC_BAFF_15BEEF5341CB__INCLUDED_)
