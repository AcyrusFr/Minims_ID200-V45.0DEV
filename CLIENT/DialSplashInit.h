#pragma once


// Boîte de dialogue CDialSplashInit

class CDialSplashInit : public CDialog
{
	DECLARE_DYNAMIC(CDialSplashInit)

public:
	CDialSplashInit(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CDialSplashInit();

// Données de boîte de dialogue
	enum { IDD = IDD_DIAG_SPLASH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CString strInitInfos;
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
};
