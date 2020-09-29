#include "StdAfx.h"
#include "DalsaSpyder3CL.h"
#include "Form_DalsaSpyder.h"

CDalsaSpyder3CL::CDalsaSpyder3CL(bool b_att)
{
	this->b_att_rep = b_att;

	this->str_charfin = CHARFINSPYDER;
}

CDalsaSpyder3CL::~CDalsaSpyder3CL(void)
{
}

bool CDalsaSpyder3CL::SetGain(int gain)
{
	CString strGain;
	float fGain =0;

	//set analog gain
	CString commande = _T("sag 0 ");
	
	fGain =(float)( gain/100.) ;
	strGain.Format(_T("%1.2f"),fGain);
	
	commande+=strGain;
	return (this->Ecrire_commande(commande));
}
bool CDalsaSpyder3CL::SetGainPourcent(int gain)
{
	//TODO GCA
	return false;
}

bool CDalsaSpyder3CL::SetTrigger(int mode)
{
	CString value;
	this->err=_T("");
	bool result=false;
//	char buff[300];
	value.Format("%d",mode);

	//DALSA SPYDER 3CL
	//MODE = 2 -> Internal SYNC, Internal PRIN, programmable Line rate and exposure time using commands ssf and set
	//MODE = 3 -> External SYNC, Internal PRIN, maximum eposure time
	//MODE = 4 -> smart EXSYNC
	//MODE = 5 -> External SYNC and PRIN
	//MODE = 6 -> External SYNC, internal PRIN, programmable exposure time
	//MODE = 7 -> Internal programmable SYNC, maximum exposure time. Factory setting
	//MODE = 8 ->Inernal SYNC, internal PRIN, programmable exposure time. maximum line rate for exposure time
	if((mode<2)||(mode>8))
	{
		this->err=_T("Valeurs hors tolérance");
		return false;
	}
	else
	{
		result=this->Ecrire_commande(_T("sem "+value));
		return result;
	}

}
bool CDalsaSpyder3CL::SetExposition(int expo)
{
	CString value;
	this->err=_T("");
	bool result=false;
//	char buff[300];
	value.Format(_T("%d"),expo);

	//DALSA SPYDER 3CL
	//Exposure time de 3 à 3300 µs
	if((expo<EXPO_MIN)||(expo>EXPO_MAX))
	{
		this->err=_T("Valeurs hors tolérance");
		return false;
	}
	else
	{
		//V 44.0 result=this->Ecrire_commande(_T("sem "+value));
		result = this->Ecrire_commande(_T("set " + value));
		return result;
	}
}
bool CDalsaSpyder3CL::SetExpoMicroSec(int expoms)
{
	CString value;
	this->err=_T("");
	bool result=false;
//	char buff[300];
	value.Format(_T("%d"),expoms);

	//DALSA SPYDER 3CL
	//Exposure time de 3 à 3300 µs
	if((expoms<EXPO_MIN)||(expoms>EXPO_MAX))
	{
		this->err=_T("Valeurs hors tolérance");
		return false;
	}
	else
	{
		//V 44.0 result=this->Ecrire_commande(_T("sem "+value));
		result = this->Ecrire_commande(_T("sem " + value));
		return result;
	}
}

bool CDalsaSpyder3CL::Save_CamConfig(void)
{
	//Save user setting to non volatile Memory

	CString commande ;
	commande = _T("wus");//Write User Setting
	return (this->Ecrire_commande(commande));
}
bool CDalsaSpyder3CL::Set_PartialArea(int startline, int endline)
{
	CString value;
	this->err=_T("");
	bool result=false;
//	char buff[300];
	

	//DALSA SPYDER 3CL
	if((startline < 0)||(startline > endline))
	{
		this->err=_T("Valeurs hors tolérance");
		return false;
	}
	else
	{
		value.Format(_T("roi %d 1 %d 1"),startline,endline);
		result=this->Ecrire_commande(value);
		return result;
	}

}
void CDalsaSpyder3CL::Open_testFrame()
{
	//si la fenêtre a déjà étée ouverte 
	//on libére la mémoire avant d'en reprendre
	if(frame->GetSafeHwnd()>0) 
	{
		delete(frame);
		frame = NULL;
	}

	frame=new CForm_DalsaSpyder();
	frame->Create(CForm_DalsaSpyder::IDD);
	//affichage de la fenêtre
	frame->UpdateData(FALSE);
	frame->SetForegroundWindow();
	frame->ShowWindow(SW_SHOW);

	((CForm_DalsaSpyder*)frame)->AssertValid();
}

bool CDalsaSpyder3CL::SetFormat(int format)
{

	CString value;
	this->err=_T("");
	bool result=false;
//	char buff[300];
	value.Format(_T("%d"),format);

	//DALSA SPYDER 3CL

	//Camera Link Mode
	//MODE = 0 -> 8 bits, 1 Tap
	//MODE = 1 -> 12bits, 1 Tap
	//MODE = 2 -> 8bits , 2Taps
	//MODE = 3 -> 12bits , 2Taps
	
	if((format<0)||(format>3))
	{
		this->err=_T("Valeurs hors tolérance");
		return false;
	}
	else
	{
		result=this->Ecrire_commande(_T("clm "+value));
		return result;
	}

}
void CDalsaSpyder3CL::AfficheRep(char CharRecv)
{

	 if(frame->GetSafeHwnd()>0)
	 {
		((CForm_DalsaSpyder*)frame)->UpdateData(TRUE);
		
		CString StrTemp,strFinTrame;
		StrTemp.Format(_T("%c"),CharRecv);

		//verification fin de trame
		strFinTrame = TRAMEFINDALSA;
		if(CharRecv == strFinTrame.GetAt(0))
		{	
			((CForm_DalsaSpyder*)frame)->str_recv += _T("\r\n");
		}
		((CForm_DalsaSpyder*)frame)->str_recv += StrTemp;	
		((CForm_DalsaSpyder*)frame)->UpdateData(FALSE);
	 }

}
