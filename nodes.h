
#ifndef __NODES_H__
#define __NODES_H__

#include <stdint.h>
#include <pthread.h>

#include "msgs.h"


#define FOREACH_NODE(OP) \
        OP(NODE_NONE)   \
        OP(NODE_CMD)  \
        OP(NODE_VIDEO)   \
        OP(NODE_MAP)   \
        OP(NODE_TIME)   \
        OP(NODE_PLAYBACK)   \
        OP(NODE_MAX)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

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
    uint8_t         msgIds[MSGID_MAX];  // True to send
    pthread_mutex_t mutex;
} Node_t;


void nodesInit(void);
void nodeInit(NodeId_t iNode);
int  nodeFindEmpty(void);
Node_t *nodeGet(NodeId_t nodeId);
void nodeRelease(NodeId_t nodeId);
const char *nodeIdToName(NodeId_t nodeId);




#endif
