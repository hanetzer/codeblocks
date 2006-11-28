#include "wxssizer.h"

#include <wx/dcclient.h>
#include <messagemanager.h>

namespace
{
    class wxsSizerPreview: public wxPanel
    {
        public:
            wxsSizerPreview(wxWindow* Parent): wxPanel(Parent,-1,wxDefaultPosition,wxDefaultSize, wxTAB_TRAVERSAL)
            {
                InheritAttributes();
                Connect(wxID_ANY,wxEVT_PAINT,(wxObjectEventFunction)&wxsSizerPreview::OnPaint);
            }

        private:

            void OnPaint(wxPaintEvent& event)
            {
                // Drawing additional border around te panel
                wxPaintDC DC(this);
                int W, H;
                GetSize(&W,&H);
                DC.SetBrush(*wxTRANSPARENT_BRUSH);
                DC.SetPen(*wxRED_PEN);
                DC.DrawRectangle(0,0,W,H);
            }
    };
}

void wxsSizerExtra::OnEnumProperties(long Flags)
{
    WXS_SIZERFLAGS(wxsSizerExtra,Flags,0);
    WXS_DIMENSION(wxsSizerExtra,Border,0,_("Border"),_("  Dialog Units"),_T("border"),0,false);
    WXS_LONG(wxsSizerExtra,Proportion,0,_("Proportion"),_T("option"),0);
}

wxString wxsSizerExtra::AllParamsCode(const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            return wxString::Format(_T("%d,"),Proportion) +
                   wxsSizerFlagsProperty::GetString(Flags) +
                   _T(",") << Border.GetPixelsCode(WindowParent,wxsCPP);

        default:
            wxsCodeMarks::Unknown(_T("wxsSizerExtra::AllParamsCode"),Language);
    }
    return wxEmptyString;
}

wxsSizer::wxsSizer(wxsItemResData* Data,const wxsItemInfo* Info):
    wxsParent(Data,Info,flVariable,NULL)
{
}

void wxsSizer::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    OnBuildSizerCreatingCode(Code,WindowParent,Language);

    bool UnknownLang = false;
    int Count = GetChildCount();
    for ( int i=0; i<Count; i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsSizerExtra* Extra = (wxsSizerExtra*)GetChildExtra(i);

        // Using same parent as we got, sizer is not a parent window
        Child->BuildCreatingCode(Code,WindowParent,Language);

        switch ( Child->GetType() )
        {
            case wxsTWidget:
            case wxsTContainer:
            case wxsTSizer:
                switch ( Language )
                {
                    case wxsCPP:
                    {
                        // cyberkoa : Left out a comma after the GetVarName()
                        Code << GetVarName() << _T("->Add(") << Child->GetVarName() << _T(",")
                             << Extra->AllParamsCode(WindowParent,wxsCPP) << _T(");\n");
                        break;
                    }

                    default:
                    {
                        UnknownLang = true;
                    }
                }
                break;

            case wxsTSpacer:
                // Spacer is responsible for adding itself into sizer
                break;

            default:
                break;
        }
    }

    if ( UnknownLang )
    {
        wxsCodeMarks::Unknown(_T("wxsSizer::BuildCreatingCode"),Language);
    }
}

wxObject* wxsSizer::OnBuildPreview(wxWindow* Parent,bool Exact,bool Store)
{
    wxWindow* NewParent = Parent;

    if ( !Exact )
    {
        NewParent = new wxsSizerPreview(Parent);
    }

    wxSizer* Sizer = OnBuildSizerPreview(NewParent);
    int Count = GetChildCount();
    for ( int i=0; i<Count; i++ )
    {
        wxsItem* Child = GetChild(i);
        wxsSizerExtra* Extra = (wxsSizerExtra*)GetChildExtra(i);

        // We pass either Parent passed to current BuildPreview function
        // or pointer to additional parent currently created
        wxObject* ChildPreview = Child->BuildPreview(NewParent,Exact,Store);
        if ( !ChildPreview ) continue;

        wxSizer* ChildAsSizer = wxDynamicCast(ChildPreview,wxSizer);
        wxWindow* ChildAsWindow = wxDynamicCast(ChildPreview,wxWindow);
        wxSizerItem* ChildAsItem = wxDynamicCast(ChildPreview,wxSizerItem);
        if ( ChildAsSizer )
        {
            Sizer->Add(ChildAsSizer,Extra->Proportion,
                wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
                Extra->Border.GetPixels(Parent));
        }
        else if ( ChildAsWindow )
        {
            Sizer->Add(ChildAsWindow,Extra->Proportion,
                wxsSizerFlagsProperty::GetWxFlags(Extra->Flags),
                Extra->Border.GetPixels(Parent));
        }
        else if ( ChildAsItem )
        {
            ChildAsItem->SetProportion(Extra->Proportion);
            ChildAsItem->SetFlag(wxsSizerFlagsProperty::GetWxFlags(Extra->Flags));
            ChildAsItem->SetBorder(Extra->Border.GetPixels(Parent));
            Sizer->Add(ChildAsItem);
        }
    }

    if ( !Exact )
    {
        NewParent->SetSizer(Sizer);
        Sizer->Fit(NewParent);
        Sizer->SetSizeHints(NewParent);
        wxSizer* OutSizer = new wxBoxSizer(wxHORIZONTAL);
        OutSizer->Add(NewParent,1,wxEXPAND,0);
        Parent->SetSizer(OutSizer);
        OutSizer->SetSizeHints(Parent);
        return NewParent;
    }

    Parent->SetSizer(Sizer);
    Sizer->SetSizeHints(Parent);
    return Sizer;
}

wxsPropertyContainer* wxsSizer::OnBuildExtra()
{
    return new wxsSizerExtra();
}

void wxsSizer::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    wxsParent::OnAddChildQPP(Child,QPP);
    // TODO: Code it
    /*
    int Index = GetChildIndex(Child);
    if ( Index >= 0 )
    {
        QPP->Register(new wxsSizerParentQP(QPP,(wxsSizerExtra*)GetChildExtra(Index)),_("Sizer"));
    }
    */
}

bool wxsSizer::OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    if ( cbC2U(Elem->Attribute("class")) == _T("spacer") )
    {
        wxsItem* Item = wxsItemFactory::Build(_T("spacer"),GetResourceData());
        if ( !AddChild(Item) )
        {
            delete Item;
            return false;
        }
        RestoreExtraData(GetChildCount()-1,Elem);
        return Item->XmlRead(Elem,IsXRC,IsExtra);
    }

    return wxsParent::OnXmlReadChild(Elem,IsXRC,IsExtra);
}

bool wxsSizer::OnXmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    wxsItem* Child = GetChild(Index);
    if ( Child->GetType() == wxsTSpacer )
    {
        Elem->SetAttribute("class","spacer");
        StoreExtraData(Index,Elem);
        return Child->XmlWrite(Elem,IsXRC,IsExtra);
    }

    return wxsParent::OnXmlWriteChild(Index,Elem,IsXRC,IsExtra);
}

wxString wxsSizer::OnXmlGetExtraObjectClass()
{
    return _T("sizeritem");
}

void wxsSizer::OnEnumItemProperties(long Flags)
{
    OnEnumSizerProperties(Flags);
}

void wxsSizer::OnAddItemQPP(wxsAdvQPP* QPP)
{
    OnAddSizerQPP(QPP);
}


