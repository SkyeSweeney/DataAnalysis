#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define USE_MSG_SIZES
#include "hub_if.h"
#include "msgs.h"
#include "nodes.h"

static void * hubif_receiveThread(void *arg);

#define SA struct sockaddr


//######################################################################
//Command interface for Data Analysis
//######################################################################

int m_sockfd;
pthread_t m_thread_id;
NodeId_t m_nodeId = NODE_NONE;

void (*m_callbacks[MSGID_MAX])(Msg_t *pMsg);


//**********************************************************************
//
//**********************************************************************
int hubif_client_init(void)
{
    int i;
    struct sockaddr_in servaddr;

    for (i=0; i<MSGID_MAX; i++)
    {
        m_callbacks[i] = NULL;
    }

    // Create raw TCP socket
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) 
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
    err = setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if (err == -1)
    {
        printf("setsockopt fail");  
    }  


    // Connect the client socket to server socket
    if (connect(m_sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
    {
        printf("connected to the server..\n");
    }

    // Start receiver thread
    pthread_create(&m_thread_id, NULL, hubif_receiveThread, NULL);

    return 0;
}



//**********************************************************************
//
//**********************************************************************
int hubif_login(NodeId_t nodeId)
{
    Msg_t msg;
    m_nodeId = nodeId;
    hubif_send(&msg, MSGID_LOGIN, 0, 0);
    return 0;
}

//**********************************************************************
//
//**********************************************************************
int hubif_logout(NodeId_t nodeId)
{
    Msg_t msg;
    m_nodeId = nodeId; // TODO: Is this needed?
    hubif_send(&msg, MSGID_LOGOUT, 0, 0);
    m_nodeId = NODE_NONE;
    return 0;
}


//**********************************************************************
//
//**********************************************************************
int hubif_register(MsgId_t msgId, void (*cb)(Msg_t *msg))
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
        hubif_send(&msg, MSGID_REGISTER, 0, 0);
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
//
//**********************************************************************
int hubif_unregister(MsgId_t msgId)
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
int hubif_send(Msg_t *msg, MsgId_e msgId, uint32_t sec, uint32_t nsec)
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
        err = send(m_sockfd, 
                  (void*)msg, 
                  sizeof(MsgHeader_t) + msg->hdr.length, 
                  0);
        if (0)
        {
            printf("Send %ld %d %d\n", sizeof(MsgHeader_t), msg->hdr.length, err);
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
//
//**********************************************************************
static void * hubif_receiveThread(void *arg)
{

    Msg_t msg;
    ssize_t got;
    uint16_t n;
    MsgId_t msgId;

    // Do forever
    for (;;)
    {

        // Clear the message
        memset(&msg, 0xff, sizeof(msg));

        // Read header
        got = read(m_sockfd, (void*)&msg.hdr, sizeof(MsgHeader_t));
        //printf("Got header %ld\n", got);

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
        got = read(m_sockfd, (void*)&msg.body, n);
        if (got != n)
        {
            printf("Got %ld and not %ld bytes\n", got, (size_t)n);
        }

        // If a valid message id
        if (msgId < MSGID_MAX)
        {
            // If a defined callback
            if (m_callbacks[msgId] != NULL)
            {
                m_callbacks[msgId](&msg);
            }

        }

    }

    return NULL;

}
