#include "Channel.h"
#include <stdlib.h>

Channel::Channel(int fd, int events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg)
{
    m_arg = arg;
    m_fd = fd;
    m_events = events;
    readCallback = readFunc;
    writeCallback = writeFunc;
    destroyCallback = destroyFunc;
}

void Channel::writeEventEnable(bool flag)
{
    if (flag)
    {
        //m_events |= (int)FDEvent::WriteEvent;
        m_events |= static_cast<int>(FDEvent::WriteEvent);
    }
    else
    {
        m_events = m_events & ~static_cast<int>(FDEvent::WriteEvent);
    }
}

bool Channel::isWriteEventEnable()
{
    return m_events & static_cast<int>(FDEvent::WriteEvent);
}
