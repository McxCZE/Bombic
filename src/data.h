#ifndef _DATA_H
#define _DATA_H

// velikost mapy
#define MAX_X 50
#define MAX_Y 50


typedef struct {
	char name[30];     // jmeno
	float  speed; // rychlost pohybu
	int  anims; // pocet animaci
	int  animspeed; // rychlost animace
	int  lives;   // pocet zivotu
	int  score;   // pocet bodu
	int  b_x;     // rozmery v bitmape
	int  b_y;     // rozmery v bitmape
	bool fly;     // zda leta (houpe se ve vzduchu)
	bool sizing;  // zda se roztahuje (slizka)
	int  alpha;   // jakou polopruhlednosti se ma kreslit
	int  dy;      // posun v y;
	int  danims;  // pocet animacnich oplicek pri zabiti
	int  ai;      // verze AI
} st_mrcha;

typedef struct {
	char name[30];     // jmeno
	int  sx;           // obrazek x
	int  sy;           // obrazek y
	bool walk;         // pruchodnost
	int  anims;        // pocet animacnich oken // jsou polozena v rade
	int  anim_speed;   // rychlost animace
	int  animbum;
	int  animsbum_speed;
	bool destructable; // zda je znicitelna
	bool firein;       // zda je zed prustrelna (pruletna)
} st_zed;

typedef struct {
	float speed;
	int anims;
	int animspeed;
	int b_x;
	int b_y;
	int dy;
} st_bomber;

typedef struct {
	int  map[MAX_X][MAX_Y];  // policka v mape
	int  podklad;            // na jakem podkladu se hraje
	bool deadmach;           // zda jde o deadmatchovou mapu (nebo normalni)
	int  x;                  // rozmery mapy
	int  y;
	int  mapmrch[MAX_X][MAX_Y];  // mape mrch
} st_map;

#define MAX_ZED 80
extern st_zed P_zed[MAX_ZED];

#define MAX_MRCH 15
extern st_mrcha P_mrcha[MAX_MRCH];

extern st_bomber P_bomber;

#endif

