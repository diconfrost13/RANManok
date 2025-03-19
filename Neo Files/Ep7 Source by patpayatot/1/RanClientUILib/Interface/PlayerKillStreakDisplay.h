#pragma	once

#include "../EngineUIlib/GUInterface/UIGroup.h"

class	CPlayerKillType;
class	CPlayerKillStreakDisplay : public CUIGroup
{
public:
	enum
	{
		FIRST_BLOOD = NO_ID + 1,
		DOUBLE_KILL,
		TRIPLE_KILL,
		ULTRA_KILL,
		RAMPAGE,
		KILLING_SPREE,
		DOMINATING,
		MEGA_KILL,
		UNSTOPPABLE,
		WICKED_SICK,
		MONSTER_KILL,
		GODLIKE,
		HOLY_SHIT,
		OWNAGE,

		MAXTYPE = 14
	};

public:
	CPlayerKillStreakDisplay ();
	virtual	~CPlayerKillStreakDisplay ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	bool	START ( UIGUID cID );
	bool	RESET ( UIGUID cID );
	bool	STOP ( UIGUID cID );

	void	ALLSTOP ();

public:
	bool	KEEP_START ( UIGUID cID );
	void	KEEP_STOP ();

private:
	CPlayerKillType*	m_pPK_TYPE[MAXTYPE];
	CUIControl*			m_PK_TYPE_KEEP[MAXTYPE];

	CUIControl*		m_pPositionControl;	
};