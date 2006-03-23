#ifndef WXSBUTTON_H
#define WXSBUTTON_H

#include "../wxswidget.h"

/** \brief Class for wxButton widget */
class wxsButton: public wxsWidget
{
    public:
        
        /** \brief Widget's info, in public scope because it will be accessed
         *         from widget manager
         */
        static wxsItemInfo Info;
        
        /** \brief Ctor
         *
         * Ctor in final classes should have only one argument - pointer to
         * resource. That's because manager will have only one argument when
         * creating widget also.
         */
        wxsButton(wxsWindowRes* Resource);

        /** \brief Function building creating code */
        virtual void BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        
    protected:
    
        /** \brief Function building preview */
        virtual wxObject* DoBuildPreview(wxWindow* Parent,bool Exact);

        /** \brief Function enumerating properties specific for this widget 
         *         only. Other properties are added automatically.
         */
        virtual void EnumWidgetProperties(long Flags);
        
    private:
    
        wxString Label;
        bool IsDefault;
};

#endif
