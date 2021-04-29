
#include <functional>


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
        
        int registerCb(MsgId_t msgid, std::function<void(Msg_t*)>cb);
        
        int unregisterCb(MsgId_t msgid);
        
        int sendMsg(Msg_t *msg, MsgId_e msgId, uint32_t sec, uint32_t nsec);

        std::function<void(Msg_t*)> m_callbacks[MSGID_MAX];

    private:

        int       m_sockFd;
        pthread_t m_thread_id;
        NodeId_t  m_nodeId = NODE_NONE;


};

#endif
