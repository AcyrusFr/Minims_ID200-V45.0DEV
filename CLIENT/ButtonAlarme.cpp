#include "StdAfx.h"
#include "ButtonAlarme.h"

CButtonAlarme::CButtonAlarme(void)
{
	bAlarme = false;
	bCligno = true;


}

CButtonAlarme::~CButtonAlarme(void)
{
		TimerCligno = NULL;
}

BEGIN_MESSAGE_MAP(CButtonAlarme, CButton)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// Gestionnaires de messages de CStaticVisu

void CButtonAlarme::Init(void)
{
	TimerCligno = ::SetTimer(this->GetSafeHwnd(),ID_TIMER_BUTALARME, 500,NULL);
}

void CButtonAlarme::OnPaint()
{
	CButton::OnPaint();//dessin du bouton

	//init dessin perso
	CPen	penLisiere;
	CBrush brush;
	CRect RectWindow;

	this->GetWindowRect(&RectWindow);
	RectWindow.NormalizeRect();
	this->ScreenToClient(&RectWindow);
	CClientDC dc(this);
	

	//dessin perso
	if(bAlarme)
	{
		if(bCligno)
		{
			penLisiere.CreatePen(PS_SOLID,3, RGB(255,0,0) );
			dc.SelectObject(&penLisiere);
			dc.SelectStockObject(NULL_BRUSH);
			dc.SetBkMode(TRANSPARENT);

			dc.Rectangle(RectWindow.left+2,RectWindow.top +2,RectWindow.right-1,RectWindow.bottom-1);
		}
		
	}
	
	

	// N'appelez pas CStatic::OnPaint() pour la peinture des messages
}

void CButtonAlarme::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == ID_TIMER_BUTALARME)
	{
		bCligno = (!bCligno);
		Invalidate();
	}
}
