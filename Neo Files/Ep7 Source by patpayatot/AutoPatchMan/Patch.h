#pragma once

// ���� ���ϰ� ���ϸ���Ʈ ��( ����, Ŭ���̾�Ʈ )
// ��ġ ������ Ŭ���̾�Ʈ ���ϸ���Ʈ ���� ��
// ���� ��ġ ���� ���� Ȯ��( extract�� ���� )
//
// ���� Ŭ���̾�Ʈ ��ġ ���� '�ڵ���ġ'�� ��� ������ ��ġ�Ѵ�.
// Ŭ���̾�Ʈ ���ϸ���Ʈ�� ��ġ���� ���ϸ���Ʈ�� ���ؾ���.
// ��ġ���� ���������� �߸��Ǿ��ų�, ������ ���ϸ���Ʈ�� �߸��Ǿ���.
// ������ ���ϸ���Ʈ�� �߸� �Ǿ��ٴ� �̾߱�� Ŭ���̾�Ʈ '���ϸ���Ʈ'�� ���۵� ���Ŀ�
// ��ġ������ ������ ������ �ٲ�ų� Ŭ���̾�Ʈ '���ϸ���Ʈ'�� ���� �̿����� ��ġ�����ʹ�
// �ٸ� ������ ���۵� ���̴�. (������ġ�� �ѹ��̶� ����Ǿ��°�? )
// ���������� ��ġ�� '������ġ'�� Ȯ���� ��, �� ������ ������
//
// ��ġ ����Ʈ�� ���ԵǾ�� �ȵǴ� ������ ������ �ʴ°�?
// (��Ʈ���� '\'�� '*.dll, Launcher*.exe, cFileList.bin, cVer.bin'�� �����ϸ� �ȵȴ�. )
// �� ������ ���ԵǾ� ���� ���, ��ġ�ϴٰ� �����ϰų� ��쿡 ���� �߸��� ��ġ�� ��
// ���ɼ��� ����.

#include "GlobalVariable.h"

const int nFILENAME = 64;
const int nSUBPATH = 128;

struct	SFILENODE
{
	char	FileName[nFILENAME];
	char	SubPath[nSUBPATH];	
	int		Ver;

public:
	SFILENODE()
	{
		memset ( FileName, 0, sizeof ( char ) * nFILENAME );
		memset ( SubPath, 0, sizeof ( char ) * nSUBPATH );
		Ver = 0;
	}
};

typedef	std::vector<SFILENODE*>		FILEVECTOR;
typedef	FILEVECTOR::iterator		FILEVECTOR_ITER;

typedef	std::map<std::string, SFILENODE*>	FILEMAP;
typedef	FILEMAP::iterator					FILEMAP_ITER;


class	CPatch;
class	CHttpPatch;

BOOL	Initialize ();
BOOL	LoadList ();
BOOL	MakeNewList ( const int cPatchVer, const int sPatchVer,
					 const int cGameVer, const int sGameVer );
//BOOL	DownloadFilesByFtp ( CPatch* pFtpPatch );
BOOL	DownloadFilesByHttp ( CHttpPatch* pHttpPatch );
BOOL	Installing ();
BOOL	Extract ();
BOOL	DeleteNotFoundFile();
BOOL	DeleteDownFiles ();
BOOL	Destroy ();
BOOL	VersionUp ( int sGameVer );
//BOOL	CheckIntegrity ( CString strPath );

//	Note : Download Success List ( DS LIst ) ����
//		���� + ���� ���� + ����Ʈ
//		��� SFILENODE ����ü�� �̿��Ѵ�.
BOOL	SaveDownList ( int sGameVer );
BOOL	LoadDownList ();
BOOL	DeleteDownList ();

BOOL	SaveCopyList ( int sGameVer );
BOOL	LoadCopyList ();
BOOL	DeleteCopyList ();

//	NOTE
//		
BOOL	GETFILE_USEHTTP ( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir = NS_GLOBAL_VAR::strDownloadTemp );

//	NOTE
//		��ġ ����
//struct	S_PATCH_THREAD_PARAM;
//DWORD	PatchByHTTP ( S_PATCH_THREAD_PARAM* pParam );
//DWORD	PatchByFTP ( S_PATCH_THREAD_PARAM* pParam );