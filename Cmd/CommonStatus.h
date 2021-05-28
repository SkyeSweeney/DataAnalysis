/////////////////////////////////////////////////////////////////////////////
// Name:        CommonStatus.h
/////////////////////////////////////////////////////////////////////////////


#ifndef CommonStatus_h
#define CommonStatus_h


class CommonStatus
{
public:
    wxFrame *ppp;
    CommonStatus(wxFrame *frame);

    void setTime(char *str);
    void setApp(char *str);
    void setConnection(bool ok);

private:
    wxFrame *m_frame;
};


#endif 

