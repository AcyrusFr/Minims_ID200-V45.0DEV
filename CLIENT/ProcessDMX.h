#pragma once
/*#3 DMX
	Modification process DMX lecteurs Leuze
	communication en soket
*/

#include "GestAlarmes.h"
#include "ServSocketDMX.h"//#3 DMX

#include"ACYListBox.h"

#define FIN_TRAME_LEUZE	_T("END")
//DMXMode
#define DMX_NOCODE	0
#define DMX_ON	1
//#define DMX_OFF	-1

#define DMX_DISCONNECTED 0
#define DMX_CONNECTED 1
//#define DMX_DEFAULT 2

#define DMXALM_PING		0
#define DMXALM_COMM		1
#define DMXALM_TIMEOUT	2

//V2
CClientSocket * ExecuteOnConnect(CString strIpConnexion);

void ExecuteOnreceive_DMXEtui(CString strReceive, CString strIPReceive);
void ExecuteOnreceive_DMXNotice1(CString strReceive, CString strIPReceive);
void ExecuteOnreceive_DMXNotice2(CString strReceive, CString strIPReceive);

void ExecuteOnDeconnexion_DMXEtui();
void ExecuteOnDeconnexion_DMXNotice1();
void ExecuteOnDeconnexion_DMXNotice2();

//SockV3
void ExecuteOnArret_DMXEtui();
void ExecuteOnArret_DMXNotice1();
void ExecuteOnArret_DMXNotice2();

class CProcessDMX
{
public:
	CProcessDMX(void);
	~CProcessDMX(void);

	//Alarme
	BOOL * pBitEtatConnexion;
	ALARME * Alarme[3];//V 44.0 
	int iNumDMX;
	//#3 DMX
	CString IdDMX;//Code à lire

	//index Variables OPC
	int IndexDemandeLecture;

	//#3 DMX process automate
	BOOL * pBitInhibition;
	/*
	0 : UNDEF /RAZ
	1 : OK,Confime
	2 : MIXUP
	3 et + : Erreur lecture
	*/
	BYTE * iValResult;

	BOOL * pBitLecture;
	BOOL  oldBitLecture;

	//Affichage
	CACYListBox *ListeAffichage;

	//#3 DMX
	CClientSocket ClientSocket;
	CString strEnCours;

	//Resultats recus
	int iNbCodesRecus;
	CStringArray strCodesBrutes;//CodeLenght|||||||||1234
	CStringArray strCodes;//1234

	bool bLectureAttente;

	bool Inhiber();//SockV4
	bool Activer();//SockV4
	void Process_BitLecture();
	int OpenWebConfig(void);

	void Process_ReceptionCode(CString strResultsLeuze);
	void Process_Connexion(CString strIpConnexion);
	void Process_Deconnexion();

	void GererAlarme();
	
};
