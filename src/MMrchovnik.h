// MMrchovnik.h: interface for the MMrchovnik class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MMRCHOVNIK_H_
#define MMRCHOVNIK_H_


#include "GBase.h"

class MMrchovnik : public GBase  
{
public:
	void LoadMrcha();
	virtual int OnKey(int nChar);
	void Init(CMainFrame *parent);
	virtual void Draw();
	virtual void Destroy();
	MMrchovnik();
	virtual ~MMrchovnik();

	int      m_bMrcha;
	int      m_sel;
	int      m_max_sel;

};

#endif // MMRCHOVNIK_H_
