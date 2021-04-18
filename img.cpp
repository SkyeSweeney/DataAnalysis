#include <wx/wx.h>
#include <wx/sizer.h>



//**********************************************************************
//
//**********************************************************************
class wxImagePanel : public wxPanel
{
    wxBitmap image;
    
public:

    // Constructor
    wxImagePanel(wxFrame* parent, wxString file, wxBitmapType format);
    
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void render(wxDC& dc);
    
    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */
    
    DECLARE_EVENT_TABLE()
};


// Event table for wxImagePanel
BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
    // some useful events
    /*
     EVT_MOTION(wxImagePanel::mouseMoved)
     EVT_LEFT_DOWN(wxImagePanel::mouseDown)
     EVT_LEFT_UP(wxImagePanel::mouseReleased)
     EVT_RIGHT_DOWN(wxImagePanel::rightClick)
     EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
     EVT_KEY_DOWN(wxImagePanel::keyPressed)
     EVT_KEY_UP(wxImagePanel::keyReleased)
     EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
     */
    
    // catch paint events
    EVT_PAINT(wxImagePanel::paintEvent)

END_EVENT_TABLE()


// some useful events
/*
 void wxImagePanel::mouseMoved(wxMouseEvent& event) {}
 void wxImagePanel::mouseDown(wxMouseEvent& event) {}
 void wxImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
 void wxImagePanel::mouseReleased(wxMouseEvent& event) {}
 void wxImagePanel::rightClick(wxMouseEvent& event) {}
 void wxImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
 void wxImagePanel::keyPressed(wxKeyEvent& event) {}
 void wxImagePanel::keyReleased(wxKeyEvent& event) {}
 */

// Declare our main frame class
class MyFrame : public wxFrame
{
public:
    // Constructor
    MyFrame(const wxString& title);

    // Event handlers
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    wxImagePanel *drawPane;
    // This class handles events
    DECLARE_EVENT_TABLE()
};

// Event table for MyFrame
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

//**********************************************************************
//
//**********************************************************************
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // Create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // The "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT,
                     wxT("&About...\tF1"),
                     wxT("Show about dialog"));

    fileMenu->Append(wxID_EXIT,
                     wxT("E&xit\tAlt-X"),
                     wxT("Quit this program"));

    // Now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(helpMenu, wxT("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // Create a status bar just for fun
    CreateStatusBar(2);
    SetStatusText(wxT("Welcome to wxWidgets!"));

    // then simply create like this
    drawPane = new wxImagePanel(this,
                                wxT("output_012021.png"),
                                wxBITMAP_TYPE_JPEG);

#if 0
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        frame = new wxFrame(NULL,
                            wxID_ANY,
                            wxT("Hello wxDC"),
                            wxPoint(50,50),
                            wxSize(320,180));

        // then simply create like this
        drawPane = new wxImagePanel(frame,
                                    wxT("output_012021.png"),
                                    wxBITMAP_TYPE_JPEG);
        sizer->Add(drawPane, 1, wxEXPAND);

        frame->SetSizer(sizer);

        frame->Show();
#endif

}

//**********************************************************************
//
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
//
//**********************************************************************
void MyFrame::OnQuit(wxCommandEvent& event)
{
    // Destroy the frame
    Close();
}

//**********************************************************************
// Constructor
//**********************************************************************
wxImagePanel::wxImagePanel(wxFrame     *parent, 
                           wxString     file, 
                           wxBitmapType format) :
wxPanel(parent)
{
    // load the file... ideally add a check to see if loading was successful
    image.LoadFile(file, format);
}

//**********************************************************************
// Called by the system of by wxWidgets when the panel needs
// to be redrawn. You can also trigger this call by
// calling Refresh()/Update().
//**********************************************************************
void wxImagePanel::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
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
void wxImagePanel::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

//**********************************************************************
// Here we do the actual rendering. I put it in a separate
// method so that it can work no matter what type of DC
// (e.g. wxPaintDC or wxClientDC) is used.
//**********************************************************************
void wxImagePanel::render(wxDC&  dc)
{
    dc.DrawBitmap( image, 0, 0, false );
}


//**********************************************************************
// Main application 
//**********************************************************************
class MyApp: public wxApp
{
    
private:
    wxFrame      *frame;
    wxImagePanel *drawPane;

public:
    bool OnInit()
    {
        // make sure to call this first
        wxInitAllImageHandlers();

        // Create the main application window
        MyFrame *myFrame = new MyFrame(wxT("Minimal wxWidgets App"));
        
        // Show it
        myFrame->Show(true);

#if 0
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        frame = new wxFrame(NULL, 
                            wxID_ANY, 
                            wxT("Hello wxDC"), 
                            wxPoint(50,50), 
                            wxSize(320,180));
        
        // then simply create like this
        drawPane = new wxImagePanel(frame, 
                                    wxT("output_012021.png"), 
                                    wxBITMAP_TYPE_JPEG);
        sizer->Add(drawPane, 1, wxEXPAND);
        
        frame->SetSizer(sizer);
        
        frame->Show();
#endif

        return true;
    } 
    
};

IMPLEMENT_APP(MyApp)
