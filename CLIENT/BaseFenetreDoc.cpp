// BaseFenetreDoc.cpp : implémentation de la classe CBaseFenetreDoc
//

#include "stdafx.h"
#include "BaseFenetreDoc.h"

#include "BaseFenetre.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBaseFenetreDoc

IMPLEMENT_DYNCREATE(CBaseFenetreDoc, CDocument)

BEGIN_MESSAGE_MAP(CBaseFenetreDoc, CDocument)
END_MESSAGE_MAP()


// construction ou destruction de CBaseFenetreDoc

CBaseFenetreDoc::CBaseFenetreDoc()
{
	//ajoutez ici le code d'une construction unique

}

CBaseFenetreDoc::~CBaseFenetreDoc()
{
}

BOOL CBaseFenetreDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	this->SetTitle("");

	return TRUE;
}




// sérialisation de CBaseFenetreDoc

void CBaseFenetreDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//  ajoutez ici le code de stockage
	}
	else
	{
		//  ajoutez ici le code de chargement
	}
}


// diagnostics pour CBaseFenetreDoc

#ifdef _DEBUG
void CBaseFenetreDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBaseFenetreDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// commandes pour CBaseFenetreDoc
