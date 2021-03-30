
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>


#include "hub_if.h"
#include "nodes.h"
#include "msgs.h"

void cbFrame(Msg_t *pMsg);
static void *displayThread(void *pargs);

pthread_mutex_t m_displayMutex;
pthread_t       m_displayThread;
sem_t           m_displaySem;

int main(int argc, char *argv[])
{

    pthread_mutex_init(&m_displayMutex, NULL);
    sem_init(&m_displaySem, 0, 0);

    hubif_client_init();
    hubif_login(NODE_VIDEO);

    hubif_register(MSGID_FRAME, cbFrame);

    // Start image display thread
    pthread_create(&m_displayThread, NULL, displayThread, NULL);
    

    for (;;)
    {
        sleep(1);
    }

}

int m_frame;
int m_sec;
int m_nsec;

// Call back for getting a message to display a frame
void cbFrame(Msg_t *pMsg)
{
    int err;

    // Try to take the mutex
    err = pthread_mutex_trylock(&m_displayMutex);

    // Able to take mutux
    if (err == 0)
    {
        // Put message data into memory
        m_frame =  pMsg->body.frame.frame;
        m_sec   =  pMsg->body.frame.sec;
        m_nsec  =  pMsg->body.frame.nsec;

        // Signal thread with semaphore
        sem_post(&m_displaySem);
    }

    printf("Got a Frame: %u sec:%u nsec:%u\n",
           pMsg->body.frame.frame,
           pMsg->body.frame.sec,
           pMsg->body.frame.nsec);
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
        printf("Displaying image %d\n", m_frame);

        // Release mutux to allow the display of next image
        pthread_mutex_unlock(&m_displayMutex);
    }
    return NULL;
}

