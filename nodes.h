
#ifndef __NODES_H__
#define __NODES_H__

#include <stdint.h>

#include "msgs.h"

typedef enum
{
   NODE_NONE   = 0,
   NODE_MASTER = 1,
   NODE_VIDEO  = 2,
   NODE_MAP    = 3,
   NODE_MAX
} NodeType_e;

// Control structure for a node
typedef struct
{
    int        sd;    // -1 for unused entry
    NodeType_e nodeType;
    uint8_t    msgIds[MSGID_MAX];  // True to send
} Node_t;




#endif
