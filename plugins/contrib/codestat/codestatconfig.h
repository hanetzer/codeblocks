/***************************************************************
 * Name:      codestatconfig.h
 * Purpose:   Code::Blocks plugin
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef CODESTATCONFIG_H
#define CODESTATCONFIG_H

#include <cbplugin.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include "language_def.h"

class CodeStatConfigDlg : public cbConfigurationPanel
{
	public:
		CodeStatConfigDlg(wxWindow* parent, LanguageDef lang[NB_FILETYPES]);
		virtual ~CodeStatConfigDlg();

        virtual wxString GetTitle(){ return _("Code statistics settings"); }
        virtual wxString GetBitmapBaseName(){ return _T("codestats"); }
        virtual void OnApply();
        virtual void OnCancel(){}
	protected:
      //void SaveSettings();
      void ComboBoxEvent(wxCommandEvent & event);
      void PrintLanguageInfo(int id);
	private:
	   LanguageDef languages[NB_FILETYPES];
      DECLARE_EVENT_TABLE()
};

#endif // CODESTATCONFIG_H
