/////////////////////////////////////////////////////////////////////////////
// Name:        cmd.h
// Purpose:     Grid control wxWidgets sample
// Author:      Michael Bedward
// Modified by:
// Copyright:   (c) Michael Bedward, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __cmd_h__
#define __cmd_h__


#include <msgs.h>
#include <hub_if.h>
#include "CommonStatus.h"

class wxGrid;

class GridApp : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
};


class GridFrame : public wxFrame
{

private:

    wxGrid         *m_grid;
    wxTextCtrl     *m_logWin;
    wxLogTextCtrl  *m_logger;
    wxTextCtrl     *m_cmdWin;
    uint32_t        m_maxRows;
    uint32_t        m_numRows;
    HubIf          *m_pHubIf;
    uint16_t        m_numToks;
    char           *m_tokens[20];


    void     parseCmd(const char *pCmd);
    uint16_t tokenize(const char *pCmd);

    void     LogCri(const char *str);
    void     LogErr(const char *str);
    void     LogWrn(const char *str);
    void     LogNtc(const char *str);
    void     LogDbg(const char *str);
    void     LogCmd(const char *str);

    void OnMessageEvent(wxCommandEvent & evt);


    void cbMessages(Msg_t *pMsg);
    void cbStatus(bool ok);
    void addRecord(const char *sim, 
                   const char *wall, 
                   LogType_e logType, 
                   const char *str);
    void OnCmd(wxCommandEvent& event);
    void ShowSelection( wxCommandEvent& );
    void OnLabelLeftClick( wxGridEvent& );
    void OnCellLeftClick( wxGridEvent& );
    void OnSelectCell( wxGridEvent& );
    void OnRangeSelected( wxGridRangeSelectEvent& );
    void OnQuit( wxCommandEvent& );
    void About( wxCommandEvent& );
    void OnGridRender( wxCommandEvent& event );
    void OnRenderPaint( wxPaintEvent& event );


public:
    GridFrame();
    ~GridFrame();


    wxLog *m_logOld;
    CommonStatus *m_pStatus;


    wxDECLARE_EVENT_TABLE();
};


#endif // griddemo_h

