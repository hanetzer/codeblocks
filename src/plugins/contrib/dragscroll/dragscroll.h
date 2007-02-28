/***************************************************************
 * Name:      dragscroll.h
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan<>
 * Copyright: (c) Pecan
 * License:   GPL
 **************************************************************/

#ifndef DRAGSCROLL_H
#define DRAGSCROLL_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "dragscroll.h"
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <sdk.h>      // precompiled headers (needed by plugin API version 1.80)
#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK

// ----------------------------------------------------------------------------
//  additional includeds for cbDragScroll
// ----------------------------------------------------------------------------
#include <manager.h>
#include <messagemanager.h>
#include <editormanager.h>
#include <configmanager.h>

#include <cbeditor.h> //need def of cbStyledTextCntl
#include <wx/listctrl.h>
#include <wx/event.h>
#include <wx/fileconf.h>
#include <wx/splitter.h>

// ---------------------------------------------------------------------------
//  Define RC2 for codeblocks V1.0RC2 or RC3 for HEAD
// ---------------------------------------------------------------------------
//#define RC2 0
//#define RC3 1

// ---------------------------------------------------------------------------
//  Logging / debugging
// ---------------------------------------------------------------------------

//----------------------------------------
#define VERSION "1.0.39 2007/02/28"
//----------------------------------------

#if defined(dsLOGGING)
    #define LOGGING 1
#endif
//debugging control
#define LOGIT wxLogDebug
#if LOGGING
    #undef LOGIT
    #define LOGIT wxLogMessage
    #define TRAP asm("int3")
#endif


// anchor to one and only DragScroll object
class MyMouseEvents;
class cbDragScrollCfg;

// ----------------------------------------------------------------------------
//  cbDragScroll class declaration
// ----------------------------------------------------------------------------
class cbDragScroll : public cbPlugin
{
	public:
		cbDragScroll();
		~cbDragScroll();
        int GetConfigurationGroup() const { return cgEditor; }
		void BuildMenu(wxMenuBar* menuBar){ return; }
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data){ return; }
        bool BuildToolBar(wxToolBar* toolBar){ return false; }
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
     virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

    static cbDragScroll* pDragScroll;
	protected:
        cbConfigurationPanel* CreatecbCfgPanel(wxWindow* parent);

    public:
        void OnDialogDone(cbDragScrollCfg* pdlg);

        bool GetMouseDragScrollEnabled() { return MouseDragScrollEnabled; }
        bool GetMouseEditorFocusEnabled(){ return MouseEditorFocusEnabled; }
        bool GetMouseFocusEnabled()      { return MouseFocusEnabled; }
        int  GetMouseDragDirection()     { return MouseDragDirection; }
        int  GetMouseDragKey()           { return MouseDragKey; }
        int  GetMouseDragSensitivity()   { return MouseDragSensitivity; }
        int  GetMouseToLineRatio()       { return MouseToLineRatio; }
        int  GetMouseRightKeyCtrl()      { return MouseRightKeyCtrl; }
        int  GetMouseContextDelay()      { return MouseContextDelay; }

        wxWindow* m_pMS_Window;
        wxWindow* m_pSearchResultsWindow;

	private:
        void OnAppStartupDone(CodeBlocksEvent& event);
        void OnAppStartupDoneInit();
        void OnDoConfigRequests(wxUpdateUIEvent& event);

        bool IsAttachedTo(wxWindow* p);
        void AttachRecursively(wxWindow *p);
        void Detach(wxWindow* thisEditor);
        void DetachAll();
        void Attach(wxWindow *p);
        void DisconnectEvtHandler(MyMouseEvents* thisEvtHandler);

        wxWindow* winExists(wxWindow *parent);
        wxWindow* FindWindowRecursively(const wxWindow* parent, const wxWindow* handle);
        void OnWindowOpen(wxEvent& event);
        void OnWindowClose(wxEvent& event);

        wxString        m_ConfigFolder;
        wxString        m_ExecuteFolder;
        wxString        m_DataFolder;
        wxString        m_CfgFilenameStr;

        wxArrayString   m_UsableWindows;
        wxArrayPtrVoid  m_EditorPtrs;
        wxLogWindow*    pMyLog;
        bool            m_bNotebooksAttached;
        wxArrayPtrVoid  m_EventHandlerArray;

        bool MouseDragScrollEnabled ;   //Enable/Disable mouse event handler
        bool MouseEditorFocusEnabled;   //Enable/Disable mouse focus() editor
        bool MouseFocusEnabled      ;   //Enable/Disable focus follows mouse
        int  MouseDragDirection     ;   //Move with or opposite mouse
        int  MouseDragKey           ;   //Right or Middle mouse key
        int  MouseDragSensitivity   ;   //Adaptive speed sensitivity
        int  MouseToLineRatio       ;   //Percentage of mouse moves that make a line
        bool MouseRightKeyCtrl      ;   //Hide Right mouse down from ListCtrl windows
        int  MouseContextDelay      ;   //Linux context menu delay to catch possible mouse scroll move

    private:
		DECLARE_EVENT_TABLE()

};//cbDragScroll

// ----------------------------------------------------------------------------
//      MOUSE DRAG and SCROLL CLASS
// ----------------------------------------------------------------------------
// Dragging modes
#define DRAG_NONE     0
#define DRAG_START    1
#define DRAG_DRAGGING 2
// ----------------------------------------------------------------------------
class MyMouseEvents : public wxEvtHandler
// ----------------------------------------------------------------------------
{

public:
    MyMouseEvents(wxWindow *window)
     { m_Window = window;
       m_DragMode       = DRAG_NONE;
       m_MouseHasMoved  = false;
       m_RatioX = 1; m_RatioY = 1;
       m_MouseMoveToLineMoveRatio    = 0.30;
       m_Direction      = -1;
       m_gtkContextDelay = 240 ;
     }
    ~MyMouseEvents();

    void OnMouseEvent(wxMouseEvent& event);

private:
    wxWindow*   m_Window;
    int         m_DragMode;
    wxPoint     m_DragStartPos;
    wxObject*   m_pEvtObject;
    bool        m_MouseHasMoved;
    double      m_MouseMoveToLineMoveRatio;
    double      m_RatioX, m_RatioY;
    int         m_StartX, m_StartY;
    int         m_InitX,  m_InitY;
    // Scroll Direction move -1(mouse direction) +1(reverse mouse direction)
    int         m_Direction;
    unsigned    m_gtkContextDelay;

    bool KeyDown(wxMouseEvent& event)
        { if ( 0 ==  cbDragScroll::pDragScroll->GetMouseDragKey() )
            return event.RightDown();
            return event.MiddleDown();
        }
    bool KeyIsDown(wxMouseEvent& event)
        { if ( 0 ==  cbDragScroll::pDragScroll->GetMouseDragKey() )
            return event.RightIsDown();
            return event.MiddleIsDown();
        }
    bool KeyUp(wxMouseEvent& event)
        { if ( 0 ==  cbDragScroll::pDragScroll->GetMouseDragKey() )
            return event.RightUp();
            return event.MiddleUp();
        }
    int GetUserDragKey()
        { return ( cbDragScroll::pDragScroll->GetMouseDragKey() ? wxMOUSE_BTN_MIDDLE:wxMOUSE_BTN_RIGHT );}

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
//  Modification/ToDo History
// ----------------------------------------------------------------------------
//
// v0.6 12/30/2005
// ----------------------------------------------------------------------------
//  tiwag conversion to unicode
//	Re: Right Mouse Drag and Scroll Plugin
//  Reply #4 on: December 29, 2005, 08:05:40 PM
//  thanks for this fine plugin,
//  in the course of switching CodeBlocks to unicode build as standard development version
//  i've done some necessary modifications to your code and also updated the project file to
//  the NewBuild global variable system.
//  attached DragScroll 0.4 NewBuild unicode
//  * DragScroll-NewBuild-u.zip (5.77 KB - downloaded 0 times.)
//  http://forums.codeblocks.org/index.php?topic=1594.msg13387#msg13387
//
//  Added scrolling in text and textCtrl windows

// V0.7 12/31/2005
//  ---------------------------------------------------------------------------
// Removed event.Skip() when first Right mouse key down to avoid in text/textctrl
// the event.Skip() was activating text/textctrl mouse activity in codeblocks
// eg., mouse RightKeydown in "Search results" caused loading of editors etc.
//  ---------------------------------------------------------------------------
// v0.11 1/19/2006
// CodeBlocks SDK version changed. Modified BuildModuleMenu().
// void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){ return; }
//
//  mod     v0.13 1/22/2006
//          Added GTK scrolling
//
//  mod     v0.14 1/25/2006 12
//          Added cbConfiguratonDialog for configuration dialog
// ----------------------------------------------------------------------------
//  commit  v0.15 2/3/2006 1:03 PM
// ----------------------------------------------------------------------------
//
//  closed  v0.15 2/3/2006 12 open 1/26/2006 5:41 PM
//          Need to stow/read user configuration settings
//          Make MSW changes for GTK
//          Removed-Test on ubuntu
//          Linux version of C::B doesnt compile; will wait for Linux nightly builds(if ever provided)
//
// ----------------------------------------------------------------------------
//  commit  v0.16 2/4/2006
// ----------------------------------------------------------------------------
//  mod     fixes for unix/GTK
//
// ----------------------------------------------------------------------------
//  commit  v0.17 2/13/2006
// ----------------------------------------------------------------------------
//  mod     2/11/2006 7
//          Set focus to editor when mouse in editor window.
//  open    2/11/2006 8
//          mod middle mouse key to avoid waits/delays
//  mod     2/13/2006 9
//          CB_IMPLEMENT_PLUGIN(cbDragScroll, "DragScroll" );
//
// ----------------------------------------------------------------------------
//  commit  v0.18 2/14/2006
// ----------------------------------------------------------------------------
//  mod     2/14/2006 3
//          Added "Mouse sets Editor Focus" configuration item
//
// ----------------------------------------------------------------------------
//  commit  v0.19 committed 2006/03/21
// ----------------------------------------------------------------------------
//  mod     2/25/2006
//          1.Added "Mouse sets Editor Focus" for GTK
//          2.Capture GTK middle mouse key immediately when used for scrolling
// ----------------------------------------------------------------------------
//  commit  v0.20 2006/04/5
// ----------------------------------------------------------------------------
//  closed  2006/04/6 open    2006/04/5
//          Conflict with Build messages context menu
//          Removed events for ListCtrl windows
// ----------------------------------------------------------------------------
//  commit  v0.21 2006/04/6
// ----------------------------------------------------------------------------
//  closed  2006/04/6
//          Resolution of above: event.Skip() on Right mouse Key down.
//          Put back events for listctrl windows
//          Catch address of Search Results window for scrolling.
// ----------------------------------------------------------------------------
//  open    2006/04/8
//          listCtrl windows activate on right mouse click. eg, Search and compiler
//          error windows move the editor window on "right click". Very annoying.
//          Suggest option to hide right mouse keydown from listCtrls
//          Added config option []"Smooth Message List Scrolling"
//                              "(Conflicts with some Context Menus)"
//          Set the Editor focus and Smooth Scrolling to default=false
// ----------------------------------------------------------------------------
//  testing v0.22 2006/04/8 Capture ListCtrl Right Mouse Keydown
// ----------------------------------------------------------------------------
//  fix     v0.23 2006/04/25
//          Added MS windows test for main window because events were getting
//          to mouse handler even though main window didnt have focus
// ----------------------------------------------------------------------------
//  commit  v0.23 2006/04/25
// ----------------------------------------------------------------------------
//  closed  opened    2006/06/11
//          split windows are unrecognized because no event is issued
//          that a split has taken place
//          Had to add wxEVT_CREATE and wxEVT_DESTROY event sinks to catch
//          split window open/close. wxWindows nor CodeBlocks has events
//          usable for the purpose.
// ----------------------------------------------------------------------------
//  commit  v0.24   2006/06/14
// ----------------------------------------------------------------------------
//  closed  2006/06/16 open    2006/06/15
//          MyMouseEvents are being leaked because of split windows. When the
//          windows close, no event is sent to allow cleanup before Destroy()
//          Deleting an eventHandler during Destroy() causes wxWidgets to crash.
//
//          Switched to runtime Connect/Disconnect/EVT_CREATE/EVT_DESTROY
//          in order to stop leaks on split windows & pushed event handlers.
// ----------------------------------------------------------------------------
//  fixed   v 0.26 2006/06/29
//          Broken by change in plugin interface 1.80
//          Had to add the following to the project file
//
//          Compile Options         #defines
//          -Winvalid-pch           BUILDING_PLUGIN
//          -pipe                   CB_PRECOMP
//          -mthreads               WX_PRECOMP
//          -fmessage-length=0      HAVE_W32API_H
//          -fexceptions            __WXMSW__
//          -include "sdk.h"        WXUSINGDLL
//                                  cbDEBUG
//                                  TIXML_USE_STL
//                                  wxUSE_UNICODE
// ----------------------------------------------------------------------------
//  commit  v0.26 2006/06/29
// ----------------------------------------------------------------------------
//  closed  2006/09/11 open    2006/07/2
//          Clean up code after conversion to Connec/Disconnect event handlers
// ----------------------------------------------------------------------------
//  open    2006/09/11
//          Complaints that config font was too small on some linux systems
//          and that the background color was incorrect.
//  closed  2006/09/11
//          Removed all Setfont()'s from config. Removed SetBackGroundColorColor()
// ----------------------------------------------------------------------------
//  ToDo
//          2006/10/23 Determine attachable windows via RTTI
//          2006/12/19 Add user specified RightKey drag dwell for GTK
// ----------------------------------------------------------------------------
//  commit  v0.28 2006/09/11
// ----------------------------------------------------------------------------
//  Commit  v0.29 2006/09/22
//          Edited manifest.xml requirement for codeblocks plugins
//          Set displayed Menu About version dynamically.
//          Removed all "eq". Conflicted with wxWidgest hash equates
//          Added (__WXMAC__) to (_WXGTK_)defines to support mac.
// ----------------------------------------------------------------------------
//  Commit  1.0.30 2006/10/16
//          Add focus follow mouse option for MSW
// ----------------------------------------------------------------------------
//  Commit  1.0.31 2006/10/18
//          Default Auto focus editor to OFF
// ----------------------------------------------------------------------------
//  Commit  1.0.32
//          Determine RTTI GetClassName() in mouse event
// ----------------------------------------------------------------------------
//  Commit  1.0.33 2006/12/19
//          33) Removed dependency on EVT_APP_STARTUP_DONE
// ----------------------------------------------------------------------------
//  Commit  1.0.36 2006/12/19 - 2006/12/22
//          34) Added focus follows mouse for wxGTK
//          35) Fixed GTK RightMouse scrolling (avoiding Context Menu conflict)
//          36) Added slider allowing user to set GTK RightMouse scrolling/context menu sensitivity
// ----------------------------------------------------------------------------
//  Commit  1.0.38
//          37) Re-instated GTK wxTextCtrl y-axis scrolling
//              GTK cannot scroll a wxListCtrl
//          38) Corrected GTK dialog layout
// ----------------------------------------------------------------------------
//  Commit  1.0.39
//          39) If exists executable folder .ini file, use it.
// ----------------------------------------------------------------------------
#endif // DRAGSCROLL_H
