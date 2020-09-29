// BaseFenetreView.h : interface de la classe CBaseFenetreView
//


#pragma once
#include "BaseFenetreDoc.h"

class CBaseFenetreView : public CView
{
protected: // création à partir de la sérialisation uniquement
	CBaseFenetreView();
	DECLARE_DYNCREATE(CBaseFenetreView)

// Attributs
public:
	CBaseFenetreDoc* GetDocument() const;

// Opérations
public:

// Substitutions
public:
	virtual void OnDraw(CDC* pDC);  // substitué pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implémentation
public:
	virtual ~CBaseFenetreView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // version de débogage dans BaseFenetreView.cpp
inline CBaseFenetreDoc* CBaseFenetreView::GetDocument() const
   { return reinterpret_cast<CBaseFenetreDoc*>(m_pDocument); }
#endif

