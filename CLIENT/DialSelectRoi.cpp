// DialSelectRoi.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialSelectRoi.h"
#include "Vision.h"

// Boîte de dialogue CDialSelectRoi

IMPLEMENT_DYNAMIC(CDialSelectRoi, CDialog)

CDialSelectRoi::CDialSelectRoi(CWnd* pParent /*=NULL*/)
	: CDialog(CDialSelectRoi::IDD, pParent)
	, strInfos(_T(""))
{
	SelectedRoi.X1 = 0;
	SelectedRoi.Y1 = 0;
	SelectedRoi.X2 = 0;
	SelectedRoi.Y2 = 0;
	SelectedRoi.dX = 0;
	SelectedRoi.dY = 0;

	bCapturing = false;
	bRoiDone = false;

	iScrollH = 0;
	iScrollV = 0;
	iScrollHMax = 0;
	iScrollVMax = 0;

	dZoomFactorH = 1;
	dZoomFactorV = 1;

	WindowsView = NULL;
	Hauteur = 0;
	Largeur = 0;
}

CDialSelectRoi::~CDialSelectRoi()
{
}

void CDialSelectRoi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, Image);
	DDX_Text(pDX, IDC_INFOS_ROI, strInfos);
	DDX_Control(pDX, IDC_BUTHAUT, but_Haut);
	DDX_Control(pDX, IDC_BUTBAS, but_Bas);
	DDX_Control(pDX, IDC_BUTGAUCHE, but_Gauche);
	DDX_Control(pDX, IDC_BUTDROITE, but_Droite);
	DDX_Control(pDX, IDC_BUTZOOMPLUS, but_zoomplus);
	DDX_Control(pDX, IDC_BUTZOOMMOINS, but_zoommoins);
	DDX_Control(pDX, IDOK, butOk);
	DDX_Control(pDX, IDCANCEL, butCancel);
	DDX_Control(pDX, IDC_SCROLLHORZ, SB_Horz);
	DDX_Control(pDX, IDC_SCROLLVERT, SB_Vert);
	DDX_Control(pDX, IDC_BUTSTRETCH, but_stretch);
	DDX_Control(pDX, IDC_BUTSTRETCHHRZ, but_StretchHrz);
	DDX_Control(pDX, IDC_BUTSTRETCHVRT, but_StretchVrt);
}


BEGIN_MESSAGE_MAP(CDialSelectRoi, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTRESET, &CDialSelectRoi::OnBnClickedButreset)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTHAUT, &CDialSelectRoi::OnBnClickedButhaut)
	ON_BN_CLICKED(IDC_BUTBAS, &CDialSelectRoi::OnBnClickedButbas)
	ON_BN_CLICKED(IDC_BUTDROITE, &CDialSelectRoi::OnBnClickedButdroite)
	ON_BN_CLICKED(IDC_BUTGAUCHE, &CDialSelectRoi::OnBnClickedButgauche)
	ON_BN_CLICKED(IDOK, &CDialSelectRoi::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTZOOMPLUS, &CDialSelectRoi::OnBnClickedButzoomplus)
	ON_BN_CLICKED(IDC_BUTZOOMMOINS, &CDialSelectRoi::OnBnClickedButzoommoins)
	ON_BN_CLICKED(IDC_BUTSTRETCH, &CDialSelectRoi::OnBnClickedButstretch)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTSTRETCHVRT, &CDialSelectRoi::OnBnClickedButstretchvrt)
	ON_BN_CLICKED(IDC_BUTSTRETCHHRZ, &CDialSelectRoi::OnBnClickedButstretchhrz)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialSelectRoi

BOOL CDialSelectRoi::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Selection ROI: ") + imgParent->sLibelle);

	//affichage de l'image

	imgVisuSelectRoi.sLibelle = _T("Image Visu Selection ROI");
	imgVisuSelectRoi.Charger(imgParent->milSys,imgParent->Roi.dX,imgParent->Roi.dY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	MbufCopy(imgParent->milBuf,imgVisuSelectRoi.milBuf);

	WindowsView = Image.GetSafeHwnd();
	Image.GetWindowRect(&WinRect);

	ScreenToClient(&WinRect);
	
	Hauteur = WinRect.Height();
	Largeur = WinRect.Width();

	GetDlgItem(IDC_BUTGAUCHE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTDROITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SCROLLHORZ)->EnableWindow(FALSE);

	if(imgParent->Roi.dX < Largeur)
	{
		Largeur = imgParent->Roi.dX;
	}
	else
	{
		iScrollHMax = imgParent->Roi.dX - Largeur;
		GetDlgItem(IDC_BUTGAUCHE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTDROITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTBAS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTHAUT)->EnableWindow(FALSE);

		GetDlgItem(IDC_SCROLLVERT)->EnableWindow(FALSE);
	}

	if(imgParent->Roi.dY < Hauteur)
	{
		Hauteur = imgParent->Roi.dY;
	}
	else
	{
		iScrollVMax = imgParent->Roi.dY - Hauteur;
		GetDlgItem(IDC_BUTBAS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTHAUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCROLLVERT)->EnableWindow(TRUE);
	}

	Image.SetWindowPos(NULL,WinRect.left,WinRect.top,Largeur,Hauteur,SWP_SHOWWINDOW |SWP_FRAMECHANGED);
	
#ifndef _UNICODE
		MdispAlloc(imgParent->milSys, M_DEFAULT, MIL_TEXT("M_DEFAULT"), M_WINDOWED, &imgVisuSelectRoi.milDisp);
#else
		MdispAlloc(imgParent->milSys, M_DEFAULT, L"M_DEFAULT", M_DEFAULT, &imgVisuSelectRoi.milDisp);//UNICODE
#endif

	MdispControl(imgVisuSelectRoi.milDisp, M_INTERPOLATION_MODE, M_DEFAULT);
	MdispControl(imgVisuSelectRoi.milDisp, M_VIEW_MODE, M_DEFAULT);

	MdispSelectWindow(imgVisuSelectRoi.milDisp,imgVisuSelectRoi.milBuf,WindowsView ) ;

	m_h_bitmap[0] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_FLECHEHAUT),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_Haut.SetBitmap(m_h_bitmap[0]);

	m_h_bitmap[1] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_FLECHEBAS),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_Bas.SetBitmap(m_h_bitmap[1]);

	m_h_bitmap[2] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_FLECHEGAUCHE),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_Gauche.SetBitmap(m_h_bitmap[2]);

	m_h_bitmap[3] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_FLECHEDROITE),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_Droite.SetBitmap(m_h_bitmap[3]);

	m_h_bitmap[4] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_ZOOMPLUS),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_zoomplus.SetBitmap(m_h_bitmap[4]);

	m_h_bitmap[5] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_ZOOMMOINS),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_zoommoins.SetBitmap(m_h_bitmap[5]);

	m_h_bitmap[6] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_ANNULER),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	butCancel.SetBitmap(m_h_bitmap[6]);

	m_h_bitmap[7] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_VALIDER),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	butOk.SetBitmap(m_h_bitmap[7]);

	m_h_bitmap[8] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_STRETCH),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_stretch.SetBitmap(m_h_bitmap[8]);

	m_h_bitmap[9] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_STRETCHHRZ),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_StretchHrz.SetBitmap(m_h_bitmap[9]);

	m_h_bitmap[10] = static_cast<HBITMAP>(LoadImage (theApp.m_hInstance, MAKEINTRESOURCE(IDB_STRETCHVRT),IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
	but_StretchVrt.SetBitmap(m_h_bitmap[10]);

	MAJ_Scrollbars();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialSelectRoi::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Gestion Selection Roi
	int ZoneClik = GetClikZone(point);
	if (ZoneClik == 1)
	{

		if(bRoiDone)
		{
			// Reset de la roi
			SelectedRoi.X1 = -1;
			SelectedRoi.Y1 = -1;
			SelectedRoi.X2 = -1;
			SelectedRoi.Y2 = -1;
			SelectedRoi.dX = -1;
			SelectedRoi.dY = -1;
			MbufCopy(imgParent->milBuf,imgVisuSelectRoi.milBuf);
			bRoiDone = false;
		}

		SetCapture();
		bCapturing = true;

		//Recuperation premier point
		SelectedRoi.X1 = (long)(((point.x - ImgRect.left) / dZoomFactorH) + iScrollH / dZoomFactorH);
		SelectedRoi.Y1 = (long)(((point.y - ImgRect.top) / dZoomFactorV) + iScrollV / dZoomFactorV);

		TracerRoi();


	}
	CDialog::OnLButtonDown(nFlags, point);
}
int CDialSelectRoi::GetClikZone(CPoint point)
{
	//1=> Image
	//0=> en dehors de l'image
	CWnd* pWnd = ChildWindowFromPoint(point);
	Image.GetWindowRect(&ImgRect);
	ScreenToClient(&ImgRect);

	if (pWnd  && pWnd->GetSafeHwnd() == Image.GetSafeHwnd())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void CDialSelectRoi::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Gestion fin capture roi
	if (GetClikZone(point) == 1)
	{
		ReleaseCapture();
		bCapturing = false;

		point.x = (long)(((point.x - ImgRect.left) / dZoomFactorH) + iScrollH / dZoomFactorH);
		point.y = (long)(((point.y - ImgRect.top) / dZoomFactorV) + iScrollV / dZoomFactorV);

		//Recuperation Taille Roi
		SelectedRoi.X2 = point.x;
		SelectedRoi.Y2 = point.y;
		SelectedRoi.dX = abs(point.x - SelectedRoi.X1);
		SelectedRoi.dY = abs(point.y - SelectedRoi.Y1);
		bRoiDone = true;
		
		TracerRoi();
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDialSelectRoi::OnMouseMove(UINT nFlags, CPoint point)
{
	// Gestion mouvement
	if(bCapturing)
	{
		point.x = (long)(((point.x - ImgRect.left) / dZoomFactorH) + iScrollH / dZoomFactorH);
		point.y = (long)(((point.y - ImgRect.top) / dZoomFactorV) + iScrollV / dZoomFactorV);

		//Recuperation Taille Roi
		SelectedRoi.X2 = point.x;
		SelectedRoi.Y2 = point.y;
		SelectedRoi.dX = abs(point.x - SelectedRoi.X1);
		SelectedRoi.dY = abs(point.y - SelectedRoi.Y1);

		TracerRoi();
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDialSelectRoi::OnBnClickedButreset()
{
	// Reset de la roi
	SelectedRoi.X1 = -1;
	SelectedRoi.Y1 = -1;
	SelectedRoi.X2 = -1;
	SelectedRoi.Y2 = -1;
	SelectedRoi.dX = -1;
	SelectedRoi.dY = -1;

	MbufCopy(imgParent->milBuf,imgVisuSelectRoi.milBuf);
	
	iScrollH = 0;
	iScrollV = 0;
	dZoomFactorH = 1;
	dZoomFactorV = 1;

	MdispControl(imgVisuSelectRoi.milDisp,M_UPDATE,M_DISABLE);
	MdispPan(imgVisuSelectRoi.milDisp,iScrollH,iScrollV) ;
	MdispZoom(imgVisuSelectRoi.milDisp,dZoomFactorH,dZoomFactorV) ;
	MdispControl(imgVisuSelectRoi.milDisp,M_UPDATE,M_ENABLE);

	MAJ_Scrollbars();

}

void CDialSelectRoi::OnDestroy()
{
	CDialog::OnDestroy();

	// Liberation affichage MIL
	imgVisuSelectRoi.Decharger();

	for(int i = 0; i< NB_BOUTONS; i ++)
	{
		DeleteObject(m_h_bitmap[i]);
	}
}


void CDialSelectRoi::UpdateScrollBarsPositions()
{
	if(SB_Vert.IsWindowEnabled()) SB_Vert.SetScrollPos(iScrollV,TRUE);
	if(SB_Horz.IsWindowEnabled()) SB_Horz.SetScrollPos(iScrollH,TRUE);
}

void CDialSelectRoi::OnBnClickedButhaut()
{
	//  Scroll vers le haut
	iScrollV -= STEP_SCROLL_PIX;
	
	if(iScrollV < 0) iScrollV = 0;

	MdispPan(imgVisuSelectRoi.milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;
	UpdateScrollBarsPositions();
}

void CDialSelectRoi::OnBnClickedButbas()
{
	// Scroll vers le bas
	iScrollV += STEP_SCROLL_PIX;
	
	if(iScrollV > iScrollVMax) iScrollV = iScrollVMax;

	MdispPan(imgVisuSelectRoi.milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;

	UpdateScrollBarsPositions();

}

void CDialSelectRoi::OnBnClickedButdroite()
{
	// Scroll vers la droite
	iScrollH += STEP_SCROLL_PIX;
	
	if(iScrollH > iScrollHMax) iScrollH = iScrollHMax;

	MdispPan(imgVisuSelectRoi.milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;
	UpdateScrollBarsPositions();
}

void CDialSelectRoi::OnBnClickedButgauche()
{
	//  Scroll vers la droite
	iScrollH -= STEP_SCROLL_PIX;

	if(iScrollH < 0) iScrollH = 0;

	MdispPan(imgVisuSelectRoi.milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;
	UpdateScrollBarsPositions();
}

void CDialSelectRoi::TracerRoi()
{
	//Tracage
	STRUCT_POINT Pt1 = {0},Pt2 = {0};
	MbufCopy(imgParent->milBuf,imgVisuSelectRoi.milBuf);
	
	if((SelectedRoi.X1 >=0)&&(SelectedRoi.Y1 >=0))
	{
		Pt1.Xpix = SelectedRoi.X1;
		Pt1.Ypix = SelectedRoi.Y1;

		TracerCroix(imgVisuSelectRoi.milBuf,&Pt1,SIZE_CROIX_TRACAGE,COLOR_TRACAGE_1);
	}

	if((SelectedRoi.X2 >=0)&&(SelectedRoi.Y2 >=0)&&(SelectedRoi.dX >0)&&(SelectedRoi.dY >0))
	{
		Pt2.Xpix = SelectedRoi.X2;
		Pt2.Ypix = SelectedRoi.Y2;
		
		//tracage rectangle
		MgraColor(M_DEFAULT,M_RGB888(GetRValue(COLOR_TRACAGE_2),GetGValue(COLOR_TRACAGE_2),GetBValue(COLOR_TRACAGE_2)));
		MgraRect(M_DEFAULT,imgVisuSelectRoi.milBuf,Pt1.Xpix,Pt1.Ypix,Pt2.Xpix,Pt2.Ypix);
		
		STRUCT_POINT ptTemp1 = {0},ptTemp2 = {0};
		memcpy_s(&ptTemp1,sizeof(STRUCT_POINT),&Pt1,sizeof(STRUCT_POINT));
		memcpy_s(&ptTemp2,sizeof(STRUCT_POINT),&Pt2,sizeof(STRUCT_POINT));
		
		ptTemp1.Xpix = ptTemp1.Xpix-1;
		ptTemp1.Ypix = ptTemp1.Ypix-1;

		ptTemp2.Xpix = ptTemp2.Xpix+1;
		ptTemp2.Ypix = ptTemp2.Ypix+1;

		MgraRect(M_DEFAULT,imgVisuSelectRoi.milBuf,ptTemp1.Xpix,ptTemp1.Ypix,ptTemp2.Xpix,ptTemp2.Ypix);
		ptTemp1.Xpix = ptTemp1.Xpix-1;
		ptTemp1.Ypix = ptTemp1.Ypix-1;

		ptTemp2.Xpix = ptTemp2.Xpix+1;
		ptTemp2.Ypix = ptTemp2.Ypix+1;
		MgraRect(M_DEFAULT,imgVisuSelectRoi.milBuf,ptTemp1.Xpix,ptTemp1.Ypix,ptTemp2.Xpix,ptTemp2.Ypix);

		if(bRoiDone) TracerCroix(imgVisuSelectRoi.milBuf,&Pt2,SIZE_CROIX_TRACAGE,COLOR_TRACAGE_1);
	}

	strInfos.Format(_T("Roi:\tX1=%ld\tY1=%ld\tX2=%ld\tY2=%ld\tdX=%ld\tdY=%ld"),SelectedRoi.X1,SelectedRoi.Y1,SelectedRoi.X2,SelectedRoi.Y2,SelectedRoi.dX,SelectedRoi.dY );
	UpdateData(FALSE);
}

void CDialSelectRoi::OnBnClickedOk()
{
	//Verification position PT1 PT2
	//Intervertion si necessaire
	if((SelectedRoi.X2 < SelectedRoi.X1)
		||(SelectedRoi.Y2 < SelectedRoi.Y1))
	{
		long lXTemp = SelectedRoi.X2;
		long lYTemp = SelectedRoi.Y2;

		SelectedRoi.X2 = SelectedRoi.X1;
		SelectedRoi.Y2 = SelectedRoi.Y1;

		SelectedRoi.X1 = lXTemp;
		SelectedRoi.Y1 = lYTemp;
	}
	else if((SelectedRoi.X1 == 0)
		&&(SelectedRoi.Y1 == 0)
		&&(SelectedRoi.X2 == 0)
		&&(SelectedRoi.Y2 == 0))
	{
		//Pas de selection => image entiére
		SelectedRoi.X1 = imgParent->Roi.X1;
		SelectedRoi.Y1 = imgParent->Roi.Y1;
		SelectedRoi.X2 = imgParent->Roi.X2;
		SelectedRoi.Y2 = imgParent->Roi.Y2;
		SelectedRoi.dX = imgParent->Roi.dX;
		SelectedRoi.dY = imgParent->Roi.dY;
	}
	OnOK();
}

void CDialSelectRoi::OnBnClickedButzoomplus()
{
	//Zoom +
	dZoomFactorH = dZoomFactorH * 2.;
	dZoomFactorV = dZoomFactorV * 2.;

	if(dZoomFactorV > ZOOM_FACTOR_MAX)
		dZoomFactorV = ZOOM_FACTOR_MAX;
	if(dZoomFactorH > ZOOM_FACTOR_MAX)
		dZoomFactorH = ZOOM_FACTOR_MAX;
	
	MdispZoom(imgVisuSelectRoi.milDisp,dZoomFactorH,dZoomFactorV) ;

	MAJ_Scrollbars();
}

void CDialSelectRoi::OnBnClickedButzoommoins()
{
	//Zoom -
	dZoomFactorH = dZoomFactorH / 2.;
	dZoomFactorV = dZoomFactorV / 2.;

	if(dZoomFactorV < ZOOM_FACTOR_MIN)
		dZoomFactorV = ZOOM_FACTOR_MIN;
	if(dZoomFactorH < ZOOM_FACTOR_MIN)
		dZoomFactorH = ZOOM_FACTOR_MIN;

	iScrollH = 0;
	iScrollV = 0;

	MdispControl(imgVisuSelectRoi.milDisp,M_UPDATE,M_DISABLE);
	MdispPan(imgVisuSelectRoi.milDisp,iScrollH,iScrollV) ;
	MdispZoom(imgVisuSelectRoi.milDisp,dZoomFactorH,dZoomFactorV) ;
	MdispControl(imgVisuSelectRoi.milDisp,M_UPDATE,M_ENABLE);
	
	MAJ_Scrollbars();
	InvalidateRect(NULL);
}


void CDialSelectRoi::OnBnClickedButstretch()
{
	//Stretch de l'image pour affichage plein ecran
	dZoomFactorH = (Largeur * 1.0) / (imgParent->Roi.dX * 1.0) ;
	dZoomFactorV = (Hauteur * 1.0) / (imgParent->Roi.dY * 1.0) ;

	iScrollHMax = 0;
	iScrollVMax = 0;

	iScrollH = 0;
	iScrollV = 0;

	MdispPan(imgVisuSelectRoi.milDisp,iScrollH,iScrollV) ;
	MdispZoom(imgVisuSelectRoi.milDisp,dZoomFactorH,dZoomFactorV) ;

	MAJ_Scrollbars();

	InvalidateRect(NULL);
}

void CDialSelectRoi::MAJ_Scrollbars()
{
	Image.GetWindowRect(&WinRect);
	ScreenToClient(&WinRect);
	Hauteur = WinRect.Height();
	Largeur = WinRect.Width();

	if(imgParent->Roi.dX * dZoomFactorH <= Largeur)
	{
		Largeur = imgParent->Roi.dX;
		iScrollHMax = 0;
	}
	else
	{
		iScrollHMax =(int)( imgParent->Roi.dX * dZoomFactorH - Largeur);
	}

	if(imgParent->Roi.dY * dZoomFactorV <= Hauteur)
	{
		Hauteur = imgParent->Roi.dY;
		iScrollVMax = 0;
	}
	else
	{
		iScrollVMax =(int)( imgParent->Roi.dY * dZoomFactorV - Hauteur);
	}

	if(iScrollHMax > 0)
	{
		SB_Horz.SetScrollRange(0,iScrollHMax,FALSE);

		double dRapport = Largeur / (imgParent->Roi.dX * dZoomFactorH);

		SCROLLINFO InfoHrz;
		SB_Horz.GetScrollInfo(&InfoHrz,SIF_ALL);
		InfoHrz.nPage = (UINT)( (InfoHrz.nMax -InfoHrz.nMin) * dRapport);
		InfoHrz.nMax = InfoHrz.nMax + InfoHrz.nPage;
		SB_Horz.SetScrollInfo(&InfoHrz,TRUE);

		SB_Horz.SetScrollPos(iScrollH,TRUE);
		SB_Horz.EnableWindow(TRUE);

		GetDlgItem(IDC_BUTGAUCHE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTDROITE)->EnableWindow(TRUE);
	}
	else
	{
		SB_Horz.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTGAUCHE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTDROITE)->EnableWindow(FALSE);
	}

	if(iScrollVMax > 0)
	{
		SB_Vert.SetScrollRange(0,iScrollVMax,FALSE);
		
		double dRapport = Hauteur / (imgParent->Roi.dY * dZoomFactorV);

		SCROLLINFO InfoVrt;
		SB_Vert.GetScrollInfo(&InfoVrt,SIF_ALL);
		InfoVrt.nPage = (UINT)( (InfoVrt.nMax -InfoVrt.nMin) * dRapport);
		InfoVrt.nMax = InfoVrt.nMax + InfoVrt.nPage;
		SB_Vert.SetScrollInfo(&InfoVrt,TRUE);

		SB_Vert.SetScrollPos(iScrollV,TRUE);
		SB_Vert.EnableWindow(TRUE);

		GetDlgItem(IDC_BUTBAS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTHAUT)->EnableWindow(TRUE);
	}
	else
	{
		SB_Vert.EnableWindow(FALSE);

		GetDlgItem(IDC_BUTBAS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTHAUT)->EnableWindow(FALSE);
	}
}


void CDialSelectRoi::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//Scrollbar Horizontale
	if((nSBCode == SB_THUMBPOSITION)||(nSBCode == SB_THUMBTRACK))
	{
		if(nPos != iScrollH)
		{
			iScrollH = nPos;
			MdispPan(imgVisuSelectRoi.milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;
			SB_Horz.SetScrollPos(iScrollH,TRUE);
		}
	}
	else
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDialSelectRoi::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//Scrollbar verticale
	if((nSBCode == SB_THUMBPOSITION)||(nSBCode == SB_THUMBTRACK))
	{
		if(nPos != iScrollV)
		{
			iScrollV = nPos;
			MdispPan(imgVisuSelectRoi.milDisp,iScrollH/dZoomFactorH,iScrollV/dZoomFactorV) ;
			SB_Vert.SetScrollPos(iScrollV,TRUE);
		}
	}
	else
	{
		CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDialSelectRoi::OnBnClickedButstretchvrt()
{
	// Stretch Vertical
	dZoomFactorV = (Hauteur * 1.0) / (imgParent->Roi.dY * 1.0) ;
	dZoomFactorH = dZoomFactorV ;

	iScrollHMax = 0;
	iScrollVMax = 0;

	iScrollH = 0;
	iScrollV = 0;

	MdispPan(imgVisuSelectRoi.milDisp,iScrollH,iScrollV) ;
	MdispZoom(imgVisuSelectRoi.milDisp,dZoomFactorH,dZoomFactorV) ;

	MAJ_Scrollbars();

	InvalidateRect(NULL);
}


void CDialSelectRoi::OnBnClickedButstretchhrz()
{
	// Stretch Horizontal
	dZoomFactorH = (Largeur * 1.0) / (imgParent->Roi.dX * 1.0) ;
	dZoomFactorV = dZoomFactorH ;

	iScrollHMax = 0;
	iScrollVMax = 0;

	iScrollH = 0;
	iScrollV = 0;

	MdispPan(imgVisuSelectRoi.milDisp,iScrollH,iScrollV) ;
	MdispZoom(imgVisuSelectRoi.milDisp,dZoomFactorH,dZoomFactorV) ;

	MAJ_Scrollbars();

	InvalidateRect(NULL);
}
