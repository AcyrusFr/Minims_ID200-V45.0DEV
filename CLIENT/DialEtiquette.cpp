// DialModifPiece.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialEtiquette.h"
#include "AC_Util.h"
#include "DiagPadAlpha.h"
#include "DiagPadNum.h"
#include "DiagListeReq.h"
#include "AC_File.h"
//#include "AcyCL.h"

#define COLOR_ETAPE_ENCOURS	RGB(0,255,0)
#define COLOR_ETAPE_PRECEDENTES	RGB(128,128,255)
// Boîte de dialogue CDialEtiquette

IMPLEMENT_DYNAMIC(CDialEtiquette, CAcyDial)

CDialEtiquette::CDialEtiquette(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialEtiquette::IDD, pParent)
{
	imgParent = NULL;

	CurrentExtract = NULL;
	iExtract = 0;
}

CDialEtiquette::~CDialEtiquette()
{
}

void CDialEtiquette::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, ImageVisu);
	DDX_ManagedControl(pDX, IDC_FORMULAIRE_EXTRACTION, form_Extract);
	DDX_Control(pDX, IDC_LISTETIQRESULT, m_ListeResult);
}


BEGIN_MESSAGE_MAP(CDialEtiquette, CAcyDial)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_COPYDATA()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialEtiquette
BOOL CDialEtiquette::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	//Construction des Menus Dynamiques
	//Menu_ModifImg.bVertical = false;
	//Menu_ModifImg.strProfil = theApp.CurrentUser.strProfil;
	//Menu_ModifImg.SendMessage(WM_CREATE,NULL,NULL);

	//ETIQ : Chargement du formulaire
	//MAJ Formulaire
	System::String^ sConnect = gcnew System::String (theConfig.str_NomBase);
	System::String^ sRepertoire = gcnew System::String (theConfig.str_RepertoireBase);

	form_Extract->Init(sConnect,sRepertoire,"","");
	form_Extract->hwnd = (System::IntPtr) this->GetSafeHwnd();		
	form_Extract->OpenForm("Etiquette", "ETIQV37", "Static Extract", false, false, false, false, false, false, false, false);		//V37
	form_Extract->SetFormMenu(false,false,false);

	form_Extract->cls->Frm_Lire();
	form_Extract->cls->Data_SetIndexChamp();
	form_Extract->cls->Frm_SetSMenu();
	form_Extract->cls->Frm_SetForm(form_Extract->pObjet, false);

	//Récupération taille affichage image
	/*WindowsView = NULL;
	WindowsView = Image.GetSafeHwnd();
	Image.GetWindowRect(&WinRect);

	ScreenToClient(&WinRect);
	
	Hauteur = WinRect.Height();
	Largeur = WinRect.Width();*/

#ifdef DEBUG
	ImageVisu.bAfficheMenu = true;
#else
	ImageVisu.bAfficheMenu = false;
#endif // DEBUG

	ImageVisu.bAfficheBandeauInfos = true;

	ImageVisu.SendMessage(WM_CREATE, NULL, NULL);
	ImageVisu.SetScrollBarsToDebut();

	ImageVisu.pExecuteFonctionParent = &ExecuterFonctionExtGeneral;

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

	m_ListeResult.Initialise(&NewFont, 1000);

	//MAJ ScrollBars
//	MAJ_Scrollbars();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialEtiquette::OnDestroy()
{
	CAcyDial::OnDestroy();
	
	// Liberation affichage MIL
	imgVisuSelectRoi.Decharger();
}

int CDialEtiquette::GetClikZone(CPoint point)
{
	////1=> Image
	////0=> en dehors de l'image
	//CWnd* pWnd = ChildWindowFromPoint(point);
	//ImageVisu.GetWindowRect(&ImgRect);
	//ScreenToClient(&ImgRect);

	//if (pWnd  && pWnd->GetSafeHwnd() == Image.GetSafeHwnd())
	//{
	//	return 4;
	//}
	///*else if (pWnd  && pWnd->GetSafeHwnd() == static_description.GetSafeHwnd())
	//{
	//	return 2;
	//}*/
	//else
	//{
	//	return 0;
	//}

	CPoint PtClient;
	PtClient.SetPoint(point.x, point.y);

	ScreenToClient(&PtClient);
	CWnd* pWnd = ChildWindowFromPoint(PtClient);

	if (pWnd  && pWnd->GetSafeHwnd() == ImageVisu.GetSafeHwnd())
	{
		return 4;
	}
	else
	{
		return 0;
	}
}
void CDialEtiquette::RefreshImage(void)
{
	if (imgParent->milBuf > 0)
	{
		MbufCopy(imgParent->milBuf,imgVisuSelectRoi.milBuf);
		imgVisuSelectRoi.ClearOverlay();
	}
}
void CDialEtiquette::SetImage(void)
{
	if(imgParent == NULL) return;

	SetWindowText(_T("Selection ROI: ") + imgParent->sLibelle);

	imgVisuSelectRoi.Decharger();
	imgVisuSelectRoi.sLibelle = _T("Image Visu Selection ROI");
	imgVisuSelectRoi.Charger(imgParent->milSys,imgParent->Roi.dX,imgParent->Roi.dY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);

	MbufCopy(imgParent->milBuf,imgVisuSelectRoi.milBuf);

	ImageVisu.SetImage(&imgVisuSelectRoi);

}



void CDialEtiquette::SetImageCellule()
{
	//V44.8 ajout graphicliste + visuimage
	ImageVisu.Decharger();
	//V37 imgParent = &theApp.EtiqScellement.imgVisu;
	imgParent = &theApp.VisionBoite.imgVisu;
	if((imgVisuSelectRoi.Roi.dX <imgParent->Roi.dX)||(imgVisuSelectRoi.Roi.dY <imgParent->Roi.dY))
	{
		SetImage();
	}

	if(imgVisuSelectRoi.milBuf == 0)
	{
		SetImage();
	}

	ImageVisu.SetImage(&imgVisuSelectRoi);
	ImageVisu.SetGraphicListe(&theApp.VisionBoite.Graph_Visu);
	ImageVisu.StretchHrz();
	ImageVisu.SetScrollBarsToDebut();

	RefreshImage();
}

int CDialEtiquette::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	//Recuperation pointeur sur Menu Parent
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	//Boutons du formulaire
	if(strFonction == "ETIQ_SELECTION")
	{
		if(theApp.EtiqTest == &theApp.EtiqScellement)
		{
			theApp.EtiqTest = &theApp.EtiqWolke;
		}
		else
		{
			theApp.EtiqTest = &theApp.EtiqScellement;
		}
		ChargerFormulaire(theApp.EtiqTest->Etiq_strCode);
		theApp.EtiqTest->LireEntete(false);
		theApp.EtiqTest->Extractions_Charger();

		//V 44.1 Affichage des erreurs de chargement des fichiers
		for (int i = 0; i < theApp.EtiqTest->Etiq_iNbExtract; i++)
		{
			if (theApp.EtiqTest->Extract[i].Extract_bIsOK == false)
			{
				m_ListeResult.InsertString(0, theApp.EtiqTest->Extract[i].strResult);
			}
		}
	}
	else if(strFonction == "ETIQ_FORMAT")
	{
		//Chargement Etiquette boite manuel
		CString strRequete=_T("");//A VOIR Requete selection/chargement OF
		strRequete = _T("SELECT DISTINCT [Code] FROM Etiquette");

		CDiagListeReq sDialog;
		sDialog.strRequete = strRequete;
		sDialog.sChamp = _T("Code");
		if(sDialog.DoModal()== IDOK)//affichage de la fenetre
		{
			//V30 : Gestion des nouveaux formats
			if(sDialog.sResult == "_Nouveau format_")
			{
				CDiagPadAlpha FrameAlpha;
				FrameAlpha.strSaisie = _T("");
				FrameAlpha.strTitre = _T("Ajout d'un nouveau format étiquette");
				FrameAlpha.strInfos = _T("Nom du format à ajouter");
				if(FrameAlpha.DoModal() == IDOK)
				{
					sDialog.sResult = FrameAlpha.strSaisie;
					theApp.EtiqTest->Etiq_strCode = FrameAlpha.strSaisie;
					theApp.EtiqTest->LireEntete(true);
				}
				else
				{
					return 0;
				}
			}
			ChargerFormulaire(sDialog.sResult);
			theApp.EtiqTest->Etiq_strCode = sDialog.sResult;
			theApp.EtiqTest->LireEntete(false);
			theApp.EtiqTest->Extractions_Charger();

			theApp.EtiqTest->Extractions_Draw(ImageVisu.TracageSelection);//V 44.8

			//V 44.1 Affichage des erreurs de chargement des fichiers
			for (int i = 0; i < theApp.EtiqTest->Etiq_iNbExtract; i++)
			{
				if (theApp.EtiqTest->Extract[i].Extract_bIsOK == false)
				{
					m_ListeResult.InsertString(0, theApp.EtiqTest->Extract[i].strResult);
				}
			}
		}
	}
	else if (strFonction == _T("ETIQ_SAVEPOS_EXTRACT"))
	{
		//Sauvegarde des positions des extractions
		PARAM_ROI RoiTemp = { 0 };
		MIL_INT Label = 0;
		if (ImageVisu.TracageSelection != NULL)
		{
			Label = ImageVisu.TracageSelection->GetSelectedLabel();
			ImageVisu.TracageSelection->GetROI(Label, RoiTemp);
		}

		for (int iExtract = 0; iExtract < theApp.EtiqTest->Etiq_iNbExtract; iExtract++)
		{
			if (theApp.EtiqTest->Extract[iExtract].iLabelTracage == Label)
			{
				memcpy_s(&theApp.EtiqTest->RoiExtractBase[iExtract], sizeof(PARAM_ROI), &RoiTemp, sizeof(PARAM_ROI));
				theApp.EtiqTest->UpdateROI();//Sauvegarde dans la BDD
				theApp.EtiqTest->Extractions_Charger();
				theApp.EtiqTest->DecalageX = 0;	//V29 : RAZ des décalages au moment du positionnement initial A VOIR
				theApp.EtiqTest->DecalageY = 0; //V29 : RAZ des décalages au moment du positionnement initial A VOIR
				theApp.EtiqTest->Apprentissage(iExtract);
				iIndexExtract = iExtract;
				break;
			}
		}

		ImageVisu.TracageSelection->DeselectALL();

	}
	
	else if (strFonction == _T("ETIQ_PARAM_EXTRACT"))
	{
		MIL_INT Label = 0;
		if (ImageVisu.TracageSelection != NULL)
		{
			Label = ImageVisu.TracageSelection->GetSelectedLabel();
		}

		for (int iExtract = 0; iExtract < theApp.EtiqTest->Etiq_iNbExtract; iExtract++)
		{
			if (theApp.EtiqTest->Extract[iExtract].iLabelTracage == Label)
			{
				iIndexExtract = iExtract;
				theApp.EtiqTest->Extract[iIndexExtract].OpenInteractiveWindow();
				break;
			}
		}

		ImageVisu.TracageSelection->DeselectALL();
	}
	else if (strFonction == _T("ETIQ_EXTRACT_MODELE"))
	{
		PARAM_ROI RoiTemp = { 0 };
		MIL_INT Label = 0;
		if (ImageVisu.TracageSelection != NULL)
		{
			Label = ImageVisu.TracageSelection->GetSelectedLabel();
			ImageVisu.TracageSelection->GetROI(Label, RoiTemp);
		}

		for (int iExtract = 0; iExtract < theApp.EtiqTest->Etiq_iNbExtract; iExtract++)
		{
			if (theApp.EtiqTest->Extract[iExtract].iLabelTracage == Label)
			{
				//Saisie du nom de fichier du modele
				CString strFichierModele = _T("");
				strFichierModele = ACYFile_ChoisirFichier(_T("Selection Fichier Modele"), true, _T("Fichiers Modeles (*.mmf)\0*.mmf\0Tous les Fichiers (*.*)\0*.*\0\0"));

				if (strFichierModele == _T(""))
				{
					return FALSE;
				}

				if (ACYFile_Trouver(strFichierModele) == true)
				{
					/*CString strResult = AcyMessageBox(theConfig.str_Registre,
						F_MSGBOX_QUEST_YN,
						MSG_F_MSGBOX_QUEST_TITRE,
						theApp.iModeCSDesign,
						_T("Etes vous certain de vouloir remplacer le fichier modele ?"));

					if (strResult != _T("OUI"))
					{
						return FALSE;
					}*/

					UINT_PTR Result = AfxMessageBox(_T("Etes vous certain de vouloir remplacer le fichier modele ?"), MB_YESNO| MB_ICONASTERISK);
						
					if (Result != IDYES)
					{
						return FALSE;
					}
				}

				//Création du modele
				if (ModeleExtraction_Creer(&RoiTemp, strFichierModele) == false)
				{
					return FALSE;
				}

				//sauvegarde de l'extraction
				theApp.EtiqTest->Extract[iExtract].strFicModele = strFichierModele;
				theApp.EtiqTest->Extract[iExtract].UpdateFichier();

				memcpy_s(&theApp.EtiqTest->RoiExtractBase[iExtract], sizeof(PARAM_ROI), &RoiTemp, sizeof(PARAM_ROI));
				theApp.EtiqTest->UpdateROI();//Sauvegarde dans la BDD
				theApp.EtiqTest->Extractions_Charger();
				theApp.EtiqTest->DecalageX = 0;	//V29 : RAZ des décalages au moment du positionnement initial A VOIR
				theApp.EtiqTest->DecalageY = 0; //V29 : RAZ des décalages au moment du positionnement initial A VOIR
				theApp.EtiqTest->Apprentissage(iExtract);
				iIndexExtract = iExtract;

				//Ouverture de la fenetre du modele pour verification 
				theApp.EtiqTest->Extract[iIndexExtract].OpenInteractiveWindow();
				break;
			}
		}

		ImageVisu.TracageSelection->DeselectALL();
	}
	else if (strFonction == "ETIQ_TEST")
	{
		//V37 
		MbufCopy(theApp.VisionBoite.imgSource.milBuf,theApp.VisionBoite.imgVisu.milBuf);

		theApp.EtiqTest->Extractions_Charger();		//V49 : rechargement systématique des extractions
		//V 44.1 Affichage des erreurs de chargement des fichiers
		for (int i = 0; i < theApp.EtiqTest->Etiq_iNbExtract; i++)
		{
			if (theApp.EtiqTest->Extract[i].Extract_bIsOK == false)
			{
				m_ListeResult.InsertString(0, theApp.EtiqTest->Extract[i].strResult);
			}
		}

		theApp.EtiqTest->Etiq_bTest = true;
		theApp.EtiqTest->Vision_DoExtraction();

		theApp.EtiqTest->Extractions_Draw(ImageVisu.TracageSelection);//V 44.8

		//V37
		if (IsWindowVisible())
		{
			RefreshImage();
		}
	}
	else if(strFonction == "ETIQ_INHIB")
	{
		//V37TODO : Gérer ce message en fonction de l'extraction en cours de chargement
		*theApp.EtiqTest->pBitInhibLecture = !*theApp.EtiqTest->pBitInhibLecture;//V 44.0
		
		CString strMessage = _T("");
		if (*theApp.EtiqTest->pBitInhibLecture == TRUE)
		{
			strMessage = theApp.EtiqTest->Etiq_strTitre + _T(" : Desactivation Contrôle Lecture");
		}
		else
		{
			strMessage = theApp.EtiqTest->Etiq_strTitre + _T(" : Reactivation Contrôle Lecture");
		}
		theApp.SetInfosString(strMessage);

	}
	else if(strFonction == "ETIQ_IMAGE")
	{
		//Chargement d'une image de test
		CFileDialog* file_dlg= new CFileDialog(TRUE);
		file_dlg->m_ofn.lpstrTitle=_T("Selection Fichier Image ");
		file_dlg->m_ofn.lpstrFilter=_T("Fichiers Images (*.bmp)\0*.bmp\0Tous les Fichier (*.*)\0*.*\0\0");
		INT_PTR resultdlg=file_dlg->DoModal();

		if(resultdlg==IDOK)
		{
			CString strFileToCopy = file_dlg->GetPathName();
			theApp.VisionBoite.ChargerImageTest(strFileToCopy);	//V37
		}
		delete (file_dlg);
	}
	else if(strFonction == "ETIQ_VISU")
	{
		if(theApp.EtiqTest->imgTrait.IsFenetreOpen)
		{
			theApp.EtiqTest->imgTrait.FenetreFermer();
			theApp.EtiqTest->imgRotation.FenetreFermer();
		}
		else
		{
			theApp.EtiqTest->imgTrait.FenetreOuvrir();
			theApp.EtiqTest->imgRotation.FenetreOuvrir();
		}
	}
	else if(strFonction == "ETIQ_ACTUALISER")
	{
		theApp.EtiqTest->Extractions_Charger();
	}
	else if(strFonction == "ETIQ_APPRENDRE")
	{
		//Apprentissage des positions relatives
		theApp.EtiqTest->UpdateReference();
		//Enregistrement de l'image étiquette de référence 
		CString strNomFichierEtiquette;
		strNomFichierEtiquette = theConfig.str_RepertoireApp + _T("\\") + theConfig.str_Machine + _T("\\IMG_Etiquettes\\") + theApp.EtiqTest->Etiq_strCode + _T(".bmp");
		//V37 MbufExport(strNomFichierEtiquette.GetBuffer(),M_BMP,theApp.EtiqScellement.imgSource.milBuf);	
		MbufExport(strNomFichierEtiquette.GetBuffer(),M_BMP,theApp.VisionBoite.imgSource.milBuf);	
	}
	else if (strFonction.Left(14) == "ETIQ_REFERENCE")
	{
		//V49 Sélection d'un référence
		int posEgal = strFonction.ReverseFind('=');
		int IDDetail;
		if(posEgal>0)
		{
			CString strID = strFonction.Right(strFonction.GetLength()-(posEgal+1));
			#ifdef _UNICODE
				IDDetail = _wtoi(strID);
			#else
				IDDetail = atoi(strID);
			#endif
			for (int i = 0; i < theApp.EtiqTest->Etiq_iNbExtract; i++)
			{
				if (theApp.EtiqTest->Extract[i].IDDetail == IDDetail)
				{
					iIndexExtract = i;
					break;
				}
			}
		}
	}
	else if (strFonction == "ETIQ_CAMERAS")
	{
		theApp.ExecuterFonctionExt("AFF_UEYE_VIEW",NULL);
	}
	//V 44.2
	else if (strFonction == "ETIQ_RELOAD")
	{
		if (theApp.EtiqTest != &theApp.EtiqScellement)
		{
			AfxMessageBox(_T("Fonction disponible uniquement pour l'etiquette de scellement"),MB_OK|MB_ICONWARNING);
			return 0;
		}

		theApp.EtiqScellement.Etiq_strCode = _T("NC");
		*theApp.EtiqScellement.pBitInhibLecture = FALSE;//V 44.0
		*theApp.EtiqScellement.pBitInhibImpression = FALSE;

		ChargerFormulaire(theApp.EtiqScellement.Etiq_strCode);

		m_ListeResult.ResetContent();
		CString strMess = theApp.EtiqScellement.Etiq_strTitre + _T(" : Chargement des données Eticoncept...");
		bool bReponseOK = theApp.EtiqScellement.ETC_ChargerData(theApp.m_Recette.OfNum);
		if (!bReponseOK)
		{
			strMess = theApp.EtiqScellement.Etiq_strEtat;
			theApp.SetInfosString(strMess);
		}
		else
		{
			strMess.Format(_T("%s : Fichier Eticoncept chargé = %s"), theApp.EtiqScellement.Etiq_strTitre, theApp.EtiqScellement.Etiq_strCode);
			m_ListeResult.InsertString(0, strMess);
			for (int i = 0; i<NB_CHAMP_MAX; i++)
			{
				if (theApp.EtiqScellement.ETC_strChampBrut[i] != _T(""))
				{
					strMess.Format(_T("%s = %s"), theApp.EtiqScellement.ETC_strChampTitre[i], theApp.EtiqScellement.ETC_strChampFormat[i]);
					m_ListeResult.InsertString(0, strMess);
				}
			}
		}

		//Chargement de l'étiquette
		if (bReponseOK)
		{
			bReponseOK = theApp.EtiqScellement.LireEntete(true);
			if (!bReponseOK)
			{
				strMess = theApp.EtiqScellement.Etiq_strEtat;
				m_ListeResult.InsertString(0, strMess);
				theApp.SetInfosString(strMess);
			}
		}

		//Chargement des extractions
		if (bReponseOK)
		{
			theApp.EtiqScellement.ETC_AffecterDataTo_EtiqDetail();

			strMess = theApp.EtiqScellement.Etiq_strTitre + _T(" : Chargement des Extractions...");

			bReponseOK = theApp.EtiqScellement.Extractions_Charger();

			//V 44.1
			for (int i = 0; i < theApp.EtiqScellement.Etiq_iNbExtract; i++)
			{
				if (theApp.EtiqScellement.Extract[i].Extract_bIsOK == false)
				{
					m_ListeResult.InsertString(0, theApp.EtiqScellement.Extract[i].strResult);
					bReponseOK = false;
				}
			}


			if (!bReponseOK)
			{
				strMess = theApp.EtiqScellement.Etiq_strEtat;
				m_ListeResult.InsertString(0, strMess);
				theApp.SetInfosString(strMess);
			}
			else
			{
				if (theApp.EtiqScellement.Etiq_iNbExtract == 0)
				{
					theApp.EtiqScellement.Etiq_bChargementOK = false;
					strMess.Format(_T("%s : Chargement NC, Nombre extractions = %d"), theApp.EtiqScellement.Etiq_strTitre, theApp.EtiqScellement.Etiq_iNbExtract);
					m_ListeResult.InsertString(0, strMess);
					theApp.SetInfosString(strMess);
				}
				else
				{

					strMess.Format(_T("%s : Chargement OK, Nombre extractions = %d"), theApp.EtiqScellement.Etiq_strTitre, theApp.EtiqScellement.Etiq_iNbExtract);
					ChargerFormulaire(theApp.EtiqScellement.Etiq_strCode);
				}
			}
		}
	}
	return 0;
}

bool CDialEtiquette::ChargerFormulaire(CString sFormat)
{
	System::String^ sIDExtraction = gcnew System::String (sFormat);
	if (form_Extract->cls->Data_Trouver(sIDExtraction,false,true))
	{
		if (form_Extract->cls->Data_Lire())
		{
			form_Extract->cls->Frm_Lire();
			form_Extract->cls->Data_SetIndexChamp();
			form_Extract->cls->Frm_SetSMenu();
			form_Extract->cls->Frm_SetForm(form_Extract->pObjet, false);

			System::String^ ssTemp = gcnew System::String (theApp.EtiqTest->Etiq_strTitre);
			form_Extract->cls->Data_Set_sValeur(2,ssTemp);
			form_Extract->cls->Data_Refresh_Valeur(2);
		}
	}
	return true;
}

BOOL CDialEtiquette::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// Reception des boutons du formulaire AcyCL

	// TODO: ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels
		 char szbuf[256];

    ::ZeroMemory(szbuf,256);

    // get the file path from the CopyData structure
    strncpy(szbuf,(char *)pCopyDataStruct->lpData,pCopyDataStruct->cbData);

	CString strFonction = CString(szbuf);
	theApp.ExecuterFonctionExt(strFonction,NULL);

	return CAcyDial::OnCopyData(pWnd, pCopyDataStruct);
}



BOOL CDialEtiquette::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetClikZone(pt) == 4)
	{
		ImageVisu.OnMouseWheel(nFlags, zDelta, pt);
	}

	return CAcyDial::OnMouseWheel(nFlags, zDelta, pt);
}
