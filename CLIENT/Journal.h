#pragma once
#include "Base.h"

#define JOURNAL_TYPELOGIN	0
#define JOURNAL_TYPEALARME	1
#define JOURNAL_TYPEINFO	2
#define JOURNAL_TYPEMODEDEGRADE	3
#define JOURNAL_TYPEMIXUP	4
#define JOURNAL_TYPEALERTE	5
#define JOURNAL_TYPE_JPARAM_GENERAL	100
#define JOURNAL_TYPE_JPARAM_VISION	101
#define JOURNAL_TYPE_JPARAM_RECETTEVISION	200

class CJournal
{
public:
	CJournal(void);
	~CJournal(void);

	//Variables BDD
#pragma warning (disable: 4995)
	CADODatabase * pBase;
#pragma warning (default: 4995)
	CString strTable;
	CString sErreur;

	int Ajouter(int Type, CString Libelle, CString Commentaire);
	int Clore(long CleID);
	int Evenement(int Type, CString Libelle, CString Commentaire, CString strUser);
};
