#include <functional>

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/timer.h>

#include "img.h"
#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

using namespace std;
using namespace std::placeholders; // for `_1`

wxDEFINE_EVENT(TIME_EVENT, wxCommandEvent);



//**********************************************************************
//  MyApp
//**********************************************************************


//**********************************************************************
// OnInit
//**********************************************************************
bool MyApp::OnInit()
{
    MyImagePanel * pMyImagePanel;
    HubIf *pHubIf;

    // Make sure to call this first to be able to understand all
    // the different image formats
    wxInitAllImageHandlers();

    // Create the main application window
    m_myFrame = new MyFrame(wxT("Data Analysis - Video"));

    // Get pointer to panel
    pMyImagePanel = m_myFrame->getMyImagePanel();
    
    // Show it
    m_myFrame->Show(true);

    // Start the hub interface
    pHubIf = new HubIf(NODE_VIDEO);
    pHubIf->client_init();

    // Register callback for the Time message
    std::function<void(Msg_t*)> pCbTime;
    pCbTime = std::bind(&MyImagePanel::cbTime, pMyImagePanel, _1);
    pHubIf->registerCb(MSGID_TIME, pCbTime);

    // Register callback for the VideoConfig message
    std::function<void(Msg_t*)> pCbVideoConfig;
    pCbVideoConfig = std::bind(&MyImagePanel::cbVideoConfig, pMyImagePanel, _1);
    pHubIf->registerCb(MSGID_VIDEO_CONFIG, pCbVideoConfig);

    // Worked!
    return true;
}  





    

// This generates code that creates MyApp and starts event loop
// One of the first things that happens is the OnInit gets called
// Think main()
IMPLEMENT_APP(MyApp)



//**********************************************************************
//  MyFrame
//**********************************************************************


// Event table for MyFrame
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

//**********************************************************************
// MyFrame constructor
// This is the main 'frame'. It has the menu and status bar
//**********************************************************************
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // Create a menu bar
    m_fileMenu = new wxMenu;

    // The "About" item should be in the help menu
    m_helpMenu = new wxMenu;
    m_helpMenu->Append(wxID_ABOUT,
                     wxT("&About...\tF1"),
                     wxT("Show about dialog"));

    m_fileMenu->Append(wxID_EXIT,
                     wxT("E&xit\tAlt-X"),
                     wxT("Quit this program"));

    // Now append the freshly created menu to the menu bar...
    m_menuBar = new wxMenuBar();
    m_menuBar->Append(m_fileMenu, wxT("&File"));
    m_menuBar->Append(m_helpMenu, wxT("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(m_menuBar);

    // Create a status bar just for fun
    CreateStatusBar(2);
    SetStatusText(wxT("NO SYNC"), 0);

    // Create an image panel
    m_myImagePanel = new MyImagePanel(this, wxBITMAP_TYPE_PNG);


    // Text for time
    m_timeTxt = new wxTextCtrl(this, 
                               wxID_ANY, 
                               "XX:XX:XX.XXXXXX",
                               wxDefaultPosition, 
                               wxDefaultSize,
                               wxTE_PROCESS_ENTER);
    m_timeTxt->SetEditable(false);
    //m_timeTxt->SetBackgroundColour(wxLIGHT_GREY);

    // Create a vertical sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    {
        // Item, proportion, flag, border
        sizer->Add(m_myImagePanel, 5, wxEXPAND); 
        sizer->Add(m_timeTxt,      1, wxEXPAND, wxALIGN_CENTER, 0);
    }    
    SetSizer(sizer);

    // Set position of window
    wxPoint pos(440,40);
    this->SetPosition(pos);

}





//**********************************************************************
// getMyImagePanel
//**********************************************************************
MyImagePanel * MyFrame::getMyImagePanel(void)
{
    return m_myImagePanel;
}

//**********************************************************************
// OnAbout
//**********************************************************************
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg;
    msg.Printf(wxT("Hello and welcome to %s"),
               wxVERSION_STRING);

    wxMessageBox(msg, wxT("About Minimal"),
                 wxOK | wxICON_INFORMATION, this);
}

//**********************************************************************
// OnQuit  
//**********************************************************************
void MyFrame::OnQuit(wxCommandEvent& event)
{
    // Destroy the frame
    Close();
}





// Event table for MyImagePanel
BEGIN_EVENT_TABLE(MyImagePanel, wxPanel)
    // some useful events
    /*
     EVT_MOTION(MyImagePanel::mouseMoved)
     EVT_LEFT_DOWN(MyImagePanel::mouseDown)
     EVT_LEFT_UP(MyImagePanel::mouseReleased)
     EVT_RIGHT_DOWN(MyImagePanel::rightClick)
     EVT_LEAVE_WINDOW(MyImagePanel::mouseLeftWindow)
     EVT_KEY_DOWN(MyImagePanel::keyPressed)
     EVT_KEY_UP(MyImagePanel::keyReleased)
     EVT_MOUSEWHEEL(MyImagePanel::mouseWheelMoved)
     EVT_TIMER(wxID_ANY, MyImagePanel::OnTimer)
     */
    
    // catch paint events
    EVT_PAINT(MyImagePanel::OnPaint)
    EVT_COMMAND(wxID_ANY, TIME_EVENT, MyImagePanel::OnTime)

END_EVENT_TABLE()

//**********************************************************************
// Constructor (inherits from wxPanel)
//**********************************************************************
MyImagePanel::MyImagePanel(wxFrame     *parent, 
                           wxBitmapType format) :
    wxPanel(parent)
{

    m_parent = (MyFrame*)parent;

    // Set the file name parts
    // TODO get from config message
    strcpy(m_path, "/home/skye/Projects/DataAnalysis/Data/thumbs/");
    strcpy(m_baseFn, "output");
    m_videoSync.sec = 0xffffffff;
    m_videoSync.nsec = 0;

}


//**********************************************************************
// Called by wxWidgets when the panel needs to be redrawn. 
// You can also trigger this call by calling Refresh()/Update().
//**********************************************************************
void MyImagePanel::OnPaint(wxPaintEvent & evt)
{
    // Create a DC for this ImagePanel
    wxPaintDC dc(this);

    // Force the render
    render(dc);
}

//**********************************************************************
// Called by wxWidgets when the panel needs to be redrawn. 
// You can also trigger this call by calling Refresh()/Update().
//**********************************************************************
void MyImagePanel::OnTime(wxCommandEvent & evt)
{
    Msg_t   *pMsg;
    char     buf[300];
    bool     ok;
    uint32_t sec, nsec;
    uint32_t h,m;
    double   fsec;
    uint32_t sn;
    static uint32_t n=0;

    // Get the message
    pMsg = (Msg_t *)evt.GetClientData();

    // Put message data into memory
    sec   =  pMsg->hdr.sec;
    nsec  =  pMsg->hdr.nsec;
    printf("Got time %u %u\n", sec, nsec);


    if (m_videoSync.sec == 0xffffffff)
    {
        printf("VideoConfig not received yet\n");
        sn = 1;
    }
    else
    {
        int32_t dt;
        // Difference in time in usec
        // TODO Fix up for signed values
        dt = (sec - m_videoSync.sec)*1000 +
             (nsec - m_videoSync.nsec)/1000000;
        sn = dt / (1.0/30.0 * 1000);
    }

    // Create the filename
    sprintf(buf, 
            "%s/%s_%06d.png",
            m_path,
            m_baseFn,
            sn);
    printf("Try to load file <%s>\n", buf);

    // Load the file into the image
    ok = m_bitMap.LoadFile(buf, wxBITMAP_TYPE_PNG);

    if (ok)
    {

        n++;
        if (n > 5)
        {
            // Convert sim time into string
            h = sec/(60*60);
            sec = sec - h*(60*60);
            m = sec/60;
            sec = sec - m*60;
            fsec = (double)nsec/1000000000.0;

            // Update time
            char buf[40];
            sprintf(buf, "Sim: %02u:%02u:%02u.%0.6f  frame: %d", h, m, sec, fsec, sn);
            m_parent->m_timeTxt->ChangeValue(buf);

            // Create a DC for this ImagePanel
            paintNow();
            n = 0;
        }
    }
}

//**********************************************************************
// Alternatively, you can use a clientDC to paint on the panel
// at any time. Using this generally does not free you from
// catching paint events, since it is possible that e.g. the window
// manager throws away your drawing when the window comes to the
// background, and expects you will redraw it when the window comes
// back (by sending a paint event).
//**********************************************************************
void MyImagePanel::paintNow()
{
    // Create a DC for this ImagePanel
    wxClientDC dc(this);

    // Force the render
    render(dc);
}

//**********************************************************************
// Here we do the actual rendering. I put it in a separate
// method so that it can work no matter what type of DC
// (e.g. wxPaintDC or wxClientDC) is used.
//**********************************************************************
void MyImagePanel::render(wxDC&  dc)
{
    // Draw the bitmap (video image) to the DC
    if (m_bitMap.IsOk())
    {
        dc.DrawBitmap( m_bitMap, 0, 0, false );
    }
}

//**********************************************************************
// Callback for a TIME message
//**********************************************************************
void MyImagePanel::cbTime(Msg_t *pMsg)
{
    wxCommandEvent evt(TIME_EVENT);
    Msg_t *msg;

    // Send the message via event to the main loop.
    // Can't process it here because of the different thread context
    msg = new Msg_t;
    *msg = *pMsg;
    evt.SetClientData(msg);

    wxPostEvent(this, evt);
}


//**********************************************************************
// Callback for VIDEO CONFIG message
//**********************************************************************
void MyImagePanel::cbVideoConfig(Msg_t *pMsg)
{
    // Put message data into memory
    m_videoSync = pMsg->body.videoConfig.videoSync;

    printf("Got a VideoConfig %u %u %u\n", 
            m_videoSync.sec, 
            m_videoSync.nsec, 
            m_videoSync.frame);

    m_parent->SetStatusText(wxT("SYNC"), 0);

}




// some useful events
/*
 void MyImagePanel::mouseMoved(wxMouseEvent& event) {}
 void MyImagePanel::mouseDown(wxMouseEvent& event) {}
 void MyImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
 void MyImagePanel::mouseReleased(wxMouseEvent& event) {}
 void MyImagePanel::rightClick(wxMouseEvent& event) {}
 void MyImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
 void MyImagePanel::keyPressed(wxKeyEvent& event) {}
 void MyImagePanel::keyReleased(wxKeyEvent& event) {}
 */
