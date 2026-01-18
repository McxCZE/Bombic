// GMrchaAI0.h: interface for the GMrchaAI0 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI0_H_
#define GMRCHAAI0_H_


class GMrcha;
class GGame;
class GMap;

class GMrchaAI0  
{
public:
	void CenterMove();
	void FakeMove();
	GMrchaAI0(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI0();

	virtual void AIMove();

protected:
	bool NeedChange();
	bool ChangeDir();

	GMrcha *m_mrcha;
	GGame  *m_game;
	GMap   *m_map;

	int m_x; // kopie od mrchy
	int m_y; 
	int m_mx; // kopie od mrchy
	int m_my; 

	int m_fx;
	int m_fy;

	int m_tochange; 
};

#endif // GMRCHAAI0_H_
