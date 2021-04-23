

#ifndef __MSGS_H__
#define __MSGS_H__

#include <stdint.h>
#include "macro.h"



#define FOREACH_MSG(OP) \
    OP(MSGID_UNK,        0) \
    OP(MSGID_LOGIN,      sizeof(BodyLogin_t)) \
    OP(MSGID_LOGOUT,     sizeof(BodyLogout_t)) \
    OP(MSGID_EXIT,       0) \
    OP(MSGID_REGISTER,   sizeof(BodyRegister_t)) \
    OP(MSGID_FRAME,      sizeof(BodyFrame_t)) \
    OP(MSGID_LOG,        sizeof(BodyLog_t)) \
    OP(MSGID_CONFIG,     sizeof(BodyConfig_t)) \
    OP(MSGID_LOCATION,   sizeof(BodyLocation_t)) \
    OP(MSGID_TIME,       sizeof(BodyTime_t)) \
    OP(MSGID_PLAYBACK,   sizeof(BodyPlayback_t)) \
    OP(MSGID_MAX,        0)


typedef enum MSG_ENUM {
    FOREACH_MSG(GENERATE_ENUM)
} MsgId_e;

#ifdef USE_MSG_STRING
static const char *MSG_STRING[] = {
    FOREACH_MSG(GENERATE_STRING)
};
#endif



#include "nodes.h"

typedef uint16_t MsgId_t;
typedef uint16_t NodeType_t;

#pragma pack(1)
typedef struct MsgHeader_s
{
    uint16_t    SOM;      // 0x534b
    MsgId_t     msgId;    // Message identifier (uin16_t)
    NodeType_t  source;   // Source of the message (uin16_t)
    uint16_t    length;   // Length of body (not header)
    uint32_t    sec;      // Unix seconds
    uint32_t    nsec;     // Nano seconds into second
} MsgHeader_t;
#pragma pack(0)


// Login message. The type you want to login as comes from the header source
#pragma pack(1)
typedef struct BodyLogin_s
{
    // No body
} BodyLogin_t;
#pragma pack(0)

// Logout message. Header only
#pragma pack(1)
typedef struct BodyLogout_s
{
    // No body
} BodyLogout_t;
#pragma pack(0)

#pragma pack(1)
typedef struct BodyRegister_s
{
    MsgId_t  msgId;  // ID of message to register
    uint16_t add;    // non zero to register, 0 to unregister
} BodyRegister_t;
#pragma pack(0)

#pragma pack(1)
typedef struct BodyFrame_s
{
    uint32_t frame;    // Frame number to display
    uint32_t sec;      // Whole seconds since 1970
    uint32_t nsec;     // nano seconds into second
} BodyFrame_t;
#pragma pack(0)

#pragma pack(1)
typedef struct BodyLog_s
{
    uint32_t sec;     // Epoch seconds of event
    uint32_t nsec;    // nanosecond into second of event
    uint8_t  level;   // Class of event (INFO, DEBUG, ...)
    char     string[1000]; // String of event
} BodyLog_t;
#pragma pack(0)

#pragma pack(1)
typedef struct BodyConfig_s
{
    char     videoDir[256]; // Directory where video files can be found
    char     videoRoot[24]; // Root file name of video files
} BodyConfig_t;
#pragma pack(0)

#pragma pack(1)
typedef struct BodyLocation_s
{
    double   northing;   // Northing::D
    double   easting;    // Easting::D
    double   altitude;   // Altitude::D
    double   speed;      // Speed::D
    double   heading;    // Heading::D
} BodyLocation_t;
#pragma pack(0)

#pragma pack(1)
typedef struct BodyTime_s
{
    uint32_t  sec;    // Basic simulation time (epoch seconds)
    uint32_t  nsec;   // Basic simulation time (nonos into above)
} BodyTime_t;
#pragma pack(0)

typedef enum {
    PLAYBACK_STOP   = 0,
    PLAYBACK_PLAY   = 1,
    PLAYBACK_REWIND = 2,
    PLAYBACK_SINGLE = 3,
    PLAYBACK_SPEED  = 4,
    PLAYBACK_LOAD   = 5,
} PlaybackCmd_e;


#pragma pack(1)
typedef struct BodyPlayback_s
{
    PlaybackCmd_e  cmd;      // Playback command (Enumeration)
    double         ratio;    // Ration to realtime
    char           fn[256];  // filename of file to playback
} BodyPlayback_t;
#pragma pack(0)


#pragma pack(1)
typedef struct BodyGeneric_s
{
    uint8_t donotuse[1024];  // Max size of a body
} BodyGeneric_t;
#pragma pack(0)


// Union of all the message bodies
#pragma pack(1)
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
    BodyPlayback_t playback;
    BodyGeneric_t  generic;
} Body_t;
#pragma pack(0)

// A generic message is a header and a body
#pragma pack(1)
typedef struct Msg_s
{
    MsgHeader_t hdr;
    Body_t      body;
} Msg_t;
#pragma pack(0)


#ifdef USE_MSG_SIZES
static const uint16_t MSG_SIZES[] = {
    FOREACH_MSG(GENERATE_SIZE)
};
#endif

#endif
