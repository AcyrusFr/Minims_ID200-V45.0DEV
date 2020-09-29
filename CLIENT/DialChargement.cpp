// DialChargement.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialChargement.h"

#include "DiagPadNum.h"
#include "DiagPadAlpha.h"

#include "DiagListeReq.h"

#include"AC_Base.h"
#include"AC_Util.h"

// Boîte de dialogue CDialChargement

IMPLEMENT_DYNAMIC(CDialChargement, CAcyDial)

CDialChargement::CDialChargement(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialChargement::IDD, pParent)
	//,Menu_Chargement("Menu_Chargement")
	, Menu_Chargement(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Chargement"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
}

CDialChargement::~CDialChargement()
{
}

void CDialChargement::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENTSLIST, m_ListEvents);
	DDX_Control(pDX, IDC_STATICMENU_CHARGEMENT, Menu_Chargement);
	DDX_Control(pDX, IDC_GRILLERECETTE, GrilleRecetteCharg);
}


BEGIN_MESSAGE_MAP(CDialChargement, CAcyDial)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialChargement

BOOL CDialChargement::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	//Initialisation de la grille

	GrilleRecetteCharg.SetDefCellHeight(25);//hauteur des cellules
	GrilleRecetteCharg.SetColumnCount(2);
	GrilleRecetteCharg.SetFixedRowCount(1);	//légende fixe
	GrilleRecetteCharg.SetRowCount(26+1);	//#2 Emballage : de 23 à 26
	GrilleRecetteCharg.SetColumnWidth(0,150);
	GrilleRecetteCharg.ExpandLastColumn();
	GrilleRecetteCharg.SetColumnResize(FALSE);
	GrilleRecetteCharg.SetListMode(TRUE);
	GrilleRecetteCharg.SetSingleRowSelection(TRUE);
	GrilleRecetteCharg.SetSingleColSelection(TRUE);
	GrilleRecetteCharg.SetFixedColumnSelection(FALSE);
	GrilleRecetteCharg.SetFixedRowSelection(FALSE);
	GrilleRecetteCharg.SetFixedBkColor(RGB(0,46,184));
	GrilleRecetteCharg.SetFixedTextColor(RGB(255,255,255));

	// Changement Police des 4 premiéres lignes
	const LOGFONT * cellLogFont = NULL;
	CFont NewFont;

	NewFont.CreateFont(
	   14,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_BOLD,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   DEFAULT_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   "Arial");                 // lpszFacename

	NewFont.GetLogFont(&myLogFont);

	for(int i = 1; i < 5; i++)
	{
		GrilleRecetteCharg.SetItemFont(i,0,&myLogFont);
		GrilleRecetteCharg.SetItemFont(i,1,&myLogFont);

		GrilleRecetteCharg.SetItemFgColour(i,0,RGB(0,0,255));
		GrilleRecetteCharg.SetItemFgColour(i,1,RGB(0,0,255));
	}

	//légende
	CGridCellBase* cellule = NULL;
	cellule = GrilleRecetteCharg.GetCell(0,0);
	cellule->SetText(_T("Libelle"));
	cellule = GrilleRecetteCharg.GetCell(0,1);
	cellule->SetText(_T("Valeur"));

	//Colonne Libelle
	GrilleRecetteCharg.SetItemText(1,0,_T("Description"));
	GrilleRecetteCharg.SetItemText(2,0,_T("OF"));
	GrilleRecetteCharg.SetItemText(3,0,_T("Article"));
	GrilleRecetteCharg.SetItemText(4,0,_T("Lot"));
	GrilleRecetteCharg.SetItemText(5,0,_T("Type de produit"));
	GrilleRecetteCharg.SetItemText(6,0,_T("Description ETUI"));
	GrilleRecetteCharg.SetItemText(7,0,_T("Code ETUI"));
	GrilleRecetteCharg.SetItemText(8,0,_T("Format Etiquette"));
	GrilleRecetteCharg.SetItemText(9,0,_T("Description Notice 1"));
	GrilleRecetteCharg.SetItemText(10,0,_T("Code Notice 1"));
	GrilleRecetteCharg.SetItemText(11,0,_T("Format Notice 1"));
	GrilleRecetteCharg.SetItemText(12,0,_T("Description Notice 2"));
	GrilleRecetteCharg.SetItemText(13,0,_T("Code Notice 2"));
	GrilleRecetteCharg.SetItemText(14,0,_T("Format Notice 2"));
	GrilleRecetteCharg.SetItemText(15,0,_T("Quantité"));
	GrilleRecetteCharg.SetItemText(16,0,_T("Peremption (Mois)"));
	GrilleRecetteCharg.SetItemText(17,0,_T("Source Chargement"));
	GrilleRecetteCharg.SetItemText(18,0,_T("Reprise"));
	GrilleRecetteCharg.SetItemText(19,0,_T("Statut"));
	GrilleRecetteCharg.SetItemText(20,0,_T("Couleur"));

	GrilleRecetteCharg.SetItemText(21,0,_T("DMX Vision"));
	GrilleRecetteCharg.SetItemText(22,0,_T("Minims"));
	GrilleRecetteCharg.SetItemText(23,0,_T("Tol. poids Etui"));

	//#2 Emballage
	GrilleRecetteCharg.SetItemText(24,0,_T("Code Emballage"));
	GrilleRecetteCharg.SetItemText(25,0,_T("Description Emballage"));
	GrilleRecetteCharg.SetItemText(26,0,_T("CB Emballage"));

	//Creation du menu
	Menu_Chargement.bVertical = true;

	Menu_Chargement.strProfil = theApp.CurrentUser.strProfil;
	Menu_Chargement.SendMessage(WM_CREATE,NULL,NULL);

	//m_ListEvents.SendMessage(ACY_LB_INIT,0,0);
	CFont NewFontListe;
	NewFontListe.CreateFont(
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

	m_ListEvents.Initialise(&NewFontListe, 1000);

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialChargement::OnChargementCodeBarre()
{
	CDiagPadNum FrameSaisie;
	CString NumOFClavier = "";
	CStringA strA;

	Recette->Reset();
	MAJRecetteWindows();

	FrameSaisie.strTitre = _T("DEBUT OF");
	FrameSaisie.strInfos = _T("Saisir le numéro f'OF à produire");

	if(FrameSaisie.DoModal() == IDOK)
	{
		NumOFClavier = FrameSaisie.strSaisie;
	}
	else
	{
		return ;
	}

	OF_Charger(NumOFClavier);
}

void CDialChargement::OnBnClickedButListe()
{
	//V2.43 :	Reprise de la procédure
	//			Uniquement chargement de CSV_Attente

	CString strA;

	Recette->Reset();
	MAJRecetteWindows();

	//Ouverture liste selection OF
	CString strRequete=_T("");//A VOIR Requete selection/chargement OF
	strRequete = _T("SELECT DISTINCT [Shop Order Number] FROM CSV_TEMP");

	CDiagListeReq sDialog;
	sDialog.strRequete = strRequete;
	sDialog.sChamp = _T("Shop Order Number");
	if(sDialog.DoModal()== IDOK)//affichage de la fenetre
	{
		Recette->Reprise = false;
	}
	else
	{
		return;
	}

	//recupération du résultat
	CString strOFNum = sDialog.sResult;

	OF_Charger(strOFNum);
}

int CDialChargement::MAJRecetteWindows()
{
	//MAJ de la grille
	
	//Colonne  Valeur
	CString strTemp = "";
	GrilleRecetteCharg.SetItemText(1,1,Recette->Article.Description);

	strTemp = Recette->OfNum;
	GrilleRecetteCharg.SetItemText(2,1,strTemp);

	GrilleRecetteCharg.SetItemText(3,1,Recette->Article.Code);
	GrilleRecetteCharg.SetItemText(4,1,Recette->strLot);


	if(Recette->Article.Type == RECETTE_TYPEBLISTER)
	{
		strTemp = _T("BLISTER");
	}
	else
	{
		strTemp = _T("SACHET");
	}
	GrilleRecetteCharg.SetItemText(5,1,strTemp);
	GrilleRecetteCharg.SetItemText(6,1,Recette->ArtEtui.Description);
	strTemp.Format(_T("%s"),Recette->ArtEtui.Code);
	GrilleRecetteCharg.SetItemText(7,1,strTemp);
	strTemp.Format(_T("%d"),Recette->ArtEtiquette.iParam1);
	GrilleRecetteCharg.SetItemText(8,1,strTemp);
	GrilleRecetteCharg.SetItemText(9,1,Recette->ArtNotice1.Description);
	strTemp.Format(_T("%s"),Recette->ArtNotice1.Code);
	GrilleRecetteCharg.SetItemText(10,1,strTemp);
	strTemp.Format(_T("%d"),Recette->ArtNotice1.iParam1);
	GrilleRecetteCharg.SetItemText(11,1,strTemp);
	GrilleRecetteCharg.SetItemText(12,1,Recette->ArtNotice2.Description);
	strTemp.Format(_T("%s"),Recette->ArtNotice2.Code);
	GrilleRecetteCharg.SetItemText(13,1,strTemp);
	strTemp.Format(_T("%d"),Recette->ArtNotice2.iParam1);
	GrilleRecetteCharg.SetItemText(14,1,strTemp);
	strTemp.Format(_T("%d"),Recette->Quantite);
	GrilleRecetteCharg.SetItemText(15,1,strTemp);
	strTemp.Format(_T("%d"),Recette->PeremptionMois);
	GrilleRecetteCharg.SetItemText(16,1,strTemp);
	strTemp.Format(_T("%d"),Recette->SourceData);
	GrilleRecetteCharg.SetItemText(17,1,strTemp);

	if(Recette->Reprise)
		strTemp = _T("TRUE");
	else
		strTemp = _T("FALSE");

	GrilleRecetteCharg.SetItemText(18,1,strTemp);

	strTemp.Format(_T("%d"),Recette->iStatut);
	GrilleRecetteCharg.SetItemText(19,1,strTemp);

	strTemp.Format(_T("%d"),Recette->ArtMinims.iParam2);
	GrilleRecetteCharg.SetItemText(20,1,strTemp);

	strTemp.Format(_T("%s"),Recette->CodeDMXLot);
	GrilleRecetteCharg.SetItemText(21,1,strTemp);

	GrilleRecetteCharg.SetItemText(22,1,Recette->ArtMinims.Code);

	strTemp.Format(_T("%4.2f"),(double)(Recette->TolEtuiPlein_cg / 100.));
	GrilleRecetteCharg.SetItemText(23,1,strTemp);

	//#2 Emballage
	GrilleRecetteCharg.SetItemText(24,1,Recette->ArtEmballage.Code);
	GrilleRecetteCharg.SetItemText(25,1,Recette->ArtEmballage.Description);
	GrilleRecetteCharg.SetItemText(26,1,Recette->ArtEmballage.CodeDMX);

	GrilleRecetteCharg.Refresh();

	return 0;
}

int CDialChargement::UnlockOFLoad(void)
{
	//on fait que s'il y'a lieu
	if(Recette->IsCSV_OK == true)
		return 0;

	//Demande USer + MDP
	if(theApp.DemandeLoginManager(Recette->UserForcage,Recette->DateForcage))
	{
		//Debloquer la suite du chargement de l'OF
		//Si fichier CSV non présent
		Recette->IsOldCSV_OK = true;
		return 1;//GCA V1.17
	}
	else
	{
		//Erreur Mot de passe ou User Non manager
		return 0;
	}
	return 0;
}

bool CDialChargement::OF_Charger(CString sNumOF)
{
	//Tentative de chargement dans la table Coupe
	if(Recette->ReCharger(sNumOF))
	{
		//Verification statut en cours de la recette
		if(Recette->iStatut = RECETTE_STATUT_TERMINEE)
		{
			if(AfxMessageBox("OF SOLDE\n Demander l'autorisation d'un MANAGER pour le reprendre!",MB_YESNO,MB_ICONQUESTION)!= IDYES) return false;
			if(!theApp.DemandeLoginManager(Recette->UserForcage,Recette->DateForcage))	return false;
		}

		Recette->Reprise = true;
		Recette->SourceData = RECETTE_SOURCE_BASE;
		Recette->iStatut = RECETTE_STATUT_CHARGEE;

		MAJRecetteWindows();
		return true;
	}

	//Tentative de chargement dans la table CSV_Attente
	if(Recette->ChargerFromCSVAttente(sNumOF))
	{
		Recette->Reprise = false;
		Recette->SourceData = RECETTE_SOURCE_CSV;
		Recette->iStatut = RECETTE_STATUT_CHARGEE;

		MAJRecetteWindows();
		return true;
	}
	return false;
}

bool CDialChargement::CSV_Charger(void)
{
	//V2.43 : Nouvelle méthode de chargement du CSV
	/*
	1 : Recherche du CSV le plus récent sur le serveur
	2 :	Si OK =>	Copie en locale sans changer de nom
	3 :				BULK INSERT sur le fichier copié
	4 :				Update CSV_TEMP avec le nom du fichier
	5 : Suppression des fichiers du serveur
	*/

	CStringA strErreur;
	//1 : Recherche du CSV le plus récent sur le serveur
	CTime time= 0;			
	WIN32_FIND_DATA fData;
	HANDLE          hfind;
	FILETIME        ft;

	int NbCSVFiles = 0;
	bool IsFileOK = false;
	CString StrTemp;
	CString StrFichierCSVSelection = _T("");
	StrTemp	= ParamStrDossierSHOPORD->value;
	StrTemp += _T("\\*.csv");

	hfind = FindFirstFile(StrTemp, &fData);
	if(hfind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{ 
				//Nom du fichier
				CString StrTemp = fData.cFileName;
				//Date de modification
				FileTimeToLocalFileTime(&(fData.ftCreationTime), &ft);
				CTime TimeFile(ft);

				//Comparaison Numero du jour pour prendre le plus récent
				if(TimeFile > time)
				{
					time = TimeFile; 
					StrFichierCSVSelection = fData.cFileName;
					IsFileOK = true;
				}
				NbCSVFiles++;
			}
		}
		while (FindNextFile(hfind, &fData));
		FindClose(hfind);
	}
	else
	{
		//Erreur d'ouverture du dossier
		strErreur = _T("Erreur d'ouverture du dossier Source CSV");
		m_ListEvents.InsertString(0,strErreur);
		return false;
	}
	
	if(!IsFileOK)
	{
		strErreur = _T("Dossier Source CSV Vide");
		m_ListEvents.InsertString(0,strErreur);
		return false;
	}

	//2 : Copie en locale sur le serveur sans changer de nom
	CString StrFichierCSVSource =ParamStrDossierSHOPORD->value + _T("\\") + StrFichierCSVSelection;
	CString StrFichierCSVDestination = ParamStrNomFichierSHOPORD_ACYRUS->value + _T("\\") + StrFichierCSVSelection;
	CopyFile(StrFichierCSVSource,StrFichierCSVDestination,FALSE);
	//Vérification de présence
	if(!TrouverFichier(StrFichierCSVDestination))
	{
		strErreur.Format(_T("Erreur copie fichier CSV : %s"),StrFichierCSVSelection);
		m_ListEvents.InsertString(0,strErreur);
		return false;
	}

	//2.5 : Copie en locale sans changer de nom
	StrFichierCSVSource = StrFichierCSVDestination;
	StrFichierCSVDestination = theConfig.str_RepertoireBase + _T("\\LastCSV.CSV");
	CopyFile(StrFichierCSVSource,StrFichierCSVDestination,FALSE);
	//Vérification de présence
	if(!TrouverFichier(StrFichierCSVDestination))
	{
		strErreur.Format(_T("Erreur copie fichier CSV : %s"),StrFichierCSVSelection);
		m_ListEvents.InsertString(0,strErreur);
		return false;
	}
	
	//3 : Requete BULK INSERT pour mettre les données dans la base
	CString strRequete = _T("");
	try
	{
		strRequete.Format( _T("EXECUTE [%s].[dbo].[Import_CSV] @FileName = N'%s' "),pParamNomBase->value,StrFichierCSVDestination);	//V2.58
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges|dbSQLPassThrough*/);	
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();

		CString strErreurReq = "ERREUR Importation Fichier CSV :";
		strErreurReq = strErreurReq + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreurReq);
		m_ListEvents.InsertString(0,strErreurReq);
		return false;
	}

	try
	{
		strRequete.Format( _T("UPDATE CSV_TEMP SET [Record ID] = '%s' "),StrFichierCSVSource); 
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges|dbSQLPassThrough*/);	
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();

		CString strErreurReq = "ERREUR Importation Fichier CSV :";
		strErreurReq = strErreurReq + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreurReq);
		m_ListEvents.InsertString(0,strErreurReq);
		return false;
	}

	//5 : Suppression des fichiers du serveur			TODO
	CString StrFichierToDelete = _T("");
	CString StrFichierToCompare = _T("");
	StrTemp	= ParamStrDossierSHOPORD->value;
	StrTemp += _T("\\*.csv");

	hfind = FindFirstFile(StrTemp, &fData);
	if(hfind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				StrFichierToDelete = ParamStrDossierSHOPORD->value + _T("\\") + fData.cFileName;
				StrFichierToCompare = fData.cFileName;
				//V24 : On garde le dernier CSV pour que l'autre machine puisse le lire
				if(StrFichierToCompare != StrFichierCSVSelection)
				{
					if(DeleteFile(StrFichierToDelete)==0)
					{
						int NumErreur = GetLastError();
						strErreur.Format(_T("Erreur purge dossier CSV: %s"),fData.cFileName);
						m_ListEvents.InsertString(0,strErreur);
					}
				}
			}
		}
		while (FindNextFile(hfind, &fData));
		FindClose(hfind);
	}
	else
	{
		strErreur = _T("Erreur purge dossier CSV");
		m_ListEvents.InsertString(0,strErreur);
		return false;
	}

	strErreur.Format(_T("Source de données : %s"),StrFichierCSVSelection);
	m_ListEvents.InsertString(0,strErreur);
	return true;
}

HBRUSH CDialChargement::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CAcyDial::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
BOOL CDialChargement::OnCommand(WPARAM wParam, LPARAM lParam)
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


bool CDialChargement::CSVListeCodesSachets_Charger(void)	//V36
{
	theApp.CB_NbCodes = 0;
	CSVListeCodesSachets_Erreur = "";

	CString StrFichierLocal =theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\ITEM_MET_Local.csv");

	//V37 : Récupération du fichier effectuée avant cette fonction
	////1-Recherche du fichier sur le serveur
	//if(TrouverFichier(ParamStrSACHET_Fichier->value))
	//{
	//	//2 : Copie en locale
	//	CopyFile(ParamStrSACHET_Fichier->value,StrFichierLocal,FALSE);
	//}
	//else
	//{
	//	CSVListeCodesSachets_Erreur = "Fichier SACHET: \nATTENTION, Mise à jour non effectuée car fichier non trouvé :\n" + ParamStrSACHET_Fichier->value;
	//	AfxMessageBox(((CDialChargement * )theApp.ChargementView)->CSVListeCodesSachets_Erreur, MB_OK | MB_ICONERROR);
	//}

	if(!TrouverFichier(StrFichierLocal))
	{
		CSVListeCodesSachets_Erreur = "Fichier SACHET  - Introuvable : \n" + StrFichierLocal;
		return false;
	}

	//Lecture des données
	CStdioFile* File = NULL;
	TCHAR szDir[255]={0};
	try
	{
		File = new CStdioFile(StrFichierLocal,CStdioFile::modeRead|CStdioFile::typeText);
	}
	catch (CFileException* ex)
	{
		ex->GetErrorMessage(szDir,255);
		CSVListeCodesSachets_Erreur = "Fichier SACHET : Erreur ouverture : \n" + StrFichierLocal + "\n" + CString(szDir);
		ex->Delete();
		return false;
	}
	
	if(File==NULL)
	{
		CSVListeCodesSachets_Erreur = "Fichier SACHET : File NULL : \n" + StrFichierLocal;
		return false;
	}

	int indice =0;
	int NbLigne = 0;
	int PointVirgule = 0;
	CString token;
	CString strLigne,strItemNumber,strItmCls,strtemp;

	File->SeekToBegin();
	while(File->ReadString(strLigne)) 
	{
		strLigne.Remove('\"');
		NbLigne++;
		indice = 0;
		strItemNumber = strLigne.Tokenize(_T(","),indice);				//A
		PointVirgule = 0;
		if(indice > 0)
		{
			for(int i=indice;i<strLigne.GetLength();i++)
			{
				if(strLigne.GetAt(i) == ',')
				{
					PointVirgule++;
					if(PointVirgule == 3)
					{
						indice = i;
						break;
					}
				}
			}
		}
		if(indice > 0)	strItmCls = strLigne.Tokenize(_T(","),indice);	//E
		if(indice > 0)  
		{
			::OutputDebugString(strItmCls);
			if(strItmCls == "BM")
			{
				strItemNumber.Format("%06d",atoi(strItemNumber));
				theApp.CB_ListeCodes[theApp.CB_NbCodes] = strItemNumber;
				theApp.CB_NbCodes++;
			}
		}
	}

	File->Close();
	delete(File);
	return true;
}

int CDialChargement::ITEM_MET_CodeBoite_Extract(CString sItemNumber)	//V37
{
	//V41ALL : Reprise de la fonction pour gestion des Guillemets. But : Ne pas prendre en compte les virgules des chaines de caractères

	ITEM_MET_CodeBoite_Erreur = "";

	CString StrFichierLocal = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\ITEM_MET_Local.csv");

	if (!TrouverFichier(StrFichierLocal))
	{
		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET  - Introuvable : \n" + StrFichierLocal;
		return -1;
	}

	//Lecture des données
	CStdioFile* File = NULL;
	TCHAR szDir[255] = { 0 };
	try
	{
		File = new CStdioFile(StrFichierLocal, CStdioFile::modeRead | CStdioFile::typeText);
	}
	catch (CFileException* ex)
	{
		ex->GetErrorMessage(szDir, 255);
		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET : Erreur ouverture : \n" + StrFichierLocal + "\n" + CString(szDir);
		ex->Delete();
		return -1;
	}

	if (File == NULL)
	{
		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET : File NULL : \n" + StrFichierLocal;
		return -1;
	}

	int indice = 0;
	CString token;
	CString strLigne, strItemNumber, strtemp, strTitreColonne;
	bool bIsFound = false;
	int iNumColonne = 0;

	//Recherche de l'entete
	File->SeekToBegin();
	File->ReadString(strLigne);
	for (int i = 0;i<strLigne.GetLength();i++)
	{
		if (strLigne.GetAt(i) == ',')								//Colonne BO (67ème)
		{
			iNumColonne++;
			indice = i;
			strTitreColonne = strLigne.Tokenize(_T(","), indice);
			strTitreColonne.Remove('\"');
			if (strTitreColonne == pParamTitreColonneWolke->value)
			{
				bIsFound = true;
				break;
			}
		}
	}

	if (!bIsFound)
	{
		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET : Colonne '" + pParamTitreColonneWolke->value + "'\nNon trouvée dans : \n" + StrFichierLocal;
		File->Close();
		delete(File);
		return -1;
	}


	//Recherche pour activation impression
	bool bAttenteNextGuillemet = false;
	int iColonneEC = 0;
	CString strColonne;
	bIsFound = false;
	File->SeekToBegin();
	while (File->ReadString(strLigne))
	{
		//Recherche de la ligne du code article
		indice = 0;
		strItemNumber = strLigne.Tokenize(_T(","), indice);				//Colonne A
		strItemNumber.Remove('\"');
		if (strItemNumber == sItemNumber)	//Comparaison avec OF en cours
		{
			//Recherche de la colonne par comptage des virgules (BO Dans Excel)
			iColonneEC = 0;
			for (int i = 0;i<strLigne.GetLength();i++)
			{
				if (strLigne.GetAt(i) == '"')
				{
					bAttenteNextGuillemet = !bAttenteNextGuillemet;
				}
				if (!bAttenteNextGuillemet)		//On n'est pas entre deux Guillements		
				{
					if (strLigne.GetAt(i) == ',')
					{
						iColonneEC++;
						indice = i;
						strColonne = strLigne.Tokenize(_T(","), indice);
						::OutputDebugString(strColonne + "\n");
						if (iColonneEC == iNumColonne)
						{
							indice = i;
							strColonne = strLigne.Tokenize(_T(","), indice);
							strColonne.Remove('\"');
							::OutputDebugString("Colonne OK '" + strColonne + "'\n");
							if (strColonne == pParamTexteWolke->value)
							{
								bIsFound = true;
							}
							break;
						}
					}
				}
			}
			break;
		}
	}

	File->Close();
	delete(File);

	if (bIsFound)
	{
		return 1;
	}
	else
	{
		ITEM_MET_CodeBoite_Erreur = "Pas d'impression : \n" + sItemNumber;
		return 0;
	}

	return -1;
}
//int CDialChargement::ITEM_MET_CodeBoite_Extract(CString sItemNumber)	//V37
//{
//	ITEM_MET_CodeBoite_Erreur = "";
//
//	CString StrFichierLocal =theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\ITEM_MET_Local.csv");
//
//	if(!TrouverFichier(StrFichierLocal))
//	{
//		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET  - Introuvable : \n" + StrFichierLocal;
//		return -1;
//	}
//
//	//Lecture des données
//	CStdioFile* File = NULL;
//	TCHAR szDir[255]={0};
//	try
//	{
//		File = new CStdioFile(StrFichierLocal,CStdioFile::modeRead|CStdioFile::typeText);
//	}
//	catch (CFileException* ex)
//	{
//		ex->GetErrorMessage(szDir,255);
//		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET : Erreur ouverture : \n" + StrFichierLocal + "\n" + CString(szDir);
//		ex->Delete();
//		return -1;
//	}
//	
//	if(File==NULL)
//	{
//		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET : File NULL : \n" + StrFichierLocal;
//		return -1;
//	}
//
//	int indice =0;
//	int NbVirgules = 0;
//	CString token;
//	CString strLigne,strItemNumber,strSec_Ref,strtemp,strTitreColonne;
//	bool bIsFound = false;
//	int iNumColonne = 0;
//
//	//Recherche de l'entete
//	File->SeekToBegin();
//	File->ReadString(strLigne);
//	for(int i=0;i<strLigne.GetLength();i++)
//	{
//		if(strLigne.GetAt(i) == ',')								//Colonne BO (67ème)
//		{
//			iNumColonne++;
//			indice = i;
//			strTitreColonne = strLigne.Tokenize(_T(","),indice);	
//			strTitreColonne.Remove('\"');
//			if(strTitreColonne == pParamTitreColonneWolke->value)
//			{
//				bIsFound = true;
//				break;
//			}
//		}
//	}
//
//	if(!bIsFound)
//	{
//		ITEM_MET_CodeBoite_Erreur = "Fichier ITEM_MET : Colonne '" + pParamTitreColonneWolke->value + "'\nNon trouvée dans : \n" + StrFichierLocal;
//		File->Close();
//		delete(File);
//		return -1;
//	}
//
//
//	//Recherche pour activation impression
//	bIsFound = false;
//	File->SeekToBegin();
//	while(File->ReadString(strLigne)) 
//	{
//		indice = 0;
//		strItemNumber = strLigne.Tokenize(_T(","),indice);				//Colonne A
//		strItemNumber.Remove('\"');
//		//Comparaison avec OF en cours
//		if(strItemNumber == sItemNumber)
//		{
//			//Lecture de la colonne BO
//			NbVirgules = 0;
//			for(int i=0;i<strLigne.GetLength();i++)
//			{
//				if(strLigne.GetAt(i) == ',')								//Colonne BO (67ème)
//				{
//					NbVirgules++;
//					indice = i;
//					strSec_Ref = strLigne.Tokenize(_T(","),indice);	
//					if(NbVirgules == iNumColonne)
//					{
//						indice = i;
//						strSec_Ref = strLigne.Tokenize(_T(","),indice);	
//						strSec_Ref.Remove('\"');
//						//V37TODO : Gestion réponse
//						if(strSec_Ref == pParamTexteWolke->value)
//						{
//							bIsFound = true;
//						}
//						break;
//					}
//				}
//			}
//			break;
//		}
//	}
//
//	File->Close();
//	delete(File);
//
//	if(bIsFound)
//	{
//		return 1;
//	}
//	else
//	{
//		ITEM_MET_CodeBoite_Erreur = "Pas d'impression : \n" + sItemNumber;
//		return 0;
//	}
//
//	return -1;
//}

bool CDialChargement::ITEM_MET_Copier(void)	//V37
{
	ITEM_MET_Copier_Erreur = "";

	CString StrFichierLocal =theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\ITEM_MET_Local.csv");

	//1-Recherche du fichier sur le serveur
	if(TrouverFichier(ParamStrSACHET_Fichier->value))
	{
		//2 : Copie en locale
		CopyFile(ParamStrSACHET_Fichier->value,StrFichierLocal,FALSE);
		return true;
	}
	else
	{
		ITEM_MET_Copier_Erreur = "Fichier ITEM_MET: \nATTENTION, Mise à jour non effectuée car fichier non trouvé :\n" + ParamStrSACHET_Fichier->value;
		AfxMessageBox(((CDialChargement * )theApp.ChargementView)->ITEM_MET_Copier_Erreur, MB_OK | MB_ICONERROR);
		return false;
	}

	return false;
}


#pragma warning(default:4995)

