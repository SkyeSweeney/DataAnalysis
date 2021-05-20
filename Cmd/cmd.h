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


    enum
    {
        ID_TOGGLEROWLABELS = 100,
        ID_TOGGLECOLLABELS,
        ID_TOGGLEEDIT,
        ID_TOGGLEROWSIZING,
        ID_TOGGLECOLSIZING,
        ID_TOGGLECOLMOVING,
        ID_TOGGLECOLHIDING,
        ID_TOGGLEGRIDSIZING,
        ID_TOGGLEGRIDDRAGCELL,
        ID_TOGGLEGRIDLINES,
        ID_AUTOSIZECOLS,
        ID_CELLOVERFLOW,
        ID_HIDECOL,
        ID_SHOWCOL,
        ID_HIDEROW,
        ID_SHOWROW,
        ID_RESIZECELL,
        ID_SETLABELCOLOUR,
        ID_SETLABELTEXTCOLOUR,
        ID_SETLABEL_FONT,
        ID_ROWLABELALIGN,
        ID_ROWLABELHORIZALIGN,
        ID_ROWLABELVERTALIGN,
        ID_COLLABELALIGN,
        ID_COLLABELHORIZALIGN,
        ID_COLLABELVERTALIGN,
        ID_CORNERLABELALIGN,
        ID_CORNERLABELHORIZALIGN,
        ID_CORNERLABELVERTALIGN,
        ID_CORNERLABELORIENTATION,
        ID_COLDEFAULTHEADER,
        ID_COLNATIVEHEADER,
        ID_COLNATIVELABELS,
        ID_COLCUSTOMHEADER,
        ID_TAB_STOP,
        ID_TAB_WRAP,
        ID_TAB_LEAVE,
        ID_TAB_CUSTOM,
        ID_GRIDLINECOLOUR,
        ID_INSERTROW,
        ID_INSERTCOL,
        ID_DELETEROW,
        ID_DELETECOL,
        ID_CLEARGRID,
        ID_EDITCELL,
        ID_SETCORNERLABEL,
        ID_SHOWSEL,
        ID_CHANGESEL,
        ID_SELCELLS,
        ID_SELROWS,
        ID_SELCOLS,
        ID_SELROWSORCOLS,
        ID_SET_CELL_FG_COLOUR,
        ID_SET_CELL_BG_COLOUR,
        ID_VTABLE,
        ID_BUGS_TABLE,
        ID_TABULAR_TABLE,
        ID_SELECT_UNSELECT,
        ID_SELECT_ALL,
        ID_SELECT_ROW,
        ID_SELECT_COL,
        ID_SELECT_CELL,
        ID_DESELECT_ALL,
        ID_DESELECT_ROW,
        ID_DESELECT_COL,
        ID_DESELECT_CELL,
        ID_SIZE_ROW,
        ID_SIZE_COL,
        ID_SIZE_ROW_LABEL,
        ID_SIZE_COL_LABEL,
        ID_SIZE_LABELS_COL,
        ID_SIZE_LABELS_ROW,
        ID_SIZE_GRID,

        ID_FREEZE_OR_THAW,

        ID_SET_HIGHLIGHT_WIDTH,
        ID_SET_RO_HIGHLIGHT_WIDTH,

        ID_TESTFUNC,

        ID_RENDER_ROW_LABEL,
        ID_RENDER_COL_LABEL,
        ID_RENDER_GRID_LINES,
        ID_RENDER_GRID_BORDER,
        ID_RENDER_SELECT_HLIGHT,
        ID_RENDER_LOMETRIC,
        ID_RENDER_COORDS,
        ID_RENDER_ZOOM,
        ID_RENDER_MARGIN,
        ID_RENDER_DEFAULT_SIZE,
    };

    wxLog *m_logOld;


    wxDECLARE_EVENT_TABLE();
};


#endif // griddemo_h

