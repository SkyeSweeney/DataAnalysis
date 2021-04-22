
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>


#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

void cbPlayback(Msg_t *pMsg);
static void *playbackThread(void *pargs);

pthread_mutex_t m_displayMutex;
pthread_t       m_playbackThread;
sem_t           m_cmdSem;

uint32_t       m_steps = 0;
char           m_fn[256] = "";
FILE          *m_fp = NULL;
double         m_ratio = 1.0;
BodyPlayback_t m_playbackCmd;



int main(int argc, char *argv[])
{

    pthread_mutex_init(&m_displayMutex, NULL);
    sem_init(&m_cmdSem, 0, 0);

    hubif_client_init();
    hubif_login(NODE_PLAYBACK);

    hubif_register(MSGID_PLAYBACK, cbPlayback);

    pthread_create(&m_playbackThread, NULL, playbackThread, NULL);
    
    Msg_t msg;

    for (int i=0; ; i++)
    {
        sleep(1);
        msg.hdr.SOM = 0x534B;
        msg.hdr.msgId = MSGID_FRAME;
        msg.hdr.source = NODE_PLAYBACK;
        msg.hdr.length = sizeof(BodyFrame_t);
        msg.hdr.sec = 0;
        msg.hdr.nsec = 0;
        msg.body.frame.frame = i;
        msg.body.frame.sec   = i;
        msg.body.frame.nsec  = i;
        hubif_send(&msg);

    }

}


// Callback for a playback command message
void cbPlayback(Msg_t *pMsg)
{

    // Copy off command
    m_playbackCmd = pMsg->body.playback;

    // Signal we got a new command
    sem_post(&m_cmdSem);

}



// Thread to playback file
static void *playbackThread(void *pargs)
{
    int err;
    PlaybackCmd_e cmd;

    for (;;)
    {

        // Try to take command semaphore
        err = sem_trywait(&m_cmdSem);

        // If a new command is available
        if (err == 0)
        {

            cmd = m_playbackCmd.cmd;

            switch(cmd)
            {
                case PLAYBACK_STOP:
                    m_steps = 0;
                    break;

                case PLAYBACK_PLAY:
                    m_steps = 0xffffffff;
                    break;

                case PLAYBACK_REWIND:
                    m_steps = 0;
                    if (m_fp != NULL)
                    {
                        fseek(m_fp, 0, SEEK_SET);
                    }
                    else
                    {
                        printf("File not open to rewind\n");
                    }
                    break;

                case PLAYBACK_SINGLE:
                    m_steps = 1;
                    break;

                case PLAYBACK_SPEED:
                    m_ratio = m_playbackCmd.ratio;
                    break;

                case PLAYBACK_LOAD:
                    m_steps = 0;
                    if (m_fp != NULL)
                    {
                        fclose(m_fp);
                    }
                    strncpy(m_fn, m_playbackCmd.fn, sizeof(m_fn));
                    m_fp = fopen(m_fn, "rb");
                    if (m_fp == NULL)
                    {
                        printf("Unable to open file <%s>\n", m_fn);
                    }
                    break;

                default:
                    printf("Received invalid playback command <%d>\n", cmd);
                    break;
            }

        }

        // if we are to step the data
        else if (m_steps != 0)
        {
            // Get next record
            // Determine if it is time to send it
            // send it if time
        }

        // In halted mode
        else
        {
            sleep(1);
        }

    }
    return NULL;
}

