//BMx
// DialCameras.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialCameras.h"
#include "Vision.h"
#include "JeuParametres.h"
#include "DialEtiquette.h"
#include "uEye.h"

// Boîte de dialogue CDialCameras

IMPLEMENT_DYNAMIC(CDialCameras, CAcyDial)

CDialCameras::CDialCameras(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialCameras::IDD, pParent)
	//,Menu_Camera("Menu_Camera")
	, Menu_Camera(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_Camera"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
{
   iCurrentCam = ID_CAM_UEYE1;
}

CDialCameras::~CDialCameras()
{
	
}

void CDialCameras::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, Grille_ListeImages);
	DDX_Control(pDX, IDC_STATICMENU_DIALCAMERA, Menu_Camera);
}


BEGIN_MESSAGE_MAP(CDialCameras, CAcyDial)
	ON_WM_DESTROY()
	ON_MESSAGE(MSG_PARAM_CHANGED,OnParamCamChanged)
	ON_NOTIFY(GVN_SELCHANGED,IDC_GRILLE,OnSelImageChanged)
	ON_NOTIFY(GVN_SELCHANGING,IDC_GRILLE,OnSelImageChanging)

	ON_MESSAGE(IS_UEYE_MESSAGE, OnUeyeMessage)
	ON_BN_CLICKED(IDC_RADIO1, &CDialCameras::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDialCameras::OnBnClickedRadio2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RAD_FORCAGE2, &CDialCameras::OnBnClickedRadForcage2)
	ON_BN_CLICKED(IDC_RAD_FORCAGE1, &CDialCameras::OnBnClickedRadForcage1)
	ON_BN_CLICKED(IDC_RAD_FORCAGE3, &CDialCameras::OnBnClickedRadForcage3)
	ON_BN_CLICKED(IDC_RAD_FORCAGE4, &CDialCameras::OnBnClickedRadForcage4)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialCameras
BOOL CDialCameras::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	//Initialisation de la grille des paramétres
	Grille_ListeImages.SetDefCellHeight(25);//hauteur des cellules
	Grille_ListeImages.SetColumnCount(2);

	Grille_ListeImages.SetRowCount(5+1);//+1 pour légende
	Grille_ListeImages.SetFixedRowCount(1);//légende fixe
	Grille_ListeImages.SetFixedColumnCount(1);//Premiére colonne fixe
	Grille_ListeImages.ExpandColumnsToFit(FALSE);
	Grille_ListeImages.SetListMode(TRUE);
	Grille_ListeImages.SetSingleRowSelection(TRUE);
	Grille_ListeImages.SetSingleColSelection(TRUE);
	Grille_ListeImages.SetFixedColumnSelection(FALSE);
	Grille_ListeImages.SetFixedRowSelection(TRUE);
	Grille_ListeImages.SetFixedBkColor(RGB(0,46,184));
	Grille_ListeImages.SetFixedTextColor(RGB(255,255,255));

	//légende
	Grille_ListeImages.SetCellText(0,0,_T("Index"));
	Grille_ListeImages.SetCellText(0,1,_T("Nom"));

	Grille_ListeImages.Refresh();

	//taille et position du static (+ facile a positionner)
	CRect rect;
	GetDlgItem(IDC_EMPLACEMENT_IMG)->GetWindowRect(&rect);
	rect.NormalizeRect();
	ScreenToClient(rect);
	
	//Construction du Menu Dynamique
	Menu_Camera.bVertical = false;
	Menu_Camera.strProfil = theApp.CurrentUser.strProfil;
	Menu_Camera.SendMessage(WM_CREATE,NULL,NULL);	

	//initialisation du pointeur vers la vue pour accés depuis fenêtre ControleCentrageDecoupe
	theApp.UeyeView = this;

	CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO1);
	iCurrentCam = ID_CAM_UEYE1;

	CheckRadioButton(IDC_RAD_FORCAGE1,IDC_RAD_FORCAGE4,IDC_RAD_FORCAGE4);
	theApp.EtiqScellement.iForcageResult = -1;
	theApp.EtiqWolke.iForcageResult = -1;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialCameras::OnDestroy()
{
	CAcyDial::OnDestroy();
	
}
int CDialCameras::MAJ_Grille()
{
	CString strTemp = _T("");

	CGridCellBase * cellule = NULL;
	int iNbImages = 0;

	for (int i = 0; i < NB_IMG_TAB; i++)
	{
		if (Images[i] != NULL)
		{
			if (Images[i]->m_ImgParent == NULL)
			{
				iNbImages ++;
			}
		}
	}

	Grille_ListeImages.SetRowCount(iNbImages + 1);
	int iCurrentLigne = 1;
	COLORREF Color = COLOR_BLANC;

	for (int i = 0; i < NB_IMG_TAB; i++)
	{
		if (Images[i] != NULL)
		{
			if (Images[i]->m_ImgParent == NULL)
			{
				if (Images[i]->IsFenetreOpen)
				{
					Color = COLOR_VERT;
				}
				else
				{
					Color = COLOR_BLANC;
				}

				strTemp.Format(_T("%d"),Images[i]->indexTabImages);
				cellule = Grille_ListeImages.GetCell(iCurrentLigne,0);
				cellule->SetText(strTemp);

				strTemp.Format(_T("%s"),Images[i]->sLibelle);
				cellule = Grille_ListeImages.GetCell(iCurrentLigne,1);
				cellule->SetText(strTemp);
				cellule->SetBackClr(Color);
				
				iCurrentLigne++;
			}
		}
	}

	//Reset de la selection
	Grille_ListeImages.ResetSelectedRange();
	Grille_ListeImages.Refresh();
	return 0;
}
BOOL CDialCameras::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CAcyDial::OnCommand(wParam, lParam);
}


LRESULT CDialCameras::OnParamCamChanged(WPARAM Param1, LPARAM Param2)
{
	JParamGeneral.SauvegarderParametres();				//ATTENTION, lié à l'application générale
	JParamVisionGeneral.SauvegarderParametres();		

	JParamuEye[ID_CAM_UEYE1].SauvegarderParametres();
	JParamuEye[ID_CAM_UEYE2].SauvegarderParametres();


	return 0L;
}
void CDialCameras::OnSelImageChanged(NMHDR* Nmhdr,LRESULT* lRes)
{
	//reglage slider
	lRes;
	NM_GRIDVIEW* infosGrid = (NM_GRIDVIEW*) Nmhdr;

	if(infosGrid->iRow >0)
	{
		CGridCellBase* cellule;
		CString strLibelleImg = _T("");
		cellule = Grille_ListeImages.GetCell(infosGrid->iRow,1);
		strLibelleImg = cellule->GetText();

		int i = infosGrid->iRow;//numero de ligne

		//ouverture fenetre de l'image selectionnée
		for (int i = 0; i < NB_IMG_TAB; i++)
		{
			if (Images[i] != NULL)
			{
				if (Images[i]->m_ImgParent == NULL)
				{
					if (Images[i]->sLibelle == strLibelleImg)
					{
						if (Images[i]->IsFenetreOpen)
						{
							Images[i]->FenetreFermer();
						}
						else
						{
							Images[i]->FenetreOuvrir();
						}

						break;
					}
				}
			}
		}

	
		MAJ_Grille();
	}
}

void CDialCameras::OnSelImageChanging(NMHDR* Nmhdr,LRESULT* lRes)
{
	
}

bool CDialCameras::OpenUeyeCam(int id_Cam,int iAdresseuEye /* = 0*/)
{
	INT nRet = IS_NO_SUCCESS;
	CString messErreur;

	// If we have an open camera, close it
	/*if (uEye[id_Cam].m_hCam != 0)
    {
		is_ExitCamera (uEye[id_Cam].m_hCam);
    }*/ 
	
    nRet = InitUeyeCam(id_Cam , uEye[id_Cam].m_hWndDisplay ,iAdresseuEye);
	if (nRet == IS_SUCCESS)
	{
		// Retrieve original image size
		is_GetSensorInfo(uEye[id_Cam].m_hCam, &uEye[id_Cam].m_sInfo);

        uEye[id_Cam].lX =(long)( uEye[id_Cam].m_sInfo.nMaxWidth);
		uEye[id_Cam].lY =(long)( uEye[id_Cam].m_sInfo.nMaxHeight);

        UpdateData(TRUE);
        nRet = this->InitDisplayMode(id_Cam);

        if (nRet == IS_SUCCESS)
        {
		    // Enable Messages
		    nRet =is_EnableMessage(uEye[id_Cam].m_hCam,    IS_DEVICE_REMOVED,      GetSafeHwnd());
		    nRet =is_EnableMessage(uEye[id_Cam].m_hCam,    IS_DEVICE_RECONNECTED,  GetSafeHwnd());
		    nRet =is_EnableMessage(uEye[id_Cam].m_hCam,    IS_FRAME,               GetSafeHwnd());
            //V53 nRet =is_EnableMessage(uEye[id_Cam].m_hCam,    IS_TRIGGER,             GetSafeHwnd());
            //V54 nRet =is_EnableMessage(uEye[id_Cam].m_hCam,    IS_CAPTURE_STATUS,      GetSafeHwnd());

			//V 44.3 
			//chargement des paramétres du jeu N°1
#if UEYE_VERSION_CODE <= UEYE_VERSION(4, 20, 0)

			is_LoadParameters(uEye[id_Cam].m_hCam, "\\uEye\\set1");
#else
			is_ParameterSet(uEye[id_Cam].m_hCam, IS_PARAMETERSET_CMD_LOAD_EEPROM, "\\uEye\\set1", (UINT)strlen("\\uEye\\set1"));
#endif

//#if UEYE_VERSION_CODE == UEYE_VERSION(4, 70, 0)
//		is_ParameterSet(uEye[id_Cam].m_hCam,IS_PARAMETERSET_CMD_LOAD_EEPROM,"\\uEye\\set1",(UINT)strlen("\\uEye\\set1"));
//#elif UEYE_VERSION_CODE == UEYE_VERSION(4, 30, 0)
//		is_ParameterSet(uEye[id_Cam].m_hCam,IS_PARAMETERSET_CMD_LOAD_EEPROM,"\\uEye\\set1",(UINT)strlen("\\uEye\\set1"));
//#else
//			//chargement des paramétres du jeu N°1
//			is_LoadParameters (uEye[id_Cam].m_hCam,"\\uEye\\set1");
//#endif
		    // Start capture
			//V51
			//nRet =is_CaptureVideo( uEye[id_Cam].m_hCam, IS_WAIT );
			//if( nRet== IS_SUCCESS)
			//{
			//	uEye[id_Cam].m_bLive = TRUE;
			//}
		}
        else
        {
  		    AfxMessageBox(TEXT("Initializing the display mode failed!"), MB_ICONWARNING );
        }

		return true;
	}
	else
	{
		messErreur.Format(TEXT("Ouverture de la Caméra N° %d ECHOUEE "),id_Cam);
	//	theApp.SetInfosString(messErreur);
		return false;
	}

	return true;

}
int CDialCameras::InitDisplayMode(int id_Cam)
{
    INT nRet = IS_NO_SUCCESS;
    
    if (uEye[id_Cam].m_hCam == NULL)
    {
        return IS_NO_SUCCESS;
    }

    if( uEye[id_Cam].m_pcImageMemory != NULL )
    {
        is_FreeImageMem( uEye[id_Cam].m_hCam, uEye[id_Cam].m_pcImageMemory, uEye[id_Cam].m_lMemoryId );
    }
    uEye[id_Cam].m_pcImageMemory = NULL;

	// Set display mode to DIB
    nRet = is_SetDisplayMode(uEye[id_Cam].m_hCam, IS_SET_DM_DIB);
	if (uEye[id_Cam].m_sInfo.nColorMode == IS_COLORMODE_BAYER)
    {
		// Setup the color depth to the current windows setting
        //is_GetColorDepth(uEye[id_Cam].m_hCam, &uEye[id_Cam].m_nBitsPerPixel, &uEye[id_Cam].m_nColorMode);
		// For CBYCRY camera models use RGB24 mode for Matrox
		//V44.3
#if UEYE_VERSION_CODE <= UEYE_VERSION(4, 20, 0)

		uEye[id_Cam].m_nColorMode = IS_SET_CM_RGB24;
#else
		uEye[id_Cam].m_nColorMode = IS_CM_BGR8_PACKED;
#endif
//#if UEYE_VERSION_CODE == UEYE_VERSION(4, 70, 0)
//		uEye[id_Cam].m_nColorMode = IS_CM_BGR8_PACKED;
//#elif UEYE_VERSION_CODE == UEYE_VERSION(4, 30, 0)
//		uEye[id_Cam].m_nColorMode = IS_CM_BGR8_PACKED;
//#else
//		uEye[id_Cam].m_nColorMode = IS_SET_CM_RGB24;
//#endif
        uEye[id_Cam].m_nBitsPerPixel = 24;
		uEye[id_Cam].m_nBytesPerPixel = 3;				// calculate bytes per pixel
    }
    else if (uEye[id_Cam].m_sInfo.nColorMode == IS_COLORMODE_CBYCRY)
    {
        // For CBYCRY camera models use RGB24 mode for Matrox
		//V44.3
#if UEYE_VERSION_CODE <= UEYE_VERSION(4, 20, 0)

		uEye[id_Cam].m_nColorMode = IS_SET_CM_RGB24;
#else
		uEye[id_Cam].m_nColorMode = IS_CM_BGR8_PACKED;
#endif
//#if UEYE_VERSION_CODE == UEYE_VERSION(4, 70, 0)
//		uEye[id_Cam].m_nColorMode = IS_CM_BGR8_PACKED;
//#elif UEYE_VERSION_CODE == UEYE_VERSION(4, 30, 0)
//		uEye[id_Cam].m_nColorMode = IS_CM_BGR8_PACKED;
//#else
//		uEye[id_Cam].m_nColorMode = IS_SET_CM_RGB24;
//#endif
       
        uEye[id_Cam].m_nBitsPerPixel = 24;
		uEye[id_Cam].m_nBytesPerPixel = 3;
    }
    else
    {
		//V44.3
#if UEYE_VERSION_CODE <= UEYE_VERSION(4, 20, 0)

		uEye[id_Cam].m_nColorMode = IS_SET_CM_Y8;
#else
		uEye[id_Cam].m_nColorMode = IS_CM_MONO8;
#endif
//#if UEYE_VERSION_CODE == UEYE_VERSION(4, 70, 0)
//		uEye[id_Cam].m_nColorMode = IS_CM_MONO8;
//#elif UEYE_VERSION_CODE == UEYE_VERSION(4, 30, 0)
//		uEye[id_Cam].m_nColorMode = IS_CM_MONO8;
//#else
//		 uEye[id_Cam].m_nColorMode = IS_SET_CM_Y8;
//#endif
        // For monochrome camera models use Y8 mode
       
        uEye[id_Cam].m_nBitsPerPixel = 8;
		uEye[id_Cam].m_nBytesPerPixel = 1;
    }

    // Allocate an image memory.
    if (is_AllocImageMem(uEye[id_Cam].m_hCam,(int)( uEye[id_Cam].lX),(int)( uEye[id_Cam].lY), uEye[id_Cam].m_nBitsPerPixel, &uEye[id_Cam].m_pcImageMemory, &uEye[id_Cam].m_lMemoryId ) != IS_SUCCESS)
    {
        AfxMessageBox(TEXT("Memory allocation failed!"), MB_ICONWARNING );
    }
    else
    {
        is_SetImageMem( uEye[id_Cam].m_hCam, uEye[id_Cam].m_pcImageMemory, uEye[id_Cam].m_lMemoryId );
    }

    if (nRet == IS_SUCCESS)
    {
        // Set the desired color mode
        is_SetColorMode(uEye[id_Cam].m_hCam, uEye[id_Cam].m_nColorMode);

		//if(pParamDoROI->value==TRUE)
		//{
		//	SetRoi(uEye[id_Cam].m_hCam,0,pParamiZoneDebut->value,uEye[id_Cam].lX,pParamSizeYUtile->value);
		//}
		//else
		//{
		//	//Taille maximale de l'image
		//	SetRoi(uEye[id_Cam].m_hCam,0,0,uEye[id_Cam].lX,uEye[id_Cam].lY);
		//}
    }   

    return nRet;
}

int CDialCameras::InitUeyeCam(int id_Cam,HWND m_hwnd,int iadresse)
{
	INT nRet =0;
	int m_CamCount=0;
	char str[100];
	// get camera count
	is_GetNumberOfCameras ( &m_CamCount );

	if(m_CamCount > MAX_CAM_UEYE )
	{
		sprintf_s( str,"%d camera connected! \n%d cameras will be used, ", m_CamCount, MAX_CAM_UEYE );
		AfxMessageBox( str , MB_ICONEXCLAMATION, 0 );
	}
	
	// init camera
	uEye[id_Cam].m_hCam = iadresse;
	nRet = is_InitCamera (&uEye[id_Cam].m_hCam, m_hwnd);	
	if(uEye[id_Cam].m_hCam > 0)
	{
		int xx=0;	//V51 pour point d'arret uniquement
	}

	if (nRet == IS_STARTER_FW_UPLOAD_NEEDED)
	{
		// Time for the firmware upload = 25 seconds by default
		INT nUploadTime = 25000;
		is_GetDuration (uEye[id_Cam].m_hCam, IS_STARTER_FW_UPLOAD, &nUploadTime);

		CString Str1, Str2, Str3;
		Str1 = "Cette Camera nécessite un nouveau firmware. Cette opération prendra environ";
		Str2 = "secondes. Veuillez patienter ...";
#ifdef _UNICODE
		Str3.Format (L"%s %d %s", Str1, nUploadTime / 1000, Str2);
#else
		Str3.Format ("%s %d %s", Str1, nUploadTime / 1000, Str2);
#endif
		AfxMessageBox (Str3, MB_ICONWARNING);

		// Set mouse to hourglass
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

		// Try again to open the camera. This time we allow the automatic upload of the firmware by
		// specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
		uEye[id_Cam].m_hCam = (HIDS) (((INT)uEye[id_Cam].m_hCam) | IS_ALLOW_STARTER_FW_UPLOAD); 
		nRet = is_InitCamera (&uEye[id_Cam].m_hCam, m_hwnd);   
	}
	else if(nRet != IS_SUCCESS)
	{
		is_ExitCamera(uEye[id_Cam].m_hCam);
	}


return nRet;
}

int CDialCameras::StopGrabUeyeCam(int id_Cam)
{
	// arreter l'acquisition
	if( uEye[id_Cam].m_hCam != 0 )
	{
		// Disable messages
        is_EnableMessage(uEye[id_Cam].m_hCam,    IS_DEVICE_REMOVED,      NULL);
		is_EnableMessage(uEye[id_Cam].m_hCam,    IS_DEVICE_RECONNECTED,  NULL);
		is_EnableMessage(uEye[id_Cam].m_hCam,    IS_FRAME,               NULL );
        is_EnableMessage(uEye[id_Cam].m_hCam,    IS_TRIGGER,             NULL );

		// Stop live video
		is_StopLiveVideo( uEye[id_Cam].m_hCam, IS_WAIT );
		
		// Free the allocated buffer
		if( uEye[id_Cam].m_pcImageMemory != NULL )
        {
#ifdef _SIMUL
			free(uEye[id_Cam].m_pcImageMemory);
#endif
  			is_FreeImageMem( uEye[id_Cam].m_hCam, uEye[id_Cam].m_pcImageMemory, uEye[id_Cam].m_lMemoryId );
        }

		uEye[id_Cam].m_pcImageMemory = NULL;
		
		// Close camera
		int iErreur = 0;
		iErreur = is_ExitCamera( uEye[id_Cam].m_hCam );
		if(iErreur != IS_SUCCESS)
		{
			CString strMessage;
			strMessage.Format(_T("Erreur N° %d lors de la fermeture de la caméra %d"),iErreur,id_Cam);
			AfxMessageBox(strMessage,MB_OK|MB_ICONERROR);
		}

        uEye[id_Cam].m_hCam = NULL;
		return 1;
	}
	else
	{
		return 0;
	}
}


LRESULT CDialCameras::OnUeyeMessage(WPARAM wParam, LPARAM lParam)
{
	HIDS idCam = (HIDS )lParam;
	int id_Cam = ID_CAM_UEYE1;
	CString strTemp;
	bool bCamOK = false;
	CString strForListe=_T("");

	//V53
	strForListe.Format("MESSAGE Ueye IDCam=%ld  Message=%ld  ",(long)lParam,(long)wParam);
	((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strForListe);

	//selection de la camera
	for(id_Cam = 0; id_Cam < MAX_CAM_UEYE;id_Cam ++)
	{
		//V51 if(uEye[id_Cam].iAdresseuEye == idCam) break;
		if(uEye[id_Cam].m_hCam == idCam)
		{
			bCamOK = true;
			break;
		}
	}
	if(!bCamOK)
	{
		return 0;
	}
	
	switch ( wParam )
	{
		case IS_DEVICE_REMOVED:
			//V51
			if(id_Cam == ID_CAM_UEYE1)
			{
				theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM3].NewValue = 1;
			}
			if(id_Cam == ID_CAM_UEYE2)
			{
				theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM4].NewValue = 1;
			}
			break;
		  case IS_DEVICE_RECONNECTED:
			//V51
			if(id_Cam == ID_CAM_UEYE1)
			{
				theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM3].NewValue = 0;
			}
			if(id_Cam == ID_CAM_UEYE2)
			{
				theApp.m_GestAlarmes.Alarme[iIndex_ALARME_CAM4].NewValue = 0;
			}

			//Application paramètres sur reconnexion camera
			uEye[id_Cam].AppliquerParametres();
			break;

		  case IS_FRAME:
				//affichage de l'image reçue à l'écran
				// + Recherche Lisiére
				if( uEye[id_Cam].m_pcImageMemory != NULL )
				{
					if (uEye[id_Cam].iSnap!= 0)
					{
						TRACE("Acquisition avec Copie en cours !!!\n");
					}

					if(uEye[id_Cam].imgGrabuEye.milBuf != 0)
					{
					
						uEye[id_Cam].bFromFile = false;
						uEye[id_Cam].iSnap = 2;
					}

					/*Affichage de l'image cumul sur DialMachine
					else if(theApp.MachineView->IsWindowVisible())
					{
						HWND HwndDisp = NULL;

						if(id_Cam == ID_CAM)
						{
							HwndDisp = ((CDialMachine * )theApp.MachineView)->static_ImgCAM0.GetSafeHwnd();
						}
						
						if(HwndDisp != NULL)
							is_RenderBitmap( uEye[id_Cam].m_hCam, uEye[id_Cam].m_lMemoryId, HwndDisp, m_nRenderMode );
					}*/
					
					/*::Calcul du Nbr D'img/sec
					time_t maintenant;
					tm tmSec;

					time(&maintenant);
					::_localtime64_s(&tmSec ,&maintenant);

					if(tmSec.tm_sec == PrevSec)
					{
						NbImgSec++;
					}
					else
					{
						Affichage
						CString strTemp;
						strTemp.Format("NB Img/sec = %d",NbImgSec);
						theApp.SetInfosString(strTemp);
						NbImgSec = 0;
						PrevSec = tmSec.tm_sec;
					}*/
					
				}
				
			 break;

		  case IS_TRIGGER:
			strForListe=_T("");
			strForListe.Format("TRIGGER camera %d",id_Cam);
			((CDialEtiquette *)theApp.EtiqView)->m_ListeResult.InsertString(0,strForListe);
			  break;

	  }    
	  return 0;
}


bool CDialCameras::CamAllocMem(int id_Cam)
{
	bool bReturn = false;
  
	// free memory
	if( uEye[id_Cam].m_pcImageMemory != NULL )
	{
		is_FreeImageMem( uEye[id_Cam].m_hCam, uEye[id_Cam].m_pcImageMemory, uEye[id_Cam].m_lMemoryId);
		uEye[id_Cam].m_pcImageMemory = NULL;
		uEye[id_Cam].m_lMemoryId = 0;
	}
  
  
	// main memory allocate and activate
	m_Ret = is_AllocImageMem(	uEye[id_Cam].m_hCam,
								uEye[id_Cam].lX,
								uEye[id_Cam].lY,
								uEye[id_Cam].m_nBitsPerPixel,
								&uEye[id_Cam].m_pcImageMemory,
								&uEye[id_Cam].m_lMemoryId);
	if( m_Ret == IS_SUCCESS )
	{
		is_SetImageMem(uEye[id_Cam].m_hCam, uEye[id_Cam].m_pcImageMemory, uEye[id_Cam].m_lMemoryId );// set memory active

		int iTempX = 0;
		int iTempY = 0;
		is_InquireImageMem (uEye[id_Cam].m_hCam, uEye[id_Cam].m_pcImageMemory, uEye[id_Cam].m_lMemoryId, &iTempX, &iTempY, &uEye[id_Cam].m_nBitsPerPixel, &uEye[id_Cam].m_nPitch );	// read back for calculation
		uEye[id_Cam].lX = (long)(iTempX);
		uEye[id_Cam].lY = (long)(iTempY);

		uEye[id_Cam].m_nImageSize = uEye[id_Cam].lX * uEye[id_Cam].lY * uEye[id_Cam].m_nBytesPerPixel;	// image size [bytes]
		
		bReturn = true;
	}
	else
	{
		bReturn = false;
	}

	return bReturn;

}

void CDialCameras::AcquisitionSoftTriggerCam(int id_Cam)
{

	if(uEye[id_Cam].m_bLive == TRUE)
	{
		is_StopLiveVideo(uEye[id_Cam].m_hCam, IS_WAIT);
		uEye[id_Cam].m_bLive = FALSE;
	}

	is_SetExternalTrigger(uEye[id_Cam].m_hCam, IS_SET_TRIGGER_SOFTWARE);
}

int CDialCameras::DoSoftwareTrigger(int id_Cam)
{
	
		/*is_SetExternalTrigger(uEye[id_Cam].m_hCam, IS_SET_TRIGGER_OFF);
	
if(uEye[id_Cam].m_bLive == TRUE)
	{
		is_CaptureVideo(uEye[id_Cam].m_hCam, IS_DONT_WAIT);
	}*/

	is_FreezeVideo(uEye[id_Cam].m_hCam,IS_DONT_WAIT);

	return 0;
}

char CDialCameras::GetMemoryPixel(int x, int y,int Id_cam)
{
	return(uEye[Id_cam].m_pcImageMemory[y * uEye[Id_cam].lX+x]);
}


int CDialCameras::SetRoi(int id_Cam, int x, int y, int dx, int dy)
{
	// Set the image size to capture
	IS_RECT imageSize;
	imageSize.s32X = x | IS_AOI_IMAGE_POS_ABSOLUTE;//on garde la position dans le plan memoire
	imageSize.s32Y = y | IS_AOI_IMAGE_POS_ABSOLUTE;//on garde la position dans le plan memoire
	imageSize.s32Width = dx;
	imageSize.s32Height = dy;

	return (int)is_AOI(id_Cam, IS_AOI_IMAGE_SET_AOI, (void*)&imageSize, sizeof(imageSize));
}

bool CDialCameras::SetRoi_AutoContrast(int id_cam,PARAM_ROI * Roi)
{
	//Set Roi pour AutoContrast
	IS_RECT rectAOI;
	rectAOI.s32X     = Roi->X1 ;
	rectAOI.s32Y     = Roi->Y1 ;
	rectAOI.s32Width = Roi->dX;
	rectAOI.s32Height = Roi->dY;

	INT nRet = is_AOI( uEye[id_cam].m_hCam, IS_AOI_AUTO_BRIGHTNESS_SET_AOI, (void*)&rectAOI, sizeof(rectAOI));
	
	if(nRet == IS_SUCCESS)
		return true;
	else
		return false;
}

bool CDialCameras::Set_AutoContrast(int id_cam,bool bEnable)
{
	//Activation AutoContrast
	double dEnable = 0;
	int iErreur = 0;
	if(bEnable)
		dEnable = 1;
	else
		dEnable = 0;

	iErreur = is_SetAutoParameter (uEye[id_cam].m_hCam, IS_SET_ENABLE_AUTO_SHUTTER, &dEnable , 0 );

	if(iErreur == IS_SUCCESS)
		return true;
	else
		return false;
}

void CDialCameras::OnBnClickedRadio1()
{
	iCurrentCam = ID_CAM_UEYE1;
}


void CDialCameras::OnBnClickedRadio2()
{
	iCurrentCam = ID_CAM_UEYE2;
}
void  CDialCameras::ExecuterFonctionExt(CString strFonction,void * BoutonParent)
{
	CImageButtonWithStyle * ParentBtn = (CImageButtonWithStyle * )BoutonParent;

	if(strFonction == "CAM_FLASH")
	{
		if(uEye[iCurrentCam].Param_Camera.Flash == IO_FLASH_MODE_CONSTANT_HIGH)
		{
			uEye[iCurrentCam].SetFlash(IO_FLASH_MODE_CONSTANT_LOW);
		}
		else
		{
			uEye[iCurrentCam].SetFlash(IO_FLASH_MODE_CONSTANT_HIGH);
		}
	}
	else if(strFonction == "CAM_SNAP")
	{
		uEye[iCurrentCam].Snap();
	}
	else if(strFonction == "CAM_FREERUN")
	{
		uEye[iCurrentCam].Freerun();
	}
	else if(strFonction == "CAM_TRIGGER")
	{
		uEye[iCurrentCam].AppliquerParametres();
	}
	else if(strFonction == "CAM_SAVEON")
	{
		uEye[iCurrentCam].bSave = true;
		ParentBtn->iFonction = 1;
		((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();	
	}
	else if(strFonction == "CAM_SAVEOFF")
	{
		uEye[iCurrentCam].bSave = false;
		ParentBtn->iFonction = 0;
		((CStaticMenu *)ParentBtn->MenuParent)->UpdateMenu();	
	}
	else if(strFonction == "CAM_FROMFILE")
	{
		//Traitement sur une image de test
		CFileDialog* file_dlg= new CFileDialog(TRUE);
		file_dlg->m_ofn.lpstrTitle=_T("Selection Fichier Image ");
		file_dlg->m_ofn.lpstrFilter=_T("Fichiers Images (*.bmp)\0*.bmp\0Tous les Fichier (*.*)\0*.*\0\0");
		INT_PTR resultdlg=file_dlg->DoModal();

		if(resultdlg==IDOK)
		{
			CString strFileToCopy = file_dlg->GetPathName();
			MIL_TEXT_PTR TxtTemp;
			TxtTemp = strFileToCopy.GetBuffer();
			
			uEye[iCurrentCam].m_sFichier = TxtTemp;
			uEye[iCurrentCam].bFromFile = true;

			uEye[iCurrentCam].iIndexSeqFile = -1;
		}
		else
		{
			uEye[iCurrentCam].bFromFile = false;
		}
		delete (file_dlg);
		
	}
	else if(strFonction == "CAM_FROMSEQUENCE")
	{
		//Traitement sur une image de test
		CFileDialog* file_dlg= new CFileDialog(TRUE);
		file_dlg->m_ofn.lpstrTitle=_T("Selection Fichier Image ");
		file_dlg->m_ofn.lpstrFilter=_T("Fichiers Images (*.bmp)\0*.bmp\0Tous les Fichier (*.*)\0*.*\0\0");
		INT_PTR resultdlg=file_dlg->DoModal();

		if(resultdlg==IDOK)
		{
			CString strFileToCopy = file_dlg->GetPathName();
			//recherche du Numero de l'image

			CString strTemp;
			CString strDossier;

			int iIndex = strFileToCopy.ReverseFind(_T('\\'));
			int iIndexC = strFileToCopy.ReverseFind(_T('c'));
			int iIndexPoint = strFileToCopy.ReverseFind(_T('.'));

			strDossier = strFileToCopy.Left(iIndex );

			strTemp = strFileToCopy.Mid(iIndexC + 1 , (iIndexPoint - (iIndexC + 1)));

			uEye[iCurrentCam].StrFileSeq.RemoveAll();

			//Recherche de tous les fichier *.bmp
			int iIndexStr = 0;
			bool bContinue = true;
			CString strTemp2;
			
			CString strSearch = _T("");
			strSearch.Format(_T("%s\\*.bmp"),strDossier);
			CFileFind f; 
			BOOL bMoreFiles = f.FindFile(strSearch); 
			while (bMoreFiles) 
			{ 
				bMoreFiles = f.FindNextFile(); 
				if (!f.IsDirectory()) 
				{
					strTemp2.Format(_T("%s\\%s"),strDossier,f.GetFileName());
					
					uEye[iCurrentCam].StrFileSeq.SetAtGrow(iIndexStr,strTemp2);
					iIndexStr ++;
				}
			}
			
			uEye[iCurrentCam].m_sFichier = strFileToCopy;
			uEye[iCurrentCam].bFromFile = true;
			uEye[iCurrentCam].iSnap = 1;

			uEye[iCurrentCam].iIndexSeqFile = 0;
			
		}

		delete (file_dlg);
		
	}
	else if(strFonction == "CAM_IMAGES")
	{
		if(uEye[iCurrentCam].IsFenetreOpened)
		{
			uEye[iCurrentCam].imgGrabuEye.FenetreFermer();
			uEye[iCurrentCam].IsFenetreOpened = false;
		}
		else
		{
			uEye[iCurrentCam].imgGrabuEye.FenetreOuvrir();
			uEye[iCurrentCam].IsFenetreOpened = true;
		}

	}
}

void CDialCameras::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_RECONNECT_UEYE1)
	{
		KillTimer(ID_TIMER_RECONNECT_UEYE1);
		
		if(theApp.Process.iEtat == ETAT_PROCESS_PRODUCTIONMARCHE)
		{
			//V51 : Pas de reconnexion en mode production pour éviter les erreurs
			SetTimer(ID_TIMER_RECONNECT_UEYE1,1000,NULL);
		}
		else
		{
			bool bOpenCam = false;
			bOpenCam = ((CDialCameras * )theApp.UeyeView)->OpenUeyeCam(uEye[ID_CAM_UEYE1].m_iNumuEye,uEye[ID_CAM_UEYE1].iAdresseuEye);

			if(!bOpenCam)
			{
				uEye[ID_CAM_UEYE1].m_hCam = -1;
				uEye[ID_CAM_UEYE1].m_nBytesPerPixel = 1;

				SetTimer(ID_TIMER_RECONNECT_UEYE1,1000,NULL);
			}
			else
			{
				if(uEye[ID_CAM_UEYE1].m_nBytesPerPixel >=3)
				{
					uEye[ID_CAM_UEYE1].imgGrabuEye.Decharger();
					uEye[ID_CAM_UEYE1].imgGrabuEye/*[i]*/.Charger(uEye[ID_CAM_UEYE1].milSys,uEye[ID_CAM_UEYE1].lX,uEye[ID_CAM_UEYE1].lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
				}
			}
		}
	}
	else if (nIDEvent == ID_TIMER_RECONNECT_UEYE2)
	{
		KillTimer(ID_TIMER_RECONNECT_UEYE2);

		if(theApp.Process.iEtat == ETAT_PROCESS_PRODUCTIONMARCHE)
		{
			//V51 : Pas de reconnexion en mode production pour éviter les erreurs
			SetTimer(ID_TIMER_RECONNECT_UEYE2,1000,NULL);
		}
		else
		{
			bool bOpenCam = false;
			bOpenCam = ((CDialCameras * )theApp.UeyeView)->OpenUeyeCam(uEye[ID_CAM_UEYE2].m_iNumuEye,uEye[ID_CAM_UEYE2].iAdresseuEye);

			if(!bOpenCam)
			{
				uEye[ID_CAM_UEYE2].m_hCam = -1;
				uEye[ID_CAM_UEYE2].m_nBytesPerPixel = 1;

				SetTimer(ID_TIMER_RECONNECT_UEYE2,1000,NULL);
			}
			else
			{
				if(uEye[ID_CAM_UEYE2].m_nBytesPerPixel >=3)
				{
					uEye[ID_CAM_UEYE2].imgGrabuEye.Decharger();
					uEye[ID_CAM_UEYE2].imgGrabuEye/*[i]*/.Charger(uEye[ID_CAM_UEYE2].milSys,uEye[ID_CAM_UEYE2].lX,uEye[ID_CAM_UEYE2].lY,3,8+M_UNSIGNED,M_IMAGE+M_PROC+M_DISP);
				}
			}
		}
	}

	CAcyDial::OnTimer(nIDEvent);
}
void CDialCameras::OnBnClickedRadForcage1()
{
	//Res OK
	theApp.EtiqScellement.iForcageResult = 0;
}
void CDialCameras::OnBnClickedRadForcage2()
{
	//Res NOK
	theApp.EtiqScellement.iForcageResult = 1;
}
void CDialCameras::OnBnClickedRadForcage3()
{
	//Res MIXUP
	theApp.EtiqScellement.iForcageResult = 2;
}
void CDialCameras::OnBnClickedRadForcage4()
{
	//Res Vision
	theApp.EtiqScellement.iForcageResult = -1;
}
