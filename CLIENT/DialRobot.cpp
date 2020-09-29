// DialRobot.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialRobot.h"


// Boîte de dialogue CDialRobot

IMPLEMENT_DYNAMIC(CDialRobot, CAcyDial)

CDialRobot::CDialRobot(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialRobot::IDD, pParent)
	//,Menu_robot(_T("Menu_ROBOT"))
	, Menu_robot(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_ROBOT"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	m_robot = NULL;
	bAllSelected = false;
}

CDialRobot::~CDialRobot()
{
}

void CDialRobot::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLEETATS, GrilleEtats);
	DDX_Control(pDX, IDC_MENUROBOT, Menu_robot);
	DDX_Control(pDX, IDC_LIST1, ListHistoRobot);
}


BEGIN_MESSAGE_MAP(CDialRobot, CAcyDial)
	ON_BN_CLICKED(IDC_RADIO_ROBOT1, &CDialRobot::OnBnClickedRadioRobot1)
	ON_BN_CLICKED(IDC_RADIO_ROBOT2, &CDialRobot::OnBnClickedRadioRobot2)
	ON_BN_CLICKED(IDC_RADIO_ROBOT3, &CDialRobot::OnBnClickedRadioRobot3)
	ON_BN_CLICKED(IDC_RADIO_ROBOTALL, &CDialRobot::OnBnClickedRadioRobotall)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialRobot

BOOL CDialRobot::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation de la grille des Etats
	GrilleEtats.SetDefCellHeight(25);//hauteur des cellules
	GrilleEtats.SetColumnCount(2);
	GrilleEtats.SetFixedRowCount(1);//légende fixe
	GrilleEtats.SetRowCount(13);
	GrilleEtats.ExpandColumnsToFit(TRUE);
	GrilleEtats.SetListMode(TRUE);
	GrilleEtats.SetSingleRowSelection(TRUE);
	GrilleEtats.SetSingleColSelection(TRUE);
	GrilleEtats.SetFixedColumnSelection(FALSE);
	GrilleEtats.SetFixedRowSelection(FALSE);
	//légende
	CGridCellBase* cellule;
	GrilleEtats.SetFixedBkColor(RGB(0,46,184));
	GrilleEtats.SetFixedTextColor(RGB(255,255,255));
	cellule = GrilleEtats.GetCell(0,0);
	cellule->SetText(_T("Paramétre"));
	cellule = GrilleEtats.GetCell(0,1);
	cellule->SetText(_T("Etat"));


	//remplissage 1ere colonne
	GrilleEtats.SetItemText(1,0,_T("Numero"));
	GrilleEtats.SetItemText(2,0,_T("X"));
	GrilleEtats.SetItemText(3,0,_T("Y"));
	GrilleEtats.SetItemText(4,0,_T("Z"));
	GrilleEtats.SetItemText(5,0,_T("Statut Position"));
	GrilleEtats.SetItemText(6,0,_T("Nb Rebus Vision"));
	GrilleEtats.SetItemText(7,0,_T("Nb Rebus Poids"));
	GrilleEtats.SetItemText(8,0,_T("Nb Elements Boite"));
	GrilleEtats.SetItemText(9,0,_T("Numero Prise"));
	GrilleEtats.SetItemText(10,0,_T("Robot OFF"));
	GrilleEtats.SetItemText(11,0,_T("Etat"));
	GrilleEtats.SetItemText(12,0,_T("Nb. Top Elec."));

	//Robot 1 par défaut
	CheckRadioButton(IDC_RADIO_ROBOT1,IDC_RADIO_ROBOT3,IDC_RADIO_ROBOT1);

	//Creation du menu robot
	Menu_robot.bVertical = true;

	Menu_robot.strProfil = theApp.CurrentUser.strProfil;
	Menu_robot.SendMessage(WM_CREATE,NULL,NULL);


	GrilleEtats.Refresh();

	//ListHistoRobot.SendMessage(ACY_LB_INIT,0,0);
	CFont NewFont;
	NewFont.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_SEMIBOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename

	ListHistoRobot.Initialise(&NewFont, 1000);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialRobot::OnBnClickedRadioRobot1()
{
	// Selection robot1
	m_robot = &theApp.m_Robot1;
	bAllSelected = false;
	UpdateFenetre();

	MAJHistoPositions();


}

void CDialRobot::OnBnClickedRadioRobot2()
{
	// Selection robot2
	m_robot = &theApp.m_Robot2;
	bAllSelected = false;
	UpdateFenetre();
	MAJHistoPositions();
}

void CDialRobot::OnBnClickedRadioRobot3()
{
	// Selection robot3
	m_robot = &theApp.m_Robot3;
	bAllSelected = false;
	UpdateFenetre();
	MAJHistoPositions();
}

void  CDialRobot::MAJHistoPositions(void)
{
	CString strTemp;


	ListHistoRobot.ResetContent();
	//ListHistoRobot.SendNotifyMessage(ACY_LB_RESETCONTENT,0,0);
	for(int i = 49; i > 0; i--)
	{
		strTemp.Format(_T("%d\t|%d\t|%d\t|%d\t|%d\t"),m_robot->HistoPos[i].iNumImg,m_robot->HistoPos[i].PosStatut,m_robot->HistoPos[i].X,m_robot->HistoPos[i].Y,m_robot->HistoPos[i].Z);
		ListHistoRobot.InsertString(0, strTemp);
	/*	strA = strTemp;
		ListHistoRobot.SendNotifyMessage(ACY_LB_INSERTSTRING,0,(LPARAM)(LPTSTR)strA.GetBuffer());
		strA.ReleaseBuffer();*/
	}

	strTemp.Format(_T("NUM\t|STATUT\t|X\t|Y\t|Z\t"));
	ListHistoRobot.InsertString(0, strTemp);
	
}

void CDialRobot::OnBnClickedRadioRobotall()
{
	// Selection de tous les robots
	bAllSelected = true;

}

int CDialRobot::UpdateFenetre(void)
{

	// affichage des informations du robot
	CString strTemp = _T("");

	if(m_robot == NULL) return 0;

	//recherche du bouton Inhibition
	//pour MAJ texte en fonction de l'Etat du Robot

	for(int i = 0; i < Menu_robot.NbMaxBoutons;i++)
	{
		if(Menu_robot.Boutons[i]->strCodeFonction[0] == _T("ROBOT_DESACTIVER"))
		{
			if(m_robot->RobotMode == ROBOT_MODEOFF)//Robot desactivé
			{
				Menu_robot.Boutons[i]->iFonction = 1;//Bouton Activer
			}
			else if(m_robot->RobotMode == ROBOT_MODEON)//Robot Activé
			{
				Menu_robot.Boutons[i]->iFonction = 0;//Bouton Desactiver
			}
		}
	}
	Menu_robot.UpdateMenu();
	//MAJ Grille

	strTemp.Format(_T("%d"),m_robot->iNumRobot);
	GrilleEtats.SetItemText(1,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->X);
	GrilleEtats.SetItemText(2,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->Y);
	GrilleEtats.SetItemText(3,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->Z);
	GrilleEtats.SetItemText(4,1,strTemp);

	strTemp.Format(_T("%d"),(int) m_robot->PosStatut);
	GrilleEtats.SetItemText(5,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->NbRebusVisionUnused);//GCA1412
	GrilleEtats.SetItemText(6,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->NbRebusPoidsUnused);
	GrilleEtats.SetItemText(7,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->NbElementsBoiteUnused);
	GrilleEtats.SetItemText(8,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->NumPrise);
	GrilleEtats.SetItemText(9,1,strTemp);

	strTemp.Format(_T("%d"),(int)m_robot->RobotMode);
	GrilleEtats.SetItemText(10,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->iEtat);
	GrilleEtats.SetItemText(11,1,strTemp);

	strTemp.Format(_T("%d"),m_robot->NumTop);
	GrilleEtats.SetItemText(12,1,strTemp);

	GrilleEtats.Refresh();

	return 1;
}
int CDialRobot::StartLecture(void)
{
	//Lancement timer pour lecture priodique des valeurs
	TimerLecture = SetTimer(TIMER_AFF_ROBOTS,200,NULL);

	return 0;
}

int CDialRobot::StopLecture(void)
{
	KillTimer(TimerLecture);
	return 0;
}

void CDialRobot::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CAcyDial::OnShowWindow(bShow, nStatus);

	// Gestion du timer d'affichage 
	if(bShow)
		StartLecture();
	else
		StopLecture();
}

void CDialRobot::OnTimer(UINT_PTR nIDEvent)
{
	// Timer de rafraichissement affichage
	switch(nIDEvent)
	{

	case TIMER_AFF_ROBOTS:
		UpdateFenetre();

		break;
	default:
		break;

	}

	CAcyDial::OnTimer(nIDEvent);
}BOOL CDialRobot::OnCommand(WPARAM wParam, LPARAM lParam)
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