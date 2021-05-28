////////////////////////////////////////////////////////////////////////////
// Name:        CommonStatus
// Author:      Skye Sweeney
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
//
#include <stdint.h>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "CommonStatus.h"


//**********************************************************************
//
//**********************************************************************
CommonStatus::CommonStatus(wxFrame *frame)
{
    m_frame = frame;

    // Create the status bar
    m_frame->CreateStatusBar(3);
    int widths[3] = {100,100,100};
    m_frame->SetStatusWidths(3, widths);

    m_frame->SetStatusText("", 0);
    m_frame->SetStatusText("", 1);
    m_frame->SetStatusText("", 2);

}

//**********************************************************************
//
//**********************************************************************
void CommonStatus::setTime(char *time)
{
    m_frame->SetStatusText(time, 0);
}

//**********************************************************************
//
//**********************************************************************
void CommonStatus::setApp(char *app)
{
    m_frame->SetStatusText(app, 1);
}

//**********************************************************************
//
//**********************************************************************
void CommonStatus::setConnection(bool ok)
{
    if (ok)
    {
        m_frame->SetStatusText("Connected", 2);
    } else {
        m_frame->SetStatusText("Disconnected", 2);
    }
}


