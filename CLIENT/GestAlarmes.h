#pragma once

//definitions alarmes
#define NB_ALARMES	300 //V21

#define ALARME_ON	1
#define ALARME_OFF	0

#define GROUPE_ROBOT1	0
#define GROUPE_ROBOT2	1
#define GROUPE_ROBOT3	2
#define GROUPE_OPC	3
#define GROUPE_MACHINE	4
#define GROUPE_DMXETUI	5
#define GROUPE_DMXNOTICE1	6
#define GROUPE_DMXNOTICE2	7
#define GROUPE_ALERTES	8
#define GROUPE_CAM1	9
#define GROUPE_CAMUV	10
#define GROUPE_CAM_UEYE1	11
#define GROUPE_CAM_UEYE2	12
#define GROUPE_PESAGE	13

#define NBGROUPES	14

//Definitions alarmes gérées par application
//V 44.0
#define CODE_ALARME_OPC					0
#define CODE_ALARME_DMXETUI				1
#define CODE_ALARME_DMXNOTICE1			2
#define CODE_ALARME_DMXNOTICE2			3
#define CODE_ALARME_CAM1				4
#define CODE_ALARME_CAM2				5

#define CODE_ALARME_CAM1_SERIAL			168
#define CODE_ALARME_CAM1_LINK			169
#define CODE_ALARME_CAM1_SYNCHRO		170

#define CODE_ALARME_CAM2_SERIAL			171
#define CODE_ALARME_CAM2_LINK			172
#define CODE_ALARME_CAM2_SYNCHRO		173

#define CODE_ALARME_COM_DMXETUI			174
#define CODE_ALARME_COM_DMXNOTICE1		175
#define CODE_ALARME_COM_DMXNOTICE2		176
#define CODE_ALARME_TIMEOUT_DMXETUI		177
#define CODE_ALARME_TIMEOUT_DMXNOTICE1	178
#define CODE_ALARME_TIMEOUT_DMXNOTICE2	179

#define CODE_ALARME_CAM3				234
#define CODE_ALARME_CAM4				235
#define CODE_ALARME_PESAGE				236

//V 44.9 
#define CODE_ALARME_CADENCE_ROBOT1		293
#define CODE_ALARME_CADENCE_ROBOT2		294
#define CODE_ALARME_CADENCE_ROBOT3		295
#define CODE_ALARME_CADENCE_MACHINE		296

extern int iIndex_ALARME_OPC		;
extern int iIndex_ALARME_DMXETUI	;
extern int iIndex_ALARME_DMXNOTICE1	;
extern int iIndex_ALARME_DMXNOTICE2	;

extern int iIndex_ALARME_COM_DMXETUI;
extern int iIndex_ALARME_COM_DMXNOTICE1;
extern int iIndex_ALARME_COM_DMXNOTICE2;
extern int iIndex_ALARME_TIMEOUT_DMXETUI;
extern int iIndex_ALARME_TIMEOUT_DMXNOTICE1;
extern int iIndex_ALARME_TIMEOUT_DMXNOTICE2;

extern int iIndex_ALARME_CAM1		;
extern int iIndex_ALARME_CAM2		;
extern int iIndex_ALARME_CAM3		;
extern int iIndex_ALARME_CAM4		;
extern int iIndex_ALARME_CAM1_SERIAL;
extern int iIndex_ALARME_CAM1_LINK	;
extern int iIndex_ALARME_CAM1_SYNCHRO;
extern int iIndex_ALARME_CAM2_SERIAL;
extern int iIndex_ALARME_CAM2_LINK	;
extern int iIndex_ALARME_CAM2_SYNCHRO;
extern int iIndex_ALARME_PESAGE;//V44.1

//V 44.9 
extern int iIndex_ALARME_Cadence_ROBOT1;
extern int iIndex_ALARME_Cadence_ROBOT2;
extern int iIndex_ALARME_Cadence_ROBOT3;
extern int iIndex_ALARME_Cadence_Machine;

typedef struct _GROUPE_ALARME
{
	int NbAlarmesLiees;
	int NbDefauts;
	CString strLibelle;

}GROUPE_ALARME;

typedef struct _ALARME
{
	bool Traca;
	CString strLibelle;
	CString strOrigine;
	int Type;
	int idGroupe;
	int Numero;
	
	int Value, NewValue;

	long IdBase;

	BOOL * DB_Val;
	int DB_Num;
	int DB_Adresse;
	int DB_Bit;

}ALARME;


class CGestAlarmes
{
public:
	CGestAlarmes(void);
	~CGestAlarmes(void);

	//Variables Alarmes
	CString strErreur;

	ALARME Alarme[NB_ALARMES];

	GROUPE_ALARME GrpAlarmes[NBGROUPES];
	
	int NbTotal;
	int ChargerFromBase(void);

	void ResetAlarmes();
	int GererAlarmes(void);
	void InitialiserAlarmes();//GCA V1.16

	
};
