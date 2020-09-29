// BaseFenetreView.cpp : implémentation de la classe CBaseFenetreView
//

#include "stdafx.h"
#include "BaseFenetre.h"

#include "BaseFenetreDoc.h"
#include "BaseFenetreView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBaseFenetreView

IMPLEMENT_DYNCREATE(CBaseFenetreView, CView)

BEGIN_MESSAGE_MAP(CBaseFenetreView, CView)
END_MESSAGE_MAP()

// construction ou destruction de CBaseFenetreView

CBaseFenetreView::CBaseFenetreView()
{
	//  ajoutez ici du code de construction

}

CBaseFenetreView::~CBaseFenetreView()
{
}

BOOL CBaseFenetreView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// dessin de CBaseFenetreView

void CBaseFenetreView::OnDraw(CDC* /*pDC*/)
{
	CBaseFenetreDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//  ajoutez ici le code de dessin pour les données natives
}


// diagnostics pour CBaseFenetreView

#ifdef _DEBUG
void CBaseFenetreView::AssertValid() const
{
	CView::AssertValid();
}

void CBaseFenetreView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBaseFenetreDoc* CBaseFenetreView::GetDocument() const // la version non déboguée est en ligne
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBaseFenetreDoc)));
	return (CBaseFenetreDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CBaseFenetreView
