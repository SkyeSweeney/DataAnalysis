

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


Node_t Nodes[NODE_MAX];

void *nodeThread(void *arg);

void main(int argc, char *argv[])
{
    int                j;
    int                hubSd;
    int                nodeSd = 0;
    struct sockaddr_in serv_addr; 
    struct sockaddr    from;
    socklen_t          len;
    uint16_t           nodeId;


    for (j=0; j<NODE_MAX; j++)
    {
        Nodes[j].sd = -1;
    }

    

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
        //nodeSd = accept(hubSd, &from, &len);
        printf(" Got one!");

        // Find an empty element in the Node structure
        for (nodeId=0; nodeId<NODE_MAX; nodeId++)
        {
            if (Nodes[nodeId].sd == -1) break;
        }

        if (nodeId < NODE_MAX)
        {
            // Add to node list
            Nodes[nodeId].sd       = nodeSd;
            Nodes[nodeId].nodeType = NODE_NONE;

            // Spawn off thread to handle it
            pthread_t threadId;
            void *pargs;
            pargs = (void*)&nodeId;
            pthread_create(&threadId, NULL, nodeThread, pargs);
    
        } 
        else 
        {
            printf("No room for node\n");
            close(nodeSd);
        }

    }

    sleep(10);


}


void *nodeThread(void *parg)
{

    uint16_t nodeId;

    // Get node id from argument
    nodeId = *((uint16_t*)parg);

    // Do till the connection is closed
    for (;;)
    {
        // Wait for message from node
        //msg = read(Nodes[nodId].sd, ..);

        // Call message processor
        //processMsg(msg);

        printf("!\n");
        sleep(1);
    }
}

#if 0

void processMsg(MSG *pMsg)
{
    // Get the source

    // Get the msgId;
    msgId = msg.hdr.id;

    // Switch on type
    switch(msgId)
    {
        // Node asking to join
        case MSG_ID_LOGIN:
            doMsgLogin(nodeId, msg);
            break;

        // Node asking to leave
        case MSG_ID_LOGOUT:
            doMsgLogout(nodeId, msg);
            break;

        // Node registering for a message
        case MSG_ID_REGISTER:
            doMsgRegister(nodeId, msg);
            break; 

        default:

            // If msgId is legal
            if (msgId < MAX_MSGS)
            {
                // For each node 
                for (i=0; i<MAX_NODES; i++)
                {
                    // Skip unused nodes
                    if (nodes[i].sd == -1) continue;

                    // If is to be sent
                    if (nodes[i].msgIds[msgId])
                    {

                        // Attempt to send message
                        ok = send(Nodes[i].sd, msg);

                        // If it failed, nuke the node
                        if (!ok)
                        {
                            // Close down node
                            deleteNode(i);
                        }
                        
                    }
                }
            }
            else
            {
                print("Someone trying to send us bad message\n");
            }
    }
    
}


void sendMsg(msg)
{
    msgId = msg.hdr.id;
    
    // if this message is needed by someone
    if (registered(msgId))
    {


    }

}



void connectionThread(void)
{
}

#endif
