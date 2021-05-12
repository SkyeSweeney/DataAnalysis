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
    uint32_t        m_numRows;

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

#if 0
    void ToggleRowLabels( wxCommandEvent& );
    void ToggleColLabels( wxCommandEvent& );
    void ToggleEditing( wxCommandEvent& );
    void ToggleRowSizing( wxCommandEvent& );
    void ToggleColSizing( wxCommandEvent& );
    void ToggleColMoving( wxCommandEvent& );
    void ToggleColHiding( wxCommandEvent& );
    void ToggleGridSizing( wxCommandEvent& );
    void ToggleGridDragCell ( wxCommandEvent& );
    void SetNativeColHeader ( wxCommandEvent& );
    void SetNativeColLabels ( wxCommandEvent& );
    void SetCustomColHeader( wxCommandEvent& );
    void SetDefaultColHeader( wxCommandEvent& );
    void SetTabBehaviour( wxCommandEvent& );
    void SetTabCustomHandler( wxCommandEvent& );
    void ToggleGridLines( wxCommandEvent& );
    void AutoSizeCols( wxCommandEvent& );
    void CellOverflow( wxCommandEvent& );
    void ResizeCell( wxCommandEvent& );
    void SetLabelColour( wxCommandEvent& );
    void SetLabelTextColour( wxCommandEvent& );
    void SetLabelFont(wxCommandEvent &);
    void SetRowLabelHorizAlignment( wxCommandEvent& );
    void SetRowLabelVertAlignment( wxCommandEvent& );
    void SetColLabelHorizAlignment( wxCommandEvent& );
    void SetColLabelVertAlignment( wxCommandEvent& );
    void SetCornerLabelHorizAlignment( wxCommandEvent& );
    void SetCornerLabelVertAlignment( wxCommandEvent& );
    void ToggleCornerLabelOrientation( wxCommandEvent& );
    void SetGridLineColour( wxCommandEvent& );

    void SetCellFgColour(wxCommandEvent &);
    void SetCellBgColour(wxCommandEvent &);

    void InsertRow( wxCommandEvent& );
    void InsertCol( wxCommandEvent& );
    void DeleteSelectedRows( wxCommandEvent& );
    void DeleteSelectedCols( wxCommandEvent& );
    void ClearGrid( wxCommandEvent& );
    void EditCell( wxCommandEvent& );
    void SetCornerLabelValue( wxCommandEvent& );
    void SelectCells( wxCommandEvent& );
    void SelectRows( wxCommandEvent& );
    void SelectCols( wxCommandEvent& );
    void SelectRowsOrCols( wxCommandEvent& );

    void FreezeOrThaw( wxCommandEvent& );

    void DeselectCell(wxCommandEvent& event);
    void DeselectCol(wxCommandEvent& event);
    void DeselectRow(wxCommandEvent& event);
    void DeselectAll(wxCommandEvent& event);
    void SelectCell(wxCommandEvent& event);
    void SelectCol(wxCommandEvent& event);
    void SelectRow(wxCommandEvent& event);
    void SelectAll(wxCommandEvent& event);
    void OnAddToSelectToggle(wxCommandEvent& event);

    void AutoSizeRow(wxCommandEvent& event);
    void AutoSizeCol(wxCommandEvent& event);
    void AutoSizeRowLabel(wxCommandEvent& event);
    void AutoSizeColLabel(wxCommandEvent& event);
    void AutoSizeLabelsCol(wxCommandEvent& event);
    void AutoSizeLabelsRow(wxCommandEvent& event);
    void AutoSizeTable(wxCommandEvent& event);

    void HideCol(wxCommandEvent& event);
    void ShowCol(wxCommandEvent& event);
    void HideRow(wxCommandEvent& event);
    void ShowRow(wxCommandEvent& event);


    void OnRowSize( wxGridSizeEvent& );
    void OnColSize( wxGridSizeEvent& );
    void OnColAutoSize( wxGridSizeEvent& );
    void OnCellValueChanging( wxGridEvent& );
    void OnCellValueChanged( wxGridEvent& );
    void OnCellBeginDrag( wxGridEvent& );

    void OnEditorShown(wxGridEvent&);
    void OnEditorHidden(wxGridEvent&);

    void OnSetHighlightWidth(wxCommandEvent&);
    void OnSetROHighlightWidth(wxCommandEvent&);

    void OnGridCustomTab(wxGridEvent& event);

    void OnGridContextMenu(wxContextMenuEvent& event);
#endif

public:
    GridFrame();
    ~GridFrame();

    void OnQuit( wxCommandEvent& );
    void About( wxCommandEvent& );
    void OnGridRender( wxCommandEvent& event );
    void OnRenderPaint( wxPaintEvent& event );

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

