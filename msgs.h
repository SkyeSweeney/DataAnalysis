

#ifndef __MSGS_H__
#define __MSGS_H__

#include <stdint.h>



typedef enum
{
    MSGID_LOGIN,
    MSGID_LOGOUT,
    MSGID_REGISTER,
    MSGID_MAX
} MsgId_e;


#include "nodes.h"

typedef struct
{
    MsgId_e    msgId;
    NodeType_e source;
    uint16_t   length;
} MsgHeader_t;

typedef struct
{
    MsgHeader_t hdr;
    NodeType_e  nodeType;
} MsgLogin_t;

typedef struct
{
    MsgHeader_t hdr;
    NodeType_e  nodeType;
} MsgLogout_t;

typedef struct
{
    MsgHeader_t hdr;
    MsgId_e     msgId;
    uint8_t     add;    // True to add, false to remove
} MsgRegister_t;


#endif
