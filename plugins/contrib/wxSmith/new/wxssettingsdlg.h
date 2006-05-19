#ifndef WXSSETTINGSDLG_H
#define WXSSETTINGSDLG_H

#include "wxsglobals.h"

//(*Headers(wxsSettingsDlg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
//*)

#include "configurationpanel.h"

class wxsSettingsDlg: public cbConfigurationPanel
{
    public:

        wxsSettingsDlg(wxWindow* parent,wxWindowID id = -1);
        virtual ~wxsSettingsDlg();

        //(*Identifiers(wxsSettingsDlg)
        enum Identifiers
        {
            ID_BUTTON1 = 0x1000,
            ID_BUTTON2,
            ID_CHECKBOX1,
            ID_CHECKBOX2,
            ID_CHECKBOX3,
            ID_CHECKBOX4,
            ID_CHECKBOX5,
            ID_CHECKBOX6,
            ID_CHECKBOX7,
            ID_CHECKBOX8,
            ID_CHOICE1,
            ID_COMBOBOX1,
            ID_RADIOBUTTON1,
            ID_RADIOBUTTON2,
            ID_RADIOBUTTON3,
            ID_RADIOBUTTON4,
            ID_SPINCTRL2,
            ID_SPINCTRL3,
            ID_STATICTEXT10,
            ID_STATICTEXT12,
            ID_STATICTEXT13,
            ID_STATICTEXT2,
            ID_STATICTEXT3,
            ID_STATICTEXT4,
            ID_STATICTEXT5,
            ID_STATICTEXT6,
            ID_STATICTEXT7,
            ID_STATICTEXT8,
            ID_STATICTEXT9
        };
        //*)

        wxString GetTitle() const { return _("wxSmith settings"); }
        wxString GetBitmapBaseName() const { return _T("wxsmith"); }
        void OnApply();
        void OnCancel(){}
    protected:

        //(*Handlers(wxsSettingsDlg)
        void OnDragTargetColClick(wxCommandEvent& event);
        void OnDragParentColClick(wxCommandEvent& event);
        void OnDragAssistTypeSelect(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsSettingsDlg)
        wxFlexGridSizer* FlexGridSizer1;
        wxFlexGridSizer* FlexGridSizer6;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxFlexGridSizer* FlexGridSizer2;
        wxComboBox* DragAssistType;
        wxButton* DragTargetCol;
        wxButton* DragParentCol;
        wxFlexGridSizer* FlexGridSizer3;
        wxRadioButton* Icons16;
        wxRadioButton* Icons32;
        wxStaticText* StaticText6;
        wxFlexGridSizer* FlexGridSizer4;
        wxRadioButton* TIcons16;
        wxRadioButton* TIcons32;
        wxStaticBoxSizer* StaticBoxSizer2;
        wxFlexGridSizer* FlexGridSizer5;
        wxStaticText* StaticText7;
        wxSpinCtrl* spinProportion;
        wxStaticText* StaticText8;
        wxBoxSizer* BoxSizer2;
        wxCheckBox* chkTop;
        wxCheckBox* chkBottom;
        wxCheckBox* chkLeft;
        wxCheckBox* chkRight;
        wxStaticText* StaticText9;
        wxCheckBox* chkExpand;
        wxStaticText* StaticText10;
        wxCheckBox* chkShaped;
        wxStaticText* StaticText12;
        wxChoice* choicePlacement;
        wxStaticText* StaticText13;
        wxSpinCtrl* spinBorder;
        wxCheckBox* chkBorderDU;
        wxBoxSizer* BoxSizer1;
        wxCheckBox* chkAutoSelect;
        //*)

    private:

        DECLARE_EVENT_TABLE()
};

#endif
