/////////////////////////////////////////////////////////////////////////////
// Name:        griddemo.h
// Purpose:     Grid control wxWidgets sample
// Author:      Michael Bedward
// Modified by:
// Copyright:   (c) Michael Bedward, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef griddemo_h
#define griddemo_h


#include <msgs.h>
#include <hub_if.h>

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


    void cbMsg(Msg_t *pMsg);
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

    void processMessage(Msg_t *pMsg);
    void processTable(Msg_t *pMsg);



public:
    GridFrame();
    ~GridFrame();

    wxLog *m_logOld;

    wxDECLARE_EVENT_TABLE();
};


#endif // griddemo_h

