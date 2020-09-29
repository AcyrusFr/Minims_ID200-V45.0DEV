// DynToolBar.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "BaseFenetre.h"
#include "DynToolBar.h"
#include "resource.h"


// CDynToolBar

IMPLEMENT_DYNAMIC(CDynToolBar, CToolBar)


CDynToolBar::CDynToolBar()
{
    m_SizeButton.cx=16;
    m_SizeButton.cy=15;

}

CDynToolBar::~CDynToolBar()
{
}


BEGIN_MESSAGE_MAP(CDynToolBar, CToolBar)
//{{AFX_MSG_MAP(CDynToolBar)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynToolBar message handlers
bool CDynToolBar::SetButtonWidth(UINT nMinWidth,UINT nMaxWidth)
{ 
    ASSERT(::IsWindow(GetSafeHwnd()));
    ASSERT(nMaxWidth-m_sizeImage.cx>=7);
    if(SendMessage(TB_SETBUTTONWIDTH, 0, MAKELPARAM(nMinWidth, nMaxWidth)))
    {
        m_sizeMiniMaxi.cx=nMinWidth;
        m_sizeMiniMaxi.cy=nMaxWidth;
        return true;
    }
    return false; 
}
//----------------------------------------------------------------
CSize CDynToolBar::GetButtonsWidth() const
{ 
    ASSERT(::IsWindow(GetSafeHwnd()));
    return m_sizeMiniMaxi; 
}
//----------------------------------------------------------------
CSize CDynToolBar::GetButtonsSize() const
{ 
    ASSERT(::IsWindow(GetSafeHwnd()));
    DWORD result=(DWORD)::SendMessage(m_hWnd,TB_GETBUTTONSIZE,0,(LPARAM)0); 
    return CSize(LOWORD(result),HIWORD(result)); 
}
//----------------------------------------------------------------
void CDynToolBar::RedrawToolBar(BOOL bRecalcLayout/*=TRUE*/, 
   BOOL bOnlyFrame/*=FALSE*/)
{
    ASSERT(::IsWindow(GetSafeHwnd()));

    if(!IsWindowVisible())return;

    if(bRecalcLayout)
    {
        CWnd *pParent=GetToolBarCtrl( ).GetParent();
        CFrameWnd* pFrameWnd=(CFrameWnd *)pParent->GetParent();
        if(pFrameWnd!=NULL)
        {
            pFrameWnd->RecalcLayout();
            for(int nIndex=0; nIndex<GetToolBarCtrl( ).GetButtonCount(); nIndex++)
            {
                RedrawButton(nIndex);
                CRect rect;
                GetItemRect(nIndex,rect);
                ValidateRect(rect);
            }
        }
    }
    else
    {
        if(!bOnlyFrame)
        {
            GetToolBarCtrl( ).RedrawWindow(NULL,NULL,
            RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_FRAME|RDW_ALLCHILDREN);
        }
    }
    if(bOnlyFrame)
    {
        GetToolBarCtrl( ).SetWindowPos(NULL,0,0,0,0,
        SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_DRAWFRAME);
    }
}
//----------------------------------------------------------------
void CDynToolBar::RedrawButton(int nIndex)
{
    ASSERT(::IsWindow(GetSafeHwnd()));
    
    if(nIndex<0 || nIndex>GetToolBarCtrl().GetButtonCount())
    {
        return;
    }
    CRect rect;
    GetToolBarCtrl( ).GetItemRect(nIndex,rect);
    GetToolBarCtrl( ).RedrawWindow(rect);
}
//----------------------------------------------------------------
void CDynToolBar::UpdateSizes() 
{ 
    SetSizes(GetButtonsSize(),m_sizeImage); 
    GetToolBarCtrl().AutoSize();
}
//----------------------------------------------------------------
bool CDynToolBar::DeleteButtonToolBar(int nIndexPos)
{
    CRect rect; 
    GetToolBarCtrl( ).GetWindowRect(&rect);
    
    if(!GetToolBarCtrl( ).DeleteButton(nIndexPos)) return false;
    
    // resize window 
    rect.right-=(m_SizeButton.cx+7);
    SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),
    SWP_NOMOVE|SWP_NOZORDER|SWP_DRAWFRAME|SWP_FRAMECHANGED);
    
    ReCalcDynamicLayout(rect);
    return true;
}
//----------------------------------------------------------------
void CDynToolBar::ReCalcDynamicLayout(CRect rect,int nIndexPos/*=-1*/)
{
    SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),
        SWP_NOMOVE|SWP_NOZORDER|SWP_DRAWFRAME|SWP_FRAMECHANGED);
    
    if(IsFloating())  
    {
        CPoint newPos(0,0);        
        ClientToScreen(&newPos);
        CRect rcNew;
        //  GetToolBarCtrl().SetRows(GetToolBarCtrl().GetRows(),TRUE, &rcNew);
        
        CalcDynamicLayout(rect.Width(),LM_HORZ | LM_COMMIT);
        
        CWnd *pParent=GetToolBarCtrl( ).GetParent();
        CFrameWnd* pFrameWnd=(CFrameWnd *)pParent->GetParent();
        
        if(pFrameWnd)
            pFrameWnd->FloatControlBar(this, newPos,CBRS_ALIGN_TOP | CBRS_SIZE_DYNAMIC);
    }
    
    RedrawToolBar();
    if(nIndexPos>0)RedrawButton(nIndexPos);
    
}
//----------------------------------------------------------------
bool CDynToolBar::AddButtonToolBar(int nIndexPos,int nidCommand,UINT nIdbitmap,int istring/*=0*/)
{
    BOOL bok;
    
    CRect rect; 
    GetToolBarCtrl( ).GetWindowRect(&rect);
    
    TBBUTTON Buttons;
    int res = GetToolBarCtrl( ).AddBitmap(1,nIdbitmap);

	if(res == -1)
	{
		AfxMessageBox("Erreur création nouveau bitmap bouton");
		Buttons.iBitmap=nIndexPos;
	}
	else
	{
		 Buttons.iBitmap=res;
	}
   
    Buttons.idCommand=nidCommand;
    Buttons.fsState=TBSTATE_ENABLED;
    Buttons.fsStyle=TBSTYLE_BUTTON;
    Buttons.dwData=0;
    Buttons.iString=istring;
    bok=GetToolBarCtrl( ).AddButtons(1,&Buttons);
    
    SetButtonWidth(0,m_SizeButton.cx+7);
    
    InitSizes(m_SizeButton);
    
    // resize window 
    rect.right+=(m_SizeButton.cx+7);
    
    ReCalcDynamicLayout(rect,nIndexPos);
    return (bok?true:false);
}

void CDynToolBar::EnableButton(int iPos, bool bEnabled)
{
	
	if(bEnabled)
	{
		GetToolBarCtrl().SetState(iPos,TBSTATE_ENABLED);
	}
	else
	{
		GetToolBarCtrl().SetState(iPos,GetToolBarCtrl().GetState(iPos)|TBSTATE_HIDDEN);
	}
	

	RedrawToolBar();
	//RedrawButton(iPos);
	
}
// Gestionnaires de messages de CDynToolBar



int CDynToolBar::SetBitmapBouton(int IndexBouton, UINT Bitmap)
{

	TBBUTTON Bouton;
	int iNum = 0;
	this->GetToolBarCtrl().AddBitmap(iNum,Bitmap);
	
	GetToolBarCtrl().GetButton(IndexBouton,&Bouton);

	SetButtonInfo(IndexBouton,Bitmap,TBSTYLE_BUTTON,iNum);
	return 0;
}
