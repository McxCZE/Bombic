#ifndef GFILE_H_
#define GFILE_H_


#include <string>
#include "data.h"



class GFile  
{
public:
	static void LoadMap(st_map *map, const std::string file);
	static void SaveMap(st_map *map, const std::string name);
	static void SaveData();
	static bool LoadData();
	GFile();
	virtual ~GFile();

};

#endif // GFILE_H_
