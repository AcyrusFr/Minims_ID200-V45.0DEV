// BaseFenetreView.h : interface de la classe CBaseFenetreView
//


#pragma once
#include "BaseFenetreDoc.h"

class CBaseFenetreView : public CView
{
protected: // cr�ation � partir de la s�rialisation uniquement
	CBaseFenetreView();
	DECLARE_DYNCREATE(CBaseFenetreView)

// Attributs
public:
	CBaseFenetreDoc* GetDocument() const;

// Op�rations
public:

// Substitutions
public:
	virtual void OnDraw(CDC* pDC);  // substitu� pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Impl�mentation
public:
	virtual ~CBaseFenetreView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions g�n�r�es de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // version de d�bogage dans BaseFenetreView.cpp
inline CBaseFenetreDoc* CBaseFenetreView::GetDocument() const
   { return reinterpret_cast<CBaseFenetreDoc*>(m_pDocument); }
#endif

