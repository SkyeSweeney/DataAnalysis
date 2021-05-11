#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <pthread.h>
#include <unistd.h>

using namespace std;
using namespace std::placeholders; // for `_1`


//**********************************************************************
// Class definition
//**********************************************************************
class MessageHandler 
{
    public:
        MessageHandler();

        // Register a callback
        void registerCb(std::function<void(int)> callback);

        // This is a 'generic' function that returns void and takes an int
        std::function<void(int)> m_callback;

        // pthread_create needs a static.
        static void * messageThread(void *pargs);
    private:
        pthread_t  m_msgThread;


};

//**********************************************************************
// Constructor
//**********************************************************************
MessageHandler::MessageHandler()
{
    // Pass in 'this' to thread so it can have access to this
    // instance of the class
    // Start up thread to process incomming messages
    pthread_create(&m_msgThread, NULL, messageThread, this);
}

//**********************************************************************
// Thread to process messages from remote
//**********************************************************************
void * MessageHandler::messageThread(void *pargs)
{
    // Argument is pointer to the class
    // Needed because pthread_create needs a static method
    MessageHandler *p;
    p = (MessageHandler *)pargs;

    // Do forever
    for(;;)
    {
        // Get a message from remote
        printf("Got message\n");

        // Call the routine provided to handle messages
        p->m_callback(1);

        // Pretend to wait for next message
        sleep(1);
    }
    return NULL;
}


//**********************************************************************
// Register a function to handle messages
//**********************************************************************
void MessageHandler::registerCb(std::function<void(int)> callback) 
{
    printf("Callback Registered\n");
    m_callback = callback;
}






//**********************************************************************
// Class definition
//**********************************************************************
class MyClass
{
    public:
        MyClass(int);
        void processMsgA(int x);

    private:
        MessageHandler *pHandler;
        int           private_x;
};

//**********************************************************************
// Constructor for our app
//**********************************************************************
MyClass::MyClass(int value) 
{

    // Create the message handling subsystem
    pHandler = new MessageHandler();

    private_x = value;

    // Register a message handler 
    pHandler->registerCb(std::bind(&MyClass::processMsgA, this, _1));
}

//**********************************************************************
// A sample routine that processes a specific message (type A)
//**********************************************************************
void MyClass::processMsgA(int x) 
{
    printf("Result:%d\n", (x+private_x));
}





//**********************************************************************
// Main method
//**********************************************************************
int main(int argc, char const *argv[]) 
{

    new MyClass(5);

    sleep(5);
    return 0;
}


// where $1 is your .cpp file name... this is the command used:
// g++ -std=c++11 -Wall -o $1 $1.cpp
// chmod 700 $1
// ./$1

