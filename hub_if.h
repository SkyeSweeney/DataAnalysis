


#ifndef __hub_if_h__
#define __hub_if_h__

#define HUB_PORT 5000

#include "msgs.h"
#include "nodes.h"


int hubif_client_init(void);

int hubif_login(NodeId_t nodeId);

int hubif_register(MsgId_t msgid, void (*cb)(Msg_t *msg));

int hubif_unregister(MsgId_t msgid);

int hubif_send(Msg_t *msg);


#endif
