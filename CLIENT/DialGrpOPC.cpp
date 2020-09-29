// DialGrpOPC.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DialGrpOPC.h"
#include "AC_Util.h"


// Boîte de dialogue CDialGrpOPC

IMPLEMENT_DYNAMIC(CDialGrpOPC, CAcyDial)

CDialGrpOPC::CDialGrpOPC(CWnd* pParent /*=NULL*/,CClientOPC* OPCGroupe/*=NULL*/)
	: CAcyDial(CDialGrpOPC::IDD, pParent)
	,voy_Auto(&theApp.iAffVoyant[INDEX_VOY_AUTO],"Auto")
	,voy_Vidage(&theApp.iAffVoyant[INDEX_VOY_VIDAGE],"Vidage")
	,voy_Maint(&theApp.iAffVoyant[INDEX_VOY_MAINT],"Maintenance")
{
	Groupe = OPCGroupe;
}

CDialGrpOPC::~CDialGrpOPC()
{
}

void CDialGrpOPC::DoDataExchange(CDataExchange* pDX)
{
	CAcyDial::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, Grille);
	DDX_Control(pDX, IDC_VOYANT_AUTO, voy_Auto);
	DDX_Control(pDX, IDC_VOYANT_VIDAGE, voy_Vidage);
	DDX_Control(pDX, IDC_VOYANT_MAINTENANCE, voy_Maint);
}


BEGIN_MESSAGE_MAP(CDialGrpOPC, CAcyDial)
	ON_BN_CLICKED(IDC_BUTSETVAL, &CDialGrpOPC::OnBnClickedButsetval)
	ON_BN_CLICKED(IDC_BUTGETVAL, &CDialGrpOPC::OnBnClickedButgetval)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RAD_MANURAF, &CDialGrpOPC::OnBnClickedRadManuraf)
	ON_BN_CLICKED(IDC_RAD_AUTORAF, &CDialGrpOPC::OnBnClickedRadAutoraf)
	ON_BN_CLICKED(IDC_BUTRESETOPCCOM, &CDialGrpOPC::OnBnClickedButresetopccom)
	ON_BN_CLICKED(IDC_RADPROCESSW, &CDialGrpOPC::OnBnClickedRadprocessw)
	ON_BN_CLICKED(IDC_RADROBOTREAD1, &CDialGrpOPC::OnBnClickedRadrobotread1)
	ON_BN_CLICKED(IDC_RADROBOTWRITE1, &CDialGrpOPC::OnBnClickedRadrobotwrite1)
	ON_BN_CLICKED(IDC_RADROBOTREAD2, &CDialGrpOPC::OnBnClickedRadrobotread2)
	ON_BN_CLICKED(IDC_RADROBOTWRITE2, &CDialGrpOPC::OnBnClickedRadrobotwrite2)
	ON_BN_CLICKED(IDC_RADROBOTREAD3, &CDialGrpOPC::OnBnClickedRadrobotread3)
	ON_BN_CLICKED(IDC_RADROBOTWRITE3, &CDialGrpOPC::OnBnClickedRadrobotwrite3)
	ON_BN_CLICKED(IDC_RADPROCESSW2, &CDialGrpOPC::OnBnClickedRadprocessw2)
	ON_BN_CLICKED(IDC_RADPROCESSW3, &CDialGrpOPC::OnBnClickedRadprocessw3)
	ON_BN_CLICKED(IDC_RADROBOTREAD4, &CDialGrpOPC::OnBnClickedRadrobotread4)
	ON_BN_CLICKED(IDC_RADROBOTWRITE4, &CDialGrpOPC::OnBnClickedRadrobotwrite4)
	ON_BN_CLICKED(IDC_RADROBOTWRITE7, &CDialGrpOPC::OnBnClickedRadrobotwrite7)
	ON_BN_CLICKED(IDC_RADROBOTWRITE5, &CDialGrpOPC::OnBnClickedRadrobotwrite5)
	ON_BN_CLICKED(IDC_RADROBOTWRITE6, &CDialGrpOPC::OnBnClickedRadrobotwrite6)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialGrpOPC

BOOL CDialGrpOPC::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation de la grille
	Grille.SetDefCellHeight(25);//hauteur des cellules
	Grille.SetColumnCount(2);
	Grille.SetFixedRowCount(1);//légende fixe
	if(Groupe != NULL)
		Grille.SetRowCount(Groupe->iNbItemForGroup + 1);
	else
		Grille.SetRowCount(MAX_ITEMS_BY_GROUP);

	Grille.ExpandColumnsToFit(TRUE);
	Grille.SetListMode(TRUE);
	Grille.SetSingleRowSelection(TRUE);
	Grille.SetSingleColSelection(TRUE);
	Grille.SetFixedColumnSelection(FALSE);
	Grille.SetFixedRowSelection(FALSE);

	//légende
	CGridCellBase* cellule;
	Grille.SetFixedBkColor(RGB(0,46,184));
	Grille.SetFixedTextColor(RGB(255,255,255));
	cellule = Grille.GetCell(0,0);
	cellule->SetText(_T("Nom Items"));
	cellule = Grille.GetCell(0,1);
	cellule->SetText(_T("Valeur [7-0]"));

	//GRoupe Recette par défaut
	CheckRadioButton(IDC_RADPROCESSW,IDC_RADROBOTWRITE7,IDC_RADPROCESSW);//V 42 IDC_RADROBOTWRITE4
	CheckRadioButton(IDC_RAD_AUTORAF,IDC_RAD_MANURAF,IDC_RAD_AUTORAF);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
void CDialGrpOPC::ResetGrille(void)
{
	
	//Initialisation de la grille Vide en gardant la légende
	for(int Ligne = 1; Ligne < Grille.GetRowCount(); Ligne ++)
	{
		Grille.SetItemText(Ligne,0,"");
		Grille.SetItemText(Ligne,1,"");
	}
	
	Grille.SetRowCount(Groupe->iNbItemForGroup + 1);
	Grille.Refresh();
}
void CDialGrpOPC::InitialiserGrille(void)
{
	
	//Initialisation de la grille pour affichage des Items

	CString strTemp = "";
	int iBool = 0;

	for(int i =0; i < Groupe->iNbItemForGroup;i++)
	{
		Grille.SetItemText(i+1,0,Groupe->my_Items[i].strItems);

		if(Groupe->my_Items[i].pValue != NULL)//on affiche que les items liés à d'autres variables
		{
			switch(Groupe->my_Items[i].Value.vt)
				{
				case VT_UINT:
					strTemp.Format("%u",Groupe->my_Items[i].Value.uintVal);
					break;
				case VT_I2:
				case VT_I1:
					strTemp.Format("%d",Groupe->my_Items[i].Value.iVal);
					break;

				case VT_I4:
					strTemp.Format("%ld",Groupe->my_Items[i].Value.lVal);
					break;

				case VT_ACYBOOL:
					strTemp = FormatToBinaryString(Groupe->my_Items[i].Value.intVal , 8);
					break;

				case VT_BOOL:
					strTemp.Format("%d",Groupe->my_Items[i].Value.boolVal);
					break;

				case VT_R4:
					strTemp.Format("%.3f",Groupe->my_Items[i].Value.fltVal); 
				break;

				default:
					strTemp = CString(Groupe->my_Items[i].Value.bstrVal);
					break;
				}

			Grille.SetItemText(i+1,1,strTemp);
		}
		else
		{
			Grille.SetItemText(i+1,1,"Item Non lié");
		}
		
	}
	
	Grille.Refresh();
}




void CDialGrpOPC::OnBnClickedButsetval()
{
	// Set Value Item en cours

	for(int i = 0; i< Groupe->iNbItemForGroup;i++)
	{
		int iTemp = 0;
		CString strValue;

		//Recupération valeur dans la grille
		strValue = Grille.GetItemText(i+1,1);//i+1 pour la ligne de la légende
		
		//MAJ des valeurs Groupe OPC
		switch(Groupe->my_Items[i].Value.vt)
		{
			case VT_UINT:
			case VT_I2:
			case VT_I4://GCA1412
					(*(int*)(Groupe->my_Items[i].pValue)) = atoi(strValue);
					break;

			case VT_ACYBOOL:
					Groupe->my_Items[i].Value.intVal;//TODO
					break;

			case VT_BOOL:
					strValue == "1" ? (*(int*)(Groupe->my_Items[i].pValue)) =TRUE:(*(int*)(Groupe->my_Items[i].pValue)) = FALSE; 
					break;

			case VT_R4:
				(*(float*)(Groupe->my_Items[i].pValue)) = (float) atof(strValue);
				break;

				default:
					iTemp = atoi(strValue);
//					Groupe->my_Items[i].pValue =(void *) iTemp ;
					break;
		}
		
	}

	//MAJ OPC Groupe
//	Groupe->bWriteThread = true;

}

void CDialGrpOPC::OnBnClickedButgetval()
{
	// Get Value item en cours

	//OPC To Data
	Groupe->OPCToData();

	InitialiserGrille();
	

}



void CDialGrpOPC::OnTimer(UINT_PTR nIDEvent)
{
	// Timer de rafraichissement affichage
	switch(nIDEvent)
	{

	case TIMER_AFF:

		if(Groupe!=NULL) 
		{
			//Groupe->OPCToData();
			InitialiserGrille();
		}

		break;
	default:
		break;

	}

	CAcyDial::OnTimer(nIDEvent);
}


int CDialGrpOPC::StartLecture(void)
{
	//Lancement timer pour lecture priodique des valeurs
	TimerAff = SetTimer(TIMER_AFF,200,NULL);

	return 0;
}

int CDialGrpOPC::StopLecture(void)
{
	KillTimer(TimerAff);
	return 0;
}
void CDialGrpOPC::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CAcyDial::OnShowWindow(bShow, nStatus);

	// Gestion du timer d'affichage 
	if(bShow)
	{
		CheckRadioButton(IDC_RAD_RAF_AUTO,IDC_RAD_RAF_MANU,IDC_RAD_RAF_AUTO);
		StartLecture();
	}
	else
	{
		CheckRadioButton(IDC_RAD_RAF_AUTO,IDC_RAD_RAF_MANU,IDC_RAD_RAF_MANU);
		StopLecture();
	}
}




void CDialGrpOPC::OnBnClickedRadManuraf()
{
	//Arret Timer
	StopLecture();
}

void CDialGrpOPC::OnBnClickedRadAutoraf()
{
	//Lancement Timer
	StartLecture();
}

void CDialGrpOPC::OnBnClickedButresetopccom()
{
	// Reset de la communication OPC
	//Si y'a une erreur sinon on fait rien
	//V 42 if(*Groupe->Alarme != 0 ) Groupe->Reset();
	if (Groupe != NULL) Groupe->Reset();
}

void CDialGrpOPC::OnBnClickedRadprocessw()
{
	// Selection du groupe Process Write
	Groupe = theApp.Process.m_OPCClient_W;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadrobotread1()
{
	// Selection du groupe Robot1 Read
	Groupe = theApp.m_Robot1.m_OPCClient_TR;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadrobotwrite1()
{
	// Selection du groupe Robot1 Write
	Groupe = theApp.m_Robot1.m_OPCClient_TW;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadrobotread2()
{
	// Selection du groupe Robot2 Read
	Groupe = theApp.m_Robot2.m_OPCClient_TR;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadrobotwrite2()
{
	// Selection du groupe Robot2 Write
	Groupe = theApp.m_Robot2.m_OPCClient_TW;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadrobotread3()
{
	// Selection du groupe Robot3 read
	Groupe = theApp.m_Robot3.m_OPCClient_TR;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadrobotwrite3()
{
	// Selection du groupe Robot3 Write
	Groupe = theApp.m_Robot3.m_OPCClient_TW;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadprocessw2()
{
	// Selection du groupe Process Read
	Groupe = theApp.Process.m_OPCClient_R;

	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadprocessw3()
{
	// Selection du groupe Recette
	Groupe = theApp.m_Recette.m_OPCClient;
	
	ResetGrille();
	InitialiserGrille();
}

void CDialGrpOPC::OnBnClickedRadrobotread4()
{
	//Selection groupe Compteurs
	Groupe = theApp.Compteurs.m_OPCClient_R;
	
	ResetGrille();
	InitialiserGrille();

}
BOOL CDialGrpOPC::OnCommand(WPARAM wParam, LPARAM lParam)
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





void CDialGrpOPC::OnBnClickedRadrobotwrite4()
{
	//Selection groupe Pesage
	Groupe = &theApp.m_Groupes[GROUPE_PESAGE_R];
	
	ResetGrille();
	InitialiserGrille();
}


void CDialGrpOPC::OnBnClickedRadrobotwrite7()
{
	// DB 301 
	//Selection groupe Pesage
	Groupe = theApp.Process.m_OPCClient_Ai;

	ResetGrille();
	InitialiserGrille();
}


void CDialGrpOPC::OnBnClickedRadrobotwrite5()
{
	// DB 302
	//Selection groupe Pesage
	Groupe = theApp.Process.m_OPCClient_AFCy;

	ResetGrille();
	InitialiserGrille();
}


void CDialGrpOPC::OnBnClickedRadrobotwrite6()
{
	//DB 303
	//Selection groupe Pesage
	Groupe = theApp.Process.m_OPCClient_Alertes;

	ResetGrille();
	InitialiserGrille();
}
