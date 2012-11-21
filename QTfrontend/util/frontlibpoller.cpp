#include <QTimerEvent>

#include "frontlibpoller.h"

FrontLibPoller::FrontLibPoller(void (*poll)(void * data), void *data, QObject *parent) :
    QObject(parent)
{
    m_poll = poll;
    m_data = data;

    m_timer.start(50, this);
}

void FrontLibPoller::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timer.timerId())
        m_poll(m_data);
    else
        QObject::timerEvent(event);
}
