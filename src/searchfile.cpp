
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "searchfile.h"
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>

#ifdef HAVE_SDL_SDL_IMAGE_H
#include "SDL_image.h"
#define LOAD_IMG(file) IMG_Load(file)
#else
#define LOAD_IMG(file) SDL_LoadBMP(file)
#endif

using namespace std;

PathList::PathList(const char *filename)
{
       basename = filename; 
       state = 0;   
       subdir_num = 0;
       fixed_subdir = nullptr;
}

PathList::PathList(const char *filename, const char *subdir)
{
       basename = filename;
       state = 0;
       fixed_subdir = subdir;
}

void PathList::reset(void)
{
       state = 0; 
       subdir_num = 0;
}

const char * PathList::next(void)
{ 
       static const char * const subdirs[] = { 
               "", 
               "menu",
               "maps", 
	       "font",
	       "eng",
	       "singlemaps",
	       "samples",
               NULL   
       }; 
       static char *env_data_dir = nullptr; 
       const char *cur_subdir; 
  
       if (fixed_subdir) {
               cur_subdir = fixed_subdir;
       } else { 
               cur_subdir = subdirs[subdir_num];
       }  
       
       switch (state) {
       case 0:   // relative path
               snprintf(buf, PATH_MAX, "./%s/%s", cur_subdir, basename);
               break;
       case 1:   // data directory taken from environment
               env_data_dir = getenv("BOMBIC_DATA_DIR");
               if (!env_data_dir) {
                       // Skip to next state if env var not set
                       state++;
                       return next();
               }
               snprintf(buf, PATH_MAX, "%s/%s/%s", env_data_dir, cur_subdir,
                                       basename);
               break;
       case 2:   // default data diretory
#ifdef PACKAGE_DATA_DIR
               snprintf(buf, PATH_MAX, PACKAGE_DATA_DIR "/%s/%s", cur_subdir,
                                       basename);
               break;
#endif
       default:
               fprintf(stderr, "File %s not found\n", basename);
               return NULL;
       }
       
       if (fixed_subdir) {
               state++; 
       } else {   
               subdir_num++;
               if (!subdirs[subdir_num]) { 
                       state++;
		      
                       subdir_num = 0;
               }                   
       }                        
#ifdef DEBUG        
       fprintf(stderr, "Trying %s\n", buf); 
#endif    
       return buf;
}



FILE * fopen_search(const char *name, const char *mode)
{                           
       PathList pl(name);
       const char *path;
       FILE *res;   
           
       do {   
               path = pl.next(); 
               if (!path) { 
                       return NULL;
               }         
               res = fopen(path, mode); 
       } while (!res); 
       return res; 
}




SDL_Surface * IMG_Load_Search(const char *name)
{
	PathList pl(name);
	const char *path;
	SDL_Surface *res;   
           
	do {   
		path = pl.next(); 
		if (!path) { 
			return NULL;
		}
		
		errno = 0;
		
		res = LOAD_IMG(path);
		if (!res && (errno != ENOENT)) {
			 fprintf(stderr, "%s\n", SDL_GetError());
		}
       } while (!res); 
       return res;
}



TTF_Font * TTF_OpenFont_Search(const char *name, int size)
{
	
	static char path[PATH_MAX];
	if (!locate_file(name, path, PATH_MAX)) return NULL;

	errno = 0;
	
	TTF_Font *res = TTF_OpenFont(path, size);
	
	if (res == nullptr) {
		cerr << "Opening font failed: " << TTF_GetError() << endl;
	}
	
	return res;
}



bool locate_file(const char *name, char *res, int size)
{
	if (name == nullptr) return false;
	
	PathList pl(name);
	const char *path;
	
	FILE *f = nullptr;   
           
	do {   
		path = pl.next(); 
		if (!path) { 
			break;
		}         
		f = fopen(path, "r"); 
	} while (!f); 

	if (!f) {	
		cerr << "Unable to locate file " << name << endl;
		return false;
	}
	
	fclose(f);
	strncpy(res, path, size);

#ifdef DEBUG
	cout << "Name:'" << name << "' Path:'" << res << "'" << endl;
#endif	
	return true; 
}



const char *locate_file(const char *name)
{
	static char res[PATH_MAX];
	if (!locate_file(name, res, PATH_MAX)) return NULL;
	return res;
}

