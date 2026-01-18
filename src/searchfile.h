#ifndef _SEARCHFILE_H_
#define _SEARCHFILE_H_

#include <fstream>
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>

#ifndef PATH_MAX
#define PATH_MAX 4096             
#endif 
                                                                                                                                                                              
class PathList
{         
public:  
       PathList(const char *filename);
       PathList(const char *filename, const char *subdir);
       const char * next(void);
       void reset(void);  
private:                                                                                                                                                                      
       const char *basename; 
       char buf[PATH_MAX]; 
       int state, subdir_num;
       const char *fixed_subdir;
};

/*
class fstream_search : public fstream
{
public:
	void open(const char *name, openmode mode);
	fstream_search(const char *name, openmode mode);
};
*/

FILE * fopen_search(const char *name, const char *mode);

SDL_Surface * IMG_Load_Search(const char *name);

TTF_Font * TTF_OpenFont_Search(const char *name, int size);


/* Returns path for file or NULL if failed. 
   Uses static buffer for returnig result => not thread safe :-(
   It tests file for open.
   Don't use this if you want to create a not existing file :-)
*/
const char * locate_file(const char *name);

/* same, but thread safe*/
bool locate_file(const char *name, char *res, int size);

#endif
