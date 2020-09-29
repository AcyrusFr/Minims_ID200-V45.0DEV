#include "StdAfx.h"
#include "ClientOPC.h"


//inlude APP
//#include "BaseFenetre.h"
#include "GestAlarmes.h"
#include "General.h"

CClientOPC::CClientOPC(void)
{
	IsInitialised = false;
	di = NULL;
    dc = NULL;
	//variables pour le bloc
	
	Alarme = NULL;
	AlarmeInit = ALARME_OFF;
	AlarmeVal = 0;
	NumDataBlock = 0;
	IndexDataBlock = 0;
	LgBuf = 0;
	Buffer = NULL;
	iTypeGroupe= 0;
	iNbItemForGroup = 0;
	iCyclePulse = 0;
	TickOPC = 0;

	iEmpty = 0;
	Area = daveDB;

	for(int y = 0; y < MAX_ITEMS_BY_GROUP;y++)
	{
		VariantInit(&my_Items[y].Value);
		my_Items[y].pValue = NULL;
		my_Items[y].strItems = "";

	}
}

CClientOPC::~CClientOPC(void)
{

}
bool CClientOPC::InitialiseServeur()
{

	//TEST TO REMOVEreturn false;
	if(theConfig.str_Machine.Right(5) == "SIMUL")
	{
		if(Alarme != NULL) *Alarme = 1;
		else AlarmeInit = 1;

		strErreur = _T("Couldn't open TCP port. Please make sure a CP is connected and the IP address is ok. ");	
		return false;
	}

	//Connexion
	 daveSetDebug(daveDebugPrintErrors); //A VOIR 
    
	 CStringA strA = CStringA(strNomServeur);
	 fds.rfd=openSocket(102, strA.GetBuffer());
	 strA.ReleaseBuffer();

    fds.wfd=fds.rfd;
    
    if (fds.rfd>0) 
	{ 
	di =daveNewInterface(fds,"IF1",0, daveProtoISOTCP, daveSpeed187k);
	daveSetTimeout(di,5000000);
	dc =daveNewConnection(di,2,0, 2);  // insert your rack and slot here
	
		if (0==daveConnectPLC(dc)) 
		{
			if(Alarme != NULL) *Alarme = 0;
			else AlarmeInit = 0;
			return true;
		}
		else 
		{
			if(Alarme != NULL) *Alarme = 2;
			else AlarmeInit = 2;
			strErreur = _T("Couldn't connect to PLC.");	
			return false;
		}
    }
	else
	{
		if(Alarme != NULL) *Alarme = 1;
		else AlarmeInit = 1;

		strErreur = _T("Couldn't open TCP port. Please make sure a CP is connected and the IP address is ok. ");	
    	return false;
    }    
	
}
bool CClientOPC::InitialiseGroupe(void)
{
	
	//pas de groupe dans ce mode
	return true;
	
}


bool CClientOPC::ReadItemsGroup()
{

	if(dc == NULL) return false;
	if(iTypeGroupe == GROUPE_WRITE) return false;

	int nRet = daveReadBytes(dc,Area,NumDataBlock,IndexDataBlock,LgBuf,Buffer);

	//V38
	//if(Alarme != NULL) *Alarme = nRet;
	//else AlarmeInit = nRet;

	if(Alarme != NULL) 
	{
		if(nRet == 0)
			*Alarme = *Alarme&(!AlarmeVal);
		else
			*Alarme = *Alarme|AlarmeVal;
	}
	else AlarmeInit = nRet;

	return true;
}

bool CClientOPC::WriteItemsGroup()
{
	if(dc == NULL) return false;
	if(iTypeGroupe == GROUPE_READ) return false;

	int nRet = daveWriteBytes(dc,Area,NumDataBlock,IndexDataBlock,LgBuf,Buffer);

	//V38
	//if(Alarme != NULL) *Alarme = nRet;
	//else AlarmeInit = nRet;

	if(Alarme != NULL) 
	{
		if(nRet == 0)
			*Alarme = *Alarme&(!AlarmeVal);
		else
			*Alarme = *Alarme|AlarmeVal;
	}
	else AlarmeInit = nRet;

	return true;
}

int CClientOPC::InitialiseItems( )
{
	if(dc == NULL) return false;
	// Allocation et Construction de la chaine 

	//Allocation
	Buffer = (unsigned char*) calloc(LgBuf,sizeof(unsigned char));
	
	//création de la chaine par défaut
	int index = 0;
	for(int i = 0; i <iNbItemForGroup; i++)
	{
		switch(my_Items[i].Value.vt)
			{
			case VT_I2://INT
				davePut16(&Buffer[index],0);
				index += 2;
				break;
			case VT_I4://DWORD //GCA1412
				davePut32(&Buffer[index],0);
				index += 4;
				break;

			case VT_ACYBOOL:
			case VT_I1://BYTE
				davePut8(&Buffer[index],0);
				index += 1;
				break;

			case VT_R4://REAL
				davePutFloat(&Buffer[index],0.);
				index += 4;
				break;

			default:
				davePut8(&Buffer[index],0);
				index += 1;
				break;
			}
	}
		
	

	IsInitialised = true;
	return 0;
}

int CClientOPC::EndGroupe(void)
{
	//deconnexion Socket

	closeSocket(fds.rfd);
	free(Buffer);
	return 0;
}

int CClientOPC::Lier(void * pVariable,int iTypeVar,CString strDesc)
{

	if((pVariable == NULL)&&(iTypeVar == VT_EMPTY))
	{
		my_Items[iNbItemForGroup].pValue = &iEmpty;
		my_Items[iNbItemForGroup].strItems = _T("Empty");
		my_Items[iNbItemForGroup].Value.vt = iTypeVar;
		my_Items[iNbItemForGroup].IndexBlock = LgBuf;
	}
	else
	{
		my_Items[iNbItemForGroup].pValue = pVariable;
		my_Items[iNbItemForGroup].Value.vt = iTypeVar;
		my_Items[iNbItemForGroup].strItems = strDesc;
		my_Items[iNbItemForGroup].IndexBlock = LgBuf;
	}

	switch(my_Items[iNbItemForGroup].Value.vt)
		{
			case VT_I2://INT
				LgBuf += 2;//16bits
				break;

			case VT_I4://DWORD//GCA1412
				LgBuf += 4;
				break;

			case VT_ACYBOOL:
			case VT_EMPTY:
			case VT_I1://BYTE
				LgBuf += 1;
				break;

			case VT_R4://REAL
				LgBuf += 4;
				break;
			default:
				LgBuf += 1;
				break;
		}

	iNbItemForGroup++;

	return  (iNbItemForGroup - 1) ;//retour index de la variable
}

int CClientOPC::OPCToData(void)
{

	if(dc == NULL) return 1;

	//on prend les valeurs des VARIANT pour les mettres dans les variables

	for(int i = 0; i < iNbItemForGroup ; i++)
	{
		if((my_Items[i].pValue != NULL)&&(iTypeGroupe == GROUPE_READ))
		{

			int iBool = 0;

			switch(my_Items[i].Value.vt)
			{
			case VT_I2://INT
				if(Area != daveDB)//pour inputs et Outputs
				{	
					my_Items[i].Value.intVal = daveSwapIed_16(daveGetU16from(&Buffer[my_Items[i].IndexBlock]));
				}
				else
				{
					unsigned char ch2 = daveGetU8from(&Buffer[my_Items[i].IndexBlock]);
					unsigned char ch1 = daveGetU8from(&Buffer[my_Items[i].IndexBlock+1]);

					unsigned short iVal1 = 0;
					iVal1 = (ch2 << 8) | ch1;
					memcpy_s(&my_Items[i].Value.intVal,sizeof(unsigned short),&iVal1,sizeof(unsigned short));
						
					//my_Items[i].Value.intVal = daveGetU16from(&Buffer[my_Items[i].IndexBlock]);
				}

				*((int*)(my_Items[i].pValue)) = my_Items[i].Value.intVal;
				break;

			case VT_I4://DWORD//GCA1412
				if(Area != daveDB)//pour inputs et Outputs
				{	
					my_Items[i].Value.intVal = daveSwapIed_32(daveGetU16from(&Buffer[my_Items[i].IndexBlock]));
				}
				else
				{
					char ch1 = daveGetU8from(&Buffer[my_Items[i].IndexBlock]);
					char ch2 = daveGetU8from(&Buffer[my_Items[i].IndexBlock+1]);
					char ch3 = daveGetU8from(&Buffer[my_Items[i].IndexBlock+2]);
					char ch4 = daveGetU8from(&Buffer[my_Items[i].IndexBlock+3]);

					int iVal1 = 0;
					iVal1 = (ch1 << 24) |(ch2 << 16) |(ch3 << 8) | ch4;
					memcpy_s(&my_Items[i].Value.uintVal,sizeof(int),&iVal1,sizeof(int));
					//my_Items[i].Value.uintVal = daveGetU32from(&Buffer[my_Items[i].IndexBlock]);
				}
				*((int*)(my_Items[i].pValue)) = my_Items[i].Value.uintVal;
				break;

			case VT_R4://REAL
				if(Area != daveDB)//pour inputs et Outputs
				{	
#pragma warning (disable :4244)
					my_Items[i].Value.fltVal = daveSwapIed_32(daveGetFloatfrom(&Buffer[my_Items[i].IndexBlock]));
#pragma warning (default :4244)
				}
				else
				{
					char ch1 = daveGetU8from(&Buffer[my_Items[i].IndexBlock]);
					char ch2 = daveGetU8from(&Buffer[my_Items[i].IndexBlock+1]);
					char ch3 = daveGetU8from(&Buffer[my_Items[i].IndexBlock+2]);
					char ch4 = daveGetU8from(&Buffer[my_Items[i].IndexBlock+3]);

					int iVal1 = 0;
					iVal1 = (ch1 << 24) |(ch2 << 16) |(ch3 << 8) | ch4;
					memcpy_s(&my_Items[i].Value.fltVal,sizeof(int),&iVal1,sizeof(int));
					//my_Items[i].Value.fltVal = daveGetFloatfrom(&Buffer[my_Items[i].IndexBlock]);
				}
				*((float*)(my_Items[i].pValue)) = my_Items[i].Value.fltVal;
				break;

			case VT_ACYBOOL:
					//bool BOOL[8];
					//Combinaison en fonction de la valeur des 8 booléen Liés
					my_Items[i].Value.intVal = daveGetU8from(&Buffer[my_Items[i].IndexBlock]);
					iBool= my_Items[i].Value.intVal;

					((BOOL*)(my_Items[i].pValue))[0] = iBool >> 0 & 0x0001;
					((BOOL*)(my_Items[i].pValue))[1] = iBool >> 1 & 0x0001;
					((BOOL*)(my_Items[i].pValue))[2] = iBool >> 2 & 0x0001;
					((BOOL*)(my_Items[i].pValue))[3] = iBool >> 3 & 0x0001;
					((BOOL*)(my_Items[i].pValue))[4] = iBool >> 4 & 0x0001;
					((BOOL*)(my_Items[i].pValue))[5] = iBool >> 5 & 0x0001;
					((BOOL*)(my_Items[i].pValue))[6] = iBool >> 6 & 0x0001;
					((BOOL*)(my_Items[i].pValue))[7] = iBool >> 7 & 0x0001;
				
					break;
			case VT_EMPTY:
				break;
			case VT_I1://BYTE
				my_Items[i].Value.bVal = daveGetU8from(&Buffer[my_Items[i].IndexBlock]);
				*((BYTE*)(my_Items[i].pValue)) = my_Items[i].Value.bVal;
				break;

			default:
				my_Items[i].Value.bVal = daveGetU8from(&Buffer[my_Items[i].IndexBlock]);
				*((BYTE*)(my_Items[i].pValue)) = my_Items[i].Value.bVal;
				break;
			}
		}
			
	}
	return 0;
}

int CClientOPC::DataToOPC(void)
{

	if(dc == NULL) return 1;

	//prendre valeur Variables pour mettre dans VARIANT

	for(int i = 0; i < iNbItemForGroup ; i++)
	{
		if((my_Items[i].pValue != NULL)&&(iTypeGroupe == GROUPE_WRITE))
		{
			int iBool = 0;

			switch(my_Items[i].Value.vt)
				{
				case VT_I2:
					my_Items[i].Value.intVal = *((int*)(my_Items[i].pValue));
					if(Area != daveDB)//pour inputs et Outputs
					{	
						davePut16(&Buffer[my_Items[i].IndexBlock],daveSwapIed_16(my_Items[i].Value.intVal));
					}
					else
					{
						int iVal1 = 0;
						memcpy_s(&iVal1,sizeof(int),&my_Items[i].Value.intVal,sizeof(int));
						char ch1 = iVal1 & 0x000000FF;
						char ch2 = (iVal1 & 0x0000FF00) >>8;

						davePut8(&Buffer[my_Items[i].IndexBlock],ch2);
						davePut8(&Buffer[my_Items[i].IndexBlock+1],ch1);

						//davePut16(&Buffer[my_Items[i].IndexBlock],my_Items[i].Value.intVal);
					}
					break;

				case VT_I4://DWORD//GCA1412
					my_Items[i].Value.uintVal = *((int*)(my_Items[i].pValue));
					if(Area != daveDB)//pour inputs et Outputs
					{	
						davePut32(&Buffer[my_Items[i].IndexBlock],daveSwapIed_32(my_Items[i].Value.uintVal));
					}
					else
					{
						int iVal1 = 0;
						memcpy_s(&iVal1,sizeof(int),&my_Items[i].Value.uintVal,sizeof(int));
						char ch1 = iVal1 & 0x000000FF;
						char ch2 = (iVal1 & 0x0000FF00) >>8;
						char ch3 = (iVal1 & 0x00FF0000) >>16;
						char ch4 = (iVal1 & 0xFF000000) >>24;

						davePut8(&Buffer[my_Items[i].IndexBlock],ch4);
						davePut8(&Buffer[my_Items[i].IndexBlock+1],ch3);
						davePut8(&Buffer[my_Items[i].IndexBlock+2],ch2);
						davePut8(&Buffer[my_Items[i].IndexBlock+3],ch1);
						//davePut32(&Buffer[my_Items[i].IndexBlock],my_Items[i].Value.uintVal);
					}
					break;
				
				case VT_R4:
					my_Items[i].Value.fltVal = *((float*)(my_Items[i].pValue));

					if(Area != daveDB)//pour inputs et Outputs
					{	
#pragma warning (disable :4244)
						davePutFloat(&Buffer[my_Items[i].IndexBlock], daveSwapIed_32(my_Items[i].Value.fltVal));
#pragma warning (default :4244)
					}
					else
					{
						int iVal1 = 0;
						memcpy_s(&iVal1,sizeof(int),&my_Items[i].Value.fltVal,sizeof(int));
						char ch1 = iVal1 & 0x000000FF;
						char ch2 = (iVal1 & 0x0000FF00) >>8;
						char ch3 = (iVal1 & 0x00FF0000) >>16;
						char ch4 = (iVal1 & 0xFF000000) >>24;

						davePut8(&Buffer[my_Items[i].IndexBlock],ch4);
						davePut8(&Buffer[my_Items[i].IndexBlock+1],ch3);
						davePut8(&Buffer[my_Items[i].IndexBlock+2],ch2);
						davePut8(&Buffer[my_Items[i].IndexBlock+3],ch1);
						//davePutFloat(&Buffer[my_Items[i].IndexBlock],my_Items[i].Value.fltVal);
					}
					break;

				case VT_ACYBOOL:
					//bool BOOL[8];
					//Combinaison en fonction de la valeur des 8 booléen Liés
					iBool = 0;
					
					iBool = (iBool | ((BOOL*)(my_Items[i].pValue))[0]);
					iBool = (iBool | (((BOOL*)(my_Items[i].pValue))[1])<<1);
					iBool = (iBool | (((BOOL*)(my_Items[i].pValue))[2])<<2);
					iBool = (iBool | (((BOOL*)(my_Items[i].pValue))[3])<<3);
					iBool = (iBool | (((BOOL*)(my_Items[i].pValue))[4])<<4);
					iBool = (iBool | (((BOOL*)(my_Items[i].pValue))[5])<<5);
					iBool = (iBool | (((BOOL*)(my_Items[i].pValue))[6])<<6);
					iBool = (iBool | (((BOOL*)(my_Items[i].pValue))[7])<<7);

					my_Items[i].Value.intVal = iBool; 
						
					davePut8(&Buffer[my_Items[i].IndexBlock],my_Items[i].Value.intVal);
					break;

				case VT_EMPTY:
					break;
				case VT_I1://BYTE
					my_Items[i].Value.intVal = *((BYTE*)(my_Items[i].pValue));
					davePut8(&Buffer[my_Items[i].IndexBlock],my_Items[i].Value.intVal);
					break;

				default:
					my_Items[i].Value.bVal = *((BYTE*)(my_Items[i].pValue));
					davePut8(&Buffer[my_Items[i].IndexBlock],my_Items[i].Value.bVal);
					break;
				}
		}
	}
	return 0;
}

void CClientOPC::Initialise(void)
{
	//Init OPC Pesons
	//m_GroupePesons.InitialiseGroupe(); //A FAIRE Si même Groupe OPC mais deux groupes différents de l'appli
	//m_GroupePesons.hServerGroup = m_GroupeRobots.hServerGroup;
	//m_GroupePesons.pIOPCItemMgt = m_GroupeRobots.pIOPCItemMgt;

	
	
}

void CClientOPC::Reset(void)
{
	//Reset de la communication OPC en cas de coupure
	//V 42 Initialise();
	//UNCOMMENT theApp.StartThreadOPC();

	//V 42
	EndGroupe();

	Sleep(50);

	InitialiseServeur();
	InitialiseGroupe();
	InitialiseItems();


}
