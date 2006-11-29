#include "wxscheckbox.h"

namespace
{
    wxsRegisterItem<wxsCheckBox> Reg(_T("CheckBox"),wxsTWidget,_T("Standard"),60);

    WXS_ST_BEGIN(wxsCheckBoxStyles)
        WXS_ST_CATEGORY("wxCheckBox")
        WXS_ST(wxCHK_2STATE)
        WXS_ST(wxALIGN_RIGHT)
        WXS_ST_MASK(wxCHK_3STATE,wxsSFAll,wxsSFMGL |wxsSFGTK12 | wxsSFOS2,true)
        WXS_ST_MASK(wxCHK_ALLOW_3RD_STATE_FOR_USER,wxsSFAll,wxsSFMGL |wxsSFGTK12 | wxsSFOS2,true)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsCheckBoxEvents)
        WXS_EVI(EVT_CHECKBOX,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEvent,Click)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsCheckBox::wxsCheckBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsBaseProperties::flAll,
        wxsCheckBoxEvents,
        wxsCheckBoxStyles,
        _T("")),
    Label(_("Label")),
    IsChecked(false)
{}


void wxsCheckBox::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxCheckBox(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,Label) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( IsChecked ) Code << GetVarName() << _T("->SetValue(")
                             << wxString::Format(_T("%d"),IsChecked) << _T(";\n");
            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsCheckBox::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsCheckBox::OnBuildPreview(wxWindow* Parent,bool Exact,bool)
{
    wxCheckBox* Preview = new wxCheckBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    if ( IsChecked ) Preview->SetValue(IsChecked);
    return SetupWindow(Preview,Exact);
}

void wxsCheckBox::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsCheckBox,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsCheckBox,IsChecked,0,_("Checked"),_("checked"),false)
}

void wxsCheckBox::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/checkbox.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsCheckBox::OnEnumDeclFiles"),Language);
    }
}
