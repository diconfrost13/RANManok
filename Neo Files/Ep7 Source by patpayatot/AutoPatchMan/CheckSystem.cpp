#include "stdafx.h"
#include "Resource.h"
#include "CheckSystem.h"
#include "RANPARAM.h"
#include "GlobalVariable.h"
#include "GetDxVer.h"
#include "s_NetClient.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_CHECK_SYSTEM
{	
	DWORD	GetDirectXVersion ()
	{
		DWORD dwVersion = 0;

		CString strTemp;
		strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), g_szClientVerFile );
		SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

		FILE* fp = fopen ( strTemp.GetString(), "rb+" );
		if ( fp )
		{
			const long DWORDSIZE = sizeof ( DWORD );
			const int sPos = ftell ( fp );
			fseek ( fp, 0, SEEK_END );
			const int ePos = ftell ( fp );
			fseek ( fp, 0, SEEK_SET );
			const int FileSize = ePos - sPos;

			if ( FileSize == (DWORDSIZE * 2) )	//	���� Dx������ �߰����� ���� ����
			{
				dwVersion = GetDXVersion ();
				fseek ( fp, 0, SEEK_END );
				if ( 1 != fwrite ( &dwVersion, sizeof ( DWORD ), 1, fp ) )
				{				
					fclose ( fp );
					return 0;
				}
			}
			else if ( FileSize == (DWORDSIZE * 3) )	//	Dx������ �߰��� ����
			{			
				fseek ( fp, -(DWORDSIZE), SEEK_END );

				//	���� �б�
				if ( 1 != fread ( &dwVersion, sizeof ( DWORD), 1, fp ) )
				{
					fclose ( fp );
					return 0;
				}

				if ( dwVersion < NS_GLOBAL_VAR::g_DxVersion )	//	�о�� ������ 8.1������ ���
				{
					dwVersion = GetDXVersion ();	//	�ٽ� ������ ������
					fseek ( fp, -(DWORDSIZE), SEEK_END );
					if ( 1 != fwrite ( &dwVersion, sizeof ( DWORD), 1, fp ) )
					{				
						fclose ( fp );
						return 0;
					}
				}
			}
			
			fclose ( fp );
		}
		else
		{
			//NS_LOG_CONTROL::Write ( ID2LAUNCHERTEXT("IDS_MESSAGE", 29 ) ); // ��ó ���࿡ �ʿ��� ������ ����
		}

		return dwVersion;
	}
};