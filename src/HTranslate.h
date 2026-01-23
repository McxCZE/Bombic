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

#define _LNG_MAIN_LAN_               lng.Translate(8)

#define _LNG_LAN_HOST_               lng.Translate(80)
#define _LNG_LAN_JOIN_               lng.Translate(81)
#define _LNG_LAN_BACK_               lng.Translate(82)
#define _LNG_LAN_WAITING_            lng.Translate(83)
#define _LNG_LAN_CONNECTED_          lng.Translate(84)
#define _LNG_LAN_START_              lng.Translate(85)
#define _LNG_LAN_IP_                 lng.Translate(86)
#define _LNG_LAN_CONNECTING_         lng.Translate(87)
#define _LNG_LAN_MAP_                lng.Translate(88)
#define _LNG_LAN_READY_              lng.Translate(89)
#define _LNG_LAN_NOT_READY_          lng.Translate(90)
#define _LNG_LAN_COOP_               lng.Translate(91)
#define _LNG_LAN_SELECT_LEVEL_       lng.Translate(92)
#define _LNG_LAN_LEVEL_OF_           lng.Translate(93)
#define _LNG_LAN_WAITING_HOST_       lng.Translate(94)
#define _LNG_LAN_DEATHMATCH_         lng.Translate(95)

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
