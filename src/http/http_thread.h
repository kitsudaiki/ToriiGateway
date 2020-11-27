#ifndef HTTP_THREAD_H
#define HTTP_THREAD_H

#include <libKitsunemimiCommon/threading/thread.h>

class HttpThread
        : public Kitsunemimi::Thread
{
public:
    HttpThread();

protected:
    void run();
};

#endif // HTTP_THREAD_H
