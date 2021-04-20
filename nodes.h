
#ifndef __NODES_H__
#define __NODES_H__

#include <stdint.h>
#include <pthread.h>

#include "msgs.h"

typedef enum __attribute__((aligned (2)))
{
   NODE_NONE      = 0,
   NODE_CMD       = 1,
   NODE_VIDEO     = 2,
   NODE_MAP       = 3,
   NODE_TIME      = 4,
   NODE_PLAYBACK  = 5,
   NODE_MAX
} NodeType_e;

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
