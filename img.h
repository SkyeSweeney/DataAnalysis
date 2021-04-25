
#ifndef __IMG_H__
#define __IMG_H__

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/timer.h>

class MyImagePanel;
class MyFrame;

//**********************************************************************
// Main application 
//**********************************************************************
class MyApp: public wxApp
{

private:
    MyFrame      *m_myFrame;
    MyImagePanel *m_myImagePanel;

public:
    bool OnInit();

};




//**********************************************************************
//
//**********************************************************************
class MyImagePanel : public wxPanel
{
    wxBitmap m_image;
    
public:

    // Constructor
    MyImagePanel(wxFrame* parent, wxString file, wxBitmapType format);
    
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void render(wxDC& dc);
    void OnTimer(wxTimerEvent & evt);
    
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

    wxTimer m_timer;
    uint16_t m_sn;
};



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
    MyImagePanel *m_myImagePanel;
    wxTextCtrl   *m_timeTxt;
    wxMenu       *m_fileMenu;
    wxMenu       *m_helpMenu;
    wxMenuBar    *m_menuBar;

    // This class handles events
    DECLARE_EVENT_TABLE()
};


#endif
