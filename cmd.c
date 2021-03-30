
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>


#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

static void cbLog(Msg_t *pMsg);
static void processUserCmd(char *pCmdBuf);
static void CmdSplitString(char *str);

static int run = 1;
static char *tokens[20];
static uint16_t nToks;

int main(int argc, char *argv[])
{
    char *pCmdBuf;
    char userPrompt[] = "> ";
    int  n;

    hubif_client_init();
    hubif_login(NODE_CMD);

    hubif_register(MSGID_LOG, cbLog);

    while (run)
    {

        (void)write(0, userPrompt, sizeof(userPrompt));

        pCmdBuf = readline(NULL); // This allocated memory
        if ((pCmdBuf != NULL) && (pCmdBuf[0] != 0))
        {
            add_history(pCmdBuf);
        }

        // If a blank line
        if (pCmdBuf[0] == '\n')
        {
            // Free the buffer allocated by getline
            free(pCmdBuf);
            continue;
        }

        // Delete newline at end of line
        n = strlen(pCmdBuf);
        if (n > 1)
        {
            if (pCmdBuf[n-1] == '\n')
            {
                pCmdBuf[n-1] = 0;
            }
        }

        n = strlen(pCmdBuf);
        if (n > 1)
        {
            // Process user command
            processUserCmd(pCmdBuf);
        }

        // Free the buffer allocated by getline
        free(pCmdBuf);

    }

    hubif_logout(NODE_CMD);

    return 0;

}

static void processUserCmd(char *pCmdBuf)
{
    char *pCmd;
    Msg_t msg;

    // Split line into tokens
    CmdSplitString(pCmdBuf);

    // Discard blank lines
    if (nToks == 0) return;

    // Get first token
    pCmd = tokens[0];

    // switch on first token
    if (strcmp(pCmd,"frame") == 0)
    {
        printf("Frame command\n");
        msg.hdr.SOM = 0x534B;
        msg.hdr.msgId = MSGID_FRAME;
        msg.hdr.source = NODE_CMD;
        msg.hdr.length = sizeof(BodyFrame_t);
        msg.body.frame.frame = atoi(tokens[1]);
        msg.body.frame.sec   = atoi(tokens[2]);
        msg.body.frame.nsec  = atoi(tokens[3]);
        hubif_send(&msg);
    }
    else if (strcmp(pCmd,"exit") == 0)
    {
        run = 0;
    }
    else if (strcmp(pCmd,"stop") == 0)
    {
        run = 0;
    }
    else
    {
        printf("Unknown command\n");
    }
    
}

static void cbLog(Msg_t *pMsg)
{
    printf("%i:%u <%d> %s\n",
           pMsg->body.log.sec,
           pMsg->body.log.nsec,
           pMsg->body.log.level,
           pMsg->body.log.string);
}


//**********************************************************************
//! @brief Convert a string into a CliArgs structure
//! @param[in] str     - Commans tring to split up
//! @return void
//**********************************************************************
static void CmdSplitString(char *str)
{
    char *saveptr = NULL;
    char *pStr;
    char *p;
    int  i;

    // Determine the number of tokens in string
    pStr = str;
    for (i=0; ;i++)
    {
        // Tokeninze to get pointers
        p = strtok_r(pStr, " ", &saveptr);
        pStr = NULL;

        // If no token, skip counting
        if (p == NULL) 
        {
            break;
        }

        // If a comment, skip counting
        if (strcmp(p, "//") == 0) 
        {
            break;
        }
        tokens[i] = p;
    }

    // Save number of tokens
    nToks = i;

    return;

}



#if 0
//----------------------------------------------------------------------
//        UNCLASSIFIED
//----------------------------------------------------------------------
//! @file
//! @copyright Copyright 2017, 2021 BAE SYSTEMS. All rights reserved.
//! @copyright Restrictions on the use, release, redistribution, 
//! @copyright display or disclosure of this file are contained in
//! @copyright the file restrictions.txt.
//----------------------------------------------------------------------
//
// PURPOSE:        
//
//! @file
//! @brief  Command dispatcher
//  
// Takes in a user string command and calls the proper command processor
// 
//
// TEST APPROACH: 
//
//! @file
//! @test Features tested using target hardware and Test Equipment
//!       Computer to perform end-to-end verification.
//
//
// NOTES:
//
//! @file
//! @note  None
//
//----------------------------------------------------------------------

//**********************************************************************
// Include files
//**********************************************************************
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sched.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


#include "Cmd.h"
#include "CmdHdr.h"
#include "CmdHdlc.h"
#include "CmdMode.h"
#include "CmdNet.h"
#include "CmdFram.h"
#include "CmdBypass.h"
#include "CmdControl.h"
#include "CmdVideo.h"
#include "CmdOverlay.h"
#include "CmdDrs.h"
#include "CmdPath.h"
#include "CmdBit.h"
#include "log.h"
#include "se.h"
#include "Reset.h"
#include "SeTime.h"
#include "Stack.h"
#include "Path.h"
#include "Str.h"
#include "Video.h"


//**********************************************************************
// Constants
//**********************************************************************

//**********************************************************************
// Local Structures, Unions, typedefs
//**********************************************************************


//**********************************************************************
// Local Prototypes
//**********************************************************************

static void CmdUsleep (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdSleep  (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdHelp   (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdVersion (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdLs     (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdCd     (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdPwd    (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdXyzzy  (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);
static void CmdReset  (LogObj_t   *pLogObj, 
                       CmdStack_t *stack,
                       uint16_t    level, 
                       CliArgs_t  *cliArgs, 
                       char       *parseTok);


static
void CmdProcessSingle(LogObj_t   *pLogObj, 
                      CmdStack_t *stack, 
                      char       *cmdStr);
//**********************************************************************
// Global Data
//**********************************************************************

//**********************************************************************
// Local Data
//**********************************************************************

//! Table of all commands supported
static TblEntry_t GeneralTbl[] =
{
  // Commands that have sub commands
  {"by.pass",    CmdBypass,     "ERR", "Bypass commands"},
  {"ov.erlay",   CmdOverlay,    "ERR", "Control video overlays"},
  {"vid.eo",     CmdVideo,      "ERR", "Control video overlays"},
  {"fr.am",      CmdFram,       "ERR", "FRAM commands"},
  {"hdr",        CmdHdr,        "ERR", "Header commands"},
  {"mode",       CmdMode,       "ERR", "Mode commands"},
  {"hdlc",       CmdHdlc,       "ERR", "HDLC commands"},
  {"net",        CmdNet,        "ERR", "Network commands"},
  {"drs",        CmdDrs,        "ERR", "DRS related commands"},
  {"path",       CmdPath,       "ERR", "Path related commands"},
  {"control",    CmdControl,    "ERR", "Control memory control"},
  {"bit",        CmdBit,        "ERR", "Bit commands"},

  {"usleep",     CmdUsleep,     "USLP", "Sleep for n microseconds"},
  {"sleep",      CmdSleep,      "SSLP", "Sleep for n seconds"},
  {"xyzzy",      CmdXyzzy,      "XYZZ", "Test routine"},
  {"reset",      CmdReset,      "RST",  "Reset FPGA registers"},

  {"r.un",       CmdRun,        "RUN",  "Execute a script file."},
  {"ls",         CmdLs,         "LS",   "List files in current directory"},
  {"cd",         CmdCd,         "CD",   "Change directories to given path"},
  {"pwd",        CmdPwd,        "PWD",  "Print the working directory"},

  {"h.elp",      CmdHelp,       "HELP", "Print this message"},
  {"?",          CmdHelp,       "HELP", "Print this message"},

  {"ve.rsion",   CmdVersion,    "VER",  "Prints software version"},

  {"q.uit",      CmdExit,       "QUIT", "Stop program"},
  {"e.xit",      CmdExit,       "EXIT", "Stop program"},
  {"x",          CmdExit,       "QUIT", "Stop program"}
};


//**********************************************************************
// Functions
//**********************************************************************

//**********************************************************************
//! @brief Compares the users command to a possible match
//!
//! Commands are provided in abc.def format. Command must match at least
//! the part up to the decimal point. If any more is given it must match 
//! exactly the remainder.
//!
//! @param[in] userStr  - String with a possible command
//! @param[in] cmd      - Command to match to.
//! @return void
//**********************************************************************
bool CommandMatches(const char *userStr, const char *cmd)
{
    char *p;
    bool retval;
    uint32_t ia, ib, is;


    // Find location of posible decimal point
    p = index(cmd, '.');

    // Do we have a decimal point in command?
    if (p != NULL)
    {
        // Determine characters before and after decimal
        ib = p - cmd;
        ia = strlen(p) - 1;

        // If we were given at least the minimum number
        if (strlen(userStr) >= ib)
        {
            // If the minimum matches
            if (strncasecmp(userStr, cmd, ib) == 0)
            {

                // Determine the number of extra characters in user string
                is = strlen(userStr)-ib;
                
                // If more than the pattern, can't match.
                if (is > ia)
                {
                    retval = false;
                }

                // If user string is shorter or equal to the pattern, 
                // then they must all match.
                else 
                {
                    if (strncasecmp(&userStr[ib], &p[1], is) == 0)
                    {
                        retval = true;
                    }
                    else
                    {
                        retval = false;
                    }
                }

            }
            else
            {
                retval = false;
            }
        }

        // Less than minimum
        else
        {
            retval = false;
        }
    }

    // No decimal. Must match exactly.
    else
    {
        
        retval = (strcasecmp(userStr, cmd) == 0);
    }

    return retval;
}


//**********************************************************************
//! @brief Process a single command
//! This routine is called from the Network or Command line interface
//! threads. It passes in the context of the command stack and the 
//! string provided by the user.
//!
//! @param[in] pLogObj  - Logger object
//! @param[in] stack   - Command source stack
//! @param[in] cmdStr  - Command string
//! @return void
//**********************************************************************
static
void CmdProcessSingle(LogObj_t   *pLogObj, 
                      CmdStack_t *stack, 
                      char       *cmdStr)
{
    int       s;
    char     *cmd;
    size_t    n;
    CliArgs_t cliArgs;


    // Strip newlines from end of line
    n = strlen(cmdStr);
    if (n > 0)
    {
        if (cmdStr[n-1] == '\n')
        {
            cmdStr[n-1] = 0;
        }
    }

    // Skip blank lines
    n = strlen(cmdStr);
    if (n == 0) return;

    // Split the command into parts for easier processing
    CmdSplitString(cmdStr, &cliArgs);

    // Process only non blank lines
    if (cliArgs.numToks != 0) 
    {
    
        // Get a shortcut to the first token
        cmd = cliArgs.toks[0];

        LogCmd(cliArgs.origStr);

        // If a comment
        if (cmd[0] == '#') 
        {
            // Echo
            LogStringL(pLogObj, cliArgs.origStr);

        }

        // If a possible command
        else
        {

            // Get size of table
            s = sizeof(GeneralTbl)/sizeof(TblEntry_t);

            // Process the command with GeneralTbl
            CmdProcessTable(pLogObj, 
                            stack,
                            GeneralTbl,
                            s,
                            0,
                            &cliArgs);
        }

    }

    // Release the string memory
    CmdReleaseString(&cliArgs);

    return;

} // end CmdProcessSingle


//**********************************************************************
//! @brief Process a single command
//! This routine is called from the Network or Command line interface
//! threads. It passes in the context of the command stack and the 
//! string provided by the user.
//!
//! The command passed in must have been allocated using malloc
//! It will be freed by the caller on return
//!
//! @param[in] pLogObj  - Logger object
//! @param[in] stack    - Command source stack
//! @param[in] cmdStr   - Command string allocated with malloc
//! @return void
//**********************************************************************
void CmdProcess(LogObj_t   *pLogObj, 
                CmdStack_t *stack, 
                char       *cmdStr)
{
    int16_t  pos;
    char    *pCmdBuf;
    size_t   nCmd;
    size_t   c;
    FILE    *fpSrc;
    int      err;

    // Process the initial command
    CmdProcessSingle(pLogObj, stack, cmdStr);

    // Continue to process commands if we are in a script
    for (;;)
    {

        // Determine the source of the next command
        pos = CmdStackPos(stack);

        // Break if we are running from the top level (user input)
        if (pos == 0) break;

        // Get the source of our next command
        fpSrc = CmdStackSource(stack);

        // Get the next command from file
        // getline allocates memory
        pCmdBuf = NULL;
        nCmd    = 0;
        c = getline(&pCmdBuf, &nCmd, fpSrc);

        // If we are at the end of the file
        if (c == -1)
        {
            // Pop stack
            err = CmdStackPop(pLogObj, stack);

            // Handle the error
            if (err != 0)
            {
                printf("Stack pop error\n");
                exit(1);
            }

            // Read the next line
            continue;
        }

        // Process the command
        CmdProcessSingle(pLogObj, stack, pCmdBuf);

        free(pCmdBuf);

    }
    return;
}


//**********************************************************************
//! @brief Process a command given a table
//! @param[in] pLogObj  - Log object pointer
//! @param[in] stack   - Command source stack.
//! @param[in] tbl     - Command jump table
//! @param[in] szTbl   - Size of command jump table
//! @param[in] level   - Nexting level of CmdProcessTable
//! @param[in] cliArgs - Pointer to command argument structure
//! @return void
//**********************************************************************
void CmdProcessTable(LogObj_t   *pLogObj, 
                     CmdStack_t *stack,
                     TblEntry_t *tbl, 
                     uint16_t    szTbl,
                     uint16_t    level,
                     CliArgs_t  *cliArgs)
{
    int  i;
    char logBuf[128];


    // Do for each entry in table
    for (i=0; i<szTbl; i++)
    {

        if (CommandMatches(cliArgs->toks[level], tbl[i].cmd))
        {

            // Pass arguments to function
            tbl[i].fx(pLogObj, stack, level, cliArgs, tbl[i].parseTok);
            break;
        }
    }

    
    // Command not found.
    if (i == szTbl)
    {
        (void)snprintf(STRN(logBuf), 
                       "ERR: Command <%s> not found", 
                       cliArgs->origStr);
        LogStringL(pLogObj, logBuf);
    }
}


//**********************************************************************
//! @brief Print usage for a command table
//! @param[in] pLogObj  - Log object pointer
//! @param[in] tbl     - Command jump table
//! @param[in] szTbl   - Size of command jump table
//! @return void
//**********************************************************************
void CmdUsage(LogObj_t   *pLogObj, 
              TblEntry_t *tbl, 
              uint16_t    szTbl)
{
    int  i;
    char buf[120];

    // Do for each entry in table
    for (i=0; i<szTbl; i++)
    {
        (void)snprintf(STRN(buf), "  %s", tbl[i].help);
        LogStringF(pLogObj, buf);
    }

    LogStringL(pLogObj, "");

}
    





//**********************************************************************
//! @brief Convert a string into a CliArgs structure
//! @param[in] str     - Commans tring to split up
//! @param[in] cliArgs - Pointer to a CliArgs_t structure to populate
//! @return void
//**********************************************************************
void CmdSplitString(char *str, CliArgs_t *cliArgs)
{
    char *saveptr = NULL;
    char *pCopy;
    char *pStr;
    char *p;
    char **pBlock;
    int  i;

    // Put a copy of original string into structure
    cliArgs->origStr = str;

    // Make a copy of the string
    pCopy = strdup(str);
    if (pCopy == NULL)
    {
        (void)printf("strdup fails\n");
        exit(1);
    }
    cliArgs->copyStr = pCopy;

    // Determine the number of tokens in string
    pStr = pCopy;
    for (i=0; ;i++)
    {
        // Tokeninze to get pointers
        p = strtok_r(pStr, " ", &saveptr);
        pStr = NULL;

        // If no token, skip counting
        if (p == NULL) 
        {
            break;
        }

        // If a comment, skip counting
        if (strcmp(p, "//") == 0) 
        {
            break;
        }
    }

    // Save number of tokens
    cliArgs->numToks = i;

    // Restore the string
    (void)strncpy(pCopy, str, strlen(str));

    // Malloc a block to hold pointers
    // coverity[misra_violation]
    pBlock = (char **)calloc(cliArgs->numToks, sizeof(char*));

    // Save this pointer
    cliArgs->toks = pBlock;

    pStr = pCopy;
    saveptr = NULL;

    for (i=0; i<cliArgs->numToks; i++)
    {

        // Tokeninze to get pointers
        p = strtok_r(pStr, " ", &saveptr);
        pStr = NULL;

        // Should not get here
        if (p == NULL) 
        {
            break;
        }

        pBlock[i] = p;
    }

    return;

}


//**********************************************************************
//! @brief Release memory help by the CliArgs_t structure
//! @param[in] cliArgs  - Pointer to CliArgs_t structure
//! @return void
//**********************************************************************
void CmdReleaseString(CliArgs_t *cliArgs)
{
    // First release the memory that holds the list of tokens
    //(void)printf("Free %p\n", cliArgs->toks);
    free(cliArgs->toks);

    // Then release the copy of the string
    //(void)printf("Free %p\n", cliArgs->copyStr);
    free(cliArgs->copyStr);
}


/** 
 *@addtogroup GlobalCommandGrp
 *@{
 * __RUN filename__ <br>
 * Runs a file as a script. Script files may call other script files up to 
 * 20 levels deep. When one script ends, it picks back up in at the line after
 * the run command in the calling script file.<br>
 * + Where:
 *   - filename - Name of filename to run as script. Program will try to find it in the path.

 *@}
 */

//**********************************************************************
//! @brief Command processor to support the "run" command
//! @param[in] pLogObj  - Logger object
//! @param[in] stack   - Command source stack.
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
void CmdRun(LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    char     *fileName;
    bool      found;
    char      newFileName[MAX_FQFN];
    char      buf[MAX_FQFN];

    // Run command must have 2 tokens (run file)
    if (cliArgs->numToks != 2)
    {
        LogStringL(pLogObj, "RUNx: Invalid number of arguments");
    }
    else
    {
        // Shortcut to second argument
        fileName = cliArgs->toks[1];

        // See if file exists in path someplace
        found = PathFindFile(fileName, 
                             newFileName, 
                             sizeof(newFileName));

        // If the script file was found
        if (found)
        {

            // Push the filename onto the stack
            CmdStackPush(pLogObj, stack, newFileName);

        } else {

            (void)snprintf(STRN(buf), 
                           "RUNx: Unable to find file <%s>", fileName);
            LogStringL(pLogObj, buf);
        }

    }
}


/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __Q.UIT__ <br>
 * __E.XIT__ <br>
 * __X__ <br>
 * Quits the program
 *@}
 */

//**********************************************************************
//! @brief Command processor to support the "exit" command
//! @param[in] pLogObj  - Logger object
//! @param[in] stack   - Command source stack.
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
void CmdExit(LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    VideoMode_e mode;
    char        buf[80];

    // Get what mode the video playback is in
    mode = VideoGetMode();

    // If we are playing video, stop it first
    if (mode == VM_PLAYING)
    {
        snprintf(STRN(buf), "%s: Stopping video, please wait.", parseTok);
        LogStringF(pLogObj, buf);
        VideoStop(pLogObj, parseTok);
    }
    
    MasterStop = 1;

} // end CmdExit


/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __VERSION__ <br>
 * Prints version information
 * + Parse Token: VER
 *@}
 */
//**********************************************************************
//! @brief Command processor to support the "version" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdVersion (LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    uint32_t fpga1 = 0;
    uint32_t fpga2 = 0;
    uint32_t hash[5];
    char     buf[132];
    uint32_t addr;

    snprintf(STRN(buf), "%s:", parseTok);
    LogStringF(pLogObj, buf);

    // Software
    (void)snprintf(STRN(buf), "  SW Git: %s", GIT_VERSION);
    LogStringF(pLogObj, buf);
    (void)snprintf(STRN(buf), "  SW Date: %s", DATE_VERSION);
    LogStringF(pLogObj, buf);

    // FPGA
    addr = offsetof(BypassMemoryLayout_t, DecompRegs.Version1);
    (void)FpgaBypassPeek(addr, &fpga1);
    addr = offsetof(BypassMemoryLayout_t, DecompRegs.Version2);
    (void)FpgaBypassPeek(addr, &fpga2);
    addr = offsetof(BypassMemoryLayout_t, DecompRegs.GitHash1);
    (void)FpgaBypassPeek(addr, &hash[4]);
    addr = offsetof(BypassMemoryLayout_t, DecompRegs.GitHash2);
    (void)FpgaBypassPeek(addr, &hash[3]);
    addr = offsetof(BypassMemoryLayout_t, DecompRegs.GitHash3);
    (void)FpgaBypassPeek(addr, &hash[2]);
    addr = offsetof(BypassMemoryLayout_t, DecompRegs.GitHash4);
    (void)FpgaBypassPeek(addr, &hash[1]);
    addr = offsetof(BypassMemoryLayout_t, DecompRegs.GitHash5);
    (void)FpgaBypassPeek(addr, &hash[0]);

    (void)snprintf(STRN(buf), 
             "  FPGA: (20%02x/%02x/%02x %02x:%02x:%02x-%02x.%02x)", 
             (fpga1>> 8)&0xff,  // year
             (fpga1>> 0)&0xff,  // Month
             (fpga2>>24)&0xff,  // day
             (fpga2>>16)&0xff,  // hour
             (fpga2>> 8)&0xff,  // minute
             (fpga2>> 0)&0xff,  // second
             (fpga1>>24)&0xff,  // Major
             (fpga1>>16)&0xff); // Minor
    LogStringF(pLogObj, buf);
    (void)snprintf(STRN(buf), 
             "  FPGA hash: %08x%08x%08x%08x%08x", 
             hash[0], hash[1], hash[2], hash[3], hash[4]);
    LogStringF(pLogObj, buf);

    LogStringL(pLogObj, "");

} // end CmdVersion



/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __USLEEP usec__ <br>
 * Sleeps the command processing for the given number of micro seconds<br>
 * + Where:
 *    - usec - Number of microseconds to sleep
 * + Parse Token: USLP
 *@}
 */

//**********************************************************************
//! @brief Command processor to support the "usleep" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdUsleep (LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    uint32_t usec;
    char     buf[80];

    
    if (cliArgs->numToks != 2)
    {
        snprintf(STRN(buf), "%sx: Invalid number of commands", parseTok);
        LogStringL(pLogObj, buf);
    }
    else
    {
        // Sleep
        usec = StrToUint32(cliArgs->toks[1]);
        usleep(usec);

        snprintf(STRN(buf), "%s: Sleep done", parseTok);
        LogStringL(pLogObj, buf);
    }

} // end CmdUsleep


/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __SLEEP sec__ <br>
 * Sleeps the command processing for the given number of seconds<br>
 * + Where:
 *    - sec - Number of seconds to sleep
 * + Parse Token: SSLP
 *@}
 */

//**********************************************************************
//! @brief Command processor to support the "sleep" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdSleep(LogObj_t *pLogObj, 
                     CmdStack_t *stack,
                     uint16_t level, 
                     CliArgs_t *cliArgs, 
                     char *parseTok)
{
    uint32_t sec;
    char     buf[80];

    
    if (cliArgs->numToks != 2)
    {
        snprintf(STRN(buf), "%sx: Invalid number of commands", parseTok);
        LogStringL(pLogObj, buf);
    }
    else
    {
        // Sleep
        sec = StrToUint32(cliArgs->toks[1]);
        sleep(sec);

        snprintf(STRN(buf), "%s: Sleep done", parseTok);
        LogStringL(pLogObj, buf);
    }

} // end CmdSleep



/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __LS__ <br>
 * List files in current directory
 * + Parse Token: LS
 *@}
 */
//**********************************************************************
//! @brief Command processor to support the "ls" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdLs (LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    // List files
    DIR           *d;
    struct dirent *dir;
    char           cwd[MAX_FQFN];
    char           buf[MAX_FQFN];
    
    // Print current directory
    getcwd(cwd, sizeof(cwd));
    
    if (cliArgs->numToks == 1)
    {

        d = opendir(".");
    
        if (d != NULL)
        {

            (void)snprintf(STRN(buf), "%s: %s", parseTok, cwd);
            LogStringF(pLogObj, buf);

            while ((dir = readdir(d)) != NULL)
            {
               LogStringF(pLogObj, dir->d_name);
            }
            LogStringL(pLogObj, "");

            closedir(d);
        } 
        else 
        {
            snprintf(STRN(buf), "%sx: Unable to perform ls command", parseTok);
            LogStringF(pLogObj, buf);
        }

    }
    else if (cliArgs->numToks == 2)
    {

        d = opendir(cliArgs->toks[1]);
    
        if (d)
        {

            (void)snprintf(STRN(buf), "%s: %s", cliArgs->toks[1], parseTok);
            LogStringF(pLogObj, buf);

            while ((dir = readdir(d)) != NULL)
            {
               LogStringF(pLogObj, dir->d_name);
            }
            LogStringL(pLogObj, "");

            closedir(d);
        }
        else
        {
            snprintf(STRN(buf), 
                     "%sx: Unable to perform ls command", 
                     parseTok);
            LogStringF(pLogObj, buf);
        }
    }
    else
    {
        snprintf(STRN(buf), "%sx: Invalid number of commands", parseTok);
        LogStringL(pLogObj, buf);
    }

} // end CmdLs

/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __PWD__ <br>
 * Print current directory
 * + Parse Token: PWD
 *@}
 */
//**********************************************************************
//! @brief Command processor to support the "pwd" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdPwd (LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    char cwd[MAX_FQFN];
    char buf[MAX_FQFN];
    
    if (cliArgs->numToks != 1)
    {
        snprintf(STRN(buf), "%sx: Invalid number of commands", parseTok);
        LogStringL(pLogObj, buf);
    }
    else
    {
        // Get current directory
        getcwd(cwd, sizeof(cwd));

        // Print current directory
        (void)snprintf(STRN(buf), "%s: %s", parseTok, cwd);
        LogStringL(pLogObj, buf);
    }

} // end CmdPwd

/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __CD dir__ <br>
 * Change to a new current directory
 * + Parse Token: CD
 *@}
 */
//**********************************************************************
//! @brief Command processor to support the "cd" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdCd (LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    int err;
    char buf[MAX_FQFN];
    
    if (cliArgs->numToks != 2)
    {
        snprintf(STRN(buf), "%sx: Invalid number of commands", parseTok);
        LogStringL(pLogObj, buf);
    }
    else
    {
        // Change current directory
        err = chdir(cliArgs->toks[1]);
        if (err == -1)
        {
            snprintf(STRN(buf), "%sx: Failed", parseTok);
            LogStringL(pLogObj, buf);
        }
        else
        {
            (void)snprintf(STRN(buf), "%s: %s", parseTok, cliArgs->toks[1]);
            LogStringL(pLogObj, buf);
        }
    }

} // end CmdCd

/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __HELP__ <br>
 * __Question Mark (?)__ <br>
 * Prints a help message
 * + Parse Token: HELP
 *@}
 */

//**********************************************************************
//! @brief Command processor to support the "help" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdHelp (LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{

    int i;
    int n;
    char buf[120];


    n = sizeof(GeneralTbl)/sizeof(TblEntry_t);

    snprintf(STRN(buf), "%s:", parseTok);
    LogStringF(pLogObj, buf);
    for (i=0; i<n; i++)
    {
        (void)snprintf(STRN(buf), "  %s - %s", GeneralTbl[i].cmd, GeneralTbl[i].help);
        LogStringF(pLogObj, buf);
    }
    LogStringL(pLogObj, "");

} // end CmdHelp

/** 
 *@addtogroup GlobalCommandGrp
 *@{
 *___
 * __XYZZY__ <br>
 * Spare test command for debugging
 * + Parse Token: XYZZ
 *@}
 */

//**********************************************************************
//! @brief Command processor to support the "xyzzy" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdXyzzy(LogObj_t *pLogObj, 
                     CmdStack_t *stack,
                     uint16_t level, 
                     CliArgs_t *cliArgs, 
                     char *parseTok)
{

    char         buf[80];
    SystemTime_t st;

    GetSystemTime(&st);

    (void)snprintf(STRN(buf), "%s: %08x %08x", parseTok, st.msb, st.lsb);
    LogStringL(pLogObj, buf);

} // end CmdXyzzy

//**********************************************************************
//! @brief Command processor to support the "Reset" command
//! @param[in] pLogObj  - Logger object
//! @param[in] level   - Nesting level.
//! @param[in] cliArgs - Pointer to a cliArgs structure that holds split cmd
//! @param[in] parseTok - Pointer to the reply parser token
//! @return void
//**********************************************************************
static void CmdReset (LogObj_t   *pLogObj, 
            CmdStack_t *stack,
            uint16_t    level, 
            CliArgs_t  *cliArgs, 
            char       *parseTok)
{
    Reset(pLogObj);
} // end CmdReset

#endif
