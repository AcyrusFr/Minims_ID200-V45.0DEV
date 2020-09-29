////////////////////////////////////////////////////////////////////////
// project: SimpleOPCClient 
//
// subject: this is a simple OPC Client that read values
// from an OPC server.
//
// file: SimpleOPCClient.h
//
//

#include "OPC.h"

#ifndef SIMPLE_OPC_CLIENT_H
#define SIMPLE_OPC_CLIENT_H

#define MAX_ITEMS_BY_GROUP	50//taille du tableau d'items

IOPCServer* InstantiateServer(wchar_t ServerName[],int * pNumError);
int AddTheGroup(IOPCServer* pIOPCServer, IOPCItemMgt* &pIOPCItemMgt, 
				 OPCHANDLE& hServerGroup,CString strGroupName);
int AddTheItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE& hServerItem,CString strItemName);
int ReadItem(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue);
int ReadItems(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem[], VARIANT varValue[],int NbItems);
int RemoveItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem);
int RemoveItems(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem[],int NbItems);
int RemoveGroup (IOPCServer* pIOPCServer, OPCHANDLE hServerGroup);


int AddItems(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE* hServerItem,OPCITEMDEF* ItemsDef,int NbItems);
int WriteItems(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem[], VARIANT varValue[],int NbItems);
int WriteItem(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue);
IOPCSyncIO* GetInterface(IUnknown* pGroupIUnknown);
int  ReadItemsThread(IOPCSyncIO* pIOPCSyncIO, OPCHANDLE hServerItem[], VARIANT varValue[],int NbItems);

IOPCServer * InitOPC( int * Error,CString strNomServeur);
void EndOPC(IOPCServer * m_Server);
CString GetStringFromErrorCode(int CodeError);
#endif // SIMPLE_OPC_CLIENT_H not defined