// DialVision.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialVision.h"
#include "DiagPadNum.h"
#include "DiagPadAlpha.h"

#include "splitfrm.h"

#define NB_LIGNES_GRILLE	15

// Boîte de dialogue CDialVision

IMPLEMENT_DYNAMIC(CDialVision, CAcyDial)

CDialVision::CDialVision(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialVision::IDD, pParent)
	//,Menu_Vision("Menu_Vision")
	, Menu_Vision(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Vision"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	JParamDetection.TypeTraca = JOURNAL_TYPE_JPARAM_RECETTEVISION;
	pParam = &JParamDetection ;
}

CDialVision::~CDialVision()
{
}

void CDialVision::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLEVISION, GrilleVision);
	DDX_Control(pDX, IDC_LIST_VISION, m_ListVision);
	DDX_Control(pDX, IDC_LIST_VISIONTEMPS, m_ListVisionTemps);
	DDX_Control(pDX, IDC_STATICMENUVISION, Menu_Vision);
	DDX_Control(pDX, IDC_GRILLEVISION2, Grille_CatDefauts);
	DDX_Control(pDX, IDC_TEXT_CADENCE, TextCadence);//V 43.2
}


BEGIN_MESSAGE_MAP(CDialVision, CAcyDial)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RAD_POS, &CDialVision::OnBnClickedRadPos)
	ON_BN_CLICKED(IDC_RAD_POS2, &CDialVision::OnBnClickedRadPos2)
	ON_BN_CLICKED(IDC_RAD_POS3, &CDialVision::OnBnClickedRadPos3)
	ON_BN_CLICKED(IDC_RAD_POS4, &CDialVision::OnBnClickedRadPos4)
	ON_BN_CLICKED(IDC_BUTEDITPARAM, &CDialVision::OnBnClickedButeditparam)
	ON_BN_CLICKED(IDC_BUTSAVEPARAM, &CDialVision::OnBnClickedButsaveparam)
	ON_BN_CLICKED(IDC_RAD_POS5, &CDialVision::OnBnClickedRadPos5)
	ON_BN_CLICKED(IDC_BUTEDITPARAM2, &CDialVision::OnBnClickedButeditparam2)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialVision

BOOL CDialVision::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	GetWindowRect(m_rect);
	m_nScrollPos = 0;

	//Initialisation de la grille
	GrilleVision.SetDefCellHeight(25);//hauteur des cellules
	GrilleVision.SetDefCellWidth(100);//largeur des cellules
	GrilleVision.SetColumnCount(4);
	GrilleVision.SetRowCount(NB_LIGNES_GRILLE+1);//+1 pour légende
	GrilleVision.SetFixedRowCount(1);//légende fixe
	GrilleVision.SetFixedColumnCount(1);//Premiére colonne fixe
	GrilleVision.ExpandColumnsToFit(FALSE);
	GrilleVision.SetListMode(TRUE);
	GrilleVision.SetSingleRowSelection(TRUE);
	GrilleVision.SetSingleColSelection(TRUE);
	GrilleVision.SetFixedColumnSelection(FALSE);
	GrilleVision.SetFixedRowSelection(FALSE);

	GrilleVision.SetFixedBkColor(RGB(0,46,184));
	GrilleVision.SetFixedTextColor(RGB(255,255,255));

	//légende
	GrilleVision.SetItemText(0,0,_T("Paramètre"));
	GrilleVision.SetItemText(0,1,_T("Etat/Valeur"));
	GrilleVision.SetItemText(0,2,_T("Valeur Min"));
	GrilleVision.SetItemText(0,3,_T("Valeur Max"));

	MAJGrilleParam();
	
	//V43-ALL
	Grille_CatDefauts.SetDefCellHeight(25);//hauteur des cellules
//	Grille_CatDefauts.SetDefCellWidth(100);//largeur des cellules
	Grille_CatDefauts.SetColumnCount(8);
	Grille_CatDefauts.SetRowCount(2);//+1 pour légende
	Grille_CatDefauts.SetFixedRowCount(1);//légende fixe
	Grille_CatDefauts.SetFixedColumnCount(0);
	Grille_CatDefauts.SetListMode(TRUE);
	Grille_CatDefauts.SetSingleRowSelection(TRUE);
	Grille_CatDefauts.SetSingleColSelection(TRUE);
	Grille_CatDefauts.SetFixedColumnSelection(FALSE);
	Grille_CatDefauts.SetFixedRowSelection(FALSE);

	Grille_CatDefauts.SetFixedBkColor(RGB(0, 46, 184));
	Grille_CatDefauts.SetFixedTextColor(RGB(255, 255, 255));

	Grille_CatDefauts.SetItemText(0, 0, _T("Surface"));
	Grille_CatDefauts.SetItemText(0, 1, _T("Inverse"));
	Grille_CatDefauts.SetItemText(0, 2, _T("Modeles"));
	Grille_CatDefauts.SetItemText(0, 3, _T("Dimensions"));
	Grille_CatDefauts.SetItemText(0, 4, _T("Angle Prise"));
	Grille_CatDefauts.SetItemText(0, 5, _T("Codes"));
	Grille_CatDefauts.SetItemText(0, 6, _T("Taches UV"));
	Grille_CatDefauts.SetItemText(0, 7, _T("Taches Couleur"));

	Grille_CatDefauts.ExpandColumnsToFit(TRUE);
	Grille_CatDefauts.Refresh();

	//Creation du menu
	Menu_Vision.bVertical = true;
	Menu_Vision.strProfil = theApp.CurrentUser.strProfil;
	Menu_Vision.SendMessage(WM_CREATE,NULL,NULL);

	//V 43.2
	TextCadence.strText = _T("0");
	TextCadence.CouleurText = RGB(0, 0, 255);
	TextCadence.Invalidate();


	CheckRadioButton(IDC_RAD_POS,IDC_RAD_POS5,IDC_RAD_POS5);//position Vision par défaut

	//m_ListVision.SendMessage(ACY_LB_INIT,0,0);
	//m_ListVisionTemps.SendMessage(ACY_LB_INIT,0,0);

	CFont NewFont;
	NewFont.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight //FW_SEMIBOLD
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename

	m_ListVision.Initialise(&NewFont, 1000);
	m_ListVisionTemps.Initialise(&NewFont, 1000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialVision::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
int nDelta;
	int nMaxPos = m_rect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/100,nMaxPos-m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/100,m_nScrollPos);
		break;

         case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nScrollPos);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nScrollPos);
		break;
	
         default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
	ScrollWindow(0,-nDelta);

	CAcyDial::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDialVision::OnSize(UINT nType, int cx, int cy)
{
		CAcyDial::OnSize(nType, cx, cy);

	//  Add your message handler code here.
	m_nCurHeight = cy;
	int nScrollMax;
	if (cy < m_rect.Height())
	{
	     nScrollMax = m_rect.Height() - cy;
	}
	else
	     nScrollMax = 0;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nMin = 0;
	si.nMax = nScrollMax;
	si.nPage = si.nMax/10;
	si.nPos = 0;

    SetScrollInfo(SB_VERT, &si, TRUE);
}

void CDialVision::OnBnClickedButsnap()
{
	//Bouton SNAP
	theCam.iModeAcquisition = 1;
	theCam.iSnap = 1;

	//CAM2
	theCamUV.iModeAcquisition = 1;
	theCamUV.iSnap = 1;
}

void CDialVision::OnBnClickedButtonTest()
{
	theCam.bSave = !theCam.bSave;

	//CAM2 
	theCamUV.bSave = !theCamUV.bSave;

	if(theCam.bSave)	theApp.SetStringListVision("SAVE ON");
	else				theApp.SetStringListVision("SAVE OFF");
}

void CDialVision::OnBnClickedImageTest()
{
	//TODO CAM2

	//Chargement fichier de simulation
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for filename
	CString sFile = _T("");

	// Initialize OPENFILENAME
	strcpy(szFile,"");
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner =  0 ;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	//ofn.lpstrFilter = "Fichiers Images MIL (*.mim)\0*.mim\0Fichiers Images (*.bmp)\0*.bmp\0Tous les Fichier (*.*)\0*.*\0\0";
	ofn.lpstrFilter = "Fichiers Images (*.bmp)\0*.bmp\0Fichiers Images MIL (*.mim)\0*.mim\0Tous les Fichier (*.*)\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST |OFN_HIDEREADONLY ;
	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn)==TRUE)
	{
		sFile = szFile;

		MIL_TEXT_PTR mFile;
		long lX = 0;
		long lY = 0;

		mFile = sFile.GetBuffer(sFile.GetLength());
		lX = (long)MbufDiskInquire(mFile,M_SIZE_X,M_NULL);
		lY = (long)MbufDiskInquire(mFile,M_SIZE_Y,M_NULL);

		if(pParamVisionSelection->value == 0)
		{
			MbufClear(theCam.Simulation_Image.milBuf,0x00);
			PARAM_ROI RoiTemp;
			RoiTemp.dX = lX;
			RoiTemp.dY = lY;

			//V29 Piste 0
			RoiTemp .X1 = (theCam.Piste[0].XMin + theCam.Piste[0].XMax)/2 - RoiTemp.dX/2;
			RoiTemp .Y1 = 0;
			if(!theCam.Simulation_RoiPiste.SetROI(&RoiTemp))
			{
				return;
			}
			MbufLoad(mFile,theCam.Simulation_RoiPiste.milBuf);
			
			//V29 Piste 1
			RoiTemp .X1 = (theCam.Piste[1].XMin + theCam.Piste[1].XMax)/2 - RoiTemp.dX/2;
			RoiTemp .Y1 = 0;
			if(!theCam.Simulation_RoiPiste.SetROI(&RoiTemp))
			{
				return;
			}
			MbufLoad(mFile,theCam.Simulation_RoiPiste.milBuf);
			
			//V29 Piste 2
			RoiTemp .X1 = (theCam.Piste[2].XMin + theCam.Piste[2].XMax)/2 - RoiTemp.dX/2;
			RoiTemp .Y1 = 0;
			if(!theCam.Simulation_RoiPiste.SetROI(&RoiTemp))
			{
				return;
			}
			MbufLoad(mFile,theCam.Simulation_RoiPiste.milBuf);
			
			theCam.Simulation_IndexLoad = 0;
			theCam.Simulation_NbRoi = 10;
			theCam.Simulation_Enable=true;
		}
		else
		{
			MbufLoad(mFile,theCamUV.imgGrab[0].milBuf);
			MbufLoad(mFile,theCamUV.imgGrab[1].milBuf);
		}
	}
	else
	{
		theCam.Simulation_Enable=false;
	}

}

void CDialVision::OnBnClickedButfreerun()
{
	// FREERUN
	theCam.iModeAcquisition = 0;
	theCam.iSnap = 1;

	//CAM2
	theCamUV.iModeAcquisition = 0;
	theCamUV.iSnap = 1;
}

void CDialVision::OnBnClickedButimages()
{
	if(theCam.IsFenetreOpened)
	{
		theCam.imgScroll.FenetreFermer();
		theCam.imgTrack.FenetreFermer();
		theCam.imgTrackTache.FenetreFermer();
		theCam.imgTrackCouleur.FenetreFermer();
		theCam.imgTrackZoom.FenetreFermer();
		theCam.imgRotationZoom.FenetreFermer();
		theCam.imgGrabVisu.FenetreFermer();
		theCam.imgSachetVertical.FenetreFermer();
		//V29 theCam.imgResultat.FenetreFermer();
		theCam.IsFenetreOpened = false;
	}
	else
	{
		theCam.imgScroll.FenetreOuvrir();
		theCam.imgTrack.FenetreOuvrir();
		theCam.imgTrackTache.FenetreOuvrir();
		theCam.imgTrackCouleur.FenetreOuvrir();
		theCam.imgTrackZoom.FenetreOuvrir();
		theCam.imgRotationZoom.FenetreOuvrir();
		theCam.imgGrabVisu.FenetreOuvrir();
		theCam.imgSachetVertical.FenetreOuvrir();
		//V29 theCam.imgResultat.FenetreOuvrir();
		theCam.IsFenetreOpened = true;
	}

	//CAM2
	if(theCamUV.IsFenetreOpened)
	{
		theCamUV.imgSrc1.FenetreFermer();	//V2.30
		theCamUV.imgSrc2.FenetreFermer();	//V2.30
		theCamUV.imgDiff.FenetreFermer();	//V2.30
		theCamUV.imgSrcPlus.FenetreFermer();	//V2.51
		theCamUV.imgBin.FenetreFermer();
		theCamUV.imgGrabVisu.FenetreFermer();
		theCamUV.IsFenetreOpened = false;
	}
	else
	{
		theCamUV.imgSrc1.FenetreOuvrir();	//V2.30
		theCamUV.imgSrc2.FenetreOuvrir();	//V2.30
		theCamUV.imgDiff.FenetreOuvrir();	//V2.30
		theCamUV.imgSrcPlus.FenetreOuvrir();	//V2.51
		theCamUV.imgBin.FenetreOuvrir();
		theCamUV.imgGrabVisu.FenetreOuvrir();
		theCamUV.IsFenetreOpened = true;
	}
}

void CDialVision::OnBnClickedButSyncon()
{
	//TODO CAM2
	theCam.CycleTestIO = (theCam.CycleTestIO+1)%5;
	switch(theCam.CycleTestIO)
	{
	case 0:
		for (int i = 0; i < 3; i++)
		{
			MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[i] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		}

		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT2 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);	//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT3 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);	//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT4 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);	//#12
		theApp.SetStringListVision("OFF OFF OFF");
		break;
	case 1:
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT2 + M_DIG_DISPATCH_IMMEDIATE,M_ON);	//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT3 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT4 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);//#12
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[0] + M_DIG_DISPATCH_IMMEDIATE,M_ON);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[1] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[2] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		theApp.SetStringListVision("ON 0 0");
		break;
	case 2:
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT2 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT3 + M_DIG_DISPATCH_IMMEDIATE,M_ON);	//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT4 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);//#12
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[0] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[1] + M_DIG_DISPATCH_IMMEDIATE,M_ON);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[2] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		theApp.SetStringListVision("0 ON 0");
		break;
	case 3:
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT2 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT3 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT4 + M_DIG_DISPATCH_IMMEDIATE,M_ON);	//#12
		
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[0] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[1] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[2] + M_DIG_DISPATCH_IMMEDIATE,M_ON);

		theApp.SetStringListVision("0 0 ON");
		break;
	case 4:
		for (int i = 0; i < 3; i++)
		{
			MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[i] + M_DIG_DISPATCH_IMMEDIATE,M_ON);
		}

		/*MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT2 + M_DIG_DISPATCH_IMMEDIATE,M_ON);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT3 + M_DIG_DISPATCH_IMMEDIATE,M_ON);
		MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT4 + M_DIG_DISPATCH_IMMEDIATE,M_ON);*/
		theApp.SetStringListVision("ON ON ON");
		break;
	}
}

void CDialVision::OnBnClickedButEclairage()
{
	
	if(*theApp.Process.pBitEclairageSup == TRUE)
	{
		*theApp.Process.pBitEclairageSup = FALSE;
		*theApp.Process.pBitEclairageUV = FALSE;
		theApp.SetStringListVision("Eclairage OFF");
	}
	else
	{
		*theApp.Process.pBitEclairageSup = TRUE;
		*theApp.Process.pBitEclairageUV = TRUE;
		theApp.SetStringListVision("Eclairage ON");
	}
	
}

void CDialVision::OnBnClickedButCompteur()
{
	//TODO CAM2

	MIL_INT Rep;
	MIL_INT Rep2;
	//MIL_LONG Rep2;
	MdigInquire(theCam.milDig,M_ROTARY_ENCODER_POSITION_TRIGGER,&Rep);
	MdigInquire(theCam.milDig,M_ROTARY_ENCODER_POSITION,&Rep2);
	TRACE("Position = %ld   Trigger = %ld\n",Rep2,Rep);
}

void CDialVision::OnBnClickedButRecharger()
{
	//V17 : rechargemnt des parametres
	m_RecetteVision.bChargerVision = true;
}

void CDialVision::OnBnClickedRadPos()
{
	theApp.m_Robot1.bPosSimul = true;
	theApp.m_Robot2.bPosSimul = true;
	theApp.m_Robot3.bPosSimul = true;

	theApp.m_Robot1.PosStatutSimul = ROBOT_POSITION_OK;
	theApp.m_Robot2.PosStatutSimul = ROBOT_POSITION_OK;
	theApp.m_Robot3.PosStatutSimul = ROBOT_POSITION_OK;
}

void CDialVision::OnBnClickedRadPos2()
{
	theApp.m_Robot1.bPosSimul = true;
	theApp.m_Robot2.bPosSimul = true;
	theApp.m_Robot3.bPosSimul = true;

	theApp.m_Robot1.PosStatutSimul = ROBOT_POSITION_REBUT;
	theApp.m_Robot2.PosStatutSimul = ROBOT_POSITION_REBUT;
	theApp.m_Robot3.PosStatutSimul = ROBOT_POSITION_REBUT;
}

void CDialVision::OnBnClickedRadPos3()
{
	theApp.m_Robot1.bPosSimul = true;
	theApp.m_Robot2.bPosSimul = true;
	theApp.m_Robot3.bPosSimul = true;

	theApp.m_Robot1.PosStatutSimul = ROBOT_POSITION_MELANGE;
	theApp.m_Robot2.PosStatutSimul = ROBOT_POSITION_MELANGE;
	theApp.m_Robot3.PosStatutSimul = ROBOT_POSITION_MELANGE;
}

void CDialVision::OnBnClickedRadPos4()
{
	theApp.m_Robot1.bPosSimul = true;
	theApp.m_Robot2.bPosSimul = true;
	theApp.m_Robot3.bPosSimul = true;

	theApp.m_Robot1.PosStatutSimul = ROBOT_POSITION_SACHETVIDE;
	theApp.m_Robot2.PosStatutSimul = ROBOT_POSITION_SACHETVIDE;
	theApp.m_Robot3.PosStatutSimul = ROBOT_POSITION_SACHETVIDE;
}
void CDialVision::OnBnClickedRadPos5()
{
	//on arrete la simulation du statut position
	theApp.m_Robot1.bPosSimul = false;
	theApp.m_Robot2.bPosSimul = false;
	theApp.m_Robot3.bPosSimul = false;
}


int CDialVision::MAJGrilleParam(void)
{
	//Affichage dans la grille

	CString strTemp;

	GrilleVision.SetRowCount(pParam->GetNumberOfParams()+1);

	for(int i = 1; i < pParam->GetNumberOfParams()+1;i++)
	{
		CGridCellBase* cellule;
		cellule = GrilleVision.GetCell(i,0);//colonne 0 Label
		cellule->SetText(pParam->Parametres[i-1]->label);

		switch(pParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			cellule = GrilleVision.GetCell(i,1);//colonne 1 Valeur
			strTemp.Format(_T("%1.2f"),((CParametreDouble*)(pParam->Parametres[i-1]))->value);
			cellule->SetText(strTemp);

			cellule = GrilleVision.GetCell(i,2);//colonne 2 Valeur Min
			strTemp.Format(_T("%1.2f"),((CParametreDouble*)(pParam->Parametres[i-1]))->value_min);
			cellule->SetText(strTemp);

			cellule = GrilleVision.GetCell(i,3);//colonne 3 Valeur Max
			strTemp.Format(_T("%1.2f"),((CParametreDouble*)(pParam->Parametres[i-1]))->value_max);
			cellule->SetText(strTemp);
			break;

		case TYPESTRING:
			cellule = GrilleVision.GetCell(i,1);//colonne 1 Valeur
			strTemp = ((CParametreString*)(pParam->Parametres[i-1]))->value;
			cellule->SetText(strTemp);

			cellule = GrilleVision.GetCell(i,2);//colonne 2 Valeur Min
			strTemp = _T("");
			cellule->SetText(strTemp);

			cellule = GrilleVision.GetCell(i,3);//colonne 3 Valeur Max
			strTemp =  _T("");
			cellule->SetText(strTemp);
			break;

		case TYPELONG:
			cellule = GrilleVision.GetCell(i,1);//colonne 1 Valeur
			strTemp.Format(_T("%ld"),((CParametreLong*)(pParam->Parametres[i-1]))->value);
			cellule->SetText(strTemp);

			cellule = GrilleVision.GetCell(i,2);//colonne 2 Valeur Min
			strTemp.Format(_T("%ld"),((CParametreLong*)(pParam->Parametres[i-1]))->value_min);
			cellule->SetText(strTemp);

			cellule = GrilleVision.GetCell(i,3);//colonne 3 Valeur Max
			strTemp.Format(_T("%ld"),((CParametreLong*)(pParam->Parametres[i-1]))->value_max);
			cellule->SetText(strTemp);
			break;

		default:
			break;
		}
		
		

	}

	GrilleVision.Refresh();


	return 0;
}

void CDialVision::OnBnClickedButeditparam()
{
	// Bouton Editer

	CCellRange Selected = GrilleVision.GetSelectedCellRange();
	if((Selected.GetMinRow() >0)&&(Selected.GetMinRow() == Selected.GetMaxRow()))
	{
		int i = Selected.GetMinRow();//numero de ligne
		CGridCellBase* cellule;
		CString strTemp;
		CString strTempValue;

		CDiagPadNum frame;
		CDiagPadAlpha frameAlpha;

		double dVal_Temp = 0;
		long lVal_Temp = 0;

		double dVal_Prev = 0;
		long lVal_Prev = 0;
		CString str_Prev = _T("");

		bool bSaveParam = true;

		UpdateData(TRUE);
		
		//Ouverture Fenêtre de saisie
		switch(pParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			dVal_Prev = ((CParametreDouble*)pParam->Parametres[i-1])->value;
			
			frame.bPassword = false;
#ifdef _UNICODE
			frame.strInfos.Format(L"Entrer Valeur Paramétre Double : %s\nValeur Min: %2.3f\nValeur Max: %2.3f",pParam->Parametres[i-1]->label, ((CParametreDouble*)pParam->Parametres[i - 1])->value_min, ((CParametreDouble*)pParam->Parametres[i - 1])->value_max);
#else
			frame.strInfos.Format("Entrer Valeur Paramétre Double : %s\nValeur Min: %2.3f\nValeur Max: %2.3f",pParam->Parametres[i-1]->label, ((CParametreDouble*)pParam->Parametres[i - 1])->value_min, ((CParametreDouble*)pParam->Parametres[i - 1])->value_max);
#endif
			
			if(frame.DoModal()!= IDOK)
			{
				bSaveParam = false;
				break;
			}
			else
			{
				strTempValue = frame.strSaisie;
			}

			cellule = GrilleVision.GetCell(i,1);//colonne 1 Valeur
			
#ifdef _UNICODE
			dVal_min = _wtof(strTemp);
#else
			dVal_Temp = atof(strTempValue);
#endif
			if((((CParametreDouble*)pParam->Parametres[i-1])->value_min <= dVal_Temp )&&
				(((CParametreDouble*)pParam->Parametres[i-1])->value_max >= dVal_Temp ))
			{
				strTemp.Format(_T("%1.2f"),dVal_Temp);
				cellule->SetText(strTemp);
				((CParametreDouble*)pParam->Parametres[i-1])->value = dVal_Temp ;
			}
			else
			{
				AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_HORSLIMITES),MB_ICONWARNING);
			}
			break;

		case TYPESTRING:
			str_Prev = ((CParametreString*)pParam->Parametres[i-1])->value;
			frameAlpha.bPassword = false;
#ifdef _UNICODE
			frameAlpha.strInfos.Format(L"Entrer Valeur Paramétre String : %s",pParam->Parametres[i-1]->label);
#else
			frameAlpha.strInfos.Format("Entrer Valeur Paramétre String : %s",pParam->Parametres[i-1]->label);
#endif
			
			if(frameAlpha.DoModal()!= IDOK)
			{
				bSaveParam = false;
				break;
			}
			else
			{
				strTempValue = frameAlpha.strSaisie;
			}

			GrilleVision.SetItemText(i,1,strTempValue);//colonne 1 Valeur
			((CParametreString*)pParam->Parametres[i-1])->value = strTempValue;
			
			break;

		case TYPELONG:
			lVal_Prev = ((CParametreLong*)pParam->Parametres[i-1])->value;
			frame.bPassword = false;
#ifdef _UNICODE
			frame.strInfos.Format(L"Entrer Valeur Paramétre Long : %s\nValeur Min: %ld\nValeur Max: %ld",pParam->Parametres[i-1]->label,((CParametreLong*)pParam->Parametres[i-1])->value_min,((CParametreLong*)pParam->Parametres[i-1])->value_max);
#else
			frame.strInfos.Format("Entrer Valeur Paramétre Long : %s\nValeur Min: %ld\nValeur Max: %ld",pParam->Parametres[i-1]->label,((CParametreLong*)pParam->Parametres[i-1])->value_min,((CParametreLong*)pParam->Parametres[i-1])->value_max);
#endif
			
			if(frame.DoModal()!= IDOK)
			{
				bSaveParam = false;
				break;
			}
			else
			{
				strTempValue = frame.strSaisie;
			}

			cellule = GrilleVision.GetCell(i,1);//colonne 1 Valeur
			
#ifdef _UNICODE
			lVal_Temp =(long) _wtof(strTempValue);
#else
			lVal_Temp =(long) atof(strTempValue);
#endif
			if((((CParametreLong*)pParam->Parametres[i-1])->value_min <= lVal_Temp )&&
				(((CParametreLong*)pParam->Parametres[i-1])->value_max >= lVal_Temp ))
			{
				strTemp.Format(_T("%ld"),lVal_Temp);
				cellule->SetText(strTemp);
				((CParametreLong*)pParam->Parametres[i-1])->value = lVal_Temp ;
			}
			else
			{
				AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_HORSLIMITES),MB_ICONWARNING);
			}
			break;

		default:
			break;
		}
		
		UpdateData(FALSE);
		GrilleVision.Refresh();
		
		if(bSaveParam ) 
		{
			pParam->SauvegarderParametres();

			m_RecetteVision.Sauvegarder();

			//Traca Evenement Modif valeur Paramétre
			CString strLibelle,strCom;

			strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_MODIF_PARAM_F),pParam->Parametres[i-1]->label);
			
			switch(pParam->Parametres[i-1]->type)
			{
				case TYPEDOUBLE:
					strCom.Format(_T("Avant: %2.3f | Après: %2.3f"),dVal_Prev,((CParametreDouble*)pParam->Parametres[i-1])->value);
					break;
				case TYPESTRING:
					strCom.Format(_T("Avant: %s | Après: %s"),str_Prev,((CParametreString*)pParam->Parametres[i-1])->value);
					break;
				case TYPELONG:
					strCom.Format(_T("Avant: %ld | Après: %ld"),lVal_Prev,((CParametreLong*)pParam->Parametres[i-1])->value);
					break;

				default:
					break;
			}
			
			theApp.theJournal.Evenement(pParam->TypeTraca,strLibelle,strCom,theApp.CurrentUser.strCode);
		}
	}
}

void CDialVision::OnBnClickedButsaveparam()
{
	//Sauvegarde de la recette Vision
	m_RecetteVision.Sauvegarder();

}

BOOL CDialVision::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// Interception de la touche ENTREE et ECHAP GCA 1.14

	CWnd * pWnd = GetFocus();
	switch(wParam)
	{
	case IDOK:
		if(pWnd!=GetDlgItem(IDOK))
			return FALSE;
		break;
	case IDCANCEL:
		if(pWnd!=GetDlgItem(IDCANCEL))
			return FALSE;
		break;
	}

	return CAcyDial::OnCommand(wParam, lParam);
}
void  CDialVision::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	if(strFonction == "VISION_SNAP")
	{
		OnBnClickedButsnap();
	}
	else if(strFonction == "VISION_FREERUN")
	{
		OnBnClickedButfreerun();
	}
	else if(strFonction == "VISION_TEST")
	{
		OnBnClickedButtonTest();
	}
	else if(strFonction == "VISION_TEST2")
	{
		OnBnClickedImageTest();
	}
	else if(strFonction == "VISION_IMAGES")
	{
		OnBnClickedButimages();
	}
	else if(strFonction == "VISION_SYNCON")
	{
		OnBnClickedButSyncon();
	}
	else if(strFonction == "VISION_ECLAIRAGE")
	{
		OnBnClickedButEclairage();
	}
	else if(strFonction == "VISION_COMPTEUR")
	{
		OnBnClickedButCompteur();
	}
	else if(strFonction == "VISION_RECHARGER")
	{
		OnBnClickedButRecharger();
	}
	//V 43-ALL
	else if (strFonction == "VISION_HISTO_TAPIS")
	{
		Vision_HistoriqueTapis_Afficher();
	}
	//V 43-ALL
	else if (strFonction == "VISION_HISTO_TAPIS_EXPORT")
	{
		Vision_HistoriqueTapis_Exporter();
	}
	
	
}

void CDialVision::OnBnClickedButeditparam2()
{
	//RAZ des compteurs stats
	m_RecetteVision.bResetCountDefVision = true;
}
void CDialVision::MAJ_GrilleCount()
{
	CString strAffichage = _T("");
	
	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		,theCam.Piste[0].lCountDefVision_Surface
		, theCam.Piste[1].lCountDefVision_Surface
		, theCam.Piste[2].lCountDefVision_Surface);
	Grille_CatDefauts.SetItemText(1, 0, strAffichage);

	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		, theCam.Piste[0].lCountDefVision_Inverse
		, theCam.Piste[1].lCountDefVision_Inverse
		, theCam.Piste[2].lCountDefVision_Inverse);
	Grille_CatDefauts.SetItemText(1, 1, strAffichage);

	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		, theCam.Piste[0].lCountDefVision_Modele
		, theCam.Piste[1].lCountDefVision_Modele
		, theCam.Piste[2].lCountDefVision_Modele);
	Grille_CatDefauts.SetItemText(1, 2, strAffichage);

	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		, theCam.Piste[0].lCountDefVision_Dimensions
		, theCam.Piste[1].lCountDefVision_Dimensions
		, theCam.Piste[2].lCountDefVision_Dimensions);
	Grille_CatDefauts.SetItemText(1, 3, strAffichage);

	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		, theCam.Piste[0].lCountDefVision_AnglePrise
		, theCam.Piste[1].lCountDefVision_AnglePrise
		, theCam.Piste[2].lCountDefVision_AnglePrise);
	Grille_CatDefauts.SetItemText(1, 4, strAffichage);

	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		, theCam.Piste[0].lCountDefVision_Codes
		, theCam.Piste[1].lCountDefVision_Codes
		, theCam.Piste[2].lCountDefVision_Codes);
	Grille_CatDefauts.SetItemText(1, 5, strAffichage);

	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		, theCam.Piste[0].lCountDefVision_TachesUV
		, theCam.Piste[1].lCountDefVision_TachesUV
		, theCam.Piste[2].lCountDefVision_TachesUV);
	Grille_CatDefauts.SetItemText(1, 6, strAffichage);
	
	strAffichage.Format(_T("%02ld|%02ld|%02ld")
		, theCam.Piste[0].lCountDefVision_TachesCouleur
		, theCam.Piste[1].lCountDefVision_TachesCouleur
		, theCam.Piste[2].lCountDefVision_TachesCouleur);
	Grille_CatDefauts.SetItemText(1, 7, strAffichage);

	Grille_CatDefauts.Refresh();

	//V 43.2 Affichage de la cadence machine
	TextCadence.strText.Format("%2.2f", float(theApp.Process.dCadence));
	TextCadence.Invalidate();

}

void CDialVision::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_AFFICHAGE)
	{
		if (IsWindowVisible())
		{
			MAJ_GrilleCount();
		}
	}

	CAcyDial::OnTimer(nIDEvent);
}


void CDialVision::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CAcyDial::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		SetTimer(ID_TIMER_AFFICHAGE, 1000,NULL);
	}
	else
	{
		KillTimer(ID_TIMER_AFFICHAGE);
	}
}
