#pragma once
#include "resource.h"
#include "Camera.h"

// Bo�te de dialogue CForm_camsim

class CForm_camsim : public CDialog
{
	DECLARE_DYNAMIC(CForm_camsim)

public:
	CForm_camsim(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CForm_camsim();

// Donn�es de bo�te de dialogue
	enum { IDD = IDD_FORMCAMSIM };
public:
	CCamera* Cam;
	int iIdCam;
	LRESULT AfficherRep(WPARAM Param1, LPARAM Param2);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
};
