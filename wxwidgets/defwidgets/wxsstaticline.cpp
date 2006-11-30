#include "wxsstaticline.h"

#include <wx/statline.h>

namespace
{
    wxsRegisterItem<wxsStaticLine> Reg(_T("StaticLine"),wxsTWidget,_T("Standard"),60);

    WXS_ST_BEGIN(wxsStaticLineStyles)
        WXS_ST_CATEGORY("wxStaticLine")
        WXS_ST(wxLI_HORIZONTAL)
        WXS_ST(wxLI_VERTICAL)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsStaticLineEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsStaticLine::wxsStaticLine(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsBaseProperties::flAll,
        wxsStaticLineEvents,
        wxsStaticLineStyles,
        _T(""))
{
    // Default the size so that it can be seen in the edit mode
    GetBaseProps()->m_Size.IsDefault = false;
    GetBaseProps()->m_Size.X = 10;
    GetBaseProps()->m_Size.Y = -1;
}



void wxsStaticLine::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxStaticLine(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticLine::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsStaticLine::OnBuildPreview(wxWindow* Parent,bool Exact,bool)
{
    wxStaticLine* Preview = new wxStaticLine(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Exact);
}

void wxsStaticLine::OnEnumWidgetProperties(long Flags)
{
}

void wxsStaticLine::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/statline.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsStaticLine::OnEnumDeclFiles"),Language);
    }
}
