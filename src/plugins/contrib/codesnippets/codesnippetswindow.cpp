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
// RCS-ID: $Id: codesnippetswindow.cpp 32 2007-04-02 17:51:48Z Pecan $

#ifdef WX_PRECOMP //
    #include "wx_pch.h"
#else
    #include <wx/button.h>
    #include <wx/imaglist.h>
    #include <wx/intl.h>
    #include <wx/sizer.h>
    #include <wx/textctrl.h>
    #include <wx/menu.h>
    #include <wx/textdlg.h>
    #include <wx/colour.h>
    #include <wx/filedlg.h>
    #include <wx/tokenzr.h>
#endif
    #include <wx/filename.h>
    #include <wx/toplevel.h>

// wxWidget headers not include in wx_pch.h
    #include <wx/clipbrd.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#if defined(BUILDING_PLUGIN)
    #if defined(CB_PRECOMP)
        #include "sdk.h"
    #else
        #include "manager.h"
        #include "messagemanager.h"
        #include "macrosmanager.h"
        #include "configmanager.h"
        #include "editormanager.h"
        #include "cbeditor.h"
        #include "globals.h"
    #endif
#endif

// project headers
#include "version.h"
#include "codesnippetswindow.h"
#include "snippetitemdata.h"
//#include "editsnippetdlg.h"
#include <tinyxml/tinyxml.h>
#include "snippetsconfig.h"
#include "snippetsimages.h"
#include "codesnippetstreectrl.h"
//#include "snippetproperty.h"
#include "messagebox.h"
#include "settingsdlg.h"
#include "menuidentifiers.h"


//-#define SNIPPETS_TREE_IMAGE_COUNT 3
// above redefined in snipimages.h to 5

//const wxString snippetsTreeImageFileNames[] = {
//	_T("allsnippets.png"),
//	_T("category.png"),
//	_T("snippet.png")
//};

int idSearchSnippetCtrl         = wxNewId();
int idSearchCfgBtn              = wxNewId();
int idSnippetsTreeCtrl          = wxNewId();

    // Context Menu items
int idMnuAddSubCategory         = wxNewId();
int idMnuRemove                 = wxNewId();
int idMnuConvertToCategory      = wxNewId();
int idMnuAddSnippet             = wxNewId();
int idMnuApplySnippet           = wxNewId();
int idMnuLoadSnippetsFromFile   = wxNewId();
int idMnuSaveSnippets           = wxNewId();
int idMnuSaveSnippetsToFile     = wxNewId();
int idMnuRemoveAll              = wxNewId();
int idMnuCopyToClipboard        = wxNewId();
int idMnuEditSnippet            = wxNewId();
//-int idMnuOpenAsFileLink         = wxNewId();
int idMnuConvertToFileLink      = wxNewId();
int idMnuProperties             = wxNewId();
int idMnuSettings               = wxNewId();
int idMnuAbout                  = wxNewId();

// Search config menu items
int idMnuCaseSensitive          = wxNewId();
int idMnuClear                  = wxNewId();
int idMnuScope                  = wxNewId();
int idMnuScopeSnippets          = wxNewId();
int idMnuScopeCategories        = wxNewId();
int idMnuScopeBoth              = wxNewId();

// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CodeSnippetsWindow, wxPanel)
    // ---
	// Tree menu events
	EVT_MENU(idMnuRemove,           CodeSnippetsWindow::OnMnuRemove)
	EVT_MENU(idMnuConvertToCategory,CodeSnippetsWindow::OnMnuConvertToCategory)
	EVT_MENU(idMnuAddSubCategory,   CodeSnippetsWindow::OnMnuAddSubCategory)
	EVT_MENU(idMnuAddSnippet,       CodeSnippetsWindow::OnMnuAddSnippet)
	EVT_MENU(idMnuApplySnippet,     CodeSnippetsWindow::OnMnuApplySnippet)
	EVT_MENU(idMnuLoadSnippetsFromFile, CodeSnippetsWindow::OnMnuLoadSnippetsFromFile)
	EVT_MENU(idMnuSaveSnippets,     CodeSnippetsWindow::OnMnuSaveSnippets)
	EVT_MENU(idMnuSaveSnippetsToFile,CodeSnippetsWindow::OnMnuSaveSnippetsAs)
	EVT_MENU(idMnuRemoveAll,        CodeSnippetsWindow::OnMnuRemoveAll)
	EVT_MENU(idMnuCopyToClipboard,  CodeSnippetsWindow::OnMnuCopyToClipboard)
	EVT_MENU(idMnuEditSnippet,      CodeSnippetsWindow::OnMnuEditSnippet)
	EVT_MENU(idMnuConvertToFileLink,CodeSnippetsWindow::OnMnuSaveSnippetAsFileLink)
	EVT_MENU(idMnuProperties,       CodeSnippetsWindow::OnMnuProperties)
	EVT_MENU(idMnuSettings,         CodeSnippetsWindow::OnMnuSettings)
   #if defined(BUILDING_PLUGIN)
	EVT_MENU(idMnuAbout,            CodeSnippetsWindow::OnMnuAbout)
   #endif
	// ---

	// Search config menu event
	EVT_MENU(idMnuCaseSensitive, CodeSnippetsWindow::OnMnuCaseSensitive)
	EVT_MENU(idMnuScopeSnippets, CodeSnippetsWindow::OnMnuChangeScope)
	EVT_MENU(idMnuScopeCategories, CodeSnippetsWindow::OnMnuChangeScope)
	EVT_MENU(idMnuScopeBoth, CodeSnippetsWindow::OnMnuChangeScope)
	EVT_MENU(idMnuClear, CodeSnippetsWindow::OnMnuClear)
	// ---

	EVT_BUTTON(idSearchCfgBtn, CodeSnippetsWindow::OnSearchCfg)
	EVT_TEXT(idSearchSnippetCtrl, CodeSnippetsWindow::OnSearch)
	EVT_TREE_ITEM_ACTIVATED(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemActivated)
	EVT_TREE_ITEM_MENU(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemMenu)
	EVT_TREE_BEGIN_DRAG(idSnippetsTreeCtrl, CodeSnippetsWindow::OnBeginDrag)
	EVT_TREE_END_DRAG(idSnippetsTreeCtrl, CodeSnippetsWindow::OnEndDrag)
	EVT_TREE_BEGIN_LABEL_EDIT(idSnippetsTreeCtrl, CodeSnippetsWindow::OnBeginLabelEdit)
	EVT_TREE_END_LABEL_EDIT(idSnippetsTreeCtrl, CodeSnippetsWindow::OnEndLabelEdit)
	EVT_TREE_ITEM_GETTOOLTIP(idSnippetsTreeCtrl, CodeSnippetsWindow::OnItemGetToolTip)
	// ---
	//-EVT_CLOSE( CodeSnippetsWindow::OnCloseWindow) Doesn't work on wxAUI windows

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
CodeSnippetsWindow::CodeSnippetsWindow(wxWindow* parent)
// ----------------------------------------------------------------------------
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
    m_isCheckingForExternallyModifiedFiles = false;

    if (not GetConfig()->pMainFrame)
        GetConfig()->pMainFrame = parent;

	// Initialize the dialog
	InitDlg();
	m_AppendItemsFromFile = false;

	// Load the settings
	GetConfig()->SettingsLoad();

    wxString s(__FUNCTION__, wxConvUTF8);
    LOGIT(s+wxT("LoadingFile:%s"),GetConfig()->SettingsSnippetsXmlFullPath.c_str());

	// Load the snippets
	GetSnippetsTreeCtrl()->LoadItemsFromFile(GetConfig()->SettingsSnippetsXmlFullPath, false);
    GetSnippetsTreeCtrl()->SaveFileModificationTime();
}
// ----------------------------------------------------------------------------
CodeSnippetsWindow::~CodeSnippetsWindow()
// ----------------------------------------------------------------------------
{
	// Save the snippets
	//-GetSnippetsTreeCtrl()->SaveItemsToFile(ConfigManager::GetFolder(sdConfig) + wxFILE_SEP_PATH + _T("codesnippets.xml"));
	GetSnippetsTreeCtrl()->SaveItemsToFile(GetConfig()->SettingsSnippetsXmlFullPath);

	//- Save the settings
    //-	ConfigManager* cfgMan = Manager::Get()->GetConfigManager(_T("codesnippets"));
    //-	cfgMan->Write(_T("casesensitive"), m_SearchConfig.caseSensitive);
    //-	cfgMan->Write(_T("scope"), m_SearchConfig.scope);
    GetConfig()->SettingsSave();

	// Release tree images
	//-delete m_SnippetsTreeImageList;
}
//// ----------------------------------------------------------------------------
// EVT_CLOSE does not work with wxAUI windows
//// ----------------------------------------------------------------------------
//void CodeSnippetsWindow::OnCloseWindow(wxCloseEvent& event)
//// ----------------------------------------------------------------------------
//{
//    asm("int3");
//    if (GetConfig()->IsPlugin())
//        GetConfig()->SettingsSaveWinPosition();
//    Destroy();
//}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::InitDlg()
// ----------------------------------------------------------------------------
{
	// Color which we're going to use as mask
	wxColor maskColor(255, 0, 255);

	// Directory where the images are located
	wxString imagesDir;
	imagesDir << _T("images") << wxFILE_SEP_PATH << _T("codesnippets") << wxFILE_SEP_PATH;

	wxBoxSizer* parentSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* searchCtrlSizer = new wxBoxSizer(wxHORIZONTAL);

	m_SearchSnippetCtrl = new wxTextCtrl(this, idSearchSnippetCtrl, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	searchCtrlSizer->Add(m_SearchSnippetCtrl, 1, wxBOTTOM|wxLEFT|wxTOP, 5);

	m_SearchCfgBtn = new wxButton(this, idSearchCfgBtn, _T(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	searchCtrlSizer->Add(m_SearchCfgBtn, 0, wxBOTTOM|wxRIGHT|wxTOP, 5);

	parentSizer->Add(searchCtrlSizer, 0, wxEXPAND, 5);

	wxBoxSizer* treeCtrlSizer = new wxBoxSizer(wxVERTICAL);

	m_SnippetsTreeCtrl = new CodeSnippetsTreeCtrl(this, idSnippetsTreeCtrl, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS);
	treeCtrlSizer->Add(GetSnippetsTreeCtrl(), 1, wxEXPAND, 5);

	parentSizer->Add(treeCtrlSizer, 1, wxEXPAND, 5);

	SetSizer(parentSizer);
	Layout();

	GetSnippetsTreeCtrl()->SetDropTarget(new SnippetsDropTarget(GetSnippetsTreeCtrl()));

//-	m_SnippetsTreeImageList = new wxImageList(16, 16, true, 3);
//-	for (int i = 0; i < SNIPPETS_TREE_IMAGE_COUNT; ++i)
//-	{
//-		wxBitmap imageFile = cbLoadBitmap(ConfigManager::LocateDataFile(imagesDir + snippetsTreeImageFileNames[i], sdDataUser|sdDataGlobal), wxBITMAP_TYPE_PNG);
//-		m_SnippetsTreeImageList->Add(imageFile, maskColor);
//-	}
//-	GetSnippetsTreeCtrl()->SetImageList(m_SnippetsTreeImageList);

        // set the treeCtrl image list
	GetSnippetsTreeCtrl()->SetImageList(GetSnipImageList());
        // Set pgm icon again, but from converted images
//?	GetMainFrame()->SetIcon( GetSnippetsTreeCtrl()->GetImageList()->GetIcon(TREE_IMAGE_ALL_SNIPPETS) );

	// Add root item
	SnippetItemData* rootData = new SnippetItemData(SnippetItemData::TYPE_ROOT);
	GetSnippetsTreeCtrl()->AddRoot(_("All snippets"), 0, -1, rootData);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnSearchCfg(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	wxMenu* searchCfgMenu = new wxMenu();
	wxMenu* searchScopeMenu = new wxMenu();

	searchScopeMenu->AppendRadioItem(idMnuScopeSnippets, _("Snippets"));
	searchScopeMenu->AppendRadioItem(idMnuScopeCategories, _("Categories"));
	searchScopeMenu->AppendRadioItem(idMnuScopeBoth, _("Snippets and categories"));
	switch (GetConfig()->m_SearchConfig.scope)
	{
		case CodeSnippetsConfig::SCOPE_SNIPPETS:
			searchScopeMenu->Check(idMnuScopeSnippets, true);
		break;

		case CodeSnippetsConfig::SCOPE_CATEGORIES:
			searchScopeMenu->Check(idMnuScopeCategories, true);
		break;

		case CodeSnippetsConfig::SCOPE_BOTH:
			searchScopeMenu->Check(idMnuScopeBoth, true);
		break;
	}

	searchCfgMenu->AppendCheckItem(idMnuCaseSensitive, _("Case sensitive"));
	if (GetConfig()->m_SearchConfig.caseSensitive)
	{
		searchCfgMenu->Check(idMnuCaseSensitive, true);
	}

	searchCfgMenu->Append(idMnuScope, _("Scope"), searchScopeMenu);
	searchCfgMenu->AppendSeparator();
	searchCfgMenu->Append(idMnuClear, _("Clear"));

	if (m_SearchSnippetCtrl->GetValue().IsEmpty())
	{
		searchCfgMenu->Enable(idMnuClear, false);
	}

	// Always pop up the menu at the same position
	wxRect btnRect = m_SearchCfgBtn->GetRect();
	PopupMenu(searchCfgMenu, btnRect.x + btnRect.GetWidth(), btnRect.y);

	searchCfgMenu->Destroy(idMnuScope);
	delete searchCfgMenu;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnSearch(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	if (m_SearchSnippetCtrl->GetValue().IsEmpty())
	{
		// Reset the root node's title
		GetSnippetsTreeCtrl()->SetItemText(GetSnippetsTreeCtrl()->GetRootItem(), _("All snippets"));

		// Reset the searchbox's background color
		m_SearchSnippetCtrl->SetBackgroundColour(wxNullColour);
		m_SearchSnippetCtrl->Refresh();
	}
	else
	{
		// Edit the root node's title so that the user knows we are
		// searching for specific item
		GetSnippetsTreeCtrl()->SetItemText(GetSnippetsTreeCtrl()->GetRootItem(), wxString::Format(_("Search \"%s\""), m_SearchSnippetCtrl->GetValue().c_str()));

		// Search it!
		wxString searchTerms = m_SearchSnippetCtrl->GetValue();
		if (not GetConfig()->m_SearchConfig.caseSensitive)
		{
			searchTerms.LowerCase();
		}

		wxTreeItemId foundID = SearchSnippet(searchTerms, GetSnippetsTreeCtrl()->GetRootItem());

		if (foundID.IsOk())
		{
			// Highlight the item
			GetSnippetsTreeCtrl()->EnsureVisible(foundID);
			GetSnippetsTreeCtrl()->SelectItem(foundID);

			// Reset the searchbox's backgroud color
			m_SearchSnippetCtrl->SetBackgroundColour(wxNullColour);
			m_SearchSnippetCtrl->Refresh();
		}
		else
		{
			// Select the root item so user doesn't think we found something
			GetSnippetsTreeCtrl()->EnsureVisible(GetSnippetsTreeCtrl()->GetRootItem());
			GetSnippetsTreeCtrl()->SelectItem(GetSnippetsTreeCtrl()->GetRootItem());

			// Add visual feedback: paint the searchbox's background with
			// light red color
			m_SearchSnippetCtrl->SetBackgroundColour(wxColor(244, 168, 168));
			m_SearchSnippetCtrl->Refresh();
		}
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnItemActivated(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
	ApplySnippet(event.GetItem());
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnItemMenu(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
    // The context menu for the selected item has been requested, either by a
    // right click or by using the menu key.

	// Get the item associated with the event
	if (const SnippetItemData* eventItem = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(event.GetItem())))
	{
		wxMenu* snippetsTreeMenu = new wxMenu();

		// Check the type of the item and add the menu items
		switch (eventItem->GetType())
		{
			case SnippetItemData::TYPE_ROOT:
				snippetsTreeMenu->Append(idMnuAddSnippet, _("Add code snippet"));
				snippetsTreeMenu->Append(idMnuAddSubCategory, _("Add subcategory"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuRemoveAll, _("Remove all items"));

				// Disable "Remove all items" if the root item does not have child items
				if (!GetSnippetsTreeCtrl()->ItemHasChildren(GetSnippetsTreeCtrl()->GetRootItem()))
					snippetsTreeMenu->Enable(idMnuRemoveAll, false);

				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuSaveSnippets, _("Save file"));
				snippetsTreeMenu->Append(idMnuSaveSnippetsToFile, _("Save file As..."));

				// Disable "Save to file..." if the root item does not have child items
				if (!GetSnippetsTreeCtrl()->ItemHasChildren(GetSnippetsTreeCtrl()->GetRootItem()))
				{	snippetsTreeMenu->Enable(idMnuSaveSnippetsToFile, false);
					snippetsTreeMenu->Enable(idMnuSaveSnippets, false);
				}

				// Check if Shift key is pressed
				if (::wxGetKeyState(WXK_SHIFT))
				{
					// Add append from file entry
					snippetsTreeMenu->Append(idMnuLoadSnippetsFromFile, _("Load from file (append)..."));
					m_AppendItemsFromFile = true;
				}
				else
				{
					// Use the normal load from file entry
					snippetsTreeMenu->Append(idMnuLoadSnippetsFromFile, _("Load from file..."));
					m_AppendItemsFromFile = false;
				}

                snippetsTreeMenu->Append(idMnuSettings, _("Settings..."));
               #if defined(BUILDING_PLUGIN)
                snippetsTreeMenu->Append(idMnuAbout, _("About..."));
               #endif
			break;

			case SnippetItemData::TYPE_CATEGORY:
				snippetsTreeMenu->Append(idMnuAddSnippet, _("Add code snippet"));
				snippetsTreeMenu->Append(idMnuAddSubCategory, _("Add subcategory"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuRemove, _("Remove"));
			break;

			case SnippetItemData::TYPE_SNIPPET:
				snippetsTreeMenu->Append(idMnuEditSnippet, _("Edit"));
                if (GetConfig()->IsPlugin())
                {   snippetsTreeMenu->Append(idMnuApplySnippet, _("Apply"));
                }
				snippetsTreeMenu->Append(idMnuCopyToClipboard, _("Copy to clipboard"));
				snippetsTreeMenu->AppendSeparator();
				snippetsTreeMenu->Append(idMnuConvertToCategory, _("Convert to Catagory"));
				snippetsTreeMenu->Append(idMnuConvertToFileLink, _("Convert to File Link..."));
				snippetsTreeMenu->Append(idMnuRemove, _("Remove"));
				snippetsTreeMenu->AppendSeparator();
                snippetsTreeMenu->Append(idMnuProperties, _("Properties..."));
			break;
		}

		// Save the item ID for later use
		//-m_MnuAssociatedItemID = eventItem->GetId();
		GetSnippetsTreeCtrl()->SetAssociatedItemID( eventItem->GetId() );

		PopupMenu(snippetsTreeMenu);

		delete snippetsTreeMenu;
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnBeginDrag(wxTreeEvent& /*event*/)
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnEndDrag(wxTreeEvent& /*event*/)
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuAddSubCategory(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Add new category using the associated item ID
	GetSnippetsTreeCtrl()->AddCategory(GetSnippetsTreeCtrl()->GetAssociatedItemID(), _("New category"), true);
	SetFileChanged(true);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuRemove(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Get the associated item id
	wxTreeItemId itemID = GetAssociatedItemID();

//	// Sanity check
//	if (itemID != GetSnippetsTreeCtrl()->GetRootItem())
//	{
//		// No questions asked
//		GetSnippetsTreeCtrl()->DeleteChildren(itemID);
//		GetSnippetsTreeCtrl()->Delete(itemID);
//		SetFileChanged(true);
//	}
    GetSnippetsTreeCtrl()->RemoveItem(itemID);
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuConvertToCategory(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // create category to hold this snippet
    // place old snippet under new category
    // delete the old snippet

    SetActiveMenuId( event.GetId() );
    GetSnippetsTreeCtrl()->ConvertSnippetToCategory( GetAssociatedItemID());

}//OnMnuConvertToCategory

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuAddSnippet(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Add new snippet using the associated item ID
	GetSnippetsTreeCtrl()->AddCodeSnippet(GetAssociatedItemID(), _("New snippet"), wxEmptyString, true);
	SetFileChanged(true);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::ApplySnippet(const wxTreeItemId& itemID)
// ----------------------------------------------------------------------------
{
	// Get the item
	if (const SnippetItemData* item = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(itemID)))
	{
		// Check that we're using the correct item type
		if (item->GetType() != SnippetItemData::TYPE_SNIPPET)
		{
			return;
		}
      #if defined(BUILDING_PLUGIN)
		// Check that editor is open
		EditorManager* editorMan = Manager::Get()->GetEditorManager();
		if(!editorMan)
		{
			return;
		}

		cbEditor* editor = editorMan->GetBuiltinActiveEditor();
		if(!editor)
		{
			return;
		}

		cbStyledTextCtrl* ctrl = editor->GetControl();
		if(ctrl)
		{
			// Apply the snippet
			wxString snippet = item->GetSnippet();
			CheckForMacros(snippet);
			//wxLeaner: http://forums.codeblocks.org/index.php/topic,5375.new.html#new
			// Honor target source line indentation
            snippet.Replace(wxT("\n"), wxT('\n') + editor->GetLineIndentString(ctrl->GetCurrentLine()));
            ctrl->AddText(snippet);
		}
	  #else //NOT defined(BUILDING_PLUGIN)
        AddTextToClipBoard( item->GetSnippet() );
	  #endif //defined(BUILDING_PLUGIN)
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuApplySnippet(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Apply the snippet using the associated item id
	ApplySnippet(GetAssociatedItemID());
}

// ----------------------------------------------------------------------------
wxTreeItemId CodeSnippetsWindow::SearchSnippet(const wxString& searchTerms, const wxTreeItemId& node)
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
					if (GetConfig()->m_SearchConfig.scope == GetConfig()->SCOPE_CATEGORIES)
					{
						ignoreThisType = true;
					}
				break;

				case SnippetItemData::TYPE_CATEGORY:
					if (GetConfig()->m_SearchConfig.scope == GetConfig()->SCOPE_SNIPPETS)
					{
						ignoreThisType = true;
					}
				break;
			}

			if (!ignoreThisType)
			{
				wxString label = GetSnippetsTreeCtrl()->GetItemText(item);

				if (not GetConfig()->m_SearchConfig.caseSensitive)
				{
					label.LowerCase();
				}

				if(label.Contains(searchTerms))
				{
					return item;
				}
			}

			if(GetSnippetsTreeCtrl()->ItemHasChildren(item))
			{
				wxTreeItemId search = SearchSnippet(searchTerms, item);
				if(search.IsOk())
				{
					return search;
				}
			}
			item = GetSnippetsTreeCtrl()->GetNextChild(node, cookie);
		}
	}

   // Return dummy item if search string was not found
   wxTreeItemId dummyItem;
   return dummyItem;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnBeginLabelEdit(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
	// Deny editing of the root item
	if (event.GetItem() == GetSnippetsTreeCtrl()->GetRootItem())
	{
		event.Veto();
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuLoadSnippetsFromFile(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	wxFileDialog dlg(this, _("Load snippets from file"), wxEmptyString, wxEmptyString, _("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxOPEN|wxFILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
	{
        LOGIT(wxT("LoadingFile:%s"),dlg.GetPath().c_str());
		GetSnippetsTreeCtrl()->LoadItemsFromFile(dlg.GetPath(), m_AppendItemsFromFile);
        GetConfig()->SettingsSnippetsXmlFullPath = dlg.GetPath();
        SetFileChanged(false);
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSaveSnippets(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	{
		GetSnippetsTreeCtrl()->SaveItemsToFile(GetConfig()->SettingsSnippetsXmlFullPath);
		SetFileChanged(false);
	}
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSaveSnippetsAs(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	wxFileDialog dlg(this, _("Save snippets to file"), wxEmptyString, _T("codesnippets.xml"), _("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxSAVE|wxOVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK)
	{
		GetSnippetsTreeCtrl()->SaveItemsToFile(dlg.GetPath());
		SetFileChanged(false);
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnEndLabelEdit(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
	// Sort all the parent item's children
	GetSnippetsTreeCtrl()->SortChildren(GetSnippetsTreeCtrl()->GetItemParent(event.GetItem()));
	SetFileChanged(true);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuRemoveAll(wxCommandEvent& /*event*/)
// ----------------------------------------------------------------------------
{
	// Remove all items
	GetSnippetsTreeCtrl()->DeleteChildren(GetSnippetsTreeCtrl()->GetRootItem());
	SetFileChanged(true);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuCaseSensitive(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	GetConfig()->m_SearchConfig.caseSensitive = (not GetConfig()->m_SearchConfig.caseSensitive);
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuChangeScope(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	if (event.GetId() == idMnuScopeSnippets)
	{
		GetConfig()->m_SearchConfig.scope = GetConfig()->SCOPE_SNIPPETS;
	}
	else if (event.GetId() == idMnuScopeCategories)
	{
		GetConfig()->m_SearchConfig.scope = GetConfig()->SCOPE_CATEGORIES;
	}
	else if (event.GetId() == idMnuScopeBoth)
	{
		GetConfig()->m_SearchConfig.scope = GetConfig()->SCOPE_BOTH;
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuClear(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // search->clear context menu item
	m_SearchSnippetCtrl->Clear();
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuCopyToClipboard(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
	if (wxTheClipboard->Open())
	{
		const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(GetAssociatedItemID()));
		if (itemData)
		{
			wxTheClipboard->SetData(new wxTextDataObject(itemData->GetSnippet()));
			wxTheClipboard->Close();
		}
	}
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuEditSnippet(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if (not IsSnippet() ) return;
	if (SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(GetAssociatedItemID() )))
	{
        if (not itemData){;} //variable unused

        wxTreeItemId itemId = GetAssociatedItemID();
        wxString FileName = GetSnippet( itemId );

        // If snippet is text, edit it as text
        if (FileName.Length() > 128)
        {   // if text is > 128 characters, open a temp file with snippet text as data.
            GetSnippetsTreeCtrl()->EditSnippetAsText();
            return;
        }
        // If snippet is non-existent file, edit as text
        if ( (FileName.IsEmpty())
            || (not ::wxFileExists( FileName)) )
        {   // if, non-existent file, open snippet text as data
            GetSnippetsTreeCtrl()->EditSnippetAsText();
            return;
        }
        GetSnippetsTreeCtrl()->EditSnippetAsFileLink();
	}
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::CheckForMacros(wxString& snippet)
// ----------------------------------------------------------------------------
{
    //FIXME: CheckForMacros in App???
  #if defined(BUILDING_PLUGIN)
	// Copied from cbEditor::Autocomplete, I admit it
	int macroPos = snippet.Find(_T("$("));
	while (macroPos != -1)
	{
		// locate ending parenthesis
		int macroPosEnd = macroPos + 2;
		int len = (int)snippet.Length();

		while (macroPosEnd < len && snippet.GetChar(macroPosEnd) != _T(')'))
			++macroPosEnd;

		if (macroPosEnd == len)
			break; // no ending parenthesis

		wxString macroName = snippet.SubString(macroPos + 2, macroPosEnd - 1);
		wxString macro = wxGetTextFromUser(wxString::Format(_("Please enter the text for \"%s\":"), macroName.c_str()), _("Macro substitution"));
		snippet.Replace(_T("$(") + macroName + _T(")"), macro);

		macroPos = snippet.Find(_T("$("));
	}

	// Replace any other macros in the generated code
	Manager::Get()->GetMacrosManager()->ReplaceMacros(snippet);
  #endif
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnItemGetToolTip(wxTreeEvent& event)
// ----------------------------------------------------------------------------
{
	// "My eyes! The goggles do nothing!"
	//
	// Use the following only on wxWidgets 2.8.
	#if wxCHECK_VERSION(2, 8, 0)
	if (const SnippetItemData* itemData = (SnippetItemData*)(GetSnippetsTreeCtrl()->GetItemData(event.GetItem())))
	{
		if (itemData->GetType() == SnippetItemData::TYPE_SNIPPET)
		{
			wxString snippetToolTip = itemData->GetSnippet();
			size_t originalLength = snippetToolTip.Len();

			// Take the first 255 characters or less, note that the
			// wxWidgets documentation doesn't say what is maximum lenght of
			// the tooltip so this can be increased if needed.
			size_t charsInToolTip = 255;
			snippetToolTip = snippetToolTip.Mid(0, charsInToolTip);

			// Replace all tabs with spaces; tabs break the tooltips
			snippetToolTip.Replace(_T("\t"), _T("    "));

			if (snippetToolTip.Len() > charsInToolTip || originalLength > charsInToolTip)
			{
				snippetToolTip = snippetToolTip.Mid(0, charsInToolTip - 4);
				snippetToolTip.Append(_T(" ..."));
			}

			event.SetToolTip(snippetToolTip);
		}
	}
	#endif
}
// ----------------------------------------------------------------------------
bool CodeSnippetsWindow::AddTextToClipBoard(const wxString& text)
// ----------------------------------------------------------------------------
{
    if ( not wxTheClipboard->Open() ) {
        wxLogError( GetConfig()->AppName + _T(":Can't open clipboard."));
        return false;
    }
    wxTheClipboard->SetData(new wxTextDataObject(text));
    wxTheClipboard->Close();

    #ifdef LOGGING
     LOGIT( wxT("AddTextToClipBoard:Text[%s]"), text.GetData() );
    #endif //LOGGING
    return true;
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuProperties(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{                                                           //(pecan 2006/9/12)
    SetActiveMenuId( event.GetId() );

	// Examine the snippet using the associated item id
	wxTreeItemId itemID = GetAssociatedItemID();
	GetSnippetsTreeCtrl()->EditSnippetProperties( itemID );
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSettings(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    SetActiveMenuId( event.GetId() );
    SettingsDlg* pDlg = new SettingsDlg( this );
    pDlg->ShowModal();
    delete pDlg;
}

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::SetSnippetImage(wxTreeItemId itemId)
// ----------------------------------------------------------------------------
{
    // set the item tree image
    if ( IsFileSnippet(itemId) )
        GetSnippetsTreeCtrl()->SetItemImage( itemId, TREE_IMAGE_SNIPPET_FILE);
    else
        GetSnippetsTreeCtrl()->SetItemImage( itemId, TREE_IMAGE_SNIPPET_TEXT);
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::CheckForExternallyModifiedFiles()
// ----------------------------------------------------------------------------
{
    // when we know we've changed the file internally, finesse this

    if ( GetFileChanged() ) return;

    wxString whichApp = wxT("CodeSnippets Plugin ");
    if (GetConfig()->IsApplication()) whichApp = wxT("CodeSnippets Program ");

    if(m_isCheckingForExternallyModifiedFiles) // for some reason, a mutex locker does not work???
        return;
    m_isCheckingForExternallyModifiedFiles = true;

    //-wxLogNull ln;
    //-bool reloadAll = false; // flag to stop bugging the user
    bool failedFiles = false;  // file failed to reload
    bool b_modified = false;

    // Was file deleted?
    if ( not wxFileExists(GetConfig()->SettingsSnippetsXmlFullPath) )
    {
        //        if( GetFileChanged() ) // Already set the flag
        //            return;;
        //        wxString msg;
        //        msg.Printf(_("%s \nhas been deleted, or is no longer available.\n"
        //                     "Do you wish to keep the file open?\n"
        //                     "Yes to keep the file, No to close it."), GetConfig()->SettingsSnippetsXmlFullPath.c_str());
        //        if (messageBox(msg, whichApp + _("File changed!"), wxICON_QUESTION | wxYES_NO) == wxYES)
        //            SetFileChanged(true);
        //        else
        //        {
        //        }
        m_isCheckingForExternallyModifiedFiles = false;
        return; // return when file does not exists
    }

    wxFileName fname( GetConfig()->SettingsSnippetsXmlFullPath );
    wxDateTime last = fname.GetModificationTime();

    //    ProjectFile* pf = ed->GetProjectFile();
    //
    //    //File changed from RO -> RW?
    //    if (ed->GetControl()->GetReadOnly() &&
    //            wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
    //    {
    //        b_modified = false;
    //        ed->GetControl()->SetReadOnly(false);
    //        if (pf)
    //            pf->SetFileState(fvsNormal);
    //    }
    //    //File changed from RW -> RO?
    //    if (!ed->GetControl()->GetReadOnly() &&
    //            !wxFile::Access(ed->GetFilename().c_str(), wxFile::write))
    //    {
    //        b_modified = false;
    //        ed->GetControl()->SetReadOnly(true);
    //        if (pf)
    //            pf->SetFileState(fvsReadOnly);
    //    }

    //Was File content changed?
    if ( last.IsLaterThan(GetSnippetsTreeCtrl()->GetSavedFileModificationTime()) )
        b_modified = true;

    if (b_modified)
    {
        // modified; ask to reload
        int ret = -1;
        {
            wxString msg;
            msg.Printf(_("%s\n\nFile is modified outside the IDE...\nDo you want to reload it (you will lose any unsaved work)?"),
                       GetConfig()->SettingsSnippetsXmlFullPath.c_str());
            if (messageBox(msg, whichApp + _("needs to Reload file?!"), wxICON_QUESTION | wxYES_NO) == wxYES)
                ret = wxYES;
            else
                ret = wxNO;

        }
        if ( ret == wxYES )
        {
            if (not GetSnippetsTreeCtrl()->LoadItemsFromFile(GetConfig()->SettingsSnippetsXmlFullPath, m_AppendItemsFromFile))
                failedFiles = true;
                // File modification time is saved by LoadItemsFromFile
                //GetSnippetsTreeCtrl()->SaveFileModificationTime(last);
        }
        else if (ret == wxNO)
            GetSnippetsTreeCtrl()->SaveFileModificationTime();
    }

    if (failedFiles)
    {
        wxString msg;
        msg.Printf(_("Could not reload file:\n\n%s"), GetConfig()->SettingsSnippetsXmlFullPath.c_str() );
        messageBox(msg, whichApp +  _("Error"), wxICON_ERROR);
    }
    m_isCheckingForExternallyModifiedFiles = false;

} // end of CheckForExternallyModifiedFiles

// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuEditSnippetAsFileLink(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Open Snippet as file from context menu
    SetActiveMenuId( event.GetId() );
    GetSnippetsTreeCtrl()->EditSnippetAsFileLink( );
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuSaveSnippetAsFileLink(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // Open Snippet as file from context menu
    SetActiveMenuId( event.GetId() );
    GetSnippetsTreeCtrl()->SaveSnippetAsFileLink( );
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::CenterChildOnParent(wxWindow* child)
// ----------------------------------------------------------------------------
{
    //For docked window we cannot get its position. Just move
    // the window under the mouse pointer
    if ( GetConfig()->IsPlugin() )
    {   wxPoint mousePosn = ::wxGetMousePosition();
        child->Move(mousePosn.x, mousePosn.y);
        return;
    }

    // If application window
    // Put Top left corner in center of parent (if possible)

    int h; int w;
    int x; int y;
    int displayX; int displayY;
    wxWindow* mainFrame = child->GetParent();
    if (not mainFrame) return;

    // move upper left dialog corner to center of parent
    ::wxDisplaySize(&displayX, &displayY);
    mainFrame->GetPosition(&x, &y );
    if ((x == 0) || (y == 0))
        if (GetConfig()->pMainFrame)
            GetConfig()->pMainFrame->GetPosition(&x, &y );
    mainFrame->GetClientSize(&w,&h);

    // move child underneath the mouse pointer
    wxPoint movePosn = ::wxGetMousePosition();
    movePosn.x = x+(w>>2);
    movePosn.y = y+(h>>2);

    // Make child is not off the screen
    wxSize size = child->GetSize();
    //-LOGIT( _T("X[%d]Y[%d]width[%d]height[%d]"), x,y,w,h );

    if ( (movePosn.x+size.GetWidth()) > displayX)
        movePosn.x = displayX-size.GetWidth();
    if ( (movePosn.y+size.GetHeight()) > displayY)
        movePosn.y = displayY-size.GetHeight();

    child->Move(movePosn.x, movePosn.y);
    return;
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::OnMnuAbout(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    wxString wxbuild(wxVERSION_STRING);

    #if defined(__WXMSW__)
        wxbuild << _T("-Windows");
    #elif defined(__UNIX__)
        wxbuild << _T("-Linux");
    #endif

    #if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
    #else
        wxbuild << _T("-ANSI build");
    #endif // wxUSE_UNICODE

    wxString buildInfo = wxbuild;
    wxString
        pgmVersionString = wxT("CodeSnippets v") + GetConfig()->GetVersion();
    buildInfo = wxT("\t")+pgmVersionString + wxT("\n")+ wxT("\t")+buildInfo;
    buildInfo = buildInfo + wxT("\n\n\t")+wxT("Original Code by Arto Jonsson");
    buildInfo = buildInfo + wxT("\n\t")+wxT("Modified/Enhanced by Pecan Heber");

    ShowSnippetsAbout( buildInfo);
}
// ----------------------------------------------------------------------------
void CodeSnippetsWindow::ShowSnippetsAbout(wxString buildInfo)
// ----------------------------------------------------------------------------
{

    //wxString msg = wxbuildinfo(long_f);
    wxString helpText;
    helpText << wxT(" Each Snippet item may specify either text or a File Link.\n")
             << wxT("\n")
             << wxT(" Snippets may be edited from within the context menu \n")
             << wxT("\n")

             << wxT(" File Link snippets are created by dragging text to a new snippet, \n")
             << wxT(" then using the context menu to \"Convert to File Link\". \n")
             << wxT(" The data will be written to the specified file and the filename \n")
             << wxT(" will be placed in the snippets text area as a Link. \n")
             << wxT("\n")

             << wxT(" Snippets are accessed by using the context menu \"Edit\" \n")
             << wxT(" or via the Properties context menu entry. \n")
             << wxT("\n")

             << wxT(" Use the \"Settings\" menu to specify an external editor and \n")
             << wxT(" to specify a non-default Snippets index file. \n")
             << wxT("\n")

             << wxT(" Both the text and file snippets may be dragged outward (Windows only)\n")
             << wxT(" or copied to the clipboard.\n")
             << wxT("\n")

             << wxT(" For MS Windows:\n")
             << wxT(" Dragging a file snippet onto an external program window \n")
             << wxT(" will open the file. Dragging it into the edit area will \n")
             << wxT(" insert the text.\n");

    messageBox(wxT("\n\n")+buildInfo+wxT("\n\n")+helpText, _("About"),wxOK, wxSIMPLE_BORDER);

}

// ////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
bool SnippetsDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
// ----------------------------------------------------------------------------
{
    // This Drop routine refers to dragging external text onto a tree item
	// Set focus to the Code snippets window
	m_TreeCtrl->SetFocus();

	wxPoint p(x, y);
	int hitTestFlags = 0;

	// Find out if there's a tree item under the coordinates
	wxTreeItemId itemID = m_TreeCtrl->HitTest(p, hitTestFlags);

	if (hitTestFlags & wxTREE_HITTEST_ONITEMBUTTON|wxTREE_HITTEST_ONITEMICON|wxTREE_HITTEST_ONITEMLABEL)
	{
		// Find out the item type
		if (SnippetItemData* item = (SnippetItemData*)(m_TreeCtrl->GetItemData(itemID)))
		{
			switch (item->GetType())
			{
				case SnippetItemData::TYPE_ROOT:
					// Add new code snippet to the root
					m_TreeCtrl->AddCodeSnippet(m_TreeCtrl->GetRootItem(), _("New snippet"), data, true);
				break;

				case SnippetItemData::TYPE_CATEGORY:
					// Add new code snippet to the category
					m_TreeCtrl->AddCodeSnippet(item->GetId(), _("New snippet"), data, true);
				break;

				case SnippetItemData::TYPE_SNIPPET:
					// Set the snippet's code to match the dropped data
					item->SetSnippet(data);
				break;
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

// ----------------------------------------------------------------------------
