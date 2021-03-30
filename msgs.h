

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
    MSGID_CONFIG   = 7,
    MSGID_LOCATION = 8,
    MSGID_TIME     = 9,
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
} BodyRegister_t;

typedef struct
{
    uint32_t frame;    // Frame number to display
    uint32_t sec;      // Whole seconds since 1970
    uint32_t nsec;     // nano seconds into second
} BodyFrame_t;

typedef struct
{
    uint32_t sec;
    uint32_t nsec;
    uint8_t  level;
    char     string[1000];
} BodyLog_t;

typedef struct
{
    char     videoDir[256]; // Directory where video files can be found
    char     videoRoot[24]; // Root file name of video files
} BodyConfig_t;

typedef struct
{
    double   northing;
    double   easting;
    double   altitude;
    double   speed;
    double   heading;
} BodyLocation_t;

typedef struct
{
    uint32_t  sec;
    uint32_t  nsec;
} BodyTime_t;

typedef struct
{
    uint8_t donotuse[1024];
} BodyGeneric_t;

typedef union
{
    BodyLogin_t    login;
    BodyLogout_t   logout;
    BodyRegister_t reg;
    BodyFrame_t    frame;
    BodyLog_t      log;
    BodyConfig_t   config;
    BodyLocation_t location;
    BodyTime_t     time;
    BodyGeneric_t  generic;
} Body_t;

typedef struct
{
    MsgHeader_t hdr;
    Body_t      body;
} Msg_t;


#endif
