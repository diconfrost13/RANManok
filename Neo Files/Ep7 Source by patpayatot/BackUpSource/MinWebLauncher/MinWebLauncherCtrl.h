#pragma once

#include "objsafe.h"

#define UUID_LENGTH 37

// �� ���Ӻ��� ��ġ�� ������Ʈ���� ����� ���´�.
// �� ������Ʈ���� �̿��ؼ� ��ġ�Ǿ����� �ƴ����� �˻��Ѵ�.
namespace GAME_REGISTRY {

	// ������ ���Ӹ���Ʈ
	TCHAR szGameName[][256] = {
		_T("Ran Online"),
		_T("Ran Online Test") };
	
	// �� ������ ��ġ�Ǿ� �ִ� ������Ʈ�� ��ġ
	TCHAR szGameRegistry[][256] = {
		_T("SOFTWARE\\DaumGame\\RAN\\ExePath"),
		_T("SOFTWARE\\DaumGame\\RANTEST\\ExePath") };

}; // End of namespace GAME_REGISTRY

// MinWebLauncherCtrl.h : CMinWebLauncherCtrl ActiveX ��Ʈ�� Ŭ������ �����Դϴ�.

// CMinWebLauncherCtrl : ������ ������ MinWebLauncherCtrl.cpp��(��) �����Ͻʽÿ�.
class CMinWebLauncherCtrl : public COleControl
{
	DECLARE_DYNCREATE(CMinWebLauncherCtrl)

// �������Դϴ�.
public:
	CMinWebLauncherCtrl();

// ������
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

// ����
protected:
	~CMinWebLauncherCtrl();

	DECLARE_OLECREATE_EX(CMinWebLauncherCtrl)    // Ŭ���� ���͸��� GUID�Դϴ�.
	DECLARE_OLETYPELIB(CMinWebLauncherCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMinWebLauncherCtrl)     // �Ӽ� ������ ID�Դϴ�.
	DECLARE_OLECTLTYPE(CMinWebLauncherCtrl)		// ���� �̸��� ��Ÿ �����Դϴ�.

// �޽��� ���Դϴ�.
	DECLARE_MESSAGE_MAP()	

// ����ġ ���Դϴ�.
	DECLARE_DISPATCH_MAP()

// �̺�Ʈ ���Դϴ�.
	DECLARE_EVENT_MAP()

// Interface map
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(ObjSafe, IObjectSafety)
	STDMETHOD_(HRESULT, GetInterfaceSafetyOptions) ( 
		/* [in] */ REFIID riid,
		/* [out] */ DWORD __RPC_FAR *pdwSupportedOptions,
		/* [out] */ DWORD __RPC_FAR *pdwEnabledOptions
	);

	STDMETHOD_(HRESULT, SetInterfaceSafetyOptions) ( 
		/* [in] */ REFIID riid,
		/* [in] */ DWORD dwOptionSetMask,
		/* [in] */ DWORD dwEnabledOptions
	);
	END_INTERFACE_PART(ObjSafe);

// ����ġ�� �̺�Ʈ ID�Դϴ�.
public:
	enum {
		dispidInstallURL = 7,		dispid = 7,	dispidCheckRegi = 6L,	dispidGoDownloadPage = 5L,		dispidStartGame = 4L,		dispidUUID = 3,		dispidGameType = 2,		dispidDownloadURL = 1
	};

public:
	// static CString m_bstrDownloadURL;
	// �ڽ��� �ε��� ���������� ������
	IWebBrowser2* m_pIWeb;
	// �ڽ��� �ε��� ���������� �����͸� �����´�
	IWebBrowser2* GetIWebPointer(void);	
	
	// ������Ʈ���� üũ�ؼ� ������ ��ġ�Ǿ����� Ȯ���Ѵ�.
	bool CheckRegistry();
	void StartGame2( TCHAR* szUUID );
	void WebMessageBox( CString strMsg ); ///< �޽��� �ڽ��� ����.
	void WebNavigate( CString strURL ); ///< Ư�� ����Ʈ�� �̵���Ų��.		

public: // Wiziard Code
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

protected:
	CString m_DownloadURL;
	LONG m_GameType;
	CString m_UUID;

	void OnDownloadURLChanged(void);	
	void OnGameTypeChanged(void);	
	void OnUUIDChanged(void);	
	void StartGame(void); ///< ������ �����Ų��.
	void GoDownloadPage(void); ///< ���Ӵٿ�ε� �������� �̵���Ų��.
	BOOL CheckRegi(void); // Registery üũ�Ѵ�...
};