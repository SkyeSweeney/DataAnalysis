

#ifndef __MSGS_H__
#define __MSGS_H__

#include <stdint.h>
#include "macro.h"


#define FOREACH_MSG(OP) \
    OP(MSGID_UNK,              0) \
    OP(MSGID_LOGIN,            sizeof(BodyLogin_t)) \
    OP(MSGID_LOGOUT,           sizeof(BodyLogout_t)) \
    OP(MSGID_EXIT,             0) \
    OP(MSGID_REGISTER,         sizeof(BodyRegister_t)) \
    OP(MSGID_LOG,              sizeof(BodyLog_t)) \
    OP(MSGID_VIDEO_CONFIG,     sizeof(BodyVideoConfig_t)) \
    OP(MSGID_LOCATION,         sizeof(BodyLocation_t)) \
    OP(MSGID_TIME,             sizeof(BodyTime_t)) \
    OP(MSGID_PLAYBACK,         sizeof(BodyPlayback_t)) \
    OP(MSGID_TABLE,            sizeof(BodyTable_t)) \
    OP(MSGID_PING,             0) \
    OP(MSGID_MAX,              0)


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

//**********************************************************************
// Definition of the common header
//**********************************************************************
#define MSG_SOM 0x534b
#pragma pack(1)
typedef struct MsgHeader_s
{
    uint16_t    SOM;      // Start of Message 0x534b
    MsgId_t     msgId;    // Message identifier (uin16_t)
    NodeType_t  source;   // Source of the message (uin16_t)
    uint16_t    length;   // Length of body (not header)
    uint32_t    sec;      // Simulation time in Unix seconds 
    uint32_t    nsec;     // Simulation time (nano seconds into second)
} MsgHeader_t;
#pragma pack(0)


//**********************************************************************
// Login message. The type you want to login as comes from the header source
//**********************************************************************
#pragma pack(1)
typedef struct BodyLogin_s
{
    // No body
} BodyLogin_t;
#pragma pack(0)

//**********************************************************************
// Logout message. Header only
//**********************************************************************
#pragma pack(1)
typedef struct BodyLogout_s
{
    // No body
} BodyLogout_t;
#pragma pack(0)

//**********************************************************************
// Message to register for a message with hub
//**********************************************************************
#pragma pack(1)
typedef struct BodyRegister_s
{
    MsgId_t  msgId;  // ID of message to register
    uint16_t add;    // non zero to register, 0 to unregister
} BodyRegister_t;
#pragma pack(0)


typedef enum
{
    LOG_CRI,
    LOG_ERR,
    LOG_WRN,
    LOG_NTC,
    LOG_DBG,
    LOG_CMD
} LogType_e;


//**********************************************************************
// Message to Log results
//**********************************************************************
#pragma pack(1)
typedef struct BodyLog_s
{
    uint8_t  level;        // Class of event (Use LogType_e)
    char     string[1000]; // String of event
} BodyLog_t;
#pragma pack(0)

//**********************************************************************
// Message to 
//**********************************************************************
#pragma pack(1)
typedef struct videoSync_s
{
    uint32_t sec;        // Sim time for a specific video frame
    uint32_t nsec;       // Sim time for a specific video frame
    uint32_t frame;      // Video frame for a specific sim time
} VideoSync_t;
#pragma pack(0)

#pragma pack(1)
typedef struct BodyConfig_s
{
    char        videoDir[256]; // Directory where video files can be found
    char        videoRoot[24]; // Root file name of video files
    VideoSync_t videoSync;     // Sim time for a specific video frame
} BodyVideoConfig_t;
#pragma pack(0)

//**********************************************************************
// Message to 
//**********************************************************************
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

//**********************************************************************
// Message to transfer simulation time.
// Leverages the time in the header
//**********************************************************************
#pragma pack(1)
typedef struct BodyTime_s
{
    // Uses the time in the header
} BodyTime_t;
#pragma pack(0)

typedef enum {
    PLAYBACK_STOP   = 0,  // Stop playback in progress
    PLAYBACK_PLAY   = 1,  // Start playing the loaded file from curr location
    PLAYBACK_REWIND = 2,  // Rewind the loaded file
    PLAYBACK_FWD    = 3,  // Single step the loaded file n times. dflt=1
    PLAYBACK_REV    = 4,  // Single step the loaded file n times. dflt=1
    PLAYBACK_SPEED  = 5,  // Set playback speed
    PLAYBACK_LOAD   = 6,  // Load a new file
    PLAYBACK_STAT   = 7,  // Report status (file, location, ...)
    PLAYBACK_GOTO   = 8,  // Goto record n
    PLAYBACK_TABLE  = 9,  // Table commands
} PlaybackCmd_e;


#pragma pack(1)
typedef struct BodyPlayback_s
{
    PlaybackCmd_e  cmd;      // Playback command (Enumeration)
    double         ratio;    // Ratio to realtime
    uint32_t       arg;      // Various uses
    char           fn[256];  // filename of file to playback
} BodyPlayback_t;
#pragma pack(0)

typedef enum {
    TABLE_CLEAR   = 0,  // Clear all columns
    TABLE_DELETE  = 1,  // Delete on column
    TABLE_ASSIGN  = 2,  // Assign a variable to a column
} TableCmd_e;

typedef enum {
    VARTYPE_DOUBLE   = 0,
    VARTYPE_FLOAT    = 1,
    VARTYPE_INT8     = 2,
    VARTYPE_UINT8    = 3,
    VARTYPE_INT16    = 4,
    VARTYPE_UINT16   = 5,
    VARTYPE_INT32    = 6,
    VARTYPE_UINT32   = 7,
    VARTYPE_INT64    = 8,
    VARTYPE_UINT64   = 9,
    VARTYPE_STRING   = 10
} VarType_e;

typedef struct VarTable_Entry_s
{
    uint16_t       col;           // Column to operate on
    MsgId_t        msgId;         // Message identifier (uin16_t)
    VarType_e      varType;       // Type of variable (float, int, ..)
    uint8_t        varValidBits;  // Number of LSB bits that are valid
    uint8_t        varByteOffset; // Offset in bytes from start of body
    uint8_t        varBitOffset;  // Number of bits to shift right
    char           name[256];     // Variable name
    char           format[16];    // Format
} VarTableEntry_t;


#pragma pack(1)
typedef struct BodyTable_s
{
    TableCmd_e      cmd;           // Playback command (Enumeration)
    VarTableEntry_t entry;    // A structure of all the things needed to place a number 
} BodyTable_t;
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
    BodyLogin_t       login;
    BodyLogout_t      logout;
    BodyRegister_t    reg;
    BodyLog_t         log;
    BodyVideoConfig_t videoConfig;
    BodyLocation_t    location;
    BodyPlayback_t    playback;
    BodyTable_t       table;
    BodyGeneric_t     generic;
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
