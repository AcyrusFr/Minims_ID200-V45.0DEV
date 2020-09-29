// DialogParam.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DialogParam.h"
#include "DiagPadNum.h"
#include "DiagPadAlpha.h"
#include "General.h"


// Boîte de dialogue CDialogParam

IMPLEMENT_DYNAMIC(CDialogParam, CAcyDial)

CDialogParam::CDialogParam(CWnd* pParent /*=NULL*/,CJeuParametres * parentParam)
	: CAcyDial(CDialogParam::IDD, pParent)
	, strLabelSelec(_T(""))
	, strValueMin(_T(""))
	, strValueMax(_T(""))
	, iSlider(0)
	, iValSlider(0)
{
	
	ParentParam = parentParam;

	//sauvegarde des valeurs initiales
	for(int i =0; i< ParentParam->GetNumberOfParams();i++)
	{
		switch(ParentParam->Parametres[i]->type)
		{
			case TYPEDOUBLE:
				ParamsInit[i]= new CParametreDouble();
				ParamsInit[i]->label = ParentParam->Parametres[i]->label;
				((CParametreDouble *)ParamsInit[i])->value = ((CParametreDouble *)ParentParam->Parametres[i])->value;
				((CParametreDouble *)ParamsInit[i])->value_min = ((CParametreDouble *)ParentParam->Parametres[i])->value_min;
				((CParametreDouble *)ParamsInit[i])->value_max = ((CParametreDouble *)ParentParam->Parametres[i])->value_max;
				break;

			case TYPESTRING:
				ParamsInit[i]= new CParametreString();
				ParamsInit[i]->label = ParentParam->Parametres[i]->label;
				((CParametreString *)ParamsInit[i])->value =((CParametreString *) ParentParam->Parametres[i])->value;
				break;

			case TYPELONG:
				ParamsInit[i]= new CParametreLong();
				ParamsInit[i]->label = ParentParam->Parametres[i]->label;
				((CParametreLong *)ParamsInit[i])->value = ((CParametreLong *)ParentParam->Parametres[i])->value;
				((CParametreLong *)ParamsInit[i])->value_min = ((CParametreLong *)ParentParam->Parametres[i])->value_min;
				((CParametreLong *)ParamsInit[i])->value_max = ((CParametreLong *)ParentParam->Parametres[i])->value_max;
				break;

		}
	}
	
	
}

CDialogParam::~CDialogParam()
{
	DeleteInitValues();
}

void CDialogParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRILLE, Grille);
	DDX_Text(pDX, IDC_EDITLABELSELEC, strLabelSelec);
	DDX_Control(pDX, IDC_SLIDER, SliderValue);
	DDX_Text(pDX, IDC_EDITLABELVALMIN, strValueMin);
	DDX_Text(pDX, IDC_EDITLABELVALMAX, strValueMax);
	DDX_Slider(pDX, IDC_SLIDER, iSlider);
	DDX_Text(pDX, IDC_EDITVALSLIDER, iValSlider);
}


BEGIN_MESSAGE_MAP(CDialogParam, CAcyDial)
	ON_NOTIFY(GVN_SELCHANGED,IDC_GRILLE,OnSelChanged)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER, &CDialogParam::OnNMCustomdrawSlider)
	//ON_BN_CLICKED(IDC_BUT_SAVE, &CDialogParam::OnBnClickedButSave)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER, &CDialogParam::OnNMReleasedcaptureSlider)
	ON_BN_CLICKED(IDOK, &CDialogParam::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUT_PLUS, &CDialogParam::OnBnClickedButPlus)
	ON_BN_CLICKED(IDC_BUT_MOINS, &CDialogParam::OnBnClickedButMoins)
	ON_BN_CLICKED(IDCANCEL, &CDialogParam::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUT_EDIT, &CDialogParam::OnBnClickedButEdit)
	ON_BN_CLICKED(IDC_RAD_GENERAL, &CDialogParam::OnBnClickedRadGeneral)
	ON_BN_CLICKED(IDC_RAD_VISION, &CDialogParam::OnBnClickedRadVision)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDialogParam

BOOL CDialogParam::OnInitDialog()
{
	CAcyDial::OnInitDialog();

	// Initialisation de la grille
	Grille.SetDefCellHeight(25);//hauteur des cellules
	Grille.SetColumnCount(4);
	Grille.SetFixedRowCount(1);//légende fixe
	Grille.SetRowCount(ParentParam->GetNumberOfParams()+1);
	Grille.ExpandColumnsToFit(TRUE);
	Grille.SetListMode(TRUE);
	Grille.SetSingleRowSelection(TRUE);
	Grille.SetSingleColSelection(TRUE);
	Grille.SetFixedColumnSelection(FALSE);
	Grille.SetFixedRowSelection(FALSE);
	Grille.SetEditable(FALSE);
	//légende
	CGridCellBase* cellule;
	Grille.SetFixedBkColor(RGB(0,46,184));
	Grille.SetFixedTextColor(RGB(255,255,255));
	cellule = Grille.GetCell(0,0);
	cellule->SetText(_T("Label Paramétre"));
	cellule = Grille.GetCell(0,1);
	cellule->SetText(_T("Valeur"));
	cellule = Grille.GetCell(0,2);
	cellule->SetText(_T("Valeur Min"));
	cellule = Grille.GetCell(0,3);
	cellule->SetText(_T("Valeur Max"));

	InitialiserGrille();

	CheckRadioButton(IDC_RAD_GENERAL,IDC_RAD_VISION,IDC_RAD_GENERAL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}

void CDialogParam::InitialiserGrille(void)
{

	CString strTemp;

	Grille.SetRowCount(ParentParam->GetNumberOfParams()+1);

	for(int i = 1; i < ParentParam->GetNumberOfParams()+1;i++)
	{
		CGridCellBase* cellule;
		cellule = Grille.GetCell(i,0);//colonne 0 Label
		cellule->SetText(ParentParam->Parametres[i-1]->label);

		switch(ParentParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp.Format(_T("%1.2f"),((CParametreDouble*)(ParentParam->Parametres[i-1]))->value);
			cellule->SetText(strTemp);

			cellule = Grille.GetCell(i,2);//colonne 2 Valeur Min
			strTemp.Format(_T("%1.2f"),((CParametreDouble*)(ParentParam->Parametres[i-1]))->value_min);
			cellule->SetText(strTemp);

			cellule = Grille.GetCell(i,3);//colonne 3 Valeur Max
			strTemp.Format(_T("%1.2f"),((CParametreDouble*)(ParentParam->Parametres[i-1]))->value_max);
			cellule->SetText(strTemp);
			break;

		case TYPESTRING:
			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp = ((CParametreString*)(ParentParam->Parametres[i-1]))->value;
			cellule->SetText(strTemp);

			cellule = Grille.GetCell(i,2);//colonne 2 Valeur Min
			strTemp = _T("");
			cellule->SetText(strTemp);

			cellule = Grille.GetCell(i,3);//colonne 3 Valeur Max
			strTemp =  _T("");
			cellule->SetText(strTemp);
			break;

		case TYPELONG:
			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp.Format(_T("%ld"),((CParametreLong*)(ParentParam->Parametres[i-1]))->value);
			cellule->SetText(strTemp);

			cellule = Grille.GetCell(i,2);//colonne 2 Valeur Min
			strTemp.Format(_T("%ld"),((CParametreLong*)(ParentParam->Parametres[i-1]))->value_min);
			cellule->SetText(strTemp);

			cellule = Grille.GetCell(i,3);//colonne 3 Valeur Max
			strTemp.Format(_T("%ld"),((CParametreLong*)(ParentParam->Parametres[i-1]))->value_max);
			cellule->SetText(strTemp);
			break;

		default:
			break;
		}
		
		

	}

	Grille.Refresh();

}

void CDialogParam::OnDestroy()
{
	CDialog::OnDestroy();

	//Liberation grille
	Grille.DeleteAllItems();
}

void CDialogParam::OnSelChanged(NMHDR* Nmhdr,LRESULT* lRes)
{
	//reglage slider
	lRes;
	NM_GRIDVIEW* infosGrid = (NM_GRIDVIEW*) Nmhdr;

	if(infosGrid->iRow >0)
	{

		CGridCellBase* cellule;
		CString strLabel;
		cellule = Grille.GetCell(infosGrid->iRow,0);//colonne 0 Label
		strLabel = cellule->GetText();
		strLabelSelec = strLabel;
		UpdateData(FALSE);

		int i = infosGrid->iRow;//numero de ligne

		switch(ParentParam->Parametres[i-1]->type)
		{
		case TYPESTRING:
			//on édite directement la case (pas de gestion du slider)
			GetDlgItem(IDC_BUT_MOINS)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUT_PLUS)->EnableWindow(FALSE);
			GetDlgItem(IDC_SLIDER)->EnableWindow(FALSE);
			
			break;
		case TYPEDOUBLE:

			GetDlgItem(IDC_BUT_MOINS)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUT_PLUS)->EnableWindow(TRUE);
			GetDlgItem(IDC_SLIDER)->EnableWindow(TRUE);

			UpdateData(TRUE);
			cellule = Grille.GetCell(infosGrid->iRow,2);//colonne 2 Valeur Min
			strValueMin = cellule->GetText();
			
			cellule = Grille.GetCell(infosGrid->iRow,3);//colonne 3 Valeur Max
			strValueMax = cellule->GetText();
			UpdateData(FALSE);

			//recalibrage du slider en fonction des valeurs min et max
#ifdef _UNICODE
			SliderValue.SetRange(((int)_wtof( strValueMin))*COEF_DOUBLE,((int)_wtof( strValueMax))*COEF_DOUBLE,TRUE);
#else
			SliderValue.SetRange(((int)atof( strValueMin))*COEF_DOUBLE,((int)atof( strValueMax))*COEF_DOUBLE,TRUE);
#endif
			

			UpdateData(TRUE);
			cellule = Grille.GetCell(infosGrid->iRow,1);//colonne 1 Valeur 
#ifdef _UNICODE
			iSlider = ((int)_wtof( cellule->GetText()))*COEF_DOUBLE;
#else
			iSlider = ((int)atof( cellule->GetText()))*COEF_DOUBLE;
#endif			
			iValSlider = iSlider;
			UpdateData(FALSE);
			break;


		case TYPELONG:

			GetDlgItem(IDC_BUT_MOINS)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUT_PLUS)->EnableWindow(TRUE);
			GetDlgItem(IDC_SLIDER)->EnableWindow(TRUE);

			UpdateData(TRUE);
			cellule = Grille.GetCell(infosGrid->iRow,2);//colonne 2 Valeur Min
			strValueMin = cellule->GetText();
			
			cellule = Grille.GetCell(infosGrid->iRow,3);//colonne 3 Valeur Max
			strValueMax = cellule->GetText();
			UpdateData(FALSE);

			//recalibrage du slider en fonction des valeurs min et max
#ifdef _UNICODE
			SliderValue.SetRange((int)_wtof( strValueMin),(int)_wtof( strValueMax),TRUE);
#else
			SliderValue.SetRange((int)atof( strValueMin),(int)atof( strValueMax),TRUE);
#endif			

			UpdateData(TRUE);
			cellule = Grille.GetCell(infosGrid->iRow,1);//colonne 1 Valeur 
#ifdef _UNICODE
			iSlider = (int)_wtof( cellule->GetText());
#else
			iSlider = (int)atof( cellule->GetText());
#endif			
			iValSlider = iSlider;
			UpdateData(FALSE);
			break;

		default:
			break;
		}
		

		Grille.Refresh();
		
	}
}

void CDialogParam::OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	pNMCD;
	//MAJ valeur Affichage
	UpdateData(TRUE);
	iValSlider = iSlider;
	UpdateData(FALSE);
	*pResult = 0;
}

void CDialogParam::OnBnClickedButSave()
{
	// Sauvegarde des valeurs dans le jeu de parametres
	CGridCellBase* cellule;
	CString strTemp;

	for(int i = 1; i < ParentParam->GetNumberOfParams()+1;i++)
	{

		switch(ParentParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp = cellule->GetText();
#ifdef _UNICODE
			((CParametreDouble*)(ParentParam->Parametres[i-1]))->value = _wtof(strTemp);
#else
			((CParametreDouble*)(ParentParam->Parametres[i-1]))->value = atof(strTemp);
#endif			
			
			cellule = Grille.GetCell(i,2);//colonne 2 Valeur Min
			strTemp = cellule->GetText();
#ifdef _UNICODE
				((CParametreDouble*)(ParentParam->Parametres[i-1]))->value_min = _wtof(strTemp);
#else
				((CParametreDouble*)(ParentParam->Parametres[i-1]))->value_min = atof(strTemp);
#endif		

			cellule = Grille.GetCell(i,3);//colonne 3 Valeur Max
			strTemp = cellule->GetText();
#ifdef _UNICODE
			((CParametreDouble*)(ParentParam->Parametres[i-1]))->value_max = _wtof(strTemp);
#else
			((CParametreDouble*)(ParentParam->Parametres[i-1]))->value_max = atof(strTemp);
#endif			
			break;

		case TYPESTRING:
			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp = cellule->GetText();
			((CParametreString*)(ParentParam->Parametres[i-1]))->value = strTemp;
			break;

		case TYPELONG:
			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp = cellule->GetText();
#ifdef _UNICODE
			((CParametreLong*)(ParentParam->Parametres[i-1]))->value =(long) _wtof(strTemp);
#else
			((CParametreLong*)(ParentParam->Parametres[i-1]))->value =(long) atof(strTemp);
#endif			

			cellule = Grille.GetCell(i,2);//colonne 2 Valeur Min
			strTemp = cellule->GetText();
#ifdef _UNICODE
			((CParametreLong*)(ParentParam->Parametres[i-1]))->value_min =(long)_wtof(strTemp);
#else
			((CParametreLong*)(ParentParam->Parametres[i-1]))->value_min =(long) atof(strTemp);
#endif			

			cellule = Grille.GetCell(i,3);//colonne 3 Valeur Max
			strTemp = cellule->GetText();
#ifdef _UNICODE
			((CParametreLong*)(ParentParam->Parametres[i-1]))->value_max =(long) _wtof(strTemp);
#else
			((CParametreLong*)(ParentParam->Parametres[i-1]))->value_max =(long) atof(strTemp);
#endif			
			break;

		default:
			break;
		}
	}


	//Sauvegarde des parametres
	ParentParam->SauvegarderParametres();
}

void CDialogParam::OnNMReleasedcaptureSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
		// Gestion Valeur du Slider
	pNMHDR;
	CCellRange Selected = Grille.GetSelectedCellRange();
	if((Selected.GetMinRow() >0)&&(Selected.GetMinRow() == Selected.GetMaxRow()))
	{
		int i = Selected.GetMinRow();//numero de ligne
		CGridCellBase* cellule;
		CString strTemp;
		UpdateData(TRUE);

		double dVal_Prev = 0;
		long lVal_Prev = 0;
		CString str_Prev = _T("");
		
		switch(ParentParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			dVal_Prev = ((CParametreDouble*)ParentParam->Parametres[i-1])->value;

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp.Format(_T("%1.2f"),((double) iSlider)/COEF_DOUBLE );
			cellule->SetText(strTemp);
			((CParametreDouble*)ParentParam->Parametres[i-1])->value = ((double) iSlider)/COEF_DOUBLE ;
			break;

		case TYPESTRING:
			//on édite directement la case (pas de gestion du slider)
			str_Prev = ((CParametreString*)ParentParam->Parametres[i-1])->value;
			break;

		case TYPELONG:
			lVal_Prev = ((CParametreLong*)ParentParam->Parametres[i-1])->value;

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			strTemp.Format(_T("%ld"),(long) iSlider );
			cellule->SetText(strTemp);
			((CParametreLong*)ParentParam->Parametres[i-1])->value = (long) iSlider ;
			break;

		default:
			break;
		}
		

		Grille.Refresh();

		//Traca Evenement Modif valeur Paramétre
		CString strLibelle,strCom;

		strLibelle = ParentParam->Parametres[i-1]->label;
		
		switch(ParentParam->Parametres[i-1]->type)
		{
			case TYPEDOUBLE:
				strCom.Format(_T("Avant: %2.3f | Après: %2.3f"),dVal_Prev,((CParametreDouble*)ParentParam->Parametres[i-1])->value);
				break;
			case TYPESTRING:
				strCom.Format(_T("Avant: %s | Après: %s"),str_Prev,((CParametreString*)ParentParam->Parametres[i-1])->value);
				break;
			case TYPELONG:
				strCom.Format(_T("Avant: %ld | Après: %ld"),lVal_Prev,((CParametreLong*)ParentParam->Parametres[i-1])->value);
				break;

			default:
				break;
		}
		
//Optimize Mem //		theApp.theJournal.Evenement(ParentParam->TypeTraca,strLibelle,strCom,theApp.CurrentUser.strCode);


	}
	*pResult = 0;
}



void CDialogParam::OnBnClickedOk()
{
	//sauvegarde des paramétres
	this->OnBnClickedButSave();

	//OnOK(); on ne ferme pas la fenetre quand on sauvegarde les paramétres
}

void CDialogParam::OnCancel()
{
	//recopie des valeurs Init
	for(int i =0; i< ParentParam->GetNumberOfParams();i++)
	{
		switch(ParentParam->Parametres[i]->type)
		{
			case TYPEDOUBLE:
				ParentParam->Parametres[i]->label = ParamsInit[i]->label ;
				((CParametreDouble *)ParentParam->Parametres[i])->value = ((CParametreDouble *)ParamsInit[i])->value ;
				break;

			case TYPESTRING:
				ParentParam->Parametres[i]->label = ParamsInit[i]->label ;
				((CParametreString *) ParentParam->Parametres[i])->value = ((CParametreString *)ParamsInit[i])->value ;
				break;

			case TYPELONG:
				ParentParam->Parametres[i]->label = ParamsInit[i]->label ;
				((CParametreLong *)ParentParam->Parametres[i])->value = ((CParametreLong *)ParamsInit[i])->value;
				break;
		}
	}
	
	InitialiserGrille();//rafraichit l'affichage
	//Simulation clic sur la premiére ligne
	NM_GRIDVIEW infosGrid;
	infosGrid.iRow = 1;
	OnSelChanged((NMHDR* )&infosGrid,NULL);

	//CDialog::OnCancel(); on ne ferme pas cette fenetre
}

void CDialogParam::OnBnClickedButPlus()
{
	// Bouton +1 Slider

	CCellRange Selected = Grille.GetSelectedCellRange();
	if((Selected.GetMinRow() >0)&&(Selected.GetMinRow() == Selected.GetMaxRow()))
	{
		int i = Selected.GetMinRow();//numero de ligne
		CGridCellBase* cellule;
		CGridCellBase* celluleMax;
		double dVal_max =0;
		long lVal_max = 0;
		CString strTemp;
		UpdateData(TRUE);
		//incrément valeur
		iSlider ++;

		double dVal_Prev = 0;
		long lVal_Prev = 0;
		CString str_Prev = _T("");	
		bool bSaveParam = true;

		switch(ParentParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			dVal_Prev = ((CParametreDouble*)ParentParam->Parametres[i-1])->value;

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			celluleMax = Grille.GetCell(i,3);//colonne 3 Max
			strTemp = celluleMax->GetText();
#ifdef _UNICODE
			dVal_max = _wtof(strTemp);
#else
			dVal_max = atof(strTemp);
#endif
			if(dVal_max>= ((double) iSlider)/COEF_DOUBLE )
			{
				strTemp.Format(_T("%1.2f"),((double) iSlider)/COEF_DOUBLE );
				cellule->SetText(strTemp);
				((CParametreDouble*)ParentParam->Parametres[i-1])->value = ((double) iSlider)/COEF_DOUBLE ;
			}
			else
			{
#ifndef _UNICODE
				AfxMessageBox("Depassement de la valeur Maxi !",MB_ICONWARNING);
#else
				AfxMessageBox(L"Depassement de la valeur Maxi !",MB_ICONWARNING);
#endif
				iSlider--;//on remet a la valeur précédente

				bSaveParam = false;
			}
			
			break;

		case TYPESTRING:
			str_Prev = ((CParametreString*)ParentParam->Parametres[i-1])->value;
			//on édite directement la case (pas de gestion du slider)
			break;

		case TYPELONG:
			lVal_Prev = ((CParametreLong*)ParentParam->Parametres[i-1])->value;

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			celluleMax = Grille.GetCell(i,3);//colonne 3 Max
			strTemp = celluleMax->GetText();
#ifdef _UNICODE
			lVal_max =(long) _wtof(strTemp);
#else
			lVal_max =(long) atof(strTemp);
#endif
			if(lVal_max>= iSlider)
			{
				strTemp.Format(_T("%ld"),(long) iSlider );
				cellule->SetText(strTemp);
				((CParametreLong*)ParentParam->Parametres[i-1])->value = (long) iSlider ;
			}
			else
			{
#ifndef _UNICODE
				AfxMessageBox("Depassement de la valeur Maxi !",MB_ICONWARNING);
#else
				AfxMessageBox(L"Depassement de la valeur Maxi !",MB_ICONWARNING);
#endif
				iSlider--;//on remet a la valeur précédente
				bSaveParam = false;
			}
			break;

		default:
			break;
		}
		
		UpdateData(FALSE);
		Grille.Refresh();
		SliderValue.SetPos(iSlider);
		SliderValue.Invalidate();

		if(bSaveParam ) 
		{
			ParentParam->SauvegarderParametres();

			//Traca Evenement Modif valeur Paramétre
			
			CString strLibelle,strCom;

			strLibelle = ParentParam->Parametres[i-1]->label;
			
			switch(ParentParam->Parametres[i-1]->type)
			{
				case TYPEDOUBLE:
					strCom.Format(_T("Avant: %2.3f | Après: %2.3f"),dVal_Prev,((CParametreDouble*)ParentParam->Parametres[i-1])->value);
					break;
				case TYPESTRING:
					strCom.Format(_T("Avant: %s | Après: %s"),str_Prev,((CParametreString*)ParentParam->Parametres[i-1])->value);
					break;
				case TYPELONG:
					strCom.Format(_T("Avant: %ld | Après: %ld"),lVal_Prev,((CParametreLong*)ParentParam->Parametres[i-1])->value);
					break;

				default:
					break;
			}
			
//Optimize Mem //			theApp.theJournal.Evenement(ParentParam->TypeTraca,strLibelle,strCom,theApp.CurrentUser.strCode);
		}

		
	}

}

void CDialogParam::OnBnClickedButMoins()
{
	// Bouton -1 Slider

	
	CCellRange Selected = Grille.GetSelectedCellRange();
	if((Selected.GetMinRow() >0)&&(Selected.GetMinRow() == Selected.GetMaxRow()))
	{
		int i = Selected.GetMinRow();//numero de ligne
		CGridCellBase* cellule;
		CGridCellBase* celluleMin;
		CString strTemp;
		double dVal_min = 0;
		long lVal_min = 0;
		UpdateData(TRUE);
		//incrément valeur
		iSlider --;

		double dVal_Prev = 0;
		long lVal_Prev = 0;
		CString str_Prev = _T("");
		bool bSaveParam = true;

		switch(ParentParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			dVal_Prev = ((CParametreDouble*)ParentParam->Parametres[i-1])->value;

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			celluleMin = Grille.GetCell(i,2);//colonne 2 Min
			strTemp = celluleMin->GetText();
#ifdef _UNICODE
			dVal_min = _wtof(strTemp);
#else
			dVal_min = atof(strTemp);
#endif
			if(dVal_min<= ((double) iSlider)/COEF_DOUBLE )
			{
				strTemp.Format(_T("%1.2f"),((double) iSlider)/COEF_DOUBLE );
				cellule->SetText(strTemp);
				((CParametreDouble*)ParentParam->Parametres[i-1])->value = ((double) iSlider)/COEF_DOUBLE ;
			}
			else
			{
#ifndef _UNICODE
				AfxMessageBox("Depassement de la valeur Min !",MB_ICONWARNING);
#else
				AfxMessageBox(L"Depassement de la valeur Min !",MB_ICONWARNING);
#endif
				iSlider++;//on remet a la valeur précédente
				bSaveParam = false;
			}
			break;

		case TYPESTRING:
			str_Prev = ((CParametreString*)ParentParam->Parametres[i-1])->value;
			//on édite directement la case (pas de gestion du slider)
			break;

		case TYPELONG:
			lVal_Prev = ((CParametreLong*)ParentParam->Parametres[i-1])->value;

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			celluleMin = Grille.GetCell(i,2);//colonne 2 Min
			strTemp = celluleMin->GetText();
#ifdef _UNICODE
			lVal_min =(long) _wtof(strTemp);
#else
			lVal_min =(long) atof(strTemp);
#endif
			if(lVal_min<= iSlider)
			{
				strTemp.Format(_T("%ld"),(long) iSlider );
				cellule->SetText(strTemp);
				((CParametreLong*)ParentParam->Parametres[i-1])->value = (long) iSlider ;
			}
			else
			{
#ifndef _UNICODE
				AfxMessageBox("Depassement de la valeur Min !",MB_ICONWARNING);
#else
				AfxMessageBox(L"Depassement de la valeur Min !",MB_ICONWARNING);
#endif
				iSlider++;//on remet a la valeur précédente
				bSaveParam = false;
			}
			break;

		default:
			break;
		}
		
		UpdateData(FALSE);
		Grille.Refresh();
		SliderValue.SetPos(iSlider);
		SliderValue.Invalidate();
		
		if(bSaveParam ) 
		{
			ParentParam->SauvegarderParametres();

			//Traca Evenement Modif valeur Paramétre
			
			CString strLibelle,strCom;

			strLibelle = ParentParam->Parametres[i-1]->label;
			
			switch(ParentParam->Parametres[i-1]->type)
			{
				case TYPEDOUBLE:
					strCom.Format(_T("Avant: %2.3f | Après: %2.3f"),dVal_Prev,((CParametreDouble*)ParentParam->Parametres[i-1])->value);
					break;
				case TYPESTRING:
					strCom.Format(_T("Avant: %s | Après: %s"),str_Prev,((CParametreString*)ParentParam->Parametres[i-1])->value);
					break;
				case TYPELONG:
					strCom.Format(_T("Avant: %ld | Après: %ld"),lVal_Prev,((CParametreLong*)ParentParam->Parametres[i-1])->value);
					break;

				default:
					break;
			}
			
//Optimize Mem //			theApp.theJournal.Evenement(ParentParam->TypeTraca,strLibelle,strCom,theApp.CurrentUser.strCode);
		}
	}

}

void CDialogParam::OnBnClickedCancel()
{
	//  ajoutez ici le code de votre gestionnaire de notification de contrôle
	OnCancel();
}

void CDialogParam::OnBnClickedButEdit()
{
	// Bouton Editer

	CCellRange Selected = Grille.GetSelectedCellRange();
	if((Selected.GetMinRow() >0)&&(Selected.GetMinRow() == Selected.GetMaxRow()))
	{
		int i = Selected.GetMinRow();//numero de ligne
		CGridCellBase* cellule;
		CString strTemp;
		CString strTempValue;

		CDiagPadNum frame;
		CDiagPadAlpha frameAlpha;

		double dVal_Temp = 0;
		long lVal_Temp = 0;

		double dVal_Prev = 0;
		long lVal_Prev = 0;
		CString str_Prev = _T("");

		bool bSaveParam = true;

		UpdateData(TRUE);
		
		//Ouverture Fenêtre de saisie
		switch(ParentParam->Parametres[i-1]->type)
		{

		case TYPEDOUBLE:
			dVal_Prev = ((CParametreDouble*)ParentParam->Parametres[i-1])->value;
			
			frame.bPassword = false;
#ifdef _UNICODE
			frame.strInfos.Format(L"Entrer Valeur Paramétre Double : %s\nValeur Min: %s\nValeur Max: %s",strLabelSelec,strValueMin,strValueMax);
#else
			frame.strInfos.Format("Entrer Valeur Paramétre Double : %s\nValeur Min: %s\nValeur Max: %s",strLabelSelec,strValueMin,strValueMax);
#endif
			
			if(frame.DoModal()!= IDOK)
			{
				bSaveParam = false;
				break;
			}
			else
			{
				strTempValue = frame.strSaisie;
			}

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			
#ifdef _UNICODE
			dVal_min = _wtof(strTemp);
#else
			dVal_Temp = atof(strTempValue);
#endif
			if((((CParametreDouble*)ParentParam->Parametres[i-1])->value_min <= dVal_Temp )&&
				(((CParametreDouble*)ParentParam->Parametres[i-1])->value_max >= dVal_Temp ))
			{
				strTemp.Format(_T("%1.2f"),dVal_Temp);
				cellule->SetText(strTemp);
				((CParametreDouble*)ParentParam->Parametres[i-1])->value = dVal_Temp ;
			}
			else
			{
#ifndef _UNICODE
				AfxMessageBox("Depassement des valeurs Limites !",MB_ICONWARNING);
#else
				AfxMessageBox(L"Depassement des valeurs Limites !",MB_ICONWARNING);
#endif
			}
			break;

		case TYPESTRING:
			str_Prev = ((CParametreString*)ParentParam->Parametres[i-1])->value;

			frameAlpha.bPassword = false;
#ifdef _UNICODE
			frameAlpha.strInfos.Format(L"Entrer Valeur Paramétre String : %s",strLabelSelec);
#else
			frameAlpha.strInfos.Format("Entrer Valeur Paramétre String : %s",strLabelSelec);
#endif
			frameAlpha.strSaisie = str_Prev;
			if(frameAlpha.DoModal()!= IDOK)
			{
				bSaveParam = false;
				break;
			}
			else
			{
				strTempValue = frameAlpha.strSaisie;
			}

			Grille.SetItemText(i,1,strTempValue);//colonne 1 Valeur
			
			((CParametreString*)ParentParam->Parametres[i-1])->value = strTempValue ;	
			break;

		case TYPELONG:
			lVal_Prev = ((CParametreLong*)ParentParam->Parametres[i-1])->value;

			frame.bPassword = false;
#ifdef _UNICODE
			frame.strInfos.Format(L"Entrer Valeur Paramétre Long : %s\nValeur Min: %s\nValeur Max: %s",strLabelSelec,strValueMin,strValueMax);
#else
			frame.strInfos.Format("Entrer Valeur Paramétre Long : %s\nValeur Min: %s\nValeur Max: %s",strLabelSelec,strValueMin,strValueMax);
#endif
			
			if(frame.DoModal()!= IDOK)
			{
				bSaveParam = false;
				break;
			}
			else
			{
				strTempValue = frame.strSaisie;
			}

			cellule = Grille.GetCell(i,1);//colonne 1 Valeur
			
#ifdef _UNICODE
			lVal_Temp =(long) _wtof(strTempValue);
#else
			lVal_Temp =(long) atof(strTempValue);
#endif
			if((((CParametreLong*)ParentParam->Parametres[i-1])->value_min <= lVal_Temp )&&
				(((CParametreLong*)ParentParam->Parametres[i-1])->value_max >= lVal_Temp ))
			{
				strTemp.Format(_T("%ld"),lVal_Temp);
				cellule->SetText(strTemp);
				((CParametreLong*)ParentParam->Parametres[i-1])->value = lVal_Temp ;
			}
			else
			{
#ifndef _UNICODE
				AfxMessageBox("Depassement des valeurs Limites !",MB_ICONWARNING);
#else
				AfxMessageBox(L"Depassement des valeurs Limites !",MB_ICONWARNING);
#endif
			}
			break;

		default:
			break;
		}
		
		UpdateData(FALSE);
		Grille.Refresh();
		SliderValue.SetPos(iSlider);
		SliderValue.Invalidate();

		if(bSaveParam ) 
		{
			ParentParam->SauvegarderParametres();

			//Traca Evenement Modif valeur Paramétre
			
			CString strLibelle,strCom;

			strLibelle = ParentParam->Parametres[i-1]->label;
			
			switch(ParentParam->Parametres[i-1]->type)
			{
				case TYPEDOUBLE:
					strCom.Format(_T("Avant: %2.3f | Après: %2.3f"),dVal_Prev,((CParametreDouble*)ParentParam->Parametres[i-1])->value);
					break;
				case TYPESTRING:
					strCom.Format(_T("Avant: %s | Après: %s"),str_Prev,((CParametreString*)ParentParam->Parametres[i-1])->value);
					break;
				case TYPELONG:
					strCom.Format(_T("Avant: %ld | Après: %ld"),lVal_Prev,((CParametreLong*)ParentParam->Parametres[i-1])->value);
					break;

				default:
					break;
			}
			
//Optimize Mem //			theApp.theJournal.Evenement(ParentParam->TypeTraca,strLibelle,strCom,theApp.CurrentUser.strCode);
		}

	}

}

void CDialogParam::OnBnClickedRadGeneral()
{
	//Selection Paramétre Vision
	DeleteInitValues();

	ParentParam = &JParamGeneral;

	SaveInitValues();

	InitialiserGrille();
	return;

	//avertissement Sauvegarde
	//Optimize Mem //UINT_PTR Result = AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_ASK_SAUVEGARDE),MB_YESNOCANCEL,MB_ICONASTERISK);
	UINT_PTR Result = AfxMessageBox("Sauvegarder les changements ?",MB_YESNOCANCEL,MB_ICONASTERISK);


	if(Result == IDYES)
	{
		//Sauvegarde des paramétres avant changement
		OnBnClickedButSave();
	}
	else if(Result == IDNO)
	{
		//Changement sans sauvegarde
	}
	else if(Result == IDCANCEL)
	{
		//Rien = pas de changemnt/ Pas de sauvegarde
		CheckRadioButton(IDC_RAD_GENERAL,IDC_RAD_VISION,IDC_RAD_GENERAL);
		return;
	}

	// Selection Jeu Paramétre Général
	DeleteInitValues();

	ParentParam = &JParamGeneral;
	
	SaveInitValues();

	InitialiserGrille();
}
void CDialogParam::DeleteInitValues()
{
	for(int i =0; i< ParentParam->GetNumberOfParams();i++)
	{
		if(ParamsInit[i]->type == TYPESTRING)
		{
			delete ((CParametreString * )ParamsInit[i] );
		}
		else
		{
			delete (ParamsInit[i] );
		}
	}
}

void CDialogParam::SaveInitValues()
{
	//sauvegarde des valeurs initiales
	for(int i =0; i< ParentParam->GetNumberOfParams();i++)
	{
		switch(ParentParam->Parametres[i]->type)
		{
			case TYPEDOUBLE:
				ParamsInit[i]= new CParametreDouble();
				ParamsInit[i]->label = ParentParam->Parametres[i]->label;
				((CParametreDouble *)ParamsInit[i])->value = ((CParametreDouble *)ParentParam->Parametres[i])->value;
				((CParametreDouble *)ParamsInit[i])->value_min = ((CParametreDouble *)ParentParam->Parametres[i])->value_min;
				((CParametreDouble *)ParamsInit[i])->value_max = ((CParametreDouble *)ParentParam->Parametres[i])->value_max;
				break;

			case TYPESTRING:
				ParamsInit[i]= new CParametreString();
				ParamsInit[i]->label = ParentParam->Parametres[i]->label;
				((CParametreString *)ParamsInit[i])->value =((CParametreString *) ParentParam->Parametres[i])->value;
				break;

			case TYPELONG:
				ParamsInit[i]= new CParametreLong();
				ParamsInit[i]->label = ParentParam->Parametres[i]->label;
				((CParametreLong *)ParamsInit[i])->value = ((CParametreLong *)ParentParam->Parametres[i])->value;
				((CParametreLong *)ParamsInit[i])->value_min = ((CParametreLong *)ParentParam->Parametres[i])->value_min;
				((CParametreLong *)ParamsInit[i])->value_max = ((CParametreLong *)ParentParam->Parametres[i])->value_max;
				break;

		}
	}
}
void CDialogParam::OnBnClickedRadVision()
{
	//Selection Paramétre Vision
	DeleteInitValues();

	ParentParam = &JParamVisionGeneral;

	SaveInitValues();

	InitialiserGrille();
	return;

	//avertissement Sauvegarde
	//Optimize Mem //UINT_PTR Result = AfxMessageBox(theApp.TableMessage.GetStr(MESSAGE_ASK_SAUVEGARDE),MB_YESNOCANCEL,MB_ICONASTERISK);
	UINT_PTR Result = AfxMessageBox("Sauvegarder les changements ?",MB_YESNOCANCEL,MB_ICONASTERISK);

	if(Result == IDYES)
	{
		//Sauvegarde des paramétres avant changement
		OnBnClickedButSave();
	}
	else if(Result == IDNO)
	{
		//Changement sans sauvegarde
	}
	else if(Result == IDCANCEL)
	{
		//Rien = pas de changemnt/ Pas de sauvegarde
		CheckRadioButton(IDC_RAD_GENERAL,IDC_RAD_VISION,IDC_RAD_VISION);
		return;
	}

	//Selection Paramétre Vision
	DeleteInitValues();

	ParentParam = &JParamVisionGeneral;

	SaveInitValues();

	InitialiserGrille();

}



