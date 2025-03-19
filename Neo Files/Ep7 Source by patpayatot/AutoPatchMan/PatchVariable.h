#pragma once

enum emFileList
{	
	FILENAME_SIZE	= 64,
	SUBPATH_SIZE	= 128,
	MD5_SIZE		= 33, // MD5�� 32�ڸ� + ���Ṯ�� 1 = 33

	VER_1_OFFSET	= 196,
	VER_2_OFFSET	= 232 // 4����Ʈ ���ķ� 229�� �ƴ� 3����Ʈ �þ 232
};

struct	SFILENODE
{
	// Version 1
	CHAR FileName[FILENAME_SIZE];
	CHAR SubPath[SUBPATH_SIZE];
	INT Ver;
	// Version 2
	CHAR szMD5[MD5_SIZE];

public:
	SFILENODE() :
	  Ver(0)
	{
		SecureZeroMemory( FileName, sizeof( FileName ) );
		SecureZeroMemory( SubPath, sizeof( SubPath ) );
		SecureZeroMemory( szMD5, sizeof( szMD5 ) );
	}
};

typedef	std::vector<SFILENODE*>		FILEVECTOR;
typedef	FILEVECTOR::iterator		FILEVECTOR_ITER;

typedef	std::map<std::string, SFILENODE*>	FILEMAP;
typedef	FILEMAP::iterator					FILEMAP_ITER;