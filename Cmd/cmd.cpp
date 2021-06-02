////////////////////////////////////////////////////////////////////////////
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

#include <functional>

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/aboutdlg.h"
#include "wx/grid.h"
#include "wx/headerctrl.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"

#include "cmd.h"
#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"
#include "CommonStatus.h"

using namespace std::placeholders; // for `_1`


uint32_t run = 0;

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------


wxDEFINE_EVENT(MESSAGE_EVENT, wxCommandEvent);


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
    EVT_COMMAND(wxID_ANY, MESSAGE_EVENT, GridFrame::OnMessageEvent)

wxEND_EVENT_TABLE()


//**********************************************************************
//
//**********************************************************************
GridFrame::GridFrame()
        : wxFrame( (wxFrame *)NULL, 
                   wxID_ANY, 
                   "CMD",
                   wxDefaultPosition,
                   wxDefaultSize )
{

    // Define length of table
    m_maxRows = 100;
    m_numRows = 1;

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
    m_pStatus = new CommonStatus(this);
    m_pStatus->setTime((char *)"Time");
    m_pStatus->setApp((char *)"App");
    m_pStatus->setConnection(true);

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

    // Initial record (since table can't be zero rows long)
    m_grid->SetCellValue( 0, 0, "" );
    m_grid->SetCellValue( 0, 1, "" );
    m_grid->SetCellValue( 0, 2, "NTC" );
    m_grid->SetCellValue( 0, 3, "Start" );

    // Create the layout
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( m_grid,
                   1,
                   wxEXPAND );

    topSizer->Add( m_logWin, 0, wxEXPAND );
    topSizer->Add( m_cmdWin, 0, wxEXPAND );

    SetSizerAndFit( topSizer );


    // Create a new hub interface
    m_pHubIf = new HubIf(NODE_CMD);
    
    // Initialize the system
    m_pHubIf->client_init();

    // Register callback for connection status change

    // Setup a callback to receive status change
    std::function<void(bool ok)> pStatusCb;
    pStatusCb = std::bind(&GridFrame::cbStatus, this, _1);
    m_pHubIf->registerStatus(pStatusCb);


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
// Process connection status changes
//**********************************************************************
void GridFrame::cbStatus(bool ok)
{
    printf("Status Change %d\n", ok);

    // Change the connection status in status bar
    m_pStatus->setConnection(ok);

    if (ok)
    {

        // Setup a callback to receive generic messages
        std::function<void(Msg_t*)> pCbMessages;
        pCbMessages = std::bind(&GridFrame::cbMessages, this, _1);

        // Register callback for the desired messages
        m_pHubIf->registerCb(MSGID_LOG,  pCbMessages);
        m_pHubIf->registerCb(MSGID_PING, pCbMessages);
    }
}


//**********************************************************************
// Process messages
//**********************************************************************
void GridFrame::cbMessages(Msg_t *pMsg)
{
    //printf("CB\n");
    // This is called from a different context.
    // Must send a message to be picked uo by main loop
    // Must package the string and level
    wxCommandEvent evt(MESSAGE_EVENT);

    // Attach message as clientData to event
    Msg_t *msg;
    msg = new Msg_t;
    *msg = *pMsg;
    evt.SetClientData(msg);

    wxPostEvent(this, evt);
}


//**********************************************************************
//
//**********************************************************************
void GridFrame::OnMessageEvent(wxCommandEvent & evt)
{
    Msg_t *pMsg;
    static int aaa = 0;

    // Get the message from the client data
    pMsg = (Msg_t *)evt.GetClientData();

    switch (pMsg->hdr.msgId)
    {
        case MSGID_LOG:

            // TODO Extract the level and string
            this->addRecord("aaa", 
                            "bbb", 
                            (LogType_e)pMsg->body.log.level,
                            pMsg->body.log.string);
            break;

        case MSGID_PING:
            char buf[24];
            sprintf(buf, "%d", aaa);
            aaa++;
            m_pStatus->setTime(buf);
            //printf("Ping\n");
            break;
    }

    delete pMsg;
}

//**********************************************************************
// Add a record to bottom of grid
//**********************************************************************
void GridFrame::addRecord(const char *sim, 
                          const char *wall, 
                          LogType_e   logType, 
                          const char *str)
{
    int      ir;
    wxColour clr;

    m_numRows++; // Number of rows in grid

    // If the grid is full
    if (m_numRows >= m_maxRows)
    {
        // Delete top row
        m_grid->DeleteRows(0, 1);

        m_numRows = m_maxRows;
    }

    // Insert row at bottom
    m_grid->AppendRows(1);

    // Get the row ID
    ir = m_numRows-1;

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
//
//**********************************************************************
void GridFrame::parseCmd(const char *cmd)
{

    char *pCmd;
    Msg_t msg;

    // Split line into tokens
    this->tokenize(cmd);

    // Discard blank lines
    if (m_numToks == 0)
    {
        return;
    }

    // Get first token
    pCmd = m_tokens[0];

    if (strcmp(pCmd,"sync") == 0)
    {
        // Send videoConfig message
        Msg_t msg;
        msg.body.videoConfig.videoSync.sec   = 0;
        msg.body.videoConfig.videoSync.nsec  = 0;
        msg.body.videoConfig.videoSync.frame = 0;
        m_pHubIf->sendMsg(&msg, MSGID_VIDEO_CONFIG, 0, 0);
    }

    else if (strcmp(pCmd,"stop") == 0)
    {
        if (m_numToks == 1 )
        {
            msg.body.playback.cmd     = PLAYBACK_STOP;
            msg.body.playback.fn[0]   = 0;
            msg.body.playback.ratio   = 1.0;
            m_pHubIf->sendMsg(&msg, MSGID_PLAYBACK, 0, 0);
        }
        else
        {
            LogErr("Wrong number of tokens");
        }
    }
    else if (strcmp(pCmd,"play") == 0)
    {
        if (m_numToks == 1)
        {
            msg.body.playback.cmd     = PLAYBACK_PLAY;
            m_pHubIf->sendMsg(&msg, MSGID_PLAYBACK, 0, 0);
        }
        else
        {
            LogErr("Wrong number of tokens");
        }
    }

    else if (strcmp(pCmd,"load") == 0)
    {
        if (m_numToks == 2)
        {
            msg.body.playback.cmd     = PLAYBACK_LOAD;
            strcpy(msg.body.playback.fn, m_tokens[1]);
            m_pHubIf->sendMsg(&msg, MSGID_PLAYBACK, 0, 0);
        }
        else
        {
            LogErr("Wrong number of tokens");
        }
    }

    else if (strcmp(pCmd,"rewind") == 0)
    {
        if (m_numToks == 1)
        {
            msg.body.playback.cmd     = PLAYBACK_REWIND;
            m_pHubIf->sendMsg(&msg, MSGID_PLAYBACK, 0, 0);
        }
        else
        {
            LogErr("Wrong number of tokens");
        }
    }

    else if (strcmp(pCmd,"exit") == 0)
    {
        run = 0;
    }
    else if (strcmp(pCmd,"stop") == 0)
    {
        run = 0;
    }

    else if (strcmp(pCmd,"?") == 0)
    {
        wxMessageBox("sync\n"
                     "start\n"
                     "exit\n"
                     "stop");
    }
    else if (strcmp(pCmd, "a") == 0)
    {
        Msg_t tableMsg;
        tableMsg.body.table.cmd = TABLE_ASSIGN;
        tableMsg.body.table.entry.col = 1;
        tableMsg.body.table.entry.msgId = MSGID_LOCATION;
        tableMsg.body.table.entry.varType = VARTYPE_DOUBLE;
        tableMsg.body.table.entry.varValidBits = 0; // NA
        tableMsg.body.table.entry.varByteOffset = 0;
        tableMsg.body.table.entry.varBitOffset = 0;
        strcpy(tableMsg.body.table.entry.name, "northing");
        strcpy(tableMsg.body.table.entry.format, "%.3f");
        m_pHubIf->sendMsg(&msg, MSGID_TABLE, 0, 0);

        tableMsg.body.table.cmd = TABLE_ASSIGN;
        tableMsg.body.table.entry.col = 1;
        tableMsg.body.table.entry.msgId = MSGID_LOCATION;
        tableMsg.body.table.entry.varType = VARTYPE_DOUBLE;
        tableMsg.body.table.entry.varValidBits = 0; // NA
        tableMsg.body.table.entry.varByteOffset = 8;
        tableMsg.body.table.entry.varBitOffset = 0;
        strcpy(tableMsg.body.table.entry.name, "easting");
        strcpy(tableMsg.body.table.entry.format, "%.3f");
        m_pHubIf->sendMsg(&msg, MSGID_TABLE, 0, 0);

        tableMsg.body.table.cmd = TABLE_ASSIGN;
        tableMsg.body.table.entry.col = 1;
        tableMsg.body.table.entry.msgId = MSGID_LOCATION;
        tableMsg.body.table.entry.varType = VARTYPE_DOUBLE;
        tableMsg.body.table.entry.varValidBits = 0; // NA
        tableMsg.body.table.entry.varByteOffset = 16;
        tableMsg.body.table.entry.varBitOffset = 0;
        strcpy(tableMsg.body.table.entry.name, "altitude");
        strcpy(tableMsg.body.table.entry.format, "%.3f");
        m_pHubIf->sendMsg(&msg, MSGID_TABLE, 0, 0);

        tableMsg.body.table.cmd = TABLE_ASSIGN;
        tableMsg.body.table.entry.col = 1;
        tableMsg.body.table.entry.msgId = MSGID_LOCATION;
        tableMsg.body.table.entry.varType = VARTYPE_DOUBLE;
        tableMsg.body.table.entry.varValidBits = 0; // NA
        tableMsg.body.table.entry.varByteOffset = 24;
        tableMsg.body.table.entry.varBitOffset = 0;
        strcpy(tableMsg.body.table.entry.name, "speed");
        strcpy(tableMsg.body.table.entry.format, "%.3f");
        m_pHubIf->sendMsg(&msg, MSGID_TABLE, 0, 0);

        tableMsg.body.table.cmd = TABLE_ASSIGN;
        tableMsg.body.table.entry.col = 1;
        tableMsg.body.table.entry.msgId = MSGID_LOCATION;
        tableMsg.body.table.entry.varType = VARTYPE_DOUBLE;
        tableMsg.body.table.entry.varValidBits = 0; // NA
        tableMsg.body.table.entry.varByteOffset = 32;
        tableMsg.body.table.entry.varBitOffset = 0;
        strcpy(tableMsg.body.table.entry.name, "heading");
        strcpy(tableMsg.body.table.entry.format, "%.3f");
        m_pHubIf->sendMsg(&msg, MSGID_TABLE, 0, 0);

    }

    else
    {
        LogErr("Unknown command");
    }

    return;
}

//**********************************************************************
//
//**********************************************************************
uint16_t GridFrame::tokenize(const char *pCmd)
{
    char  copy[1024];
    char *saveptr = NULL;
    char *pStr;
    char *p;
    int  i;

    strcpy(copy, pCmd);

    // Determine the number of tokens in string
    pStr = copy;

    for (i=0; ;i++)
    {
        // Tokeninze to get pointers
        p = strtok_r(pStr, " ", &saveptr);
        pStr = NULL;

        // If no token, skip counting
        if (p == NULL)
        {
            break;
        }

        // If a comment, skip counting
        if (strcmp(p, "//") == 0)
        {
            break;
        }
        m_tokens[i] = p;
    }

    // Save number of tokens
    m_numToks = i;

    return m_numToks;

}

//**********************************************************************
//
//**********************************************************************
void GridFrame::LogCri(const char *str)
{
    this->addRecord("aaa", "aaa", LOG_CRI, str);
}

//**********************************************************************
//
//**********************************************************************
void GridFrame::LogErr(const char *str)
{
    this->addRecord("aaa", "aaa", LOG_ERR, str);
}

//**********************************************************************
//
//**********************************************************************
void GridFrame::LogWrn(const char *str)
{
    this->addRecord("aaa", "aaa", LOG_WRN, str);
}

//**********************************************************************
//
//**********************************************************************
void GridFrame::LogNtc(const char *str)
{
    this->addRecord("aaa", "aaa", LOG_NTC, str);
}

//**********************************************************************
//
//**********************************************************************
void GridFrame::LogDbg(const char *str)
{
    this->addRecord("aaa", "aaa", LOG_DBG, str);
}

//**********************************************************************
//
//**********************************************************************
void GridFrame::LogCmd(const char *str)
{
    this->addRecord("aaa", "aaa", LOG_CMD, str);
}


//**********************************************************************
// Event happens when user hits Enter in command window
//**********************************************************************
void GridFrame::OnCmd(wxCommandEvent& event)
{
    wxString    cmd;
    wxTextCtrl *win;
    const char *pCmd;
    int         err;

    // Get the window ID
    win = (wxTextCtrl *)event.GetEventObject();

    // Get the value
    cmd = win->GetValue();
    pCmd = cmd.mb_str().data();

    // Append it to the table
    this->LogCmd(pCmd);

    // Force us to see this new row
    m_grid->GoToCell(m_numRows-1, 0);

    // Clear the command
    win->SetValue("");

    // Parse the command
    parseCmd(pCmd);


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

