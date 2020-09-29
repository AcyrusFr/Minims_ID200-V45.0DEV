#pragma once
#include "ClientOPC.h"
#include "peson.h"
#include "GestAlarmes.h"


#define ROBOT_MODEON	0
#define ROBOT_MODEOFF	-1
#define ROBOT_MODEPARKING	1
#define ROBOT_MODEVIDAGE	2

#define ROBOT_POSITION_RIEN	0
#define ROBOT_POSITION_OK	1
#define ROBOT_POSITION_REBUT	2
#define ROBOT_POSITION_MELANGE	3
#define ROBOT_POSITION_SACHETVIDE	4

#define ROBOT_ETATHORSCYCLE	0
#define ROBOT_ETATENCYCLE	1
#define ROBOT_ETATDEFAUT	2

#define ROBOT_INHIBER_NORMAL	0
#define ROBOT_INHIBER_FORCEON	1
#define ROBOT_INHIBER_FORCEOFF	2

typedef struct _STRUCT_POS
{
	int X;
	int Y;
	int Z;
	int PosStatut;
	int iNumImg;
}
STRUCT_POS;


class CRobot
{
public:
	CRobot(void);
	~CRobot(void);

	//Variables des robots
	CClientOPC* m_OPCClient_TR;//Track Read
	CClientOPC* m_OPCClient_TW;//Track write

	//Pointeurs Pesons associés
	CPeson * peson[2];

	int iNumRobot;

	//position de la piéce a gérer
	int X;//1/10mm
	int Y;
	int Z;
	int PosStatut;// 0-> Rien; 1-> OK ; 2-> Rebut; 3-> Melange
	int PosStatutSimul;// 0-> Rien; 1-> OK ; 2-> Rebut; 3-> Melange
	bool bPosSimul;

	int NumPrise;//numéro de prise pour vérification transaction
	int NumTop;

	//Historique Positions
	STRUCT_POS HistoPos[50];
	int iIndexHisto;

	//index Variables OPC
	//int IndexAckPos;

	//Bit Parking
	BOOL * pBitParking;
	BOOL * pBitInhibition;

	//BOOL
	BOOL WBit1[8];
	BOOL WBit2[8];
	BOOL WBit3[8];

	BOOL RBit1[8];
	BOOL RBit2[8];

	//Index des pesons associés au robot
	//UNUSED int IndexPeson1;
	//UNUSED int IndexPeson2;
	//Variables temporaire pour acquittement pesons
	//UNUSED int AcquPeson;

	//inhibition
	short RobotMode;

	//Etat
	int iEtat;
	//mot de défaut
	int DefWord;

	//Comptage UNUSED GCA1412
	int NbElementsBoiteUnused;
	int NbRebusPoidsUnused;
	int NbRebusVisionUnused;

	//void GererAcquittement(void);
	int SetPosition(int TypePosition, int fX, int fY, int fZ);
	int inhiber(void* BoutonParent,int iMode);
	int GererAlarme(void);
	static bool GererCycle(int);
	bool InitOF(void);
};
