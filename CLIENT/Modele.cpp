#include "stdafx.h"
#include "Modele.h"

CModele::CModele(void)
{
	IsCalculable = false;
	Result = M_NULL;
	SearchContext = M_NULL;

	ColorTracage = RGB(255,255,255);

	Model_Angle = 0;
	Model_CentreX = 0;
	Model_CentreY = 0;
	Model_dX = 0;
	Model_dY = 0;

	strFile = _T("");

}

CModele::~CModele(void)
{
	IsCalculable = false;
}
bool CModele::Charger(void)
{
	double Modele_OrigineX = 0.0;
	double Modele_OrigineY = 0.0;
	long Modele_DirectionY = 0;

	if(SearchContext>0)		MmodFree(SearchContext);	
	if(Result>0)			MmodFree(Result);	

	if(strFile == _T(""))
	{
		return false;	
	}

	SearchContext = MmodRestore(strFile.GetBuffer(),*MF_System,M_DEFAULT,M_NULL);
	strFile.ReleaseBuffer();
	MmodAllocResult(*MF_System, M_DEFAULT, &Result);
	MmodPreprocess(SearchContext, M_DEFAULT);
	
	double Modele_Scale = 0.0;
	MmodInquire(SearchContext,M_DEFAULT,M_PIXEL_SCALE,&Modele_Scale);
	MmodInquire(SearchContext,M_DEFAULT,M_REFERENCE_ANGLE,&Model_Angle);

	if(Modele_Scale == 0.0)	Modele_Scale = 1.0;
	if(Modele_Scale > 100.0)	Modele_Scale = 1.0;
	if(Modele_Scale !=1.0)
	{
		MmodInquire(SearchContext,M_DEFAULT,M_REFERENCE_X,&this->Model_CentreX);
		MmodInquire(SearchContext,M_DEFAULT,M_REFERENCE_Y,&this->Model_CentreY);

		//Modele DXF
		this->Model_dX = MmodInquire(SearchContext,M_DEFAULT,M_BOX_SIZE_X,M_NULL) * Modele_Scale;
		this->Model_dY = MmodInquire(SearchContext,M_DEFAULT,M_BOX_SIZE_Y,M_NULL) * Modele_Scale;

		MmodInquire(SearchContext,M_DEFAULT,M_BOX_OFFSET_X,&Modele_OrigineX);
		MmodInquire(SearchContext,M_DEFAULT,M_BOX_OFFSET_Y,&Modele_OrigineY);
		MmodInquire(SearchContext,M_DEFAULT,M_CAD_Y_AXIS+M_TYPE_LONG,&Modele_DirectionY);
		if(Modele_DirectionY == M_NO_FLIP)
		{
			Model_CentreX = (Model_CentreX - Modele_OrigineX)* Modele_Scale;
			Model_CentreY = (Model_CentreY - Modele_OrigineY)* Modele_Scale;
		}
		else
		{
			Model_CentreX = (Model_CentreX - Modele_OrigineX)* Modele_Scale;
			Model_CentreY = (Modele_OrigineY*-1.0 - Model_CentreY)* Modele_Scale;
		}
	}
	else
	{
		this->Model_dX = MmodInquire(SearchContext,M_DEFAULT,M_BOX_SIZE_X,M_NULL) * Modele_Scale;
		this->Model_dY = MmodInquire(SearchContext,M_DEFAULT,M_BOX_SIZE_Y,M_NULL) * Modele_Scale;
		MmodInquire(SearchContext,M_DEFAULT,M_REFERENCE_X,&this->Model_CentreX);
		MmodInquire(SearchContext,M_DEFAULT,M_REFERENCE_Y,&this->Model_CentreY);
	}

	//Verification point reference
	if( (long) Model_CentreX == M_DEFAULT)
	{
		Model_CentreX = Model_dX/2.0;
	}

	if( (long) Model_CentreY == M_DEFAULT)
	{
		Model_CentreY = Model_dY/2.0;
	}

	IsCalculable = true;

	return true;
}

bool CModele::SetControl(MIL_INT ControlType,double ControlValue)
{
	MmodControl(SearchContext,M_ALL,ControlType,ControlValue);
	return true;
}
bool CModele::SetControl(MIL_INT index,MIL_INT ControlType,double ControlValue)
{
	MmodControl(SearchContext,index,ControlType ,ControlValue);
	return true;
}
bool CModele::GetControl(MIL_INT Index,MIL_INT ControlType,long * Value)
{
	MmodInquire(SearchContext,Index,ControlType + M_TYPE_LONG, Value);
	return true;
}

bool CModele::SetRoi(double X,double Y,double dX,double dY)
{
	long longX,longY,longdX,longdY;

	longX = (long)X;
	longY = (long)Y;
	longdX = (long)dX;
	longdY = (long)dY;

	if(longX >= 0)	RoiX = longX;
	if(longY >= 0)	RoiY = longY;
	if(longdX >= 0)	RoiXSize = longdX;
	if(longdY >= 0)	RoiYSize = longdY;

	IsCalculable = VerifierRoi();
	return IsCalculable;
}

bool CModele::CenterRoi(double X,double Y,bool Redim)
{
	CenterRoi(X,Y);
	if(!Redim)	return IsCalculable;

	IsCalculable = RedimRoi();
	return IsCalculable;
}

bool CModele::CenterRoi(double X,double Y)
{
	long longX;
	long longY;
	
	longX = (long)X;
	longY = (long)Y;

	RoiX =(long) (longX - Model_CentreX + Model_dX/2 - RoiXSize/2 );
	RoiY =(long)( longY - Model_CentreY + Model_dY/2 - RoiYSize/2 );

	IsCalculable = VerifierRoi();
	return IsCalculable;
}

bool CModele::VerifierRoi()
{
	//vérification de la ROI
	if(RoiX < 0)						return false;
	if(RoiY < 0)						return false;
	if((RoiX+RoiXSize) > mImg->Roi.dX)	return false;
	if((RoiY+RoiYSize) > mImg->Roi.dY)	return false;

	RoiRedimX = RoiX;
	RoiRedimY = RoiY;
	RoiRedimXSize = RoiXSize;
	RoiRedimYSize = RoiYSize;

	return true;
}

bool CModele::RedimRoi()
{
	RoiRedimX = RoiX;
	RoiRedimY = RoiY;
	RoiRedimXSize = RoiXSize;
	RoiRedimYSize = RoiYSize;
	if(RoiRedimX < 0)	{RoiRedimX = 0; RoiRedimXSize = RoiXSize + RoiX;}
	if(RoiRedimY < 0)	{RoiRedimY = 0; RoiRedimYSize = RoiYSize + RoiY;}
	if((RoiRedimX+RoiRedimXSize) > mImg->Roi.dX)	RoiRedimXSize = mImg->Roi.dX - RoiRedimX;
	if((RoiRedimY+RoiRedimYSize) > mImg->Roi.dY)	RoiRedimYSize = mImg->Roi.dY - RoiRedimY;

	if(RoiRedimXSize < 20)		return false;
	if(RoiRedimYSize < 20)		return false;

	return true;
}

bool CModele::Trouver(void)
{
	if(SearchContext == 0) return false;
	if(Result == 0) return false;

	int i;
//	PARAM_ROI Roi;
	MF_NbRes  = 0L;

	////Vérifier la validité de la ROI
	//if(!IsCalculable)	return false;

	//Roi.X1 = RoiRedimX;
	//Roi.Y1 = RoiRedimY;
	//Roi.dX = RoiRedimXSize;
	//Roi.dY = RoiRedimYSize;
	//Roi.X2 = Roi.X1 + Roi.dX;
	//Roi.Y2 = Roi.Y1 + Roi.dY;
	// TO REMOVE MappControl(M_ERROR,M_PRINT_DISABLE);

	//mImg->ChargerChild(FALSE,iChild,RoiRedimX,RoiRedimY,RoiRedimXSize,RoiRedimYSize);
	//mImg->SetROI(&Roi);

	//Recherche du modèle
	MmodFind(SearchContext, mImg->milBuf, Result );

	if(MappGetError(M_CURRENT, M_NULL))
	{
		MappControl(M_ERROR,M_PRINT_ENABLE);
		return false;
	}

	//Analyse du résultat
	MmodGetResult(Result, M_DEFAULT, M_NUMBER+M_TYPE_LONG, &MF_NbRes);

	//Analyse du résultat
	lIsTimeout = M_FALSE;
	MmodGetResult(Result, M_GENERAL, M_TIMEOUT_END  + M_TYPE_LONG, &lIsTimeout);
	
	if(MappGetError(M_CURRENT, M_NULL))
	{
		MappControl(M_ERROR,M_PRINT_ENABLE);
		return false;
	}

	if ( (MF_NbRes > 0) && (MF_NbRes <= MODELE_MAXNB) )
	{
		//Récupération des résultats
		

		MmodGetResult(Result, M_DEFAULT, M_POSITION_X, ResRoiX);
		MmodGetResult(Result, M_DEFAULT, M_POSITION_Y, ResRoiY);
		MmodGetResult(Result, M_DEFAULT, M_ANGLE, ResAngle);
		MmodGetResult(Result, M_DEFAULT, M_SCALE, ResScale);//VOIR M_SCORE
		MmodGetResult(Result, M_DEFAULT, M_SCORE, ResScore);

		for(i=0;i<MF_NbRes;i++)
		{
			//ResImgX[i] = RoiRedimX + ResRoiX[i];
			//ResImgY[i] = RoiRedimY + ResRoiY[i];
			ResImgX[i] = ResRoiX[i];
			ResImgY[i] = ResRoiY[i];
		}
		return true;
	}
	else
	{
		//RAZ des valeurs si modèle pas trouvé
		ZeroMemory(&ResImgX,MODELE_MAXNB * sizeof(double)); 
		ZeroMemory(&ResImgY,MODELE_MAXNB * sizeof(double));
		ZeroMemory(&ResRoiX,MODELE_MAXNB * sizeof(double));
		ZeroMemory(&ResRoiY,MODELE_MAXNB * sizeof(double));
		ZeroMemory(&ResAngle,MODELE_MAXNB * sizeof(double));
		ZeroMemory(&ResScale,MODELE_MAXNB * sizeof(double));
		ZeroMemory(&ResScore,MODELE_MAXNB * sizeof(double));

		MF_NbRes  = 0L;
		return false;
	}
}

bool CModele::DrawBox(MIL_ID Image,MIL_INT Index /*= M_ALL */, COLORREF Color /*= COLORTRACAGE*/)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MmodDraw(M_DEFAULT,Result, Image,M_DRAW_BOX, Index, M_DEFAULT);
	return true;
}

bool CModele::DrawTitre( MIL_ID Image, COLORREF Color /*= COLORTRACAGE*/)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MgraText(M_DEFAULT,Image,0,0,mTitre);
	return true;
}

bool CModele::DrawEdges(MIL_ID Image,MIL_INT Index /*= M_ALL */, COLORREF Color /*= COLORTRACAGE*/)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MmodDraw(M_DEFAULT,Result, Image,M_DRAW_EDGES, Index, M_DEFAULT);
	return true;
}

bool CModele::DrawPosition(MIL_ID Image,MIL_INT Index /*= M_ALL */, COLORREF Color /*= COLORTRACAGE*/)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MmodDraw(M_DEFAULT,Result, Image,M_DRAW_POSITION, Index, M_DEFAULT);
	return true;
}
bool CModele::Decharger(void)
{
	if(SearchContext>0)	
	{
		MmodFree(SearchContext);	
		SearchContext = 0;
	}
	if(Result>0)
	{
		MmodFree(Result);	
		Result = 0;
	}


	return true;
}

#ifdef _WIN64
void CModele::Calibrer(void)
{
	//TODO X64
}
#else
void CModele::Calibrer(void)
{
	McalUniform(SearchContext,0,0,1,1,0,M_DEFAULT) ;
	McalUniform(Result,0,0,1,1,0,M_DEFAULT) ;
}
#endif

void CModele::GetInteractiveResults()
{
	MmodGetResult(Result, M_DEFAULT, M_INTERACTIVE, M_NULL);
}
bool CModele::DrawROI(MIL_ID Image, COLORREF Color/*= ColorTracage*/)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(Color),GetGValue(Color),GetBValue(Color)));
	MgraRect(M_DEFAULT,Image,0,0,mImg->Roi.dX-1,mImg->Roi.dY-1);
	return true;
}

bool CModele::CreateFromImage(MIL_ID Image,double OffsetX ,double OffsetY,double ModelCY ,double ModelCX)
{
	if(SearchContext>0)		MmodFree(SearchContext);	
	if(Result>0)			MmodFree(Result);	

	SearchContext = MmodAlloc(*MF_System, M_GEOMETRIC,M_DEFAULT,M_NULL);
	MmodAllocResult(*MF_System, M_DEFAULT, &Result);

	MmodDefine(SearchContext,M_IMAGE,Image,OffsetX ,OffsetY ,ModelCY ,ModelCX );

	MmodPreprocess(SearchContext, M_DEFAULT);

	return true;
}
bool CModele::AddFromImage(MIL_ID Image,double OffsetX ,double OffsetY,double ModelCY ,double ModelCX)
{
	if(SearchContext <= 0) return false;	
	if(Result <= 0) return false;

	MmodDefine(SearchContext,M_IMAGE,Image,OffsetX ,OffsetY ,ModelCY ,ModelCX );

	MmodPreprocess(SearchContext, M_DEFAULT);

	return true;
}

void CModele::ModifyParamsContext()
{
	SetControl(M_CONTEXT,M_INTERACTIVE,M_DEFAULT);
}
void CModele::SaveToFile()
{
	MmodSave(strFile.GetBuffer(),SearchContext,M_DEFAULT);
	strFile.ReleaseBuffer();
}

bool CModele::CreateFromDXF(CString strFichierDXF,double dPxSize /*= 0*/,MIL_ID Calibration/*= 0*/)
{
	//recherche du fichier
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(strFichierDXF, &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
	}
	else
	{
		return false;
	}

	//verification extension
	if(strFichierDXF.Right(4) != _T(".DXF"))
	{
		return false;
	}

	if(SearchContext>0)		MmodFree(SearchContext);	
	if(Result>0)			MmodFree(Result);	

	SearchContext = MmodAlloc(*MF_System, M_GEOMETRIC,M_DEFAULT,M_NULL);
	MmodAllocResult(*MF_System, M_DEFAULT, &Result);

	MappControl(M_ERROR,M_PRINT_DISABLE);

	MmodDefineFromFile(SearchContext,M_DXF_FILE,strFichierDXF.GetBuffer(),M_DEFAULT);
	strFichierDXF.ReleaseBuffer();

	if(MappGetError(M_CURRENT, M_NULL))
	{
		if(SearchContext>0)	
		{
			MmodFree(SearchContext);	
			SearchContext = 0;
		}
		if(Result>0)
		{
			MmodFree(Result);	
			Result = 0;
		}

		MappControl(M_ERROR,M_PRINT_ENABLE);
		return false;
	}

	if(dPxSize > 0)
	{
		if(Calibration > 0)
		{
			SetControl(M_ASSOCIATED_CALIBRATION,(double)Calibration);
		}

		SetControl(M_CONTEXT,M_SCALE,dPxSize);
		SetControl(M_CONTEXT,M_SEARCH_SCALE_RANGE,M_ENABLE);
		//SetControl(M_CAD_Y_AXIS,(double)M_FLIP);
		SetControl(M_CAD_Y_AXIS,(double)M_NO_FLIP);

		double Model_Angle = 0;
		double Model_Angle_Tolerance = 30;
		SetControl(M_ANGLE,Model_Angle);
		SetControl(M_ANGLE_DELTA_NEG,Model_Angle_Tolerance);
		SetControl(M_ANGLE_DELTA_POS,Model_Angle_Tolerance);

		double dScale = 0;
		double dPxSizeX = 0;
		double dPxSizeY = 0;
		double dPxOffsetX = 0;
		double dPxOffsetY = 0;
		double dPxScale = dPxSize;
		SetControl(0,M_PIXEL_SCALE,dPxScale);

		MmodInquire(SearchContext,0,M_SCALE + M_TYPE_DOUBLE, &dScale);
		
		MmodInquire(SearchContext,0,M_BOX_SIZE_X + M_TYPE_DOUBLE, &Model_dX);
		MmodInquire(SearchContext,0,M_BOX_SIZE_Y + M_TYPE_DOUBLE, &Model_dY);

		if(dScale == M_DEFAULT)
		{
			dScale = 1.0;
		}
		//Verification de la taille maximale pour model finder
		double dIsValid = 0;
		MmodInquire(SearchContext,0,M_VALID + M_TYPE_DOUBLE, &dIsValid);
		if(dIsValid == M_FALSE)
		{
			MappControl(M_ERROR,M_PRINT_ENABLE);
			return false;
		}
	}
	
	MmodPreprocess(SearchContext, M_DEFAULT);
	if(MappGetError(M_CURRENT, M_NULL))
	{
		MappControl(M_ERROR,M_PRINT_ENABLE);
		return false;
	}
	MappControl(M_ERROR,M_PRINT_ENABLE);

	double iNbModels = 0;

	MmodInquire(SearchContext,M_CONTEXT ,M_NUMBER_MODELS,&iNbModels);
	
	if(iNbModels == 1)
	{
		//replacement origine au centre de l'image modéle
		MmodInquire(SearchContext,0,M_REFERENCE_X,&this->Model_CentreX);
		MmodInquire(SearchContext,0,M_REFERENCE_Y,&this->Model_CentreY);
		
		if( (long) Model_CentreX == M_DEFAULT)
		{
			Model_CentreX = 0;
		}

		if( (long) Model_CentreY == M_DEFAULT)
		{
			Model_CentreY = 0;
		}

		SetControl(M_REFERENCE_X,Model_CentreX);
		SetControl(M_REFERENCE_Y,Model_CentreY);
	}

	return true;
}