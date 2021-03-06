
#ifndef __IMG_H__
#define __IMG_H__

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#include <wx/wx.h>
#include <wx/sizer.h>

#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"
#include "CommonStatus.h"



class MyImagePanel;
class MyFrame;

//**********************************************************************
// Main application 
//**********************************************************************
class MyApp: public wxApp
{

private:
    MyFrame      *m_myFrame;


public:
    bool OnInit();

    MyImagePanel *m_pMyImagePanel;

};




//**********************************************************************
//
//**********************************************************************
class MyImagePanel : public wxPanel
{
    wxBitmap m_bitMap;
    
public:

    // Constructor
    MyImagePanel(wxFrame* parent, wxBitmapType format);
    
    void OnPaint(wxPaintEvent & evt);
    void OnMessage(wxCommandEvent & evt);
    void OnStatus(wxCommandEvent & evt);
    void paintNow();
    void render(wxDC& dc);
    void setFrame(uint32_t sn);
    void processTime(Msg_t *pMsg);
    void processVideoConfig(Msg_t *pMsg);
    void cbStatus(bool ok);
    void cbMessages(Msg_t *pMsg);


    
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


    MyFrame    *m_parent;

private:
    char        m_path[128];
    char        m_baseFn[128];
    VideoSync_t m_videoSync;

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
    void cbStatus(bool ok);

    MyImagePanel *getMyImagePanel(void);

    wxTextCtrl   *m_timeTxt;
    CommonStatus *m_pStatus;
    HubIf        *m_pHubIf;

private:
    MyImagePanel *m_myImagePanel;
    wxMenu       *m_fileMenu;
    wxMenu       *m_helpMenu;
    wxMenuBar    *m_menuBar;

    // This class handles events
    DECLARE_EVENT_TABLE()
};


#endif
