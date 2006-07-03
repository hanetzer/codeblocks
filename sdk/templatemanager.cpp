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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/menu.h>
    #include <wx/filename.h>
    #include <wx/msgdlg.h>
    #include <wx/log.h>

    #include "templatemanager.h"
    #include "manager.h"
    #include "configmanager.h"
    #include "messagemanager.h"
    #include "projectmanager.h"
    #include "cbproject.h"
    #include "globals.h"
    #include "compilerfactory.h"
    #include <wx/dir.h>
#endif

#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include "filefilters.h"
#include "newfromtemplatedlg.h"

TemplateManager::TemplateManager()
{
	//ctor
	Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

TemplateManager::~TemplateManager()
{
	//dtor
    // this is a core manager, so it is removed when the app is shutting down.
    // in this case, the app has already un-hooked us, so no need to do it ourselves...
//	Manager::Get()->GetAppWindow()->RemoveEventHandler(this);
}

void TemplateManager::CreateMenu(wxMenuBar* menuBar)
{
}

void TemplateManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

void TemplateManager::BuildToolsMenu(wxMenu* menu)
{
}

void TemplateManager::LoadUserTemplates()
{
    wxLogNull zero; // disable error logging

    m_UserTemplates.Clear();
    wxString baseDir = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + _T("UserTemplates");

    wxDir dir(baseDir);

    if (!dir.IsOpened())
        return;

    wxString filename;
    bool ok = dir.GetFirst(&filename, _T("*"), wxDIR_DIRS);
    while (ok)
    {
        m_UserTemplates.Add(filename);
        ok = dir.GetNext(&filename);
    }

	Manager::Get()->GetMessageManager()->DebugLog(_T("%d user templates loaded"), m_UserTemplates.GetCount());
}

cbProject* TemplateManager::NewProject()
{
	cbProject* prj = NULL;

	LoadUserTemplates();
	NewFromTemplateDlg dlg(m_UserTemplates);
    PlaceWindow(&dlg);
	if (dlg.ShowModal() == wxID_OK)
	{
        if (dlg.SelectedUserTemplate())
            prj = NewProjectFromUserTemplate(dlg);
        else
            prj = NewProjectFromTemplate(dlg);
	}
	return prj;
}

cbProject* TemplateManager::NewProjectFromTemplate(NewFromTemplateDlg& dlg)
{
	cbProject* prj = NULL;
    cbWizardPlugin* wiz = dlg.GetWizard();
	if (wiz)
	{
		// wizard, too easy ;)
		CompileTargetBase *ret = wiz->Launch(dlg.GetWizardIndex());
		switch (wiz->GetOutputType(dlg.GetWizardIndex()))
		{
		    case cbWizardPlugin::otProject: prj = dynamic_cast<cbProject*>(ret); break;
		    default: break;
		}
    }
    return prj;
}

cbProject* TemplateManager::NewProjectFromUserTemplate(NewFromTemplateDlg& dlg)
{
    cbProject* prj = NULL;
    if (!dlg.SelectedUserTemplate())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("TemplateManager::NewProjectFromUserTemplate() called when no user template was selected ?!?"));
        return NULL;
    }

    wxString path = Manager::Get()->GetConfigManager(_T("template_manager"))->Read(_T("/projects_path"));
    wxString sep = wxFileName::GetPathSeparator();
    // select directory to copy user template files
    path = ChooseDirectory(0, _("Choose a directory to create the new project"),
                        path, _T(""), false, true);
    if (path.IsEmpty())
    {
        return NULL;
    }
    else if(path.Mid(path.Length() - 1) == wxFILE_SEP_PATH)
    {
        path.RemoveLast();
    }

    wxBusyCursor busy;

    wxString templ = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + _T("UserTemplates");
    templ << sep << dlg.GetSelectedUserTemplate();
    if (!wxDirExists(templ))
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("Cannot open user-template source path '%s'!"), templ.c_str());
        return NULL;
    }

    // copy files
    wxString project_filename;
    wxArrayString files;
    wxDir::GetAllFiles(templ, &files);
    int count = 0;
    int total_count = files.GetCount();
    for (size_t i = 0; i < files.GetCount(); ++i)
    {
        wxFileName dstname(files[i]);
        dstname.MakeRelativeTo(templ + sep);
        wxString src = files[i];
        wxString dst = path + sep + dstname.GetFullPath();
//        Manager::Get()->GetMessageManager()->DebugLog("dst=%s, dstname=%s", dst.c_str(), dstname.GetFullPath().c_str());
        if (!CreateDirRecursively(dst))
            Manager::Get()->GetMessageManager()->DebugLog(_T("Failed creating directory for %s"), dst.c_str());
        if (wxCopyFile(src, dst, true))
        {
            if (FileTypeOf(dst) == ftCodeBlocksProject)
                project_filename = dst;
            ++count;
        }
        else
            Manager::Get()->GetMessageManager()->DebugLog(_T("Failed copying %s to %s"), src.c_str(), dst.c_str());
    }
    if (count != total_count)
        cbMessageBox(_("Some files could not be loaded with the template..."), _("Error"), wxICON_ERROR);
    else
    {
        // open new project
        if (project_filename.IsEmpty())
            cbMessageBox(_("User-template saved succesfuly but no project file exists in it!"));
        else
        {
        	// ask to rename the project file, if need be
        	wxFileName fname(project_filename);
        	wxString newname = wxGetTextFromUser(_("If you want, you can change the project's filename here (without extension):"), _("Change project's filename"), fname.GetName());
        	if (!newname.IsEmpty() && newname != fname.GetName())
        	{
        		fname.SetName(newname);
        		wxRenameFile(project_filename, fname.GetFullPath());
        		project_filename = fname.GetFullPath();
        	}
            prj = Manager::Get()->GetProjectManager()->LoadProject(project_filename);
            if(prj && !newname.IsEmpty())
            {
		prj->SetTitle(newname);
		Manager::Get()->GetProjectManager()->RebuildTree(); // so the tree shows the new name
		// TO DO : title update of window --> ??how ; throw an acitivate or opened event ??
            }
        }
    }
    return prj;
}

void TemplateManager::SaveUserTemplate(cbProject* prj)
{
    wxLogNull ln; // we check everything ourselves

    if (!prj)
        return;

    // save project & all files
    if (!prj->SaveAllFiles() ||
        !prj->Save())
    {
        cbMessageBox(_("Could not save project and/or all its files. Aborting..."), _("Error"), wxICON_ERROR);
        return;
    }

    // create destination dir
    wxString templ = ConfigManager::GetConfigFolder() + wxFILE_SEP_PATH + _T("UserTemplates");
    if (!CreateDirRecursively(templ, 0755))
    {
        cbMessageBox(_("Couldn't create directory for user templates:\n") + templ, _("Error"), wxICON_ERROR);
        return;
    }

    // check if it exists and ask a different title
    wxString title = prj->GetTitle();
    while (true)
    {
        // ask for template title (unique)
        wxTextEntryDialog dlg(0, _("Enter a title for this template"), _("Enter title"), title);
        PlaceWindow(&dlg);
        if (dlg.ShowModal() != wxID_OK)
            return;

        title = dlg.GetValue();
        if (!wxDirExists(templ + wxFILE_SEP_PATH + title))
        {
            templ << wxFILE_SEP_PATH << title;
            wxMkdir(templ, 0755);
            break;
        }
        else
            cbMessageBox(_("You have another template with the same title.\nPlease choose another title..."));
    }

    wxBusyCursor busy;

    // copy project and all files to destination dir
    int count = 0;
    int total_count = prj->GetFilesCount();
    templ << wxFILE_SEP_PATH;
    wxFileName fname;
    for (int i = 0; i < total_count; ++i)
    {
        wxString src = prj->GetFile(i)->file.GetFullPath();
        wxString dst = templ + prj->GetFile(i)->relativeToCommonTopLevelPath;
        Manager::Get()->GetMessageManager()->DebugLog(_T("Copying %s to %s"), src.c_str(), dst.c_str());
        if (!CreateDirRecursively(dst))
            Manager::Get()->GetMessageManager()->DebugLog(_T("Failed creating directory for %s"), dst.c_str());
        if (wxCopyFile(src, dst, true))
            ++count;
        else
            Manager::Get()->GetMessageManager()->DebugLog(_T("Failed copying %s to %s"), src.c_str(), dst.c_str());
    }

    // cbProject doesn't have a GetRelativeToCommonTopLevelPath() function, so we simulate it here
    // to find out the real destination file to create...
    wxString topLevelPath = prj->GetCommonTopLevelPath();
    fname.Assign(prj->GetFilename());
    fname.MakeRelativeTo(topLevelPath);
    fname.Assign(templ + fname.GetFullPath());
    if (!CreateDirRecursively(fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR), 0755))
    {
        cbMessageBox(_("Failed to create the directory for the project file!"), _("Error"), wxICON_ERROR);
        ++count;
    }
    else
    {
        if (!wxCopyFile(prj->GetFilename(), fname.GetFullPath()))
        {
            Manager::Get()->GetMessageManager()->DebugLog(_T("Failed to copy the project file: %s"), fname.GetFullPath().c_str());
            cbMessageBox(_("Failed to copy the project file!"), _("Error"), wxICON_ERROR);
            ++count;
        }
    }

    if (count == total_count)
        cbMessageBox(_("User-template saved succesfuly"), _("Information"), wxICON_INFORMATION | wxOK);
    else
        cbMessageBox(_("Some files could not be saved with the template..."), _("Error"), wxICON_ERROR);
}
