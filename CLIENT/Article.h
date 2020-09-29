#pragma once

class CArticle
{
public:
	CArticle(void);
	~CArticle(void);

	//Base de données
	long ID;
	CString Code;
	CString Description;
	int Type;
	CString Article1;
	CString Article2;
	CString Article3;
	CString Article4;
	CString Article5;
	CString Article6;	//#2 Emballage
	int iParam1;
	int iParam2;
	CString CodeDMX;

	int* SourceDataRecette;

	bool ChargerFromCode(CString strCode);
	bool Ajouter(void);
	void Copy(CArticle * ArtDest);
	void Reset(void);
};
