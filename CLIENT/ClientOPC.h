#pragma once

//Include DLL
//#define BCCWIN

//#include "nodavesimple.h"
#include "openSocket.h"
#include "SiemensS7_x64.h"

//definition variable pour Profibus
#define MAX_ITEMS_BY_GROUP	105//GCA 3.7 Old: 50 //GCA1412
#define GROUPE_READ	0
#define GROUPE_WRITE	1
#define VT_ACYBOOL	100

typedef struct _ACY_ITEM
{
	CString strItems;

	VARIANT Value; //valeurs OPC
	void * pValue; //pionteurs application
	int IndexBlock;//Index de la variable dans le bloc/registre automate

}ACY_ITEM;


class CClientOPC
{
public:
	CClientOPC(void);
	~CClientOPC(void);

	//client OPC

	bool IsInitialised;
	CString strNomServeur;

	int* Alarme;
	int AlarmeInit;
	int AlarmeVal;	//V38

	CString strErreur;

	//Variable Siemens
	daveInterface * di;
    daveConnection * dc;
	 _daveOSserialType fds;

	 //Variable Item_EMPTY
	 int iEmpty;
	//variables pour le bloc
	int Area;
	int NumDataBlock,IndexDataBlock,LgBuf;
	unsigned char* Buffer;
	PDU pdu;
	int iNbItemForGroup;//nombre d'items pour le groupe
	int iTypeGroupe;//flag R/W
	ACY_ITEM my_Items[MAX_ITEMS_BY_GROUP];
	

	//Nb cycle Pulse
	int iCyclePulse;
	long TickOPC;
	
	bool InitialiseGroupe(void);
	bool InitialiseServeur();
	bool ReadItemsGroup();
	bool WriteItemsGroup();

	int InitialiseItems();
	int EndGroupe(void);
	int Lier( void * pVariable,int iTypeVar,CString strDesc);
	int OPCToData(void);
	int DataToOPC(void);
	bool (*GererCycle)(int);
	/*void GererCycleRobot(void * Robot);
	void GererCycleProcess(void * Process);
	void GererCyclePeson(void * pPeson);
	void GererCyleDMX(void* pDMX);*/
	void Initialise(void);
	void Reset(void);
};
