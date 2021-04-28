

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>


#include "msgs.h"
#include "nodes.h"
#include "hub_if.h"



static void *nodeThread(void *arg);
static void *userThread(void *parg);
static void processMsg(NodeId_t nodeId, Msg_t *pMsg);
static void processLogin(NodeId_t nodeId, Msg_t *pMsg);
static void processLogout(NodeId_t nodeId, Msg_t *pMsg);
static void processRegister(NodeId_t nodeId, Msg_t *pMsg);
static void processExit(NodeId_t nodeId, Msg_t *pMsg);

//**********************************************************************
//
//**********************************************************************
int main(int argc, char *argv[])
{
    int                hubSd;
    int                nodeSd = 0;
    struct sockaddr_in serv_addr; 
    struct sockaddr    from;
    socklen_t          len;
    NodeId_t           nodeId;


    // Initialize the Node structure
    nodesInit();

    // Start the user interface thread
    pthread_t userThreadId;
    pthread_create(&userThreadId, NULL, userThread, NULL);

    // Open up TCP listen socket
    hubSd = socket(AF_INET, SOCK_STREAM, 0);

    // Create address to bind to
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(HUB_PORT); 

    // Bind to address
    bind(hubSd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    // Listen on socket for up to 10 simultaneous connection requests
    listen(hubSd, 10); 

    // Do forever
    for (; ; )
    {

        // Accept connection
        len = sizeof(from);
        nodeSd = accept(hubSd, &from, &len);
        printf("Got connection from TBD on socket %d\n", nodeSd);

        // Find an empty control structure for this node
        nodeId = nodeFindEmpty();

        // If we found an empty spot
        if (nodeId != NODE_MAX)
        {
            // Add to node list
            Node_t *pNode;
            pNode = nodeGet(nodeId);
            pNode->sd       = nodeSd;
            pNode->nodeType = NODE_NONE;
            nodeRelease(nodeId);

            // Spawn off thread to handle incomming messages.
            // Pass it in the node id so it knows who its talking to.
            pthread_t threadId;
            pthread_create(&threadId, NULL, nodeThread, (void*)&nodeId);
    
        } 
        else 
        {
            printf("No room for node\n");
            close(nodeSd);
        }

    }

}


//**********************************************************************
//
//**********************************************************************
static void *userThread(void *parg)
{

    int i;
    Node_t  *pNode;
    char    *pCmdBuf;

    for (;;)
    {
        // Get a user command
        pCmdBuf = readline("Cmd> ");

        // Parse command

        // Switch based on command
        switch(1)
        {
            case 1:

                printf("| id | sd  | NodeType             |\n");
                for (i=0; i<NODE_MAX; i++)
                {
                    // Get the socket we are to use
                    pNode = nodeGet(i);
                    printf("| %02d | %03d | %-20s |\n", 
                           i, 
                           pNode->sd, 
                           nodeIdToName(pNode->nodeType));
                    nodeRelease(i);
                }
                break;
            default:
                break;
        }
        free(pCmdBuf);

    }
    return NULL;
}


//**********************************************************************
//
//**********************************************************************
// Thread to handle incoming messages from one node
// The nodeId of the node we are handling is passed in vias parg
static void *nodeThread(void *parg)
{

    NodeId_t nodeId;
    int      sd;
    ssize_t  n;
    Msg_t    msg;
    Node_t  *pNode;

    // Get node id from argument
    nodeId = *((NodeId_t*)parg);

    // Get the socket we are to use
    pNode = nodeGet(nodeId);
    sd = pNode->sd;
    nodeRelease(nodeId);

    //printf("Node %d sd %d\n", nodeId, sd);

    // Do till the connection is closed
    for (;;)
    {

        // Read the next header into the message buffer
        n = read(sd, (void*)(&msg.hdr), sizeof(MsgHeader_t));
        
        if (n == 0)
        {
            printf("Node closed. Logging out.\n");
            close(sd);
            pNode->sd = -1;
            pNode->nodeType = NODE_NONE;
            nodeRelease(nodeId);
            break;
        }

        // Verify header
        if (msg.hdr.SOM != 0x534b)
        {
            // Flush buffer and hope for the best
            continue;
        }

        // If we have a body
        if (msg.hdr.length > 0)
        {

            // Read body
            n = read(sd, (void*)(&msg.body), msg.hdr.length);
        
            if (n == 0)
            {
                printf("Node closed\n");
                break;
            }
        }

        // Call message processor
        processMsg(nodeId, &msg);

    }

    return NULL;
}


//**********************************************************************
//
//**********************************************************************
static void processMsg(NodeId_t nodeId, Msg_t *pMsg)
{
    MsgId_e    msgId;
    uint16_t   len;
    int        ok;
    Node_t     *pNode;

    // Get the source
    len = pMsg->hdr.length;

    // Get the msgId;
    msgId = (MsgId_e)pMsg->hdr.msgId;

    // Switch on type
    switch(msgId)
    {
        // Node asking to join
        case MSGID_LOGIN:
            processLogin(nodeId, pMsg);
            break;

        // Node asking to leave
        case MSGID_LOGOUT:
            processLogout(nodeId, pMsg);
            break;

        // Node registering for a message
        case MSGID_REGISTER:
            processRegister(nodeId, pMsg);
            break; 

        // Node registering for a message
        case MSGID_EXIT:
            processExit(nodeId, pMsg);
            break; 

        default:


            // If msgId is legal
            if (msgId < MSGID_MAX)
            {
                //printf("Message to route %d\n", msgId);

                // For each node 
                for (nodeId=0; nodeId<NODE_MAX; nodeId++)
                {

                    pNode = nodeGet(nodeId);

                    // If a defined node
                    if (pNode->sd != -1)
                    {

                        // If is to be sent
                        if (pNode->msgIds[msgId])
                        {

                            // Attempt to send message
                            //printf("Sending\n");
                            ok = send(pNode->sd, pMsg, len+sizeof(MsgHeader_t), 0);
    
                            // If it failed, nuke the node
                            if (ok != 0)
                            {
                                // Close down node
                                //deleteNode(i);
                            }
                        }
                    } // Defined node

                    nodeRelease(nodeId);

                } // for each node
            }
            else
            {
                printf("Someone trying to send us bad message %d\n", msgId);
            }
    }
    
}

//**********************************************************************
//
//**********************************************************************
static void processLogin(NodeId_t nodeId, Msg_t *pMsg)
{
    Node_t *pNode;
    pNode = nodeGet(nodeId);
    pNode->nodeType = (NodeType_e)pMsg->hdr.source;
    printf("Log in from node %d as type %d\n", nodeId, pMsg->hdr.source);
    nodeRelease(nodeId);
}

//**********************************************************************
//
//**********************************************************************
static void processLogout(NodeId_t nodeId, Msg_t *pMsg)
{
    Node_t *pNode;
    pNode = nodeGet(nodeId);
    pNode->sd = -1;
    pNode->nodeType = NODE_NONE;
    printf("Log out nodeId %d\n", nodeId);
    nodeRelease(nodeId);
}

//**********************************************************************
//
//**********************************************************************
static void processRegister(NodeId_t nodeId, Msg_t *pMsg)
{
    MsgId_t msgId;
    uint16_t add;
    Node_t *pNode;

    msgId = pMsg->body.reg.msgId;
    add   = pMsg->body.reg.add;
    pNode = nodeGet(nodeId);
    printf("Register %d %d\n", msgId, add);
    pNode->msgIds[msgId] = add;
    nodeRelease(nodeId);
}

//**********************************************************************
//
//**********************************************************************
static void processExit(NodeId_t nodeId, Msg_t *pMsg)
{
    printf("Exit\n");
}
