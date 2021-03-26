

#ifndef __MSGS_H__
#define __MSGS_H__

#include <stdint.h>



typedef enum
{
    MSGID_UNK      = 0,
    MSGID_LOGIN    = 1,
    MSGID_LOGOUT   = 2,
    MSGID_EXIT     = 3,
    MSGID_REGISTER = 4,
    MSGID_FRAME    = 5,
    MSGID_LOG      = 6,
    MSGID_MAX
} MsgId_e;


#include "nodes.h"

typedef uint16_t MsgId_t;
typedef uint16_t NodeType_t;

#pragma pack(1)
typedef struct
{
    uint16_t    SOM;    // 0x534b
    MsgId_t     msgId;
    NodeType_t  source;
    uint16_t    length;
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
    MsgId_t  msgId;  // ID of message to register
    uint16_t add;    // non zero to register, 0 to unregister
} BodyRegister;

typedef struct
{
    uint32_t frame;    // Frame number to display
    uint32_t sec;
    uint32_t nsec;
} BodyFrame;

typedef struct
{
    uint32_t ts;
    uint8_t  level;
    char     string[1000];
} BodyLog;

typedef struct
{
    uint8_t donotuse[1024];
} BodyGeneric;

typedef union
{
    BodyLogin_t  login;
    BodyLogout_t logout;
    BodyRegister reg;
    BodyFrame    frame;
    BodyLog      log;
    BodyGeneric  generic;
} Body_t;

typedef struct
{
    MsgHeader_t hdr;
    Body_t      body;
} Msg_t;


#endif
