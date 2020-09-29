// DialPesons.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialPesons.h"

#include "splitfrm.h"
#include "DialValidLot.h"
#include "AC_Base.h"
#include "DiagPadNum.h"
#include "DiagPadAlpha.h"

// Boîte de dialogue CDialPesons

IMPLEMENT_DYNAMIC(CDialPesons, CAcyDial)

CDialPesons::CDialPesons(CWnd* pParent /*=NULL*/, CPeson * pPeson/* = NULL*/)
	: CAcyDial(CDialPesons::IDD, pParent)
	//,MenuDiagPeson(_T("Menu_PESON"))
	, MenuDiagPeson(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_PESON"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
	Peson = pPeson;
	bSelectAllPesons = false;

}

CDialPesons::~CDialPesons()
{
}

void CDialPesons::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLEETATS, GrilleEtats);
	DDX_Control(pDX, IDC_LIST2, m_ListeHisto);
	DDX_Control(pDX, IDC_MENU_PESON, MenuDiagPeson);
}


BEGIN_MESSAGE_MAP(CDialPesons, CAcyDial)
	ON_MESSAGE(WM_PESON_MAJMESURE,OnMAJMesure)
	ON_MESSAGE(WM_PESON_DEFAUT_POIDSMIN,OnDefautPoidsMin)
	ON_MESSAGE(WM_PESON_DEFAUT_POIDSMAX,OnDefautPoidsMax)
	ON_BN_CLICKED(IDC_RADIO_PESON1, &CDialPesons::OnBnClickedRadioPeson1)
	ON_BN_CLICKED(IDC_RADIO_PESON2, &CDialPesons::OnBnClickedRadioPeson2)
	ON_BN_CLICKED(IDC_RADIO_PESON3, &CDialPesons::OnBnClickedRadioPeson3)
	ON_BN_CLICKED(IDC_RADIO_PESON4, &CDialPesons::OnBnClickedRadioPeson4)
	ON_BN_CLICKED(IDC_RADIO_PESON5, &CDialPesons::OnBnClickedRadioPeson5)
	ON_BN_CLICKED(IDC_RADIO_PESON6, &CDialPesons::OnBnClickedRadioPeson6)
	ON_BN_CLICKED(IDC_RADIO_PESONALL, &CDialPesons::OnBnClickedRadioPesonall)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialPesons







BOOL CDialPesons::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation de la grille des Etats
	GrilleEtats.SetDefCellHeight(25);//hauteur des cellules
	GrilleEtats.SetColumnCount(2);
	GrilleEtats.SetFixedRowCount(1);//légende fixe
	GrilleEtats.SetRowCount(10);
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

	//Légende
	GrilleEtats.SetItemText(1,0,_T("Etat"));
	GrilleEtats.SetItemText(2,0,_T("Derniére Mesure"));
	GrilleEtats.SetItemText(3,0,_T("Statut Derniére Mesure"));
	GrilleEtats.SetItemText(4,0,_T("Valeur dynamique stabilisée"));
	GrilleEtats.SetItemText(5,0,_T("Poids Etalon 1"));
	GrilleEtats.SetItemText(6,0,_T("Statut Etalon 1")); //0->lu par vision; 1->pesage OK; 2-> Pesage NOK (à archiver)
	GrilleEtats.SetItemText(7,0,_T("Poids Etalon 2"));
	GrilleEtats.SetItemText(8,0,_T("Statut Etalon 2"));
	GrilleEtats.SetItemText(9,0,_T("Peson OFF"));
	

	//Peson 1 par défaut
	CheckRadioButton(IDC_RADIO_PESON1,IDC_RADIO_PESON6,IDC_RADIO_PESON1);

	//Construction du Menu Dynamique
	MenuDiagPeson.bVertical = true;

	MenuDiagPeson.strProfil = theApp.CurrentUser.strProfil;
	MenuDiagPeson.SendMessage(WM_CREATE,NULL,NULL);

	//m_ListeHisto.SendMessage(ACY_LB_INIT,0,0);
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

	m_ListeHisto.Initialise(&NewFont, 1000);

	UpdateDialPeson();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialPesons::OnBnClickedRadioPeson1()
{
	//Peson 1 selectionné
	bSelectAllPesons = false;
	Peson = &theApp.Pesons[0];
	
	UpdateDialPeson();
}

void CDialPesons::OnBnClickedRadioPeson2()
{
	//Peson 2 selectionné
	bSelectAllPesons = false;
	Peson = &theApp.Pesons[1];
	
	UpdateDialPeson();
}

void CDialPesons::OnBnClickedRadioPeson3()
{
	//Peson3 selectionné
	bSelectAllPesons = false;
	Peson = &theApp.Pesons[2];
	
	UpdateDialPeson();
}

void CDialPesons::OnBnClickedRadioPeson4()
{
	//Peson 4 selectionné
	bSelectAllPesons = false;
	Peson = &theApp.Pesons[3];
	
	UpdateDialPeson();
}

void CDialPesons::OnBnClickedRadioPeson5()
{
	//Peson 5selectionné
	bSelectAllPesons = false;
	Peson = &theApp.Pesons[4];
	
	UpdateDialPeson();
}

void CDialPesons::OnBnClickedRadioPeson6()
{
	//Peson 6 selectionné
	bSelectAllPesons = false;
	Peson = &theApp.Pesons[5];
	
	UpdateDialPeson();
}

int CDialPesons::AfficherHistoMesures(void)
{
	//affichage des NB_MESURES_HISTO_PESONS derniéres mesures 

	if(bSelectAllPesons )return -1;
	
	CString strTemp = "";
	int index = 0;

	m_ListeHisto.ResetContent();
	
	for(int i = 0; i < PESON_NB_MESURES_HISTO ;i++) //Peson->indexHisto + 1 => Valeur la + ancienne
	{
		index = (i + (Peson->indexHisto) + PESON_NB_MESURES_HISTO) % PESON_NB_MESURES_HISTO;

		strTemp.Format(_T("Mesure %d = %2.3f"),index,Peson->TabMesures[index]/1000.);	
	
		m_ListeHisto.InsertString(0, strTemp);
	}

	return 0;
}

int CDialPesons::UpdateDialPeson(void)
{
	// MAJ AFfichage avec Peson En cours
	AfficherParamGrille();
	AfficherHistoMesures();

	//recherche du bouton Inhibition
	//pour MAJ texte en fonction de l'Etat du peson

	for(int i = 0; i < MenuDiagPeson.NbMaxBoutons;i++)
	{
		if(MenuDiagPeson.Boutons[i]->strCodeFonction[0] == _T("PESON_DESACTIVER"))
		{
			if(Peson->PesonMode == PESON_MODEOFF)//Peson desactivé
			{
				MenuDiagPeson.Boutons[i]->iFonction = 1;//Bouton Activer
			}
			else if(Peson->PesonMode == PESON_MODEON)//Peson Activé
			{
				MenuDiagPeson.Boutons[i]->iFonction = 0;//Bouton Desactiver
			}
		}
	}
	MenuDiagPeson.UpdateMenu();

	return 0;
}

void CDialPesons::OnBnClickedRadioPesonall()
{
	//Selection de tous les pesons
	bSelectAllPesons = true;
	
}

int CDialPesons::AfficherParamGrille(void)
{
	CString strTemp = "";
	
	strTemp.Format(_T("%d"),Peson->iEtat);
	GrilleEtats.SetItemText(1,1,strTemp);

	strTemp.Format(_T("%d"),Peson->LastMesure);
	GrilleEtats.SetItemText(2,1,strTemp);

	strTemp.Format(_T("%d"),Peson->StatutLastMesure);
	GrilleEtats.SetItemText(3,1,strTemp);

	strTemp.Format(_T("%d"),Peson->StabilisedValue);
	GrilleEtats.SetItemText(4,1,strTemp);

	strTemp.Format(_T("%d"),Peson->PoidsEtalon1);
	GrilleEtats.SetItemText(5,1,strTemp);

	strTemp.Format(_T("%d"),Peson->StatutEtalon1);
	GrilleEtats.SetItemText(6,1,strTemp); //0->lu par vision; 1->pesage OK; 2-> Pesage NOK (à archiver)

	strTemp.Format(_T("%d"),Peson->PoidsEtalon2);
	GrilleEtats.SetItemText(7,1,strTemp);
	
	strTemp.Format(_T("%d"),Peson->StatutEtalon2);
	GrilleEtats.SetItemText(8,1,strTemp);

	strTemp.Format(_T("%d"),Peson->PesonMode);
	GrilleEtats.SetItemText(9,1,strTemp);

	//Changement couleur Cellule en fonction de l'Etat du peson
	CGridCellBase * cell = NULL;
	cell = GrilleEtats.GetCell(9,1);
	if(Peson->PesonMode == PESON_MODEOFF)
	{
		//Gris
		cell->SetBackClr(RGB(236,233,216));
	}
	else
	{
		//Vert
		cell->SetBackClr(RGB(0,255,0));
	}
	
	GrilleEtats.Refresh();
	return 0;
}


void CDialPesons::OnBnClickedButSavemesuresdyna()
{
	//Sauvegarde résultats Mesures dynamiques dans BDD
	CString strRequete = _T("");

	strRequete.Format( _T("INSERT INTO MesuresEntete ([NumPeson],[NbMesures],[RefEchantillon],[RefProcedure],[PoidsNom]) VALUES(%d,%d,'%s','%s',%2.3f)"),Peson->iNumPesonBL,Peson->NbMesuresVerifDyna,Peson->RefEchantillonsVerifDyna,Peson->RefProcedureVerifDyna,Peson->PoidsNomVerifDyna); 

	try
	{
		theBase.dao_Base.Execute(strRequete);
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();

		CString strErreur = "ERREUR AJOUT Verification Dynamique::Table MesuresEntete\r\nDescription :\n";
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return ;
	}

	CADORecordset rsMesures (&theBase.dao_Base);
	
	long IDEntete = 0;
	IDEntete = 0;
	strRequete.Format(_T("SELECT  MAX(ID) AS Index FROM MesuresEntete;"));
	try
	{
		rsMesures.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
		
		if (rsMesures.IsOpen())
		{
			//V 39 IDEntete = (long) ACBaseGetInt("Index",&rsMesures);
			IDEntete = ACBaseGetLong("Index", &rsMesures);
		}

		rsMesures.Close();
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		CString strErreur = "ERREUR AJOUT Verification Dynamique::Table MesuresEntete\r\nDescription :\n";
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return ;
	}

	//Ecriture des mesures Data
	for(int i = 0; i < Peson->NbMesuresVerifDyna;i++)
	{
		strRequete.Format( _T("INSERT INTO MesuresData ([IDEntete],[Mesure]) VALUES(%ld,%2.3f)"),IDEntete,Peson->TabMesures[i]/1000.); 

		try
		{
			theBase.dao_Base.Execute(strRequete);
		}
		catch (CException* e)
		{
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause,255);
			e->Delete();
			CString strErreur = "ERREUR AJOUT Verification Dynamique::Table MesuresData\r\nDescription :\n";
			strErreur = strErreur + szCause;

			//Affichage journal événements
			theApp.SetEventsJournalStr(strErreur);
			return ;
		}
	}

	//Sauvegarde dans Fichier .CSV Export si NomDossier != ""
	//Nom du fichier = RefProcedure.CSV
	if(pParamDossierExport->value != _T(""))
	{
		CString FileName;
		CString FileData;
		FileName.Format(_T("%s\\%s.CSV"),pParamDossierExport->value,Peson->RefProcedureVerifDyna);
		try
		{
			CStdioFile File(FileName,CStdioFile::typeText|CStdioFile::modeCreate);

			//Ecriture du fichier
			for(int i = 0; i < Peson->NbMesuresVerifDyna;i++)
			{
				FileData.Format(_T("%2.3f"),Peson->TabMesures[i]/1000.);
				File.WriteString(FileData);
			}

			File.Close();
		}
		catch (CException* e)
		{
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause,255);
			e->Delete();
			CString strErreur = "ERREUR Sauvegarde Verif Dyna Fichier CSV\r\nDescription :\n";
			strErreur = strErreur + szCause;

			//Affichage journal événements
			theApp.SetEventsJournalStr(strErreur);
			return ;

		}
	}
}



int CDialPesons::StartLecture(void)
{
	//Lancement timer pour lecture priodique des valeurs
	TimerLecture = SetTimer(TIMER_AFF_PESONS,200,NULL);

	return 0;
}

int CDialPesons::StopLecture(void)
{
	KillTimer(TimerLecture);
	return 0;
}
LRESULT CDialPesons::OnMAJMesure(WPARAM Param1, LPARAM Param2)
{
	int iPeson = (int) Param1;

	//MAJ uniquement pour peson selectionné
	if(iPeson == Peson->iNumPeson)
			AfficherHistoMesures();
	return 0;
}
LRESULT CDialPesons::OnDefautPoidsMin(WPARAM Param1, LPARAM Param2)
{
	int iPeson = (int) Param1;
	if (iPeson == theApp.PesonEtui.iNumPeson)
	{
		theApp.PesonEtui.DefautPoidsMin.Ajouter();
	}
	else
	{
		theApp.Pesons[iPeson - 1].DefautPoidsMin.Ajouter();
	}
	return 0;
}
LRESULT CDialPesons::OnDefautPoidsMax(WPARAM Param1, LPARAM Param2)
{
	int iPeson = (int) Param1;
	if (iPeson == theApp.PesonEtui.iNumPeson)
	{
		theApp.PesonEtui.DefautPoidsMax.Ajouter();
	}
	else
	{
		theApp.Pesons[iPeson - 1].DefautPoidsMax.Ajouter();
	}
	
	return 0;
}

void CDialPesons::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CAcyDial::OnShowWindow(bShow, nStatus);

	// Gestion du timer d'affichage 
	if(bShow)
		StartLecture();
	else
		StopLecture();
}

void CDialPesons::OnTimer(UINT_PTR nIDEvent)
{
	// Timer de rafraichissement affichage
	switch(nIDEvent)
	{

	case TIMER_AFF_PESONS:

		if(Peson!=NULL)
		{
			AfficherParamGrille();
		}

		break;
	default:
		break;

	}

	CAcyDial::OnTimer(nIDEvent);
}
BOOL CDialPesons::OnCommand(WPARAM wParam, LPARAM lParam)
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

void  CDialPesons::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	if(strFonction == "PESON_VERIF_ETALONNAGE")
	{

//A VOIR (besoin recette)		if(theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION) return -1;
		if(theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONARRET)
		{
			theApp.SetInfosString(theApp.TableMessage.GetStr(MESSAGE_PRODUCTIONARRET_NOK));
			return ;
		}


		*theApp.m_Recette.pBitRazEtalons = TRUE;

		if(bSelectAllPesons)
		{
			//Ouverture de la page de verification du lot
			/*for(int i= 0; i < 6; i++)
			{
				theApp.Pesons[i].CurrentRecette = &theApp.m_Recette;
			}*/

			((CDialValidLot * )theApp.ValidLotView)->Recette = &theApp.m_Recette;
			((CDialValidLot * )theApp.ValidLotView)->bAllPesons = true;
			((CDialValidLot * )theApp.ValidLotView)->UpdatePesonWindow();
			theApp.DialMachineEnCours = theApp.ValidLotView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
		}
		else
		{	

			((CDialValidLot * )theApp.ValidLotView)->bAllPesons = false;
			((CDialValidLot * )theApp.ValidLotView)->iPesonSelected = Peson->iNumPeson - 1;//iPeson = index (0...5)| iNum = Numero (1...6)
			((CDialValidLot * )theApp.ValidLotView)->UpdatePesonWindow();
			theApp.DialMachineEnCours = theApp.ValidLotView;
			((CSplitFrame *)(AfxGetMainWnd()))->OnViewMachine();
		}

		theApp.Process.iOldEtat = theApp.Process.iEtat;
		theApp.Process.iEtat = ETAT_PROCESS_VERIFETALONNAGE;

		theApp.MAJMenuEtat();

	}
	else if(strFonction == "PESON_VERIF_DYNAMIQUE")
	{
		//Verification selection d'un seul peson
		if(bSelectAllPesons) return ;

		if(Peson == NULL) return ;

		if(theApp.Process.iEtat == ETAT_PROCESS_VERIFDYNAMIQUE)
		{
			//fin de la procédure
			//demande confirmation enregistrement
			if(AfxMessageBox("Enregistrement des mesures ?",MB_YESNO,MB_ICONQUESTION)== IDYES)
			{
				OnBnClickedButSavemesuresdyna();
			}
			theApp.Process.iEtat  = theApp.Process.iOldEtat;
			
			//MAj de l'Etat du process
			::SendMessage(theApp.MachineView->GetSafeHwnd(),WM_MAJ_ETAT,0,0);

			//Changement etat bouton
			if(ParentBtn != NULL)ParentBtn->iFonction = 0;
			return ;
		}

		//Verification Etat Process = HORS PRODUCTION
		if(theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION)
		{
			theApp.SetInfosString(theApp.TableMessage.GetStr(MESSAGE_HORSPRODUCTION_NOK));
			return ;
		}

		//demander letype du produit
		CDiagPadNum FrameSaisie;
		CString strInfos;
		
		FrameSaisie.strSaisie = _T("");
		FrameSaisie.strTitre = _T("Type Produit");
		
		strInfos.Format(_T("type Produit: 1 = BLISTER ; 2 = SACHET"),PESON_NB_MESURES_HISTO);
		FrameSaisie.strInfos = strInfos;

		if(FrameSaisie.DoModal() == IDOK)
		{

	#ifdef _UNICODE
			theApp.m_Recette.ArtMinims.iParam1 =(int) _wtof(FrameSaisie.strSaisie);
	#else
			theApp.m_Recette.ArtMinims.iParam1 =(int) atof(FrameSaisie.strSaisie);
	#endif
			//Copie Type Minims => Type Boite
			theApp.m_Recette.Article.Type = theApp.m_Recette.ArtMinims.iParam1;


			if((theApp.m_Recette.Article.Type <1)||(theApp.m_Recette.Article.Type>2))
				return ;
			
		}
		else
		{
			return ;
		}

		//nombre de mesures
		
		FrameSaisie.strSaisie = _T("");
		FrameSaisie.strTitre = _T("Nombre Mesures Dynamiques");
		
		strInfos.Format(_T("Nombre de mesures souhaitées durant la vérification dynamique MAX = %d"),PESON_NB_MESURES_HISTO);
		FrameSaisie.strInfos = strInfos;

		if(FrameSaisie.DoModal() == IDOK)
		{

	#ifdef _UNICODE
			Peson->NbMesuresVerifDyna =(int) _wtof(FrameSaisie.strSaisie);
	#else
			Peson->NbMesuresVerifDyna =(int) atof(FrameSaisie.strSaisie);
	#endif
			
			if(Peson->NbMesuresVerifDyna > PESON_NB_MESURES_HISTO)
			{
				
				Peson->NbMesuresVerifDyna = PESON_NB_MESURES_HISTO;
				strInfos.Format(_T("Depassement MAX ! => Nombre de mesures = %d"),PESON_NB_MESURES_HISTO);

				theApp.SetInfosString(strInfos);
			}
		}
		else
		{
			return ;
		}

		//demander le Poids Nominal
		FrameSaisie.strSaisie = _T("");
		FrameSaisie.strTitre = _T("Poids Nominal");
		FrameSaisie.strInfos = _T("Poids Nominal (masse d'essai)");

		if(FrameSaisie.DoModal() == IDOK)
		{

	#ifdef _UNICODE
			Peson->PoidsNomVerifDyna = _wtof(FrameSaisie.strSaisie);
	#else
			Peson->PoidsNomVerifDyna = atof(FrameSaisie.strSaisie);
	#endif
		}
		else
		{
			return ;
		}


		//demander la référence Echantillons
		CDiagPadAlpha FrameAlpha;
		
		FrameAlpha.strSaisie = _T("");
		FrameAlpha.strTitre = _T("Référence échantillons");
		FrameAlpha.strInfos = _T("Référence des échantillons utilisés");

		if(FrameAlpha.DoModal() == IDOK)
		{

	#ifdef _UNICODE
			Peson->RefEchantillonsVerifDyna = FrameAlpha.strSaisie;
	#else
			Peson->RefEchantillonsVerifDyna = FrameAlpha.strSaisie;
	#endif
		}
		else
		{
			return ;
		}

		//demander la référence Procédure
		FrameAlpha.strSaisie = _T("");
		FrameAlpha.strTitre = _T("Référence Procédure");
		FrameAlpha.strInfos = _T("Référence de la Procédure");

		if(FrameAlpha.DoModal() == IDOK)
		{

	#ifdef _UNICODE
			Peson->RefProcedureVerifDyna = FrameAlpha.strSaisie;
	#else
			Peson->RefProcedureVerifDyna = FrameAlpha.strSaisie;
	#endif
		}
		else
		{
			return ;
		}

		//lancer les mesures
		if(Peson->NbMesuresVerifDyna > 0)
		{
			//Changement etat bouton
			if(ParentBtn != NULL)ParentBtn->iFonction = 1;
			
			//lancement procédure
			Peson->Do_VerifDynamique();
		}

		theApp.MAJMenuEtat();

	}
	else if(strFonction == "PESON_DESACTIVER")
	{
		if(bSelectAllPesons)
		{
			for(int i = 0; i < 6; i++)
			{
				theApp.Pesons[i].inhiber(NULL,PESON_INHIBER_FORCEOFF);
			}
		}
		else if(Peson != NULL)
		{
			Peson->inhiber(BoutonParent,PESON_INHIBER_NORMAL);
		}

	}
	else if(strFonction == "PESON_ACTIVER")
	{

		if(bSelectAllPesons)
		{
			for(int i = 0; i < 6; i++)
			{
				theApp.Pesons[i].inhiber(NULL,PESON_INHIBER_FORCEON);
			}
		}
		else if(Peson != NULL)
		{
			Peson->inhiber(BoutonParent,PESON_INHIBER_NORMAL);
		}

	}
	else if(strFonction == "PESON_TARE")
	{
		if((theApp.Process.iEtat != ETAT_PROCESS_PRODUCTIONARRET)&&
			(theApp.Process.iEtat != ETAT_PROCESS_HORSPRODUCTION))
		{
			theApp.SetInfosString(theApp.TableMessage.GetStr(MESSAGE_HORSOUARRETPRODUCTION_NOK));
			return ;
		}


		if(bSelectAllPesons)
		{
			for(int i = 0; i < 6; i++)
			{
				theApp.Pesons[i].Do_Tare();
			}
		}
		else if(Peson != NULL)
		{
			Peson->Do_Tare();
		}

	}
}