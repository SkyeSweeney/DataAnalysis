#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/timer.h>

#include "img.h"



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
    SetStatusText(wxT("Welcome to wxWidgets!"));

    // Create an image panel
    m_myImagePanel = new MyImagePanel(this,
                                      wxBITMAP_TYPE_PNG);

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

    m_myImagePanel->start();

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
