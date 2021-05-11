
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>


#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

static void cbPlayback(Msg_t *pMsg);
static void *playbackThread(void *pargs);
static void processCmd(void);

pthread_mutex_t m_displayMutex;
pthread_t       m_playbackThread;
sem_t           m_cmdSem;

uint32_t       m_steps = 0;
char           m_fn[256] = "";
FILE          *m_fin = NULL;
double         m_ratio = 1.0;
BodyPlayback_t m_playbackCmd;
HubIf         *m_pHubIf;



//**********************************************************************
//
//**********************************************************************
int main(int argc, char *argv[])
{

    pthread_mutex_init(&m_displayMutex, NULL);
    sem_init(&m_cmdSem, 0, 0);

    m_pHubIf = new HubIf();

    m_pHubIf->client_init();
    m_pHubIf->login(NODE_PLAYBACK);

    m_pHubIf->registerCb(MSGID_PLAYBACK, cbPlayback);

    pthread_create(&m_playbackThread, NULL, playbackThread, NULL);
    

    for (;;)
    {
        sleep(1);
    }
    
#if 0
    Msg_t msg;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000*1000*1000/30;

    uint32_t sec = 0;
    uint32_t nsec = 0;

    for (int i=0; ; i++)
    {
        usleep(33333);
        m_pHubIf->sendMsg(&msg, MSGID_TIME, sec, nsec);

        nsec += 1000*1000*1000/30;
        if (nsec > 1000000000)
        {
            nsec -= 1000000000;
            sec += 1;
        }
    }
#endif

}


//**********************************************************************
//
//**********************************************************************
// Callback for a playback command message
void cbPlayback(Msg_t *pMsg)
{

    // Copy off command
    m_playbackCmd = pMsg->body.playback;

    // Signal we got a new command
    sem_post(&m_cmdSem);

}



//**********************************************************************
//
//**********************************************************************
// Thread to playback file
static void *playbackThread(void *pargs)
{
    int err;
    Msg_t msg;
    size_t n;

    // Do forever
    for (;;)
    {

        // If we have more steps to make
        if (m_steps > 0)
        {

            // To ease error handling
            for (;;)
            {

                // Read the next header into the message buffer
                n = fread((void*)(&msg.hdr), sizeof(MsgHeader_t), 1, m_fin);

                // Did not read what we wanted
                if (n == 0)
                {
                    if (feof(m_fin) == 0)
                    {
                        printf("Error reading header.\n");
                        m_steps = 0;
                    }
                    break;
                }

                // Verify header
                if (msg.hdr.SOM != 0x534b)
                {
                    // Flush buffer and hope for the best
                    printf("Invalid SOM.\n");
                    m_steps = 0;
                    break;
                }

                // If we have a body
                if (msg.hdr.length > 0)
                {

                    // Read body
                    n = fread((void*)(&msg.body), msg.hdr.length, 1, m_fin);

                    if (n == 0)
                    {
                        printf("Error reading body.\n");
                        m_steps = 0;
                        break;
                    }

                } // Have body

                usleep(33333);

                // Now we send this mesage to hub
                m_pHubIf->sendMsg(&msg, 
                                  (MsgId_e)msg.hdr.msgId, 
                                  msg.hdr.sec, 
                                  msg.hdr.nsec);

                break;

            }

            // Check for a new command
            err = sem_trywait(&m_cmdSem);

            // If a new command is available
            if (err == 0)
            {
                // Process it
                processCmd();

            }
        }

        // In halted mode
        else
        {
            // Wait here till we get a new user command
            err = sem_wait(&m_cmdSem);

            // Process it
            processCmd();
        }

    } // Do infinity

    return NULL;
}


//**********************************************************************
//
//**********************************************************************
void processCmd(void)
{

    PlaybackCmd_e cmd;

    cmd = m_playbackCmd.cmd;

    switch(cmd)
    {
        case PLAYBACK_STOP:
            printf("STOP\n");
            m_steps = 0;
            break;

        case PLAYBACK_PLAY:
            printf("PLAY\n");
            m_steps = 0xffffffff;
            break;

        case PLAYBACK_REWIND:
            printf("REWIND\n");
            m_steps = 0;
            if (m_fin != NULL)
            {
                fseek(m_fin, 0, SEEK_SET);
            }
            else
            {
                printf("File not open to rewind\n");
            }
            break;

        case PLAYBACK_FWD:
            printf("FWD\n");
            m_steps = m_playbackCmd.arg;
            break;

        case PLAYBACK_REV:
            printf("REV Not implemented\n");
            break;

        case PLAYBACK_SPEED:
            printf("SPEED\n");
            m_ratio = m_playbackCmd.ratio;
            break;

        case PLAYBACK_LOAD:
            printf("LOAD\n");
            m_steps = 0;

            // Close file if open
            if (m_fin != NULL)
            {
                fclose(m_fin);
            }

            // Save the given file
            strncpy(m_fn, m_playbackCmd.fn, sizeof(m_fn));

            // Open the file
            m_fin = fopen(m_fn, "rb");

            // If the open failes
            if (m_fin == NULL)
            {
                printf("Unable to open file <%s>\n", m_fn);
                m_fn[0] = 0;
            }
            break;

        case PLAYBACK_STAT:
            printf("STAT Not implemented\n");
            // Compose message and send
            break;

        case PLAYBACK_GOTO:
            printf("GOTO Not implemented\n");
            break;

        default:
            printf("Received invalid playback command <%d>\n", cmd);
            break;
    }

}