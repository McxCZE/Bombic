// GBase.cpp: implementation of the GBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBase.h"
#include "MainFrm.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBase::GBase()
{
	
}

GBase::~GBase()
{

}

void GBase::Destroy()
{

}

void GBase::Init(CMainFrame *parent) 
{
	m_pParent = parent;
	m_font    = parent->m_font;
	m_fontL   = parent->m_fontL;
};
