
#include "stdafx.h"
#include "MyStrTable.h"
#include "BaseFenetre.h"
#include "AC_Base.h"

CMyStrTable::CMyStrTable(void)
{
	NbStr = 0;
	for(int i = 0; i < NB_MAX_STR;i++)
	{
		Index[i] = 0;
	}

	m_Codes.SetSize(NB_MAX_STR);

}
CMyStrTable::~CMyStrTable(void)
{

}
int CMyStrTable::ChargerFromBase(void)
{

	//Chargement des alarmes depuis la BDD
	#pragma warning (disable: 4995)
	CADORecordset rsStrTable (&theBase.dao_Base);
	int indexStr = 0;
	NbStr = 0;

	CString strvalue = ""
		,strTemp = ""
		,strTemp2 = "";


	if (ACBaseStrRsOpen("","","Message",&rsStrTable))	
	{
		rsStrTable.MoveFirst();

		do
		{
			strTemp = ACBaseGetStr("Code",&rsStrTable);
			m_Codes.SetAt(indexStr,strTemp);

			int iTemp = ACBaseGetInt("Cle",&rsStrTable);
			if(iTemp > NB_MAX_STR)
			{
				//Erreur
				AfxMessageBox("Erreur Chargemet String Table ACYRUS");
			}
			else
			{
				Index[iTemp] = indexStr;
			}

			strTemp.Empty();
			strTemp = ACBaseGetStr("Message",&rsStrTable);
			strTemp2 = ACBaseGetStr("Message2",&rsStrTable);

			if(strTemp2 != _T(""))
			{
				strTemp = strTemp + _T("\n") + strTemp2;
			}

			m_String.SetAtGrow(indexStr,strTemp);
			
			rsStrTable.MoveNext();

			NbStr++;
			indexStr ++;
		}
		while(!rsStrTable.IsEOF());


		rsStrTable.Close();
	}


#pragma warning (default: 4995)

	return NbStr;
}

CString CMyStrTable::GetStr(int CleMessage)
{
	CString str;

	str = m_String.GetAt(Index[CleMessage]);

	return str;
}

CString CMyStrTable::GetStr(CString strCode)
{
	CString str;

	for(int i = 0; i < NB_MAX_STR;i++)
	{
		str = m_Codes.GetAt(i);
		if(str == strCode)
			return m_String.GetAt(i);
	}
	

	return CString("ERROR");
}

CString CMyStrTable::GetCode(int CleMessage)
{
	CString str;

	str = m_Codes.GetAt(Index[CleMessage]);

	return str;
}
