// MinWebLauncherCtrl.cpp : CMinWebLauncherCtrl ActiveX ��Ʈ�� Ŭ������ �����Դϴ�.

#include "stdafx.h"
#include "MinWebLauncher.h"
#include "MinWebLauncherCtrl.h"
#include "MinWebLauncherPropPage.h"
#include "helpers.h"
#include "Registry.h"
#include "strsafe.h"
#include ".\minweblauncherctrl.h"
#include "./DaumParam/DaumGameParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMinWebLauncherCtrl, COleControl)

// �޽��� ���Դϴ�.
BEGIN_MESSAGE_MAP(CMinWebLauncherCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// ����ġ ���Դϴ�.
BEGIN_DISPATCH_MAP(CMinWebLauncherCtrl, COleControl)
	DISP_PROPERTY_NOTIFY_ID(CMinWebLauncherCtrl, "DownloadURL", dispidDownloadURL, m_DownloadURL, OnDownloadURLChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CMinWebLauncherCtrl, "GameType", dispidGameType, m_GameType, OnGameTypeChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CMinWebLauncherCtrl, "UUID", dispidUUID, m_UUID, OnUUIDChanged, VT_BSTR)
	DISP_FUNCTION_ID(CMinWebLauncherCtrl, "StartGame", dispidStartGame, StartGame, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMinWebLauncherCtrl, "GoDownloadPage", dispidGoDownloadPage, GoDownloadPage, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMinWebLauncherCtrl, "CheckRegi", dispidCheckRegi, CheckRegi, VT_BOOL, VTS_NONE)
END_DISPATCH_MAP()

// �̺�Ʈ ���Դϴ�.
BEGIN_EVENT_MAP(CMinWebLauncherCtrl, COleControl)
END_EVENT_MAP()

// �Ӽ� �������Դϴ�.

// TODO: �ʿ信 ���� �Ӽ� �������� �߰��մϴ�. ī��Ʈ�� �̿� ���� �����ؾ� �մϴ�.
BEGIN_PROPPAGEIDS(CMinWebLauncherCtrl, 1)
	PROPPAGEID(CMinWebLauncherPropPage::guid)
END_PROPPAGEIDS(CMinWebLauncherCtrl)

// Ŭ���� ���͸��� GUID�� �ʱ�ȭ�մϴ�.
IMPLEMENT_OLECREATE_EX(CMinWebLauncherCtrl, "MINWEBLAUNCHER.MinWebLauncherCtrl.1",
	0x5fffa267, 0xb81, 0x42b4, 0xbe, 0x64, 0x77, 0xb5, 0xc9, 0xfe, 0x28, 0x7f)

// ���� ���̺귯�� ID�� �����Դϴ�.
IMPLEMENT_OLETYPELIB(CMinWebLauncherCtrl, _tlid, _wVerMajor, _wVerMinor)

// �������̽� ID�Դϴ�.
const IID BASED_CODE IID_DMinWebLauncher =
		{ 0xB0242861, 0x6F9F, 0x499F, { 0x8A, 0x8D, 0x9B, 0x53, 0xF2, 0xA8, 0x5, 0x68 } };
const IID BASED_CODE IID_DMinWebLauncherEvents =
		{ 0x55FE2265, 0x552A, 0x4624, { 0xA7, 0x1A, 0xBD, 0xA0, 0xB9, 0x66, 0xDA, 0x9C } };

// ��Ʈ�� ���� �����Դϴ�.
static const DWORD BASED_CODE _dwMinWebLauncherOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMinWebLauncherCtrl, IDS_MINWEBLAUNCHER, _dwMinWebLauncherOleMisc)

// CMinWebLauncherCtrl::CMinWebLauncherCtrlFactory::UpdateRegistry -
// CMinWebLauncherCtrl���� �ý��� ������Ʈ�� �׸��� �߰��ϰų� �����մϴ�.
BOOL CMinWebLauncherCtrl::CMinWebLauncherCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: ��Ʈ���� ����Ʈ �� ������ ��Ģ�� �ؼ��ϴ���
	// Ȯ���մϴ�. �ڼ��� ������ MFC Technical Note 64��
	// �����Ͻʽÿ�. ��Ʈ���� ����Ʈ �� ��Ģ��
	// ���� �ʴ� ��� �������� ���� ��° �Ű� ������ �����մϴ�.
	// afxRegApartmentThreading�� 0���� �����մϴ�.

	// Mark as safe for scripting-failure OK.
    HRESULT hr = CreateComponentCategory(CATID_SafeForScripting, 
        L"Controls that are safely scriptable");
    
    if (SUCCEEDED(hr))
        // Only register if category exists.
        RegisterCLSIDInCategory(m_clsid, CATID_SafeForScripting);
        // Don't care if this call fails.
    
    // Mark as safe for data initialization.
    hr = CreateComponentCategory(CATID_SafeForInitializing, 
        L"Controls safely initializable from persistent data");
    
    if (SUCCEEDED(hr))
        // Only register if category exists.
        RegisterCLSIDInCategory(m_clsid, CATID_SafeForInitializing);
        // Don't care if this call fails.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MINWEBLAUNCHER,
			IDB_MINWEBLAUNCHER,
			afxRegApartmentThreading,
			_dwMinWebLauncherOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

// CMinWebLauncherCtrl::CMinWebLauncherCtrl - �������Դϴ�.
CMinWebLauncherCtrl::CMinWebLauncherCtrl()
	: m_pIWeb( NULL )
{
	InitializeIIDs(&IID_DMinWebLauncher, &IID_DMinWebLauncherEvents);
	// TODO: ���⿡�� ��Ʈ���� �ν��Ͻ� �����͸� �ʱ�ȭ�մϴ�.
}

// CMinWebLauncherCtrl::~CMinWebLauncherCtrl - �Ҹ����Դϴ�.
CMinWebLauncherCtrl::~CMinWebLauncherCtrl()
{
	// TODO: ���⿡�� ��Ʈ���� �ν��Ͻ� �����͸� �����մϴ�.
}

// CMinWebLauncherCtrl::OnDraw - �׸��� �Լ��Դϴ�.
void CMinWebLauncherCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: ���� �ڵ带 ����ڰ� ���� �ۼ��� �׸��� �ڵ�� �ٲٽʽÿ�.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

// CMinWebLauncherCtrl::DoPropExchange - ���Ӽ� �����Դϴ�.
void CMinWebLauncherCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));

	// ���������� �Ķ���ͷ� �ѱ�� �Էµǰ� �ϱ� ���ؼ��� �̰��� ������ �־�� �Ѵ�.
	// �ٿ�ε� URL �� �ѱ��.
	PX_String( pPX, _T("DownloadURL"), m_DownloadURL );
	PX_String( pPX, _T("UUID"), m_UUID );
	PX_Long  ( pPX, _T("GameType"), m_GameType, 0 );
	
	COleControl::DoPropExchange(pPX);

	// TODO: �������� ����� ���� �Ӽ� ��ο� ���� PX_ functions�� ȣ���մϴ�.
}

// CMinWebLauncherCtrl::GetControlFlags -
// MFC�� ActiveX ��Ʈ�� ������ ����� �����ϴ� �÷����Դϴ�.
//
DWORD CMinWebLauncherCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// Ȱ�� �� ��Ȱ�� ���� ���̿��� ��ȯ�� ����
	// ��Ʈ���� �ٽ� �׸� �� �����ϴ�.
	dwFlags |= noFlickerActivate;
	return dwFlags;
}

// CMinWebLauncherCtrl::OnResetState - ��Ʈ���� �⺻ ���·� �ٽ� �����մϴ�.
void CMinWebLauncherCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange�� ��� �ִ� �⺻���� �ٽ� �����մϴ�.

	// TODO: ���⿡�� �ٸ� ��� ��Ʈ���� ���¸� �ٽ� �����մϴ�.
}

/////////////////////////////////////////////////////////////
// Interface map
BEGIN_INTERFACE_MAP( CMinWebLauncherCtrl, COleControl )
	INTERFACE_PART(CMinWebLauncherCtrl, IID_IObjectSafety, ObjSafe)
//	INTERFACE_PART(COcxPuzCtrl, IID_IObjectSafety, ObjSafe)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////
// IObjectSafety member functions
ULONG FAR EXPORT CMinWebLauncherCtrl::XObjSafe::AddRef()
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    return pThis->ExternalAddRef();
}

ULONG FAR EXPORT CMinWebLauncherCtrl::XObjSafe::Release()
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    return pThis->ExternalRelease();
}

HRESULT FAR EXPORT CMinWebLauncherCtrl::XObjSafe::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    return (HRESULT) pThis->ExternalQueryInterface(&iid, ppvObj);
}

const DWORD dwSupportedBits = 
        INTERFACESAFE_FOR_UNTRUSTED_CALLER |
        INTERFACESAFE_FOR_UNTRUSTED_DATA;
const DWORD dwNotSupportedBits = ~ dwSupportedBits;
        
HRESULT STDMETHODCALLTYPE 
    CMinWebLauncherCtrl::XObjSafe::GetInterfaceSafetyOptions( 
        /* [in] */ REFIID riid,
        /* [out] */ DWORD __RPC_FAR *pdwSupportedOptions,
        /* [out] */ DWORD __RPC_FAR *pdwEnabledOptions)
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)

    HRESULT retval = ResultFromScode(S_OK);

    // does interface exist?
    IUnknown FAR* punkInterface;
    retval = pThis->ExternalQueryInterface(&riid, 
                    (void * *)&punkInterface);
    if (retval != E_NOINTERFACE) {  // interface exists
        punkInterface->Release(); // release it--just checking!
    }
    
    // we support both kinds of safety and have always both set,
    // regardless of interface
    *pdwSupportedOptions = *pdwEnabledOptions = dwSupportedBits;

    return retval; // E_NOINTERFACE if QI failed
}

HRESULT STDMETHODCALLTYPE 
    CMinWebLauncherCtrl::XObjSafe::SetInterfaceSafetyOptions( 
        /* [in] */ REFIID riid,
        /* [in] */ DWORD dwOptionSetMask,
        /* [in] */ DWORD dwEnabledOptions)
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    
    // does interface exist?
    IUnknown FAR* punkInterface;
    pThis->ExternalQueryInterface(&riid,
        (void**)&punkInterface);
    if (punkInterface) {    // interface exists
        punkInterface->Release(); // release it--just checking!
    }
    else { // interface doesn't exist
        return ResultFromScode(E_NOINTERFACE);
    }

    // can't set bits we don't support
    if (dwOptionSetMask & dwNotSupportedBits) { 
        return ResultFromScode(E_FAIL);
    }
    
    // can't set bits we do support to zero
    dwEnabledOptions &= dwSupportedBits;
    // (we already know there are no extra bits in mask )
    if ((dwOptionSetMask & dwEnabledOptions) !=
            dwOptionSetMask) {
        return ResultFromScode(E_FAIL);
    }                               
    
    // don't need to change anything since we're always safe
    return ResultFromScode(S_OK);
}

// CMinWebLauncherCtrl �޽��� ó�����Դϴ�.

// ������Ʈ���� üũ�ؼ� ������ ��ġ�Ǿ����� Ȯ���Ѵ�.
bool CMinWebLauncherCtrl::CheckRegistry()
{
	if (m_GameType < 0 || m_GameType >= 10)
		m_GameType = 0;

	CRegString regGame(GAME_REGISTRY::szGameRegistry[m_GameType], _T(""), TRUE, HKEY_LOCAL_MACHINE);
	CString strReg = regGame.read();

	if (strReg == _T(""))
	{
		// ������Ʈ���� ����...
		// ��ġ���� �ʾҴٰ� �Ǵ��ϰ� �ٿ�ε� �������� �̵���Ų��.
		WebMessageBox( _T("������ ��ġ�Ǿ����� �ʽ��ϴ�.") );

		return false;

	}	

	return true;
}

void CMinWebLauncherCtrl::StartGame2( TCHAR* szUUID )
{
	if (m_GameType < 0 || m_GameType >= 10)
	m_GameType = 0;

	CRegString regGame(GAME_REGISTRY::szGameRegistry[m_GameType], _T(""), TRUE, HKEY_LOCAL_MACHINE);
	CString strReg = regGame.read();

    if (strReg == _T(""))
	{
		WebMessageBox( _T("������ ��ġ�� �ȵǾ��ֽ��ϴ�.") );
		GoDownloadPage();
	}
	else
	{
		// ������Ʈ���� ����...		
		CString strCmd = strReg;

		CDaumGameParameter cDGP;
		TCHAR szParameter[MAX_DGPARAMETER_LENGTH] = {0};
		size_t length = 0;
		::StringCchLength( szUUID, MAX_DGPARAMETER_LENGTH, &length );
		::StringCchCopy( szParameter, length+1 , szUUID );	

		if ( cDGP.Set ( "Parameter", szParameter ) == FALSE )
		{
			return;
		}

		// ���Ľ���
		int result = (int)ShellExecute( NULL, _T("open"), strCmd, NULL, NULL, SW_SHOWNORMAL);
		if ( result < 32 )
		{
			WebMessageBox( _T("���� ���ĸ� �����Ҽ� �����ϴ�.") );
			return;
		}

	}

}

void CMinWebLauncherCtrl::WebMessageBox(CString strMsg)
{
	if (m_pIWeb != NULL)
	{
		HWND hWnd = NULL;
		m_pIWeb->get_HWND( (long*) &hWnd );
		if (hWnd != NULL)
		{
			::MessageBox( hWnd,  strMsg, _T("Error"), MB_OK );
		}
	}
}

///< Ư�� ����Ʈ�� �̵���Ų��.
void CMinWebLauncherCtrl::WebNavigate(CString strURL)
{
	if (m_pIWeb != NULL)
	{		
		COleVariant varURL( strURL );
		m_pIWeb->Navigate2( varURL, NULL, NULL, NULL, NULL );
	}
}

void CMinWebLauncherCtrl::OnDownloadURLChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ �Ӽ� ó���� �ڵ带 �߰��մϴ�.
	SetModifiedFlag();
}

int CMinWebLauncherCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	// �ڽ��� �ε��� ���������� ������ ����
	m_pIWeb = GetIWebPointer();
	return 0;
}

void CMinWebLauncherCtrl::OnDestroy()
{
	COleControl::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	// ��Ʈ���� ������� OnCreate ���� ������ Web Browser �� �����͸� Release �� �ش�.
	if (m_pIWeb != NULL)
	    m_pIWeb->Release() ;
}

IWebBrowser2* CMinWebLauncherCtrl::GetIWebPointer(void)
{
	HRESULT hr;
	IOleContainer *pIContainer = NULL;
	IWebBrowser2 *pIWeb = NULL;
	IServiceProvider *pISP = NULL;
	
	// Get IOleClientSite interface pointer.
	LPOLECLIENTSITE pIClientSite = GetClientSite();
	
	// Get IOleContainer interface poineter.
	hr = pIClientSite->GetContainer( &pIContainer );
	if (hr != S_OK) 
	{
	    pIClientSite->Release();
	    return NULL;
	}
	
	// Get IServiceProvider interface pointer.
	hr = pIClientSite->QueryInterface(
							IID_IServiceProvider,
							(void **) &pISP );
	if (hr != S_OK) 
	{
	    pIContainer->Release();
	    pIClientSite->Release();
	    return NULL;
	}

	// Get IWebBrowser2 interface pointer.
	hr = pISP->QueryService(
					IID_IWebBrowserApp,
					IID_IWebBrowser2,
					(void**) &pIWeb );
	if (hr != S_OK) 
	{
	    pIContainer->Release();
	    pIClientSite->Release();
	    pISP->Release();
	    return NULL;
	}
	
	// release interface.
	pIContainer->Release();
	pIClientSite->Release();
	pISP->Release();
	return pIWeb;	
}

void CMinWebLauncherCtrl::OnGameTypeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ �Ӽ� ó���� �ڵ带 �߰��մϴ�.
	// WebMessageBox( _T("OnGameTypeChanged") );
	SetModifiedFlag();
}

void CMinWebLauncherCtrl::OnUUIDChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ �Ӽ� ó���� �ڵ带 �߰��մϴ�.
	// WebMessageBox( _T("OnUUIDChanged") );
	SetModifiedFlag();
}

void CMinWebLauncherCtrl::StartGame(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   	// TODO: ���⿡ ����ġ ó���⸦ �߰��մϴ�.
	if ( m_UUID.GetLength() < (UUID_LENGTH-1) )
	{
		WebMessageBox( _T("Please login at the official website to start Ran-Online.") );
		return;
	}

	TCHAR szUUID[UUID_LENGTH] = {0};
	::StringCchCopy( szUUID, UUID_LENGTH, m_UUID.GetBuffer() );	
	StartGame2( szUUID );
}
void CMinWebLauncherCtrl::GoDownloadPage(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ ����ġ ó���⸦ �߰��մϴ�.
	if (m_pIWeb != NULL)
	{
		WebNavigate( m_DownloadURL );
	}
}

BOOL CMinWebLauncherCtrl::CheckRegi(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if ( CheckRegistry() )
	{
		return TRUE;
	}
	
	return FALSE;

}



