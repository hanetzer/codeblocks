#include "wxsspinbutton.h"

namespace
{
    wxsRegisterItem<wxsSpinButton> Reg(_T("SpinButton"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsSpinButtonStyles,_T("wxSP_VERTICAL|wxSP_ARROW_KEYS"))
        WXS_ST_CATEGORY("wxsSpinButton")

    // NOTE (cyberkoa##): wxSP_HORIZONTAL, wxSP_VERTICAL are not found in HELP but in wxMSW's XRC. Assume same as spinbutton
        WXS_ST_MASK(wxSP_HORIZONTAL,wxsSFAll,wxsSFGTK,true)
        WXS_ST(wxSP_VERTICAL)

        WXS_ST(wxSP_ARROW_KEYS)
        WXS_ST(wxSP_WRAP)
    WXS_ST_END()



    WXS_EV_BEGIN(wxsSpinButtonEvents)
        WXS_EVI(EVT_SPIN,wxEVT_SCROLL_THUMBTRACK,wxSpinEvent,Change)
        WXS_EVI(EVT_SPIN_UP,wxEVT_SCROLL_LINEUP,wxSpinEvent,ChangeUp)
        WXS_EVI(EVT_SPIN_DOWN,wxEVT_SCROLL_LINEDOWN,wxSpinEvent,ChangeDown)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsSpinButton::wxsSpinButton(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSpinButtonEvents,
        wxsSpinButtonStyles),
    Value(0),
    Min(0),
    Max(100)
{}

void wxsSpinButton::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( GetParent() )
            {
                Code<< GetVarName() << _T(" = new wxSpinButton(");
            }
            else
            {
                Code<< _T("Create(");
            }
            Code<< WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,GetVarName(),false) << _T(");\n");

            if ( Value ) Code << GetVarName() << _T("->SetValue(") << wxString::Format(_T("%d"),Value) << _T(");\n");
            if ( Max > Min ) Code << GetVarName() << _T("->SetRange(") << wxString::Format(_T("%d"),Min) << _T(",") << wxString::Format(_T("%d"),Max) << _T(");\n");
            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSpinButton::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsSpinButton::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxSpinButton* Preview = new wxSpinButton(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    if ( Value ) Preview->SetValue(Value);
    if ( Max > Min ) Preview->SetRange(Min,Max);

    return SetupWindow(Preview,Flags);
}

void wxsSpinButton::OnEnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsSpinButton,Value,0,_("Value"),_T("value"),0)
    WXS_LONG(wxsSpinButton,Min,0,_("Min Value"),_T("min"),0)
    WXS_LONG(wxsSpinButton,Max,0,_("Max Value"),_T("max"),0)
}

void wxsSpinButton::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/spinbutt.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsSpinButton::OnEnumDeclFiles"),Language);
    }
}
