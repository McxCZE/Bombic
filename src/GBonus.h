// GBonus.h: interface for the GBonus class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_H_
#define GBONUS_H_


#include "GBomber.h"
#include "GMap.h"

#define TIME_BONUS_ILL    500
#define TIME_BONUS_EXTRA 1000

class GBonus  
{
public:
	virtual void AfterPut(int bombID);
	virtual bool HitProtect();
	virtual void End();
	virtual GBonus* GetCopy();
	virtual void BeforeInput(bool *left, bool *right, bool *up, bool *down, bool *action);
	virtual void OneTime();
	virtual void Picked(GBomber *bomber, GBonus **self);
	virtual void Move();
	virtual void DrawOnPlayer(int x, int y);
	virtual void DrawOnGround(int x, int y);
	virtual void BeforeMove(float *speed);
	virtual void Init(int mx, int my, GBonus **m_self, int bBonusy);
	GBonus();
	virtual ~GBonus();

	GBonus **m_self;
	int m_my;
	int m_mx;
	int m_type;
	int m_timer_MAX; // puvodni nastaveni kolik casu
	int m_timer;  // odpocitava se
	bool m_onetime;
	bool m_illness;
	bool m_shouldDelete;  // Flag for deferred deletion
	int m_shx;
	int m_shy;
	int m_bBmp;
	int m_bView;
	GBomber *m_bomber;
	GMap    *m_map;
};

#endif // GBONUS_H_
