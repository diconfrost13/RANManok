///////////////////////////////////////////////////////////////////////////////
// s_CDbGame.cpp
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CDbmanager.h"
#include "GLogicData.h"
#include "s_CCfg.h"
#include <strstream>
#include "GLCharAG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ���ο� ĳ���͸� �����Ѵ�.
// ����ڹ�ȣ
// ĳ��������
// ���� �׷�
// ĳ���͸�
int CDbmanager::CreateNewCharacter(SCHARDATA2* pCharData2)
{
	DB_LIST* pDB = GetFreeConnectionGame();	
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nRetCode = 0;
	
	int nChaNum = 0;
	int nChaMaxNum = 0;
	DWORD dwUserNum = pCharData2->m_dwUserID;
	DWORD dwSvrNum = pCharData2->m_dwServerID;

	// SCHARDATA2 sCha;
	// sCha.operator =n;
	
	// ���� ������ ĳ���� ������ �����´�.
	std::strstream strTemp2;
	strTemp2 << "SELECT COUNT(*) FROM ChaInfo WHERE UserNum=";
	strTemp2 << dwUserNum;
	strTemp2 << " AND SGNum=";
	strTemp2 << dwSvrNum;
	strTemp2 << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp2.str());
	
	nRetCode = dbsqlexec(pDBProc);
	if (nRetCode == FAIL)
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	nRetCode = dbresults(pDBProc);
	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL)
	{
		// Bind data
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nChaMaxNum );
		dbnextrow(pDBProc); // get rows
		dbcancel(pDBProc);
        // �̹� ������ ĳ���� ���ں��� ���� ������������ ����..
		if (nChaMaxNum >= MAX_SERVERCHAR_NUM)
		{
			ReleaseConnectionGame(pDB);
			return DB_CHA_MAX;
		}
	} 
	else 
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	// ���ο� ĳ���͸� �����Ѵ�.
	// ��ø ChaDex, wDex
	// ���� ChaIntel, wInt
	// ü�� ChaStrong, wStr
	// ��   ChaPower, wPow
	// ���� ChaSpirit, wSpi
	// �ٷ� ChaStrength, wSta
	std::strstream strTemp;
	strTemp << "INSERT INTO ChaInfo(UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaBright, ChaLevel, ChaMoney, ChaDex, ChaIntel,";
	strTemp << "ChaStrong, ChaPower, ChaSpirit, ChaStrength, ChaStRemain, ";
	strTemp << "ChaAttackP, ChaDefenseP, ChaFightA, ChaShootA, ChaExp, ";
	strTemp << "ChaSkillPoint, ChaHP, ChaMP, ChaSP, ChaPK, ";
	strTemp << "ChaStartMap, ChaStartGate, ChaPosX, ChaPosY, ChaPosZ,";
	strTemp << "ChaSkills, ChaSkillSlot, ChaPutOnItems, ChaInven ) Values ( ";
		
	strTemp << dwUserNum << ",";
	strTemp << dwSvrNum << ",";
	strTemp << "'" << pCharData2->m_szName << "',";
	strTemp << pCharData2->m_emTribe << ","; 
	strTemp << pCharData2->m_emClass << ",";

    strTemp << pCharData2->m_nBright << ",";
	strTemp << pCharData2->m_wLevel << ",";
	strTemp << pCharData2->m_lnMoney << ","; 
	strTemp << pCharData2->m_sStats.wDex << ","; // ��ø ChaDex, wDex
	strTemp << pCharData2->m_sStats.wInt << ","; // ���� ChaIntel, wInt

	strTemp << pCharData2->m_sStats.wStr << ","; // ü�� ChaStrong, wStr
	strTemp << pCharData2->m_sStats.wPow << ","; // ��   ChaPower, wPow
	strTemp << pCharData2->m_sStats.wSpi << ","; // ���� ChaSpirit, wSpi
	strTemp << pCharData2->m_sStats.wSta << ","; // �ٷ� ChaStrength, wSta
	strTemp << pCharData2->m_wStatsPoint << ",";

	strTemp << pCharData2->m_wAP << ",";
	strTemp << pCharData2->m_wDP << ",";
	strTemp << pCharData2->m_wPA << ",";
	strTemp << pCharData2->m_wSA << ",";
	strTemp << pCharData2->m_sExperience.lnNow << ",";

	strTemp << pCharData2->m_dwSkillPoint << ",";		
	strTemp << pCharData2->m_sHP.dwData << ",";
	strTemp << pCharData2->m_sMP.dwData << ",";
	strTemp << pCharData2->m_sSP.dwData << ",";
	strTemp << pCharData2->m_wPK << ",";

	strTemp << pCharData2->m_sStartMapID.dwID << ",";
	strTemp << pCharData2->m_dwStartGate << ",";
	strTemp << pCharData2->m_vStartPos.x << ",";
	strTemp << pCharData2->m_vStartPos.y << ",";
	strTemp << pCharData2->m_vStartPos.z << ",";

	strTemp << " '', '', '', '')";
	strTemp << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp.str());	
    	
	// send command buffer to SQL server
	nRetCode = dbsqlexec(pDBProc);
	
	if (nRetCode == FAIL) 
	{
		ReleaseConnectionGame(pDB);
		return DB_CHA_DUF;
	}
	dbcancel(pDBProc);

	// ������ ĳ������ ������ȣ�� �����´�.
	dbcmd(pDBProc, (LPCSTR) "SELECT @@IDENTITY" );
	dbsqlexec(pDBProc);
	nRetCode = dbresults(pDBProc);

	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL) 
	{
		// ĳ���� ������ȣ ����
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nChaNum );
		dbnextrow(pDBProc);
		pCharData2->m_dwCharID; 
		ReleaseConnectionGame(pDB);
		
		CByteStream ByteStream;
		LPBYTE pBuffer = NULL;
		DWORD dwSize;
		// Character Skill 
		pCharData2->GETEXPSKILLS_BYBUF(ByteStream);		
		ByteStream.GetBuffer( pBuffer, dwSize);
		WriteImage("ChaInfo.ChaSkills", nChaNum, (char *) pBuffer, dwSize );

		// Character Skill Quikc Slot
		pCharData2->GETSKILL_QUICKSLOT(ByteStream);		
		ByteStream.GetBuffer( pBuffer, dwSize);
		WriteImage("ChaInfo.ChaSkillSlot", nChaNum, (char *) pBuffer, dwSize );

		// Character Put on item
		pCharData2->GETPUTONITEMS_BYBUF(ByteStream);
		ByteStream.GetBuffer( pBuffer, dwSize);
		WriteImage("ChaInfo.ChaPutOnItems", nChaNum, (char *) pBuffer, dwSize);

		// Character Inventory
		pCharData2->m_cInventory.GETITEM_BYBUFFER(ByteStream);
		ByteStream.GetBuffer( pBuffer, dwSize);		
		WriteImage("ChaInfo.ChaInven", nChaNum, (char *) pBuffer, dwSize);

		// User Inventory
		// 1. Check User Inven		
		// 2. If exist skip
		// 3. not exist, write iventory image
		BOOL bInven = CheckInven(CCfg::GetInstance()->GetServerGroup(), dwUserNum);
		if (!bInven)
		{
			MakeUserInven(CCfg::GetInstance()->GetServerGroup(), dwUserNum);
		}			
		return nChaNum;
	} 
	else 
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

// ĳ���͸� �����Ѵ�
int	CDbmanager::SaveCharacter ( LPVOID _pbuffer )
{
	SCHARDATA2* pCharData2 = reinterpret_cast<SCHARDATA2*> ( _pbuffer );

	if (pCharData2 == NULL)
		return DB_ERROR;

	DWORD dwChaNum = pCharData2->m_dwCharID;
	
	if (dwChaNum < 0)
		return DB_ERROR;
	
	DWORD dwUserNum = pCharData2->m_dwUserID;
	DWORD dwSvrNum = pCharData2->m_dwServerID;
	DWORD dwChaID = pCharData2->m_dwCharID;
	

	// ��ø ChaDex, wDex
	// ���� ChaIntel, wInt
	// ü�� ChaStrong, wStr
	// ��   ChaPower, wPow
	// ���� ChaSpirit, wSpi
	// �ٷ� ChaStrength, wSta

	// ĳ���͸� ���� �Ѵ�.
	std::strstream strTemp;
	strTemp << "UPDATE ChaInfo SET ";
	strTemp << "  ChaBright=" << pCharData2->m_nBright;		
	strTemp << ", ChaLevel=" << pCharData2->m_wLevel;
	strTemp << ", ChaMoney=" << pCharData2->m_lnMoney;
	strTemp << ", ChaDex=" << pCharData2->m_sStats.wDex;		// ��ø
	strTemp << ", ChaIntel=" << pCharData2->m_sStats.wInt;		// ����
	
	strTemp << ", ChaStrong=" << pCharData2->m_sStats.wStr;		// ü��
	strTemp << ", ChaPower=" << pCharData2->m_sStats.wPow;		// ��
	strTemp << ", ChaSpirit=" << pCharData2->m_sStats.wSpi;		// ����
	strTemp << ", ChaStrength=" << pCharData2->m_sStats.wSta;	// �ٷ�
	strTemp << ", ChaStRemain=" << pCharData2->m_wStatsPoint;

	strTemp << ", ChaAttackP=" << pCharData2->m_wAP;	
	strTemp << ", ChaDefenseP=" << pCharData2->m_wDP;
	strTemp << ", ChaFightA=" << pCharData2->m_wPA;
	strTemp << ", ChaShootA=" << pCharData2->m_wSA;
	strTemp << ", ChaExp=" << pCharData2->m_sExperience.lnNow;

	strTemp << ", ChaSkillPoint=" << pCharData2->m_dwSkillPoint;	
	strTemp << ", ChaHP=" << pCharData2->m_sHP.dwData;
	strTemp << ", ChaMP=" << pCharData2->m_sMP.dwData;
	strTemp << ", ChaSP=" << pCharData2->m_sSP.dwData;
	strTemp << ", ChaPK=" << pCharData2->m_wPK;

	strTemp << ", ChaStartMap=" << pCharData2->m_sStartMapID.dwID;	
	strTemp << ", ChaStartGate=" << pCharData2->m_dwStartGate;
	strTemp << ", ChaPosX=" << pCharData2->m_vStartPos.x;
	strTemp << ", ChaPosY=" << pCharData2->m_vStartPos.y;
	strTemp << ", ChaPosZ=" << pCharData2->m_vStartPos.z;

    // Add : 2003-09-24 
    strTemp << ", ChaSaveMap="  << pCharData2->m_sSaveMapID.dwID;
    strTemp << ", ChaSavePosX=" << pCharData2->m_vSavePos.x;
    strTemp << ", ChaSavePosY=" << pCharData2->m_vSavePos.y;
    strTemp << ", ChaSavePosZ=" << pCharData2->m_vSavePos.z;
    ////////////////////////////////////////////////////////////

	strTemp << "  WHERE ChaNum=" << dwChaNum;
	strTemp << '\0';
	
	DB_LIST* pDB = GetFreeConnectionGame();	
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;	

	dbcmd(pDBProc, (LPCSTR) strTemp.str());
    	
	// send command buffer to SQL server
	nRetCode = dbsqlexec(pDBProc);
	
	if (nRetCode == FAIL) 
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}		
	ReleaseConnectionGame(pDB);
	
	CByteStream ByteStream;
	LPBYTE pBuffer = NULL;
	DWORD dwSize;
	// Skill 
	pCharData2->GETEXPSKILLS_BYBUF(ByteStream);		
	ByteStream.GetBuffer( pBuffer, dwSize);	
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaSkills", dwChaNum, (char *) pBuffer, dwSize );
	pBuffer = NULL;

	// Skill Quick Slot	
	pCharData2->GETSKILL_QUICKSLOT(ByteStream);		
	ByteStream.GetBuffer(pBuffer, dwSize);	
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaSkillSlot", dwChaNum, (char *) pBuffer, dwSize );
	pBuffer = NULL;

	// Put on item
	pCharData2->GETPUTONITEMS_BYBUF(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaPutOnItems", dwChaNum, (char *) pBuffer, dwSize);
	pBuffer = NULL;

	// Inventory
	pCharData2->m_cInventory.GETITEM_BYBUFFER(ByteStream);
	ByteStream.GetBuffer( pBuffer, dwSize);		
	if (pBuffer != NULL)
		WriteImage("ChaInfo.ChaInven", dwChaNum, (char *) pBuffer, dwSize);

    // ����� �κ��丮 ����
	if ( pCharData2->m_bServerStorage )
	{
		pCharData2->GETSTORAGE_BYBUF(ByteStream);
		ByteStream.GetBuffer( pBuffer, dwSize);		
		if (pBuffer != NULL)
		{
			WriteUserInven(CCfg::GetInstance()->GetServerGroup(), 
							pCharData2->m_lnStorageMoney, 
							pCharData2->m_dwUserID , 
							(char *) pBuffer, 
							dwSize);
		}
	}
	
	return DB_OK;	
}

// ĳ���� ����
int CDbmanager::DelCharacter(int nUsrNum, int nChaNum, const char* szPass2)
{		
	int		nRetCode = 0;	
	DB_LIST* pDB = GetFreeConnectionUser();
	assert(pDB);
	PDBPROCESS pDBProc = pDB->dbproc;
	
	// 2�� ��й�ȣ üũ
	std::strstream strTemp;
	strTemp << "SELECT UserNum FROM UserInfo WHERE UserNum=";
	strTemp << nUsrNum;
	strTemp << " AND UserPass2='";
	strTemp << szPass2;
	strTemp << "'";
	strTemp << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp.str());

	nRetCode = dbsqlexec(pDBProc);
	if (nRetCode == FAIL)
	{			
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}
	
	nRetCode = dbresults(pDBProc);
	if (nRetCode == NO_MORE_RESULTS || nRetCode == FAIL)
	{		
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	if (dbnextrow(pDBProc) == NO_MORE_ROWS )
	{
		ReleaseConnectionUser(pDB);
		return DB_ERROR;
	}

	ReleaseConnectionUser(pDB);
	
    // ĳ���� ����
	pDB	= GetFreeConnectionGame();
	assert(pDB);
	pDBProc = pDB->dbproc;

	std::strstream strTemp2;
	strTemp2 << "DELETE FROM ChaInfo WHERE ChaNum=";
	strTemp2 << nChaNum;
	strTemp2 << '\0';
	
	dbcmd (pDBProc, (LPCSTR) strTemp2.str());
	
	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc);
	
	if (nRetCode == FAIL)
	{			
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
	else
	{
		ReleaseConnectionGame(pDB);
		return DB_OK;
	}
}

// �ش� ������� ĳ���� �⺻ ������ �����´�.
int CDbmanager::GetChaBAInfo(int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO* ncbi)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 
	
	int	nPos = 0;
	RETCODE nRetCode = 0;
	int nChaNum = 0;	
	
	std::strstream strTemp;
	strTemp << "SELECT ChaNum FROM ChaInfo WHERE UserNum=";
	strTemp << nUsrNum << " AND SGNum=" << nSvrGrp << " ORDER BY ChaNum";
	strTemp << '\0';

	dbcmd (pDBProc, (LPCSTR) strTemp.str());

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{		
		ncbi->nChaSNum = 0;		
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	int nLoop = 0;

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{
		// Bind data
		dbbind(pDBProc, 1,    INTBIND, (DBINT) 0, (BYTE *) &nChaNum);
		while ( dbnextrow(pDBProc) != NO_MORE_ROWS ) // get all rows	 
		{ 
			ncbi->nChaNum[nLoop] = nChaNum;			
			nLoop++;
			if (nLoop == MAX_SERVERCHAR_NUM)
				break;			
		}
		ncbi->nChaSNum = nLoop;
		ReleaseConnectionGame(pDB);
		return DB_OK;
	}
	else
	{
		ncbi->nChaSNum = 0;
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

// �ش� ������� ĳ���� ������ �����´�.
int CDbmanager::GetChaAllInfo(int nUserNum)
{		
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 
	
	int	nPos = 0;
	RETCODE nRetCode = 0;
	int nChaNum = 0;

	std::strstream strTemp;
	strTemp << "SELECT ChaNum FROM ChaInfo WHERE UserNum =" << nUserNum;
	strTemp << '\0';
	
	dbcmd (pDBProc, (LPCSTR) strTemp.str());

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{
		// Bind data
		dbbind(pDBProc, 1,    INTBIND, (DBINT) 0, (BYTE *) &nChaNum);
		while ( dbnextrow(pDBProc) != NO_MORE_ROWS ) // get all rows	 
		{
            GetCharacter(nChaNum);
		}	
		ReleaseConnectionGame(pDB);
		return DB_OK;
	}
	else
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}	
}

// ĳ���� ������ �����´� (���̳ʸ� ���� ����Ÿ)
SCHARDATA* CDbmanager::GetCharacter(int nChaNum)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;	
	SCHARDATA* pChaData = new SCHARDATA;
	
	// ĳ���� ������ �����´�.
	std::strstream strTemp;
	strTemp << "SELECT UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaBright, ChaLevel, ChaDex, ChaIntel, ChaPower, ";
	strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaStRemain, ChaAttackP, ";
	strTemp << "ChaDefenseP, ChaFightA, ChaShootA,  ChaSkillPoint, ChaHP, ";
	strTemp << "ChaMP, ChaSP, ChaPK, ChaStartMap, ChaStartGate, ";
	strTemp << "ChaPosX, ChaPosY, ChaPosZ, CAST(ChaMoney AS CHAR(15)), CAST(ChaExp AS CHAR(15)), ";
    // Add : 2003-09-24
    strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ ";
    ////////////////////////////////////////////////////////////////

	strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << '\0';

	dbcmd(pDBProc, (LPCSTR) strTemp.str());

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{
		delete pChaData;
		ReleaseConnectionGame(pDB);
		return NULL;
	}

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{		
		// dwUserNum ����� ��ȣ
		// dwSvrNum �����׷�
		// Bind data
		dbbind(pDBProc,  1,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwUserID);
		dbbind(pDBProc,  2, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwServerID);
		dbbind(pDBProc,  3,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &pChaData->m_szName);
		dbbind(pDBProc,  4,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_emTribe);
		dbbind(pDBProc,  5, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_emClass);

		dbbind(pDBProc,  6, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_nBright);
		dbbind(pDBProc,  7, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wLevel);		
		dbbind(pDBProc,  8, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wDex); // ��ø
		dbbind(pDBProc,  9, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wInt); // ����
		dbbind(pDBProc, 10, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wPow); // ��

		dbbind(pDBProc, 11, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wStr); // ü��	
		dbbind(pDBProc, 12, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wSpi); // ����
		dbbind(pDBProc, 13, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStats.wSta); // �ٷ�
		dbbind(pDBProc, 14, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wStatsPoint);
		dbbind(pDBProc, 15, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wAP);

		dbbind(pDBProc, 16, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wDP);
		dbbind(pDBProc, 17, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wPA);
		dbbind(pDBProc, 18, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wSA);
		dbbind(pDBProc, 19, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwSkillPoint);		
		dbbind(pDBProc, 20, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sHP.dwData);

		dbbind(pDBProc, 21, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sMP.dwData);
		dbbind(pDBProc, 22, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sSP.dwData);
		dbbind(pDBProc, 23, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_wPK);
		dbbind(pDBProc, 24, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStartMapID.dwID);
		dbbind(pDBProc, 25, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwStartGate);

		dbbind(pDBProc, 26, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.x);

		dbbind(pDBProc, 27, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.y);
		dbbind(pDBProc, 28, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.z);		
		char szMoney[15+1];
		dbbind(pDBProc, 29,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szMoney);		
		char szExp[15+1];
		dbbind(pDBProc, 30,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szExp);	

        // Add : 2003-09-24
        dbbind(pDBProc, 31, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sSaveMapID.dwID);
        dbbind(pDBProc, 32, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.x);
        dbbind(pDBProc, 33, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.y);
        dbbind(pDBProc, 34, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.z);
        //////////////////////////////////////////////////////////////////////////////
		
		dbnextrow(pDBProc); 

		// Money data type convert
		// Character money		
		pChaData->m_lnMoney = _atoi64(szMoney);
		// Character experience		
		pChaData->m_sExperience.lnNow = _atoi64(szExp);
	}
	else
	{
		delete pChaData;
		ReleaseConnectionGame(pDB);
		return NULL;
	}
	ReleaseConnectionGame(pDB);
	return pChaData;	
}

// ĳ���� ������ �����´� (���̳ʸ� ���Ե���Ÿ)
int CDbmanager::GetCharacterInfo(int nChaNum, SCHARDATA2* pChaData2)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;
		
	// ĳ���� ������ �����´�.
	std::strstream strTemp;
	strTemp << "SELECT UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaBright, ChaLevel, ChaDex, ChaIntel, ChaPower, ";
	strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaStRemain, ChaAttackP, ";
	strTemp << "ChaDefenseP, ChaFightA, ChaShootA,  ChaSkillPoint, ChaHP, ";
	strTemp << "ChaMP, ChaSP, ChaPK, ChaStartMap, ChaStartGate, ";
	strTemp << "ChaPosX, ChaPosY, ChaPosZ, CAST(ChaMoney AS CHAR(15)), CAST(ChaExp AS CHAR(15)), ";
    // Add : 2003-09-24    
    strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ ";
    ////////////////////////////////////////////////////////////////
    
    strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << '\0';
	
	nRetCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbcmd %d", nChaNum);
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbsqlexec %d", nChaNum);		
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}	

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{		
		// dwUserNum ����� ��ȣ
		// dwSvrNum �����׷�
		// Bind data
		dbbind(pDBProc,  1,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwUserID);
		dbbind(pDBProc,  2, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwServerID);
		dbbind(pDBProc,  3,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &pChaData2->m_szName);
		dbbind(pDBProc,  4,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_emTribe);
		dbbind(pDBProc,  5, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_emClass);

		dbbind(pDBProc,  6, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_nBright);
		dbbind(pDBProc,  7, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wLevel);		
		dbbind(pDBProc,  8, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wDex); // ��ø
		dbbind(pDBProc,  9, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wInt); // ����
		dbbind(pDBProc, 10, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wPow); // ��

		dbbind(pDBProc, 11, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wStr); // ü��	
		dbbind(pDBProc, 12, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wSpi); // ����
		dbbind(pDBProc, 13, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStats.wSta); // �ٷ�
		dbbind(pDBProc, 14, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wStatsPoint);
		dbbind(pDBProc, 15, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wAP);

		dbbind(pDBProc, 16, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wDP);
		dbbind(pDBProc, 17, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wPA);
		dbbind(pDBProc, 18, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wSA);
		dbbind(pDBProc, 19, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwSkillPoint);		
		dbbind(pDBProc, 20, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sHP.dwData);

		dbbind(pDBProc, 21, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sMP.dwData);
		dbbind(pDBProc, 22, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sSP.dwData);
		dbbind(pDBProc, 23, SMALLBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_wPK);
		dbbind(pDBProc, 24, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sStartMapID.dwID);
		dbbind(pDBProc, 25, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_dwStartGate);

		dbbind(pDBProc, 26, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vStartPos.x);

		dbbind(pDBProc, 27, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vStartPos.y);
		dbbind(pDBProc, 28, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vStartPos.z);		
				
		char szMoney[15+1];
		dbbind(pDBProc, 29,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szMoney);		
		char szExp[15+1];
		dbbind(pDBProc, 30,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szExp);

        // Add : 2003-09-24
        dbbind(pDBProc, 31, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_sSaveMapID.dwID);
        dbbind(pDBProc, 32, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vSavePos.x);
        dbbind(pDBProc, 33, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vSavePos.y);
        dbbind(pDBProc, 34, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData2->m_vSavePos.z);
        //////////////////////////////////////////////////////////////////////////////
		
		dbnextrow(pDBProc); 
		ReleaseConnectionGame(pDB);	

		// Money data type convert
		// Character money		
		pChaData2->m_lnMoney = _atoi64(szMoney);
		// Character experience		
		pChaData2->m_sExperience.lnNow = _atoi64(szExp);

		// ��ų		
		CByteStream ByteStream;
		
		nRetCode = ReadImage("ChaInfo.ChaSkills", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		pChaData2->SETEXPSKILLS_BYBUF(ByteStream);

		// Skill Quick Slot
		nRetCode = ReadImage("ChaInfo.ChaSkillSlot", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		pChaData2->SETSKILL_QUICKSLOT(ByteStream);
		
		// ���������
		nRetCode = ReadImage("ChaInfo.ChaPutOnItems", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		SETPUTONITEMS_BYBUF(pChaData2->m_PutOnItems,ByteStream);

		// ĳ���� �κ��丮
		nRetCode = ReadImage("ChaInfo.ChaInven", nChaNum, ByteStream);
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		pChaData2->m_cInventory.SETITEM_BYBUFFER(ByteStream);
		
		return DB_OK;
	}
	else
	{	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbresults %d", nChaNum);				
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

int CDbmanager::GetCharacterInfo(int nChaNum, GLCHARAG_DATA* pChaData)
{
	assert(pChaData&&"(GLCHARAG_DATA*)�� ���� ��ȿ���� �ʽ��ϴ�.");

	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;
	RETCODE nRetCode = 0;
		
	// ĳ���� ������ �����´�.
	std::strstream strTemp;
	strTemp << "SELECT UserNum, SGNum, ChaName, ChaClass, ChaStartMap, ChaStartGate, ";
	strTemp << "ChaPosX, ChaPosY, ChaPosZ, ";
    // Add : 2003-09-24
    strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ ";    
    //////////////////////////////////////////////////////////////////////////////
	strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << std::ends;
	
	nRetCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbcmd %d", nChaNum);
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc); 
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbsqlexec %d", nChaNum);		
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}	

	nRetCode = dbresults(pDBProc);
	if ((nRetCode != NO_MORE_RESULTS) && (nRetCode != FAIL) )
	{
		// dwUserNum ����� ��ȣ
		// dwSvrNum �����׷�
		// Bind data
		dbbind(pDBProc,  1,	INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwUserID);
		dbbind(pDBProc,  2, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwServerID);
		dbbind(pDBProc,  3,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &pChaData->m_szName);
		dbbind(pDBProc,  4, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_emClass);

		dbbind(pDBProc,  5, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sStartMapID.dwID);
		dbbind(pDBProc,  6, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_dwStartGate);

		dbbind(pDBProc,  7, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.x);
		dbbind(pDBProc,  8, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.y);
		dbbind(pDBProc,  9, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vStartPos.z);

        // Add : 2003-09-24
        dbbind(pDBProc, 10, INTBIND,	(DBINT) 0, (LPBYTE) &pChaData->m_sSaveMapID.dwID);
        dbbind(pDBProc, 11, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.x);
        dbbind(pDBProc, 12, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.y);
        dbbind(pDBProc, 13, FLT4BIND,	(DBINT) 0, (LPBYTE) &pChaData->m_vSavePos.z);
        //////////////////////////////////////////////////////////////////////////////
		
		dbnextrow(pDBProc); 
		ReleaseConnectionGame(pDB);	

		return DB_OK;
	}
	else
	{	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : GetCharacterInfo dbresults %d", nChaNum);				
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
}

int CDbmanager::GetChaBInfo(int nChaNum, SCHARINFO_LOBBY* sci)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 
	
	std::strstream strTemp;
	strTemp << "SELECT ChaName, ChaClass, ChaDex, ChaIntel, ChaPower,";
	strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaLevel FROM ChaInfo ";
	strTemp << "WHERE ChaNum=" << nChaNum;
	strTemp << '\0';
	
	dbcmd (pDBProc, (LPCSTR) strTemp.str());
	// send command buffer to SQL server
	nRetCode = dbsqlexec (pDBProc);
	if (nRetCode == FAIL)
	{
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
	nRetCode = dbresults(pDBProc);
	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL)
	{
		// Bind data
		sci->m_dwCharID = nChaNum;
		dbbind(pDBProc,  1,	NTBSTRINGBIND, (DBINT) CHR_ID_LENGTH, (LPBYTE) &sci->m_szName);
		dbbind(pDBProc,  2,	INTBIND,	(DBINT) 0, (LPBYTE) &sci->m_emClass);
		dbbind(pDBProc,  3, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wDex);
		dbbind(pDBProc,  4, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wInt);
		dbbind(pDBProc,  5, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wPow);
		dbbind(pDBProc,  6, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wStr);
		dbbind(pDBProc,  7, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wSpi);
		dbbind(pDBProc,  8, SMALLBIND,	(DBINT) 0, (LPBYTE) &sci->m_sStats.wSta);
		dbbind(pDBProc,  9, INTBIND,	(DBINT) 0, (LPBYTE) &sci->m_wLevel);
		
		dbnextrow(pDBProc);
		ReleaseConnectionGame(pDB);

		// ��������� ����
		CByteStream ByteStream;
		nRetCode = ReadImage ( "ChaInfo.ChaPutOnItems", nChaNum, ByteStream );
		if (nRetCode == DB_ERROR)
			return DB_ERROR;
		SETPUTONITEMS_BYBUF ( sci->m_PutOnItems,ByteStream );
	
		return DB_OK;
	}
	else
	{
		return DB_ERROR;
	}	
}

// table �� �÷��� image, text, ntext ���϶� 
// ���ڵ�ִ� ������ �ݵ�� ��� �����Ͷ� �־��־�� �Ѵ�.
// null �� ��쿡 timestamp, txtprt �� ���� ���ؼ� ������ ���Եȴ�.
// objName ���� ������ �÷��� pData �� nSize ��ŭ �ְԵȴ�.
// objName : tablename.columnname
int	CDbmanager::WriteImage(const char* objName, int nChaNum, char* pData, int nSize)
{
	DB_LIST* pDBSelect = GetFreeConnectionGame();
	ASSERT(pDBSelect);
	PDBPROCESS pDBSelectProc = pDBSelect->dbproc; 
	
	RETCODE nRetCode = 0;
	int nTemp = 0;
	
	std::strstream strTemp;
	strTemp << "SELECT " << objName << " FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	strTemp << '\0';

	nRetCode = dbcmd(pDBSelectProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbcmd %d", nChaNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
		
	nRetCode = dbsqlexec(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbsqlexec %d", nChaNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}

	nRetCode = dbresults(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbresults %d", nChaNum);						
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
	
	// dbbind(pDBSelectProc, 1, INTBIND, (DBINT) 0, (BYTE *) &nTemp);
	
	DB_LIST* pDBUpdate = GetFreeConnectionGame();
	PDBPROCESS pDBUpdateProc = pDBUpdate->dbproc;

	while (dbnextrow(pDBSelectProc) != NO_MORE_ROWS)
	{
		nRetCode = dbupdatetext(pDBUpdateProc, // pdbproc
								objName, // dest_object
								dbtxptr(pDBSelectProc, 1), // dest_textptr
								dbtxtimestamp(pDBSelectProc, 1), // dest_timestamp
								UT_TEXT, // update_type
								0, // 0, -1 insert_offset
								-1, // 0, -1 delete_length
								NULL, // src_object
								nSize, // src_size
								(LPCBYTE) pData); // src_text

		if (nRetCode == FAIL) 
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : WriteImage dbupdatetext %d", nChaNum);								
			ReleaseConnectionGame(pDBUpdate);
			ReleaseConnectionGame(pDBSelect);
			return DB_ERROR;
		}
	}
	ReleaseConnectionGame(pDBUpdate);
	ReleaseConnectionGame(pDBSelect);

	return DB_OK;
}

// objName �ش� �÷��� �̹��������͸� �о��.
// objName : tablename.columnname
int	CDbmanager::ReadImage ( const char* objName, int nChaNum, CByteStream &ByteStream )
{	
	ByteStream.ClearBuffer ();
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 

	DBINT nInt = 0;
	int nTotalByte = 0;
	RETCODE nRetCode = 0;	
		
	std::strstream strTemp;
	strTemp << "SELECT " << objName << " FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : ReadImage dbcmd %d", nChaNum);		
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : ReadImage dbsqlexec %d", nChaNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}
	if (dbresults(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : ReadImage dbresults %d", nChaNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}	
	char pBuffer[DB_IMAGE_BUF_SIZE];
	while (1)
	{
		nInt = dbreadtext(pDBProc, pBuffer, DB_IMAGE_BUF_SIZE);
		if (nInt == NO_MORE_ROWS || nInt == 0 )	
			break;
		nTotalByte += nInt;
		ByteStream.WriteBuffer ( (LPBYTE)pBuffer, nInt );		
	}
	ReleaseConnectionGame(pDB);

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] ���� �̹��� ����� �۴ٸ� ���۸� ����.
	if ( nTotalByte < 12 )		ByteStream.ClearBuffer ();

	return nTotalByte;
}

// ����� �κ��丮�� ����Ÿ�� �����Ѵ�.
int	CDbmanager::WriteUserInven(int SGNum, // �����׷� ��ȣ
							   LONGLONG llMoney, // LONGLONG __int64, ������ ���� ��
							   DWORD dwUserNum, // ����ڹ�ȣ
							   char* pData, // ����Ÿ������
							   int nSize) // ������

{
	// User Inventory
	// 1. Check User Inven		
	// 2. If exist skip
	// 3. not exist, write iventory image
	BOOL bInven = CheckInven(SGNum, dwUserNum);
	if (!bInven)
	{
		MakeUserInven(SGNum, dwUserNum);
	}

	// Update Money
	DB_LIST* pDBMoney = GetFreeConnectionGame();
	ASSERT(pDBMoney);
	PDBPROCESS pDBMoneyProc = pDBMoney->dbproc; 

	std::strstream strMoney;
	strMoney << "UPDATE UserInven SET UserMoney=" << llMoney ;
	strMoney << "WHERE (UserNum=" <<  dwUserNum << ") AND ";
	strMoney << "(SGNum=" << SGNum << ")";
	strMoney << '\0';

	nRetCode = dbcmd(pDBMoneyProc, (LPCSTR) strMoney.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbcmd %d", dwUserNum);				
		ReleaseConnectionGame(pDBMoney);
		return DB_ERROR;
	}
		
	nRetCode = dbsqlexec(pDBMoneyProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbsqlexec %d", dwUserNum);				
		ReleaseConnectionGame(pDBMoney);
		return DB_ERROR;
	}

	ReleaseConnectionGame(pDBMoney);
	

	// Update Image
	DB_LIST* pDBSelect = GetFreeConnectionGame();
	ASSERT(pDBSelect);
	PDBPROCESS pDBSelectProc = pDBSelect->dbproc; 
	
	RETCODE nRetCode = 0;
	int nTemp = 0;
	
	std::strstream strTemp;
	strTemp << "SELECT UserInven.UserInven FROM UserInven WHERE (UserNum=" <<  dwUserNum << ") AND ";
	strTemp << "(SGNum=" << SGNum << ")";
	strTemp << '\0';

	nRetCode = dbcmd(pDBSelectProc, (LPCSTR) strTemp.str());
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbcmd %d", dwUserNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
		
	nRetCode = dbsqlexec(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbsqlexec %d", dwUserNum);				
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}

	nRetCode = dbresults(pDBSelectProc);
	if (nRetCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbresults %d", dwUserNum);						
		ReleaseConnectionGame(pDBSelect);
		return DB_ERROR;
	}
	
	// dbbind(pDBSelectProc, 1, INTBIND, (DBINT) 0, (BYTE *) &nTemp);
	
	DB_LIST* pDBUpdate = GetFreeConnectionGame();
	PDBPROCESS pDBUpdateProc = pDBUpdate->dbproc;

	while (dbnextrow(pDBSelectProc) != NO_MORE_ROWS)
	{
		nRetCode = dbupdatetext(pDBUpdateProc, // pdbproc
								"UserInven.UserInven", // dest_object
								dbtxptr(pDBSelectProc, 1), // dest_textptr
								dbtxtimestamp(pDBSelectProc, 1), // dest_timestamp
								UT_TEXT, // update_type
								0, // 0, -1 insert_offset
								-1, // 0, -1 delete_length
								NULL, // src_object
								nSize, // src_size
								(LPCBYTE) pData); // src_text
			
		if (nRetCode == FAIL) 
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:WriteUserInven dbupdatetext %d", dwUserNum);
			ReleaseConnectionGame(pDBUpdate);
			ReleaseConnectionGame(pDBSelect);
			return DB_ERROR;
		}
	}
	ReleaseConnectionGame(pDBUpdate);
	ReleaseConnectionGame(pDBSelect);

	return DB_OK;
}

int	CDbmanager::ReadUserInven(SCHARDATA2* pChaData2)
{
	BOOL bInven = CheckInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->m_dwUserID);
	if (!bInven)
	{
		MakeUserInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->m_dwUserID);
	}

	// �����κ��丮, money
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 

	RETCODE nReturnCode = 0;
	int nUserNum = 0;
	
	// ĳ���� ������ �����´�.
	std::strstream strTemp;
	strTemp << "SELECT CAST(UserMoney AS CHAR(15)) FROM UserInven WHERE UserNum=" << pChaData2->m_dwUserID << " AND ";
	strTemp << "SGNum=" << CCfg::GetInstance()->GetServerGroup() ;
	strTemp << '\0';
	
	nReturnCode = dbcmd(pDBProc, (LPCSTR) strTemp.str());
	if (nReturnCode == FAIL)
	{
        CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbcmd");
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}
	// send command buffer to SQL server
	nReturnCode = dbsqlexec (pDBProc); 
	if (nReturnCode == FAIL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbresults");
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}

	nReturnCode = dbresults(pDBProc);
	if (nReturnCode != NO_MORE_RESULTS && nReturnCode != FAIL)
	{
		// Bind data
		char szMoney[15+1];
		dbbind(pDBProc, 1,	NTBSTRINGBIND, (DBINT) 0, (LPBYTE) szMoney);
		dbnextrow(pDBProc); 

		// Money data type convert
		// Character money		
		pChaData2->m_lnStorageMoney = _atoi64(szMoney);
		ReleaseConnectionGame(pDB);
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbresults");
		ReleaseConnectionGame(pDB);
		return DB_ERROR;
	}	

	// image
	CByteStream ByteStream;
	nRetCode = ReadUserInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->m_dwUserID, ByteStream);
	if (nRetCode == DB_ERROR)
		return DB_ERROR;

	pChaData2->SETSTORAGE_BYBUF(ByteStream);
	return DB_OK;
}

// ����� �κ��丮���� ����Ÿ�� �о�´�.
int	CDbmanager::ReadUserInven(int SGNum, DWORD dwUserNum, CByteStream &ByteStream)
{	
	ByteStream.ClearBuffer ();
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc; 

	DBINT nInt = 0;
	int nTotalByte = 0;
	RETCODE nRetCode = 0;	
		
	std::strstream strTemp;
	strTemp << "SELECT UserInven.UserInven FROM UserInven where (UserNum=" <<  dwUserNum << ")";	
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbcmd %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbsqlexec %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}
	if (dbresults(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:ReadUserInven dbresults %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}
	char pBuffer[DB_IMAGE_BUF_SIZE];
	while (1)
	{
		nInt = dbreadtext(pDBProc, pBuffer, DB_IMAGE_BUF_SIZE);
		if (nInt == NO_MORE_ROWS || nInt == 0 )	
			break;
		nTotalByte += nInt;
		ByteStream.WriteBuffer ( (LPBYTE)pBuffer, nInt );		
	}
	ReleaseConnectionGame(pDB);	
	if ( nTotalByte < 12 )		
		ByteStream.ClearBuffer ();
	return nTotalByte;
	/*
	LPBYTE pData;
	int nBytes=0;

	while (dbnextrow(pDBProc) != NO_MORE_ROWS)
	{		
		nBytes = dbdatlen(pDBProc, 1);
		if (nBytes == 0)
			break;
		nTotalByte += nBytes;
		pData = ((BYTE*) dbdata(pDBProc, 1));
		ByteStream.WriteBuffer((LPBYTE) pData, nBytes);
	}
	ReleaseConnectionGame(pDB);
	if ( nTotalByte < 12 )		
		ByteStream.ClearBuffer ();

    return nTotalByte;
	*/
}

// �����κ��丮�� �ִ��� üũ�Ѵ�.
// ���ٸ� ���Ӱ� �����Ѵ�.
bool CDbmanager::CheckInven(int SGNum, DWORD dwUserNum)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;

	RETCODE nRetCode = 0;
	int		nInvenNum = 0;

	std::strstream strTemp;
	strTemp << "SELECT count(*) FROM UserInven where (UserNum=" <<  dwUserNum << ") AND ";
	strTemp << "(SGNum=" << SGNum << ")";
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CheckInven dbcmd %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return false;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CheckInven dbsqlexec %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return false;
	}

	nRetCode = dbresults(pDBProc);
	if (nRetCode != NO_MORE_RESULTS && nRetCode != FAIL)
	{
		// Bind data
		dbbind(pDBProc, 1, INTBIND, (DBINT)0, (BYTE *) &nInvenNum);	
		dbnextrow(pDBProc);
	}

	ReleaseConnectionGame(pDB);

	if (nInvenNum > 0)
	{
		return true;
	}
	else // make new inven
	{
		return false;
	}
}

int	CDbmanager::MakeUserInven(int SGNum, DWORD dwUserNum)
{
	DB_LIST* pDB = GetFreeConnectionGame();
	PDBPROCESS pDBProc = pDB->dbproc;

	RETCODE nRetCode = 0;
	int		nInvenNum = 0;

	std::strstream strTemp;
	strTemp << "INSERT INTO UserInven (SGNum, UserNum, UserMoney, UserInven) VALUES( ";
	strTemp << SGNum << ",";
	strTemp << dwUserNum << ",";
	strTemp << "0,";
	strTemp << "'')";
	strTemp << '\0';

	if (dbcmd(pDBProc, (LPCSTR) strTemp.str()) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:MakeUserInven dbcmd %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}
	if (dbsqlexec(pDBProc) == FAIL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:MakeUserInven dbsqlexec %d", dwUserNum);
		ReleaseConnectionGame(pDB);	
		return DB_ERROR;
	}

	ReleaseConnectionGame(pDB);
	return DB_OK;
}
