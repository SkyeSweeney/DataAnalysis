


#ifndef __hub_if_h__
#define __hub_if_h__

#define HUB_PORT 5000

#include "msgs.h"
#include "nodes.h"

class HubIf
{

    public:

        HubIf();

        ~HubIf();

        int client_init(void);

        int login(NodeId_t nodeId);
      
        int logout(NodeId_t nodeId);
        
        int registerCb(MsgId_t msgid, void (*cb)(Msg_t *msg));
        
        int unregisterCb(MsgId_t msgid);
        
        int sendMsg(Msg_t *msg, MsgId_e msgId, uint32_t sec, uint32_t nsec);

    private:

        int       m_sockFd;
        pthread_t m_thread_id;
        NodeId_t  m_nodeId = NODE_NONE;

        void (*m_callbacks[MSGID_MAX])(Msg_t *pMsg);

};

#endif
