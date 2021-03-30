
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

int main(int argc, char *argv[])
{
    char *pCmdBuf;
    char userPrompt[] = "> ";
    int  n;

    hubif_client_init();
    hubif_login(NODE_CMD);

    hubif_register(MSGID_LOG, cbLog);

    for (;;)
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

        // Process user command
        processUserCmd(pCmdBuf);

        // Free the buffer allocated by getline
        free(pCmdBuf);

        }

}

static void processUserCmd(char *pCmdBuf)
{
    // Split
    // Discard balnk lines
    // Get first token
    // switch on first token
    
}

static void cbLog(Msg_t *pMsg)
{
    printf("%i:%u <%d> %s\n",
           pMsg->body.log.sec,
           pMsg->body.log.nsec,
           pMsg->body.log.level,
           pMsg->body.log.string);
}

