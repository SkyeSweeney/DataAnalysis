

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

#include "msgs.h"
#include "nodes.h"


#define PORT 5000


void *nodeThread(void *arg);
void processMsg(Msg_t *pMsg);

void main(int argc, char *argv[])
{
    int                j;
    int                hubSd;
    int                nodeSd = 0;
    struct sockaddr_in serv_addr; 
    struct sockaddr    from;
    socklen_t          len;
    NodeId_t           nodeId;


    // Initialize the Node structure
    nodesInit();

    // Open up TCP listen socket
    hubSd = socket(AF_INET, SOCK_STREAM, 0);

    // Create address to bind to
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    // Bind to address
    bind(hubSd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    // Listen on socket for up to 10 simultaneous connection requests
    listen(hubSd, 10); 

    // Do forever
    for (j=0; j<5; j++)
    {

        // Accept connection
        printf("Waiting for connection...");
        len = sizeof(from);
        nodeSd = accept(hubSd, &from, &len);
        printf(" Got one!\n");

        nodeId = nodeFindEmpty();

        if (nodeId != NODE_MAX)
        {
            // Add to node list
            Node_t *pNode;
            pNode = nodeGet(nodeId);
            pNode->sd       = nodeSd;
            pNode->nodeType = NODE_NONE;
            nodeRelease(nodeId);

            // Spawn off thread to handle it passing in the nodeId
            pthread_t threadId;
            pthread_create(&threadId, NULL, nodeThread, (void*)&nodeId);
    
        } 
        else 
        {
            printf("No room for node\n");
            close(nodeSd);
        }

    }

    sleep(10);


}


// pargs points to a NodeId_t
void *nodeThread(void *parg)
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

    printf("Node %d sd %d\n", nodeId, sd);

    // Do till the connection is closed
    for (;;)
    {

        // Read the next header into the message buffer
        n = read(sd, (void*)(&msg.hdr), sizeof(MsgHeader_t));
        
        if (n == 0)
        {
            printf("Node closed\n");
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
        processMsg(&msg);

    }
}


void processMsg(Msg_t *pMsg)
{
    MsgId_e    msgId;
    NodeType_e src;
    uint16_t   len;
    int        ok;
    NodeId_t   nodeId;
    Node_t     *pNode;

    // Get the source
    src = pMsg->hdr.source;
    len = pMsg->hdr.length;

    // Get the msgId;
    msgId = pMsg->hdr.msgId;

    // Switch on type
    switch(msgId)
    {
        // Node asking to join
        case MSGID_LOGIN:
            printf("Login\n");
            //doMsgLogin(nodeId, msg);
            break;

        // Node asking to leave
        case MSGID_LOGOUT:
            printf("Logout\n");
            //doMsgLogout(nodeId, msg);
            break;

        // Node registering for a message
        case MSGID_REGISTER:
            printf("Register\n");
            //doMsgRegister(nodeId, msg);
            break; 

        default:


            // If msgId is legal
            if (msgId < MSGID_MAX)
            {
                printf("Message to route %d\n", msgId);

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
                            ok = send(pNode->sd, pMsg, len+6, 0);
    
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

