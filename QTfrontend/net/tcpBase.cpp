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

#include "tcpBase.h"

#include <QMessageBox>
#include <QList>
#include <QApplication>
#include <QImage>
#include <QThread>

#include "hwconsts.h"

#ifdef HWLIBRARY
extern "C" void Game(char**arguments);

//NOTE: most likely subclassing QThread is wrong
class EngineThread : public QThread
{
protected:
    void run();
};

void EngineThread::run()
{
    char *args[12];
    args[0] = "1";      //cShowFPS
    args[1] = "65000";  //ipcPort
    args[2] = "1024";   //cScreenWidth
    args[3] = "768";    //cScreenHeight
    args[4] = "0";      //cReducedQuality
    args[5] = "en.txt"; //cLocaleFName
    args[6] = "koda";   //UserNick
    args[7] = "1";      //SetSound
    args[8] = "1";      //SetMusic
    args[9] = "0";      //cAltDamage
    args[10]= "../Resources/hedgewars/Data";   //cPathPrefix
    args[11]= NULL;     //recordFileName
    Game(args);
}
#endif

QList<TCPBase*> srvsList;

TCPBase::~TCPBase()
{
    // make sure this object is not in the server list anymore
    srvsList.removeOne(this);
}

TCPBase::TCPBase(QObject *parent) :
    QObject(parent),
    m_hasStarted(false)
{

}

void TCPBase::RealStart()
{
    onEngineStart();

#ifdef HWLIBRARY
    EngineThread engineThread;// = new EngineThread(this);
    engineThread.start();
#else
    QProcess * process;

    process = new QProcess();
    QStringList arguments = getArguments();

    // redirect everything written on stdout/stderr
    if(isDevBuild)
        process->setProcessChannelMode(QProcess::ForwardedChannels);

    process->start(bindir->absolutePath() + "/hwengine", arguments);
#endif
    m_hasStarted = true;
}

void TCPBase::clientDisconnected()
{
    emit nextPlease();

    deleteLater();
}

void TCPBase::iStart()
{
    disconnect(srvsList.first(), SIGNAL(nextPlease()), this, SLOT(iStart()));

    RealStart();
}

void TCPBase::start(bool couldCancelPreviousRequest)
{
    if(srvsList.isEmpty())
    {
        srvsList.push_back(this);

        RealStart();
    }
    else
    {
        TCPBase * last = srvsList.last();
        if(couldCancelPreviousRequest
            && last->couldBeRemoved()
            && (last->parent() == parent()))
        {
            srvsList.removeLast();
            last->deleteLater();
            start(couldCancelPreviousRequest);
        } else
        {
            connect(srvsList.last(), SIGNAL(nextPlease()), this, SLOT(iStart()));
            srvsList.push_back(this);
        }
    }
}

bool TCPBase::couldBeRemoved()
{
    return false;
}
