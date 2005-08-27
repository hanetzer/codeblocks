#ifndef __WXSSCROLLBAR_H
#define __WXSSCROLLBAR_H

#include "../wxsdefwidget.h"
#include "wxsstdmanager.h"

WXS_ST_DECLARE(wxsScrollBarStyles)

wxsDWDeclareBegin(wxsScrollBar,propWidget,wxsScrollBarId)
    int value;
    int thumbsize;
    int range;
    int pagesize;
wxsDWDeclareEnd()


#endif
