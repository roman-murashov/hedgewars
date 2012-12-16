#ifndef FRONTLIBPOLLER_H
#define FRONTLIBPOLLER_H

#include <QObject>
#include <QBasicTimer>

class FrontLibPoller : public QObject
{
    Q_OBJECT
public:
    explicit FrontLibPoller(void (*poll)(void *), void * data, QObject *parent = 0);

private:
    QBasicTimer m_timer;

    void (*m_poll)(void *);
    void * m_data;

    void timerEvent(QTimerEvent *event);
};

#endif // FRONTLIBPOLLER_H
