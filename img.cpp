#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/timer.h>

#include "img.h"
#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"


static void cbTime(Msg_t *pMsg);
static void cbVideoConfig(Msg_t *pMsg);

static pthread_mutex_t g_displayMutex;
static pthread_t       g_displayThread;
static sem_t           g_displaySem;

static VideoSync_t     g_videoSync;

static uint32_t        g_sec;
static uint32_t        g_nsec;
static uint32_t        g_sn;

// Thread to process image requests from queue
static void *displayThread(void *pargs)
{
    for (;;)
    {
        // Wait for receiver to notify us we hae a new frame to display
        sem_wait(&g_displaySem);

        // If frame is cached
        if (0)
        {
            // Get image from cache
        } else {
            // Get image form file
            // Add it to cache
        }

        // Display the image
        // Display the frame and time
        //
        if (g_videoSync.sec == 0)
        {
            printf("VideoConfig not received yet\n");
            g_sn = 0;
        }
        else
        {
            int32_t dt;
            // Difference in time in usec
            // TODO Fix up for signed values
            dt = (g_sec - g_videoSync.sec)*1000 +
                 (g_nsec - g_videoSync.nsec)/1000000;
            g_sn = dt / (1.0/30.0 * 1000);
        }

        printf("Displaying image %d\n", g_sn);

        // Release mutux to allow the display of next image
        pthread_mutex_unlock(&g_displayMutex);
    }
    return NULL;
}





//**********************************************************************
//  MyApp
//**********************************************************************


//**********************************************************************
// OnInit
//**********************************************************************
bool MyApp::OnInit()
{
    // Make sure to call this first to be able to understand all
    // the different image formats
    wxInitAllImageHandlers();

    // Create the main application window
    m_myFrame = new MyFrame(wxT("Data Analysis - Video"));
    
    // Show it
    m_myFrame->Show(true);

    hubif_client_init();
    hubif_login(NODE_VIDEO);

    hubif_register(MSGID_TIME,         cbTime);
    hubif_register(MSGID_VIDEO_CONFIG, cbVideoConfig);

    // Start image display thread
    pthread_create(&g_displayThread, NULL, displayThread, NULL);

    // Worked!
    return true;
}  

//**********************************************************************
//
//**********************************************************************
void cbTime(Msg_t *pMsg)
{
    int err;

    // Try to take the mutex
    err = pthread_mutex_trylock(&g_displayMutex);

    // Able to take mutux
    if (err == 0)
    {
        // Put message data into memory
        g_sec   =  pMsg->hdr.sec;
        g_nsec  =  pMsg->hdr.nsec;

        // Signal thread with semaphore
        sem_post(&g_displaySem);
    }

    printf("Got a Time: sec:%u nsec:%u\n",
           pMsg->hdr.sec,
           pMsg->hdr.nsec);

}


//**********************************************************************
//
//**********************************************************************
void cbVideoConfig(Msg_t *pMsg)
{
    // Put message data into memory
    g_videoSync = pMsg->body.videoConfig.videoSync;

    printf("Got a VideoConfig\n");

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
    SetStatusText(wxT("Welcome to wxWidgets!"));

    // Create an image panel
    m_myImagePanel = new MyImagePanel(this, wxBITMAP_TYPE_PNG);


    // Text for time
    m_timeTxt = new wxTextCtrl(this, 
                               wxID_ANY, 
                               "10:23:33.123456. aaaaaaaaaaaaaaaa",
                               wxDefaultPosition, 
                               wxDefaultSize,
                               wxTE_PROCESS_ENTER);

    // Create a vertical sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    {
        // Item, proportion, flag, border
        sizer->Add(m_myImagePanel, 5, wxEXPAND); 
        sizer->Add(m_timeTxt,      1, wxEXPAND, wxALIGN_CENTER, 0);
    }    
    SetSizer(sizer);

    wxSize sz;
    sz = m_myImagePanel->GetClientSize();
    printf("%d %d\n", sz.GetWidth(), sz.GetHeight());

    m_myImagePanel->start();

    // Set position of window
    wxPoint pos(440,40);
    this->SetPosition(pos);

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
     */
    
    // catch paint events
    EVT_PAINT(MyImagePanel::paintEvent)
    EVT_TIMER(wxID_ANY, MyImagePanel::OnTimer)

END_EVENT_TABLE()

//**********************************************************************
// Constructor (inherits from wxPanel)
//**********************************************************************
MyImagePanel::MyImagePanel(wxFrame     *parent, 
                           wxBitmapType format) :
    wxPanel(parent),
    m_timer(this),
    m_sn(0)
{

    // Set the file name parts
    strcpy(m_path, "/home/skye/Projects/DataAnalysis/thumbs/");
    strcpy(m_baseFn, "output");

    //parent->SetWidth(320);
    //parent->SetHeight(180);
}

//**********************************************************************
// Start timer
//**********************************************************************
void MyImagePanel::start()
{

    // Start the timer to go off every 30 ms
    m_timer.Start(30);

}

//**********************************************************************
// OnTimer
//**********************************************************************
void MyImagePanel::OnTimer(wxTimerEvent & evt)
{
    char buf[300];
    bool ok;

    m_sn++;

    // Create the filename
    sprintf(buf, 
            "%s/%s_%06d.png",
            m_path,
            m_baseFn,
            m_sn);

    // Load the file into the image
    ok = m_bitMap.LoadFile(buf, wxBITMAP_TYPE_PNG);
    (void)ok;

    // Force a paint
    this->paintNow();

    //wxSize sz;
    //sz = this->GetSize();
    //printf("%d %d\n", sz.GetWidth(), sz.GetHeight());

    wxSize sz;
    sz = m_bitMap.GetSize();
    printf("%d %d\n", sz.GetWidth(), sz.GetHeight());

}

//**********************************************************************
// Called by the system of by wxWidgets when the panel needs
// to be redrawn. You can also trigger this call by
// calling Refresh()/Update().
//**********************************************************************
void MyImagePanel::paintEvent(wxPaintEvent & evt)
{
    // Create a DC for this ImagePanel
    wxPaintDC dc(this);

    // Force the render
    render(dc);
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
