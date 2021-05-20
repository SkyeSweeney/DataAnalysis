
#ifndef __NODES_H__
#define __NODES_H__

#include <stdint.h>
#include <pthread.h>

#include "macro.h"


#define MAX_REGISTERED_MSGS 30

#define FOREACH_NODE(OP)       \
        OP(NODE_HUB,       0)  \
        OP(NODE_NONE,      0)  \
        OP(NODE_CMD,       0)  \
        OP(NODE_VIDEO,     0)  \
        OP(NODE_MAP,       0)  \
        OP(NODE_TIME,      0)  \
        OP(NODE_PLAYBACK,  0)  \
        OP(NODE_ROBOT,     0)  \
        OP(NODE_MAX,       0)


typedef enum NODE_ENUM {
    FOREACH_NODE(GENERATE_ENUM)
} NodeType_e;

#ifdef USE_NODE_STRING
static const char *NODE_STRING[] = {
    FOREACH_NODE(GENERATE_STRING)
};
#endif


typedef uint16_t NodeId_t;

// Control structure for a node
typedef struct
{
    int             sd;    // -1 for unused entry
    NodeType_e      nodeType;
    uint8_t         msgIds[MAX_REGISTERED_MSGS];  // True to send
    pthread_mutex_t mutex;
} Node_t;


void        nodesInit(void);
void        nodeInit(NodeId_t iNode);
int         nodeFindEmpty(void);
Node_t     *nodeGet(NodeId_t nodeId);
void        nodeRelease(NodeId_t nodeId);
const char *nodeIdToName(NodeId_t nodeId);




#endif
