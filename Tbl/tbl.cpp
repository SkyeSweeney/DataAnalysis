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

#include "tbl.h"
#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

using namespace std::placeholders; // for `_1`


uint32_t run = 0;
VarTableEntry_t varTable[20];

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------


wxDEFINE_EVENT(MSG_EVENT, wxCommandEvent);


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
    EVT_GRID_CELL_LEFT_CLICK( GridFrame::OnCellLeftClick )
    EVT_GRID_SELECT_CELL( GridFrame::OnSelectCell )
    EVT_GRID_RANGE_SELECT( GridFrame::OnRangeSelected )
    EVT_COMMAND(wxID_ANY, MSG_EVENT, GridFrame::msgEvent)

wxEND_EVENT_TABLE()


//**********************************************************************
//
//**********************************************************************
GridFrame::GridFrame()
        : wxFrame( (wxFrame *)NULL, 
                   wxID_ANY, 
                   "Table View",
                   wxDefaultPosition,
                   wxDefaultSize )
{

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

    // Create the status bar
    CreateStatusBar(3);
    SetStatusText("aaaa", 0);
    SetStatusText("bbbb", 1);
    SetStatusText("cccc", 2);
    int widths[3] = {100,100,100};
    SetStatusWidths(3, widths);

    // this will create a grid and, by default, an associated grid
    // table for strings
    m_grid->CreateGrid( m_numRows, 20 );

    // No row labels
    m_grid->SetRowLabelSize( 0 );

    // No edit
    m_grid->EnableEditing(false);

    // Set column titles
    m_grid->SetColLabelValue(0, wxString("Sim Time"));

    // Initial record (since table can't be zero rows long)
    m_grid->SetCellValue( 0, 0, "" );

    // Create the layout
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( m_grid,
                   1,
                   wxEXPAND );

    topSizer->Add( m_logWin, 0, wxEXPAND );

    SetSizerAndFit( topSizer );


    // Create a new hub interface
    m_pHubIf = new HubIf();
    m_pHubIf->client_init();
    m_pHubIf->login(NODE_CMD);

    // Register callback for various messages
    std::function<void(Msg_t*)> pCbMsg;
    pCbMsg = std::bind(&GridFrame::cbMsg, this, _1);
    m_pHubIf->registerCb(MSGID_LOCATION, pCbMsg);
    m_pHubIf->registerCb(MSGID_TABLE,    pCbMsg);

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
// Process log message
//**********************************************************************
void GridFrame::cbMsg(Msg_t *pMsg)
{
    // This is called from a different context.
    // Must send a message to be picked uo by main loop
    // Must package the string and level
    wxCommandEvent evt(MSG_EVENT);

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
void GridFrame::msgEvent(wxCommandEvent & evt)
{
    Msg_t *pMsg;

    // Get the message from the client data
    pMsg = (Msg_t *)evt.GetClientData();

    // Process appropriately
    switch (pMsg->hdr.msgId)
    {
        case MSGID_LOCATION:
            processMessage(pMsg);
            break;
        case MSGID_TABLE:
            processTable(pMsg);
            break;
        default:
            break;
    }    

    // Delete the message
    delete pMsg;
}


//**********************************************************************
// Process a message
//**********************************************************************
void GridFrame::processMessage(Msg_t *pMsg)
{
    char     buf[64];
    int      ir;
    wxColour clr;
    MsgId_t  msgId;

    msgId = pMsg->hdr.msgId;

    char *p = (char *)&(pMsg->body);


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

    // For each active item in the table
    for (int iCol=0; iCol<20; iCol++)
    {
        // Skip if not active
        if (varTable[iCol].name[0] == 0) continue;

        // Skip if not the right message
        if (msgId != varTable[iCol].msgId) continue;


        // Switch based on type
        switch (varTable[iCol].varType)
        {
            case VARTYPE_DOUBLE:
                {
                    double var = *(double *)&p[varTable[iCol].varByteOffset];
                    sprintf(buf, varTable[iCol].format, var);
                }
                break;

            case VARTYPE_FLOAT:
                {
                    float var = *(double *)&p[varTable[iCol].varByteOffset];
                    sprintf(buf, varTable[iCol].format, var);
                }
                break;

            case VARTYPE_INT8:
                {
                    int8_t var= *(uint8_t *)&p[varTable[iCol].varByteOffset];
                    sprintf(buf, varTable[iCol].format, var);
                }
                break;

            case VARTYPE_UINT8:
                {
                    uint8_t var= *(int8_t *)&p[varTable[iCol].varByteOffset];
                    sprintf(buf, varTable[iCol].format, var);
                }
                break;

            case VARTYPE_INT16:
            case VARTYPE_UINT16:
            case VARTYPE_INT32:
            case VARTYPE_UINT32:
            case VARTYPE_INT64:
            case VARTYPE_UINT64:
            case VARTYPE_STRING:
                break;
            default:
                sprintf(buf, "???");
                break;

        }

        m_grid->SetCellValue(ir, iCol, buf);

    } // for each column entry


}


//**********************************************************************
//
//**********************************************************************
void GridFrame::processTable(Msg_t *pMsg)
{
    BodyTable_t *pBody;
    uint16_t col;

    pBody = &pMsg->body.table;
    col = pBody->entry.col;
    printf("AAA\n");

    // switch based on command type
    switch (pBody->cmd)
    {
        case TABLE_CLEAR:
            for (int i=0; i<20; i++)
            {
                varTable[i].name[0] = 0;

                // Remove the column heading
                m_grid->SetColLabelValue(i, "");
            }

            // Delete the data from all colums
            // TODO

            // Reset back to row one
            // TODO

            break;

        case TABLE_DELETE:

            varTable[col].name[0] = 0;
            m_grid->SetColLabelValue(col, "");

            // Delete the data from the column
            // TODO

            break;

        case TABLE_ASSIGN:
            printf("Assign %d %s\n", col, varTable[col].name);

            // Check for a legal column
            if ((col > 1) && (col < 20))
            {
                if (varTable[col].name[0] != 0)
                {
                    // Delete the column
                }
                varTable[col] = pBody->entry;
                m_grid->SetColLabelValue(col, varTable[col].name);
            }
            break;

        default:
            break;
    }

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

