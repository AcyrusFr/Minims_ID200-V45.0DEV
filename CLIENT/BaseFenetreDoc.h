// BaseFenetreDoc.h : interface de la classe CBaseFenetreDoc
//


#pragma once


class CBaseFenetreDoc : public CDocument
{
protected: // création à partir de la sérialisation uniquement
	CBaseFenetreDoc();
	DECLARE_DYNCREATE(CBaseFenetreDoc)

// Attributs
public:

// Opérations
public:

// Substitutions
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implémentation
public:
	virtual ~CBaseFenetreDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
};


