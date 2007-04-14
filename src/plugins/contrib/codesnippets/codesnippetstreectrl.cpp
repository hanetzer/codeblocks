/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: codesnippetstreectrl.cpp 48 2007-04-14 03:18:16Z Pecan $

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
    #include <wx/msgdlg.h>
#endif
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/dataobj.h>
    #include <wx/dnd.h>

#if defined(BUILDING_PLUGIN)
    #include "sdk.h"
    #ifndef CB_PRECOMP
        #include "manager.h"
        #include "messagemanager.h"
        #include "globals.h"
    #endif
#else
#endif

#include <tinyxml/tinyxml.h>
#include "snippetitemdata.h"
#include "codesnippetstreectrl.h"
#include "snippetsconfig.h"
#include "messagebox.h"
#include "menuidentifiers.h"
#include "editsnippetdlg.h"
#include "editsnippetframe.h"


IMPLEMENT_DYNAMIC_CLASS(CodeSnippetsTreeCtrl, wxTreeCtrl)

BEGIN_EVENT_TABLE(CodeSnippetsTreeCtrl, wxTreeCtrl)
    //FIXME: EVT_TREE_ITEM_ACTIVATED(idSnippetsTreeCtrl,     CodeSnippetsTreeCtrl::OnItemActivated)
	EVT_TREE_BEGIN_DRAG(idSnippetsTreeCtrl, CodeSnippetsTreeCtrl::OnBeginTreeItemDrag)
	EVT_TREE_END_DRAG(idSnippetsTreeCtrl,   CodeSnippetsTreeCtrl::OnEndTreeItemDrag)
	EVT_LEAVE_WINDOW(                       CodeSnippetsTreeCtrl::OnLeaveWindow) //(pecan 2006/9/10)
	EVT_MOTION(                             CodeSnippetsTreeCtrl::OnMouseEvent)
	//FIXME: EVT_TREE_BEGIN_LABEL_EDIT(idSnippetsTreeCtrl,   CodeSnippetsTreeCtrl::OnBeginLabelEdit)
	//FIXME: EVT_TREE_END_LABEL_EDIT(idSnippetsTreeCtrl,     CodeSnippetsTreeCtrl::OnEndLabelEdit)
	EVT_TREE_SEL_CHANGED(idSnippetsTreeCtrl,CodeSnippetsTreeCtrl::OnItemSelected)
	//EVT_TREE_SEL_CHANGING(idSnippetsTreeCtrl,CodeSnippetsTreeCtrl::OnItemSelectChanging)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippetsTreeCtrl::CodeSnippetsTreeCtrl(wxWindow *parent, const wxWindowID id,
                                const wxPoint& pos, const wxSize& size, long style)
// ----------------------------------------------------------------------------
	: wxTreeCtrl(parent, id, pos, size, style)
{
    m_fileChanged = false;
    m_bMouseLeftWindow = false;
    m_pTopDialog = 0;

    m_pSnippetsTreeCtrl = this;
    GetConfig()->SetSnippetsTreeCtrl(this);

   #if defined(__WXGTK__) && defined(HAVEX11DEV)
    // Get XWindows info
    m_pXDisplay = 0;
    int Event, Error;
    int Major, Minor;
    m_pXDisplay = XOpenDisplay( 0 );
    // does the remote display have the Xtest-extension?
    if (m_pXDisplay){
        if ( ! XTestQueryExtension (m_pXDisplay, &Event, &Error, &Major, &Minor ) )
        {   // nope, extension not supported
            LOGIT(wxT("XTest extension not supported"));
            XCloseDisplay(m_pXDisplay);
            m_pXDisplay = 0;
        }
    }
   #endif
}

// ----------------------------------------------------------------------------
CodeSnippetsTreeCtrl::~CodeSnippetsTreeCtrl()
// ----------------------------------------------------------------------------
{
        //dtor
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnItemSelectChanging(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // UNUSED
    if (m_pTopDialog) event.Veto();
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnItemSelected(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{                                                           //(pecan 2006/9/12)

    //CodeSnippetsTreeCtrl* pTree = (CodeSnippetsTreeCtrl*)event.GetEventObject();
    wxTreeItemId itemId = event.GetItem();

    //-SetStatusText(wxEmptyString);

    // Get the item associated with the event
	if (const SnippetItemData* eventItem =
        (SnippetItemData*)GetItemData(event.GetItem()))
	{
        wxString itemDescription;
		// Check the type of the item and add the menu items
		switch (eventItem->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
			break;

			case SnippetItemData::TYPE_CATEGORY:
			break;

			case SnippetItemData::TYPE_SNIPPET:
			//-SetStatusText( GetSnippetDescription(itemId) );
			break;
		}

		// Save the item ID for later use
		m_MnuAssociatedItemID = eventItem->GetId();

	}
}

// ----------------------------------------------------------------------------
int CodeSnippetsTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
// ----------------------------------------------------------------------------
{
	// Get the items' data first
	const SnippetItemData* data1 = (SnippetItemData*)(GetItemData(item1));
	const SnippetItemData* data2 = (SnippetItemData*)(GetItemData(item2));

	if (data1 && data2)
	{
		int compareIndex1 = 0;
		switch (data1->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				compareIndex1 = 0;
			break;

			case SnippetItemData::TYPE_CATEGORY:
				compareIndex1 = 1;
			break;

			case SnippetItemData::TYPE_SNIPPET:
				compareIndex1 = 2;
			break;
		}
		int compareIndex2 = 0;
		switch (data2->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				compareIndex2 = 0;
			break;

			case SnippetItemData::TYPE_CATEGORY:
				compareIndex2 = 1;
			break;

			case SnippetItemData::TYPE_SNIPPET:
				compareIndex2 = 2;
			break;
		}

		// Compare the types
		if (compareIndex1 == compareIndex2)
		{
			// If they are both the same type, just use normal compare
			return wxTreeCtrl::OnCompareItems(item1, item2);
		}
		else if (compareIndex1 > compareIndex2)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::FindItemByLabel(const wxString& searchTerms, const wxTreeItemId& node, int requestType)
// ----------------------------------------------------------------------------
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetSnippetsTreeCtrl()->GetFirstChild(node, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		if (const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(item)))
		{
			bool ignoreThisType = false;

			switch (itemData->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					ignoreThisType = true;
				break;

				case SnippetItemData::TYPE_SNIPPET:
					if (requestType == CodeSnippetsConfig::SCOPE_CATEGORIES)
					{
						ignoreThisType = true;
					}
				break;

				case SnippetItemData::TYPE_CATEGORY:
					if (requestType == CodeSnippetsConfig::SCOPE_SNIPPETS)
					{
						ignoreThisType = true;
					}
				break;
			}

			if (!ignoreThisType)
			{
				wxString label = GetSnippetsTreeCtrl()->GetItemText(item);

				if(0 == label.Cmp(searchTerms))
				{
					return item;
				}
			}

			if(GetSnippetsTreeCtrl()->ItemHasChildren(item))
			{
				wxTreeItemId search = FindItemByLabel(searchTerms, item, requestType);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetSnippetsTreeCtrl()->GetNextChild(node, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem = (void*)(0);
   return dummyItem;
}
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::FindItemById(const wxTreeItemId& itemToFind, const wxTreeItemId& startNode, int itemToFindType)
// ----------------------------------------------------------------------------
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetSnippetsTreeCtrl()->GetFirstChild(startNode, cookie );

	// Loop through all items
	while(item.IsOk())
	{
		if (const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(item)))
		{
			bool ignoreThisType = false;

			switch (itemData->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					ignoreThisType = true;
				break;

				case SnippetItemData::TYPE_SNIPPET:
					if (itemToFindType == CodeSnippetsConfig::SCOPE_CATEGORIES)
					{
						ignoreThisType = true;
					}
				break;

				case SnippetItemData::TYPE_CATEGORY:
					if (itemToFindType == CodeSnippetsConfig::SCOPE_SNIPPETS)
					{
						ignoreThisType = true;
					}
				break;
			}

			if (!ignoreThisType)
			{
				wxString label = GetSnippetsTreeCtrl()->GetItemText(item);

				if( itemToFind == item)
				{
					return item;
				}
			}

			if(GetSnippetsTreeCtrl()->ItemHasChildren(item))
			{
				wxTreeItemId search = FindItemById(itemToFind, item, itemToFindType);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetSnippetsTreeCtrl()->GetNextChild(startNode, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem = (void*)(0);
   return dummyItem;
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveItemsToXmlNode(TiXmlNode* node, const wxTreeItemId& parentID)
// ----------------------------------------------------------------------------
{
	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(parentID, cookie);

	// Loop through all items
	while(item.IsOk())
	{
		// Get the item's information
		if (const SnippetItemData* data = (SnippetItemData*)(GetItemData(item)))
		{
			// Begin item element
			TiXmlElement element("item");

			// Write the item's name
			element.SetAttribute("name", cbU2C(GetItemText(item)));

			if (data->GetType() == SnippetItemData::TYPE_CATEGORY)
			{
				// Category
				element.SetAttribute("type", "category");

				// Check if this category has children
				if(ItemHasChildren(item))
				{
					// If it has, check those too
					SaveItemsToXmlNode(&element, item);
				}
			}
			else
			{
				// Snippet
				element.SetAttribute("type", "snippet");

				TiXmlElement snippetElement("snippet");
				TiXmlText snippetElementText(cbU2C(data->GetSnippet()));

				snippetElement.InsertEndChild(snippetElementText);

				element.InsertEndChild(snippetElement);
			}

			// Insert the item we created as parent node's child
			node->InsertEndChild(element);

			// Check the next child
			item = GetNextChild(parentID, cookie);
		}
		else
		{
			return;
		}
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::LoadItemsFromXmlNode(const TiXmlElement* node, const wxTreeItemId& parentID)
// ----------------------------------------------------------------------------
{
	for (; node; node = node->NextSiblingElement())
	{
		// Check if the node has attributes
		const wxString itemName(cbC2U(node->Attribute("name")));
		const wxString itemType(cbC2U(node->Attribute("type")));

		// Check the item type
		if (itemType == _T("category"))
		{
			// Add new category
			wxTreeItemId newCategoryId = AddCategory(parentID, itemName, false);

			// Load the child items
			if (!node->NoChildren())
			{
				LoadItemsFromXmlNode(node->FirstChildElement(), newCategoryId);
			}
		}
		else if (itemType == _T("snippet"))
		{
			// Get the snippet
			if (const TiXmlElement* snippetElement = node->FirstChildElement("snippet"))
			{
				if (const TiXmlNode* snippetElementText = snippetElement->FirstChild())
				{
					if (snippetElementText->ToText())
					{
						AddCodeSnippet(parentID, itemName, cbC2U(snippetElementText->Value()), false);
					}
				}
				else
				{
					// Create a new snippet with no code in it
					AddCodeSnippet(parentID, itemName, wxEmptyString, false);
				}
			}
			else
			{
			  #if defined(BUILDING_PLUGIN)
				Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Error loading XML file; element \"snippet\" cannot be found."));
			  #else     //NOT defined(BUILDING_PLUGIN)
			    //FIXME:
			    wxMessageBox(_T("CodeSnippets: Error loading XML file; element \"snippet\" cannot be found."));
			  #endif    //defined(BUILDING_PLUGIN)
			}
		}
		else
		{
		  #if defined(BUILDING_PLUGIN)
			// Unknown
			Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Error loading XML file; attribute \"type\" is \"") + itemType + _T("\" which is invalid item type."));
		  #else
		    //FIXME:
		    wxMessageBox(_T("CodeSnippets: Error loading XML file; attribute \"type\" is \"") + itemType + _T("\" which is invalid item type."));
          #endif
			return;
		}
	} // end for
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveItemsToFile(const wxString& fileName)
// ----------------------------------------------------------------------------
{
	TiXmlDocument doc;
	TiXmlDeclaration header("1.0", "UTF-8", "yes");
	doc.InsertEndChild(header);

	TiXmlComment comment;
	comment.SetValue("Trust me. I know what I'm doing.");
	doc.InsertEndChild(comment);

	TiXmlElement snippetsElement("snippets");
	SaveItemsToXmlNode(&snippetsElement, GetRootItem());

	doc.InsertEndChild(snippetsElement);

	doc.SaveFile(fileName.mb_str());
	SetFileChanged(false);
	SaveFileModificationTime();

    #ifdef LOGGING
     LOGIT( _T("File saved:[%s]"),fileName.c_str() );
    #endif //LOGGING
}

// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::LoadItemsFromFile(const wxString& fileName, bool bAppendItems)
// ----------------------------------------------------------------------------
{
	if (!bAppendItems)
        DeleteChildren( GetRootItem() );

    bool retcode = true;
	if (wxFileExists(fileName))
	{
		// Don't remove any whitespace
		TiXmlBase::SetCondenseWhiteSpace(false);

		TiXmlDocument doc;

		if (doc.LoadFile(fileName.mb_str()))
		{
			TiXmlElement* root = doc.RootElement();
			if (root)
			{
				// Get the first element
				TiXmlElement* firstChild = root->FirstChildElement("item");
				if (firstChild)
				{
					LoadItemsFromXmlNode(firstChild, GetRootItem());
				}
			}
		}
		else
		{
		    retcode = false;
			// Create a backup copy of the offending file
			wxString backupFile = fileName;
			backupFile.Append(_T(".bak"));

			// Overwrite it
			wxCopyFile(fileName, backupFile, true);
           #if defined(BUILDING_PLUGIN)
			Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + cbC2U(doc.ErrorDesc()));
			Manager::Get()->GetMessageManager()->DebugLog(_T("CodeSnippets: Backup of the failed file has been created."));
		   #else
            //-wxMessageBox(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + cbC2U(doc.ErrorDesc()));
            messageBox(_T("CodeSnippets: Cannot load file \"") + fileName + _T("\": ") + cbC2U(doc.ErrorDesc()));
   			//-wxMessageBox(_T("CodeSnippets: Backup of the failed file has been created."));
   			messageBox(_T("CodeSnippets: Backup of the failed file has been created."));
		   #endif
		}
	}
	// Show the first level of items
	if (  GetRootItem() && GetRootItem().IsOk())
        Expand( GetRootItem() );

    SetFileChanged(false);
    SaveFileModificationTime();
    return retcode;

}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::AddCodeSnippet(const wxTreeItemId& parent,
                            wxString title, wxString codeSnippet, bool editNow)
// ----------------------------------------------------------------------------
{
	wxTreeItemId newItemID = InsertItem(parent, GetLastChild(parent), title, 2, -1,
                new SnippetItemData(SnippetItemData::TYPE_SNIPPET, codeSnippet));

	// Sort 'em
	SortChildren(parent);

	if (editNow)
	{
		// Let the user to edit the item
		EnsureVisible(newItemID);
		EditLabel(newItemID);
	}

	if ( newItemID.IsOk() )
        SetSnippetImage(newItemID);

    SetFileChanged(true);

}

// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::AddCategory(const wxTreeItemId& parent, wxString title, bool editNow)
// ----------------------------------------------------------------------------
{
	wxTreeItemId newCategoryID = InsertItem(parent, GetLastChild(parent), title, 1, -1, new SnippetItemData(SnippetItemData::TYPE_CATEGORY));

	// Sort 'em
	SortChildren(parent);

	if (editNow)
	{
		// Let the user edit the category
		EnsureVisible(newCategoryID);
		EditLabel(newCategoryID);
	}

	return newCategoryID;
}
// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::RemoveItem(wxTreeItemId itemId)
// ----------------------------------------------------------------------------
{
	// Get the associated item id
	wxTreeItemId itemID = itemId;
    SnippetItemData* pItemData = (SnippetItemData*)(GetItemData(itemId));

    // put deleted items in .trash category
    wxTreeItemId trashId = FindItemByLabel(wxT(".trash"), GetRootItem(), CodeSnippetsConfig::SCOPE_CATEGORIES);
    if ( trashId==(void*)0 )
        trashId = AddCategory(GetRootItem(), wxT(".trash"), false);
    // if item is NOT already in the trash, copy item to .trash item
    if (not ( itemID == FindItemById( itemId, trashId, pItemData->GetType()) ))
    {
        TiXmlDocument* pDoc =  CopyTreeNodeToXmlDoc( itemID);
        CopyXmlDocToTreeNode(pDoc, trashId);
        delete pDoc;
    }
	// Sanity check
	if (itemID != GetRootItem() )
	{
		// No questions asked
		DeleteChildren(itemID);
		Delete(itemID);
        SetFileChanged(true);
		return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SetSnippetImage(wxTreeItemId itemId)
// ----------------------------------------------------------------------------
{
    // set the item tree image
    if ( IsFileSnippet(itemId) )
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_FILE);
    else
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_TEXT);
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveFileModificationTime(wxDateTime savedTime)
// ----------------------------------------------------------------------------
{
    if (savedTime != time_t(0))
    {   m_LastXmlModifiedTime = savedTime;
        return;
    }
    if (::wxFileExists(GetConfig()->SettingsSnippetsXmlFullPath) )
    {   wxFileName fname( GetConfig()->SettingsSnippetsXmlFullPath );
        m_LastXmlModifiedTime = fname.GetModificationTime();
    }
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnBeginTreeItemDrag(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // -----------------------
    // TREE_BEGIN_DRAG
    // -----------------------
    CodeSnippetsTreeCtrl* pTree = (CodeSnippetsTreeCtrl*)event.GetEventObject();

    //#ifdef LOGGING
    //	 LOGIT( wxT("ScrapList::OnTreeCtrlEvent %p"), pTree );
    //#endif //LOGGING

    #ifdef LOGGING
     LOGIT( _T("TREE_BEGIN_DRAG %p"), pTree );
    #endif //LOGGING
    // On MSW the current selection may not be the same as the current itemId
    // If the user just clicks and drags, the two are different
    m_pEvtTreeCtrlBeginDrag = pTree;
    m_TreeItemId        = event.GetItem();
        // At this point we could solve the above problem with
        // pTree->SelectItem(m_TreeItemId) ; But for now, we'll just
        // record the actual current itemId.
    m_MnuAssociatedItemID = m_TreeItemId;
    m_TreeMousePosn       = ::wxGetMousePosition();
    m_TreeText            = pTree->GetSnippet(m_TreeItemId);
    if (m_TreeText.IsEmpty())
        m_pEvtTreeCtrlBeginDrag = 0;

    event.Allow();

    // -----------------------------------------
    // Do *not* event.Skip() or GTK will break
    //event.Skip();
    // -----------------------------------------

    return;
}//OnBeginTreeItemDrag
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnEndTreeItemDrag(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // -----------------------
    // TREE_END_DRAG
    // -----------------------


    #ifdef LOGGING
     wxTreeCtrl* pTree = (wxTreeCtrl*)event.GetEventObject();
     LOGIT( _T("TREE_END_DRAG %p"), pTree );
    #endif //LOGGING

    wxTreeItemId targetItem = (wxTreeItemId)event.GetItem();
    wxTreeItemId sourceItem = m_MnuAssociatedItemID;

    // veto the drag if mouse has moved out of the Tree window
        // Note: Even if mouse is dragged out of the tree window,
        //      FindFocus() is returning the Tree window. So the
        //      following test does not work.
        //if (pTree == wxWindow::FindFocus() )
        //    event.Allow();
        //else return;

    // If user dragged item out of the window, it'll be dropped by the
    // target application. So just clear the status and return.
    if (m_bMouseLeftWindow)
    {   // user dragged item out of the window
        m_bMouseLeftWindow = false;
        return;
    }

    event.Allow();
    // if source and target are snippets, create a new category and enclose both.
    if ( IsSnippet(targetItem) )
    {
        if ( targetItem = ConvertSnippetToCategory(targetItem)) {;}//ok
        else return;
        // now targetItem is a category
    }


    // Save the source item node to an Xml Document
    // Load it into the target item node
    // delete the source item node

    // create Xml document from source(dragged) tree item
    TiXmlDocument* pDoc = CopyTreeNodeToXmlDoc( sourceItem );
    if (not pDoc) return;

    TiXmlElement* root = pDoc->RootElement();
    if (root)
    {
        // Get the source xml element
        TiXmlElement* firstChild = root->FirstChildElement("item");
        if (firstChild)
        {   // insert into target Tree items from source xml document
            LoadItemsFromXmlNode( firstChild, targetItem);
        }
    }
    // remove the old tree item
    if (not m_MouseCtrlKeyDown){
            RemoveItem( sourceItem );
    }

    delete pDoc; pDoc = 0;

    // -----------------------------------------
    // Do *not* event.Skip() or GTK will break
    //event.Skip();
    // -----------------------------------------

    return;

}//OnEndTreeItemDrag
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnLeaveWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    // unless we can compile with the X11/Xtst header we cannot
    // allow dragging a tree item out of the treeCtrl. It really
    // messes up the tree mouse navigation
    #if defined(__WXGTK__) && !defined(HAVEX11DEV)
     event.Skip(); return;
    #endif

    // -----------------------
    // LEAVE_WINDOW
    // -----------------------

    // User has dragged mouse out of source window.
    // if EVT_TREE_BEGIN_DRAG is pending, create a drag source to be used
    // in the destination window.

    #ifdef LOGGING
     //LOGIT( _T("MOUSE EVT_LEAVE_WINDOW") );
    #endif //LOGGING

    // Left mouse key must be down (dragging)
    if (not event.LeftIsDown() ) {event.Skip();return;}
    // check if data is available
    if ( m_TreeText.IsEmpty()) {event.Skip();return;}
    if (not m_pEvtTreeCtrlBeginDrag) {event.Skip(); return;}

    #ifdef LOGGING
     LOGIT( _T("LEAVE_WINDOW %p"), event.GetEventObject() );
    #endif //LOGGING

    // when user drags an item out of the window, this routine is called
    // before EVT_END_DRAG, who will clear this flag
    m_bMouseLeftWindow = true;

    // we now have data, create both a simple text and filename drop source
    wxTextDataObject* textData = new wxTextDataObject();
    wxFileDataObject* fileData = new wxFileDataObject();
        // fill text and file sources with snippet
    wxDropSource textSource( *textData, (wxWindow*)event.GetEventObject() );
    textData->SetText( GetSnippet(m_MnuAssociatedItemID) );
    wxDropSource fileSource( *fileData, (wxWindow*)event.GetEventObject() );
    fileData->AddFile( (GetSnippet(m_MnuAssociatedItemID).Len() > 128) ?
              wxString(wxEmptyString) : GetSnippet(m_MnuAssociatedItemID) );
        // set composite data object to contain both text and file data
    wxDataObjectComposite *data = new wxDataObjectComposite();
    data->Add( (wxDataObjectSimple*)textData );
    data->Add( (wxDataObjectSimple*)fileData, true ); // set file data as preferred
        // create the drop source containing both data types
    wxDropSource source( *data, (wxWindow*)event.GetEventObject()  );

    #ifdef LOGGING
     LOGIT( _T("DropSource Text[%s],File[%s]"),
                textData->GetText().GetData(),
                fileData->GetFilenames().Item(0).GetData() );
    #endif //LOGGING
        // allow both copy and move
    int flags = 0;
    flags |= wxDrag_AllowMove;
    // do the dragNdrop
    wxDragResult result = source.DoDragDrop(flags);
    // report the results
    #if LOGGING
        wxString pc;
        switch ( result )
        {
            case wxDragError:   pc = _T("Error!");    break;
            case wxDragNone:    pc = _T("Nothing");   break;
            case wxDragCopy:    pc = _T("Copied");    break;
            case wxDragMove:    pc = _T("Moved");     break;
            case wxDragCancel:  pc = _T("Cancelled"); break;
            default:            pc = _T("Huh?");      break;
        }
        LOGIT( wxT("ScrapList::OnLeftDown DoDragDrop returned[%s]"),pc.GetData() );
    #else
        wxUnusedVar(result);
    #endif // wxUSE_STATUSBAR

    // ---WORKAROUNG --------------------------------------------------
    // Since we dragged outside the tree control with an EVT_TREE_DRAG_BEGIN
    // pending, WX will not send the EVT_TREE_DRAG_END from a
    // mouse_key_up event unless the user re-clicks inside the tree control.
    // The mouse is still captured and the tree exibits very bad behavior.
    // GTK does not have this problem.
    //2007/01/6 This *is* happening on andLinux

    // To solve this, send an mouse_key_up to the tree control so it
    // releases the mouse and receives an EVT_TREE_DRAG_END event.

    if ( m_pEvtTreeCtrlBeginDrag )
    {   //send Mouse LeftKeyUp to Tree Control window
        #ifdef LOGGING
         //LOGIT( _T("Sending Mouse LeftKeyUp") );
        #endif //LOGGING

        // move mouse into the Tree control
        wxPoint CurrentMousePosn = ::wxGetMousePosition();
      #if defined(__WXMSW__)
        MSW_MouseMove( m_TreeMousePosn.x, m_TreeMousePosn.y );
        // send mouse LeftKeyUp
        INPUT    Input={0};
        Input.type          = INPUT_MOUSE;
        Input.mi.dwFlags    = MOUSEEVENTF_LEFTUP;
        ::SendInput(1,&Input,sizeof(INPUT));
        // put mouse back in pre-moved "dropped" position
        MSW_MouseMove( CurrentMousePosn.x, CurrentMousePosn.y );
      #endif //(__WXMSW__)
      #if defined(__WXGTK__) && defined(HAVEX11DEV)
        if (m_pXDisplay)
        {   // move cursor to window and send a left button up event
            XTestFakeMotionEvent(m_pXDisplay, -1, m_TreeMousePosn.x, m_TreeMousePosn.y, 0);
            XTestFakeButtonEvent( m_pXDisplay, 1, false, 0);
            // put mouse back in pre-moved "dropped" position
            XTestFakeMotionEvent( m_pXDisplay, -1, CurrentMousePosn.x, CurrentMousePosn.y, 0 );
            XFlush(m_pXDisplay);
        }
      #endif//(__WXGTK__)&&(HAVEX11DEV)
    }

    delete textData; //wxTextDataObject
    delete fileData; //wxFileDataObject
    m_pEvtTreeCtrlBeginDrag = 0;
    m_TreeText = wxEmptyString;

    event.Skip();
    return;
}//OnLeaveWindow
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnMouseEvent(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    //remember event window pointer
    //wxObject* m_pEvtObject = event.GetEventObject();

    // memorize position of the mouse ctrl key as copy/delete flag
    m_MouseCtrlKeyDown = event.ControlDown();
    #ifdef LOGGING
     //LOGIT(wxT("MouseCtrlKeyDown[%s]"), m_MouseCtrlKeyDown?wxT("Down"):wxT("UP") );
    #endif
    event.Skip();

}
// ----------------------------------------------------------------------------
#if defined(__WXMSW__)
void CodeSnippetsTreeCtrl::MSW_MouseMove(int x, int y )
// ----------------------------------------------------------------------------
{
    // Move mouse uses a very strange coordinate system.
    // It uses screen positions with a range of 0 to 65535

    // Move the MSW mouse to absolute screen coords x,y
      double fScreenWidth   = ::GetSystemMetrics( SM_CXSCREEN )-1;
      double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1;
      double fx = x*(65535.0f/fScreenWidth);
      double fy = y*(65535.0f/fScreenHeight);
      INPUT  Input={0};
      Input.type      = INPUT_MOUSE;
      Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
      Input.mi.dx = (LONG)fx;
      Input.mi.dy = (LONG)fy;
      ::SendInput(1,&Input,sizeof(INPUT));
}
#endif
// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsTreeCtrl::ConvertSnippetToCategory(wxTreeItemId itemId)
// ----------------------------------------------------------------------------
{
    // Convert Snippet to Category, then insert the old snippet
    wxTreeItemId badItemId = (void*)0;

    if ( not IsSnippet(itemId)) return badItemId;
        // memorize id of this snippet
    wxTreeItemId oldItemId = itemId;
    if ( not oldItemId.IsOk() ) return badItemId;
        // memorize parent id of this snippet
    wxTreeItemId itemParent = GetItemParent(oldItemId);

        // create Xml document from selected tree item
    TiXmlDocument* pDoc = CopyTreeNodeToXmlDoc( oldItemId );
    if (not pDoc) return badItemId;

    // Create new Category
    wxTreeItemId newCategoryId = AddCategory( itemParent,
                GetItemText(oldItemId), false );

    // Insert old Snippet Item under new Category
    TiXmlElement* root = pDoc->RootElement();
    if (root)
    {
            // Get the first xml element
        TiXmlElement* firstChild = root->FirstChildElement("item");
        if (firstChild)
        {   // insert Tree items from xml document
            //-LoadSnippets(firstChild, newCategoryId);
            LoadItemsFromXmlNode( firstChild, newCategoryId);

        }
    }
        // remove the old tree item
    RemoveItem( oldItemId );
    delete pDoc; pDoc = 0;
    return newCategoryId;
}
// ----------------------------------------------------------------------------
TiXmlDocument* CodeSnippetsTreeCtrl::CopyTreeNodeToXmlDoc(wxTreeItemId TreeItemId  )
// ----------------------------------------------------------------------------
{
    // Copy a Tree node to an Xml Document

    wxTreeItemId itemId = TreeItemId;
    if (not itemId.IsOk() )
    {
    	itemId = GetSelection();
    }
    if (not itemId.IsOk()) return NULL;

	TiXmlDocument* pDoc = new TiXmlDocument;
	TiXmlDeclaration header("1.0", "UTF-8", "yes");
	pDoc->InsertEndChild(header);

	TiXmlElement snippetsElement("snippets");
	CopySnippetsToXmlDoc(&snippetsElement, itemId);

	pDoc->InsertEndChild(snippetsElement);

	return pDoc;

} // end of SaveSnippetsToFile
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::CopySnippetsToXmlDoc(TiXmlNode* Node, const wxTreeItemId& itemID)
// ----------------------------------------------------------------------------
{
    // Recursively copy Tree item and all its children to an Xml document node

	wxTreeItemIdValue cookie;
	//wxTreeItemId item = GetSnippetsTreeCtrl->GetFirstChild(parentID, cookie );
    wxTreeItemId item = itemID;

	// Loop through all items
	while(item.IsOk())
	{
		// Get the item's information
		const SnippetItemData* data = (SnippetItemData*)GetItemData(item);

		if (!data)
		{
			return;
		}

		// Begin item element
		TiXmlElement element("item");

		// Write the item's name
		element.SetAttribute("name", GetItemText(item).mb_str());

		// Write the type of the item
		switch (data->GetType())
		{
			case SnippetItemData::TYPE_CATEGORY:
				element.SetAttribute("type", "category");
			break;

			case SnippetItemData::TYPE_SNIPPET:
				element.SetAttribute("type", "snippet");
			break;

			default:
			break;
		}

		// And the snippet
		if (data->GetType() == SnippetItemData::TYPE_SNIPPET)
		{
			TiXmlElement snippetElement("snippet");
			TiXmlText snippetElementText(data->GetSnippet().mb_str());
            snippetElement.InsertEndChild(snippetElementText);
			element.InsertEndChild(snippetElement);
			//#ifdef LOGGING
			// LOGIT( _T("Snippet[%s]"), data->GetSnippet().GetData() );
			//#endif //LOGGING
		}

		// Check if this item has children
		if(ItemHasChildren(item))
		{
			// If it has, check those too
			//-SaveSnippets(&element, item);
			SaveItemsToXmlNode(&element, item);
		}

		// Insert the item we created as parent node's child
		//parentNode->InsertEndChild(element);
		Node->InsertEndChild(element);

		// Check the next child
		item = GetNextChild( itemID, cookie);
	}
}//CopySnippetsToXmlDoc
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::CopyXmlDocToTreeNode(TiXmlDocument* pTiXmlDoc, wxTreeItemId targetItem  )
// ----------------------------------------------------------------------------
{
    TiXmlElement* root = pTiXmlDoc->RootElement();
    if (root)
    {
        // Get the source xml element
        TiXmlElement* firstChild = root->FirstChildElement("item");
        if (firstChild)
        {   // insert into target Tree items from source xml document
            LoadItemsFromXmlNode( firstChild, targetItem);
        }
    }

}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippetAsFileLink()
// ----------------------------------------------------------------------------
{
    // Open snippet text as a file name. Ie, the text should contain a filename.
    // Else just open a temp file with the snippet text as data.

    if (not IsSnippet() ) return;

	// If snippet is file, open it
	wxTreeItemId itemId = GetAssociatedItemID();
	wxString FileName = GetSnippet( itemId );
	SnippetItemData* pSnippetItemData = (SnippetItemData*)GetItemData(GetAssociatedItemID());

    if (FileName.Length() > 128)
    {   // if text is > 128 characters, open a temp file with snippet text as data.
        EditSnippetAsText();
        return;
    }
	if ( (FileName.IsEmpty())
        || (not ::wxFileExists( FileName)) )
    {   // if, non-existent file, open snippet text as data
        EditSnippetAsText();
        return;
    }

    // we have an actual file name, not just text.
    // use user specified editor, else hard coded pgms.
    wxString pgmName = GetConfig()->SettingsExternalEditor;
    if ( pgmName.IsEmpty() || (not ::wxFileExists(pgmName)) )
    {
        //        #if defined(__WXMSW__)
        //                pgmName = wxT("notepad");
        //        #elif defined(__UNIX__)
        //                pgmName = wxT("xterm -e less");
        //        #endif
        EditSnippet( pSnippetItemData, FileName);
    }
    else    // edit file with external program
    {   // file name must be surrounded with quotes when using wxExecute
        wxString execString = pgmName + wxT(" \"") + FileName + wxT("\"");
        #ifdef LOGGING
         LOGIT( _T("OpenAsFileLink[%s]"), execString.GetData() );
        #endif //LOGGING
        ::wxExecute( execString);
    }
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippetAsText()
// ----------------------------------------------------------------------------
{

	SnippetItemData* pSnippetItemData = (SnippetItemData*)GetItemData(GetAssociatedItemID());

    // if no user specified editor, use default editor
    wxString editorName = GetConfig()->SettingsExternalEditor ;
    if ( editorName.IsEmpty() || (not ::wxFileExists(editorName)) )
    {
        EditSnippet( pSnippetItemData );
        return;
    }

    if ( editorName.IsEmpty() || (not ::wxFileExists(editorName)) )
    {
        #if defined(__WXMSW__)
                editorName = wxT("notepad");
        #elif defined(__UNIX__)
                editorName = wxT("vi");
        #endif
        wxString msg(wxT("Using default editor: ")+editorName+wxT("\n"));
        if (GetConfig()->IsApplication() ) msg = msg + wxT("Use Menu->");
        else msg = msg + wxT("Right click Root item. Use ");
        msg = msg + wxT("Settings to set a better editor.\n");
        messageBox( msg );
    }

    // let user edit the snippet text
    // write text to temp file
    // invoke the editor
    // read text back into snippet

    wxFileName tmpFileName = wxFileName::CreateTempFileName(wxEmptyString);

    wxFile tmpFile( tmpFileName.GetFullPath(), wxFile::write);
    if (not tmpFile.IsOpened() )
    {
        messageBox(wxT("Open failed for:")+tmpFileName.GetFullPath());
        return ;
    }
    wxString snippetData( GetSnippet() );
    tmpFile.Write( cbU2C(snippetData), snippetData.Length());
    tmpFile.Close();
        // Invoke the external editor on the temporary file
        // file name must be surrounded with quotes when using wxExecute
    wxString execString = editorName + wxT(" \"") + tmpFileName.GetFullPath() + wxT("\"");

    #ifdef LOGGING
     LOGIT( _T("Properties wxExecute[%s]"), execString.GetData() );
    #endif //LOGGING

        // Invoke the external editor and wait for its termination
    ::wxExecute( execString, wxEXEC_SYNC);
        // Read the edited data back into the snippet text
    tmpFile.Open(tmpFileName.GetFullPath(), wxFile::read);
    if (not tmpFile.IsOpened() )
    {   messageBox(wxT("Abort.Error reading temp data file."));
        return;
    }
    unsigned long fileSize = tmpFile.Length();

    #ifdef LOGGING
     LOGIT( _T("New file size[%d]"),fileSize );
    #endif //LOGGING

    // check the data for validity
    char pBuf[fileSize+1];
    size_t nResult = tmpFile.Read( pBuf, fileSize );
    if ( wxInvalidOffset == (int)nResult )
        messageBox(wxT("Error reading temp file"));
    pBuf[fileSize] = 0;
    tmpFile.Close();

    #ifdef LOGGING
      //LOGIT( _T("pBuf[%s]"), pBuf );
    #endif //LOGGING

        // convert data back to internal format
    snippetData = cbC2U( pBuf );

     #ifdef LOGGING
      //LOGIT( _T("snippetData[%s]"), snippetData.GetData() );
     #endif //LOGGING

        // delete the temporary file
    ::wxRemoveFile( tmpFileName.GetFullPath() );

        // update Tree item
    SetSnippet( snippetData );

    return;
}//EditSnippetAsText
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::SaveSnippetAsFileLink()
// ----------------------------------------------------------------------------
{
    wxTreeItemId itemId = GetAssociatedItemID();
    if (not itemId.IsOk()) return;

    if ( not IsSnippet()) return;

    // Dump the snippet text to file, then place the filename in the snippet text area

    // Dump Snippet field into a temporary file
    wxString snippetLabel = GetSnippetLabel();
    wxString snippetData = GetSnippet();
    int answer = wxYES;
    // if file already exists preserve the old data
    if ( ::wxFileExists( snippetData) )
    {   // item snippet is already a filename
        answer = messageBox(
            wxT("Item is already a file link named:")+snippetData
                + wxT(" \nAre you sure you want to rewrite the file?"),
            wxT("Warning"),wxYES|wxNO); //, GetMainFrame(), mousePosn.x, mousePosn.y);
        if ( wxYES == answer)
        {   // read data from old file
            wxFile oldFile( snippetData, wxFile::read);
            if (not oldFile.IsOpened() )
            {   messageBox(wxT("Abort.Error reading data file."));
                return;
            }
            unsigned long fileSize = oldFile.Length();
            char* pBuf = new char[fileSize+1];
            oldFile.Read( pBuf, fileSize );
            pBuf[fileSize] = 0;
            snippetData = cbC2U(  pBuf );
            oldFile.Close();
            delete [] pBuf;
        }
    }
    if ( wxNO == answer ) return;

    // filter filename, removing all illegal filename characters
    wxString newFileName = snippetLabel+wxT(".txt");
    wxFileName snippetFileName( newFileName) ;
    //newFileName = snippetFileName.GetFullName();
    wxString forbidden = snippetFileName.GetForbiddenChars();
    for (size_t i=0; i < forbidden.Length(); ++i)
        newFileName.Replace( wxString(forbidden[i]), wxT(""),true);
        //#ifdef LOGGING
        // LOGIT( _T("forbidden[%s],filename[%s]"), forbidden.GetData(),newFileName.GetData());
        //#endif //LOGGING

    // Ask user for filename
    wxFileDialog dlg(this,                      //parent  window
                 _("Save as text file"),        //message
                 wxEmptyString,                 //default directory
                 newFileName,                   //default file
                 wxT("*.*"),                    //wildcards
                 wxSAVE | wxOVERWRITE_PROMPT);  //style
    // move dialog into the parents frame space
    wxPoint mousePosn = ::wxGetMousePosition();
    (&dlg)->Move(mousePosn.x, mousePosn.y);
    if (dlg.ShowModal() != wxID_OK) return;

    newFileName = dlg.GetPath();

    #ifdef LOGGING
     LOGIT( _T("New filename[%s]"), newFileName.GetData() );
    #endif //LOGGING;

    if ( newFileName.IsEmpty() ) return;
    // Verify filename, or create
    wxFile newFile( newFileName, wxFile::write);
    if (not newFile.IsOpened() )
    {
        messageBox(wxT("Open failed for:")+newFileName);
        return ;
    }
    newFile.Write( cbU2C(snippetData), snippetData.Length());
    newFile.Close();
    // update Tree item
    SetSnippet( newFileName );

    // verify the item tree image
    if ( IsFileSnippet() )
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_FILE);
    else
        SetItemImage( itemId, TREE_IMAGE_SNIPPET_TEXT);

    return;
}//OnMnuSaveAsFile
// ----------------------------------------------------------------------------
bool CodeSnippetsTreeCtrl::EditSnippetProperties(wxTreeItemId& itemId)
// ----------------------------------------------------------------------------
{
    if ( not IsSnippet(itemId) ) return false;

    int result = 0;
    wxSemaphore waitSem;
    SnippetProperty* pdlg = new SnippetProperty(GetSnippetsTreeCtrl(), itemId, &waitSem);
    result = ExecuteDialog(pdlg, waitSem);
    // Save any changed data
	if ( result == wxID_OK )
	{
        // verify the item tree image
        SetSnippetImage(itemId);
	}
    if (result == wxID_OK) {
        SetFileChanged(true);
    }
    pdlg->Destroy();
    return (result = (result==wxID_OK));
}

// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::EditSnippet(SnippetItemData* pSnippetItemData, wxString fileName)
// ----------------------------------------------------------------------------
{
    if (SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(GetAssociatedItemID() ))) 	{
        // Wait for the non-modal edit dialog to finish
        int retcode = 0;
        wxSemaphore waitSem;
//        EditSnippetDlg* pdlg = new EditSnippetDlg(GetSnippetsTreeCtrl()->GetItemText(GetAssociatedItemID()),
//                                        itemData->GetSnippet(), &waitSem, &retcode, fileName);
        wxString snippetText = itemData->GetSnippet();
        EditSnippetFrame* pdlg = new EditSnippetFrame(GetSnippetsTreeCtrl()->GetItemText(GetAssociatedItemID()),
                                        &snippetText, &waitSem, &retcode, fileName);
        retcode = ExecuteFrame( pdlg, waitSem, retcode);
        // Save any changed data
		if (retcode == wxID_OK)
		{
            // If XML text snippet, just save back to XML file
            if (fileName.IsEmpty())
            {
                itemData->SetSnippet(pdlg->GetText());
                GetSnippetsTreeCtrl()->SetItemText(GetAssociatedItemID(), pdlg->GetName());
            }
            else //This was an external file
            {
                ;// Modified external files already saved by dialog
            }
			SetFileChanged(true);
		}
        pdlg->Destroy();
	}
}
// ----------------------------------------------------------------------------
int CodeSnippetsTreeCtrl::ExecuteFrame(wxFrame* pdlg, wxSemaphore& waitSem, int& retcode)
// ----------------------------------------------------------------------------
{
        m_pTopDialog = (wxDialog*)pdlg;
        retcode = 0;

        //- Disable the parent for user input until the edit dialog returns
        //- This also causes scintilla's cursor to disappear until moving the mouse
        //- from an enabled window back to scintilla
        //-this->GetParent()->Disable();

    //LOGIT( _T("ExecuteFrame") );
    wxWindow* pw = this;
    //LOGIT( _T("this[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
    if (pw && pw->GetParent()) //
    {   pw = pw->GetParent();
        //LOGIT( _T("parent1[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
    }
    if (pw && pw->GetParent())  //This is the SnippetWindow parent
    {   pw = pw->GetParent();
        //LOGIT( _T("parent2[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
    }
    //    if (pw && pw->GetParent())  //Snippet tree frame
    //    {   pw = pw->GetParent();
    //        LOGIT( _T("parent3[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
    //    }
    //    if (pw && pw->GetParent())  //
    //    {   pw = pw->GetParent();
    //        LOGIT( _T("parent5[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
    //    }

        // Grab CodeBlocks close function so dlg isn't orphaned|crashed on close)
        GetConfig()->GetMainFrame()->Connect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown,NULL,this);
        // Grab parents close function so dlg isn't orphaned|crashed on close)
        pw->Connect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown,NULL,this);

        #if defined(BUILDING_PLUGIN)
          // Disable the plugin View menu item
            Manager::Get()->GetAppWindow()->GetMenuBar()->Enable(idViewSnippets, false);
        #endif

        if ( pdlg->Show() )
        {
            // Just check to see if the semaphore has been posted.
            // Don't do a real wait, else the edit dialog will freeze
            while( wxSEMA_BUSY == waitSem.TryWait())
            {   waitSem.WaitTimeout(20);
                wxYield();
            }
            // retcode set by return frame
            //-retcode = pdlg->GetReturnCode();
        }
        // Release CodeBlocks closeWindow function
        GetConfig()->GetMainFrame()->Disconnect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown);
        // Release parents closeWindow function
        pw->Disconnect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown);

        #if defined(BUILDING_PLUGIN)
          // Enable the plugin View menu item
          Manager::Get()->GetAppWindow()->GetMenuBar()->Enable(idViewSnippets, true);
        #endif

		//- re-enable the tree for user input
        //-this->GetParent()->Enable(true);

        m_pTopDialog = 0;
         LOGIT( _T("ExecuteFrame RetCode[%s]"), (retcode == wxID_OK)? wxT("OK"):wxT("Cancel") );
        return retcode;
}

// ----------------------------------------------------------------------------
int CodeSnippetsTreeCtrl::ExecuteDialog(wxDialog* pdlg, wxSemaphore& waitSem)
// ----------------------------------------------------------------------------
{
    m_pTopDialog = pdlg;
    int retcode = 0;

    wxWindow* pw = this;
    if (pw && pw->GetParent()) //
    {   pw = pw->GetParent();
    }
    if (pw && pw->GetParent())  //This is the SnippetWindow parent
    {   pw = pw->GetParent();
    }

    // Grab main apps close function so dlg isn't orphaned|crashed on close
    GetConfig()->GetMainFrame()->Connect( wxEVT_CLOSE_WINDOW,
        (wxObjectEventFunction)(wxEventFunction)
        (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown,NULL,this);

    // Grab parents close function so dlg isn't orphaned|crashed on close)
    pw->Connect( wxEVT_CLOSE_WINDOW,
        (wxObjectEventFunction)(wxEventFunction)
        (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown,NULL,this);

        #if defined(BUILDING_PLUGIN)
          // Disable the plugin View menu item
            Manager::Get()->GetAppWindow()->GetMenuBar()->Enable(idViewSnippets, false);
        #endif

        if ( pdlg->Show() )
        {
            // Just check to see if the semaphore has been posted.
            // Don't do a real wait, else the edit dialog will freeze
            while( wxSEMA_BUSY == waitSem.TryWait())
            {   waitSem.WaitTimeout(20);
                wxYield();
            }
            retcode = pdlg->GetReturnCode();
        }
        // Release main apps closeWindow function
        GetConfig()->GetMainFrame()->Disconnect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown);

        // Release parents closeWindow function
        pw->Disconnect( wxEVT_CLOSE_WINDOW,
            (wxObjectEventFunction)(wxEventFunction)
            (wxCloseEventFunction) &CodeSnippetsTreeCtrl::OnShutdown);

        #if defined(BUILDING_PLUGIN)
          // Enable the plugin View menu item
          Manager::Get()->GetAppWindow()->GetMenuBar()->Enable(idViewSnippets, true);
        #endif

        m_pTopDialog = 0;

        return retcode;
}
// ----------------------------------------------------------------------------
void CodeSnippetsTreeCtrl::OnShutdown(wxCloseEvent& event)
// ----------------------------------------------------------------------------
{
    // Here because our Connect() intercepted wxTheApp EVT_CLOSE
    // Blink this modeless dialog just like it was a modal dialog
    wxWindow* oldTop = wxTheApp->GetTopWindow();
    wxDialog* pdlg = this->m_pTopDialog;
    wxTheApp->SetTopWindow( pdlg );
    pdlg->RequestUserAttention();
    wxTheApp->SetTopWindow(oldTop);
    event.Veto();
    //event.Skip(); causes app to crash
    return;
}
// ----------------------------------------------------------------------------
