////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	CSoundSourceManagerDoc
//	����	:	�������� ����Ÿ(wav, �ڸ�Ʈ...) ������ ��
//	�ΰ�����:
//	������:	Search, Backup, ���� ����, ���� �̸��� ���� ���� �ߺ� üũ
//				Category �̿�.
//	�ۼ���	:	���⿱
//	eMail	:	kysung@mincoms.co.kr
//	WWW		:	http://www.ran-online.co.kr
//	
//	�α�
//	$6.	2002-11-28T18:57	[�⺻ ���丮.. Sounds�� ����]
//	$5.	2002-10-31T09:30	[SoundSourceMan Ŭ������ Doc�� ���� ���� ����]
//	$4.	2002-10-24T17:36	[����Ÿ Locking ����, ���� ���� ����]
//	$3.	2002-10-24T11:03	[SSoundSource����ü��,
//							�ڵ��̸����� �ٲ�� ���� ���� �̸� ���� �ʵ����]
//							[Lock �ʵ� ���� ( �����Ҽ� ������ ����� �ʵ� )]
//							[���� �׽�Ʈ ��� ����]
//							[���� �̸� ���� ����]
//	$2.	2002-10-23T16:43	[���� ���� �߰�]
//	$1.	2002-10-22T21:07	[���� �ۼ�, ���̺� �ε�]
//							[���� ���� �ý���]
//							[Primary Key ���� ��� �ۼ�]
//
////////////////////////////////////////////////////////////////////////////////

// SoundSourceManagerDoc.h : CSoundSourceManagerDoc Ŭ������ �������̽�
//
#pragma once

class CSoundSourceManagerDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CSoundSourceManagerDoc();
	DECLARE_DYNCREATE(CSoundSourceManagerDoc)

//	<--	���� ���� �ý���	-->	//
private:
	BOOL	ExtraSaveFile ();
	BOOL	ExtraLoadFile ();
	BOOL	ProgramNormalExit();

//	<--	���̺�/�ε�	-->	//
public:
	BOOL	DefaultSaveFile ();
	BOOL	DefaultLoadFile ();

// ������
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CSoundSourceManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()

};