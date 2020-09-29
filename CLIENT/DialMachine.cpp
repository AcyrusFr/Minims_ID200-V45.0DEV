// DialMachine.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialMachine.h"
#include "DialPesons.h"
#include "DialDefautPiste.h"

// Boîte de dialogue CDialMachine

IMPLEMENT_DYNAMIC(CDialMachine, CAcyDial)

CDialMachine::CDialMachine(CWnd* pParent /*=NULL*/)
	: CAcyDial(CDialMachine::IDD, pParent)
	,voy_robot1(&theApp.iAffVoyant[INDEX_VOY_ROBOT1],"R1")
	,voy_robot2(&theApp.iAffVoyant[INDEX_VOY_ROBOT2],"R2")
	,voy_robot3(&theApp.iAffVoyant[INDEX_VOY_ROBOT3],"R3")
	,voy_Opc(&theApp.iAffVoyant[INDEX_VOY_OPC],"COM")
	,voy_peson1(&theApp.iAffVoyant[INDEX_VOY_PESON1],"P11")
	,voy_peson2(&theApp.iAffVoyant[INDEX_VOY_PESON2],"P12")
	,voy_peson3(&theApp.iAffVoyant[INDEX_VOY_PESON3],"P21")
	,voy_peson4(&theApp.iAffVoyant[INDEX_VOY_PESON4],"P22")
	,voy_peson5(&theApp.iAffVoyant[INDEX_VOY_PESON5],"P31")
	,voy_peson6(&theApp.iAffVoyant[INDEX_VOY_PESON6],"P32")
	,voy_DMX1(&theApp.iAffVoyant[INDEX_VOY_DMXETUI],"DM1")
	,voy_DMX2(&theApp.iAffVoyant[INDEX_VOY_DMXNOTICE1],"DM2")
	,voy_DMX3(&theApp.iAffVoyant[INDEX_VOY_DMXNOTICE2],"DM3")
	,voy_Cam1(&theApp.iAffVoyant[INDEX_VOY_CAM1],"CAM")
	,voy_camUV(&theApp.iAffVoyant[INDEX_VOY_CAMUV],"UV")
	,voy_pesage(&theApp.iAffVoyant[INDEX_VOY_PESAGE],"PES")
	//,MenuDialMachine("Menu_DialMachine")
	, MenuDialMachine(&ExecuterFonctionExtGeneral, &theBase.dao_Base, _T("Menu_DialMachine"), CODE_UTILISATEUR_DEFAUT, &theConfig.str_RepertoireApp, &theConfig.str_RepertoireData, &theConfig.str_RepertoireBase, &theConfig.str_RepertoireApp)
	//,voy_Cam3(&theApp.iAffVoyant[INDEX_VOY_CAM3],"C3")
	//,voy_Cam4(&theApp.iAffVoyant[INDEX_VOY_CAM4],"C4")
	//,voy_wolke(&theApp.iAffVoyant[INDEX_VOY_WOLKE],"WOL")
	//,voy_etiquette(&theApp.iAffVoyant[INDEX_VOY_ETIQUETTE],"ETQ")
	//V 44.0 ,voy_CamBoite(&theApp.iAffVoyant[INDEX_VOY_CAMBOITE],"CB")
	,voy_ImpWolke(&theApp.iAffVoyant[INDEX_VOY_IMPWOLKE],"WI")
	,voy_ImpScellement(&theApp.iAffVoyant[INDEX_VOY_IMPCELLEMENT],"SI")
	,voy_LectWolke(&theApp.iAffVoyant[INDEX_VOY_LECTWOLKE],"WL")
	,voy_LectScellement(&theApp.iAffVoyant[INDEX_VOY_LECTCELLEMENT],"SL")
{
	//V 44.8 grande resolution
	if (pParamModeAffichageResolution->value == 1)
	{
		voy_robot1.lSizeFont = 20;
		voy_robot2.lSizeFont = 20;
		voy_robot3.lSizeFont = 20;
		voy_Opc.lSizeFont = 20;
		voy_peson1.lSizeFont = 20;
		voy_peson2.lSizeFont = 20;
		voy_peson3.lSizeFont = 20;
		voy_peson4.lSizeFont = 20;
		voy_peson5.lSizeFont = 20;
		voy_peson6.lSizeFont = 20;
		voy_DMX1.lSizeFont = 20;
		voy_DMX2.lSizeFont = 20;
		voy_DMX3.lSizeFont = 20;
		voy_Cam1.lSizeFont = 20;
		voy_camUV.lSizeFont = 20;
		voy_pesage.lSizeFont = 20;
		voy_ImpWolke.lSizeFont = 20;
		voy_ImpScellement.lSizeFont = 20;
		voy_LectWolke.lSizeFont = 20;
		voy_LectScellement.lSizeFont = 20;
	}

}

CDialMachine::~CDialMachine()
{
}

void CDialMachine::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, GrilleEtats);
	DDX_Control(pDX, IDC_VOYANT_ROBOT1, voy_robot1);
	DDX_Control(pDX, IDC_VOYANT_ROBOT2, voy_robot2);
	DDX_Control(pDX, IDC_VOYANT_ROBOT3, voy_robot3);
	DDX_Control(pDX, IDC_VOYANT_COM, voy_Opc);
	DDX_Control(pDX, IDC_VOYANT_PESON1, voy_peson1);
	DDX_Control(pDX, IDC_VOYANT_PESON2, voy_peson2);
	DDX_Control(pDX, IDC_VOYANT_PESON3, voy_peson3);
	DDX_Control(pDX, IDC_VOYANT_PESON4, voy_peson4);
	DDX_Control(pDX, IDC_VOYANT_PESON5, voy_peson5);
	DDX_Control(pDX, IDC_VOYANT_PESON6, voy_peson6);
	DDX_Control(pDX, IDC_VOYANT_DMX1, voy_DMX1);
	DDX_Control(pDX, IDC_VOYANT_DMX2, voy_DMX2);
	DDX_Control(pDX, IDC_VOYANT_DMX3, voy_DMX3);
	DDX_Control(pDX, IDC_VOYANT_ROBOTM10, voy_Cam1);
	DDX_Control(pDX, IDC_STATICMENU_DIALMACHINE, MenuDialMachine);
	DDX_Control(pDX, IDC_TEXT_DESCRIPTION, TextDescription);
	DDX_Control(pDX, IDC_TEXT_OF, TextOf);
	DDX_Control(pDX, IDC_TEXT_ARTICLE, TextArticle);
	DDX_Control(pDX, IDC_TEXT_LOT, TextLot);
	DDX_Control(pDX, IDC_STATIC_DESCRIPTION, static_description);
	DDX_Control(pDX, IDC_STATIC_OF, static_of);
	DDX_Control(pDX, IDC_STATIC_ARTICLE, static_article);
	DDX_Control(pDX, IDC_STATIC_LOT, static_lot);
	DDX_Control(pDX, IDC_LIST_DEFMACHINE, ListDefMachine);
	DDX_Control(pDX, IDC_VOYANT_CAM2, voy_camUV);
	DDX_Control(pDX, IDC_LIST1, Liste_Alertes);
	DDX_Control(pDX, IDC_VOYANT_PESAGE, voy_pesage);
	//DDX_Control(pDX, IDC_VOYANT_CAM3, voy_Cam3);
	//DDX_Control(pDX, IDC_VOYANT_CAM4, voy_Cam4);
	//DDX_Control(pDX, IDC_VOYANT_WOLKE, voy_wolke);
	//DDX_Control(pDX, IDC_VOYANT_ETIQUETTE, voy_etiquette);
//V 44.0	DDX_Control(pDX, IDC_VOYANT_CAMBOITE, voy_CamBoite);			//V37
	DDX_Control(pDX, IDC_VOYANT_IMPWOLKE, voy_ImpWolke);			//V37
	DDX_Control(pDX, IDC_VOYANT_IMPCELLEMENT, voy_ImpScellement);	//V37
	DDX_Control(pDX, IDC_VOYANT_LECTWOLKE, voy_LectWolke);			//V37
	DDX_Control(pDX, IDC_VOYANT_LECTCELLEMENT, voy_LectScellement);	//V37

	DDX_Control(pDX, IDC_TEXT_CADENCE, TextCadence);			//V16
	DDX_Control(pDX, IDC_STATIC_CADENCE, static_cadence);

	//V 44.8 grande resolution
	DDX_Control(pDX, IDC_STATIC_CAD_VUS, static_Cadence_Vues);
	DDX_Control(pDX, IDC_STATIC_CAD_PRIS, static_Cadence_Pris);
	DDX_Control(pDX, IDC_STATIC_CAD_RATIO, static_Cadence_Ratio);
	DDX_Control(pDX, IDC_STATIC_CAD_PISTE1, static_Cadence_Piste[0]);
	DDX_Control(pDX, IDC_STATIC_CAD_PISTE2, static_Cadence_Piste[1]);
	DDX_Control(pDX, IDC_STATIC_CAD_PISTE3, static_Cadence_Piste[2]);
	DDX_Control(pDX, IDC_STATIC_CAD_P1_VUS, Textcadence_Vues[0]);
	DDX_Control(pDX, IDC_STATIC_CAD_P2_VUS, Textcadence_Vues[1]);
	DDX_Control(pDX, IDC_STATIC_CAD_P3_VUS, Textcadence_Vues[2]);
	DDX_Control(pDX, IDC_STATIC_CAD_P1_PRIS, Textcadence_Pris[0]);
	DDX_Control(pDX, IDC_STATIC_CAD_P2_PRIS, Textcadence_Pris[1]);
	DDX_Control(pDX, IDC_STATIC_CAD_P3_PRIS, Textcadence_Pris[2]);
	DDX_Control(pDX, IDC_STATIC_CAD_P1_RATIO, Textcadence_Ratio[0]);
	DDX_Control(pDX, IDC_STATIC_CAD_P2_RATIO, Textcadence_Ratio[1]);
	DDX_Control(pDX, IDC_STATIC_CAD_P3_RATIO, Textcadence_Ratio[2]);
}


BEGIN_MESSAGE_MAP(CDialMachine, CAcyDial)
	ON_WM_TIMER()
	//ON_MESSAGE(WM_MAJ_COMPTEURS,OnMAJCompteurs)
	ON_MESSAGE(WM_MAJ_ETAT,OnMAJEtat)
	ON_MESSAGE(WM_MAJ_PREVDIALOG,OnMAJPrevDialog)
	ON_MESSAGE(WM_MAJ_VIEWMACHINE,OnMAJViewMachine)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialMachine

BOOL CDialMachine::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	/*m_Menu.strProfil = theApp.CurrentUser.strProfil;
	m_Menu.SendMessage(WM_CREATE,NULL,NULL);*/
	TextDescription.strText = _T("");
	TextOf.strText = _T("");
	TextArticle.strText = _T("");
	TextLot.strText = _T("");
	TextCadence.strText = _T("0");

	TextDescription.bSautDeLigne = true;
	
	TextDescription.CouleurText = RGB(0,0,255);
	TextOf.CouleurText = RGB(0,0,255);
	TextArticle.CouleurText = RGB(0,0,255);
	TextLot.CouleurText = RGB(0,0,255);

	//V 44.7
	if (pParamCadence_MinMachine->value > 0)
	{
		//Control cadence actif => couleur Rouge/Vert
		TextCadence.CouleurText = RGB(0, 255, 0);
	}
	else
	{
		//Control cadence inactif => couleur Bleu
		TextCadence.CouleurText = RGB(0, 0, 255);
	}
	

	TextDescription.Invalidate();
	TextOf.Invalidate();
	TextArticle.Invalidate();
	TextLot.Invalidate();
	TextCadence.Invalidate();

	CString strTemp = _T("");
	static_description.GetWindowText(strTemp);
	static_description.strText = strTemp;
	static_of.GetWindowText(strTemp);
	static_of.strText = strTemp;
	static_article.GetWindowText(strTemp);
	static_article.strText = strTemp;
	static_lot.GetWindowText(strTemp);
	static_lot.strText = strTemp;
	static_cadence.GetWindowText(strTemp);
	static_cadence.strText = strTemp;

	static_description.iSizePolice = 15;
	static_of.iSizePolice = 15;
	static_article.iSizePolice = 15;
	static_lot.iSizePolice = 15;
	static_cadence.iSizePolice = 15;

	static_description.bSouligne = false;
	static_of.bSouligne = false;
	static_article.bSouligne = false;
	static_lot.bSouligne = false;
	static_cadence.bSouligne = false;

	static_description.Invalidate();
	static_of.Invalidate();
	static_article.Invalidate();
	static_lot.Invalidate();
	static_cadence.Invalidate();

	

	switch (pParamModeAffichageResolution->value)
	{
	case 0:// petite resolution : affichage sur dialogue vision
	default:

		//masquage des champs de la dialogue principale
		static_Cadence_Vues.ShowWindow(SW_HIDE);
		static_Cadence_Pris.ShowWindow(SW_HIDE);
		static_Cadence_Ratio.ShowWindow(SW_HIDE);
		for (int i = 0; i < 3; i++)
		{
			static_Cadence_Piste[i].ShowWindow(SW_HIDE);

			Textcadence_Vues[i].ShowWindow(SW_HIDE);
			Textcadence_Pris[i].ShowWindow(SW_HIDE);
			Textcadence_Ratio[i].ShowWindow(SW_HIDE);
		}

		break;

	case 1:// Grande resolution : affichage sur dialogue Machine
		//V 44.8 grande Resolution
		static_Cadence_Vues.strText = _T("Vus (Minims/min)");
		static_Cadence_Pris.strText = _T("Pris (Minims/min)");
		static_Cadence_Ratio.strText = _T("Ratio (%)");
		for (int i = 0; i < 3; i++)
		{
			static_Cadence_Piste[i].strText.Format(_T("Piste %d :"), (i + 1));

			Textcadence_Vues[i].strText = _T("0");
			Textcadence_Pris[i].strText = _T("0");
			Textcadence_Ratio[i].strText = _T("0");
		}

		static_Cadence_Vues.iSizePolice = 15;
		static_Cadence_Pris.iSizePolice = 15;
		static_Cadence_Ratio.iSizePolice = 15;
		for (int i = 0; i < 3; i++)
		{
			static_Cadence_Piste[i].iSizePolice = 15;

			Textcadence_Vues[i].iSizePolice = 20;
			Textcadence_Pris[i].iSizePolice = 20;
			Textcadence_Ratio[i].iSizePolice = 20;
		}
		static_Cadence_Ratio.bSouligne = false;
		static_Cadence_Vues.bSouligne = false;
		static_Cadence_Pris.bSouligne = false;
		for (int i = 0; i < 3; i++)
		{
			static_Cadence_Piste[i].bSouligne = false;

			Textcadence_Vues[i].bSouligne = false;
			Textcadence_Pris[i].bSouligne = false;

			Textcadence_Vues[i].CouleurText = RGB(0, 0, 255);
			Textcadence_Pris[i].CouleurText = RGB(0, 0, 255);
			Textcadence_Ratio[i].CouleurText = RGB(0, 0, 255);
		}

		//affichage des champs
		static_Cadence_Vues.ShowWindow(SW_SHOW);
		static_Cadence_Pris.ShowWindow(SW_SHOW);
		static_Cadence_Ratio.ShowWindow(SW_HIDE);
		for (int i = 0; i < 3; i++)
		{
			static_Cadence_Piste[i].ShowWindow(SW_SHOW);

			Textcadence_Vues[i].ShowWindow(SW_SHOW);
			Textcadence_Pris[i].ShowWindow(SW_SHOW);
			Textcadence_Ratio[i].ShowWindow(SW_HIDE);
		}

		//refresh
		static_Cadence_Vues.Invalidate();
		static_Cadence_Pris.Invalidate();
		static_Cadence_Ratio.Invalidate();
		for (int i = 0; i < 3; i++)
		{
			static_Cadence_Piste[i].Invalidate();

			Textcadence_Vues[i].Invalidate();
			Textcadence_Pris[i].Invalidate();
			Textcadence_Ratio[i].Invalidate();
		}
		break;
	}


	// Initialisation de la grille des Etats
	GrilleEtats.SetDefCellHeight(25);//hauteur des cellules
	GrilleEtats.SetColumnCount(2);
	GrilleEtats.SetFixedRowCount(1);//légende fixe
	GrilleEtats.SetRowCount(13);		//V1.30 Ajout Tache UV
	GrilleEtats.SetColumnResize(FALSE);
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
	cellule->SetText(_T("Compteur"));
	cellule = GrilleEtats.GetCell(0,1);
	cellule->SetText(_T("Valeur"));

	GrilleEtats.SetItemText(1,0,_T("Boites complètes"));
	GrilleEtats.SetItemText(2,0,_T("Boites prélevées"));
	GrilleEtats.SetItemText(3,0,_T("Boites éjectées"));

	GrilleEtats.SetItemText(4,0,_T("Minims emballés"));
	GrilleEtats.SetItemText(5,0,_T("Minims éjectés"));
	GrilleEtats.SetItemText(6,0,_T("Rebuts vision"));
	GrilleEtats.SetItemText(7,0,_T("Rebuts poids"));
	GrilleEtats.SetItemText(8,0,_T("Vision OK"));
	GrilleEtats.SetItemText(9,0,_T("Vision DMX"));
	GrilleEtats.SetItemText(10,0,_T("Vision Tache"));
	GrilleEtats.SetItemText(11,0,_T("Vision TacheUV"));
	GrilleEtats.SetItemText(12,0,_T("Vision CB"));

	
	MAJGrilleComptage();

	//Construction du Menu Dynamique
	MenuDialMachine.bVertical = true;

	MenuDialMachine.strProfil = theApp.CurrentUser.strProfil;
	MenuDialMachine.SendMessage(WM_CREATE,NULL,NULL);	
	
	// A VOIR lancement du timer process

	theApp.TimerAlive = this->SetTimer(ID_TIMER_ALIVE,1000,NULL);
	theApp.TimerEnCoursMachine = this->SetTimer(ID_TIMER_MAJ_ENCOURSMACHINE,1000,NULL);

	this->SetTimer(ID_TIMER_AFFICHAGE,500,NULL);//TImer d'affichage V1.18
	this->SetTimer(ID_TIMER_CALCUL_CADENCE, DELAI_CALCUL_CADENCE, NULL);//V 45.0 toutes les 10 minutes
	//Changement de la police de caractère
	CFont NewFont;	
	NewFont.CreateFont(
				   20,                        // nHeight
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
				   "Arial");                 // lpszFacename

	ListDefMachine.Initialise(&NewFont);
	Liste_Alertes.Initialise(&NewFont);

	ListDefMachine.SetCustomColor(RGB(255, 0, 0));
	Liste_Alertes.SetCustomColor(RGB(0, 0, 255));
	/*ListDefMachine.SetCustomFont(&NewFont);
	Liste_Alertes.SetCustomFont(&NewFont);

	ListDefMachine.SetCustomColor(RGB(255,0,0));
	Liste_Alertes.SetCustomColor(RGB(0,0,255));*/

	InitialiserTextes();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

int CDialMachine::MAJGrilleComptage(void)
{
	//Initialisation de la grille pour affichage des paramètres/Etats
	CString strTemp;
	int iLigne;

	iLigne = 1;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbBoiteOK[0] + theApp.m_Recette.NbBoiteOK[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbBoitePrelev[0] + theApp.m_Recette.NbBoitePrelev[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbBoiteNOK[0] + theApp.m_Recette.NbBoiteNOK[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbPieceOK[0] + theApp.m_Recette.NbPieceOK[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbPieceNOK[0] + theApp.m_Recette.NbPieceNOK[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbRebutVision[0] + theApp.m_Recette.NbRebutVision[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbRebutPoids[0] + theApp.m_Recette.NbRebutPoids[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);

	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbVisionOK[0] + theApp.m_Recette.NbVisionOK[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbVisionDMX[0] + theApp.m_Recette.NbVisionDMX[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbVisionTache[0] + theApp.m_Recette.NbVisionTache[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbVisionTacheUV[0] + theApp.m_Recette.NbVisionTacheUV[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);
	iLigne ++;
	strTemp.Format(_T("%ld"),(theApp.m_Recette.NbVisionCB[0] + theApp.m_Recette.NbVisionCB[1]));
	GrilleEtats.SetItemText(iLigne,1,strTemp);

	GrilleEtats.Refresh();

	//V16 : Cadence
	TextCadence.strText.Format(_T("%2.2f"),float(theApp.Process.dCadence));
	TextCadence.Invalidate();


	return 0;
}


void CDialMachine::OnTimer(UINT_PTR nIDEvent)
{
	// OnTimer
	switch(nIDEvent)
	{
	case ID_TIMER_LOGOUT:
		//Logout Auto + Gestion des modes GCA V1.18
		if((theApp.Process.iEtat == ETAT_PROCESS_HORSPRODUCTION)||
			(theApp.Process.iEtat == ETAT_PROCESS_PRODUCTIONARRET))
		{
			theApp.ChargerUtilisateurDefaut(); //V 40 theApp.ChargerUtilisateur(CODE_UTILISATEUR_DEFAUT);
			//V 40 theApp.MAJMenu();
		}
		break;

	case ID_TIMER_ALIVE:
		theApp.GererSynchro();
		break;

	case ID_TIMER_PULSEVISION:
		KillTimer(theApp.TimerVision);
		//RESET SYNCHRO
		for (int i = 0; i < 3; i++)
		{
			MdigControl(theCam.milDigs,M_USER_BIT_STATE + theCam.milUSERBIT[i] + M_DIG_DISPATCH_IMMEDIATE,M_OFF);
		}
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT2 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);	//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT3 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);	//#12
		//MdigControl(theCam.milDigs,M_USER_BIT_STATE + M_USER_BIT4 + M_DIG_DISPATCH_IMMEDIATE,M_OFF);	//#12
		break;

	case ID_TIMER_AFFICHAGE:

		//MAj de la grille des compteurs
		//::SendMessage(theApp.MachineView->GetSafeHwnd(),WM_MAJ_COMPTEURS,0,0);
		MAJGrilleComptage();

		//MAJ tableau Mesures pour peson selectionné
		if ((theApp.PesonsView != NULL)&&(((CDialPesons *)theApp.PesonsView)->Peson != NULL))
		{
			if (((CDialPesons *)theApp.PesonsView)->Peson->indexHisto != ((CDialPesons *)theApp.PesonsView)->Peson->indexAffichage)
			{
				::SendMessage(theApp.PesonsView->GetSafeHwnd(), WM_PESON_MAJMESURE, ((CDialPesons *)theApp.PesonsView)->Peson->iNumPeson, 0);
				((CDialPesons *)theApp.PesonsView)->Peson->indexAffichage = ((CDialPesons *)theApp.PesonsView)->Peson->indexHisto;
			}
		}
		
		break;

		//Maj de la table EnCoursMachine periodiquement
	case ID_TIMER_MAJ_ENCOURSMACHINE:
		theApp.SetEncoursMachine();
		break;

		//V 44.8
	case ID_TIMER_CALCUL_CADENCE:
#ifdef DEBUG
		theCam.Piste[0].lCountVision_OK++;
		theCam.Piste[1].lCountVision_OK = theCam.Piste[1].lCountVision_OK + 2;
		theCam.Piste[2].lCountVision_OK = theCam.Piste[2].lCountVision_OK + 3;

		theApp.m_Robot1.peson[0]->lNbMesuresEffectuees++;
		theApp.m_Robot1.peson[1]->lNbMesuresEffectuees++;

		theApp.m_Robot2.peson[0]->lNbMesuresEffectuees++;
		theApp.m_Robot2.peson[1]->lNbMesuresEffectuees++;

		theApp.m_Robot3.peson[0]->lNbMesuresEffectuees++;
		theApp.m_Robot3.peson[1]->lNbMesuresEffectuees++;
#endif // DEBUG

		theApp.Process.Cadence_CalculerMoy();
		
		theApp.Process.Cadence_CalculerRobots();

		switch (pParamModeAffichageResolution->value)
		{
		case 0:// petite resolution : affichage sur dialogue vision
		default:
			((CDialDefautPiste *)theApp.DefautPisteView)->Textcadence_Vues[0].strText.Format(_T("%2.2f"), theApp.Process.CadenceTapis[0].dMoy_Cadence);
			((CDialDefautPiste *)theApp.DefautPisteView)->Textcadence_Vues[1].strText.Format(_T("%2.2f"), theApp.Process.CadenceTapis[1].dMoy_Cadence);
			((CDialDefautPiste *)theApp.DefautPisteView)->Textcadence_Vues[2].strText.Format(_T("%2.2f"), theApp.Process.CadenceTapis[2].dMoy_Cadence);

			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Pris[0].strText.Format(_T("%2.2f"), theApp.Process.CadencePrises[0].dMoy_Cadence);
			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Pris[1].strText.Format(_T("%2.2f"), theApp.Process.CadencePrises[1].dMoy_Cadence);
			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Pris[2].strText.Format(_T("%2.2f"), theApp.Process.CadencePrises[2].dMoy_Cadence);

			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Vues[0].Invalidate();
			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Vues[1].Invalidate();
			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Vues[2].Invalidate();
			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Pris[0].Invalidate();
			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Pris[1].Invalidate();
			((CDialDefautPiste * )theApp.DefautPisteView)->Textcadence_Pris[2].Invalidate();

			break;

		case 1:// Grande resolution : affichage sur dialogue Machine
			Textcadence_Vues[0].strText.Format(_T("%2.2f"), theApp.Process.CadenceTapis[0].dMoy_Cadence);
			Textcadence_Vues[1].strText.Format(_T("%2.2f"), theApp.Process.CadenceTapis[1].dMoy_Cadence);
			Textcadence_Vues[2].strText.Format(_T("%2.2f"), theApp.Process.CadenceTapis[2].dMoy_Cadence);

			Textcadence_Pris[0].strText.Format(_T("%2.2f"), theApp.Process.CadencePrises[0].dMoy_Cadence);
			Textcadence_Pris[1].strText.Format(_T("%2.2f"), theApp.Process.CadencePrises[1].dMoy_Cadence);
			Textcadence_Pris[2].strText.Format(_T("%2.2f"), theApp.Process.CadencePrises[2].dMoy_Cadence);

			Textcadence_Vues[0].Invalidate();
			Textcadence_Vues[1].Invalidate();
			Textcadence_Vues[2].Invalidate();
			Textcadence_Pris[0].Invalidate();
			Textcadence_Pris[1].Invalidate();
			Textcadence_Pris[2].Invalidate();
			break;
		}

		break;

	default:
		break;

	}

	CAcyDial::OnTimer(nIDEvent);
}


LRESULT CDialMachine::OnMAJCompteurs(WPARAM wParam, LPARAM lParam)
{
	MAJGrilleComptage();
	return 0L;
}
LRESULT CDialMachine::OnMAJEtat(WPARAM wParam, LPARAM lParam)
{
	theApp.MAJMenuEtat();
	return 0L;
}
LRESULT CDialMachine::OnMAJPrevDialog(WPARAM wParam, LPARAM lParam)
{
	theApp.DialMachineEnCours = theApp.DialMachinePrev;
	theApp.ExecuterFonctionExt("AFF_CURRENTMACHINEVIEW",NULL);

	return 0L;
}

LRESULT CDialMachine::OnMAJViewMachine(WPARAM wParam, LPARAM lParam)
{
	theApp.ExecuterFonctionExt("AFF_MACHINEVIEW",NULL);

	return 0L;
}



void CDialMachine::InitialiserTextes(void)
{
	//GCA09.01
	//Mettre les Textes a jours en fonction de la Recette
	TextDescription.strText = theApp.m_Recette.Article.Description;
	TextOf.strText = theApp.m_Recette.OfNum;
	TextArticle.strText = theApp.m_Recette.Article.Code;
	TextLot.strText = theApp.m_Recette.strLot;
	TextCadence.strText = "0";

	TextDescription.Invalidate();
	TextOf.Invalidate();
	TextArticle.Invalidate();
	TextLot.Invalidate();
	TextCadence.Invalidate();

	UpdateData(FALSE);
}

void CDialMachine::SetStateButProcess(int iState)
{
	for(int i = 0; i < MenuDialMachine.NbMaxBoutons;i++)
	{
		if(MenuDialMachine.Boutons[i]->strCodeFonction[0] == _T("PROCESS_VALIDER"))
		{
			MenuDialMachine.Boutons[i]->iFonction = iState;
		}
	}

	MenuDialMachine.UpdateMenu();
}
BOOL CDialMachine::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// Interception de la touche ENTREE et ECHAP GCA 1.14

	return CAcyDial::OnCommand(wParam, lParam);
}
