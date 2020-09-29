#pragma once

#include "BaseFenetre.h"
#include "gridctrl.h"
#include "staticmenu.h"

#include "AcyDial.h"//Scroll + Detachement

#define ID_TIMER_RECONNECT_UEYE1	1000
#define ID_TIMER_RECONNECT_UEYE2	1001

// Boîte de dialogue CDialCameras

class CDialCameras : public CAcyDial
{
	DECLARE_DYNAMIC(CDialCameras)

public:
	CDialCameras(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialCameras();

// Données de boîte de dialogue
	enum { IDD = IDD_DIALCAMERAS1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()

public:
	public :

		// uEye variables
	// camera variables
	INT		m_Ret;			            // return value of uEye SDK functions
	//int		m_CamCount;					// number of active cameras
	//bool	m_bErrRep;					// enable/disable the error report
	
    INT     m_nRenderMode;  // render  mode
   // INT     m_nFlipHor;			// horizontal flip flag
  //  INT     m_nFlipVert;		// vertical flip flag
	//bool	b_ueye;//indique si la camera est une uEye
	//INT m_nOverlaySizeX;
	//INT m_nOverlaySizeY;
	// Key color
  //  COLORREF m_rgbKeyColor;  

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	CGridCtrl Grille_ListeImages;
	CStaticMenu Menu_Camera;
	int MAJ_Grille();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnParamCamChanged(WPARAM Param1, LPARAM Param2);
	void OnSelImageChanged(NMHDR* Nmhdr,LRESULT* lRes);
	void OnSelImageChanging(NMHDR* Nmhdr,LRESULT* lRes);

public:
	bool OpenUeyeCam(int id_Cam,int iAdresseuEye = 0);
	int InitUeyeCam(int id_Cam,HWND hwnd,int iAdresseuEye);
	int StopGrabUeyeCam(int id_Cam);
	int InitDisplayMode(int id_Cam);
	LRESULT OnUeyeMessage(WPARAM wparam, LPARAM lParam);
	bool CamAllocMem(int id_Cam);//allocation buffer pour image camera
	void AcquisitionSoftTriggerCam(int id_Cam);
	int DoSoftwareTrigger(int id_cam);
	char GetMemoryPixel(int x, int y,int id_cam);
	int SetRoi(int id_Cam, int x, int y, int dx, int dy);

	bool SetRoi_AutoContrast(int id_cam,PARAM_ROI * Roi);
	bool Set_AutoContrast(int id_cam,bool bEnable = true);

	int iCurrentCam;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	void ExecuterFonctionExt(CString strFonction,void * BoutonParent);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedRadForcage2();
	afx_msg void OnBnClickedRadForcage1();
	afx_msg void OnBnClickedRadForcage3();
	afx_msg void OnBnClickedRadForcage4();
};
