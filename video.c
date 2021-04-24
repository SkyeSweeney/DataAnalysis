
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>


#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

void cbTime(Msg_t *pMsg);
void cbVideoConfig(Msg_t *pMsg);
static void *displayThread(void *pargs);

static pthread_mutex_t m_displayMutex;
static pthread_t       m_displayThread;
static sem_t           m_displaySem;
static VideoSync_t     m_videoSync;

static uint32_t        m_sec;
static uint32_t        m_nsec;
static uint32_t        m_frame;

int main(int argc, char *argv[])
{

    pthread_mutex_init(&m_displayMutex, NULL);
    sem_init(&m_displaySem, 0, 0);

    m_videoSync.frame = 0;
    m_videoSync.sec   = 0;
    m_videoSync.nsec  = 0;

    hubif_client_init();
    hubif_login(NODE_VIDEO);

    hubif_register(MSGID_TIME,         cbTime);
    hubif_register(MSGID_VIDEO_CONFIG, cbVideoConfig);

    // Start image display thread
    pthread_create(&m_displayThread, NULL, displayThread, NULL);
    

    for (;;)
    {
        sleep(1);
    }

}

// Call back for getting a message to display a frame
void cbTime(Msg_t *pMsg)
{
    int err;

    // Try to take the mutex
    err = pthread_mutex_trylock(&m_displayMutex);

    // Able to take mutux
    if (err == 0)
    {
        // Put message data into memory
        m_sec   =  pMsg->hdr.sec;
        m_nsec  =  pMsg->hdr.nsec;

        // Signal thread with semaphore
        sem_post(&m_displaySem);
    }

    printf("Got a Time: sec:%u nsec:%u\n",
           pMsg->hdr.sec,
           pMsg->hdr.nsec);
}

// Call back for getting a message to display a frame
void cbVideoConfig(Msg_t *pMsg)
{
    // Put message data into memory
    m_videoSync = pMsg->body.videoConfig.videoSync;

    printf("Got a VideoConfig\n");
}


// Thread to process image requests from queue
static void *displayThread(void *pargs)
{
    for (;;)
    {
        // Wait for receiver to notify us we hae a new frame to display
        sem_wait(&m_displaySem);

        // If frame is cached
        if (0)
        {
            // Get image from cache
        } else {
            // Get image form file
            // Add it to cache
        }

        // Display the image
        // Display the frame and time
        //
        if (m_videoSync.sec == 0)
        {
            printf("VideoConfig not received yet\n");
            m_frame = 0;
        }
        else
        {
            int32_t dt;
            // Difference in time in usec
            // TODO Fix up for signed values
            dt = (m_sec - m_videoSync.sec)*1000 + 
                 (m_nsec - m_videoSync.nsec)/1000000;
            m_frame = dt / (1.0/30.0 * 1000);  
        }

        printf("Displaying image %d\n", m_frame);

        // Release mutux to allow the display of next image
        pthread_mutex_unlock(&m_displayMutex);
    }
    return NULL;
}

