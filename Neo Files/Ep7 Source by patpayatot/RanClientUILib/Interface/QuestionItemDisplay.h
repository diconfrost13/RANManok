#pragma	once

#include "../EngineUIlib/GUInterface/UIGroup.h"

class	CQuestionItemType;
class	CQuestionItemDisplay : public CUIGroup
{
public:
	enum
	{
		SPEED_UP = NO_ID + 1,
		CRAZY_TIME,
		POWER_UP,
		EXP_TIME,

		EXP_GET,
		LUCKY,
		BOMB,
		OH_NO,

		SPEED_UP_M,
		MADNESS,		
		ATTACK_UP_M,
		HEAL,	

		FIRST_BLOOD,
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

		MAXTYPE = 26
	};

public:
	CQuestionItemDisplay ();
	virtual	~CQuestionItemDisplay ();

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
	CQuestionItemType*	m_pQI_TYPE[MAXTYPE];
	CUIControl*			m_QI_TYPE_KEEP[MAXTYPE];

	CUIControl*		m_pPositionControl;	
};