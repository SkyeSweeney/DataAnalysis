#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <functional>

#define USE_MSG_SIZES
#include "hub_if.h"
#include "msgs.h"
#include "nodes.h"

static void * receiverThread(void *arg);

#define SA struct sockaddr

typedef struct ReceiverThreadArgs_s
{
    HubIf *pHubIf;
    int    sockFd;
} ReceiverThreadArgs_t;


//######################################################################
//Command interface for Data Analysis
//######################################################################


//**********************************************************************
// Constructor
//**********************************************************************
HubIf::HubIf()
{
}

//**********************************************************************
// Destructor
//**********************************************************************
HubIf::~HubIf()
{
}

//**********************************************************************
//
//**********************************************************************
int HubIf::client_init(void)
{
    int i;
    struct sockaddr_in servaddr;

    for (i=0; i<MSGID_MAX; i++)
    {
        m_callbacks[i] = NULL;
    }

    // Create raw TCP socket
    m_sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockFd == -1) 
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully created..\n");
    }


    // Assign IP, PORT
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(HUB_PORT);

    int flag = 1;  
    int err;
    err = setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if (err == -1)
    {
        printf("setsockopt fail");  
    }  
    err = setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
    if (err == -1)
    {
        printf("setsockopt fail");  
    }  


    // Connect the client socket to server socket
    if (connect(m_sockFd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
    {
        printf("connection with the server failed (%d)...\n", errno);
        exit(0);
    }
    else
    {
        printf("connected to the server..\n");
    }

    ReceiverThreadArgs_t rta;
    rta.pHubIf = this;
    rta.sockFd = m_sockFd;

    // Start receiver thread
    pthread_create(&m_thread_id, NULL, receiverThread, (void *)&rta);

    return 0;
}



//**********************************************************************
// Login to the remote hub
//**********************************************************************
int HubIf::login(NodeId_t nodeId)
{
    Msg_t msg;
    m_nodeId = nodeId;
    this->sendMsg(&msg, MSGID_LOGIN, 0, 0);
    return 0;
}

//**********************************************************************
// Logout of the remote hub
//**********************************************************************
int HubIf::logout(NodeId_t nodeId)
{
    Msg_t msg;
    m_nodeId = nodeId; // TODO: Is this needed?
    this->sendMsg(&msg, MSGID_LOGOUT, 0, 0);
    m_nodeId = NODE_NONE;
    return 0;
}


//**********************************************************************
// Register a callback to handle a specific message
//**********************************************************************
int HubIf::registerCb(MsgId_t msgId, std::function<void(Msg_t*)>cb)
{
    int retval;
    Msg_t msg;

    if (msgId < MSGID_MAX)
    {
        // Save the callback function
        m_callbacks[msgId] = cb;

        // Send message to hub to have then send us this message
        msg.body.reg.msgId = msgId;
        msg.body.reg.add   = 1;
        this->sendMsg(&msg, MSGID_REGISTER, 0, 0);
        retval = 0;
    }
    else
    {
        printf("Registering a bad message\n");
        retval = 1;
    }
    return retval;
}

//**********************************************************************
// Unregister a callback for the specific messageId
//**********************************************************************
int HubIf::unregisterCb(MsgId_t msgId)
{
    if (msgId < MSGID_MAX)
    {
        m_callbacks[msgId] = NULL;
    }
    return 0;
}

//**********************************************************************
// Send a message to hub
//**********************************************************************
int HubIf::sendMsg(Msg_t *msg, MsgId_e msgId, uint32_t sec, uint32_t nsec)
{
    int err;

    // Check for leacal messageId
    if (msgId < MSGID_MAX)
    {

        // Populate the header as best we can
        msg->hdr.SOM    = 0x534B;
        msg->hdr.msgId  = msgId;
        msg->hdr.source = m_nodeId;
        msg->hdr.length = MSG_SIZES[msgId];
        msg->hdr.sec    = sec;
        msg->hdr.nsec   = nsec;


        if (0)
        {
            printf("SOM:%x msgId:%d Source:%d Length:%d\n",
                   msg->hdr.SOM,
                   msg->hdr.msgId,
                   msg->hdr.source,
                   msg->hdr.length);
        }
        err = send(m_sockFd, 
                  (void*)msg, 
                  sizeof(MsgHeader_t) + msg->hdr.length, 
                  0);
        if (0)
        {
            printf("Send %ld %d %d\n", 
                   sizeof(MsgHeader_t), 
                   msg->hdr.length, err);
        }
    }
    else
    {
        printf("Invalid msgid in send\n");
        err = 1;
    }

    return err;
}


//**********************************************************************
// Receiver thread
//**********************************************************************
static void * receiverThread(void *arg)
{

    Msg_t    msg;
    ssize_t  got;
    uint16_t n;
    MsgId_t  msgId;
    int      sockFd;
    HubIf   *pHubIf;

    // Extract arguments from structure
    ReceiverThreadArgs_t *rta;
    rta = (ReceiverThreadArgs_t *)arg;
    sockFd = rta->sockFd;
    pHubIf = rta->pHubIf;

    // Do forever
    for (;;)
    {

        // Clear the message
        memset(&msg, 0xff, sizeof(msg));

        // Read header
        got = read(sockFd, (void*)&msg.hdr, sizeof(MsgHeader_t));
        if (got == -1)
        {
            printf("error %d %d\n", errno, sockFd);
        }
        else
        {
            printf("Got header %ld\n", got);
        }

        // Check header
        if (msg.hdr.SOM != 0x534B)
        {
            // For now just hope next read works
            // Should really read byte by byte till we find the SOM
            continue;
        }

        // Get the message ID and size
        msgId = msg.hdr.msgId;
        n = msg.hdr.length;
        //printf("Body size is %d\n", n);

        // Read body
        got = read(sockFd, (void*)&msg.body, n);
        if (got != n)
        {
            printf("Got %ld and not %ld bytes\n", got, (size_t)n);
        }

        // If a valid message id
        if (msgId < MSGID_MAX)
        {

            // If a defined callback
            if (pHubIf->m_callbacks[msgId] != NULL)
            {
                pHubIf->m_callbacks[msgId](&msg);
            }

        }

    }

    return NULL;

}
