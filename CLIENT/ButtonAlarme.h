#pragma once

#define ID_TIMER_BUTALARME	3000
class CButtonAlarme : public CButton
{

public:
	CButtonAlarme(void);
	virtual ~CButtonAlarme(void);


	bool bAlarme;
	UINT_PTR TimerCligno;
	bool bCligno;

	void Init(void);
protected:
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
