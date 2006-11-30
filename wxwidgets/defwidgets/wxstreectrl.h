#ifndef WXSTREECTRL_H
#define WXSTREECTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsTreeCtrl widget */
class wxsTreeCtrl: public wxsWidget
{
    public:

        wxsTreeCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,bool Exact,bool Store);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);


};

#endif
