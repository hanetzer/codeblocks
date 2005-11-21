#ifndef DEBUGGERTREE_H
#define DEBUGGERTREE_H

#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/dynarray.h>

#include "debugger_defs.h"

extern int cbCustom_WATCHES_CHANGED;

class DebuggerTree : public wxPanel
{
	public:
		DebuggerTree(wxEvtHandler* debugger, wxNotebook* parent);
		virtual ~DebuggerTree();
		wxTreeCtrl* GetTree(){ return m_pTree; }

		void BeginUpdateTree(); ///< Clears and freezes the tree for massive updates
		void BuildTree(const wxString& infoText, WatchStringFormat fmt); ///< adds a new node to the tree, parsing @c infoText
		void EndUpdateTree(); ///< Un-freezes the tree

		void ClearWatches();
		void SetWatches(const WatchesArray& watches);
		const WatchesArray& GetWatches();
		void AddWatch(const wxString& watch, WatchFormat format = Undefined, bool notify = true);
		void DeleteWatch(const wxString& watch, WatchFormat format = Any, bool notify = true);
		void DeleteAllWatches();
		Watch* FindWatch(const wxString& watch, WatchFormat format = Any);
		int FindWatchIndex(const wxString& watch, WatchFormat format = Any);
	protected:
		void BuildTreeGDB(const wxString& infoText);
		void BuildTreeCDB(const wxString& infoText);
        void NotifyForChangedWatches();
		void ShowMenu(wxTreeItemId id, const wxPoint& pt);
		void OnTreeRightClick(wxTreeEvent& event);
		void OnRightClick(wxCommandEvent& event);
		void OnAddWatch(wxCommandEvent& event);
		void OnLoadWatchFile(wxCommandEvent& event);
		void OnSaveWatchFile(wxCommandEvent& event);
		void OnEditWatch(wxCommandEvent& event);
		void OnDeleteWatch(wxCommandEvent& event);
		void OnDeleteAllWatches(wxCommandEvent& event);

		void ParseEntry(const wxTreeItemId& parent, wxString& text);
		wxTreeCtrl* m_pTree;
		wxNotebook* m_pParent;
		wxEvtHandler* m_pDebugger;
		int m_PageIndex;
		WatchesArray m_Watches;
        wxArrayString m_TreeState;
        int m_NumUpdates;
        int m_CurrNumUpdates;
	private:
        int FindCharOutsideQuotes(const wxString& str, wxChar ch); // returns position of ch in str
        int FindCommaPos(const wxString& str); // ignores commas in function signatures
        bool m_InUpdateBlock;
		DECLARE_EVENT_TABLE()
};

#endif // DEBUGGERTREE_H

