

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
typedef struct MsgHeader_s
{
    uint16_t    SOM;    // 0x534b
    MsgId_t     msgId;
    NodeType_t  source;
    uint16_t    length;
} MsgHeader_t;
#pragma pack(0)


// Login message. The type you want to login as comes from the header source
typedef struct BodyLogin_s
{
    // No body
} BodyLogin_t;

// Logout message. Header only
typedef struct BodyLogout_s
{
    // No body
} BodyLogout_t;

typedef struct BodyRegister_s
{
    MsgId_t  msgId;  // ID of message to register
    uint16_t add;    // non zero to register, 0 to unregister
} BodyRegister_t;

typedef struct BodyFrame_s
{
    uint32_t frame;    // Frame number to display
    uint32_t sec;      // Whole seconds since 1970
    uint32_t nsec;     // nano seconds into second
} BodyFrame_t;

typedef struct BodyLog_s
{
    uint32_t sec;     // Epoch seconds of event
    uint32_t nsec;    // nanosecond into second of event
    uint8_t  level;   // Class of event (INFO, DEBUG, ...)
    char     string[1000]; // String of event
} BodyLog_t;

typedef struct BodyConfig_s
{
    char     videoDir[256]; // Directory where video files can be found
    char     videoRoot[24]; // Root file name of video files
} BodyConfig_t;

typedef struct BodyLocation_s
{
    double   northing;   // Northing::D
    double   easting;    // Easting::D
    double   altitude;   // Altitude::D
    double   speed;      // Speed::D
    double   heading;    // Heading::D
} BodyLocation_t;

typedef struct BodyTime_s
{
    uint32_t  sec;    // Basic simulation time (epoch seconds)
    uint32_t  nsec;   // Basic simulation time (nonos into above)
} BodyTime_t;

typedef struct BodyTest_s
{
    union f_u
    {
        uint32_t val;
        struct u_s
        {
            uint32_t a:10;
            uint32_t b:10;
            uint32_t c:10;
            uint32_t d:2;
        } u_t;
        struct s_s
        {
            uint32_t q;
            uint16_t r;
        } s_t;
    } f_t;

} BodyTest_t;

typedef struct BodyGeneric_s
{
    uint8_t donotuse[1024];  // Generic max size message
} BodyGeneric_t;


// Union of all the message bodies
typedef union Body_s
{
    BodyLogin_t    login;
    BodyLogout_t   logout;
    BodyRegister_t reg;
    BodyFrame_t    frame;
    BodyLog_t      log;
    BodyConfig_t   config;
    BodyLocation_t location;
    BodyTime_t     time;
    BodyTest_t     test;
    BodyGeneric_t  generic;
} Body_t;

// A generic message is a header and a body
typedef struct Msg_s
{
    MsgHeader_t hdr;
    Body_t      body;
} Msg_t;


#endif
