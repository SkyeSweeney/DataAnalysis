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

// Holds the arguments to be passed into thread
typedef struct ConnectionThreadArgs_s
{
    HubIf *pHubIf;
} ConnectionThreadArgs_t;

static void * connectionThread(void *arg);

// Arguments must be fixed in memory
static ConnectionThreadArgs_t cta;

// Shorthand
#define SA struct sockaddr




//######################################################################
//Command interface for Data Analysis
//######################################################################


//**********************************************************************
// Constructor
//**********************************************************************
HubIf::HubIf()
{
    m_sockFd = -1;
    m_run    = true;
    m_debug  = false;
}

//**********************************************************************
// Destructor
//**********************************************************************
HubIf::~HubIf()
{
}

//**********************************************************************
// Kill the interface to shutdown app
//**********************************************************************
void HubIf::shutdown(void)
{
    // Clear the run flag
    m_run = false;

    // Now close the socket. That should piss lots of people
    m_sockFd = -1;

    // Wait for thread to terminate
    pthread_join(m_connectionThread_id, NULL);
}

//**********************************************************************
// Initialize a client interface
//**********************************************************************
int HubIf::client_init(void)
{
    int i;

    // Clear out all callbacks
    for (i=0; i<MSGID_MAX; i++)
    {
        m_callbacks[i] = NULL;
    }
    m_statusCb = NULL;


    // Start connection thread
    // This will continue to create connections as the last ones fail
    // Each connection will process incomming messages via callbacks
    // that must be registered with the interface.
    cta.pHubIf = this;
    if (cta.pHubIf == NULL)
    {
        printf("NULL-2\n");
        exit(1);
    }
    pthread_create(&m_connectionThread_id, 
                   NULL, 
                   connectionThread, 
                   (void *)&cta);

    return 0;
}


//**********************************************************************
// Connection thread
// This thread continues to attempt to make connections to hub
//**********************************************************************
static void * connectionThread(void *arg)
{
    Msg_t    msg;
    ssize_t  got;
    uint16_t n;
    MsgId_t  msgId;
    HubIf   *pHubIf;
    int      tempSd;
    struct sockaddr_in servaddr;

    // Extract arguments from structure
    ConnectionThreadArgs_t *pCta;
    pCta = (ConnectionThreadArgs_t *)arg;
    pHubIf = pCta->pHubIf;
    if (pHubIf == NULL)
    {
        printf("NULL-1\n");
        exit(1);
    }

    // For every connection
    while(pHubIf->m_run)
    {

        printf("Attempt to create a connection.\n");

        // Create raw TCP socket
        tempSd = socket(AF_INET, SOCK_STREAM, 0);
        if (tempSd == -1) 
        {
            printf("socket creation failed...\n");
            sleep(1);
            continue;
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
        err = setsockopt(tempSd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
        if (err == -1)
        {
            printf("setsockopt fail");  
            close(tempSd);
            continue;
        }  
        err = setsockopt(tempSd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
        if (err == -1)
        {
            printf("setsockopt fail");  
            close(tempSd);
            continue;
        }  


        // Connect the client socket to server socket
        if (connect(tempSd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
        {
            printf("connection with the server failed (%d)...\n", errno);
            close(tempSd);
            sleep(1);
            continue;
        }
        else
        {
            printf("connected to the server..\n");

            // Now that we are a connection, set it in the HubIf
            pHubIf->m_sockFd = tempSd;

            // If a defined callback
            if (pHubIf->m_statusCb != NULL)
            {
                pHubIf->m_statusCb(true);
            }
        }


        // Do for every incomming message
        for (;;)
        {

            // Clear the message
            memset(&msg, 0xff, sizeof(msg));

            // Read header
            got = read(pHubIf->m_sockFd, (void*)&msg.hdr, sizeof(MsgHeader_t));

            if (got == -1)
            {
                printf("Error on header read %d %d\n", errno, pHubIf->m_sockFd);
                break;
            }
            else if (got == 0)
            {
                printf("EOF on read of header\n");
                break;
            }
            else if (got == sizeof(MsgHeader_t))
            {
                // Life is good
            }
            else
            {
                printf("Bad size on header read\n");
                break;
            }

            // Check header
            if (msg.hdr.SOM != MSG_SOM)
            {
                printf("Bad SOM in header\n");
                break;
            }

            // Get the message ID and size
            msgId = msg.hdr.msgId;
            n = msg.hdr.length;

            if (n > 0)
            {

                // Read body
                got = read(pHubIf->m_sockFd, (void*)&msg.body, n);

                if (got == -1)
                {
                    printf("Error on body read %d %d\n", errno, pHubIf->m_sockFd);
                    break;
                }
                else if (got == 0)
                {
                    printf("EOF on read of body\n");
                    break;
                }
                else if (got == n)
                {
                    // Life is good
                }
                else
                {
                    printf("Bad size on body read\n");
                    break;
                }

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

        } // For each message

        close(pHubIf->m_sockFd);
        pHubIf->m_sockFd = -1;
        printf("Receiver thread terminated.\n");

        // If a defined callback
        if (pHubIf->m_statusCb != NULL)
        {
            pHubIf->m_statusCb(false);
        }

    } // for each connection

    return NULL;

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
// Register a callback to handle connection changes
//**********************************************************************
void HubIf::registerStatus(std::function<void(bool ok)>cb)
{
    // Save the callback function
    m_statusCb = cb;
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

    if (m_sockFd < 1) return(0);

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


        if (m_debug)
        {
            printf("SOM:%x msgId:%d Source:%d Length:%d\n",
                   msg->hdr.SOM,
                   msg->hdr.msgId,
                   msg->hdr.source,
                   msg->hdr.length);
        }

        // Attempt to send the message
        err = send(m_sockFd, 
                  (void*)msg, 
                  sizeof(MsgHeader_t) + msg->hdr.length, 
                  0);

        if (err == -1)
        {
            printf("Error on send %d\n", errno);
        } 
        else
        {
            printf("Sent type:%d hdrSize:%ld bodySize:%d\n", 
                   msg->hdr.msgId,
                   sizeof(MsgHeader_t), 
                   msg->hdr.length);
        }
    }
    else
    {
        printf("Invalid msgid(%d) in send\n", msgId);
        err = 1;
    }

    return err;
}
