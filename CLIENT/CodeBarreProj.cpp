#include "StdAfx.h"
#include "CodeBarreProj.h"


/*****************************************************************************/

CCodeBarreProj::CCodeBarreProj(void)
{
	iOrientation = CB_ORIENTATION_GD;
}

CCodeBarreProj::~CCodeBarreProj(void)
{
}

//int CCodeBarreProj::Extraire(long* pData, long lNbData, long lDiviseur)
int CCodeBarreProj::Extraire(long* pData, long lNbData, long lDiviseur)
{
	//DIMENSIONNEMENT DES VARIABLES
	//long *pLissage;
	//pLissage = (long*) malloc( sizeof(long) * 1000);
	//
	//long *pDiff;
	//pDiff = (long*) malloc( sizeof(long) * 1000);

	//int *barre;
	//barre = (int*) malloc( sizeof(int) * 1000);		//100 barres maxi
	//memset( barre, 0, lNbData * sizeof( int ));

	long minval = 0;
	long maxval = 0;

	//A paramétrer
	int iLissage = 6;
	sCode[0] = 'N';
	sCode[1] = 'C';
	sCode[2] = 0;

	////RECHERCHE DES SEUILS DE DETECTION SUR LA COURBE DIFFERENTIELLE
	long histo[256];
	//recherche des seuils par histogramme
	long histoseuil=0;
	long  histotemp=0;
	memset( histo, 0, 256 * sizeof(long));	//V31ALL : (long), pas (int) sinon plantage application

	if(lNbData>900)		//V31ALL : au cas ou...
	{
		return 0;	
	}

	if (iLissage>0)
	{
		for(int li=0;li<lNbData-iLissage;li++)
		{
			long lLissage = 0;
			pLissage[li] = 0;
			for(int i=0;i<iLissage;i++)
				pLissage[li] = pLissage[li] + pData[li+i];
			pLissage[li] = pLissage[li]/(long)(iLissage);
		}
		for(int li=lNbData-iLissage;li<lNbData;li++)
		{
			pLissage[li] = pLissage[lNbData-iLissage-1];
		}
	}
	else
	{
		for(int li=0;li<lNbData-iLissage;li++)
		{
			pLissage[li] = pLissage[li]/(long)(iLissage);
		}
	}

	//Courbe différentielle
	int iDecalageDiff = 1;
	for(int li=0;li<lNbData-iDecalageDiff;li++)
	{
		pDiff[li] = pLissage[li+1]-pLissage[li];
	}
	for(int li=lNbData-iDecalageDiff;li<lNbData;li++)
	{
		pDiff[li] = 0;
	}
		
	//Histogramme
	for(int li=0;li<lNbData-1;li++)
	{
		histo[pDiff[li]/lDiviseur] = histo[pDiff[li]/lDiviseur] + 1;
	}
	for(int i=255;i>0;i--)
	{
		histotemp = histotemp +  histo[i];
		if(histotemp>10)
		{
			histoseuil = (long)(i * lDiviseur);
			break;
		}
	}
	minval = histoseuil*(-1)/3;
	maxval = histoseuil/3;

	//RECHERCHE DE LA POSITION DU CODE BARRE
	posdebut = lNbData/2;
	posfin = lNbData/2;
	int posdebutMin = lNbData/2;
	int posfinMin = lNbData/2;
	int posdebutMax = lNbData/2;
	int posfinMax = lNbData/2;
	long distanceMax = 30;
	int etp=0;
	for(int li=lNbData/2;li>0;li--)
	{
		if(labs(pDiff[li])>maxval)
		{
			posdebut = li;
		}
		if((posdebut-li) > distanceMax)
		{
			break;
		}
	}
	for(int li=lNbData/2;li<lNbData;li++)
	{
		if(labs(pDiff[li])>maxval)
		{
			posfin = li;
		}
		if((li-posfin) > distanceMax)
		{
			break;
		}
	}

	posdebut = posdebut - distanceMax;
	if(posdebut<0)	posdebut = 0;
	posfin = posfin + distanceMax;
	if(posfin>(lNbData-1))	posfin = lNbData-1;


	//V31ALL : A gerer en fonction des besoins
	//CString mFichier;
	//FILE * fic;
	//mFichier = "C:\\ACY_CLIENTS\\BAUSCHLOMB\\CodeProj"+Libelle+".txt";
	//if ((fopen_s(&fic,mFichier.GetString(),"w+")) == NULL ) 
	//{
	//	for(int i=0;i<lNbData;i++)
	//	{
	//		fprintf(fic,"%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n",maxval,posdebut,posfin,pData[i],pLissage[i],pDiff[i]);
	//	}
	//	fclose(fic);
	//}


	//LECTURE DES BARRES
	iNbBarre=0;
	int iSens = 1;
	long lastmin = 0;
	long lastmax = 0;
	bool PassageZeroOK = false;


	bool bAttenteDebutPic=true;
	bool bAttenteFinPic=false;
	long MaxPicVal = 0;
	long MaxPicPos = 0;
	long lInverseur = -1;
	int BarreDebut[100];
	int BarreFin[100];
bool bFinPic = false;//wxc
	iNbBarre = -1;
	for(int li=posdebut;li<posfin;li++)
	{
		//Attente de passer le seuil
		if(bAttenteDebutPic)
		{
			if((pDiff[li]*lInverseur)>maxval)
			{
				bAttenteDebutPic = false;
				MaxPicVal = pDiff[li]*lInverseur;
				MaxPicPos = li;
				bAttenteFinPic = true;
			}
		}
		if(bAttenteFinPic)
		{
			//Mémorisation de la position du pic max
			if((pDiff[li]*lInverseur)>MaxPicVal)
			{
				MaxPicVal = pDiff[li]*lInverseur;
				MaxPicPos = li;
			}
			//Attente de la fin du pic
//wxc
			bFinPic = false;
			if(lInverseur==1)
			{
				if(pDiff[li]<(-maxval))	bFinPic = true;
			}
			else
			{
				if(pDiff[li]>maxval)	bFinPic = true;
			}
			if(bFinPic)
//wxc
			//if(pDiff[li]*lInverseur < MaxPicVal*lInverseur/5)
			{
				iNbBarre++;
				if(iNbBarre < 100)	// AFAIRE
				{
					//Inversion de sens
					//=> Finalisation de la barre précédente
					if(iNbBarre>0)
					{
						BarreFin[iNbBarre-1] = MaxPicPos;
						barre[iNbBarre-1] = (BarreFin[iNbBarre-1] - BarreDebut[iNbBarre-1]) * lInverseur;
					}
					//=> Initialisation de la barre suivante
					BarreDebut[iNbBarre] = MaxPicPos;

					//RAZ des variables pour la suite
					bAttenteDebutPic = true;
					bAttenteFinPic = false;
					MaxPicVal = 0;
					MaxPicPos = 0;
					lInverseur = -lInverseur;
				}
			}
		}
	}

//wxc Cloture de la derniere barre
iNbBarre++;
if(iNbBarre < 100)	// AFAIRE
{
	//Inversion de sens
	//=> Finalisation de la barre précédente
	if(iNbBarre>0)
	{
		BarreFin[iNbBarre-1] = MaxPicPos;
		barre[iNbBarre-1] = (BarreFin[iNbBarre-1] - BarreDebut[iNbBarre-1]) * lInverseur;
	}
}

	if(iNbBarre>99)
	{
		return -6;
	}

	//V31ALL : A gerer en fonction des besoins
	//mFichier = "C:\\ACY_CLIENTS\\BAUSCHLOMB\\CodeBarres" + Libelle + ".txt";
	//if ((fopen_s(&fic,mFichier.GetString(),"w+")) == NULL ) 
	//{
	//	for(int i=0;i<iNbBarre;i++)
	//	{
	//		fprintf(fic,"%ld\n",barre[i]);
	//	}
	//	fclose(fic);
	//}

	int iRep = 0;
	iRep = Lire_Code2of5();

	//free(pLissage);
	//free(pDiff);
	//free(barre);

	return iRep;
}

void CCodeBarreProj::RechercheLargeurBarre(int iDebut, int iFin)
{
	int HistoBarre[100];

	for(int i=0;i<100;i++)
	{
		HistoBarre[i] = 0;
	}
	for(int i=iDebut;i<=iFin;i++)
	{
		if(abs(barre[i])<100)
		{
			HistoBarre[abs(barre[i])] ++;
		}
	}

	//Recherche du trou entre fines et larges
	int HistoOrdre[100];
	int iOrdre = 0;
	for(int i=6;i<20;i++)
	{
		if( HistoBarre[i] > 0)
		{
			HistoOrdre[iOrdre] = i;
			iOrdre++;
		}
	}
	for(int i=0;i<iOrdre;i++)
	{
		if( HistoOrdre[i+1] - HistoOrdre[i] > 3)
		{
			iSeuilBarre = (HistoOrdre[i+1] + HistoOrdre[i])/2;
			break;
		}
	}
	iSeuilBarreNoire = iSeuilBarre; 
	iSeuilBarreBlanche = -iSeuilBarre;
}

void CCodeBarreProj::CreerMotif()
{
	for(int i=0;i<iNbBarre;i++)
	{
		if(barre[i]>0)
		{
			if(barre[i]>iSeuilBarreNoire)
			{
				motif[i]='2';
			}
			else
			{
				motif[i]='1';
			}
		}
		else
		{
			if(barre[i]<iSeuilBarreBlanche)
			{
				motif[i]='4';
			}
			else
			{
				motif[i]='3';
			}
		}
	}
	motif[iNbBarre]=0;
}

long CCodeBarreProj::ExtraireMotif(int iDebut, int iFin, int iPas)
{
	char sTemp[100];
	int i;

	for(i=iDebut;i<=iFin;i+=iPas)
	{
		sTemp[i-iDebut]= motif[i];
	}
	sTemp[i-iDebut]=0;
	return atol(sTemp);
}


int CCodeBarreProj::Lire_Code2of5()
{
	//EXTRACTION DU CODE	
	int iNbLarge;
	int iSeuilLocal;
	int iReprise;
	int iInc=0;
	//char sCode[100];
	int i,j;
	long result = 0;
	int iRep = 1;
	//NOMBRE 2 int iNombreExacte = iNbBarre;
	int iNombreExacte = 0;
	
	//Test nombre de barres
	//Début			= 2 noires + 2 blanches = 4
	//Fin			= 2 noires + 1 blanche  = 2
	//Caractères	= 5 noires + 5 blanches = 10 (multiple)
	//TOTAL	minimum	= 9 noires + 8 blanches = 17
	
	//minimum
	if(iNbBarre < 27)
	{
		return -1;
	}
	//maximum
	if(iNbBarre > 80)	///A VOIR
	{
		return -2;
	}
	//Nombre exacte
	iNombreExacte = iNbBarre;
	int iReste = 0;
	iReste = iNbBarre - 7;
	iReste = iReste %10;
	if(iReste != 0)
	{
		//ESSAI NOMBRE 1
		bNbrExact = false;
		iNombreExacte = iNbBarre - 7;
		iNombreExacte = iNombreExacte / 10;
		iNombreExacte = iNombreExacte*10 + 7;
	}
	else
	{
		bNbrExact = true;
	}

	//HISTOGRAMME DE LA LARGEUR DES BARRES	
	iSeuilBarre = 15;
	iSeuilBarreNoire = 15;
	iSeuilBarreBlanche = -15;


	RechercheLargeurBarre(0,iNbBarre-1);
	CreerMotif();


	//ESSAI NOMBRE 1 : Recherche du caractère de début / Fin à partir du début
	int iSensLecture = 0;
	bool bDebutOK[4];
	bool bFinOK[4];
	int iDebut[4];
	int iFin[4];

	long ResDebut;
	long ResFin;
	//Sens 0 : Aligné à gauche,sens positif
	for(i=0;i<=(iNbBarre-iNombreExacte);i++)
	{
		iDebut[0] = i;
		iFin[0] = i+iNombreExacte-3;

		RechercheLargeurBarre(iDebut[0],iFin[0]+2);
		CreerMotif();

		ResDebut = ExtraireMotif(iDebut[0],iDebut[0]+3,1);
		ResFin = ExtraireMotif(iFin[0],iFin[0]+2,1);
		bDebutOK[0] = false;
		bFinOK[0] = false;
		if(ResDebut == 1313)
		{
			bDebutOK[0] = true;
		}
		if(ResFin == 231)
		{
			bFinOK[0] = true;
		}
		if(bDebutOK[0] && bFinOK[0])
		{
			break;
		}
	}

	if(!(bDebutOK[0] && bFinOK[0]))
	{
		//Sens 1 : Aligné à gauche ,sens negatif
		for(i=0;i<=(iNbBarre-iNombreExacte);i++)
		{
			iDebut[1] = i + iNombreExacte-4;
			iFin[1] = i;

			RechercheLargeurBarre(iFin[1],iDebut[1]+3);
			CreerMotif();

			ResDebut = ExtraireMotif(iDebut[1],iDebut[1]+3,1);
			ResFin = ExtraireMotif(iFin[1],iFin[1]+2,1);
			bDebutOK[1] = false;
			bFinOK[1] = false;
			if(ResDebut == 3131)
			{
				bDebutOK[1] = true;
			}
			if(ResFin == 132)
			{
				bFinOK[1] = true;
			}
			if(bDebutOK[1] && bFinOK[1])
			{
				break;
			}
		}
	}

	int *br;
	int iBr = 0;
	int iTemp = 0;
	br = (int*) malloc( sizeof(int) * 100);
	memset( br, 0, 100 * sizeof( int ));
	if(bDebutOK[0] && bFinOK[0])
	{
		//RechercheLargeurBarre(iDebut[0]+4,iFin[0]);
		//CreerMotif();
		for(int i=iDebut[0]+4;i<(iFin[0]);i++)
		{
			br[iBr] = barre[i];
			iBr++;
		}
	}
	else if(bDebutOK[1] && bFinOK[1])
	{
		//RechercheLargeurBarre(iFin[1]+3,iDebut[1]-1);
		//CreerMotif();
		for(int i=iDebut[1]-1;i>(iFin[1]+2);i--)
		{
			br[iBr] = barre[i];
			iBr++;
		}
	}
	else
	{
		return - 4;
		free(br);
	}


	iInc = -1;
	for(i=0;i<iBr;i+=10)
	{
		iSeuilLocal = iSeuilBarre;
		iReprise = 0;
		iNbLarge = 0;

		
		//BARRES NOIRES
		iInc++;
		for(j=0;j<5;j++)
		{
			if(br[i+j*2]>iSeuilLocal)
			{
				//Barre large
				motif[j]='2';
				iNbLarge++;
			}
			else
			{
				//Barre étroite
				motif[j]='1';
			}
		}

		motif[5]=0;
		result = atol(motif);
		switch(result)
		{
			case 11221:
				sCode[iInc]='0';
				break;
			case 21112:
				sCode[iInc]='1';
				break;
			case 12112:
				sCode[iInc]='2';
				break;
			case 22111:
				sCode[iInc]='3';
				break;
			case 11212:
				sCode[iInc]='4';
				break;
			case 21211:
				sCode[iInc]='5';
				break;
			case 12211:
				sCode[iInc]='6';
				break;
			case 11122:
				sCode[iInc]='7';
				break;
			case 21121:
				sCode[iInc]='8';
				break;
			case 12121:
				sCode[iInc]='9';
				break;
			default:
				sCode[iInc]='_';
				iRep = 0;
				//pExt->bResultOK = false;
				break;

		}
		//if(App.bPrintDetail)		printf("Résultat = %s %ld %c\n",motif,result,sCode[iInc]);

		//ESPACES
		iInc++;
		for(j=0;j<5;j++)
		{
			if(br[i+j*2+1]<iSeuilLocal*-1)
			{
				//Barre large
				motif[j]='2';
				iNbLarge++;
			}
			else
			{
				//Barre étroite
				motif[j]='1';
			}
		}

		motif[5]=0;
		result = atol(motif);
		switch(result)
		{
			case 11221:
				sCode[iInc]='0';
				break;
			case 21112:
				sCode[iInc]='1';
				break;
			case 12112:
				sCode[iInc]='2';
				break;
			case 22111:
				sCode[iInc]='3';
				break;
			case 11212:
				sCode[iInc]='4';
				break;
			case 21211:
				sCode[iInc]='5';
				break;
			case 12211:
				sCode[iInc]='6';
				break;
			case 11122:
				sCode[iInc]='7';
				break;
			case 21121:
				sCode[iInc]='8';
				break;
			case 12121:
				sCode[iInc]='9';
				break;
			default:
				sCode[iInc]='_';
				iRep = 0;
				//pExt->bResultOK = false;
				break;
		}
		//if(App.bPrintDetail)		printf("Résultat = %s %ld %c\n",motif,result,sCode[iInc]);
	}

	sCode[iInc+1]=0;

	free(br);
	return iRep;
}


