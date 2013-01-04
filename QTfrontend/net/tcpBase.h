/*
 * Hedgewars, a free turn based strategy game
 * Copyright (c) 2006-2007 Igor Ulyanov <iulyanov@gmail.com>
 * Copyright (c) 2004-2012 Andrey Korotaev <unC0Rr@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef _TCPBASE_INCLUDED
#define _TCPBASE_INCLUDED

#include <QObject>
#include <QProcess>

class TCPBase : public QObject
{
        Q_OBJECT

    public:
        TCPBase(QObject * parent = 0);
        virtual ~TCPBase();

        virtual bool couldBeRemoved();

    signals:
        void nextPlease();

    protected:
        bool m_hasStarted;
        int m_port;

        void start(bool couldCancelPreviousRequest);

        virtual QStringList getArguments() = 0;
        virtual void onEngineStart() = 0;

        void clientDisconnected();

    private:
        void RealStart();

    private slots:
        void iStart();
};

#ifdef HWLIBRARY
class EngineInstance : public QObject
{
    Q_OBJECT
public:
    EngineInstance(QObject *parent = 0);
    ~EngineInstance();

    int port;
public slots:
    void start(void);
signals:
    void finished(void);
private:
};
#endif

#endif // _TCPBASE_INCLUDED
