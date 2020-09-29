#pragma once


// CDynToolBar

class CDynToolBar : public CToolBar
{
	DECLARE_DYNAMIC(CDynToolBar)

// Construction
public:
CDynToolBar();

// Attributes
public:
    void  SetSizes(SIZE sizeButton, SIZE sizeImage)
    {
        GetToolBarCtrl( ).SetButtonSize(sizeButton);
        if(GetToolBarCtrl( ).SetBitmapSize(sizeImage))
        m_sizeImage=sizeImage;
    }

   inline void InitSizes(CSize cSizeBitmapSize) 
    { 
        m_SizeButton=cSizeBitmapSize;
        SetSizes(CSize(cSizeBitmapSize.cx+7,cSizeBitmapSize.cy+6),cSizeBitmapSize); 
        GetToolBarCtrl().AutoSize();    
    }
    bool  SetButtonWidth(UINT nMinWidth,UINT nMaxWidth);
    CSize GetButtonsWidth() const;
    CSize GetButtonsSize()  const;

    bool AddButtonToolBar(int nIndexPos,int nidCommand,UINT nIdbitmap,int istring=0);
    bool DeleteButtonToolBar(int nIndexPos);

    void RedrawToolBar(BOOL bRecalcLayout=TRUE,BOOL bOnlyFrame=FALSE);
    void RedrawButton(int nIndex);
    void UpdateSizes();

    void ReCalcDynamicLayout(CRect rect,int nIndexPos=-1);
	
	void EnableButton(int iPos, bool bEnabled);

// Attributes
private:
    CSize m_sizeMiniMaxi;
    CSize m_SizeButton;
    CSize m_sizeImage;

// Operations
public:

// Overrides
// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(CToolBarEx)
//}}AFX_VIRTUAL

// Implementation
public:
virtual ~CDynToolBar();

// Generated message map functions
protected:
//{{AFX_MSG(CToolBarEx)
// NOTE - the ClassWizard will add and remove member functions here.
//}}AFX_MSG

DECLARE_MESSAGE_MAP()
public:
	int SetBitmapBouton(int IndexBouton, UINT Bitmap);
};


