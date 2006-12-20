/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision:  $
* $Id:  $
* $HeadURL:  $
*/

#include "wxsitemeditorcontent.h"
#include "wxsitemeditordragassist.h"
#include "wxsbaseproperties.h"
#include "wxsitemresdata.h"
#include "wxsparent.h"
#include "wxsitemeditor.h"

BEGIN_EVENT_TABLE(wxsItemEditorContent,wxsDrawingWindow)
    EVT_MOUSE_EVENTS(wxsItemEditorContent::OnMouse)
END_EVENT_TABLE()

wxsItemEditorContent::wxsItemEditorContent(wxWindow* Parent,wxsItemResData* Data,wxsItemEditor* Editor):
    wxsDrawingWindow(Parent,-1),
    m_Data(Data),
    m_Editor(Editor),
    m_MouseState(msIdle),
    m_AssistTarget(NULL),
    m_AssistParent(NULL),
    m_AssistAddAfter(false)
{
    m_Assist = new wxsItemEditorDragAssist(this);
}

wxsItemEditorContent::~wxsItemEditorContent()
{
    delete m_Assist;
}

void wxsItemEditorContent::PaintExtra(wxDC* DC)
{
    m_Assist->DrawExtra(m_AssistTarget,m_AssistParent,m_AssistAddAfter,DC);

    for ( size_t i = m_DragPoints.Count(); i-- > 0; )
    {
        DragPointData* DPD = m_DragPoints[i];
        if ( DPD->Grey )
        {
            DC->SetPen(*wxGREY_PEN);
            DC->SetBrush(*wxGREY_BRUSH);
        }
        else
        {
            DC->SetPen(*wxBLACK_PEN);
            DC->SetBrush(*wxBLACK_BRUSH);
        }
        int PosX = DPD->PosX - m_DragBoxSize/2;
        int PosY = DPD->PosY - m_DragBoxSize/2;
    	DC->DrawRectangle(PosX , PosY, m_DragBoxSize, m_DragBoxSize );
    }

}

void wxsItemEditorContent::RefreshSelection()
{
    RebuildDragPoints();
    FullRepaint();
}

void wxsItemEditorContent::ClearDragPoints()
{
    for ( size_t i = m_DragPoints.Count(); i-- > 0; )
    {
        delete m_DragPoints[i];
    }
    m_DragPoints.Clear();
}

void wxsItemEditorContent::GreyDragPoints()
{
    for ( size_t i = m_DragPoints.Count(); i-->0; )
    {
        m_DragPoints[i]->Grey = true;
    }
}

void wxsItemEditorContent::RebuildDragPoints()
{
    ClearDragPoints();
    AddDragPoints(m_Data->GetRootItem(),m_Data->GetLastSelection());
}

void wxsItemEditorContent::AddDragPoints(wxsItem* Item,wxsItem* RootSelection)
{
    if ( Item->GetIsSelected() )
    {
        int PosX, PosY;
        int SizeX, SizeY;
        if ( FindAbsoluteRect(Item,PosX,PosY,SizeX,SizeY) )
        {
            bool Grey = Item!=RootSelection;
            DragPointData* ItemPoints[DragBoxTypeCnt];

            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                ItemPoints[i] = new DragPointData;
                ItemPoints[i]->Grey = Grey;
                ItemPoints[i]->PosX = PosX;
                ItemPoints[i]->PosY = PosY;
                ItemPoints[i]->Item = Item;
                ItemPoints[i]->Type = (DragBoxType)i;

                if ( i == Top || i == Btm )
                {
                    ItemPoints[i]->PosX += SizeX / 2;
                }
                else if ( i == RightTop || i == Right || i == RightBtm )
                {
                    ItemPoints[i]->PosX += SizeX;
                }

                if ( i==Left || i == Right )
                {
                    ItemPoints[i]->PosY += SizeY / 2;
                }
                else if ( i == LeftBtm || i == Btm || i == RightBtm )
                {
                    ItemPoints[i]->PosY += SizeY;
                }

                ItemPoints[i]->DragInitPosX = ItemPoints[i]->PosX;
                ItemPoints[i]->DragInitPosY = ItemPoints[i]->PosY;
            }

            for ( int i=0; i<DragBoxTypeCnt; ++i )
            {
                memcpy(ItemPoints[i]->ItemPoints,ItemPoints,sizeof(ItemPoints[0]->ItemPoints));
                m_DragPoints.Add(ItemPoints[i]);
            }
        }
    }

    wxsParent* Parent = Item->ConvertToParent();
    if ( Parent )
    {
        for ( int i = Parent->GetChildCount(); i-->0; )
        {
            AddDragPoints(Parent->GetChild(i),RootSelection);
        }
    }
}

void wxsItemEditorContent::UpdateDragPoints(DragPointData* anyPoint)
{
    DragPointData** ItemPoints = anyPoint->ItemPoints;
    wxsItem* Item = anyPoint->Item;

    int PosX, PosY;
    int SizeX, SizeY;
    if ( FindAbsoluteRect(Item,PosX,PosY,SizeX,SizeY) )
    {
        for ( int i=0; i<DragBoxTypeCnt; ++i )
        {
            ItemPoints[i]->PosX = PosX;
            ItemPoints[i]->PosY = PosY;
            ItemPoints[i]->Item = Item;

            if ( i == Top || i == Btm )
            {
                ItemPoints[i]->PosX += SizeX / 2;
            }
            else if ( i == RightTop || i == Right || i == RightBtm )
            {
                ItemPoints[i]->PosX += SizeX;
            }

            if ( i==Left || i == Right )
            {
                ItemPoints[i]->PosY += SizeY / 2;
            }
            else if ( i == LeftBtm || i == Btm || i == RightBtm )
            {
                ItemPoints[i]->PosY += SizeY;
            }

            ItemPoints[i]->DragInitPosX = ItemPoints[i]->PosX;
            ItemPoints[i]->DragInitPosY = ItemPoints[i]->PosY;
        }
    }
}

bool wxsItemEditorContent::FindAbsoluteRect(wxsItem* Item,int& PosX,int& PosY,int& SizeX,int& SizeY)
{
    if ( !Item ) return false;
    ItemToRectT::iterator i = m_ItemToRect.find(Item);
    if ( i==m_ItemToRect.end() ) return false;
    wxRect& Rect = (*i).second;
    PosX = Rect.GetX();
    PosY = Rect.GetY();
    SizeX = Rect.GetWidth();
    SizeY = Rect.GetHeight();
    return true;
}

wxsItem* wxsItemEditorContent::FindItemAtPos(int PosX,int PosY,wxsItem* SearchIn)
{
    int itemPosX;
    int itemPosY;
    int itemSizeX;
    int itemSizeY;

    if ( !FindAbsoluteRect(SearchIn,itemPosX,itemPosY,itemSizeX,itemSizeY) ) return NULL;

    if ( PosX < itemPosX ) return NULL;
    if ( PosX >= (itemPosX+itemSizeX) ) return NULL;
    if ( PosY < itemPosY ) return NULL;
    if ( PosY >= (itemPosY+itemSizeY) ) return NULL;

    wxsParent* parent = SearchIn->ConvertToParent();
    if ( parent )
    {
        for ( int i = parent->GetChildCount(); i-->0; )
        {
            wxsItem* f = FindItemAtPos(PosX,PosY,parent->GetChild(i));
            if ( f )
            {
                return f;
            }
        }
    }

    return SearchIn;
}

wxsItemEditorContent::DragPointData* wxsItemEditorContent::FindDragPointAtPos(int PosX,int PosY)
{
    for ( size_t i=m_DragPoints.Count(); i-->0; )
    {
        DragPointData* DPD = m_DragPoints[i];
        int dpx = DPD->PosX - (m_DragBoxSize/2);
        int dpy = DPD->PosY - (m_DragBoxSize/2);

        if ( (PosX >= dpx) && (PosX < dpx+m_DragBoxSize) &&
             (PosY >= dpy) && (PosY < dpy+m_DragBoxSize) )
        {
            return DPD;
        }
    }

// TODO (SpOoN#1#): Search for edges

    return NULL;
}

wxsItemEditorContent::DragPointData* wxsItemEditorContent::FindDragPointFromItem(wxsItem* Item)
{
    for ( size_t i = 0; i<m_DragPoints.Count(); i+= 8 )
    {
        if ( m_DragPoints[i]->Item == Item )
        {
            return m_DragPoints[i];
        }
    }
    return NULL;
}

void wxsItemEditorContent::OnMouse(wxMouseEvent& event)
{
    switch ( m_MouseState )
    {
        case msDraggingPointInit: OnMouseDraggingPointInit (event); break;
        case msDraggingPoint:     OnMouseDraggingPoint     (event); break;
        case msDraggingItemInit:  OnMouseDraggingItemInit  (event); break;
        case msDraggingItem:      OnMouseDraggingItem      (event); break;
        default:                  OnMouseIdle              (event); break;
    }
}

void wxsItemEditorContent::OnMouseIdle(wxMouseEvent& event)
{
    BlockFetch(false);
    m_DragInitPosX = event.GetX();
    m_DragInitPosY = event.GetY();
    if ( event.LeftIsDown() && !event.RightIsDown() && !event.MiddleIsDown() )
    {
        // Selecting / drag init event
        int MouseX = event.GetX();
        int MouseY = event.GetY();

        wxsItem* OnCursor = FindItemAtPos(MouseX,MouseY,m_Data->GetRootItem());
        if ( !OnCursor ) OnCursor = m_Data->GetRootItem();

        wxWindow* Preview = GetPreviewWindow(OnCursor);
        bool NeedRefresh = false;
        if ( Preview )
        {
            int PosX, PosY, SizeX, SizeY;
            FindAbsoluteRect(OnCursor,PosX,PosY,SizeX,SizeY);
            NeedRefresh = OnCursor->MouseClick(Preview,MouseX-PosX,MouseY-PosY);
        }

        DragPointData* DPD = FindDragPointAtPos(MouseX,MouseY);

        if ( DPD )
        {
            // If there's drag point, starting point-dragging sequence
            m_CurDragPoint = DPD;
            m_MouseState = msDraggingPointInit;
        }
        else
        {
            if ( !OnCursor->GetIsSelected() )
            {
                m_Data->SelectItem(OnCursor,!event.ControlDown());
            }
            else
            {
                m_Data->SelectItem(OnCursor,false);
            }

            m_CurDragPoint = FindDragPointFromItem(OnCursor);
            m_CurDragItem = OnCursor;
            m_MouseState = msDraggingItemInit;
        }

        if ( NeedRefresh )
        {
            m_Editor->RebuildPreview();
            m_MouseState = msIdle;
        }
    }

    if ( !event.LeftIsDown() && event.RightDown() && !event.MiddleIsDown() )
    {
        // Menu invoking event
    }

    if ( !event.LeftIsDown() && !event.RightIsDown() && !event.MiddleIsDown() )
    {
        // Updating cursor

        DragPointData* DPD = FindDragPointAtPos(event.GetX(),event.GetY());

        if ( DPD )
        {
    		switch ( DPD->Type )
    		{
                case LeftTop:
                case RightBtm:
                    SetCur(wxCURSOR_SIZENWSE);
                    break;

                case Top:
                case Btm:
                    SetCur(wxCURSOR_SIZENS);
                    break;

                case RightTop:
                case LeftBtm:
                    SetCur(wxCURSOR_SIZENESW);
                    break;

                case Left:
                case Right:
                    SetCur(wxCURSOR_SIZEWE);
                    break;

                default:
                    SetCur(wxCURSOR_ARROW);
    		}
        }
        else
        {
            SetCur(wxCURSOR_ARROW);
        }

    }
}

void wxsItemEditorContent::OnMouseDraggingPointInit(wxMouseEvent& event)
{
    BlockFetch(true);

    if ( event.RightIsDown() || event.MiddleIsDown() || !event.LeftIsDown() )
    {
        m_MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    if ( DeltaX<0 ) DeltaX = -DeltaX;
    int DeltaY = event.GetY() - m_DragInitPosY;
    if ( DeltaY<0 ) DeltaY = -DeltaY;

    if ( DeltaX + DeltaY > m_MinDragDistance )
    {
        m_MouseState = msDraggingPoint;
    }
}

void wxsItemEditorContent::OnMouseDraggingPoint(wxMouseEvent& event)
{
    if ( event.RightIsDown() || event.MiddleIsDown() )
    {
        // Cancelling change
        for ( size_t i=0; i<m_DragPoints.Count(); i++ )
        {
            m_DragPoints[i]->PosX = m_DragPoints[i]->DragInitPosX;
            m_DragPoints[i]->PosY = m_DragPoints[i]->DragInitPosY;
        }
        m_MouseState = msIdle;
        return;
    }

    if ( !event.LeftIsDown() )
    {
        m_Data->BeginChange();

        wxsBaseProperties* Props = m_CurDragPoint->Item->GetBaseProps();
        if ( Props )
        {
            DragPointData* leftTop = m_CurDragPoint->ItemPoints[LeftTop];
            DragPointData* rightBtm = m_CurDragPoint->ItemPoints[RightBtm];
            int OldPosX = leftTop->DragInitPosX;
            int OldPosY = leftTop->DragInitPosY;
            int OldSizeX = rightBtm->DragInitPosX - OldPosX;
            int OldSizeY = rightBtm->DragInitPosY - OldPosY;
            int NewPosX = leftTop->PosX;
            int NewPosY = leftTop->PosY;
            int NewSizeX = rightBtm->PosX - NewPosX;
            int NewSizeY = rightBtm->PosY - NewPosY;

            if ( NewSizeX < 0 )
            {
                NewPosX += NewSizeX;
                NewSizeX = -NewSizeX;
            }

            if ( NewSizeY < 0 )
            {
                NewPosY += NewSizeY;
                NewSizeY = -NewSizeY;
            }

            wxWindow* Preview = GetPreviewWindow(m_CurDragPoint->Item);

            if ( Preview )
            {
                if ( NewPosX!=OldPosX || NewPosY!=OldPosY )
                {
                    if ( m_CurDragItem->GetParent() && (m_CurDragItem->GetParent()->GetType() == wxsTSizer) )
                    {
                        Props->m_Position.SetPosition(wxDefaultPosition,Preview->GetParent());
                    }
                    else
                    {
                        Props->m_Position.SetPosition(wxPoint(NewPosX,NewPosY),Preview->GetParent());
                    }
                }

                if ( NewSizeX!=OldSizeX || NewSizeY!=OldSizeY )
                {
                    Props->m_Size.SetSize(wxSize(NewSizeX,NewSizeY),Preview->GetParent());
                }
            }
        }

        m_MouseState = msIdle;
        m_Data->EndChange();
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    int DeltaY = event.GetY() - m_DragInitPosY;

    DragPointData* leftTop = m_CurDragPoint->ItemPoints[LeftTop];
    DragPointData* rightBtm = m_CurDragPoint->ItemPoints[RightBtm];

    switch ( m_CurDragPoint->Type )
    {
        case LeftTop:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case Top:
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case RightTop:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case Left:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            break;

        case Right:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            break;

        case LeftBtm:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        case Btm:
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        case RightBtm:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        default:;
    }

    int LX = leftTop->PosX;
    int LY = leftTop->PosY;
    int RX = rightBtm->PosX;
    int RY = rightBtm->PosY;

    DragPointData** ItemPoints = leftTop->ItemPoints;

    ItemPoints[Top]->PosX = (LX+RX)/2;
    ItemPoints[Top]->PosY = LY;
    ItemPoints[RightTop]->PosX = RX;
    ItemPoints[RightTop]->PosY = LY;
    ItemPoints[Left]->PosX = LX;
    ItemPoints[Left]->PosY = (LY+RY) / 2;
    ItemPoints[Right]->PosX = RX;
    ItemPoints[Right]->PosY = (LY+RY) / 2;
    ItemPoints[LeftBtm]->PosX = LX;
    ItemPoints[LeftBtm]->PosY = RY;
    ItemPoints[Btm]->PosX = (LX+RX)/2;
    ItemPoints[Btm]->PosY = RY;
    FullRepaint();
}

void wxsItemEditorContent::OnMouseDraggingItemInit(wxMouseEvent& event)
{
    BlockFetch(true);

    if ( event.RightIsDown() || event.MiddleIsDown() || !event.LeftIsDown() )
    {
        m_MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    if ( DeltaX<0 ) DeltaX = -DeltaX;
    int DeltaY = event.GetY() - m_DragInitPosY;
    if ( DeltaY<0 ) DeltaY = -DeltaY;

    if ( DeltaX + DeltaY > m_MinDragDistance )
    {
        m_MouseState = msDraggingItem;
        m_Assist->NewDragging();
        SetCur(wxCURSOR_SIZING);
    }
}

void wxsItemEditorContent::OnMouseDraggingItem(wxMouseEvent& event)
{
    if ( event.RightIsDown() || event.MiddleIsDown() )
    {
        // Cancelling change
        for ( size_t i=0; i<m_DragPoints.Count(); i++ )
        {
            m_DragPoints[i]->PosX = m_DragPoints[i]->DragInitPosX;
            m_DragPoints[i]->PosY = m_DragPoints[i]->DragInitPosY;
        }
        m_MouseState = msIdle;
        m_AssistParent = NULL;
        m_AssistTarget = NULL;
        m_AssistAddAfter = false;
        m_Assist->NewDragging();
        return;
    }

    if ( !event.LeftIsDown() )
    {
        m_Data->BeginChange();

        // Applying change
        wxsBaseProperties* Props = m_CurDragItem->GetBaseProps();
        if ( Props )
        {
            if ( m_CurDragPoint->PosX != m_CurDragPoint->DragInitPosX ||
                 m_CurDragPoint->PosY != m_CurDragPoint->DragInitPosY )
            {
                wxsParent* NewParent = NULL;
                wxsItem* AtCursor = NULL;
                bool AddAfter = true;
                if ( FindDraggingItemTarget(event.GetX(),event.GetY(),m_CurDragItem,NewParent,AtCursor,AddAfter) )
                {
                    if ( (m_CurDragItem->GetParent() == NewParent) || NewParent->CanAddChild(m_CurDragItem,false) )
                    {
                        // TODO (SpOoN#1#): Update resource tree after update

                        wxsParent* CurParent = m_CurDragItem->GetParent();

                        if ( CurParent != NewParent ||
                             NewParent->GetType() == wxsTSizer )
                        {
                            if ( AtCursor != m_CurDragItem )
                            {
                                // Storing extra data
                                int CurIndex = CurParent->GetChildIndex(m_CurDragItem);
                                TiXmlElement ExtraData("extra");
                                CurParent->StoreExtraData(CurIndex,&ExtraData);

                                // Unbinding from old parent
                                m_CurDragItem->GetParent()->UnbindChild(m_CurDragItem);

                                // Adding to new one
                                int NewIndex = -1;
                                if ( AtCursor )
                                {
                                    NewIndex = NewParent->GetChildIndex(AtCursor);
                                    if ( AddAfter ) NewIndex++;
                                }

                                NewParent->AddChild(m_CurDragItem,NewIndex);

                                // Restoring extra data
                                NewIndex = NewParent->GetChildIndex(m_CurDragItem);
                                NewParent->RestoreExtraData(NewIndex,&ExtraData);
                            }
                        }

                        wxsBaseProperties* Props = m_CurDragItem->GetBaseProps();
                        if ( Props )
                        {
                            if ( NewParent->GetType() == wxsTSizer )
                            {
                                Props->m_Position.SetPosition(wxDefaultPosition,NULL);
                            }
                            else
                            {
                                // Calculating new position
                                int PosX;
                                int PosY;
                                int SizeX;
                                int SizeY;
                                if ( FindAbsoluteRect(m_CurDragItem,PosX,PosY,SizeX,SizeY) )
                                {
                                    PosX += m_CurDragPoint->PosX - m_CurDragPoint->DragInitPosX;
                                    PosY += m_CurDragPoint->PosY - m_CurDragPoint->DragInitPosY;
                                    ClientToScreen(&PosX,&PosY);
                                    wxWindow* PreviewParent = GetPreviewWindow(m_CurDragItem)->GetParent();
                                    if ( PreviewParent )
                                    {
                                        PreviewParent->ScreenToClient(&PosX,&PosY);
                                        Props->m_Position.SetPosition(wxPoint(PosX,PosY),PreviewParent);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        m_MouseState = msIdle;
        m_AssistTarget = NULL;
        m_AssistParent = NULL;
        m_AssistAddAfter = false;
        m_Assist->NewDragging();
        m_Data->EndChange();
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    int DeltaY = event.GetY() - m_DragInitPosY;

    for ( size_t i=0; i<m_DragPoints.Count(); i++ )
    {
        m_DragPoints[i]->PosX = m_DragPoints[i]->DragInitPosX + DeltaX;
        m_DragPoints[i]->PosY = m_DragPoints[i]->DragInitPosY + DeltaY;
    }

    if ( !FindDraggingItemTarget(event.GetX(),event.GetY(),m_CurDragItem,m_AssistParent,m_AssistTarget,m_AssistAddAfter) )
    {
        m_AssistTarget = NULL;
        m_AssistParent = NULL;
        m_AssistAddAfter = false;
    }
    FullRepaint();
}

bool wxsItemEditorContent::FindDraggingItemTarget(int PosX,int PosY,wxsItem* Dragging,wxsParent*& NewParent,wxsItem*& AtCursor,bool& AddAfter)
{
    // Searching for item at cursor position
    wxsItem* Cursor = FindItemAtPos(PosX,PosY,m_Data->GetRootItem());
    if ( !Cursor ) Cursor = m_Data->GetRootItem();

    // Avoiding shifting into dragged item
    wxsParent* DraggedAsParent = Dragging->ConvertToParent();
    if ( DraggedAsParent && DraggedAsParent->IsGrandChild(Cursor) )
    {
        // Can not drag into own child
        return false;
    }

    NewParent = Cursor->ConvertToParent();

    if ( NewParent && !::wxGetKeyState(WXK_ALT) )
    {
        AtCursor = NULL;
        AddAfter = true;
        return true;
    }

    NewParent = Cursor->GetParent();
    if ( !NewParent )
    {
        // Should never be here, just in case
        return false;
    }

    if ( NewParent->GetType() == wxsTSizer )
    {
        AtCursor = Cursor;
        AddAfter = true;

        int ItemPosX;
        int ItemPosY;
        int ItemSizeX;
        int ItemSizeY;
        if ( FindAbsoluteRect(Cursor,ItemPosX,ItemPosY,ItemSizeX,ItemSizeY) )
        {
            // If cursor is on the left side, changing AddAfter flag to false
            if ( PosX < ItemPosX+(ItemSizeX/2) )
            {
                AddAfter = false;
            }
        }
    }
    else
    {
        AtCursor = NULL;
        AddAfter = true;
    }
    return true;
}

void wxsItemEditorContent::NewPreview()
{
    // Giving some time for items to recalculate positions
    Manager::Yield();
    RecalculateMaps();
    RebuildDragPoints();
    ContentChanged();
}

wxWindow* wxsItemEditorContent::GetPreviewWindow(wxsItem* Item)
{
    if ( !Item ) return NULL;
    ItemToWindowT::iterator i = m_ItemToWindow.find(Item);
    if ( i==m_ItemToWindow.end() ) return NULL;
    return (*i).second;
}

void wxsItemEditorContent::RecalculateMaps()
{
    m_ItemToRect.clear();
    m_ItemToWindow.clear();
    RecalculateMapsReq(m_Data->GetRootItem());
}

void wxsItemEditorContent::RecalculateMapsReq(wxsItem* Item)
{
    if ( Item->GetLastPreview() )
    {
        wxWindow* win = wxDynamicCast(Item->GetLastPreview(),wxWindow);
        if ( win )
        {
            m_ItemToWindow[Item] = win;
            if ( win->IsShown() )
            {
                int PosX = 0;
                int PosY = 0;
                int SizeX = 0;
                int SizeY = 0;
                win->GetPosition(&PosX,&PosY);
                win->GetParent()->ClientToScreen(&PosX,&PosY);
                ScreenToClient(&PosX,&PosY);
                CalcUnscrolledPosition(PosX,PosY,&PosX,&PosY);
                win->GetSize(&SizeX,&SizeY);
                m_ItemToRect[Item] = wxRect(PosX,PosY,SizeX,SizeY);

                wxsParent* Parent = Item->ConvertToParent();
                if ( Parent )
                {
                    for ( int i=0; i<Parent->GetChildCount(); i++ )
                    {
                        if ( Parent->IsChildPreviewVisible(Parent->GetChild(i)) )
                        {
                            RecalculateMapsReq(Parent->GetChild(i));
                        }
                    }
                }
            }
        }
    }
}
