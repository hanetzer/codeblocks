#include "wxsresource.h"
#include "wxsextresmanager.h"

wxsResource::wxsResource(wxsProject* Owner,const wxString& ResourceName,const wxString& ResourceType,const wxString& GUI,wxsCodingLang Language):
    m_ResourceType(ResourceType),
    m_ResourceName(ResourceName),
    m_GUI(GUI),
    m_Owner(Owner),
    m_Language(Language)
{}

wxsResource::~wxsResource()
{}

void wxsResource::EditOpen()
{
    if ( m_Editor )
    {
        m_Editor->Activate();
    }
    else
    {
        m_Editor = OnCreateEditor();
    }
}

void wxsResource::EditClose()
{
    if ( m_Editor )
    {
        m_Editor->Close();
        // Inside Close() m_Editor should be zeroed
    }
}

void wxsResource::EditorClosed()
{
    m_Editor = NULL;
    if ( !m_Owner )
    {
        wxsExtRes()->EditorClosed(this);
    }
}

void wxsResource::BuildTreeEntry(const wxsResourceItemId& Parent)
{
    m_TreeItemId = wxsTree()->AppendItem(Parent,GetResourceName());
}

bool wxsResource::ReadConfig(const TiXmlElement* Node)
{
    m_ResourceName = cbC2U(Node->Attribute("name")));
    m_Language = wxsCodeMarks::Id(cbC2U(Node->Attribute("language")));
    if ( GetResourceName().empty() ) return false;
    return OnReadConfig(Node);
}

bool wxsResource::WriteConfig(TiXmlElement* Node)
{
    bool Result = OnWriteConfig(Node);
    Node->SetAttribute("name",cbU2C(m_ResourceName));
    Node->SetAttribute("language",cbC2U(m_Language));
    return Result;
}

IMPLEMENT_CLASS(wxsResource,wxObject)
