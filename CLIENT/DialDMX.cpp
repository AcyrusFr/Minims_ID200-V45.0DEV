// DialDMX.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialDMX.h"
#include "DiagPadAlpha.h"

// Boîte de dialogue CDialDMX

IMPLEMENT_DYNAMIC(CDialDMX, CAcyDial)

CDialDMX::CDialDMX(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialDMX::IDD, pParent)
	//,Menu_DMX("Menu_DMX")
	, Menu_DMX(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_DMX"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	m_ProcessDMX = NULL;
}

CDialDMX::~CDialDMX()
{
}

void CDialDMX::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLEETATS, m_GrilleDMX);
	DDX_Control(pDX, IDC_STATICMENU_DMX, Menu_DMX);
	DDX_Control(pDX, IDC_LIST1, ListeDMX);
	DDX_Control(pDX, IDC_LIST2, ListeDMX2);
	DDX_Control(pDX, IDC_LIST3, ListeDMX3);
}


BEGIN_MESSAGE_MAP(CDialDMX, CAcyDial)
	ON_BN_CLICKED(IDC_RADDMX1, &CDialDMX::OnBnClickedRaddmx1)
	ON_BN_CLICKED(IDC_RADDMX2, &CDialDMX::OnBnClickedRaddmx2)
	ON_BN_CLICKED(IDC_RADDMX3, &CDialDMX::OnBnClickedRaddmx3)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialDMX

void CDialDMX::OnBnClickedRaddmx1()
{
	// Selection du datamatrix Etui
	m_ProcessDMX = &theApp.m_DMXEtui;
	UpdateDMXWindow();
}

void CDialDMX::OnBnClickedRaddmx2()
{
	// Selection du datamatrix Notice 1
	m_ProcessDMX = &theApp.m_DMXNotice1;
	UpdateDMXWindow();
}

void CDialDMX::OnBnClickedRaddmx3()
{
	// Selection du datamatrix Notice 2
	m_ProcessDMX = &theApp.m_DMXNotice2;
	UpdateDMXWindow();
}

int CDialDMX::UpdateDMXWindow(void)
{
	//MAJ de la fenêtre en fonction du processDMX en cours
	if(m_ProcessDMX != NULL)
	{
		//Affichage de la liste du lecteur en cours
		if (m_ProcessDMX == &theApp.m_DMXEtui)
		{
			GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_LIST2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LIST3)->ShowWindow(SW_HIDE);
		}
		else if (m_ProcessDMX == &theApp.m_DMXNotice1)
		{
			GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LIST2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_LIST3)->ShowWindow(SW_HIDE);
		}
		else if (m_ProcessDMX == &theApp.m_DMXNotice2)
		{
			GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LIST2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LIST3)->ShowWindow(SW_SHOW);
		}

		CString strTemp;
		//Etat Connexion
		if (m_ProcessDMX->pBitEtatConnexion != NULL)
		{
			if (*m_ProcessDMX->pBitEtatConnexion != DMX_CONNECTED)
			{
				m_GrilleDMX.SetItemText(1, 1, _T("NOK (Deconnecté)"));
			}
			else
			{
				m_GrilleDMX.SetItemText(1, 1, _T("OK"));
			}
		}
		else
		{
			m_GrilleDMX.SetItemText(1, 1, _T("NULL"));
		}
		//Code à Lire
		strTemp.Format(_T("%s"),m_ProcessDMX->IdDMX);
		m_GrilleDMX.SetItemText(2,1,strTemp);
		
		//Inhibition
		if (m_ProcessDMX->pBitInhibition != NULL)
		{
			if (*m_ProcessDMX->pBitInhibition != FALSE)
			{
				m_GrilleDMX.SetItemText(3, 1, _T("Lecteur Inhibé"));
			}
			else
			{
				m_GrilleDMX.SetItemText(3, 1, _T("Lecteur Activé"));
			}
		}
		else
		{
			m_GrilleDMX.SetItemText(3, 1, _T("NULL"));
		}
		//Nb Codes Lus
		strTemp.Format(_T("%d"), m_ProcessDMX->iNbCodesRecus);
		m_GrilleDMX.SetItemText(4, 1, strTemp);

		//Codes Lus
		CString strTemp2 = _T("");
		strTemp = _T("");

		for (int i = 0; i < m_ProcessDMX->iNbCodesRecus; i++)
		{
			strTemp2.Format(_T("%s"), m_ProcessDMX->strCodes.GetAt(i));
			strTemp = strTemp + _T("|") + strTemp2;
		}
		
		m_GrilleDMX.SetItemText(5, 1, strTemp);
		//Com OPC
		m_GrilleDMX.SetItemText(6, 1, _T(""));
		//Bit Lecture
		if (m_ProcessDMX->pBitLecture != NULL)strTemp.Format(_T("%d"), (int) *m_ProcessDMX->pBitLecture);
		else strTemp.Format(_T("NULL"));
		m_GrilleDMX.SetItemText(7, 1, strTemp);
		//Bit Connexion
		if (m_ProcessDMX->pBitEtatConnexion != NULL)strTemp.Format(_T("%d"), (int)*m_ProcessDMX->pBitEtatConnexion);
		else strTemp.Format(_T("NULL"));
		m_GrilleDMX.SetItemText(8, 1, strTemp);
		//Bit Inhibition
		if(m_ProcessDMX->pBitInhibition != NULL) strTemp.Format(_T("%d"), (int)*m_ProcessDMX->pBitInhibition);
		else strTemp.Format(_T("NULL"));
		m_GrilleDMX.SetItemText(9, 1, strTemp);
		//Val Result
		if (m_ProcessDMX->iValResult != NULL)strTemp.Format(_T("%d"), (int)*m_ProcessDMX->iValResult);
		else strTemp.Format(_T("NULL"));
		m_GrilleDMX.SetItemText(10, 1, strTemp);

		//recherche du bouton Inhibition
		//pour MAJ texte en fonction de l'Etat du peson

		for(int i = 0; i < Menu_DMX.NbMaxBoutons;i++)
		{
			if ((m_ProcessDMX->pBitInhibition != NULL)&&
				(Menu_DMX.Boutons[i]->strCodeFonction[0] == _T("DMX_DESACTIVER")))
			{
				//if(m_ProcessDMX->DMXMode == DMX_OFF)
				//{
				//	Menu_DMX.Boutons[i]->iFonction = 1;//Bouton Activer
				//}
				//else if(m_ProcessDMX->DMXMode == DMX_ON)
				//{
				//	Menu_DMX.Boutons[i]->iFonction = 0;//Bouton Desactiver
				//}
				if(*m_ProcessDMX->pBitInhibition == TRUE)
				{
					Menu_DMX.Boutons[i]->iFonction = 1;//Bouton Activer
				}
				else
				{
					Menu_DMX.Boutons[i]->iFonction = 0;//Bouton Desactiver
				}
			}
		}

		Menu_DMX.UpdateMenu();

	}
	else
	{
		m_GrilleDMX.SetItemText(1, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(2, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(3, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(4, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(5, 1, _T("NULL"));
								   
		m_GrilleDMX.SetItemText(6, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(7, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(8, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(9, 1, _T("NULL"));
		m_GrilleDMX.SetItemText(10, 1, _T("NULL"));
	}

	m_GrilleDMX.Refresh();
	
	return 0;
}

BOOL CDialDMX::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation de la grille des Etats
	m_GrilleDMX.SetDefCellHeight(25);//hauteur des cellules
	m_GrilleDMX.SetColumnCount(2);
	m_GrilleDMX.SetFixedRowCount(1);//légende fixe
	m_GrilleDMX.SetRowCount(11);
	m_GrilleDMX.ExpandColumnsToFit(TRUE);
	m_GrilleDMX.SetListMode(TRUE);
	m_GrilleDMX.SetSingleRowSelection(TRUE);
	m_GrilleDMX.SetSingleColSelection(TRUE);
	m_GrilleDMX.SetFixedColumnSelection(FALSE);
	m_GrilleDMX.SetFixedRowSelection(FALSE);
	//légende
	CGridCellBase* cellule;
	m_GrilleDMX.SetFixedBkColor(RGB(0,46,184));
	m_GrilleDMX.SetFixedTextColor(RGB(255,255,255));
	cellule = m_GrilleDMX.GetCell(0,0);
	cellule->SetText(_T("Paramétre"));
	cellule = m_GrilleDMX.GetCell(0,1);
	cellule->SetText(_T("Etat"));

	//Légende
	m_GrilleDMX.SetItemText(1,0,_T("Etat Connexion"));
	m_GrilleDMX.SetItemText(2,0,_T("Code à Lire"));
	m_GrilleDMX.SetItemText(3,0,_T("Inhibition"));
	m_GrilleDMX.SetItemText(4, 0, _T("Nb Codes Lus"));
	m_GrilleDMX.SetItemText(5, 0, _T("Codes Lus"));
	
	m_GrilleDMX.SetItemText(6, 0, _T("Com OPC"));
	m_GrilleDMX.SetItemText(7, 0, _T("Bit Lecture"));
	m_GrilleDMX.SetItemText(8, 0, _T("Bit Connexion"));
	m_GrilleDMX.SetItemText(9, 0, _T("Bit Inhibition"));
	m_GrilleDMX.SetItemText(10, 0, _T("Val Result"));

	//Initialisation du menu
	Menu_DMX.bVertical = true;

	Menu_DMX.strProfil = theApp.CurrentUser.strProfil;
	Menu_DMX.SendMessage(WM_CREATE, NULL, NULL);
	

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

	ListeDMX.Initialise(&NewFont, 300);
	ListeDMX2.Initialise(&NewFont, 300);
	ListeDMX3.Initialise(&NewFont, 300);

	theApp.m_DMXEtui.ListeAffichage = &ListeDMX;
	theApp.m_DMXNotice1.ListeAffichage = &ListeDMX2;
	theApp.m_DMXNotice2.ListeAffichage = &ListeDMX3;

	m_ProcessDMX = &theApp.m_DMXEtui;
	CheckRadioButton(IDC_RADDMX1,IDC_RADDMX3,IDC_RADDMX1);

	UpdateDMXWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

int CDialDMX::StartLecture(void)
{
	//Lancement timer pour lecture priodique des valeurs
	TimerLecture = SetTimer(TIMER_AFF_DMX,200,NULL);

	return 0;
}

int CDialDMX::StopLecture(void)
{
	KillTimer(TimerLecture);
	return 0;
}


void CDialDMX::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CAcyDial::OnShowWindow(bShow, nStatus);

	// Gestion du timer d'affichage 
	if(bShow)
		StartLecture();
	else
		StopLecture();
}

void CDialDMX::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{

	case TIMER_AFF_DMX:
			UpdateDMXWindow();
		

		break;
	default:
		break;

	}
	CAcyDial::OnTimer(nIDEvent);
}
BOOL CDialDMX::OnCommand(WPARAM wParam, LPARAM lParam)
{

	return CAcyDial::OnCommand(wParam, lParam);
}
void  CDialDMX::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle *)BoutonParent;

	if(strFonction == _T("DMX_DESACTIVER"))
	{

		if(m_ProcessDMX != NULL)
		{
			if ((theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONDEMARRAGE) &&
				(theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONMARCHE))
			{
				ParentBtn->iFonction = 1;
				m_ProcessDMX->Inhiber();
			}
			else
			{
				//Message Inhibition Interdite dans ce mode
				if (m_ProcessDMX->ListeAffichage != NULL)
				{
					m_ProcessDMX->ListeAffichage->ResetContent();
					m_ProcessDMX->ListeAffichage->InsertString(0, theApp.TableMessage.GetStr(MESSAGE_INHIBITION_INTERDITE));
				}
			}

			//Traca si Différent de HORS PRODUCTION
			if (theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION)
			{
				CString strLibelle;

				if (m_ProcessDMX->ClientSocket.bIsConnected == false)
				{
					strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_DESACTIVATION_DMX), _T("Pas de connexion au lecteur"));
				}
				else
				{
					strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_DESACTIVATION_DMX), m_ProcessDMX->ClientSocket.strIP);
				}

				theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE, strLibelle, _T(""), theApp.CurrentUser.strCode);
			}
		}

	}
	else if(strFonction == _T("DMX_ACTIVER"))
	{

		if(m_ProcessDMX != NULL)
		{
			if (theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION)
			{
				if (m_ProcessDMX->IdDMX == _T(""))
				{
					ParentBtn->iFonction = 1;

					m_ProcessDMX->Inhiber();

					((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();
					if (m_ProcessDMX->ListeAffichage != NULL)
					{
						m_ProcessDMX->ListeAffichage->ResetContent();
						m_ProcessDMX->ListeAffichage->InsertString(0, "Activation impossible, pas de code à lire");
					}
					return;
				}
			}

			
			if ((theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONDEMARRAGE) &&
				(theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONMARCHE))
			{
				ParentBtn->iFonction = 0;
				m_ProcessDMX->Activer();
			}
			else
			{
				//Message Activation Interdite dans ce mode
				if (m_ProcessDMX->ListeAffichage != NULL)
				{
					m_ProcessDMX->ListeAffichage->ResetContent();
					m_ProcessDMX->ListeAffichage->InsertString(0, theApp.TableMessage.GetStr(MESSAGE_INHIBITION_INTERDITE));
				}
			}

			//Traca si Différent de HORS PRODUCTION
			if (theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION)
			{
				CString strLibelle;
				
				if (m_ProcessDMX->ClientSocket.bIsConnected == false)
				{
					strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_ACTIVATION_DMX), _T("Pas de connexion au lecteur"));
				}
				else
				{
					strLibelle.Format(theApp.TableMessage.GetStr(MESSAGE_ACTIVATION_DMX), m_ProcessDMX->ClientSocket.strIP);
				}

				theApp.theJournal.Evenement(JOURNAL_TYPEMODEDEGRADE, strLibelle, _T(""), theApp.CurrentUser.strCode);
			}
		}

	}
	else if(strFonction == _T("DMX_CONFIGWEB"))
	{
		if(m_ProcessDMX != NULL)
		{
			m_ProcessDMX->OpenWebConfig();
		}
	}
	else if (strFonction == "DMX_TESTCODE")
	{
		//SockV3
		if (m_ProcessDMX != NULL)
		{
			if (m_ProcessDMX->ClientSocket.bIsConnected)
			{
				if (AfxMessageBox(_T("Deconnecter le lecteur DMX ?"), MB_YESNO, MB_ICONQUESTION) == IDYES)
				{
					m_ProcessDMX->ListeAffichage->SetCustomColor(COLOR_ROUGE);
					m_ProcessDMX->ListeAffichage->InsertString(0, _T("Test deconnexion manuelle"));

					m_ProcessDMX->ClientSocket.Deconnecter();
				}
			}
		}
		return;
		//Fin SockV3

		if (theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION)
		{
			if (m_ProcessDMX != NULL)
			{
				m_ProcessDMX->ListeAffichage->SetCustomColor(COLOR_ROUGE);
				m_ProcessDMX->ListeAffichage->InsertString(0, theApp.TableMessage.GetStr(MESSAGE_HORSPRODUCTION_NOK));
			}
			return;
		}

		if (m_ProcessDMX != NULL)
		{
			//Ajout commentaire
			CDiagPadAlpha frmSaisie;

			frmSaisie.strTitre = _T("Code à Lire");
			frmSaisie.strInfos = _T("");
			frmSaisie.strSaisie = _T("");

			if (frmSaisie.DoModal() != IDOK)
				return;


			m_ProcessDMX->IdDMX = frmSaisie.strSaisie;
		}
	}

}
