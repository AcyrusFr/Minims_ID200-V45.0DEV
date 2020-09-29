#include "StdAfx.h"

#include "ComHTML.h"
#include <winsock2.h>
#include "Ipexport.h"
#include <icmpapi.h>

CComHTML::CComHTML(void)
{
	pSession = NULL;
	pConnection = NULL ;
	pFile = NULL;
	IsInitialized = false;
	strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
}

CComHTML::~CComHTML(void)
{
}

bool CComHTML::SendPOSTcmd(CStringA strCmd)
{
	if(!IsInitialized) 
	{
		strErreur = _T("Connexion non Initialisee");
		strRep = _T("Connexion non Initialisee");
		return false;
	}

	char strFormData[255] ={0};

	strcpy_s(strFormData,strCmd.GetBuffer());
	strCmd.ReleaseBuffer();

	try
	{
		pFile =
		  pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
								  _T("/cgi-bin"));
	  
	   BOOL result = pFile->SendRequest(strHeaders,
		  (LPVOID)strFormData,(DWORD) strlen(strFormData));
	
	   if(result == 0)
	   {
			strErreur = "Erreur SendRequest";
	   }
		//Reception réponse
		CString strTemp;
		strRep.Empty();
	
		while(pFile->ReadString(strTemp))
		{
			strRep += strTemp;
		}

		 pFile->Close();
		delete(pFile);
		pFile = NULL;
		

	}
	catch(CInternetException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = "ERREUR SendPOSTcmd HTML \nDesc :\n";
		strErreur = strErreur + szCause;
		return false;
	}
	return true;
}

bool CComHTML::SendGETcmd(CStringA strCmd)
{
	if(!IsInitialized) 
	{
		strErreur = _T("Connexion non Initialisee");
		return false;
	}

	char strFormData[255] ={0};

	strcpy_s(strFormData,strCmd.GetBuffer());
	strCmd.ReleaseBuffer();

	try
	{
		pFile =
		  pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET,
								  _T("/cgi-bin"));
	  
	   BOOL result = pFile->SendRequest(strHeaders,
		  (LPVOID)strFormData,(DWORD) strlen(strFormData));
		
	  /* CString strUrl = _T("");
	   CString strCookieName = _T("");
	   CString strCookieData = _T("");
	   strUrl = _T("http://") + strServeur;
	   strCookieName = _T("http-session-id");

	   pConnection->GetSession()->GetCookie(strUrl,strCookieName,strCookieData);

#ifdef _UNICODE
		lSessionId = _wtol(strCookieData);
#else
		lSessionId = atol(strCookieData);
#endif*/

	   if(result == 0)
	   {
			strErreur = "Erreur SendRequest";
	   }
		//Reception réponse
		CString strTemp;
		strRep.Empty();
	
		while(pFile->ReadString(strTemp))
		{
			strRep += strTemp;
		}

//		strRep += _T("CookieData:") + strCookieData;

		 pFile->Close();
		delete(pFile);
		pFile = NULL;
		

	}
	catch(CInternetException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = "ERREUR SendPOSTcmd HTML \nDesc :\n";
		strErreur = strErreur + szCause;
		return false;
	}
	return true;
}

bool CComHTML::Connect(void)
{
	// Connect
	if(IsInitialized) //Déjà Connecté
	{
		strErreur = _T("Connexion Déjà Initialisee");
		return false;
	}

	try
	{

		pConnection = pSession->GetHttpConnection(strServeur);
		 
	}
	catch(CInternetException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = "ERREUR Connect HTML \nDesc :\n";
		strErreur = strErreur + szCause;
		return false;
	}
	
	IsInitialized = true;
	return true;
}

bool CComHTML::Disconnect(void)
{

	// Dispose Internet Connexion
	if(!IsInitialized) 
	{
		strErreur = _T("Connexion non Initialisee");
		return false;
	}

	try
	{
		
		pConnection->Close();
		delete(pConnection);
		pConnection = NULL;

	}
	catch(CInternetException* e)
	{
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause,255);
		e->Delete();
		strErreur = "ERREUR Disconnect HTML \nDesc :\n";
		strErreur = strErreur + szCause;
		return false;
	}
	IsInitialized = false;
	return true;
}

bool CComHTML::TraiterReponse(CString sChamp, CString sParam, CString* sValue)
{
	  CString strjob,strParam,strValue,strTemp2,strNomJob;
  int index =0;
  int index2 =0,indexParam = 0,indexValue = 0;
  int indexJob = -1;
  CString strJobId;



  bool bFinded = false;

  //Traitement reponse xml
  CString strTemp0,strTemp1,strTemp3;

  strTemp0 = strRep.Mid(255);
  strTemp1 = strRep.Mid(512);
  strTemp3 = strRep.Mid(767);

  do
  {
	indexJob = strRep.Find(sChamp,index);
	if(indexJob > 0)
	{
		strjob = strRep.Mid(indexJob);
		do
		{
			strTemp2 = strjob.Tokenize(_T("<"),index2);
			indexParam = strTemp2.Find(_T("param id"),0);
			indexValue = strTemp2.Find(_T("value"),0);
			
			if((indexParam != -1)&&(indexValue != -1))//format <param id=xx value=yy/>
			{
				strParam = strTemp2.Mid(indexParam,((indexValue -1)- indexParam));
				strValue = strTemp2.Mid(indexValue);
				
				strParam.Replace(_T("param id="),_T(""));
				strValue.Replace(_T("value="),_T(""));
				strValue.Replace(_T("/>"),_T(""));

				strParam.Trim('"');
				strValue.Trim('"');


			}
			else if(indexParam != -1)//format <param id=xx>YYYYY</param>
			{
				indexValue = strTemp2.Find(_T(">"),0);

				if(indexValue != -1)
				{
					strParam = strTemp2.Mid(indexParam,((indexValue -1)- indexParam));
					strValue = strTemp2.Mid(indexValue);
					
					strParam.Replace(_T("param id="),_T(""));
					strValue.Replace(_T("</param>"),_T(""));

					strParam.Trim('"');
					strValue.Trim('"');
					strValue.Trim('>');
				}
			}

			if(strParam == sParam)
			{
				*sValue = strValue;

				bFinded = true;
				//Arret du traitement on a ce qu'il nous faut
				index2 = -1;
				index = -1;
			}

		}
		while(index2>0);

	}

  }
  while (index > 0);

	return bFinded;
}

bool CComHTML::IsWebConnected(CString strWebSiteOrIP)
{
	//TEST TO REMOVEreturn false;

   // --- Quit if incorrect website to ping
   if (strWebSiteOrIP == _T("")) return false; // no string


   // --- Initialise the use of "Ws2_32.dll"
   WSADATA WSAData;
   if (WSAStartup(MAKEWORD(2,2),&WSAData) != 0) return false;

  
   // --- Retrieve the host information
   // --- corresponding to a host name from a host database
   bool     bRet = false;
   HOSTENT* Host = gethostbyname(strWebSiteOrIP);
   if (Host)
   {
      SOCKADDR_IN SocketIn;
      SocketIn.sin_family           = AF_INET;
      SocketIn.sin_port             = htons(80); // HTTP port
      SocketIn.sin_addr.S_un.S_addr = (DWORD)*((DWORD*)Host->h_addr_list[0]);

	  IP_OPTION_INFORMATION ioi;
	  ICMP_ECHO_REPLY       ierr;
	  HANDLE hFile = IcmpCreateFile();

	  ioi.Ttl         = 255;
	  ioi.Tos         = 0;
	  ioi.Flags       = 0;
	  ioi.OptionsSize = 0;
	  ioi.OptionsData = NULL;
      
	  DWORD replySize = sizeof(ICMP_ECHO_REPLY);

	  if (replySize < 40)
	  {
		replySize = 40;
	  }

	  int iNbRecv = (int) IcmpSendEcho(hFile,*((DWORD*)Host->h_addr_list[0]),NULL,0,&ioi,&ierr,replySize,5000);
	 // IcmpCloseHandle(hFile);
	 // bRet = true;

	  if(iNbRecv > 0)
	  {
		  switch (ierr.Status)
		  {
		  case IP_SUCCESS     :  
			  bRet = true;
			  break;

			case IP_BUF_TOO_SMALL      :           
			case IP_DEST_NET_UNREACHABLE       :           
			case IP_DEST_HOST_UNREACHABLE      :           
			case IP_DEST_PROT_UNREACHABLE      :           
			case IP_DEST_PORT_UNREACHABLE      :           
			case IP_NO_RESOURCES         :           
			case IP_BAD_OPTION          :           
			case IP_HW_ERROR           :           
			case IP_PACKET_TOO_BIG       :           
			case IP_REQ_TIMED_OUT       :           
			case IP_BAD_REQ        :           
			case IP_BAD_ROUTE       :           
			case IP_TTL_EXPIRED_TRANSIT        :           
			case IP_TTL_EXPIRED_REASSEM        :           
			case IP_PARAM_PROBLEM          :           
			case IP_SOURCE_QUENCH         :           
			case IP_OPTION_TOO_BIG         :           
			case IP_BAD_DESTINATION         : 
			default:
				//Erreur
				  Disconnect();
				  bRet = false;

				break;

		 
			  break;
		  }
	  }
	  else
	  {

		DWORD dwError = GetLastError();
		dwError = 0;

		//Erreur
		Disconnect();
		bRet = false;
	  }

	  IcmpCloseHandle(hFile);
   }
   
   
   // --- Release the resources
   WSACleanup();
   return bRet;
}
