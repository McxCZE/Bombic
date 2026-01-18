// HTranslate.cpp: implementation of the HTranslate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTranslate.h"
#include <stdio.h>
#include "searchfile.h"


HTranslate lng;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HTranslate::HTranslate()
{
	m_langs = 0;

	FILE *f;
	char  c[255];

	f = fopen_search("langid.txt", "rt");

	if (f == nullptr) return;
	
	while (!feof(f)) {
		fscanf(f, "%s", c);
		m_lang[m_langs++] = c;
		if (MAX_LANGS == m_langs) return;
	}

	fclose(f);

}

HTranslate::~HTranslate()
{

}

bool HTranslate::LoadLanguage(const char *filename)
{
	FILE *f;
	int   c;

	f = fopen_search(filename, "rt");

	if (f == nullptr) return false;
	
	m_loaded = 0;
	while (!feof(f)) {
		m_data[m_loaded].s = "";
		fscanf(f, "%d ", &m_data[m_loaded].ID);
		while ((c = getc(f)) != ';' && c != -1) m_data[m_loaded].s += c;
		m_loaded++;
	}

	fclose(f);

	return true;
}

std::string HTranslate::Translate(int ID)
{
	for (int i = 0; i < m_loaded; i++)
		if (m_data[i].ID == ID) return m_data[i].s;

	return "";
}

bool HTranslate::LoadLanguage(int ID)
{
	if (ID >= m_langs) return false;
	return LoadLanguage(m_lang[ID].c_str());
}
