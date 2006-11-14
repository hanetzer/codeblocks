#include "wxsparent.h"

//#include "wxsglobals.h"
#include "wxsitemfactory.h"

wxsParent::wxsParent(wxsItemRes* Resource,const wxsItemInfo* Info,unsigned long PropertiesFlags,const wxsEventDesc* Events):
    wxsItem(Resource,Info,PropertiesFlags,Events)
{
}

wxsParent::~wxsParent()
{
    for ( size_t i = Children.Count(); i-- > 0; )
    {
        delete Children[i];
        if ( Extra[i] )
        {
            delete Extra[i];
        }
    }
    Children.Clear();
    Extra.Clear();
}

wxsItem* wxsParent::GetChild(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= GetChildCount() ) return NULL;
    return Children[Index];
}

bool wxsParent::AddChild(wxsItem* Child,int Position)
{
    if ( !Child ) return false;
    if ( !CanAddChild(Child,true) ) return false;
    if ( Child->GetParent() != NULL )
    {
        Child->GetParent()->UnbindChild(Child);
    }

    Child->m_Parent = this;

    if ( Position<0 || Position>=GetChildCount() )
    {
        Children.Add(Child);
        Extra.Add(OnBuildExtra());
    }
    else
    {
        Children.Insert(Child,Position);
        Extra.Insert(OnBuildExtra(),Position);
    }
    return true;
}

void wxsParent::UnbindChild(int Index)
{
    if ( Index < 0 ) return;
    if ( Index >= GetChildCount() ) return;

    Children.RemoveAt(Index);
    if ( Extra[Index] )
    {
        delete Extra[Index];
    }
    Extra.RemoveAt(Index);
}

void wxsParent::UnbindChild(wxsItem* Child)
{
    int Index = Children.Index(Child);
    if ( Index == wxNOT_FOUND ) return;
    Children.RemoveAt(Index);
    if ( Extra[Index] )
    {
        delete Extra[Index];
    }
    Extra.RemoveAt(Index);
}

int wxsParent::MoveChild(int OldIndex,int NewIndex )
{
    if ( OldIndex < 0 ) return -1;
    if ( OldIndex >= GetChildCount() ) return -1;
    if ( NewIndex < 0 ) NewIndex = 0;
    if ( NewIndex >= GetChildCount() ) NewIndex = GetChildCount() - 1;

    if ( OldIndex == NewIndex ) return OldIndex;

    wxsItem* Child = Children[OldIndex];
    wxsPropertyContainer* Ext = Extra[OldIndex];
    Children.RemoveAt(OldIndex);
    Extra.RemoveAt(OldIndex);
    Children.Insert(Child,NewIndex);
    Extra.Insert(Ext,NewIndex);
    return NewIndex;
}

int wxsParent::GetChildIndex(wxsItem* Child)
{
    int Index = Children.Index(Child);
    return (Index == wxNOT_FOUND) ? -1 : Index;
}

bool wxsParent::IsGrandChild(wxsItem* Child,bool Safe)
{
    if ( !Safe )
    {
        while ( Child != NULL )
        {
            if ( Child == this ) return true;
            Child = Child->GetParent();
        }
        return false;

    }
    else
    {
        if ( Child == this ) return true;
        for ( int i=0; i<GetChildCount(); i++ )
        {
            wxsItem* MyChild = GetChild(i);
            if ( MyChild == Child ) return true;
            wxsParent* MyChildAsParent = MyChild->ConvertToParent();
            if ( MyChildAsParent->IsGrandChild(Child,true) ) return true;
        }
    }

    return false;
}

void wxsParent::StoreExtraData(int Index,TiXmlElement* Element)
{
    if ( Index < 0 ) return;
    if ( Index >= GetChildCount() ) return;
    if ( !Extra[Index] ) return;

    Extra[Index]->XmlWrite(Element);
}

void wxsParent::RestoreExtraData(int Index,TiXmlElement* Element)
{
    if ( Index < 0 ) return;
    if ( Index >= GetChildCount() ) return;
    if ( !Extra[Index] ) return;

    Extra[Index]->XmlRead(Element);
}

void wxsParent::OnEnumChildProperties(wxsItem* Child,long Flags)
{
    // Enumerating properties of child item
    Child->EnumItemProperties(Flags);

    // Adding properties from extra data container when there's extra data
    // associated. It will be disabled in xml opearations since
    // it's done on parent's level
    if ( !(Flags&flXml) )
    {
        int Index = GetChildIndex(Child);
        if ( (Index >= 0) && (Index < (int)Extra.Count()) && Extra[Index] )
        {
            SubContainer(Extra[Index],Flags);
        }
    }
}

void wxsParent::OnAddChildQPP(wxsItem* Child,wxsAdvQPP* QPP)
{
    // Parent may block item's private QPP and force valid order of panels
    Child->OnAddItemQPP(QPP);
}

wxsPropertyContainer* wxsParent::GetChildExtra(int Index)
{
    if ( Index < 0 ) return NULL;
    if ( Index >= GetChildCount() ) return NULL;
    return Extra[Index];
}

bool wxsParent::OnXmlRead(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsItem::XmlRead(Elem,IsXRC,IsExtra);
    if ( IsXRC )
    {
        for ( TiXmlElement* Object = Elem->FirstChildElement(); Object; Object = Object->NextSiblingElement() )
        {
            if ( strcmp(Object->Value(),"object") ) continue;
            if ( !OnXmlReadChild(Object,IsXRC,IsExtra) ) Ret = false;
        }
    }
    return Ret;
}

bool wxsParent::OnXmlWrite(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    bool Ret = wxsItem::XmlWrite(Elem,IsXRC,IsExtra);
    if ( IsXRC )
    {
        for ( size_t i=0; i<Children.Count(); i++ )
        {
            TiXmlElement* Object = Elem->InsertEndChild(TiXmlElement("object"))->ToElement();
            if ( !OnXmlWriteChild((int)i,Object,IsXRC,IsExtra) )
            {
                Elem->RemoveChild(Object);
                Ret = false;
            }
        }
    }
    return Ret;
}

bool wxsParent::OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    wxString ExtraName = OnXmlGetExtraObjectClass();
    TiXmlElement* RealElem = Elem;

    // Finding out what's real node for item
    if ( !ExtraName.empty() )
    {
        if ( cbC2U(Elem->Attribute("class")) != ExtraName ) return false;
        RealElem = Elem->FirstChildElement("object");
        if ( !RealElem ) return false;
    }

    // Creating new item from class name
    wxsItem* NewItem = wxsItemFactory::Build(cbC2U(RealElem->Attribute("class")),GetResource());
    if ( !NewItem )
    {
        NewItem = wxsItemFactory::Build(_T("Custom"),GetResource());
        if ( !NewItem ) return false;
    }

    // Trying to add new item to this class
    if ( !AddChild(NewItem) )
    {
        delete NewItem;
        return false;
    }

    // Loading extra data
    if ( !ExtraName.empty() )
    {
        RestoreExtraData(Children.Count()-1,Elem);
    }

    // Loading item
    return NewItem->XmlRead(RealElem,IsXRC,IsExtra);
}

bool wxsParent::OnXmlWriteChild(int Index,TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    wxString ExtraName = OnXmlGetExtraObjectClass();
    TiXmlElement* RealElem = Elem;

    // Storing extra data
    if ( !ExtraName.empty() )
    {
        RealElem = Elem->InsertEndChild(TiXmlElement("object"))->ToElement();
        Elem->SetAttribute("class",cbU2C(ExtraName));
        StoreExtraData(Index,Elem);
    }

    // Saving child item
    return Children[Index]->XmlWrite(RealElem,IsXRC,IsExtra);
}
