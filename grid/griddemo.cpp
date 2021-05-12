/////////////////////////////////////////////////////////////////////////////
// Name:        Data Analysis program
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

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/aboutdlg.h"

#include "wx/grid.h"
#include "wx/headerctrl.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"

#include "griddemo.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(GridApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// GridApp
// ----------------------------------------------------------------------------

bool GridApp::OnInit()
{
    GridFrame *frame = new GridFrame;
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// GridFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE( GridFrame, wxFrame )
    EVT_MENU      ( wxID_ABOUT, GridFrame::About )
    EVT_MENU      ( wxID_EXIT,  GridFrame::OnQuit )
    EVT_TEXT_ENTER( wxID_ANY,   GridFrame::OnCmd  )
    EVT_GRID_CELL_LEFT_CLICK( GridFrame::OnCellLeftClick )
    EVT_GRID_SELECT_CELL( GridFrame::OnSelectCell )
    EVT_GRID_RANGE_SELECT( GridFrame::OnRangeSelected )
wxEND_EVENT_TABLE()


GridFrame::GridFrame()
        : wxFrame( (wxFrame *)NULL, 
                   wxID_ANY, 
                   "Data Analysis",
                   wxDefaultPosition,
                   wxDefaultSize )
{

    // Define length of table
    m_numRows = 100;

    //SetIcon(wxICON(sample));

    //--------------------------------------------
    // Create menus
    //--------------------------------------------
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append( wxID_EXIT, "E&xit\tAlt-X" );


    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append( wxID_ABOUT, "&About wxGrid demo" );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( fileMenu, "&File" );
    menuBar->Append( helpMenu, "&Help" );
    SetMenuBar( menuBar );


    // Create the grid
    m_grid = new wxGrid( this,
                       wxID_ANY,
                       wxPoint( 0, 0 ),
                       FromDIP(wxSize( 800, 450 )) );


    // Create logger window
    m_logWin = new wxTextCtrl( this,
                             wxID_ANY,
                             wxEmptyString,
                             wxDefaultPosition,
                             wxSize(-1, 8*GetCharHeight()),
                             wxTE_MULTILINE );

    m_logger = new wxLogTextCtrl( m_logWin );
    m_logOld = wxLog::SetActiveTarget( m_logger );
    wxLog::DisableTimestamp();

    // Command prompt window
    m_cmdWin = new wxTextCtrl( this,
                             wxID_ANY,
                             wxEmptyString,
                             wxDefaultPosition,
                             wxSize(-1, 2*GetCharHeight()),
                             wxTE_PROCESS_ENTER );

    // Create the status bar
    CreateStatusBar(3);
    SetStatusText("aaaa", 0);
    SetStatusText("bbbb", 1);
    SetStatusText("cccc", 2);
    int widths[3] = {100,100,100};
    SetStatusWidths(3, widths);

    // this will create a grid and, by default, an associated grid
    // table for strings
    m_grid->CreateGrid( m_numRows, 4 );

    // No row labels
    m_grid->SetRowLabelSize( 0 );

    // No edit
    m_grid->EnableEditing(false);

    // Set column titles
    m_grid->SetColLabelValue(0, wxString("Sim Time"));
    m_grid->SetColLabelValue(1, wxString("Wall Time"));
    m_grid->SetColLabelValue(2, wxString("Class"));
    m_grid->SetColLabelValue(3, wxString("String"));

    m_grid->SetColSize(3, 500);

    // Fill table for debug purposes
    char buf[30];
    for (int i=0; i<m_numRows; i++)
    {
        sprintf(buf, "%04d", 100-i-1);
        this->addRecord(buf, buf, LOG_DBG, "Debug");
    }

    this->addRecord("S1", "W1", LOG_ERR, "error");
    this->addRecord("S2", "W2", LOG_CMD, "command");


    // Create the layout
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( m_grid,
                   1,
                   wxEXPAND );

    topSizer->Add( m_logWin, 0, wxEXPAND );
    topSizer->Add( m_cmdWin, 0, wxEXPAND );

    SetSizerAndFit( topSizer );


    // Set up to talk to hub
    // Accept messages
    // LOG

    // Center on screen
    Centre();
}


//**********************************************************************
//
//**********************************************************************
GridFrame::~GridFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}


//**********************************************************************
// Add a record to bottom of grid
//**********************************************************************
void GridFrame::addRecord(const char *sim, 
                          const char *wall, 
                          LogType_e logType, 
                          const char *str)
{
    int ir = 100 - 1;
    wxColour clr;

    // Delete top row
    m_grid->DeleteRows(0, 1);

    // Insert row at bottom
    m_grid->AppendRows(1);

    m_grid->SetCellValue( ir, 0, sim );
    m_grid->SetCellValue( ir, 1, wall );
    m_grid->SetCellValue( ir, 3, str );

    switch(logType)
    {
    case LOG_CRI:
        m_grid->SetCellValue( ir, 2, "CRI" );
        clr = *wxRED;
        break;
    case LOG_ERR:
        m_grid->SetCellValue( ir, 2, "ERR" );
        clr = *wxRED;
        break;
    case LOG_WRN:
        m_grid->SetCellValue( ir, 2, "WRN" );
        clr = *wxYELLOW;
        break;
    case LOG_NTC:
        m_grid->SetCellValue( ir, 2, "NTC" );
        clr = *wxBLUE;
        break;
    case LOG_DBG:
        m_grid->SetCellValue( ir, 2, "DBG" );
        clr = *wxBLACK;
        break;
    case LOG_CMD:
        m_grid->SetCellValue( ir, 2, "CMD" );
        clr = *wxGREEN;
        break;
    default:
        m_grid->SetCellValue( ir, 2, "???" );
        clr = *wxCYAN;
        m_grid->SetCellBackgroundColour(ir, 2, *wxLIGHT_GREY);
        break;
    }

    m_grid->SetCellTextColour(ir, 0, clr);
    m_grid->SetCellTextColour(ir, 1, clr);
    m_grid->SetCellTextColour(ir, 2, clr);
    m_grid->SetCellTextColour(ir, 3, clr);

}


//**********************************************************************
// Event happens when user hits Enter in command window
//**********************************************************************
void GridFrame::OnCmd(wxCommandEvent& event)
{
    wxTextCtrl *win;
    wxString    v;

    // Get the window ID
    win = (wxTextCtrl *)event.GetEventObject();

    // Get the value
    v = win->GetValue();

    // Parse the command

    // Append it to the table
    this->addRecord("aaa", "aaa", LOG_CMD, v.mb_str().data());

    // Force us to see this new row
    m_grid->GoToCell(m_numRows-1, 0);

    win->SetValue("");

}
  

//**********************************************************************
//
//**********************************************************************
void GridFrame::OnCellLeftClick( wxGridEvent& ev )
{
    wxLogMessage("Left click at row %d, col %d", ev.GetRow(), ev.GetCol());

    // Not sure what to use this for, but it will be usefull I am sure

    // you must call event skip if you want default m_grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}



//**********************************************************************
//
//**********************************************************************
void GridFrame::OnSelectCell( wxGridEvent& ev )
{
    wxString logBuf;
    if ( ev.Selecting() )
    {
        logBuf << "Selected ";
    }
    else
    {
        logBuf << "Deselected ";
    }

    logBuf << "cell at row " << ev.GetRow()
           << " col " << ev.GetCol()
           << " ( ControlDown: "<< (ev.ControlDown() ? 'T':'F')
           << ", ShiftDown: "<< (ev.ShiftDown() ? 'T':'F')
           << ", AltDown: "<< (ev.AltDown() ? 'T':'F')
           << ", MetaDown: "<< (ev.MetaDown() ? 'T':'F') << " )";


    wxLogMessage( "%s", logBuf );

    // you must call Skip() if you want the default processing
    // to occur in wxGrid
    ev.Skip();
}

//**********************************************************************
//
//**********************************************************************
void GridFrame::OnRangeSelected( wxGridRangeSelectEvent& ev )
{
    wxString logBuf;

    if ( ev.Selecting() )
    {
        logBuf << "Selected ";
    }
    else
    {
        logBuf << "Deselected ";
    }

    logBuf << "cells from row " << ev.GetTopRow()
           << " col " << ev.GetLeftCol()
           << " to row " << ev.GetBottomRow()
           << " col " << ev.GetRightCol()
           << " ( ControlDown: "<< (ev.ControlDown() ? 'T':'F')
           << ", ShiftDown: "<< (ev.ShiftDown() ? 'T':'F')
           << ", AltDown: "<< (ev.AltDown() ? 'T':'F')
           << ", MetaDown: "<< (ev.MetaDown() ? 'T':'F') << " )";
    wxLogMessage( "%s", logBuf );

    ev.Skip();
}


//**********************************************************************
//
//**********************************************************************
void GridFrame::About(  wxCommandEvent& WXUNUSED(ev) )
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("DataAnalysis");
    aboutInfo.SetDescription(_("Data Analysis program from robot"));
    aboutInfo.AddDeveloper("Skye Sweeney");

    // this is just to force the generic version of the about
    // dialog under wxMSW so that it's easy to test if the m_grid
    // repaints correctly when it has lost focus and a dialog
    // (different from the Windows standard message box -- it doesn't
    // work with it for some reason) is moved over it.
    aboutInfo.SetWebSite("http://www.fll-freak.com");

    wxAboutBox(aboutInfo, this);
}


//**********************************************************************
//
//**********************************************************************
void GridFrame::OnQuit( wxCommandEvent& WXUNUSED(ev) )
{
    Close( true );
}

