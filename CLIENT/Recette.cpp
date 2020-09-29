#include "StdAfx.h"
#include "Recette.h"

#include "BaseFenetre.h"
#include "DialChargement.h"
//V 40 #include "ListBoxNotify.h"

#include"AC_Base.h"

CRecette::CRecette(void)
{
	 IsCSV_OK = false;
	 IsOldCSV_OK = false;
	 IsFinLot_Forced = false;
		
	 IsLoaded = false;
	 Peremption = 0;
	 PeremptionMois = 0;
	 iCyclePulse = 0;
	 
	 Quantite = 0;
	 ArtMinims.iParam2 = RECETTE_COULEUR_NO;

	 strLot = _T("");

	 ArtMinims.Type = RECETTE_TYPEMINIMS;
	 ArtEtui.Type = RECETTE_TYPEETUI; 
	 ArtNotice1.Type = RECETTE_TYPENOTICE; 
	 ArtNotice2.Type = RECETTE_TYPENOTICE; 
	 ArtEtiquette.Type = RECETTE_TYPEETIQ; 
	 //V44.6  ArtEtiqBoite.Type = RECETTE_TYPEETIQBOITE;
	 ArtEmballage.Type = RECETTE_TYPEEMBALLAGE;	//#2 Emballage

	 strcpy_s(CodeDMXLot,""); 
	
	 ArtNotice1.iParam1 = 0;
	 ArtNotice2.iParam1 = 0;
	 Article.iParam1 = 0;
	 ArtEtiquette.iParam1 = 0;

	// ArtEtiqBoite.iParam1 = 0;//GCA V3.4
	

	 GestionNotice1 = 0;
	 GestionNotice2 = 0;

	for(int i = 0; i < 2 ; i++)
	{
		NbBoiteOK[i] = 0;
		NbBoiteNOK[i] = 0;
		NbBoitePrelev[i] = 0;

		NbEtuiOK[i] = 0;
		NbEtuiNOK[i] = 0;
		//#3 DMX UNUSED NbEtuiPleinNOK[i] = 0;

		NbNotice1OK[i] = 0;
		NbNotice1NOK[i] = 0;
		NbNotice2OK[i] = 0;
		NbNotice2NOK[i] = 0;

		NbPieceOK[i] = 0;
		NbPieceNOK[i] = 0;

		NbRebutVision[i] = 0;
		NbRebutPoids[i] = 0;

		NbVisionOK[i] = 0;
		NbVisionDMX[i] = 0;
		NbVisionTache[i] = 0;
		NbVisionTacheUV[i] = 0;
		NbVisionCB[i] = 0;


	}

	 ID = 0;
	 IDArticle = 0;

	 SourceData = -1;
	 Reprise = false;
	 iStatut = RECETTE_STATUT_NO;

	UserForcage  = _T("");
	DateForcage  = _T("");
	RepertoireImages = _T("");
	
	QuantiteParBoite = 20;
	bIsEnCours = false;//GCA V1.17
	TolEtuiPlein_cg = 0;

}

CRecette::~CRecette(void)
{
}


bool CRecette::ChargerFromCSVAttente(CString l_OF_Number)
{
	//TODO TRACA : Faire une fonction de type int avec
	//-1 = Erreur dans la fonction
	//0 = Pas d'enregistrement trouvé
	//1 = Chargement OK

	strErreur = _T("");

	long lTemp = 0;
	CString strTemp;
	bool bDateOK = false;

#pragma warning( disable: 4995)
	CADORecordset rs (&theBase.dao_Base);

	if (ACBaseStrRsOpen(l_OF_Number, _T("[Shop Order Number]"), _T("CSV_TEMP"),&rs))
	{
		//MAJ des valeurs de la recette
		
		rs.MoveFirst();//GCA V1.18

		OfNum = l_OF_Number;
		//Item Number => Numero d'article
		//Numero de lot
		strLot = ACBaseGetStr(_T("Lot Number"),&rs);
		Quantite =(int) ACBaseGetDouble(_T("Quantity Required"),&rs);
		Peremption = ACBaseGetInt(_T("Shelf Life Days"),&rs);
		PeremptionMois =(int) floor(Peremption / 30.);

		lTemp =  ACBaseGetLong(_T("Due Date"),&rs);

		strTemp = _T("");
		strTemp  = ACBaseGetStr(_T("lot identifiant"),&rs);
		CStringA strATemp = CStringA(strTemp);
		strcpy_s(CodeDMXLot, strATemp.GetBuffer());
		strATemp.ReleaseBuffer();

		Article.Code = ACBaseGetStr(_T("Item Number"),&rs);
		Article.Description = ACBaseGetStr(_T("Item Description"),&rs);
		

		strTemp = ACBaseGetStr(_T("Item Group Code"),&rs);
		if(strTemp == _T("AB09"))
		{
			Article.Type = RECETTE_TYPESACHET;	
			Article.iParam1 = RECETTE_TYPESACHET;	//V25 Gestion poids
		}
		else if(strTemp == _T("AB10"))
		{
			Article.Type = RECETTE_TYPEBLISTER;
			Article.iParam1 = RECETTE_TYPEBLISTER;	//V25 Gestion poids
		}
		
		//lecture des articles liés 
		do
		{
			strTemp = ACBaseGetStr(_T("Itm Cls"),&rs);
			if(strTemp == _T("AK"))//Sachet
			{
				ArtMinims.ID = 0;
				ArtMinims.Code = ACBaseGetStr(_T("Child Item Number"),&rs);
				ArtMinims.CodeDMX = ACBaseGetStr(_T("lot identifiant"),&rs);//A VOIR doublé avec CodeDMXLot
				ArtMinims.Description = ACBaseGetStr(_T("Child Item Description"),&rs);
				ArtMinims.Type = RECETTE_TYPEMINIMS;
				ArtMinims.Article1 = _T("");
				ArtMinims.Article2 = _T("");
				ArtMinims.Article3 = _T("");
				ArtMinims.Article4 = _T("");
				ArtMinims.Article5 = _T("");	//GCA V1.17
				ArtMinims.Article6 = _T("");	//#2 Emballage
				ArtMinims.iParam1 = Article.Type;
				ArtMinims.iParam2 = 0;	//Pas de couleur pour les sachets
				//V25 Gestion poids
				int xx = ACBaseGetInt(_T("Packaging Size"),&rs);
				if(xx == 4)
				{
					Article.iParam1 = 4;	//V25 Gestion poids : Cas particulier 4 millilitres
				}
			}
			else if(strTemp == _T("AL"))//Blister
			{
				ArtMinims.ID = 0;
				ArtMinims.Code = ACBaseGetStr(_T("Child Item Number"),&rs);
				ArtMinims.CodeDMX = ACBaseGetStr(_T("lot identifiant"),&rs);
				ArtMinims.Description = ACBaseGetStr(_T("Child Item Description"),&rs);
				ArtMinims.Type = RECETTE_TYPEMINIMS;
				ArtMinims.Article1 = _T("");
				ArtMinims.Article2 = _T("");
				ArtMinims.Article3 = _T("");
				ArtMinims.Article4 = _T("");
				ArtMinims.Article5 = _T("");//GCA V1.17
				ArtMinims.Article6 = _T("");	//#2 Emballage
				ArtMinims.iParam1 = Article.Type;
				ArtMinims.iParam2 = ACBaseGetInt(_T("Standard Package Type"),&rs);
				//V25 Gestion poids
				int xx = ACBaseGetInt(_T("Packaging Size"),&rs);
				if(xx == 4)
				{
					Article.iParam1 = 4;	//V25 Gestion poids : Cas particulier 4 millilitres
				}
			}
			else if(strTemp == _T("BI"))//Etui
			{
				ArtEtui.ID = 0;
				ArtEtui.Code = ACBaseGetStr(_T("Child Item Number"),&rs);
				ArtEtui.CodeDMX =ACBaseGetStr(_T("Code à Lire"),&rs);
				ArtEtui.Description = ACBaseGetStr(_T("Child Item Description"),&rs);
				ArtEtui.Type = RECETTE_TYPEETUI;
				ArtEtui.Article1 = _T("");
				ArtEtui.Article2 = _T("");
				ArtEtui.Article3 = _T("");
				ArtEtui.Article4 = _T("");
				ArtEtui.Article5 = _T("");//GCA V1.17
				ArtEtui.Article6 = _T("");	//#2 Emballage
				ArtEtui.iParam1 = 0;
				ArtEtui.iParam2 = 0;
			}
			else if(strTemp == _T("BK"))//Notice 1
			{
				ArtNotice1.ID = 0;
				ArtNotice1.Code = ACBaseGetStr(_T("Child Item Number"),&rs);
				ArtNotice1.CodeDMX = ACBaseGetStr(_T("Code à Lire"),&rs);
				ArtNotice1.Description = ACBaseGetStr(_T("Child Item Description"),&rs);
				ArtNotice1.Type = RECETTE_TYPENOTICE;
				ArtNotice1.Article1 = _T("");
				ArtNotice1.Article2 = _T("");
				ArtNotice1.Article3 = _T("");
				ArtNotice1.Article4 = _T("");
				ArtNotice1.Article5 = _T("");	//GCA V1.17
				ArtNotice1.Article6 = _T("");	//#2 Emballage
				ArtNotice1.iParam1 = ACBaseGetInt(_T("Packaging Size"),&rs);//TODO Conversion Format
				ArtNotice1.iParam2 = 0;

				//Gestion format Notice avec table de conversion format CSV_SizeConv
				CADORecordset rsSize (&theBase.dao_Base);
				
				strTemp.Format(_T("%d"),ArtNotice1.iParam1);

				if (ACBaseStrRsOpen(strTemp, _T("[CSV Size]"), _T("CSV_SizeConv"),&rsSize))
				{
					ArtNotice1.iParam1 = ACBaseGetInt(_T("ACYRUS Size"),&rsSize);
				}
				else
				{
					ArtNotice1.iParam1 = RECETTE_FORMAT_2;//Format 2 par défaut
				}
				if(rsSize.IsOpen()) rsSize.Close();

			}
			else if(strTemp == _T("BO"))//Notice 2
			{
				ArtNotice2.ID = 0;
				ArtNotice2.Code = ACBaseGetStr(_T("Child Item Number"),&rs);
				ArtNotice2.CodeDMX = ACBaseGetStr(_T("Code à Lire"),&rs);
				ArtNotice2.Description = ACBaseGetStr(_T("Child Item Description"),&rs);
				ArtNotice2.Type = RECETTE_TYPENOTICE;
				ArtNotice2.Article1 = _T("");
				ArtNotice2.Article2 = _T("");
				ArtNotice2.Article3 = _T("");
				ArtNotice2.Article4 = _T("");
				ArtNotice2.Article5 = _T("");//GCA V1.17
				ArtNotice2.Article6 = _T("");	//#2 Emballage
				ArtNotice2.iParam1 = ACBaseGetInt(_T("Packaging Size"),&rs);
				ArtNotice2.iParam2 = 0;

				//Gestion format Notice avec table de conversion format CSV_SizeConv
				CADORecordset rsSize (&theBase.dao_Base);

				strTemp.Format(_T("%d"),ArtNotice2.iParam1);

				if (ACBaseStrRsOpen(strTemp, _T("[CSV Size]"), _T("CSV_SizeConv"),&rsSize))
				{
					ArtNotice2.iParam1 = ACBaseGetInt(_T("ACYRUS Size"),&rsSize);
				}
				else
				{
					ArtNotice2.iParam1 = RECETTE_FORMAT_2;//Format 2 par défaut
				}
				if(rsSize.IsOpen()) rsSize.Close();

			}
			else if(strTemp == _T("BJ"))//Etiquette Finale
			{
				ArtEtiquette.ID = 0;
				ArtEtiquette.Code = ACBaseGetStr(_T("Child Item Number"),&rs);
				ArtEtiquette.CodeDMX = ACBaseGetStr(_T("Code à Lire"),&rs);
				ArtEtiquette.Description = ACBaseGetStr(_T("Child Item Description"),&rs);
				ArtEtiquette.Type = RECETTE_TYPEETIQ;
				ArtEtiquette.Article1 = _T("");
				ArtEtiquette.Article2 = _T("");
				ArtEtiquette.Article3 = _T("");
				ArtEtiquette.Article4 = _T("");
				ArtEtiquette.Article5 = _T("");//GCA V1.17
				ArtEtiquette.Article6 = _T("");	//#2 Emballage
				ArtEtiquette.iParam2 = 0;
				
				ArtEtiquette.iParam1 = RECETTE_FORMAT_NO;
				//Gestion format etiquette si presence code dans table petit format
				//sinon grand format
				CADORecordset rsFormat (&theBase.dao_Base);
				if (ACBaseStrRsOpen(ArtEtiquette.Code, _T("Code"), _T("PetitFormatEtiq"),&rsFormat))
				{
					ArtEtiquette.iParam1 = RECETTE_FORMAT_1;
				}
				else
				{
					ArtEtiquette.iParam1 = RECETTE_FORMAT_2;
				}
				if(rsFormat.IsOpen()) rsFormat.Close();
				
			}
			else if(strTemp == _T("BM"))	//#2 Emballage
			{
				ArtEmballage.ID = 0;
				ArtEmballage.Code = ACBaseGetStr(_T("Child Item Number"),&rs);
				ArtEmballage.CodeDMX =ACBaseGetStr(_T("Code à Lire"),&rs);
				ArtEmballage.Description = ACBaseGetStr(_T("Child Item Description"),&rs);
				ArtEmballage.Type = RECETTE_TYPEEMBALLAGE;
				ArtEmballage.Article1 = _T("");
				ArtEmballage.Article2 = _T("");
				ArtEmballage.Article3 = _T("");
				ArtEmballage.Article4 = _T("");
				ArtEmballage.Article5 = _T("");
				ArtEmballage.Article6 = _T("");
				ArtEmballage.iParam1 = 0;
				ArtEmballage.iParam2 = 0;
			}
		
			rs.MoveNext();
		}
		while(!rs.IsEOF());

		//Mise à jour de la nomenclature article
		Article.Article1 = ArtNotice1.Code;
		Article.Article2 = ArtNotice2.Code;
		Article.Article3 = ArtEtui.Code;
		Article.Article4 = ArtMinims.Code;
		Article.Article5 = ArtEtiquette.Code;//GCA V1.17
		Article.Article6 = ArtEmballage.Code;//#2 Emballage


		rs.Close();

		//A VOIR Recuperation de la date du jour sous la forme JJMMAAAA
		CTime time;
		tm t1; 
		CString temp;
		int iJour,iMois,iAnnee,iJourEff,iMoisEff,iAnneeEff;
		
		//recupération de la date et l'heure 
		time= CTime::GetCurrentTime();
		t1 = *(time.GetLocalTm( &t1 ));

		iJour = t1.tm_mday;
		iMois = (t1.tm_mon+1);//+1-> janvier =0
		iAnnee = (t1.tm_year+1900);
		
		temp.Format(_T("%08ld"),lTemp);
#ifdef _UNICODE
		iJourEff = _wtoi(temp.Mid(0, 2));
		iMoisEff = _wtoi(temp.Mid(2, 2));
		iAnneeEff = _wtoi(temp.Mid(4, 4));
#else
		iJourEff = atoi(temp.Mid(0,2));
		iMoisEff =atoi(temp.Mid(2,2));
		iAnneeEff = atoi(temp.Mid(4,4));
#endif
		
		//V25 Gestion poids
		//if(Article.Type == RECETTE_TYPESACHET)
		//{
		//	PoidsNominal = (float) pParamSachetNom->value;
		//	PoidsMini = (float) pParamSachetMin->value;
		//	PoidsMaxi = (float) pParamSachetMax->value;
		//}
		//else
		//{
		//	PoidsNominal = (float) pParamBlisterNom->value;
		//	PoidsMini = (float) pParamBlisterMin->value;
		//	PoidsMaxi = (float) pParamBlisterMax->value;
		//}
		switch (Article.iParam1)
		{
			case RECETTE_TYPEBLISTER:
				PoidsNominal = (float) pParamBlisterNom->value;
				PoidsMini = (float) pParamBlisterMin->value;
				PoidsMaxi = (float) pParamBlisterMax->value;
				TolEtuiPlein_cg = (int)(pParamToleranceEtuiBlister->value * 100);
				break;
			case RECETTE_TYPESACHET:
				PoidsNominal = (float) pParamSachetNom->value;
				PoidsMini = (float) pParamSachetMin->value;
				PoidsMaxi = (float) pParamSachetMax->value;
				TolEtuiPlein_cg = (int)(pParamToleranceEtuiSachet->value * 100);
				break;
			case RECETTE_TYPESACHET_4ML:
				PoidsNominal = (float) pParamSachet4mlNom->value;
				PoidsMini = (float) pParamSachet4mlMin->value;
				PoidsMaxi = (float) pParamSachet4mlMax->value;
				TolEtuiPlein_cg = (int)(pParamToleranceEtuiSachet4ml->value * 100);
				break;
		}

		PoidsNominal_mg =(int)( PoidsNominal * 1000.);
		PoidsMini_mg =(int)( PoidsMini * 1000.);
		PoidsMaxi_mg =(int)( PoidsMaxi * 1000.);

		//if(Article.Type == RECETTE_TYPESACHET)
		//{
		//	TolEtuiPlein_cg = (int)(pParamToleranceEtuiSachet->value * 100);
		//}
		//else
		//{
		//	TolEtuiPlein_cg = (int)(pParamToleranceEtuiBlister->value * 100);
		//}

		IsLoaded = true;
		
	}
	else
	{
		//Erreur OF inexistant
		strErreur = _T("Erreur Chargement OF:  OF inexistant (aucun enregistrement)");
		IsLoaded = false;
	}

	return IsLoaded;
}

bool CRecette::Copy(CRecette * Recette_Dest)
{
	Recette_Dest->m_OPCClient = this->m_OPCClient;;
	//Alarme
	Recette_Dest->Alarme = this->Alarme;
	Recette_Dest->strErreur = this->strErreur;

	this->Article.Copy(&Recette_Dest->Article);
	this->ArtMinims.Copy(&Recette_Dest->ArtMinims);
	this->ArtNotice1.Copy(&Recette_Dest->ArtNotice1);
	this->ArtNotice2.Copy(&Recette_Dest->ArtNotice2);
	this->ArtEtui.Copy(&Recette_Dest->ArtEtui);
	this->ArtEtiquette.Copy(&Recette_Dest->ArtEtiquette);

	//V44.6 this->ArtEtiqBoite.Copy(&Recette_Dest->ArtEtiqBoite);
	this->ArtEmballage.Copy(&Recette_Dest->ArtEmballage);	//#2 Emballage

	Recette_Dest->strLot = this->strLot;
	
	Recette_Dest->IsCSV_OK = this->IsCSV_OK;
	Recette_Dest->IsOldCSV_OK = this->IsOldCSV_OK;
	Recette_Dest->IsLoaded = this->IsLoaded;
	Recette_Dest->IsFinLot_Forced = this->IsFinLot_Forced;

	Recette_Dest->OfNum = this->OfNum;

	Recette_Dest->Quantite = this->Quantite;
	Recette_Dest->Peremption = this->Peremption;
	Recette_Dest->PeremptionMois = this->PeremptionMois;

	//Datamatrix
	strcpy_s(Recette_Dest->CodeDMXLot , this->CodeDMXLot);

	Recette_Dest->GestionNotice1 = this->GestionNotice1;
	Recette_Dest->GestionNotice2 = this->GestionNotice2;
	
	//Process

	for(int i = 0; i < 2 ; i++)
	{
		Recette_Dest->NbBoiteOK[i] = this->NbBoiteOK[i];
		Recette_Dest->NbBoiteNOK[i] = this->NbBoiteNOK[i];
		Recette_Dest->NbBoitePrelev[i] = this->NbBoitePrelev[i];

		Recette_Dest->NbEtuiOK[i] = this->NbEtuiOK[i];
		Recette_Dest->NbEtuiNOK[i] = this->NbEtuiNOK[i];
		//#3 DMX UNUSED Recette_Dest->NbEtuiPleinNOK[i] = this->NbEtuiPleinNOK[i];

		Recette_Dest->NbNotice1OK[i] = this->NbNotice1OK[i];
		Recette_Dest->NbNotice1NOK[i] = this->NbNotice1NOK[i];
		Recette_Dest->NbNotice2OK[i] = this->NbNotice2OK[i];

		Recette_Dest->NbNotice2NOK[i] = this->NbNotice2NOK[i];
		Recette_Dest->NbPieceOK[i] = this->NbPieceOK[i];
		Recette_Dest->NbPieceNOK[i] = this->NbPieceNOK[i];

		Recette_Dest->NbRebutVision[i] = this->NbRebutVision[i];
		Recette_Dest->NbRebutPoids[i] = this->NbRebutPoids[i];

		Recette_Dest->NbVisionOK[i] = this->NbVisionOK[i];
		Recette_Dest->NbVisionDMX[i] = this->NbVisionDMX[i];
		Recette_Dest->NbVisionTache[i] = this->NbVisionTache[i];
		Recette_Dest->NbVisionTacheUV[i] = this->NbVisionTacheUV[i];
		Recette_Dest->NbVisionCB[i] = this->NbVisionCB[i];
	}

	Recette_Dest->PoidsNominal = 	this->PoidsNominal;
	Recette_Dest->PoidsMini = this->PoidsMini;
	Recette_Dest->PoidsMaxi = this->PoidsMaxi;

	Recette_Dest->PoidsNominal_mg = this->PoidsNominal_mg;
	Recette_Dest->PoidsMini_mg	  = this->PoidsMini_mg;
	Recette_Dest->PoidsMaxi_mg    = this->PoidsMaxi_mg ;

	Recette_Dest->SourceData = this->SourceData;
	Recette_Dest->Reprise = this->Reprise;
	Recette_Dest->iStatut = this->iStatut;
	Recette_Dest->ID = this->ID;
	Recette_Dest->IDArticle = this->IDArticle;

	Recette_Dest->UserForcage = this->UserForcage;
	Recette_Dest->DateForcage = this->DateForcage;

	Recette_Dest->bIsEnCours = this->bIsEnCours;

	Recette_Dest->TolEtuiPlein_cg = this->TolEtuiPlein_cg;

	return true;
}

int CRecette::Reset(void)
{
	 OfNum = _T("");
	 IsCSV_OK = false;
	 //GCA V1.17 IsOldCSV_OK = false;

	 IsLoaded = false;
	 Peremption = 0;
	 PeremptionMois = 0;
	 IsFinLot_Forced = false;
	 
	 //TODO Article.Reset();
	 Quantite = 0;

	 strLot = _T("");

	 Article.Reset();
	 ArtMinims.Reset();
	 ArtEtui.Reset();
	 ArtNotice1.Reset();
	 ArtNotice2.Reset();
	 ArtEtiquette.Reset();
	 //V44.6  ArtEtiqBoite.Reset();
	 ArtEmballage.Reset();	//#2 Emballage

	 strcpy_s(CodeDMXLot,""); 

	 GestionNotice1 = 0;
	 GestionNotice2 = 0;
	

	for(int i = 0; i < 2 ; i++)
	{
		NbBoiteOK[i] = 0;
		NbBoiteNOK[i] = 0;
		NbBoitePrelev[i] = 0;

		NbEtuiOK[i] = 0;
		NbEtuiNOK[i] = 0;
		//#3 DMX UNUSED NbEtuiPleinNOK[i] = 0;

		NbNotice1OK[i] = 0;
		NbNotice1NOK[i] = 0;
		NbNotice2OK[i] = 0;
		NbNotice2NOK[i] = 0;

		NbPieceOK[i] = 0;
		NbPieceNOK[i] = 0;

		NbRebutVision[i] = 0;
		NbRebutPoids[i] = 0;

		NbVisionOK[i] = 0;
		NbVisionDMX[i] = 0;
		NbVisionTache[i] = 0;
		NbVisionTacheUV[i] = 0;
		NbVisionCB[i] = 0;
	}

	Article.Description = _T("");

	PoidsNominal = 0;
	PoidsMini	  = 0;
	PoidsMaxi    = 0 ;

	PoidsNominal_mg = 0;
	PoidsMini_mg	  = 0;
	PoidsMaxi_mg    = 0 ;
	
	 Reprise = false;
	 SourceData = -1;
	 iStatut = RECETTE_STATUT_NO;

	 ID = 0;
	 IDArticle = 0;

	 bIsEnCours = false;

	TolEtuiPlein_cg = 0;
	RepertoireImages.Format(_T("%s\\%s"),theConfig.str_RepertoireData,NOM_DOSSIER_IMAGES);
	return 0;
}

int CRecette::SavePoidsEtalonPeson(int iPeson)
{
	short Modes[6] = {NULL};
	int iPoidsEta1[6] = {NULL};
	int iPoidsEta2[6] = {NULL};

	//Sauvegarde des valeurs du peson en cours
	iPoidsEta1[iPeson] = theApp.Pesons[iPeson].PoidsEtalon1;
	iPoidsEta2[iPeson] = theApp.Pesons[iPeson].PoidsEtalon2;
	Modes[iPeson] = theApp.Pesons[iPeson].PesonMode;

	//Sauvegarde dans BDD
	#pragma warning (disable: 4995)
	CString strRequete=_T("");
	
	strRequete.Format( _T("INSERT INTO LotPoids ([IDOF],[IDLot],[Date],[Utilisateur],[Type],[EtatPeson1],[Peson1Etalon1],[Peson1Etalon2],[EtatPeson2],[Peson2Etalon1],[Peson2Etalon2],[EtatPeson3],[Peson3Etalon1],[Peson3Etalon2],[EtatPeson4],[Peson4Etalon1],[Peson4Etalon2],[EtatPeson5],[Peson5Etalon1],[Peson5Etalon2],[EtatPeson6],[Peson6Etalon1],[Peson6Etalon2],[E1_Nom],[E1_Min],[E1_Max],[E2_Nom],[E2_Min],[E2_Max]) VALUES(%ld,%ld,GetDate(),'%s',2,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)"),ID,IDLot,theApp.CurrentUser.strCode,Modes[0],iPoidsEta1[0]/1000.,iPoidsEta2[0]/1000.,Modes[1],iPoidsEta1[1]/1000.,iPoidsEta2[1]/1000.,Modes[2],iPoidsEta1[2]/1000.,iPoidsEta2[2]/1000.,Modes[3],iPoidsEta1[3]/1000.,iPoidsEta2[3]/1000.,Modes[4],iPoidsEta1[4]/1000.,iPoidsEta2[4]/1000.,Modes[5],iPoidsEta1[5]/1000.,iPoidsEta2[5]/1000.,(float) pParamPoidsEtalon1->value,(float) pParamPoidsMinEtalon1->value,(float) pParamPoidsMaxEtalon1->value,(float) pParamPoidsEtalon2->value,(float) pParamPoidsMinEtalon2->value,(float) pParamPoidsMaxEtalon2->value); //Type = 2 => VErif. Etalonnage
	try
	{
		theBase.dao_Base.Execute(strRequete);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Ecriture CRecette::Verif Etalonnage Fct:SavePoidsEtalonPeson\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}

	return 0;
}

int CRecette::SavePoidsEtalon(void)
{
	//Sauvegarde des valeurs des poids etalon
	for(int i = 0; i < 6 ;i ++)
	{
		PoidsEtalonSaved[i][0] = theApp.Pesons[i].PoidsEtalon1;
		PoidsEtalonSaved[i][1] = theApp.Pesons[i].PoidsEtalon2;
	}
	
	//Sauvegarde dans BDD
	#pragma warning (disable: 4995)
	CString strRequete=_T("");
	
	strRequete.Format( _T("INSERT INTO LotPoids ([IDOF],[IDLot],[Date],[Utilisateur],[Type],[EtatPeson1],[Peson1Etalon1],[Peson1Etalon2],[EtatPeson2],[Peson2Etalon1],[Peson2Etalon2],[EtatPeson3],[Peson3Etalon1],[Peson3Etalon2],[EtatPeson4],[Peson4Etalon1],[Peson4Etalon2],[EtatPeson5],[Peson5Etalon1],[Peson5Etalon2],[EtatPeson6],[Peson6Etalon1],[Peson6Etalon2],[E1_Nom],[E1_Min],[E1_Max],[E2_Nom],[E2_Min],[E2_Max]) VALUES(%ld,%ld, GetDate(),'%s',0,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)")
		,ID,IDLot,theApp.CurrentUser.strCode,theApp.Pesons[0].PesonMode,PoidsEtalonSaved[0][0]/1000.,PoidsEtalonSaved[0][1]/1000.,theApp.Pesons[1].PesonMode,PoidsEtalonSaved[1][0]/1000.,PoidsEtalonSaved[1][1]/1000.,theApp.Pesons[2].PesonMode,PoidsEtalonSaved[2][0]/1000.,PoidsEtalonSaved[2][1]/1000.,theApp.Pesons[3].PesonMode,PoidsEtalonSaved[3][0]/1000.,PoidsEtalonSaved[3][1]/1000.,theApp.Pesons[4].PesonMode,PoidsEtalonSaved[4][0]/1000.,PoidsEtalonSaved[4][1]/1000.,theApp.Pesons[5].PesonMode,PoidsEtalonSaved[5][0]/1000.,PoidsEtalonSaved[5][1]/1000.,(float) pParamPoidsEtalon1->value,(float) pParamPoidsMinEtalon1->value,(float) pParamPoidsMaxEtalon1->value,(float) pParamPoidsEtalon2->value,(float) pParamPoidsMinEtalon2->value,(float) pParamPoidsMaxEtalon2->value); //Type = 0 => Validation DEBUT
	try
	{
		theBase.dao_Base.Execute(strRequete);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Ecriture CRecette::Valid Debut Lot\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}

	return 0;
}
int CRecette::SavePoidsEtalonFin(void)
{
	//Sauvegarde des valeurs des poids etalon
	for(int i = 0; i < 6 ;i ++)
	{
		PoidsEtalonSavedFin[i][0] = theApp.Pesons[i].PoidsEtalon1;
		PoidsEtalonSavedFin[i][1] = theApp.Pesons[i].PoidsEtalon2;
	}

	//Sauvegarde dans BDD
	#pragma warning (disable: 4995)
	CString strRequete=_T("");
	
	//V2.53 : Remplacement PoidsEtalonSaved->PoidsEtalonSavedFin
	strRequete.Format( _T("INSERT INTO LotPoids ([IDOF],[IDLot],[Date],[Utilisateur],[Type],[EtatPeson1],[Peson1Etalon1],[Peson1Etalon2],[EtatPeson2],[Peson2Etalon1],[Peson2Etalon2],[EtatPeson3],[Peson3Etalon1],[Peson3Etalon2],[EtatPeson4],[Peson4Etalon1],[Peson4Etalon2],[EtatPeson5],[Peson5Etalon1],[Peson5Etalon2],[EtatPeson6],[Peson6Etalon1],[Peson6Etalon2],[E1_Nom],[E1_Min],[E1_Max],[E2_Nom],[E2_Min],[E2_Max]) VALUES(%ld,%ld,GetDate(),'%s',1,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%d,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)")
		,ID,IDLot,theApp.CurrentUser.strCode,theApp.Pesons[0].PesonMode,PoidsEtalonSavedFin[0][0]/1000.,PoidsEtalonSavedFin[0][1]/1000.,theApp.Pesons[1].PesonMode,PoidsEtalonSavedFin[1][0]/1000.,PoidsEtalonSavedFin[1][1]/1000.,theApp.Pesons[2].PesonMode,PoidsEtalonSavedFin[2][0]/1000.,PoidsEtalonSavedFin[2][1]/1000.,theApp.Pesons[3].PesonMode,PoidsEtalonSavedFin[3][0]/1000.,PoidsEtalonSavedFin[3][1]/1000.,theApp.Pesons[4].PesonMode,PoidsEtalonSavedFin[4][0]/1000.,PoidsEtalonSavedFin[4][1]/1000.,theApp.Pesons[5].PesonMode,PoidsEtalonSavedFin[5][0]/1000.,PoidsEtalonSavedFin[5][1]/1000.,(float) pParamPoidsEtalon1->value,(float) pParamPoidsMinEtalon1->value,(float) pParamPoidsMaxEtalon1->value,(float) pParamPoidsEtalon2->value,(float) pParamPoidsMinEtalon2->value,(float) pParamPoidsMaxEtalon2->value);  //Type = 1 => Validation FIN
	try
	{
		theBase.dao_Base.Execute(strRequete);
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Ecriture CRecette::Valid Debut Lot\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}
	return 0;
}

int CRecette::Ajouter(void)
{
	
	CString strCom = _T("");
	CString strRequete=_T("");
	CString strUser = _T("");
	
	if(SourceData == RECETTE_SOURCE_CSV)
		strUser = theApp.CurrentUser.strCode;
	else
		strUser = UserForcage;

	CADORecordset rsTemp (&theBase.dao_Base);
	if(ACBaseStrRsOpen(OfNum, _T("Numero"), _T("Coupes"),&rsTemp))
	{
		//Lot existant => Récupération de l'ID
		ID = ACBaseGetLong(_T("ID"),&rsTemp);
		rsTemp.Close();
		return 1;
	}

	//Lot inexistant => Ajout dans la base 
	//#2 Emballage strRequete.Format( _T("INSERT INTO Coupes ([Type],[IDLot],[DescriptionArticle],[IDArticle],[DateDebut],[Machine],[UtilisateurDebut],[Article],[Lot],[Numero],[Quantite],[Peremption],[FormatEtiquette],[CodeEtiquette],[FormatNotice1],[FormatNotice2],[DescriptionEtui],[DescriptionNotice1],[DescriptionNotice2],[CodeEtui],[CodeNotice1],[CodeNotice2],[SourceData],[Statut],[DMXVision],[Minims],[DMXEtui],[DMXNotice1],[DMXNotice2]) VALUES(%d,%ld,'%s',%ld,GetDate(),'%s','%s','%s','%s','%s',%d,%d,%d,'%s',%d,%d,'%s','%s','%s','%s','%s','%s',%d,%d,'%s','%s','%s','%s','%s')"),ArtMinims.iParam1,IDLot,Article.Description,IDArticle,theConfig.str_Machine,strUser,Article.Code,strLot,OfNum,Quantite,Peremption,ArtEtiquette.iParam1,ArtEtiquette.Code,ArtNotice1.iParam1 ,ArtNotice2.iParam1,ArtEtui.Description,ArtNotice1.Description,ArtNotice2.Description,ArtEtui.Code,ArtNotice1.Code,ArtNotice2.Code,SourceData,iStatut,CodeDMXLot,ArtMinims.Code,ArtEtui.CodeDMX,ArtNotice1.CodeDMX,ArtNotice2.CodeDMX); 
	//V25 Gestion poids strRequete.Format( _T("INSERT INTO Coupes ([Type],[IDLot],[DescriptionArticle],[IDArticle],[DateDebut],[Machine],[UtilisateurDebut],[Article],[Lot],[Numero],[Quantite],[Peremption],[FormatEtiquette],[CodeEtiquette],[FormatNotice1],[FormatNotice2],[DescriptionEtui],[DescriptionNotice1],[DescriptionNotice2],[CodeEtui],[CodeNotice1],[CodeNotice2],[SourceData],[Statut],[DMXVision],[Minims],[DMXEtui],[DMXNotice1],[DMXNotice2],[CodeEmballage],[DescriptionEmballage],[DMXEmballage]) VALUES(%d,%ld,'%s',%ld,GetDate(),'%s','%s','%s','%s','%s',%d,%d,%d,'%s',%d,%d,'%s','%s','%s','%s','%s','%s',%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s')"),ArtMinims.iParam1,IDLot,Article.Description,IDArticle,theConfig.str_Machine,strUser,Article.Code,strLot,OfNum,Quantite,Peremption,ArtEtiquette.iParam1,ArtEtiquette.Code,ArtNotice1.iParam1 ,ArtNotice2.iParam1,ArtEtui.Description,ArtNotice1.Description,ArtNotice2.Description,ArtEtui.Code,ArtNotice1.Code,ArtNotice2.Code,SourceData,iStatut,CodeDMXLot,ArtMinims.Code,ArtEtui.CodeDMX,ArtNotice1.CodeDMX,ArtNotice2.CodeDMX,ArtEmballage.Code,ArtEmballage.Description,ArtEmballage.CodeDMX); 
	//V44.6 strRequete.Format( _T("INSERT INTO Coupes ([Type],[IDLot],[DescriptionArticle],[IDArticle],[DateDebut],[Machine],[UtilisateurDebut],[Article],[Lot],[Numero],[Quantite],[Peremption],[FormatEtiquette],[CodeEtiquette],[FormatNotice1],[FormatNotice2],[DescriptionEtui],[DescriptionNotice1],[DescriptionNotice2],[CodeEtui],[CodeNotice1],[CodeNotice2],[SourceData],[Statut],[DMXVision],[Minims],[DMXEtui],[DMXNotice1],[DMXNotice2],[CodeEmballage],[DescriptionEmballage],[DMXEmballage],[PoidsType]) VALUES(%d,%ld,'%s',%ld,GetDate(),'%s','%s','%s','%s','%s',%d,%d,%d,'%s',%d,%d,'%s','%s','%s','%s','%s','%s',%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d)"),ArtMinims.iParam1,IDLot,Article.Description,IDArticle,theConfig.str_Machine,strUser,Article.Code,strLot,OfNum,Quantite,Peremption,ArtEtiquette.iParam1,ArtEtiquette.Code,ArtNotice1.iParam1 ,ArtNotice2.iParam1,ArtEtui.Description,ArtNotice1.Description,ArtNotice2.Description,ArtEtui.Code,ArtNotice1.Code,ArtNotice2.Code,SourceData,iStatut,CodeDMXLot,ArtMinims.Code,ArtEtui.CodeDMX,ArtNotice1.CodeDMX,ArtNotice2.CodeDMX,ArtEmballage.Code,ArtEmballage.Description,ArtEmballage.CodeDMX,Article.iParam1); 

	strRequete.Format(_T("INSERT INTO Coupes ([Type],[IDLot],[DescriptionArticle],[IDArticle],[DateDebut],[Machine],[UtilisateurDebut],[Article],[Lot],[Numero],[Quantite],[Peremption],[FormatEtiquette],[CodeEtiquette],[FormatNotice1],[FormatNotice2],[DescriptionEtui],[DescriptionNotice1],[DescriptionNotice2],[CodeEtui],[CodeNotice1],[CodeNotice2],[SourceData],[Statut],[DMXVision],[Minims],[DMXEtui],[DMXNotice1],[DMXNotice2],[CodeEmballage],[DescriptionEmballage],[DMXEmballage],[PoidsType],[DMXEtiquette]) VALUES(%d,%ld,'%s',%ld,GetDate(),'%s','%s','%s','%s','%s',%d,%d,%d,'%s',%d,%d,'%s','%s','%s','%s','%s','%s',%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s')"),
		ArtMinims.iParam1, IDLot, Article.Description, IDArticle, theConfig.str_Machine, strUser, Article.Code, strLot, OfNum, Quantite, Peremption, ArtEtiquette.iParam1, ArtEtiquette.Code, ArtNotice1.iParam1, ArtNotice2.iParam1, ArtEtui.Description, ArtNotice1.Description, ArtNotice2.Description, ArtEtui.Code, ArtNotice1.Code, ArtNotice2.Code, SourceData, iStatut, CodeDMXLot, ArtMinims.Code, ArtEtui.CodeDMX, ArtNotice1.CodeDMX, ArtNotice2.CodeDMX, ArtEmballage.Code, ArtEmballage.Description, ArtEmballage.CodeDMX, Article.iParam1,ArtEtiquette.CodeDMX);
	try
	{
		theBase.dao_Base.Execute(strRequete);
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR AJOUT CRecette::Table Coupe\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}

	//Relecture pour récupération de l'ID
	ID = 0;
	if(rsTemp.IsOpen()) rsTemp.Close();

	if(ACBaseStrRsOpen(OfNum, _T("Numero"), _T("Coupes"),&rsTemp))
	{
		//Lot ajouté correctement => Récupération de l'ID
		ID = ACBaseGetLong(_T("ID"),&rsTemp);
		rsTemp.Close();
		return 1;
	}
	else
	{
		strErreur = _T("ERREUR AJOUT CRecette::Table Coupe\n");
		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}
}

int CRecette::Update(void)
{
	#pragma warning (disable: 4995)
	CString strRequete=_T("");
	CString strTemp=_T("");
	
	strRequete.Format( _T("UPDATE Coupes SET [Machine] = '%s'"),theConfig.str_Machine);
	strTemp.Format(_T(",[UtilisateurFin] = '%s'"),theApp.CurrentUser.strCode);
	strRequete += strTemp;
	strTemp.Format(_T(",[Article] = '%s'"),Article.Code);
	strRequete += strTemp;
	strTemp.Format(_T(",[DateFin] = GetDate()"));
	strRequete += strTemp;
	strTemp.Format(_T(",[Lot] = '%s'"),strLot);
	strRequete += strTemp;
	strTemp.Format(_T(",[Numero] = '%s'"),OfNum);
	strRequete += strTemp;
	strTemp.Format(_T(",[Quantite] = %d"),Quantite);
	strRequete += strTemp;
	strTemp.Format(_T(",[Peremption] = %d"),Peremption);
	strRequete += strTemp;
	strTemp.Format(_T(",[FormatEtiquette] = %d"),ArtEtiquette.iParam1);
	strRequete += strTemp;
	strTemp.Format(_T(",[CodeEtiquette] = '%s'"),ArtEtiquette.Code);
	strRequete += strTemp;
	strTemp.Format(_T(",[FormatNotice1] = %d"),ArtNotice1.iParam1);
	strRequete += strTemp;
	strTemp.Format(_T(",[FormatNotice2] = %d"),ArtNotice2.iParam1);
	strRequete += strTemp;
	strTemp.Format(_T(",[DescriptionEtui] = '%s'"),ArtEtui.Description);
	strRequete += strTemp;
	strTemp.Format(_T(",[DescriptionNotice1] = '%s'"),ArtNotice1.Description);
	strRequete += strTemp;
	strTemp.Format(_T(",[DescriptionNotice2] = '%s'"),ArtNotice2.Description);
	strRequete += strTemp;
	strTemp.Format(_T(",[CodeEtui] = '%s'"),ArtEtui.Code);
	strRequete += strTemp;
	strTemp.Format(_T(",[CodeNotice1] = '%s'"),ArtNotice1.Code);
	strRequete += strTemp;
	strTemp.Format(_T(",[CodeNotice2] = '%s'"),ArtNotice2.Code);
	strRequete += strTemp;
	strTemp.Format(_T(",[DMXVision] = '%s'"),CodeDMXLot);
	strRequete += strTemp;
	strTemp.Format(_T(",[Minims] = '%s'"),ArtMinims.Code);
	strRequete += strTemp;//0.99
	strTemp.Format(_T(",[DMXEtui] = '%s'"),ArtEtui.CodeDMX);
	strRequete += strTemp;
	strTemp.Format(_T(",[DMXNotice1] = '%s'"),ArtNotice1.CodeDMX);
	strRequete += strTemp;
	strTemp.Format(_T(",[DMXNotice2] = '%s'"),ArtNotice2.CodeDMX);
	strRequete += strTemp;
	//GCA 1412
	strTemp.Format(_T(",[NbEtuiOK] = %ld"),(NbEtuiOK[0] + NbEtuiOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbEtuiVideNOK] = %ld"),(NbEtuiNOK[0] + NbEtuiNOK[1]));
	strRequete += strTemp;
	//#3 DMX UNUSED strTemp.Format(_T(",[NbEtuiPleinNOK] = %ld"),(NbEtuiPleinNOK[0] + NbEtuiPleinNOK[1]));
	strTemp.Format(_T(",[NbEtuiPleinNOK] = 0"));
	strRequete += strTemp;

	strTemp.Format(_T(",[NbNotice1OK] = %ld"),(NbNotice1OK[0] + NbNotice1OK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbNotice1NOK] = %ld"),(NbNotice1NOK[0] + NbNotice1NOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbNotice2OK] = %ld"),(NbNotice2OK[0] + NbNotice2OK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbNotice2NOK] = %ld"),(NbNotice2NOK[0] + NbNotice2NOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbPieceOK] = %ld"),(NbPieceOK[0] + NbPieceOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbPieceNOK] = %ld"),(NbPieceNOK[0] + NbPieceNOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[SourceData] = %d"),SourceData);
	strRequete += strTemp;
	strTemp.Format(_T(",[Statut] = %d"),iStatut);
	strRequete += strTemp;
	strTemp.Format(_T(",[IDLot] = %d"),IDLot);
	strRequete += strTemp;
	strTemp.Format(_T(",[IDArticle] = %d"),IDArticle);
	strRequete += strTemp;
	strTemp.Format(_T(",[NbBoiteOK] = %ld"),(NbBoiteOK[0] + NbBoiteOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbBoiteNOK] = %ld"),(NbBoiteNOK[0] + NbBoiteNOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbBoitePrelev] = %ld"),(NbBoitePrelev[0] + NbBoitePrelev[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbVisionOK] = %ld"),(NbVisionOK[0] + NbVisionOK[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbVisionDMX] = %ld"),(NbVisionDMX[0] + NbVisionDMX[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbVisionTache] = %ld"),(NbVisionTache[0] + NbVisionTache[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbVisionSoudure] = %ld"),(NbVisionCB[0] + NbVisionCB[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbRebutVision] = %ld"),(NbRebutVision[0] + NbRebutVision[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbRebutPoids] = %ld"),(NbRebutPoids[0] + NbRebutPoids[1]));
	strRequete += strTemp;
	strTemp.Format(_T(",[NbVisionTacheUV] = %ld"),(NbVisionTacheUV[0] + NbVisionTacheUV[1]));
	strRequete += strTemp;
	//#2 Emballage
	strTemp.Format(_T(",[CodeEmballage] = '%s'"),ArtEmballage.Code);
	strRequete += strTemp;
	strTemp.Format(_T(",[DescriptionEmballage] = '%s'"),ArtEmballage.Description);
	strRequete += strTemp;
	strTemp.Format(_T(",[DMXEmballage] = '%s'"),ArtEmballage.CodeDMX);
	strRequete += strTemp;

	//V 44.6
	strTemp.Format(_T(",[DMXEtiquette] = '%s'"), ArtEtiquette.CodeDMX);
	strRequete += strTemp;

	strTemp.Format(_T(" WHERE ID = %ld"),ID);
	strRequete += strTemp;
	
	
	try
	{
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR UPDATE CRecette::Table Coupe\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}
	return 1;
}

bool CRecette::GererCycle(int iNum)
{
	CRecette * pRecette = &theApp.m_Recette;

	//Gestion Compteurs
	pRecette->NbEtuiNOK[1] = theApp.Compteurs.NbDMXNonLu[0];
	pRecette->NbNotice1NOK[1] = theApp.Compteurs.NbDMXNonLu[1];
	pRecette->NbNotice2NOK[1] = theApp.Compteurs.NbDMXNonLu[2];

	/*V 40 
	pRecette->NbEtuiOK[1] = theApp.Compteurs.NbDMXLu[0];
	pRecette->NbNotice1OK[1] = theApp.Compteurs.NbDMXLu[1];
	pRecette->NbNotice2OK[1] = theApp.Compteurs.NbDMXLu[2];*/

	pRecette->NbRebutVision[1] = theApp.Compteurs.NbRebusVision[0] + theApp.Compteurs.NbRebusVision[1] + theApp.Compteurs.NbRebusVision[2] ;
	pRecette->NbRebutPoids[1] = theApp.Compteurs.NbRebusPoids[0] + theApp.Compteurs.NbRebusPoids[1] + theApp.Compteurs.NbRebusPoids[2] ;

	pRecette->NbPieceOK[1] =theApp.Compteurs.NbElementsBoite[0] + theApp.Compteurs.NbElementsBoite[1] + theApp.Compteurs.NbElementsBoite[2] ;

	pRecette->NbPieceNOK[1] = pRecette->NbRebutVision[1] + pRecette->NbRebutPoids[1];
	
	//GESTION Pulse Raz Etalons
	if(*pRecette->pBitRazEtalons == TRUE)
	{
		//on attend N cycles
		if(pRecette->iCyclePulse >= pParamNbCyclePulse->value)
		{
			pRecette->iCyclePulse = 0;
			//on repasse à 0
			*pRecette->pBitRazEtalons = FALSE;
		}
		else
		{
			pRecette->iCyclePulse++ ;
		}
	}
	return false;
}

int CRecette::AjouterArticle(void)
{

	Article.Ajouter();
	IDArticle = Article.ID;

	ArtEtui.Ajouter();
	ArtNotice1.Ajouter();
	ArtNotice2.Ajouter();
	ArtMinims.Ajouter();
	ArtEtiquette.Ajouter();	//GCA V1.17	
	//V44.6 	ArtEtiqBoite.Ajouter();	//GCA V3.4
	ArtEmballage.Ajouter();	//#2 Emballage

	return 1;
}

bool CRecette::ReCharger(CString strOf)
{

	strErreur = _T("");
	CString strTemp;

#pragma warning( disable: 4995)
	CADORecordset rs (&theBase.dao_Base);
#pragma warning( default: 4995)
	

	if (ACBaseStrRsOpen(strOf, _T("[Numero]"), _T("Coupes"),&rs))
	{
		//MAJ des valeurs de la recette
		OfNum = strOf;
		strLot = ACBaseGetStr(_T("Lot"),&rs);
		ID = ACBaseGetLong(_T("ID"),&rs);
		IDLot = ACBaseGetLong(_T("IDLot"),&rs);
		IDArticle = ACBaseGetLong(_T("IDArticle"),&rs);
		Quantite = ACBaseGetLong(_T("Quantite"),&rs);
		Peremption = ACBaseGetInt(_T("Peremption"),&rs);
		PeremptionMois = (int) floor(Peremption/30.);

		strTemp = _T("");
		strTemp  = ACBaseGetStr(_T("DMXVision"),&rs);

		CStringA strATemp = CStringA(strTemp);
		strcpy_s(CodeDMXLot, strATemp.GetBuffer());
		strATemp.ReleaseBuffer();

		Article.Code = ACBaseGetStr(_T("Article"),&rs);
		Article.Type = ACBaseGetInt(_T("Type"),&rs);
		Article.Description = ACBaseGetStr(_T("DescriptionArticle"),&rs);
		Article.iParam1= ACBaseGetInt(_T("PoidsType"),&rs);	//V25 Gestion poids

		ArtMinims.Code  = ACBaseGetStr(_T("Minims"),&rs);
		ArtMinims.iParam1 = Article.Type;

		ArtEtui.Code = ACBaseGetStr(_T("CodeEtui"),&rs);
		ArtEtui.Description  = ACBaseGetStr(_T("DescriptionEtui"),&rs);
		ArtEtui.CodeDMX = ACBaseGetStr(_T("DMXEtui"),&rs);

		ArtNotice1.Code = ACBaseGetStr(_T("CodeNotice1"),&rs);
		ArtNotice1.Description  = ACBaseGetStr(_T("DescriptionNotice1"),&rs);
		ArtNotice1.iParam1 = ACBaseGetInt(_T("FormatNotice1"),&rs);
		ArtNotice1.CodeDMX = ACBaseGetStr(_T("DMXNotice1"),&rs);

		ArtNotice2.Code = ACBaseGetStr(_T("CodeNotice2"),&rs);
		ArtNotice2.Description  = ACBaseGetStr(_T("DescriptionNotice2"),&rs);
		ArtNotice2.iParam1 = ACBaseGetInt(_T("FormatNotice2"),&rs);
		ArtNotice2.CodeDMX = ACBaseGetStr(_T("DMXNotice2"),&rs);

		ArtEtiquette.Code = ACBaseGetStr(_T("CodeEtiquette"),&rs);//GCA V1.17
		ArtEtiquette.iParam1 = ACBaseGetInt(_T("FormatEtiquette"),&rs);//GCA V1.17

		//#2 Emballage
		ArtEmballage.Code = ACBaseGetStr(_T("CodeEmballage"),&rs);
		ArtEmballage.Description = ACBaseGetStr(_T("DescriptionEmballage"),&rs);
		ArtEmballage.CodeDMX = ACBaseGetStr(_T("DMXEmballage"),&rs);
		
		//Compteurs
		NbBoiteOK[0] = ACBaseGetLong(_T("NbBoiteOK"),&rs);
		NbBoiteNOK[0] = ACBaseGetLong(_T("NbBoiteNOK"),&rs);
		NbBoitePrelev[0] = ACBaseGetLong(_T("NbBoitePrelev"),&rs);

		NbEtuiOK[0] = ACBaseGetLong(_T("NbEtuiOK"),&rs);
		NbEtuiNOK[0] = ACBaseGetLong(_T("NbEtuiVideNOK"),&rs);
		//#3 DMX UNUSED NbEtuiPleinNOK[0] = ACBaseGetLong("NbEtuiPleinNOK",&rs);
		NbNotice1OK[0] = ACBaseGetLong(_T("NbNotice1OK"),&rs);
		NbNotice1NOK[0] = ACBaseGetLong(_T("NbNotice1NOK"),&rs);
		NbNotice2OK[0]  = ACBaseGetLong(_T("NbNotice2OK"),&rs);
		NbNotice2NOK[0] = ACBaseGetLong(_T("NbNotice2NOK"),&rs);
		NbPieceOK[0]  = ACBaseGetLong(_T("NbPieceOK"),&rs);
		NbPieceNOK[0]  = ACBaseGetLong(_T("NbPieceNOK"),&rs);

		NbVisionOK[0] = ACBaseGetLong(_T("NbVisionOK"),&rs);
		NbVisionDMX[0] = ACBaseGetLong(_T("NbVisionDMX"),&rs);
		NbVisionTache[0] = ACBaseGetLong(_T("NbVisionTache"),&rs);
		NbVisionCB[0] = ACBaseGetLong(_T("NbVisionSoudure"),&rs);
		NbVisionTacheUV[0] = ACBaseGetLong(_T("NbVisionTacheUV"),&rs);

		NbRebutVision[0] = ACBaseGetLong(_T("NbRebutVision"),&rs);
		NbRebutPoids[0] = ACBaseGetLong(_T("NbRebutPoids"),&rs);

		//V 44.6
		ArtEtiquette.CodeDMX = ACBaseGetStr(_T("DMXEtiquette"), &rs);

		rs.Close();

		//V25 Gestion poids
		//if(ArtMinims.iParam1 == RECETTE_TYPESACHET)//Article.Type
		//{
		//	PoidsNominal = (float) pParamSachetNom->value;
		//	PoidsMini = (float) pParamSachetMin->value;
		//	PoidsMaxi = (float) pParamSachetMax->value;
		//}
		//else
		//{
		//	PoidsNominal = (float) pParamBlisterNom->value;
		//	PoidsMini = (float) pParamBlisterMin->value;
		//	PoidsMaxi = (float) pParamBlisterMax->value;
		//}
		switch (Article.iParam1)
		{
			case RECETTE_TYPEBLISTER:
				PoidsNominal = (float) pParamBlisterNom->value;
				PoidsMini = (float) pParamBlisterMin->value;
				PoidsMaxi = (float) pParamBlisterMax->value;
				TolEtuiPlein_cg = (int)(pParamToleranceEtuiBlister->value * 100);
				break;
			case RECETTE_TYPESACHET:
				PoidsNominal = (float) pParamSachetNom->value;
				PoidsMini = (float) pParamSachetMin->value;
				PoidsMaxi = (float) pParamSachetMax->value;
				TolEtuiPlein_cg = (int)(pParamToleranceEtuiSachet->value * 100);
				break;
			case RECETTE_TYPESACHET_4ML:
				PoidsNominal = (float) pParamSachet4mlNom->value;
				PoidsMini = (float) pParamSachet4mlMin->value;
				PoidsMaxi = (float) pParamSachet4mlMax->value;
				TolEtuiPlein_cg = (int)(pParamToleranceEtuiSachet4ml->value * 100);
				break;
		}

		PoidsNominal_mg =(int)( PoidsNominal * 1000.);
		PoidsMini_mg =(int)( PoidsMini * 1000.);
		PoidsMaxi_mg =(int)( PoidsMaxi * 1000.);

		//V25 Gestion poids
		//if(Article.Type == RECETTE_TYPESACHET)
		//{
		//	TolEtuiPlein_cg = (int)(pParamToleranceEtuiSachet->value * 100);
		//}
		//else
		//{
		//	TolEtuiPlein_cg = (int)(pParamToleranceEtuiBlister->value * 100);
		//}

		IsLoaded = true;
	}
	else
	{
		strErreur = theApp.TableMessage.GetStr(MESSAGE_SELECOF_NOK) + _T(" Fonction Recharger");
		IsLoaded = false;
	}

	return IsLoaded;
}

int CRecette::AjouterLot(void)
{
	bool bLotExist = false;
	CString strRequete=_T("");

	//Verification Doublon
	#pragma warning (disable: 4995)
	CADORecordset rsLot (&theBase.dao_Base);

	if(ACBaseStrRsOpen(strLot, _T("Numero"), _T("Lot"),&rsLot))
	{
		rsLot.Close();

		//Lot Existant
		bLotExist = true;
	
	}

	if(bLotExist)
	{
		//Récupération Lot Existant IDLot + (DateDebut A VOIR)
		CADORecordset rsIdLot (&theBase.dao_Base);
		
		IDLot = 0;
		strRequete.Format(_T("SELECT * FROM Lot WHERE Numero = '%s'"),strLot);
		try
		{
			rsIdLot.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
			
			if (rsIdLot.IsOpen())
			{
				if (rsIdLot.GetRecordCount() > 0)
				{
					//V 40 IDLot = ACBaseGetInt(_T("ID"),&rsIdLot);
					IDLot = ACBaseGetLong(_T("ID"), &rsIdLot);
				}
			}

			rsIdLot.Close();
		}
		catch (CException* e)
		{
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause,255);
			e->Delete();
			strErreur = _T("ERREUR Lecture Lot Existant\r\nDescription :\n");
			strErreur = strErreur + szCause;

			//Affichage journal événements
			theApp.SetEventsJournalStr(strErreur);
			return 0;
		}

	}
	else
	{

		//Ecriture Table Lot 

		strRequete.Format( _T("INSERT INTO Lot ([DateDebut],[Numero],[Article],[CodeDMX]) VALUES(GetDate(),'%s','%s','%s')"),strLot,ArtMinims.Code,CodeDMXLot); 

		try
		{
			theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
			
		}
		catch (CException* e)
		{
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause,255);
			e->Delete();
			strErreur = _T("ERREUR Ecriture CRecette::Table Lot\r\nDescription :\n");
			strErreur = strErreur + szCause;

			//Affichage journal événements
			theApp.SetEventsJournalStr(strErreur);
			return 0;
		}

		//Récupération IDLot
		CADORecordset rsIdLot (&theBase.dao_Base);
		
		IDLot = 0;
		strRequete.Format(_T("SELECT  MAX(ID) AS idMax FROM Lot;"));
		try
		{
			rsIdLot.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
			
			if (rsIdLot.IsOpen())
			{
				//V 40 IDLot = ACBaseGetInt(_T("ID"),&rsIdLot);
				IDLot = ACBaseGetLong(_T("idMax"), &rsIdLot);
			}

			rsIdLot.Close();
		}
		catch (CException* e)
		{
			TCHAR   szCause[255];
			e->GetErrorMessage(szCause,255);
			e->Delete();
			strErreur = _T("ERREUR Lecture IDLot\r\nDescription :\n");
			strErreur = strErreur + szCause;

			//Affichage journal événements
			theApp.SetEventsJournalStr(strErreur);
			return 0;
		}
	}

#pragma warning (default:4995)
	return 1;
}

int CRecette::UpdateLot(void)
{
	
	//Update de la table Lot (DateFin)
	#pragma warning (disable: 4995)
	CString strRequete=_T("");
	CString strTemp=_T("");
	
	
	strRequete.Format( _T("UPDATE Lot SET [DateFin] = GetDate()"),theConfig.str_Machine);
	strTemp.Format(_T(" WHERE ID = %ld"),IDLot);
	strRequete += strTemp;
	
	try
	{
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR UPDATE CRecette::Table Lot\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return 0;
	}
	return 1;
}
//GCA V1.17 Table EnCoursMachine
bool CRecette::IsRecetteEnCours()
{
	//Verification si une recette est déja en cours dans la table
	CADORecordset rsEnCours (&theBase.dao_Base);
	bIsEnCours = false;
	CString strRequete;

	strRequete.Format(_T("SELECT * FROM EnCoursMachine WHERE Machine = '%s'"),theConfig.str_Machine);
	try
	{
		rsEnCours.Open ( /*dbOpenDynaset,*/strRequete/*,dbSeeChanges*/);
		
		if (rsEnCours.IsOpen())
		{
			if (rsEnCours.GetRecordCount() > 0)
			{
				ID = ACBaseGetLong(_T("IDCoupe"),&rsEnCours);
				OfNum = ACBaseGetStr(_T("NumOf"),&rsEnCours);
				strLot = ACBaseGetStr(_T("NumLot"),&rsEnCours);
				if(ID>0)
					bIsEnCours = true;
			}
		}

		rsEnCours.Close();
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Verification Recette En Cours\r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return false;
	}

	return bIsEnCours;
}
void CRecette::SetRecetteEnCours()
{
	//Enregistrement dans la table EnCours
	CString strTemp;

	CString strRequete;
	strRequete = _T("UPDATE EnCoursMachine SET [IDCoupe]=");

	strTemp.Format(_T("%ld"),ID);
	strRequete += strTemp;
	strRequete += _T(",[NumOf]='") + OfNum + _T("'");
	strRequete += _T(",[NumLot]='") + strLot + _T("'");
	strRequete += _T(" WHERE [Machine]='") + theConfig.str_Machine + _T("'");

	try
	{
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Ecriture Table EnCoursMachine \r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return ;
	}
}
void CRecette::EndRecetteEnCours()
{
	CString strRequete;
	strRequete = _T("UPDATE EnCoursMachine SET [IDCoupe]=0");
	strRequete +=_T(",[NumOf]=''");
	strRequete +=_T(",[NumLot]=''");
	strRequete +=_T(" WHERE [Machine]='")+ theConfig.str_Machine + _T("'");

	try
	{
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Ecriture Table EnCoursMachine \r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return ;
	}
}

void CRecette::SetUtilisateurFin()
{
	CString strRequete;

	//V2.50 MAJ Utilisateur Fin coupes
	strRequete.Format( _T("UPDATE Coupes SET [UtilisateurFin] = '%s' WHERE ID = %ld"),UserFin,ID);
	try
	{
		theBase.dao_Base.Execute(strRequete/*,dbSeeChanges*/);
		
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = _T("ERREUR Ecriture Table Coupes \r\nDescription :\n");
		strErreur = strErreur + szCause;

		//Affichage journal événements
		theApp.SetEventsJournalStr(strErreur);
		return ;
	}
}

#pragma warning( default: 4995)

