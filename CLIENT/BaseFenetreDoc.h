// BaseFenetreDoc.h : interface de la classe CBaseFenetreDoc
//


#pragma once


class CBaseFenetreDoc : public CDocument
{
protected: // cr�ation � partir de la s�rialisation uniquement
	CBaseFenetreDoc();
	DECLARE_DYNCREATE(CBaseFenetreDoc)

// Attributs
public:

// Op�rations
public:

// Substitutions
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Impl�mentation
public:
	virtual ~CBaseFenetreDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions g�n�r�es de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
};


