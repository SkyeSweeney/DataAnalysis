/////////////////////////////////////////////////////////////////////////////
// Name:        tbl.h
// Purpose:     Grid control wxWidgets sample
// Author:      Michael Bedward
// Modified by:
// Copyright:   (c) Michael Bedward, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __tbl_h__
#define __tbl_h__


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
    HubIf          *m_pHubIf;
    uint32_t        m_maxRows;
    uint32_t        m_numRows;


    void msgEvent(wxCommandEvent & evt);


    void cbMessages(Msg_t *pMsg);
    void cbStatus(bool ok);
    void addRecord(const char *sim, 
                   const char *wall, 
                   LogType_e logType, 
                   const char *str);
    void ShowSelection( wxCommandEvent& );
    void OnLabelLeftClick( wxGridEvent& );
    void OnCellLeftClick( wxGridEvent& );
    void OnSelectCell( wxGridEvent& );
    void OnRangeSelected( wxGridRangeSelectEvent& );
    void OnQuit( wxCommandEvent& );
    void About( wxCommandEvent& );
    void OnGridRender( wxCommandEvent& event );
    void OnRenderPaint( wxPaintEvent& event );

    void OnMessageEvent(wxCommandEvent & evt);


    void processMessage(Msg_t *pMsg);
    void processTable(Msg_t *pMsg);



public:
    GridFrame();
    ~GridFrame();

    wxLog *m_logOld;
    CommonStatus *m_pStatus;


    wxDECLARE_EVENT_TABLE();
};


#endif // griddemo_h

