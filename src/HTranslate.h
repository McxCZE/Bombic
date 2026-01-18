// HTranslate.h: interface for the HTranslate class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HTRANSLATE_H_
#define HTRANSLATE_H_


// jazykove defines

#define _LNG_BOMBIC_                 lng.Translate(0)

#define _LNG_MAIN_STORY_             lng.Translate(1)
#define _LNG_MAIN_DEAD_              lng.Translate(2)
#define _LNG_MAIN_EXIT_              lng.Translate(3)
#define _LNG_MAIN_MRCHOVNIK_         lng.Translate(4)
#define _LNG_MAIN_SETTING_           lng.Translate(5)
#define _LNG_MAIN_HELP_              lng.Translate(6)
#define _LNG_MAIN_CREDITS_           lng.Translate(7)

#define _LNG_SINGLE_START_           lng.Translate(10)
#define _LNG_SINGLE_PLAYERS_         lng.Translate(11)
#define _LNG_SINGLE_CODE_            lng.Translate(12)
#define _LNG_SINGLE_BACK_            lng.Translate(13)

#define _LNG_DEADMATCH_START_        lng.Translate(20)
#define _LNG_DEADMATCH_PLAYERS_      lng.Translate(21)
#define _LNG_DEADMATCH_MONSTERS_     lng.Translate(22)
#define _LNG_DEADMATCH_VICTORIES_    lng.Translate(23)
#define _LNG_DEADMATCH_BONUSLEVEL_   lng.Translate(24)
#define _LNG_DEADMATCH_BACK_         lng.Translate(25)

#define _LNG_PAUSED_                 lng.Translate(40)

#define MAX_TEXTS 500
#define MAX_LANGS 5


#include <string>

class HTranslate  
{
public:
	bool LoadLanguage(int ID);
	std::string Translate(int ID);
	bool    LoadLanguage(const char *filename);
	HTranslate();
	virtual ~HTranslate();

	int      m_langs; 

private:
	int m_loaded; 
	struct data {
		std::string s;
		int  ID;
	} m_data[MAX_TEXTS];

	std::string  m_lang[5];
};

extern HTranslate lng;

#endif // HTRANSLATE_H_
