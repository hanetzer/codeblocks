//------------------------------------------------------------------------------
// Header generated by wxDesigner from file: stedlgs.wdr
// Do not modify this file, all changes will be lost!
//------------------------------------------------------------------------------

#ifndef __WDR_stedlgs_H__
#define __WDR_stedlgs_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "stedlgs_wdr.h"
#endif

// Include wxWidgets' headers

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/statline.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/toolbar.h>
#include <wx/tglbtn.h>

// Declare window functions

const int ID_STEDLG_STYLE_NOTEBOOK = 20000;
const int ID_STEDLG_STYLELANG_CHOICE = 20001;
const int ID_LINE = 20002;
const int ID_STEDLG_FONT_CHECKBOX = 20003;
const int ID_STEDLG_FONT_BUTTON = 20004;
const int ID_STEDLG_FONT_CHOICE = 20005;
const int ID_STEDLG_FONTSIZE_CHECKBOX = 20006;
const int ID_STEDLG_FONTSIZE_SPINCTRL = 20007;
const int ID_STEDLG_ATTRIBUTES_CHECKBOX = 20008;
const int ID_STEDLG_BOLD_CHECKBOX = 20009;
const int ID_STEDLG_ITALICS_CHECKBOX = 20010;
const int ID_STEDLG_UNDERLINE_CHECKBOX = 20011;
const int ID_STEDLG_EOLFILL_CHECKBOX = 20012;
const int ID_STEDLG_FONTFORE_CHECKBOX = 20013;
const int ID_STEDLG_FONTFORE_BUTTON = 20014;
const int ID_STEDLG_FONTBACK_CHECKBOX = 20015;
const int ID_STEDLG_FONTBACK_BUTTON = 20016;
wxSizer *wxSTEditorStyleSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_TEXT = 20017;
const int ID_STEDLG_LANG_CHOICE = 20018;
const int ID_STEDLG_FILEPATTERN_TEXTCTRL = 20019;
const int ID_STEDLG_LANG_NOTEBOOK = 20020;
wxSizer *wxSTEditorLangSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEPROP_FILENAME_TEXTCTRL = 20021;
const int ID_STEPROP_FILESIZE_TEXT = 20022;
const int ID_STEPROP_FILEOPENED_TEXT = 20023;
const int ID_STEPROP_FILEMODIFIED_TEXT = 20024;
const int ID_STEPROP_FILEACCESSED_TEXT = 20025;
const int ID_STEPROP_FILECREATED_TEXT = 20026;
const int ID_STEPROP_LANGUAGE_TEXT = 20027;
const int ID_STEPROP_NUMLINES_TEXT = 20028;
const int ID_STEPROP_NUMCHARS_TEXT = 20029;
const int ID_STEPROP_NUMWORDS_TEXT = 20030;
const int ID_STEPROP_NUMTABS_TEXT = 20031;
const int ID_STEPROP_EOLCHARS_TEXT = 20032;
wxSizer *wxSTEditorPropertiesSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_FIND_COMBO = 20033;
const int ID_STEDLG_FIND_BITMAPBUTTON = 20034;
const int ID_STEDLG_REPLACE_TEXT = 20035;
const int ID_STEDLG_REPLACE_COMBO = 20036;
const int ID_STEDLG_REPLACE_BITMAPBUTTON = 20037;
const int ID_STEDLG_WHOLEWORD_CHECKBOX = 20038;
const int ID_STEDLG_BACKWARDS_CHECKBOX = 20039;
const int ID_STEDLG_MATCHCASE_CHECKBOX = 20040;
const int ID_STEDLG_WRAP_CHECKBOX = 20041;
const int ID_STEDLG_WORDSTART_CHECKBOX = 20042;
const int ID_STEDLG_REGEXP_FIND_CHECKBOX = 20043;
const int ID_STEDLG_FINDALL_CHECKBOX = 20044;
const int ID_STEDLG_BOOKMARKALL_CHECKBOX = 20045;
const int ID_STEDLG_SCOPEWHOLE_RADIOBUTTON = 20046;
const int ID_STEDLG_SCOPECURSOR_RADIOBUTTON = 20047;
const int ID_STEDLG_SCOPEALLDOCS_RADIOBUTTON = 20048;
const int ID_STEDLG_FIND_BUTTON = 20049;
const int ID_STEDLG_REPLACE_BUTTON = 20050;
const int ID_STEDLG_REPLACEFIND_BUTTON = 20051;
const int ID_STEDLG_REPLACEALL_BUTTON = 20052;
wxSizer *wxSTEditorFindReplaceSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_INSERT_PREPEND_RADIOBUTTON = 10;
const int ID_STEDLG_INSERT_APPEND_RADIOBUTTON = 11;
const int ID_STEDLG_INSERT_ATCOLUMN_RADIOBUTTON = 12;
const int ID_STEDLG_INSERT_COLUMN_SPINCTRL = 20053;
const int ID_STEDLG_INSERT_SURROUND_RADIOBUTTON = 13;
const int ID_STEDLG_INSERT_PREPEND_TEXT = 20054;
const int ID_STEDLG_INSERT_PREPEND_COMBO = 20055;
const int ID_STEDLG_INSERT_PREPEND_BITMAPBUTTON = 20056;
const int ID_STEDLG_INSERT_APPEND_COMBO = 20057;
const int ID_STEDLG_INSERT_APPEND_BITMAPBUTTON = 20058;
const int ID_STEDLG_INSERT_EDITOR = 20059;
wxSizer *wxSTEditorInsertTextSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_ZOOM_SPINCTRL = 20060;
const int ID_STEDLG_EDGE_MODE_CHOICE = 20061;
const int ID_STEDLG_EDGE_COLUMN_SPINCTRL = 20062;
const int ID_STEDLG_VIEW_LINEMARGIN_CHECKBOX = 20063;
const int ID_STEDLG_VIEW_MARKERMARGIN_CHECKBOX = 20064;
const int ID_STEDLG_CARET_LINE_VISIBLE_CHECKBOX = 20065;
const int ID_STEDLG_CARET_WIDTH_SPINCTRL = 20066;
const int ID_STEDLG_CARET_PERIOD_SPINCTRL = 20067;
wxSizer *wxSTEditorViewPrefsSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_PRINT_COLOURMODE_CHOICE = 20068;
const int ID_STEDLG_PRINT_MAGNIFICATION_SPINCTRL = 20069;
const int ID_STEDLG_PRINT_WRAPMODE_CHECKBOX = 20070;
const int ID_STEDLG_PRINT_LINENUMBERS_CHOICE = 20071;
wxSizer *wxSTEditorPrintPrefsSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_USE_TABS_CHECKBOX = 20072;
const int ID_STEDLG_TAB_WIDTH_SPINCTRL = 20073;
const int ID_STEDLG_TAB_INDENTS_CHECKBOX = 20074;
const int ID_STEDLG_INDENT_WIDTH_SPINCTRL = 20075;
const int ID_STEDLG_BACKSPACE_UNINDENTS_CHECKBOX = 20076;
const int ID_STEDLG_AUTOINDENT_CHECKBOX = 20077;
const int ID_STEDLG_INDENT_GUIDES_CHECKBOX = 20078;
const int ID_STEDLG_EOL_MODE_CHOICE = 20079;
const int ID_STEDLG_VIEW_EOL_CHECKBOX = 20080;
const int ID_STEDLG_VIEW_WHITESPACE_CHECKBOX = 20081;
wxSizer *wxSTEditorTabsPrefsSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_HIGHLIGHT_SYNTAX_CHECKBOX = 20082;
const int ID_STEDLG_HIGHLIGHT_PREPROC_CHECKBOX = 20083;
const int ID_STEDLG_HIGHLIGHT_BRACES_CHECKBOX = 20084;
const int ID_STEDLG_LOAD_INIT_LANG_CHECKBOX = 20085;
wxSizer *wxSTEditorHighlightingPrefsSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_VIEW_FOLDMARGIN_CHECKBOX = 20086;
const int ID_STEDLG_FOLD_STYLES_LISTBOX = 20087;
const int ID_STEDLG_FOLDMARGIN_STYLE_CHOICE = 20088;
const int ID_STEDLG_WRAP_MODE_CHECKBOX = 20089;
const int ID_STEDLG_WRAP_VISUALFLAGS_CHOICE = 20090;
const int ID_STEDLG_WRAP_VISUALFLAGSLOC_CHOICE = 20091;
const int ID_STEDLG_WRAP_STARTINDENT_SPINCTRL = 20092;
wxSizer *wxSTEditorFoldPrefsSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_WINDOWS_LISTBOX = 20093;
const int ID_STEDLGS_WINDOWS_ACTIVATE_BUTTON = 20094;
const int ID_STEDLGS_WINDOWS_SAVE_BUTTON = 20095;
const int ID_STEDLGS_WINDOWS_CLOSE_BUTTON = 20096;
wxSizer *wxSTEditorWindowsSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_COLUMNIZE_BEFORE_COMBO = 20097;
const int ID_STEDLG_COLUMNIZE_FORMAT_BUTTON = 20098;
const int ID_STEDLG_COLUMNIZE_AFTER_COMBO = 20099;
const int ID_STEDLG_COLUMNIZE_ORIGINAL_BUTTON = 20100;
const int ID_STEDLG_COLUMNIZE_PRESERVE_COMBO = 20101;
const int ID_STEDLG_COLUMNIZE_CHECKBOX = 20102;
const int ID_STEDLG_COLUMNIZE_IGNORE_COMBO = 20103;
const int ID_STEDLG_COLUMNIZE_EDITOR = 20104;
wxSizer *wxSTEditorColumnizeSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_EXPORT_FORMAT_CHOICE = 20105;
const int ID_STEDLG_EXPORT_FILENAME_COMBO = 20106;
const int ID_STEDLG_EXPORT_FILENAME_BITMAPBUTTON = 20107;
const int ID_STEDLG_EXPORT_EXTENSION_CHECKBOX = 20108;
wxSizer *wxSTEditorExportSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_LANG_KEYWORD_CHOICE = 20109;
const int ID_STEDLG_LANG_KEYWORD_TEXTCTRL = 20110;
const int ID_STEDLG_LANG_USERKEYWORD_TEXTCTRL = 20111;
wxSizer *wxSTEditorLangKeywordSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_LANG_STYLE_CHOICE = 20112;
const int ID_STEDLG_LANG_STYLE_EDITOR = 20113;
wxSizer *wxSTEditorLangStyleSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

const int ID_STEDLG_LOAD_UNICODE_CHOICE = 20114;
const int ID_STEDLG_SAVE_WHITESPACE_CHECKBOX = 20115;
const int ID_STEDLG_SAVE_LINEENDINGS_CHECKBOX = 20116;
wxSizer *wxSTEditorLoadSavePrefsSizer( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );

// Declare menubar functions

// Declare toolbar functions

// Declare bitmap functions

wxBitmap wxSTEBitmapsFunc( size_t index );

#endif

// End of generated file
