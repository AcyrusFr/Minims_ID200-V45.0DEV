#pragma once
//************************************
//définitions pour communication série
//**************************************
#define XON  17
#define XOFF 19
// Pour l'initialisation de la RS232
#define ASCII_XON	0x11
#define ASCII_XOFF	0x13

#define WM_MESSFORMCAM	WM_USER+2000
#define WM_AFFCHAINE	WM_USER+2001

#define TRIGGER_FREERUN			-1
#define TRIGGER_ONPULSE			-2

//#include "WndMessCom.h"
#include "GestAlarmes.h"//GCA V1.18

#define CAMALM_UNUSED	0
#define CAMALM_SERIAL 1
#define CAMALM_LINK 2//A VOIR
#define CAMALM_SYNCHRO 3//A VOIR

class CCamera
{
	//constructeur
public:
	CCamera(bool b_att =true);
	
	//destructeur
public:
	~CCamera(void);
	CDialog* frame;
	CDialog * DiagCam;
	BOOL scruter;
	BOOL ThreadEnded;
	char BuffReception[512];
	unsigned int iCurrentBufReceptLenght;
	CWinThread * ThreadLecture;
//************************************
//Prototypes fonctions pour communication série
//**************************************
public:
	ALARME * p_Alarme[4];//GCA V1.18 //V 44.0 changement nom pour eviter mélange

	BOOL Initialise_DCB();
	BOOL LanceLecture_RS232();
	
	static UINT ReadThread(LPVOID pParam);
	  virtual bool Ecrire_commande(CString commande);
	  virtual bool OpenPort(int numport,long vitesse, char parite, int longueur,int bit_stop);
	 virtual bool SetGain(int gain)
	 {
		 //si la fonction n'est pas définie dans les classes dérivés
		 //celle-ci sera executée
		 return false;
	 };
	 virtual bool SetGainPourcent(int gain);
	 
	 virtual bool SetTrigger(int mode)
	 {
		 return false;
	 };
	 virtual bool SetExposition(int expo)
	 {
		 return false;
	 };
	 virtual bool SetExpoMicroSec(int expoms);
	 
	 virtual bool Save_CamConfig(void)
	 {
		 return false;
	 };
	 virtual bool Set_PartialArea(int startline, int endline)
	 {
		 return false;
	 };
	 virtual bool Init_Cam(char * buffrecv)
	 {
		 return false;
	 };
	  virtual bool Ecrire_buff(char* buff, unsigned int nbre);
	  virtual bool Lire_buff(char* buff,size_t size_buff);
	 virtual void Open_testFrame();
	 
	 virtual bool SetFormat(int format)
	 {
		 return false;
	 };
	 virtual  void AfficheRep(char CharRecv);
	 

//************************************
//attributs/variables pour communication série
//**************************************
public:
	   public:
	   CDialog*			winMessCom;
       CString            m_StrError;
       DWORD              m_comerr;
       HANDLE             m_hCom;
       UINT               m_nInputBufferSize;
       UINT               m_nOutputBufferSize;
       DCB                m_dcb;
       DWORD              m_EventMask;
       COMMTIMEOUTS       m_ComTimeouts;
       OVERLAPPED         m_Ov;
	   OVERLAPPED		 osLect;
       unsigned int       m_count;

	   bool				  b_XonXoff;
       long               m_baudrate;
       char               m_parity;
       int                m_wordlength;
       int                m_stopbits;
	   int                m_numport;
	  // CString			  strTypecam;
	   CString			  str_charfin;
	   //variables pour controles camera
	   long Gain_min;
	   long Gain_max;
	   long Expo_min;
	   long Expo_max;
	   long Mode_min;
	   long Mode_max;
	   long Format_min;
	   long Format_max;
	   int val_gainP;

	   //variables cameras
	   	int gain;
		int gainPourcent;
		int mode;
		int expo;
		int expoInit;
		int Format;

		 CString			  Regkey;

	   //variables pour gestion d'erreur
	   bool			      b_aff_err;
	   bool				b_att_rep;
	   CString			  err;
	   CString			  last_command;

	   //autres variables
       CString			  str_DocsPath;
	   CString			  str_received;
	   CString			  str_camconfigfile;

	   //multicams
	   int iIdCam;

	   char PrevCarac;
};
