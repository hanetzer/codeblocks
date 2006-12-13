#ifndef REGEXDLG_H
#define REGEXDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(RegExDlg)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
//*)

#include <wx/html/htmlwin.h>

#ifndef CB_PRECOMP
  #include <wx/listctrl.h> // wxListEvent
  #include <wx/choice.h>
  #include <wx/checkbox.h>
  #include <wx/regex.h>
#endif

class RegExDlg: public wxDialog
{
	public:

		RegExDlg(wxWindow* parent,wxWindowID id = -1);
		virtual ~RegExDlg();

		//(*Identifiers(RegExDlg)
		//*)

		void EndModal(int retCode);

	protected:

		//(*Handlers(RegExDlg)
		void OnCloseButton(wxCommandEvent& event){ Close(); };
		void RunBenchmark(wxCommandEvent& event);
		void OnInit(wxInitDialogEvent& event);
		void OnRegExItemActivated(wxListEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
		//*)

	private:


        wxArrayString GetBuiltinMatches(const wxString& text);
        wxArrayString GetPregMatches(const wxString& text);

        wxTextCtrl *m_regex;
        wxTextCtrl *m_quoted;
        wxChoice *m_library;
        wxCheckBox *m_nocase;
        wxCheckBox *m_newlines;
        wxTextCtrl *m_text;
        wxHtmlWindow *m_output;

        wxRegEx m_wxre;

		DECLARE_EVENT_TABLE()
};

#endif
