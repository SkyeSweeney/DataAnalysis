

#ifndef __MSGS_H__
#define __MSGS_H__

#include <stdint.h>



typedef enum
{
    MSGID_UNK      = 0,
    MSGID_LOGIN    = 1,
    MSGID_LOGOUT   = 2,
    MSGID_REGISTER = 3,
    MSGID_MAX
} MsgId_e;


#include "nodes.h"

typedef uint16_t MsgId_t;
typedef uint16_t NodeType_t;

#pragma pack(1)
typedef struct
{
    MsgId_t     msgId;
    NodeType_t  source;
    uint16_t    length;
    uint16_t    spare;
} MsgHeader_t;
#pragma pack(0)


// Login message. The type you want to login as comes from the header source
typedef struct
{
} BodyLogin_t;

// Logout message. Header only
typedef struct
{
} BodyLogout_t;

typedef struct
{
    MsgId_t  nodeType;
    uint16_t add;
} BodyRegister;

typedef struct
{
    uint8_t donotuse[1024];
} BodyGeneric;

typedef union
{
    BodyLogin_t  login;
    BodyLogout_t logout;
    BodyRegister reg;
    BodyGeneric  generic;
} Body_t;

typedef struct
{
    MsgHeader_t hdr;
    Body_t      body;
} Msg_t;


#endif
