/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "app.h"
#include <wx/fs_zip.h>
#include <wx/xrc/xmlres.h>
#include <wx/cmdline.h>
#include <wx/regex.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include "../sdk/editormanager.h"
#include "../sdk/projectmanager.h"

#ifdef __WXMSW__
	#include <wx/msw/registry.h>
	#include <shlobj.h> // for SHChangeNotify()
	#define DDE_SERVICE	"CODEBLOCKS"
	#define DDE_TOPIC	"CodeBlocksDDEServer"
#else
    #include "prefix.h" // binreloc
#endif

#include "globals.h"

#include "../sdk/configmanager.h"

#if wxUSE_CMDLINE_PARSER
static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _T("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, _T("na"), _T("no-check-associations"), _T("don't perform any association checks"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T("ns"), _T("no-splash-screen"), _T("don't display a splash screen while loading"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, _T("d"), _T("debug-log"), _T("display application's debug log"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, _T(""), _T("prefix"),  _T("the shared data dir prefix"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_PARAM, _T(""), _T(""),  _T("filename(s)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
    { wxCMD_LINE_NONE }
};
#endif // wxUSE_CMDLINE_PARSER

#ifdef __WXMSW__
DDEServer* g_DDEServer = 0L;
#endif

IMPLEMENT_APP(CodeBlocksApp)

BEGIN_EVENT_TABLE(CodeBlocksApp, wxApp)
	EVT_ACTIVATE_APP(CodeBlocksApp::OnAppActivate)
END_EVENT_TABLE()

bool CodeBlocksApp::OnInit()
{
    m_pSplash = 0;
    wxHandleFatalExceptions(true);

#ifdef __WXMSW__
    m_ExceptionHandlerLib = LoadLibrary("exchndl.dll");
#endif

    if (ParseCmdLine(0L) == -1)
        return false;

    wxImage::AddHandler(new wxBMPHandler);
    wxImage::AddHandler(new wxPNGHandler);
	wxInitAllImageHandlers();

    SetVendorName(APP_VENDOR);
    SetAppName(APP_NAME" v"APP_VERSION);
    ConfigManager::Init(wxConfigBase::Get());

    ShowSplashScreen();

    // load all application-resources
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();

    ConfigManager::Get()->Write("app_path", GetAppPath());
    ConfigManager::Get()->Write("data_path", GetAppPath() + "/share/CodeBlocks");
    m_HasDebugLog = ConfigManager::Get()->Read("/message_manager/has_debug_log", (long int)0) | m_HasDebugLog;
    ConfigManager::Get()->Write("/message_manager/has_debug_log", m_HasDebugLog);

    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxString res = resPath + "/resources.zip";
    if (!CheckResource(res))
    	return false;
    /// @todo Checkout why it doesn't work with VC++ unless "#zip:*.xrc" appended
	wxXmlResource::Get()->Load(res+"#zip:*.xrc");

    MainFrame *frame = new MainFrame((wxFrame*)0L);
    frame->Show(TRUE);
    SetTopWindow(frame);

    if (ParseCmdLine(frame) == 0)
		Manager::Get()->GetProjectManager()->LoadWorkspace();

#ifdef __WXMSW__
	if (!m_NoAssocs && ConfigManager::Get()->Read("/environment/check_associations", 1) == 1)
		CheckAssociations();

	if (ConfigManager::Get()->Read("/environment/use_dde", 1) == 1)
	{
		g_DDEServer = new DDEServer(frame);
		g_DDEServer->Create(DDE_SERVICE);
	}
#endif

    HideSplashScreen();

    frame->ShowTips(); // this func checks if the user wants tips, so no need to check here

#ifdef __WXMSW__
    // for windows users only, display a message that no compiler is provided
    if (ConfigManager::Get()->Read("version", "") != APP_ACTUAL_VERSION)
    {
        // this is a (probably) newer version; show a message box with
        // important notes
        
        wxString msg;
        msg = _("Welcome to " APP_NAME "!\n\n"
                "" APP_NAME " is a front-end for various compilers. It does not install "
                "any compiler on the system.\nIt is up to you to install the "
                "compiler(s) of your choice and configure " APP_NAME ", if necessary, to use it.\n"
                "To make things easier for you, " APP_NAME " comes with predefined settings "
                "for three major compilers:\n\n"
                "1. GNU GCC (http://www.mingw.org)\n"
                "2. Microsoft's Free Visual C++ Toolkit 2003 (http://msdn.microsoft.com/visualc/vctoolkit2003/)\n"
                "3. Borland C++ Compiler 5.5 (http://www.borland.com/products/downloads/download_cbuilder.html#)\n\n"
                "The above compilers are freely available on the net. Download "
                "and install any one of them (or all three of them!). If you install "
                "one of these compilers in its pre-defined installation path, there is nothing "
                "more you need to do. " APP_NAME " is already configured for the default installations "
                "of the above compilers!");
        wxMessageBox(msg, _("Information"), wxICON_INFORMATION);
        
        // update the version
        ConfigManager::Get()->Write("version", APP_ACTUAL_VERSION);
    }
#endif
	return TRUE;
}

int CodeBlocksApp::OnExit()
{
#ifdef __WXMSW__
	if (g_DDEServer)
		delete g_DDEServer;
    if (m_ExceptionHandlerLib)
        FreeLibrary(m_ExceptionHandlerLib);
#endif
    return 0;
}

bool CodeBlocksApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    return wxApp::OnCmdLineParsed(parser);
}

void CodeBlocksApp::OnFatalException()
{
    wxMessageBox(_("Something terrible has happened to " APP_NAME " and it "
                    "will terminate immediately.\n"
                    "We are sorry for the inconvenience..."));
}

void CodeBlocksApp::ShowSplashScreen()
{
    HideSplashScreen();

	if (!m_NoSplash && ConfigManager::Get()->Read("/environment/show_splash", 1) == 1)
	{
		wxBitmap bitmap;
		if (bitmap.LoadFile(GetAppPath() + "/share/CodeBlocks/images/splash.png", wxBITMAP_TYPE_PNG))
		{
			m_pSplash = new wxSplashScreen(bitmap,
										wxSPLASH_CENTRE_ON_SCREEN,// | wxSPLASH_TIMEOUT,
										6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
										wxSIMPLE_BORDER);// | wxSTAY_ON_TOP);
		}
		wxYield();
	}
}

void CodeBlocksApp::HideSplashScreen()
{
    if (m_pSplash)
        delete m_pSplash;
    m_pSplash = 0;
}

bool CodeBlocksApp::CheckResource(const wxString& res)
{
    if (!wxFileExists(res))
    {
        HideSplashScreen();
    	wxString msg;
    	msg.Printf("Cannot find %s...\n"
    		APP_NAME" was configured to be installed in '%s'.\n"
    		"Please use the command-line switch '--prefix' or "
            "set the DATA_PREFIX environment variable "
    		"to point where "APP_NAME" is installed,\n"
    		"or try re-installing the application...",
    		res.c_str(),
    		ConfigManager::Get()->Read("app_path", wxEmptyString).c_str());
    	wxMessageBox(msg);
    	return false;
    }

    return true;
}

wxString CodeBlocksApp::GetAppPath() const
{
    wxString base;
#ifdef __WXMSW__
    wxChar name[MAX_PATH] = {0};
    GetModuleFileName(0L, name, MAX_PATH);
    wxFileName fname(name);
    base = fname.GetPath(wxPATH_GET_VOLUME);
#else
    // SELFPATH is a macro from prefix.h (binreloc)
    // it returns the absolute filename of us
    // similar to win32 GetModuleFileName()...
    base = wxFileName(SELFPATH).GetPath();
#endif
    return base;
}

int CodeBlocksApp::ParseCmdLine(MainFrame* handlerFrame)
{
    // code shamelessely taken from the console wxWindows sample :)
	bool filesInCmdLine = false;

#if wxUSE_CMDLINE_PARSER
#if wxUSE_UNICODE
    wxChar **wargv = new wxChar *[argc + 1];

    {
        for ( int n = 0; n < argc; n++ )
        {
            wxMB2WXbuf warg = wxConvertMB2WX(argv[n]);
            wargv[n] = wxStrdup(warg);
        }

        wargv[n] = NULL;
    }

    #define argv wargv
#endif // wxUSE_UNICODE

    wxCmdLineParser parser(cmdLineDesc, argc, argv);

#if wxUSE_UNICODE
    {
        for ( int n = 0; n < argc; n++ )
            free(wargv[n]);

        delete [] wargv;
    }
#endif // wxUSE_UNICODE

    switch ( parser.Parse() )
    {
        case -1:
            return -1;

        case 0:
            {
//                if (handlerFrame)
//                {
//                    int count = parser.GetParamCount();
//					filesInCmdLine = count != 0;
//                    for ( int param = 0; param < count; ++param )
//                        handlerFrame->Open(parser.GetParam(param));
//                }
//                else
                {
                    wxString val;
                    if (parser.Found(_("prefix"), &val))
                        wxSetEnv("DATA_PREFIX", val);
					m_NoAssocs = parser.Found(_("no-check-associations"), &val);
					m_NoSplash = parser.Found(_("no-splash-screen"), &val);
					m_HasDebugLog = parser.Found(_("debug-log"), &val);
						
                }
            }
            break;

        default:
//            wxLogMessage(_T("Syntax error detected, aborting."));
            break;
    }
#endif // wxUSE_CMDLINE_PARSER
    return filesInCmdLine ? 1 : 0;
}

#ifdef __WXMSW__
void CodeBlocksApp::SetAssociations()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);
	
	DoSetAssociation(CODEBLOCKS_EXT, APP_NAME" project file", name, "1");
	DoSetAssociation(WORKSPACE_EXT, APP_NAME" workspace file", name, "1");
	DoSetAssociation(C_EXT, "C source file", name, "2");
	DoSetAssociation(CPP_EXT, "C++ source file", name, "3");
	DoSetAssociation(CC_EXT, "C++ source file", name, "3");
	DoSetAssociation(CXX_EXT, "C++ source file", name, "3");
	DoSetAssociation(H_EXT, "C/C++ header file", name, "4");
	DoSetAssociation(HPP_EXT, "C/C++ header file", name, "4");
	DoSetAssociation(HH_EXT, "C/C++ header file", name, "4");
	DoSetAssociation(HXX_EXT, "C/C++ header file", name, "4");
	
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0L, 0L);
}

void CodeBlocksApp::DoSetAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
	wxRegKey key; // defaults to HKCR
	
	key.SetName("HKEY_CLASSES_ROOT\\." + ext);
	key.Create();
	key = "CodeBlocks." + ext;
	
	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext);
	key.Create();
	key = descr;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\DefaultIcon");
	key.Create();
	key = exe + "," + icoNum;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\command");
	key.Create();
	key = "\"" + exe + "\" \"%1\"";

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\ddeexec");
	key.Create();
	key = "[Open(\"%1\")]";

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\ddeexec\\application");
	key.Create();
	key = DDE_SERVICE;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\ddeexec\\topic");
	key.Create();
	key = DDE_TOPIC;
}

// sets file associations if not there (depending on user prefs)
void CodeBlocksApp::CheckAssociations()
{
	wxChar name[MAX_PATH] = {0};
	GetModuleFileName(0L, name, MAX_PATH);
	
	if (!DoCheckAssociation(CODEBLOCKS_EXT, APP_NAME" project file", name, "1") ||
        !DoCheckAssociation(WORKSPACE_EXT, APP_NAME" workspace file", name, "1") ||
        !DoCheckAssociation(C_EXT, "C source file", name, "2") ||
        !DoCheckAssociation(CPP_EXT, "C++ source file", name, "3") ||
        !DoCheckAssociation(CC_EXT, "C++ source file", name, "3") ||
        !DoCheckAssociation(CXX_EXT, "C++ source file", name, "3") ||
        !DoCheckAssociation(H_EXT, "C/C++ header file", name, "4") ||
        !DoCheckAssociation(HPP_EXT, "C/C++ header file", name, "4") ||
        !DoCheckAssociation(HH_EXT, "C/C++ header file", name, "4") ||
        !DoCheckAssociation(HXX_EXT, "C/C++ header file", name, "4"))
    {
        HideSplashScreen();
        wxString msg;
        msg.Printf(_("%s is not currently the default application for C/C++ source files\nDo you want to set it as default?"), APP_NAME);
        int answer = wxMessageBox(msg,
                                    _("File associations"),
                                    wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT);
        if (answer == wxYES)
            SetAssociations();
        else
        {
            wxMessageBox(_("File associations will *not* be checked from now on, on program startup.\n"
                           "If you want to enable the check, go to \"Settings/Environment\" and check \"Check & set file associations\"..."),
                         _("Information"),
                         wxICON_INFORMATION);
            ConfigManager::Get()->Write("/environment/check_associations", 0L);
        }
    }
}

// returns true if association is already established
bool CodeBlocksApp::DoCheckAssociation(const wxString& ext, const wxString& descr, const wxString& exe, const wxString& icoNum)
{
    wxLogNull no_log_here;
	wxRegKey key; // defaults to HKCR
	
	key.SetName("HKEY_CLASSES_ROOT\\." + ext);
	if (!key.Open())
        return false;
	
	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext);
	if (!key.Open())
        return false;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\DefaultIcon");
	if (!key.Open())
        return false;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\command");
	if (!key.Open())
        return false;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\ddeexec");
	if (!key.Open())
        return false;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\ddeexec\\application");
	if (!key.Open())
        return false;

	key.SetName("HKEY_CLASSES_ROOT\\CodeBlocks." + ext + "\\shell\\open\\ddeexec\\topic");
	if (!key.Open())
        return false;
    
    return true;
}

//// DDE

wxConnectionBase* DDEServer::OnAcceptConnection(const wxString& topic)
{
    if (topic == DDE_TOPIC)
        return new DDEConnection(m_Frame);

    // unknown topic
    return 0L;
}

bool DDEConnection::OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format)
{
	wxString strData = data;
	if (!strData.StartsWith("[Open(\""))
		return false;

	wxRegEx reCmd("\"(.*)\"");
	if (reCmd.Matches(strData))
	{
		wxString file = reCmd.GetMatch(strData, 1);
        m_Frame->Open(file, false); // don't add to history, files that open through DDE
	}

    return true;
}
#endif

// event handlers

void CodeBlocksApp::OnAppActivate(wxActivateEvent& event)
{
	if (!event.GetActive())
		return;
    
    Manager::Get()->GetEditorManager()->CheckForExternallyModifiedFiles();
	cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
	if (ed)
		ed->GetControl()->SetFocus();
}
