#include "StdAfx.h"
#include "Article.h"

#include"AC_Base.h"
#include"BaseFenetre.h"

CArticle::CArticle(void)
{

	ID = 0;
	Code = _T("");
	Description = _T("");

	Type = 0;

	Article1 = _T("");
	Article2 = _T("");
	Article3 = _T("");
	Article4 = _T("");
	Article5 = _T("");
	Article6 = _T("");

	iParam1 = 0;
	iParam2 = 0;
	CodeDMX = _T("");
	SourceDataRecette = NULL;

}

CArticle::~CArticle(void)
{
}

bool CArticle::Ajouter(void)
{
	CString strRequete=_T("");

	//Verification Doublon
	#pragma warning (disable: 4995)
	CADORecordset rsArticle (&theBase.dao_Base);

	if(ACBaseStrRsOpen(Code, _T("Code"), _T("Articles"),&rsArticle))
	{
		//Article Existant

		if(*SourceDataRecette == RECETTE_SOURCE_MANU)		//TODONEW : Faire le test avant et enlever ce test
		{
			//Récupération des infos
			Code = ACBaseGetStr(_T("Code"),&rsArticle);
			Description = ACBaseGetStr(_T("Description"),&rsArticle);
			Type = ACBaseGetInt(_T("Type"),&rsArticle);
			Article1 = ACBaseGetStr(_T("Article1"),&rsArticle);
			Article2 = ACBaseGetStr(_T("Article2"),&rsArticle);
			Article3 = ACBaseGetStr(_T("Article3"),&rsArticle);
			Article4 = ACBaseGetStr(_T("Article4"),&rsArticle);
			Article5 = ACBaseGetStr(_T("Article5"),&rsArticle);//GCA V1.17
			Article6 = ACBaseGetStr(_T("Article6"),&rsArticle);//#2 Emballage
			iParam1 = ACBaseGetInt(_T("Param1"),&rsArticle);
			iParam2 = ACBaseGetInt(_T("Param2"),&rsArticle);
			CodeDMX = ACBaseGetStr(_T("CodeDMX"),&rsArticle);
			ID = ACBaseGetLong(_T("ID"),&rsArticle);
			
			rsArticle.Close();
			return true;
		}

		//MAJ des infos
		rsArticle.Edit();
		
		ACBaseSetStr(Code, _T("Code"),&rsArticle);
		ACBaseSetStr(Description, _T("Description"),&rsArticle);
		ACBaseSetInt(Type, _T("Type"),&rsArticle);
		ACBaseSetStr(Article1,_T("Article1"),&rsArticle);
		ACBaseSetStr(Article2,_T("Article2"),&rsArticle);
		ACBaseSetStr(Article3,_T("Article3"),&rsArticle);
		ACBaseSetStr(Article4,_T("Article4"),&rsArticle);
		ACBaseSetStr(Article5,_T("Article5"),&rsArticle);//GCA V1.17
		ACBaseSetStr(Article6,_T("Article6"),&rsArticle);//#2 Emballage
		ACBaseSetInt(iParam1,_T("Param1"),&rsArticle);
		ACBaseSetInt(iParam2,_T("Param2"),&rsArticle);
		ACBaseSetStr(CodeDMX,_T("CodeDMX"),&rsArticle);

		rsArticle.Update();

		//Recuperation ID article 
		ID = ACBaseGetLong(_T("ID"),&rsArticle);

		rsArticle.Close();	
		return true;
	
	}
	else
	{
		
		CADORecordset rsIdArticle (&theBase.dao_Base);
	
		try
		{
			//Article a créer
			//#2 Emballage strRequete.Format( _T("INSERT INTO Articles ([Code],[Description],[Type],[Article1],[Article2],[Article3],[Article4],[Article5],[Param1],[Param2],[CodeDMX]) VALUES('%s','%s',%d,'%s','%s','%s','%s','%s',%d,%d,'%s')"),Code,Description,Type,Article1,Article2,Article3,Article4,Article5,iParam1,iParam2,CodeDMX); 
			strRequete.Format( _T("INSERT INTO Articles ([Code],[Description],[Type],[Article1],[Article2],[Article3],[Article4],[Article5],[Param1],[Param2],[CodeDMX],[Article6]) VALUES('%s','%s',%d,'%s','%s','%s','%s','%s',%d,%d,'%s','%s')"),Code,Description,Type,Article1,Article2,Article3,Article4,Article5,iParam1,iParam2,CodeDMX,Article6); 
			theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);

			ID = 0;
			strRequete.Format(_T("SELECT  MAX(ID) AS idMax FROM Coupes;"));
	
			rsIdArticle.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
			
			if (rsIdArticle.IsOpen())
			{
				//V 40 ID = ACBaseGetInt(_T("idMax"),&rsIdArticle);
				ID = ACBaseGetLong(_T("idMax"), &rsIdArticle);
			}

			rsIdArticle.Close();
			
		}
		catch (CException* e)
		{
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause,255);
			e->Delete();
			CString strErreur = _T("ERREUR Création Article \r\nDescription :\n");
			strErreur = strErreur + szCause;

			//Affichage journal événements
			theApp.SetEventsJournalStr(strErreur);
			return false;
		}

		return true;
		
	}
	return false;
}

void CArticle::Copy(CArticle * ArtDest)
{
	ArtDest->Code =	this->Code ;		
	ArtDest->Description =	this->Description ;
								
	ArtDest->Article1 =	this->Article1 ;
	ArtDest->Article2 =	this->Article2 ;
	ArtDest->Article3 =	this->Article3 ;
	ArtDest->Article4 =	this->Article4 ;
	ArtDest->Article5 =	this->Article5 ;
	ArtDest->Article6 =	this->Article6 ;
								
	ArtDest->SourceDataRecette = this->SourceDataRecette ;
	ArtDest->Type =	this->Type ;
								
	ArtDest->iParam1 =	this->iParam1;
	ArtDest->iParam2 =	this->iParam2;
	ArtDest->CodeDMX =	this->CodeDMX;
	ArtDest->ID =	this->ID;
}

void CArticle::Reset()
{
	Code = _T("");
	Description = _T("");

	Article1 = _T("");
	Article2 = _T("");
	Article3 = _T("");
	Article4 = _T("");
	Article5 = _T("");
	Article6 = _T("");

	SourceDataRecette = NULL;
	//Type = 0;

	iParam1 = 0;
	iParam2 = 0;
	CodeDMX = _T("");
	ID = 0;
}

bool CArticle::ChargerFromCode(CString strCode)
{
	Code = strCode;

	CADORecordset rsTemp(&theBase.dao_Base);
	//Recherche de l'article
	if(ACBaseStrRsOpen(Code, _T("Code"), _T("Articles"),&rsTemp))
	{
		ID = ACBaseGetLong(_T("ID"),&rsTemp);
		Description = ACBaseGetStr(_T("Description"),&rsTemp);
		Type = ACBaseGetInt(_T("Type"),&rsTemp);
		Article1 = ACBaseGetStr(_T("Article1"),&rsTemp);
		Article2 = ACBaseGetStr(_T("Article2"),&rsTemp);
		Article3 = ACBaseGetStr(_T("Article3"),&rsTemp);
		Article4 = ACBaseGetStr(_T("Article4"),&rsTemp);
		Article5 = ACBaseGetStr(_T("Article5"),&rsTemp);
		Article6 = ACBaseGetStr(_T("Article6"),&rsTemp);
		iParam1 = ACBaseGetInt(_T("Param1"),&rsTemp);
		iParam2 = ACBaseGetInt(_T("Param2"),&rsTemp);
		CodeDMX = ACBaseGetStr(_T("CodeDMX"),&rsTemp);
		rsTemp.Close();
		return true;
	}
	else
	{
		return false;
	}
}
