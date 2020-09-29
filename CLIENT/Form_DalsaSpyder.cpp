// Form_DalsaSpyder.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "Form_DalsaSpyder.h"



// Boîte de dialogue CForm_DalsaSpyder

IMPLEMENT_DYNAMIC(CForm_DalsaSpyder, CAcyDial)

CForm_DalsaSpyder::CForm_DalsaSpyder(CWnd* pParent /*=NULL*/)
	: CAcyDial(CForm_DalsaSpyder::IDD, pParent)
	, str_commande(_T(""))
	, str_recv(_T(""))
{
	this->Cam = &theApp.DalsaCamera1; 
}

CForm_DalsaSpyder::~CForm_DalsaSpyder()
{
}

void CForm_DalsaSpyder::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT4, str_InfosCom);
	DDX_Text(pDX, IDC_EDIT10, str_InfosReg);
	DDX_GridControl(pDX, IDC_GRILLE, grille);
	DDX_Text(pDX, IDC_EDIT_CMD, str_commande);
	DDX_Text(pDX, IDC_EDIT2, str_recv);
}


BEGIN_MESSAGE_MAP(CForm_DalsaSpyder, CAcyDial)
	ON_BN_CLICKED(IDC_BUT_WRITECMD, &CForm_DalsaSpyder::OnBnClickedButWritecmd)
	ON_BN_CLICKED(IDC_BUT_OPENDOC, &CForm_DalsaSpyder::OnBnClickedButOpendoc)
	ON_BN_CLICKED(IDC_BUTGETVALUE, &CForm_DalsaSpyder::OnBnClickedButgetvalue)
	ON_BN_CLICKED(IDC_BUTSETVALUE, &CForm_DalsaSpyder::OnBnClickedButsetvalue)
	ON_MESSAGE(WM_MESSFORMCAM, &CForm_DalsaSpyder::AfficherRep)
	ON_BN_CLICKED(IDC_RADIO_CAM1, &CForm_DalsaSpyder::OnBnClickedRadioCam1)
	ON_BN_CLICKED(IDC_RADIO_CAM2, &CForm_DalsaSpyder::OnBnClickedRadioCam2)
END_MESSAGE_MAP()


// Gestionnaires de messages de CForm_DalsaSpyder

BOOL CForm_DalsaSpyder::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    // Gestion Touche Entrée

    CWnd *pWnd = GetFocus();
    switch(wParam)
    {
      case IDOK: if(pWnd!=GetDlgItem(IDOK))
                 {
						
					 //Envoi de la commande quand on fait la touche Entrée
					 OnBnClickedButWritecmd();
                      return FALSE;
                 }
                 break;

      case IDCANCEL:if(pWnd!=GetDlgItem(IDCANCEL))
                    {
                        return FALSE;
                    }
                    break;
    }
    return CAcyDial::OnCommand(wParam, lParam);
}

LRESULT CForm_DalsaSpyder::AfficherRep(WPARAM Param1, LPARAM Param2)
{
		char CharRecv= (char) Param1;
		
		Cam->AfficheRep(CharRecv);
		return 0;
}


BOOL CForm_DalsaSpyder::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	//Initialisation de la grille
	grille.SetColumnCount(4);
	grille.SetRowCount(NBPARAMAFFDALSA);
	grille.SetFixedColumnCount(2);
	grille.SetFixedRowCount(1);
	grille.SetFixedBkColor(RGB(0,46,184));
	grille.SetFixedTextColor(RGB(255,255,255));

	//taille des cellules
	BOOL res=FALSE;
	res=grille.SetColumnWidth(0,300);
	res=grille.SetColumnWidth(1,75);
	res=grille.SetColumnWidth(2,85);
	res=grille.SetColumnWidth(3,60);
	grille.ExpandColumnsToFit(FALSE);

	grille.SetDefCellHeight(20);
	
	//remplissage des titres
	grille.SetCellText(0,0,_T("Paramétre"));
	grille.SetCellText(0,1,_T("commande"));
	grille.SetCellText(0,2,_T("Valeur"));
	grille.SetCellText(0,3,_T("Nb Param"));

	//remplissage des colonnes paramétre et Commande
	grille.SetCellText(1,0,_T("FPN Pixel Coeficients"));
	grille.SetCellText(1,1,_T("ccf"));
	grille.SetCellText(1,3,_T("0"));

	grille.SetCellText(2,0,_T("PRNU Pixel Coeficients"));
	grille.SetCellText(2,1,_T("ccp"));
	grille.SetCellText(2,3,_T("0"));

	grille.SetCellText(3,0,_T("Camera Link Mode"));
	grille.SetCellText(3,1,_T("clm"));
	grille.SetCellText(1,3,_T("1"));

	grille.SetCellText(4,0,_T("LUT status"));
	grille.SetCellText(4,1,_T("eil"));
	grille.SetCellText(4,3,_T("0"));

	grille.SetCellText(5,0,_T("Pixel Correction Status"));
	grille.SetCellText(5,1,_T("epc"));
	grille.SetCellText(5,3,_T("2"));

	grille.SetCellText(6,0,_T("Camera Model Number"));
	grille.SetCellText(6,1,_T("gcm"));
	grille.SetCellReadOnly(6,2,true);//Read Only

	grille.SetCellText(7,0,_T("Camera Serial Number"));
	grille.SetCellText(7,1,_T("gcs"));
	grille.SetCellReadOnly(7,2,true);//Read Only

	grille.SetCellText(8,0,_T("Camera Software Version"));
	grille.SetCellText(8,1,_T("gcv"));
	grille.SetCellReadOnly(8,2,true);//Read Only

	grille.SetCellText(9,0,_T("Maximum Exposure time for current line rate"));
	grille.SetCellText(9,1,_T("ger"));
	grille.SetCellReadOnly(9,2,true);//Read Only

	grille.SetCellText(10,0,_T("Current ROI"));
	grille.SetCellText(10,1,_T("roi"));
	grille.SetCellText(10,3,_T("4"));

	grille.SetCellText(11,0,_T("Settings saved"));
	grille.SetCellText(11,1,_T("rus"));
	grille.SetCellText(11,3,_T("0"));

	grille.SetCellText(12,0,_T("Gain Analogique"));
	grille.SetCellText(12,1,_T("sag"));
	grille.SetCellText(12,3,_T("2"));

	grille.SetCellText(13,0,_T("Update Gain Reference ATTENTION"));
	grille.SetCellText(13,1,_T("ugr"));
	grille.SetCellText(13,3,_T("0"));

	grille.SetCellText(14,0,_T("Horizontal Binning factor"));
	grille.SetCellText(14,1,_T("sbh"));
	grille.SetCellText(14,3,_T("1"));

	grille.SetCellText(15,0,_T("Serial Baud Rate ATTENTION"));
	grille.SetCellText(15,1,_T("sbr"));
	grille.SetCellText(15,3,_T("1"));

	grille.SetCellText(16,0,_T("Current Expo Mode"));
	grille.SetCellText(16,1,_T("sem"));
	grille.SetCellText(16,3,_T("1"));

	grille.SetCellText(17,0,_T("Current Exposure Time"));
	grille.SetCellText(17,1,_T("set"));
	grille.SetCellText(17,3,_T("1"));

	grille.SetCellText(18,0,_T("Internal Temperature ( °C )"));
	grille.SetCellText(18,1,_T("vt"));
	grille.SetCellReadOnly(18,2,true);//Read Only

	grille.SetCellText(19,0,_T("Tension Alimentation ( V )"));
	grille.SetCellText(19,1,_T("vv"));
	grille.SetCellReadOnly(19,2,true);//Read Only

	grille.SetCellText(19,0,_T("Lire Tous les Paramétres"));
	grille.SetCellText(19,1,_T("gcp"));
	grille.SetCellReadOnly(19,2,true);//Read Only
	
	//empeche le redimensionnement des colonnes
	grille.SetColumnResize(FALSE);
	grille.SetRowResize(FALSE);

	grille.Refresh();

	//Camera 1 par défaut
	CheckRadioButton(IDC_RADIO_CAM1,IDC_RADIO_CAM2,IDC_RADIO_CAM1);

	//affichage des valeurs COM et REG de la cam
	CString str_1;
	CString str_2;
	str_1.Format(_T("COM%d,%d,%c,%d,%d"),Cam->m_numport,Cam->m_baudrate,Cam->m_parity,Cam->m_wordlength,Cam->m_stopbits);
	str_2=theConfig.str_Registre+_T("\\")+Cam->Regkey;
	this->str_InfosCom=str_1;
	this->str_InfosReg=str_2;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CForm_DalsaSpyder::OnBnClickedButWritecmd()
{
	this->UpdateData(TRUE);
	if(str_commande == _T("")) return;

	// Ecrire commande Générique
	bool result;
	char buff[300];
	CString commande;
	ZeroMemory(buff,300);
	this->UpdateData(TRUE);
	//envoi commande
	result=Cam->Ecrire_commande(this->str_commande);
	this->UpdateData(FALSE);
}

void CForm_DalsaSpyder::OnBnClickedButOpendoc()
{
	// Ouvrir PDF de la caméra correspondante
	HINSTANCE result;
	CString path;
	CString pdfpath;

	//path=Cam->str_DocsPath;
	pdfpath=_T("C:\\ACYRUS\\Docs\\49968-Spyder 3 CL series Manual.pdf");
	path+=pdfpath; 
	result=ShellExecute(NULL,_T("open"),path,NULL,NULL,SW_SHOWDEFAULT);
	if((_int64)result==ERROR_FILE_NOT_FOUND)
	{
		AfxMessageBox(_T("Fichier introuvable: verifier le chemin d'accés au fichier \n\r( C:\\ACYRUS\\Docs\\49968-Spyder 3 CL series Manual.pdf )"),MB_ICONINFORMATION,0);
	}
}

void CForm_DalsaSpyder::OnBnClickedButgetvalue()
{
	// Get Value selectionnée dans la grille
	
	CCellRange range;
	CGridCellBase * cellule_cmd;
	CString commande,strTemp;

	//recupération de la selection
	range=grille.GetSelectedCellRange();
	
	if((range.GetMaxRow() == -1) &&( range.GetMaxCol() == -1))
		return;
	// Set Value selectionnée dans la grille
	cellule_cmd = grille.GetCell(range.GetMinRow(),1);
	
	commande = cellule_cmd->GetText();
	
	strTemp = _T("get ");
	strTemp += commande;
	commande = strTemp;

	Cam->Ecrire_commande(commande);

	this->UpdateData(FALSE);
	grille.Refresh();

}

void CForm_DalsaSpyder::OnBnClickedButsetvalue()
{

	CCellRange range;
	CGridCellBase * cellule_val;
	CGridCellBase * cellule_cmd;
	CGridCellBase * cellule_nbParam;
	DWORD State = 0;
	CString commande;
	CString valeur,strnbparam;

	//recupération de la selection
	range=grille.GetSelectedCellRange();

	// Set Value selectionnée dans la grille
	cellule_val=grille.GetCell(range.GetMinRow(),2);
	cellule_cmd=grille.GetCell(range.GetMinRow(),1);
	cellule_nbParam=grille.GetCell(range.GetMinRow(),3);
	
	//verification si on peut ecrire la valeur
	State = cellule_val->GetState();
	if( (State & GVIS_READONLY) != GVIS_READONLY)
	{
		commande=cellule_cmd->GetText();
		strnbparam = cellule_nbParam->GetText();

		//verification du nombre de paramétres a envoyer
		if(strnbparam != _T("0"))
		{
			valeur=cellule_val->GetText();
			commande+=(_T(" ")+valeur);
		}

		Cam->Ecrire_commande(commande);

		this->UpdateData(FALSE);
		grille.Refresh();
	}
	else
	{
		AfxMessageBox(_T("Valeur en lecture seule"),MB_ICONINFORMATION,MB_OK);
	}

}

void CForm_DalsaSpyder::OnBnClickedRadioCam1()
{
	// Selection Camera 1
	this->Cam = &theApp.DalsaCamera1; 
	RefreshWindow();
}

void CForm_DalsaSpyder::OnBnClickedRadioCam2()
{
	// Selection Camera 2
	this->Cam = &theApp.DalsaCamera2; 
	RefreshWindow();
}

int CForm_DalsaSpyder::RefreshWindow(void)
{
	//affichage des valeurs COM et REG de la cam
	CString str_1;
	CString str_2;
	str_1.Format(_T("COM%d,%d,%c,%d,%d"),Cam->m_numport,Cam->m_baudrate,Cam->m_parity,Cam->m_wordlength,Cam->m_stopbits);
	str_2=theConfig.str_Registre+_T("\\")+Cam->Regkey;
	this->str_InfosCom=str_1;
	this->str_InfosReg=str_2;
	UpdateData(FALSE);

	return 0;
}
