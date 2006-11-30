#ifndef WXSSTATICBITMAP_H
#define WXSSTATICBITMAP_H

#include "../wxswidget.h"

/** \brief Class for wxsStaticBitmap widget */
class wxsStaticBitmap: public wxsWidget
{
    public:

        wxsStaticBitmap(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,bool Exact,bool Store);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxString Label;
        wxsBitmapData Bitmap;
};

#endif
