#include "stdafx.h"
#include "CodePiste.h"

CCodePiste::CCodePiste(void)
{
	IsCalculable = false;
	//Result = M_NULL;
	ColorTracage = RGB(255,255,255);
	Context = 0;
	CodeResult = 0;
}

CCodePiste::~CCodePiste(void)
{
	IsCalculable = false;
}
void CCodePiste::Decharger(void)
{
	if(CodeResult>0)	McodeFree(CodeResult);	
	CodeResult = 0;
	if(Context>0)		McodeFree(Context);	
	Context = 0;

	IsCalculable = false;
}
bool CCodePiste::Charger(void)
{
	if(CodeResult>0)	McodeFree(CodeResult);	
	CodeResult = 0;
	if(Context>0)		McodeFree(Context);	
	Context = 0;
	MIL_INT MilType;
	switch(Type)
	{
		case 1:
			MilType = M_DATAMATRIX;
			break;
		case 2:
			MilType = M_PHARMACODE;
			break;
		default:
			MilType = M_DATAMATRIX;
	}


//#ifdef MIL8		//MIL8 ou MIL9
//	McodeAlloc(*MF_System, MilType, M_DEFAULT, &Context);
//	McodeAllocResult(*MF_System,M_DEFAULT, &CodeResult);
//	McodeControl(Context, M_SEARCH_ANGLE, 0); 
//	McodeControl(Context, M_SEARCH_ANGLE_DELTA_NEG, 180); 
//	McodeControl(Context, M_SEARCH_ANGLE_DELTA_POS, 180); 
//	McodeControl(Context, M_TIMEOUT, 50); 
//	McodeControl(Context, M_USE_PRESEARCH, M_ENABLE); 
//#else
	McodeAlloc(*MF_System, M_DEFAULT, M_DEFAULT, &Context);
	McodeAllocResult(*MF_System,M_DEFAULT, &CodeResult);
	McodeModel(Context,M_ADD,M_DATAMATRIX,M_NULL,M_DEFAULT,M_NULL);
//#endif

	IsCalculable = true;
	return true;
}

bool CCodePiste::ChargerFromFile(void)
{
	if(CodeResult>0)	McodeFree(CodeResult);	
	CodeResult = 0;
	if(Context>0)	McodeFree(Context);	
	Context = 0;

	McodeAllocResult(*MF_System,M_DEFAULT, &CodeResult);
	Context = McodeRestore(MF_File.GetBuffer(),*MF_System,M_DEFAULT,M_NULL);
	MF_File.ReleaseBuffer();

	IsCalculable = true;
	return true;
}

bool CCodePiste::SetControl(long ControlType,double ControlValue)
{
	McodeControl(Context,ControlType,ControlValue);
	return true;
}

bool CCodePiste::SetRoi(double X,double Y,double dX,double dY)
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

bool CCodePiste::CenterRoi(double X,double Y,bool Redim)
{
	CenterRoi(X,Y);
	if(!Redim)	return IsCalculable;

	IsCalculable = RedimRoi();
	return IsCalculable;
}

bool CCodePiste::CenterRoi(double X,double Y)
{
	long longX;
	long longY;
	
	longX = (long)X;
	longY = (long)Y;

	RoiX =(long) (longX - Code_PosX + Code_dX/2 - RoiXSize/2 );
	RoiY =(long)( longY - Code_PosY + Code_dY/2 - RoiYSize/2 );

	IsCalculable = VerifierRoi();
	return IsCalculable;
}

bool CCodePiste::VerifierRoi()
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

bool CCodePiste::RedimRoi()
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

bool CCodePiste::Trouver(void)
{
	MF_NbRes  = 0L;
	bResult = false;
	Code_PosX = 0;
	Code_PosY = 0;

//#ifdef MIL8			//MIL8 ou MIL9
//	McodeRead(Context, mImg->milBuf, M_DEFAULT);
//	McodeGetResult(Context, M_STATUS+M_TYPE_LONG, &BarcodeStatus);
//	McodeGetResult(Context, M_SCORE+M_TYPE_DOUBLE, &Score);
//	NbErreurs = 0;
//	if (BarcodeStatus == M_STATUS_OK)
//	{
//		McodeGetResult(Context, M_STRING, BarcodeString);
//		bResult = true;
//	}
//	return bResult;
//#else
	if (mImg->milBuf <= 0)
	{
		return false;
	}

	McodeRead(Context, mImg->milBuf, CodeResult);
	McodeGetResult(CodeResult, M_STATUS + M_TYPE_MIL_INT, &BarcodeStatus);
	if (BarcodeStatus == M_STATUS_READ_OK)
	{
		McodeGetResult(CodeResult, M_SCORE+M_TYPE_DOUBLE, &Score);
		McodeGetResult(CodeResult, M_NUMBER_OF_ERRORS+M_TYPE_LONG, &NbErreurs);
		McodeGetResult(CodeResult, M_STRING, BarcodeString);

		McodeGetResult(CodeResult, M_POSITION_X+M_TYPE_DOUBLE, &Code_PosX);
		McodeGetResult(CodeResult, M_POSITION_Y+M_TYPE_DOUBLE, &Code_PosY);
		bResult = true;
	}
//#endif
	return bResult;
}



bool CCodePiste::DrawTitre()
{
	MgraColor(M_DEFAULT, 0xFF);
	MgraText(M_DEFAULT,mImg->milBuf,0,0,mTitre);
	return true;
}
bool CCodePiste::DrawBox(void)
{
	MgraColor(M_DEFAULT, 0x00);
	McodeDraw(M_DEFAULT,CodeResult, mImg->milBuf,M_DRAW_BOX, M_ALL, M_DEFAULT);
	return true;
}
bool CCodePiste::DrawEdges(void)
{
	//MgraColor(M_DEFAULT, M_RGB888(GetRValue(ColorTracage),GetGValue(ColorTracage),GetBValue(ColorTracage)));
	MgraColor(M_DEFAULT, 0xFF);
	McodeDraw(M_DEFAULT,CodeResult, mImg->milBuf,M_DRAW_EDGES, M_ALL, M_DEFAULT);
	return true;
}

bool CCodePiste::DrawPosition(void)
{
	MgraColor(M_DEFAULT, 0xFF);
	McodeDraw(M_DEFAULT,CodeResult, mImg->milBuf,M_DRAW_POSITION, M_ALL, M_DEFAULT);
	return true;
}

bool CCodePiste::DrawBox(MIL_ID Image)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(ColorTracage),GetGValue(ColorTracage),GetBValue(ColorTracage)));
	McodeDraw(M_DEFAULT,CodeResult, Image,M_DRAW_BOX, M_ALL, M_DEFAULT);
	return true;
}
bool CCodePiste::DrawEdges(MIL_ID Image)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(ColorTracage),GetGValue(ColorTracage),GetBValue(ColorTracage)));
	McodeDraw(M_DEFAULT,CodeResult, Image,M_DRAW_EDGES, M_ALL, M_DEFAULT);
	return true;
}

bool CCodePiste::DrawPosition(MIL_ID Image)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(ColorTracage),GetGValue(ColorTracage),GetBValue(ColorTracage)));
	McodeDraw(M_DEFAULT,CodeResult, Image,M_DRAW_POSITION, M_ALL, M_DEFAULT);
	return true;
}

bool CCodePiste::DrawROI(void)
{
	MgraColor(M_DEFAULT, M_RGB888(GetRValue(ColorTracage),GetGValue(ColorTracage),GetBValue(ColorTracage)));
	MgraRect(M_DEFAULT,mImg->milBuf,0,0,mImg->Roi.dX-1,mImg->Roi.dY-1);
	return true;
}

bool CCodePiste::ModifyParamsCode()
{
	McodeControl(Context, M_INTERACTIVE, M_DEFAULT); 
	return true;
}

bool CCodePiste::SaveToFile(void)
{
	
	McodeSave( MF_File.GetBuffer(),Context,M_DEFAULT);
	MF_File.ReleaseBuffer();
	return true;
}