// DiagListeReq.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DiagListeReq.h"


// Boîte de dialogue CDiagListeReq

IMPLEMENT_DYNAMIC(CDiagListeReq, CDialog)

CDiagListeReq::CDiagListeReq(CWnd* pParent /*=NULL*/)
	: CDialog(CDiagListeReq::IDD, pParent)
{
	strRequete = "";
}

CDiagListeReq::~CDiagListeReq()
{
}

void CDiagListeReq::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Liste);
}


BEGIN_MESSAGE_MAP(CDiagListeReq, CDialog)
	ON_BN_CLICKED(IDOK, &CDiagListeReq::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CDiagListeReq::OnLbnDblclkList1)
END_MESSAGE_MAP()


// Gestionnaires de messages de CDiagListeReq

BOOL CDiagListeReq::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString strTemp;
	COleVariant var;

	VariantInit(var);

	//Ouverture de la liste
	#pragma warning( disable: 4995)
	CADORecordset rs (&theBase.dao_Base);
#pragma warning( default: 4995)
	rs.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
	
	if (rs.IsOpen())
	{
		if (rs.GetRecordCount() > 0 )	
		{
			rs.MoveFirst();

			do
			{
				//Formatage en fonction du type du champ
				rs.GetFieldValue(sChamp,var);

				if(var.vt != VT_EMPTY)
				{
					if(var.vt == VT_I4)
					{
						strTemp.Format("%ld",var.lVal);
					}
					else
					{
						strTemp = CString(var.bstrVal);
					}
				}

				m_Liste.AddString(strTemp);
				rs.MoveNext();
			}while(!rs.IsEOF());
		}
	}
	rs.Close();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION : les pages de propriétés OCX devraient retourner FALSE
}
void CDiagListeReq::OnBnClickedOk()
{
	if(m_Liste.GetCurSel()>-1)
	{
		m_Liste.GetText(m_Liste.GetCurSel(),sResult);
	}
	OnOK();
}

void CDiagListeReq::OnLbnDblclkList1()
{
	OnBnClickedOk();
}