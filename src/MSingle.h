// MSingle.h: interface for the MSingle class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MSINGLE_H_
#define MSINGLE_H_


#include <string>
#include "GBase.h"

#define MAX_SINGLE_SEL 4

class MSingle : public GBase  
{
public:
	int OnKey(int nChar);
	void Draw();
	virtual void Destroy();
	virtual void Init(CMainFrame *parent);
	MSingle();
	virtual ~MSingle();

	int      m_bPozadi;
	int      m_players;
	int      m_bDeadBMP;
	int      m_bDead;
	int      m_sel;
	std::string  m_code;
};

#endif // MSINGLE_H_
