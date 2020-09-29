#pragma once


// CEditClick

class CEditClick : public CEdit
{
	DECLARE_DYNAMIC(CEditClick)

public:
	CEditClick();
	virtual ~CEditClick();

protected:
	DECLARE_MESSAGE_MAP()
public:
	bool bAlphaNum;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


