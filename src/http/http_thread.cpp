#include "http_thread.h"

#include <libKitsunemimiCommon/threading/event.h>

HttpThread::HttpThread()
    : Kitsunemimi::Thread() {}

/**
 * @brief HttpThread::run
 */
void
HttpThread::run()
{
    while(m_abort == false)
    {
        Kitsunemimi::Event* event = getEventFromQueue();
        if(event != nullptr)
        {
            event->processEvent();
            delete event;
        }
        else
        {
            sleepThread(10000);
        }
    }
}

