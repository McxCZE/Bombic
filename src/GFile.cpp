// GFile.cpp: implementation of the GFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GFile.h"
#include "data.h"
#include "searchfile.h"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

st_zed P_zed[MAX_ZED] = {
	{"zem",                  0, 0, true,  0, 1, 0, 1, false},
	{"strana leva",          1, 0, false, 0, 1, 0, 1, false},
	{"strana leva-horni",    2, 0, false, 0, 1, 0, 1, false},
	{"strana horni",         3, 0, false, 0, 1, 0, 1, false},
	{"strana prava-horni",   4, 0, false, 0, 1, 0, 1, false},
	{"strana prava",         5, 0, false, 0, 1, 0, 1, false},
	{"strana leva-doli",     6, 0, false, 0, 1, 0, 1, false},
	{"strana dolni",         7, 0, false, 0, 1, 0, 1, false},
	{"strana prava dolni",   8, 0, false, 0, 1, 0, 1, false},
	{"nepruchodna",          9, 0, false, 0, 1, 0, 1, false},
	{"nepruchodna s teckou", 0, 1, false, 3, 3, 0, 1, false},
	{"znicitelna",           4, 1, false, 0, 1, 5, 3, true},
};

st_mrcha P_mrcha[MAX_MRCH] = {
	{"Koule",          4, 4, 2, 1, 100, 50, 70, false, false, 255, 30},
	{"Slizoun",         2, 4, 3, 2, 100, 50, 70, false, false, 255, 30},
	{"Mracek",          4, 4, 2, 1, 100, 50, 70, false, false, 255, 30},
	{"Mesicek",         2, 4, 3, 2, 100, 50, 70, false, false, 255, 30},
};

st_bomber P_bomber = {4, 8, 2, 50, 70, 30};

#define DAT_FILE "bomber.dat"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GFile::GFile()
{

}

GFile::~GFile()
{

}

bool GFile::LoadData()
{
	const char *df = locate_file(DAT_FILE);
	if (df == nullptr) return false;
	fstream f(df, ios::in|ios::binary);
	if (!f) {
		cerr << "Unalbe to read file " << DAT_FILE << endl;
		return false;
	}
	f.read((char*)&P_bomber, sizeof(st_bomber));
	f.read((char*)&P_mrcha[0], sizeof(st_mrcha) * MAX_MRCH);
	f.read((char*)&P_zed[0], sizeof(st_zed) * MAX_ZED);
	
	f.close();

	return true;
}

void GFile::SaveData()
{
	fstream f(DAT_FILE, ios::out|ios::binary);
	if (!f) {
		cerr << "Unalbe to write file" << DAT_FILE << endl;
		return;
	}
	f.write((const char*)&P_bomber, sizeof(st_bomber));
	f.write((const char*)&P_mrcha[0], sizeof(st_mrcha) * MAX_MRCH);
	f.write((const char*)&P_zed[0], sizeof(st_zed) * MAX_ZED);
	f.close();
}

void GFile::SaveMap(st_map *map, const string name)
{
	fstream f(name.c_str(), ios::out|ios::binary);
	if (!f) {
		cerr <<	"Unable to save map" << name << endl;
	}
	
	f.write((const char*)map, sizeof(st_map));
	f.close();
}

void GFile::LoadMap(st_map *map, const string file)
{
	const char *df = locate_file(file.c_str());
	if (df == nullptr) return;
	fstream f(df, ios::in|ios::binary);
	if (!f) {
		cerr << "Unable to open file" << file << endl;
	}
	
	f.read((char*)map, sizeof(st_map));
	f.close();
}
