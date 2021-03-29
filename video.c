
#include <stdio.h>
#include <unistd.h>

#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

void cbFrame(Msg_t *pMsg);

int main(int argc, char *argv[])
{

    hubif_client_init();
    hubif_login(NODE_VIDEO);

    hubif_register(MSGID_FRAME, cbFrame);

    for (;;)
    {
        sleep(1);
    }

}

void cbFrame(Msg_t *pMsg)
{
    printf("Got a Frame\n");
}

