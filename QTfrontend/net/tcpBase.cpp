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
extern "C" void GenLandPreview(int port);


EngineInstance::EngineInstance(QObject *parent)
    : QObject(parent)
{
    port = 0;
}

EngineInstance::~EngineInstance()
{
}

void EngineInstance::start()
{
#if 0
    char *args[11];
    args[0] = "65000";  //ipcPort
    args[1] = "1024";   //cScreenWidth
    args[2] = "768";    //cScreenHeight
    args[3] = "0";      //cReducedQuality
    args[4] = "en.txt"; //cLocaleFName
    args[5] = "koda";   //UserNick
    args[6] = "1";      //SetSound
    args[7] = "1";      //SetMusic
    args[8] = "0";      //cAltDamage
    args[9]= datadir->absolutePath().toAscii().data(); //cPathPrefix
    args[10]= NULL;     //recordFileName
    Game(args);
#endif
    GenLandPreview(port);
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
    QThread *thread = new QThread;
    EngineInstance *instance = new EngineInstance;
    instance->port = IPCServer->serverPort();

    instance->moveToThread(thread);

    connect(thread, SIGNAL(started()), instance, SLOT(start(void)));
    connect(instance, SIGNAL(finished()), thread, SLOT(quit()));
    connect(instance, SIGNAL(finished()), instance, SLOT(deleteLater()));
    connect(instance, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
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
