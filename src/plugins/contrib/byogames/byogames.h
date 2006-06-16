/***************************************************************
 * Name:      byogames.h
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#ifndef BYOGAMES_H
#define BYOGAMES_H

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK

class BYOGames : public cbToolPlugin
{
	public:

		BYOGames();
		~BYOGames();
		int Configure(){ return  0; }
		int GetConfigurationPriority() const{ return  50; }
		int GetConfigurationGroup() const{ return  cgUnknown; }
		cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent){ return  0; }
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
	protected:
	private:

        int SelectGame();
};

// Declare the plugin's hooks
CB_DECLARE_PLUGIN();

#endif
