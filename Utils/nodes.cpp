


#define USE_NODE_STRING
#include "nodes.h"


static Node_t m_nodes[NODE_MAX];

void nodesInit(void)
{
    int i;
    for (i=0; i<NODE_MAX; i++)
    {
        nodeInit(i);
    }
}

void nodeInit(NodeId_t nodeId)
{
    NodeId_t i;
    m_nodes[nodeId].sd = -1;
    m_nodes[nodeId].nodeType = NODE_NONE;
    for(i=0; i<MAX_REGISTERED_MSGS; i++)
    {
        m_nodes[nodeId].msgIds[i] = 0;
    }
    pthread_mutex_init(&m_nodes[nodeId].mutex, NULL);
}

int  nodeFindEmpty(void)
{
    NodeId_t nodeId;

    // Find an empty element in the Node structure
    for (nodeId=0; nodeId<NODE_MAX; nodeId++)
    {
        if (m_nodes[nodeId].sd == -1) break;
    }

    if (nodeId >= NODE_MAX)
    {
        nodeId = NODE_MAX;
    } 
    return nodeId;
}

Node_t *nodeGet(NodeId_t nodeId)
{
    // Take mutex
    pthread_mutex_lock(&m_nodes[nodeId].mutex);
    return  &m_nodes[nodeId];
}

void nodeRelease(NodeId_t nodeId)
{
    // Release mutex
    pthread_mutex_unlock(&m_nodes[nodeId].mutex);
}

const char *nodeIdToName(NodeId_t nodeId)
{

    if (nodeId >= NODE_MAX)
    {
        nodeId = NODE_MAX;
    }
    return NODE_STRING[nodeId];
}

