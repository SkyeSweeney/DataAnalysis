#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

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

void (*m_callbacks[MSGID_MAX])(Msg_t *pMsg);


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
    if (-1 == setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {  
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


NodeId_t m_nodeId = NODE_NONE;

int hubif_login(NodeId_t nodeId)
{
    Msg_t msg;
    msg.hdr.SOM = 0x534B;
    msg.hdr.msgId = MSGID_LOGIN;
    msg.hdr.source = nodeId;
    msg.hdr.length = 0;
    hubif_send(&msg);
    m_nodeId = nodeId;
    return 0;
}


int hubif_register(MsgId_t msgId, void (*cb)(Msg_t *msg))
{
    Msg_t msg;
    if (msgId < MSGID_MAX)
    {
        m_callbacks[msgId] = cb;
    }
    msg.hdr.SOM = 0x534B;
    msg.hdr.msgId = MSGID_REGISTER;
    msg.hdr.source = m_nodeId;
    msg.hdr.length = sizeof(BodyRegister_t);
    msg.body.reg.msgId = MSGID_FRAME;
    msg.body.reg.add   = 1;
    hubif_send(&msg);
    return 0;
}

int hubif_unregister(MsgId_t msgId)
{
    if (msgId < MSGID_MAX)
    {
        m_callbacks[msgId] = NULL;
    }
    return 0;
}

int hubif_send(Msg_t *msg)
{
    int err;
    printf("SOM:%x msgId:%d Source:%d Length:%d\n",
           msg->hdr.SOM,
           msg->hdr.msgId,
           msg->hdr.source,
           msg->hdr.length);
    err = send(m_sockfd, (void*)msg, sizeof(MsgHeader_t) + msg->hdr.length, 0);
    printf("Send %d\n", err);
    return err;
}


static void * hubif_receiveThread(void *arg)
{

    Msg_t msg;
    int   err;
    uint16_t n;
    MsgId_t msgId;

    // Do forever
    for (;;)
    {

        // Read header
        err = read(m_sockfd, (void*)&msg.hdr, sizeof(MsgHeader_t));

        // Check header
        if (msg.hdr.SOM != 0x534B)
        {
            // For now jsut hope next read works
            continue;
        }
        n = msg.hdr.length;

        // Read body
        err = read(m_sockfd, (void*)&msg.body, n);

        msgId = msg.hdr.msgId;

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
