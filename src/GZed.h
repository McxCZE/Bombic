// GZed.h: interface for the GZed class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GZED_H_
#define GZED_H_


#include "GObject.h"
#include "data.h"

class GZed : public GObject
{
public:
	bool FireIn() const;
	virtual void Move();
	virtual bool Hit();
	void Draw(int bx, int by);
	void Init(GGame *game, int x, int y, int bBmp, int type);
	bool WalkIn() const;
	GZed();
	virtual ~GZed();

	bool m_destroiig;

	st_zed m_params;
};

#endif // GZED_H_
