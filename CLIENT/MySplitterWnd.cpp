#include "StdAfx.h"
#include "MySplitterWnd.h"

BEGIN_MESSAGE_MAP(CMySplitterWnd, CSplitterWnd)
//{{AFX_MSG_MAP(CMySplitterWnd)
ON_WM_SETCURSOR()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------
BOOL CMySplitterWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if(!m_bExTracking) return FALSE;
    return CSplitterWnd::OnSetCursor(pWnd, nHitTest, message);
}
//---------------------------------------
void CMySplitterWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
   if(!m_bExTracking) return ;
   CSplitterWnd::OnMouseMove(nFlags, point);
}