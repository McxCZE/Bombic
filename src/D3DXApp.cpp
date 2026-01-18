// D3DXApp.cpp : Defines the class behaviors for the application.
//

#include "config.h"
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <iostream>

#include "stdafx.h"
#include "D3DXApp.h"
#include "Config.h"

#include "MainFrm.h"
#include "GFile.h"
#include "Version.h"

#include <SDL_mixer.h>

using namespace std;

// Global settings - linked to GameConfig
float gspeed = 1.5f;
int   gmaxmrchovnik = 0;
bool  gdeadmatch_bombkill = true;
bool  gexplode_predict_visible = true;
int   glangfile = 1;
bool  gsoundavailable = true;
bool  gfullscreen = false;

// Command line options
std::string olevelfile = "";
bool   odeadmath = false;
int    oplayers = 1;

HDSoundBuffer g_sb[25];

bool load_samples();

// Config file path
static std::string g_configPath;

// Load settings from config file to global variables
static void LoadSettingsFromConfig()
{
    gspeed = g_config.speed;
    gmaxmrchovnik = g_config.maxMrchovnik;
    gdeadmatch_bombkill = g_config.deadmatchBombkill;
    gexplode_predict_visible = g_config.explodePredictVisible;
    glangfile = g_config.langFile;
    gsoundavailable = g_config.soundAvailable;
    gfullscreen = g_config.fullscreen;
}

// Save global variables to config file
static void SaveSettingsToConfig()
{
    g_config.speed = gspeed;
    g_config.maxMrchovnik = gmaxmrchovnik;
    g_config.deadmatchBombkill = gdeadmatch_bombkill;
    g_config.explodePredictVisible = gexplode_predict_visible;
    g_config.langFile = glangfile;
    g_config.soundAvailable = gsoundavailable;
    g_config.fullscreen = gfullscreen;
    g_config.Save(g_configPath);
}

/*
CD3DXAppApp::CD3DXAppApp()
{

	CFile file;
	if (file.Open(INI_FILE, CFile::modeRead|CFile::typeBinary)) {
		file.Read(&gspeed, sizeof(float));
		file.Read(&gmaxmrchovnik, sizeof(int));
		file.Read(&gdeadmatch_bombkill, sizeof(bool));
		file.Read(&gexplode_predict_visible, sizeof(bool));
		file.Read(&glangfile, sizeof(int));
		file.Read(&m_fullscreen, sizeof(bool));
		file.Read(&gsoundavailable, sizeof(bool));
		file.Close();
	}
	
}

CD3DXAppApp::~CD3DXAppApp()
{
	CFile file;
	
	if (file.Open(INI_FILE, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) {
		file.Write(&gspeed, sizeof(float));
		file.Write(&gmaxmrchovnik, sizeof(int));
		file.Write(&gdeadmatch_bombkill, sizeof(bool));
		file.Write(&gexplode_predict_visible, sizeof(bool));
		file.Write(&glangfile, sizeof(int));
		file.Write(&m_fullscreen, sizeof(bool));
		file.Write(&gsoundavailable, sizeof(bool));
		file.Close();
	}
	
}
*/
/////////////////////////////////////////////////////////////////////////////
// The one and only CD3DXAppApp object

void printhelp()
{
	cout << BOMBIC_FULL_VERSION << endl
	     << "Build: " << BOMBIC_BUILD_DATE << " " << BOMBIC_BUILD_TIME << endl
	     << endl
	     << "Usage:" << endl
	     << "  -h, --help                    this help" << endl
	     << "  -p X, --players X             number of players (1-4)" << endl
	     << "  -f, --fullscreen              fullscreen mode" << endl
	     << "  -m [map.ble], --map [map.ble] load specific map" << endl
	     << "  -d, --deathmatch              deathmatch mode" << endl
	     << endl
	     << "Config file: " << GameConfig::GetDefaultPath() << endl;

	exit(0);
}

void processargs(int argc, char* argv[])
{
	static struct option long_options[] = {
		{"fullscreen", 0, 0, 'f'},
		{"map", 1, 0, 'm'},
		{"players", 1, 0, 'p'},
		{"deathmatch", 0, 0, 'd'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};

	int option_index = 0;

	while(1) {
		char c = getopt_long (argc, argv, "fm:p:dh",long_options, &option_index);
		if (c == -1) break;
		
		switch (c) {
		case 'f':
			gfullscreen = true;
			break;
		case 'd':
			odeadmath = true;
			
			break;
		case 'p':
			oplayers = atoi(optarg);
			break;
		case 'm' :
			olevelfile = optarg;
			break;
		case 'h' :
			printhelp();
			break;
		}
	}
	
	cout << "Players: " << oplayers << endl;
	cout << "Map: " << olevelfile << endl;
	cout << "Fullscreen: " << gfullscreen << endl;
	cout << "DeathMatch: " << odeadmath << endl;
}


//CD3DXAppApp theApp;
HD3DX d3dx;

/////////////////////////////////////////////////////////////////////////////
// CD3DXAppApp initialization

int EventLoop(CMainFrame* pFrame)
{
	SDL_Event event;
	while ( SDL_PollEvent(&event) ) {
		switch (event.type) {
		case SDL_QUIT :
			return 0;
			break;

		case SDL_WINDOWEVENT :
			if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
			    event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				d3dx.HandleResize();
			}
			break;

		case SDL_KEYDOWN :
			if (pFrame)
				if (!pFrame->OnKeyDown(event.key.keysym.sym))
				return 0;
			break;
		}
	}
	return 1;
}


int main(int argc, char* argv[])
{
	srand( (unsigned)time( nullptr ) );

	// Load configuration
	g_configPath = GameConfig::GetDefaultPath();
	g_config.Load(g_configPath);
	LoadSettingsFromConfig();

	processargs(argc, argv);

	SDL_Init(0);
	

	if (!d3dx.Create(800, 600, 32, !gfullscreen)) {
		return 1;
	}


	if (!dsound.Create()) {
		return 1;
	}
	
	if (!load_samples()) {
		return 1;
	}

	if (!dinput.Create()) {
		return 1;
	}
	
	GFile::LoadData();

	lng.LoadLanguage(glangfile);


	CMainFrame* pFrame = new CMainFrame;

	int bonuslevel = 11;
	if (odeadmath) bonuslevel = MAX_BONUS_LEVEL_DEADMATCH;

	// the true is for turning monsters on
	if ( olevelfile != "" ) {
	   pFrame->StartGame(olevelfile, oplayers, odeadmath, true, bonuslevel);
	} else {
	  pFrame->StartMenu();
	}
	
	int time1, time2; // mereni casu
	time2 = time1 = SDL_GetTicks();

	while(EventLoop(pFrame)/* && (dinput.m_key[SDLK_q] == 0)*/) {

		time1 = SDL_GetTicks();

		if ( time1 - time2 < 25 ) SDL_Delay(25-(time1-time2)); 
		time2 =  SDL_GetTicks();;
		
		pFrame->OnTimer();

	}

	d3dx.Destroy();



	// Save configuration before exit
	SaveSettingsToConfig();

	SDL_Quit();

	return 0;
}


bool load_samples()
{
	/*
	#define SND_MENU_CHNG         0
	#define SND_MENU_SEL          1
	#define SND_GAME_EXPLODE      2
	#define SND_GAME_MEGA_EXPLODE 3
	#define SND_GAME_BOMBPUT      4
	#define SND_GAME_BONUS        5
	#define SND_GAME_TIME_BONUS   6
	#define SND_GAME_DEAD_BOMBER  7
	napalmbomba   8
	9 - psani na stroji
	*/
	g_sb[0].LoadFromRes("_menu1.wav");
	g_sb[1].LoadFromRes("_menu2.wav");
	g_sb[SND_GAME_EXPLODE].LoadFromRes("bomba.wav");
	g_sb[SND_GAME_MEGA_EXPLODE].LoadFromRes("megabomba.wav");
	g_sb[4].LoadFromRes("_polozeni_bomby.wav");
	g_sb[SND_GAME_BONUS].LoadFromRes("bonus.wav");
	g_sb[SND_GAME_TIME_BONUS].LoadFromRes("bonus_casovy.wav");
	g_sb[7].LoadFromRes("smrt.wav");
	g_sb[8].LoadFromRes("_napalm.wav");
	g_sb[9].LoadFromRes("B3.wav");

	// mrchy
	g_sb[10].LoadFromRes("_smrt_koule.wav");
	g_sb[11].LoadFromRes("_smrt_slizky.wav");
	g_sb[12].LoadFromRes("_smrt_mesicka.wav");
	g_sb[13].LoadFromRes("_smrt_mesicka.wav");
	g_sb[14].LoadFromRes("megabomba.wav"); // televize
//	g_sb[15].LoadFromRes(""); // TV blesk
	g_sb[16].LoadFromRes("_napalm.wav");  // koule ostnata
	g_sb[17].LoadFromRes("_smrt_mesicka.wav"); // duch
	g_sb[18].LoadFromRes("_smrt_koule.wav");
	g_sb[19].LoadFromRes("_smrt_slizky.wav");
	g_sb[20].LoadFromRes("_smrt_dudlik.wav");
//	g_sb[21].LoadFromRes("SMRT_MESIC"); - sen
//	g_sb[22].LoadFromRes("SMRT_MESIC");
//	g_sb[23].LoadFromRes("SMRT_MESIC");
	g_sb[24].LoadFromRes("_napalm.wav"); // cervenokulka

	return true;
}
