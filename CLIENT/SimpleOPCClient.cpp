////////////////////////////////////////////////////////////////////////
// project: SimpleOPCClient
//
// subject: this is a simple OPC Client that read values 
// from an OPC server.
//
// file: SimpleOPCClient.cpp
//

///*
//ERREUR 0: Pas d'erreur
//ERREUR 1: Nom Serveur OPC Incorrect
//ERREUR 2: Echec Creation IOPCServer 
//ERREUR 3: Impossible d'ajouter le groupe OPC
//ERREUR 4: Impossible d'ajouter l'item OPC
//ERREUR 5: Impossible d'ajouter les items OPC
//ERREUR 6: Lecture Echouée (Resultat fonctions)
//ERREUR 7: Echec Lecture (pValue == NULL)
//ERREUR 8: Lecture multiple Echouée
//ERREUR 9: Echec Lecture
//ERREUR 10: Ecriture Echouée
//ERREUR 11: Ecriture MultiItems Echouée
//ERREUR 12: Remove Item 
//ERREUR 13: Remove MultiItems
//ERREUR 14: Remove Groupe*/
//
//

#include "stdafx.h"

#include <atlbase.h>
#include <objbase.h>
#include <iostream>
#include "opc.h"
#include "SimpleOPCClient.h"
#include "opc_i.c"


using namespace std;


//#define REMOTE_SERVER_NAME L"your_path"

//////////////////////////////////////////////////////////////////////
// Read the value of an item on an OPC server. 
//
//void main(void)
//{
//	IOPCServer* pIOPCServer = NULL;   //pointer to IOPServer interface
//	IOPCItemMgt* pIOPCItemMgt = NULL; //pointer to IOPCItemMgt interface
//
//	OPCHANDLE hServerGroup; // server handle to the group
//	OPCHANDLE hServerItem;  // server handle to the item
//
//
//	// have to be done before using microsoft COM library:
//	CoInitialize(NULL);
//
//	// Let's instantiante the IOPCServer interface and get a pointer of it:
//	pIOPCServer = InstantiateServer(OPC_SERVER_NAME);
//
//	// Add the OPC group the OPC server and get an handle to the IOPCItemMgt
//	//interface:
//	AddTheGroup(pIOPCServer, pIOPCItemMgt, hServerGroup);
//
//	// Add the OPC item
//    AddTheItem(pIOPCItemMgt, hServerItem);
//
//	//Read the value of the item from device:
//	VARIANT varValue; //to stor the read value
//	VariantInit(&varValue);
//	ReadItem(pIOPCItemMgt, hServerItem, varValue);
//
//	// print the read value:
//	cout << "Read value: " << varValue.XVAL << endl;
//
//	// Remove the OPC item:
//	RemoveItem(pIOPCItemMgt, hServerItem);
//
//	// Remove the OPC group: 
//    RemoveGroup(pIOPCServer, hServerGroup);
//
//	// release the interface references:
//	pIOPCItemMgt->Release();
//	pIOPCServer->Release();
//
//	//close the COM library:
//	CoUninitialize();
//}

////////////////////////////////////////////////////////////////////
// Instantiate the IOPCServer interface of the OPCServer
// having the name ServerName. Return a pointer to this interface
//
IOPCServer* InstantiateServer(wchar_t ServerName[],int * iError)
{
	CLSID CLSID_OPCServer;
	HRESULT hr;

	// get the CLSID from the OPC Server Name:
	hr = CLSIDFromString(ServerName, &CLSID_OPCServer);
	if(FAILED(hr))
	{
		*iError = 1;//ERREUR 1: Nom Serveur OPC Incorrect
	}

	//queue of the class instances to create
	LONG cmq = 1; // nbr of class instance to create.
	MULTI_QI queue[1] =
		{{&IID_IOPCServer,
		NULL,
		0}};

	//Server info:
	//COSERVERINFO CoServerInfo =
    //{
	//	/*dwReserved1*/ 0,
	//	/*pwszName*/ REMOTE_SERVER_NAME,
	//	/*COAUTHINFO*/  NULL,
	//	/*dwReserved2*/ 0
    //}; 

	// create an instance of the IOPCServer
	hr = CoCreateInstanceEx(CLSID_OPCServer, NULL, CLSCTX_SERVER,
		/*&CoServerInfo*/NULL, cmq, queue);
	if(hr != 0)
	{
		*iError = 2;//ERREUR 2:Echec Creation IOPCServer 
	}

	// return a pointer to the IOPCServer interface:
	return(IOPCServer*) queue[0].pItf;

}


/////////////////////////////////////////////////////////////////////
// Add group "Group1" to the Server whose IOPCServer interface
// is pointed by pIOPCServer. 
// Returns a pointer to the IOPCItemMgt interface of the added group
// and a server opc handle to the added group.
//
int AddTheGroup(IOPCServer* pIOPCServer, IOPCItemMgt* &pIOPCItemMgt, 
				 OPCHANDLE& hServerGroup,CString strGroupName)
{
	DWORD dwUpdateRate = 0;
	OPCHANDLE hClientGroup = 0;

	// Add an OPC group and get a pointer to the IUnknown I/F:
	HRESULT hr = pIOPCServer->AddGroup(/*szName*/(LPCWSTR) strGroupName.GetBuffer(),
		/*bActive*/ FALSE,
		/*dwRequestedUpdateRate*/ dwUpdateRate,
		/*hClientGroup*/ hClientGroup,
		/*pTimeBias*/ 0,
		/*pPercentDeadband*/ 0,
		/*dwLCID*/0,
		/*phServerGroup*/&hServerGroup,
		&dwUpdateRate,
		/*riid*/ IID_IOPCItemMgt,
		/*ppUnk*/ (IUnknown**) &pIOPCItemMgt);


	if(FAILED(hr))
	{
		strGroupName.ReleaseBuffer();
		return 3;//ERREUR 3: Impossible d'ajouter le groupe OPC
	}

	strGroupName.ReleaseBuffer();
	return 0;
}



//////////////////////////////////////////////////////////////////
// Add the Item ITEM_ID to the group whose IOPCItemMgt interface
// is pointed by pIOPCItemMgt pointer. Return a server opc handle
// to the item.
 
int AddTheItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE& hServerItem,CString strItemName)
{
	HRESULT hr;
	int iErreur = 0;

	// Array of items to add:
	//OPCITEMDEF ItemArray[1] =
	//{{
	///*szAccessPath*/ L"",
	///*szItemID*/ ITEM_ID,
	///*bActive*/ FALSE,
	///*hClient*/ 1,
	///*dwBlobSize*/ 0,
	///*pBlob*/ NULL,
	///*vtRequestedDataType*/ VT,
	///*wReserved*/0
	//}};

	OPCITEMDEF ItemArray[1] =
	{{
	/*szAccessPath*/ L"",
		/*szItemID*/(LPWSTR) strItemName.GetBuffer(),
	/*bActive*/ FALSE,
	/*hClient*/ 1,
	/*dwBlobSize*/ 0,
	/*pBlob*/ NULL,
	/*vtRequestedDataType*/ VT_BSTR,//VT_UINT,
	/*wReserved*/0
	}};
	strItemName.ReleaseBuffer();

	//Add Result:
	OPCITEMRESULT* pAddResult=NULL;
	HRESULT* pErrors = NULL;

	// Add an Item to the previous Group:
	hr = pIOPCItemMgt->AddItems(1, ItemArray, &pAddResult, &pErrors);
	if(hr != 0)
	{
		iErreur = 4;//ERREUR 4: Impossible d'ajouter l'item OPC
	}

	// Server handle for the added item:
	hServerItem = pAddResult[0].hServer;

	// release memory allocated by the server:
	CoTaskMemFree(pAddResult->pBlob);

	CoTaskMemFree(pAddResult);
	pAddResult = NULL;

	CoTaskMemFree(pErrors);
	pErrors = NULL;
	
	return iErreur;
}
int  AddItems(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE* hServerItem,OPCITEMDEF* ItemsDef,int NbItems)
{

	HRESULT hr;
	int iErreur = 0;

	// Array of items to add:
	//OPCITEMDEF ItemArray[1] =
	//{{
	///*szAccessPath*/ L"",
	///*szItemID*/ ITEM_ID,
	///*bActive*/ FALSE,
	///*hClient*/ 1,
	///*dwBlobSize*/ 0,
	///*pBlob*/ NULL,
	///*vtRequestedDataType*/ VT,
	///*wReserved*/0
	//}};
	OPCITEMDEF* ItemsArray = ( OPCITEMDEF* )calloc(NbItems,sizeof(OPCITEMDEF));

		for(int i = 0; i< NbItems;i++)
		{
			ItemsArray[i] = ItemsDef[i];
			
		}
	

	//Add Result:
	OPCITEMRESULT* pAddResult=NULL;
	HRESULT* pErrors = NULL;

	// Add an Item to the previous Group:
	hr = pIOPCItemMgt->AddItems((DWORD)NbItems, ItemsArray, &pAddResult, &pErrors);
	if(hr != 0)
	{
		iErreur = 5;//ERREUR 5: Impossible d'ajouter les items OPC
		return iErreur;
	}

	if(iErreur == 0)
	{
		for(int i = 0; i< NbItems;i++)
		{
			// Server handle for the added item:
			hServerItem[i] = pAddResult[i].hServer;
		}
	}
	
	free((void*)ItemsArray);
	// release memory allocated by the server:
	CoTaskMemFree(pAddResult->pBlob);

	CoTaskMemFree(pAddResult);
	pAddResult = NULL;

	CoTaskMemFree(pErrors);
	pErrors = NULL;

	return iErreur;
}

///////////////////////////////////////////////////////////////////////////////
// Read from device the value of the item having the "hServerItem" server 
// handle and belonging to the group whose one interface is pointed by
// pGroupIUnknown. The value is put in varValue. 
//
int  ReadItem(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue)
{
	// value of the item:
	OPCITEMSTATE* pValue = NULL;
	int iErreur = 0;

	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**) &pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Read(OPC_DS_DEVICE, 1, &hServerItem, &pValue, &pErrors);
	if(hr != 0)
	{
		iErreur = 6;//ERREUR 6: Lecture Echouée
	}

	if(pValue==NULL)
	{
		iErreur = 7;//ERREUR 7: Echec Lecture
	}


	varValue = pValue[0].vDataValue;

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	CoTaskMemFree(pValue);
	pValue = NULL;

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();
	
	return iErreur;
}
IOPCSyncIO* GetInterface(IUnknown* pGroupIUnknown)
{
	OPCITEMSTATE* pValue = NULL;
	int iErreur = 0;

	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**) &pIOPCSyncIO);
	
	return pIOPCSyncIO;
}

int  ReadItemsThread(IOPCSyncIO* pIOPCSyncIO, OPCHANDLE hServerItem[], VARIANT varValue[],int NbItems)
{
	int iErreur = 0;
	OPCITEMSTATE* pValue = NULL;
	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Read(OPC_DS_DEVICE, NbItems, hServerItem, &pValue, &pErrors);
	if(hr != 0)
	{
		iErreur = 8;//ERREUR 8: Lecture multiple Echouée
	}

	if(pValue==NULL)
	{
		iErreur = 9;//ERREUR 9: Echec Lecture
	}
	else
	{
		for(int i = 0; i < NbItems;i++)
		{
			varValue[i] = pValue[i].vDataValue;
		}
	}

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	CoTaskMemFree(pValue);
	pValue = NULL;

	return iErreur;
}
///////////////////////////////////////////////////////////////////////////////
// Read from device the value of the item having the "hServerItem" server 
// handle and belonging to the group whose one interface is pointed by
// pGroupIUnknown. The value is put in varValue. 
//
int  ReadItems(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem[], VARIANT varValue[],int NbItems)
{
	// value of the item:
	OPCITEMSTATE* pValue = NULL;
	int iErreur = 0;

	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**) &pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Read(OPC_DS_DEVICE, NbItems, hServerItem, &pValue, &pErrors);
	if(hr != 0)
	{
		iErreur = 8;//ERREUR 8: Lecture multiple Echouée
	}

	if(pValue==NULL)
	{
		iErreur = 9;//ERREUR 9: Echec Lecture
	}
	else
	{
		for(int i = 0; i < NbItems;i++)
		{
			varValue[i] = pValue[i].vDataValue;
		}
	}

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	CoTaskMemFree(pValue);
	pValue = NULL;

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();

	return iErreur;
}
///////////////////////////////////////////////////////////////////////////////
// Write from device the value of the item having the "hServerItem" server 
// handle and belonging to the group whose one interface is pointed by
// pGroupIUnknown. The value is put in varValue. 
//
int WriteItem(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue)
{
	// value of the item:
	//OPCITEMSTATE* pValue = NULL;

	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	int iErreur = 0;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**) &pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Write(1, &hServerItem, &varValue, &pErrors);
	if(hr != 0)
	{
		iErreur = 10;//ERREUR 10: Ecriture Echouée
	}

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	/*CoTaskMemFree(pValue);
	pValue = NULL;*/

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();
	
	return iErreur;
}

int WriteItems(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem[] , VARIANT varValue[] ,int NbItems)
{
	// value of the item:
	//OPCITEMSTATE* pValue = NULL;

	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	int iErreur = 0;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**) &pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Write(NbItems, hServerItem, varValue, &pErrors);
	if(hr != 0)
	{
		iErreur = 11;//ERREUR 11: Ecriture MultiItems Echouée
	}

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	/*CoTaskMemFree(pValue);
	pValue = NULL;*/

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();

	return iErreur;
}

///////////////////////////////////////////////////////////////////////////
// Remove the item whose server handle is hServerItem from the group
// whose IOPCItemMgt interface is pointed by pIOPCItemMgt
//
int RemoveItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem)
{
	// server handle of items to remove:
	OPCHANDLE hServerArray[1];
	int iErreur =0;
	hServerArray[0] = hServerItem;
	
	//Remove the item:
	HRESULT* pErrors; // to store error code(s)
	HRESULT hr = pIOPCItemMgt->RemoveItems(1, hServerArray, &pErrors);
	if(hr != 0)
	{
		iErreur = 12;//ERREUR 12: Remove Item 
	}


	//release memory allocated by the server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	return iErreur;
}
int RemoveItems(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem[],int NbItems)
{
	// server handle of items to remove:
	int iErreur = 0;
	//Remove the item:
	HRESULT* pErrors; // to store error code(s)
	HRESULT hr = pIOPCItemMgt->RemoveItems(NbItems, hServerItem, &pErrors);
	if(hr != 0)
	{
		iErreur = 13;//ERREUR 13: Remove MultiItems
	}

	//release memory allocated by the server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	return iErreur;
}


IOPCServer * InitOPC(int *iError,CString strNomServeur)
{
	CoInitialize(NULL);

	//Initialisation du client OPC
	*iError = 0;
	IOPCServer * m_Ser = NULL;

	wchar_t strNameServer[50];
	CStringW str = CStringW(strNomServeur);

	wcscpy_s(strNameServer ,50, str.GetBuffer());
	str.ReleaseBuffer();

	m_Ser = InstantiateServer(strNameServer,iError);

	return m_Ser;
}

CString GetStringFromErrorCode(int CodeError)
{
	CString strErreur = "";
	switch(CodeError)
	{
	case 0:
		strErreur = "Pas d'erreur";
		break;
	case 1:
		strErreur = "Nom Serveur OPC Incorrect";
		break;
	case 2:
		strErreur = "Echec Creation IOPCServer";
		break;
	case 3:
		strErreur = "Impossible d'ajouter le groupe OPC";
		break;
	case 4:
		strErreur = "Impossible d'ajouter l'item OPC";
		break;
	case 5:
		strErreur = "Impossible d'ajouter les items OPC";
		break;
	case 6:
		strErreur = "Lecture Echouée (Resultat fonctions)";
		break;
	case 7:
		strErreur = "Echec Lecture (pValue == NULL)";
		break;
	case 8:
		strErreur = "Lecture multiple Echouée";
		break;
	case 9:
		strErreur = "Echec Lecture";
		break;
	case 10:
		strErreur = "Ecriture Echouée";
		break;
	case 11:
		strErreur = "Ecriture MultiItems Echouée";
		break;
	case 12:
		strErreur = "Erreur Remove Item";
		break;
	case 13:
		strErreur = "Erreur Remove MultiItems";
		break;
	case 14:
		strErreur = "Erreur Remove Groupe";
		break;



	default:
		strErreur = "Code Erreur Inconnu ou Invalide";
		break;

	}

	return strErreur;

}

void EndOPC(IOPCServer * m_Server)
{
	if(m_Server != NULL)
		m_Server->Release();
	//close the COM library:
	CoUninitialize();
}

////////////////////////////////////////////////////////////////////////
// Remove the Group whose server handle is hServerGroup from the server
// whose IOPCServer interface is pointed by pIOPCServer
//
int RemoveGroup (IOPCServer* pIOPCServer, OPCHANDLE hServerGroup)
{
	// Remove the group:
	int iErreur = 0;
	HRESULT hr = pIOPCServer->RemoveGroup(hServerGroup, TRUE);//Bool Forcage de la fermeture
	if(hr != 0)
	{
		iErreur = 14;//ERREUR 14: Remove Groupe
	}

	return iErreur;
}