/*
 * Hedgewars, a free turn based strategy game
 * Copyright (c) 2006-2007 Ulyanov Igor <iulyanov@gmail.com>
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

#include "hwconsts.h"
#include "hwmap.h"
#include "frontlibpoller.h"

HWMap::HWMap(QObject * parent) :
    TCPBase(parent)
{
    m_conn = NULL;
    m_map = NULL;
}

HWMap::~HWMap()
{
    if(m_conn)
        flib_mapconn_destroy(m_conn);
    if(m_map)
        flib_map_destroy(m_map);
}

bool HWMap::couldBeRemoved()
{
    return !m_hasStarted;
}

void HWMap::getImage(const QString & seed, int filter, int mapgen, int maze_size, const QByteArray & drawMapData)
{
    switch(mapgen)
    {
        case MAPGEN_REGULAR: m_map =
                flib_map_create_regular(
                    seed.toUtf8().constData()
                    , ""
                    , filter);
                break;
        case MAPGEN_MAZE: m_map =
                flib_map_create_maze(
                    seed.toUtf8().constData()
                    , ""
                    , maze_size);
                break;
        case MAPGEN_DRAWN: m_map =
                flib_map_create_drawn(
                    seed.toUtf8().constData()
                    , ""
                    , (const uint8_t*)drawMapData.constData()
                    , drawMapData.size()
                    );
                break;
        default:
            Q_ASSERT_X(false, "HWMap::getImage", "Unknown generator");
    }
    qDebug(m_map->seed);
    start(true);
}

QStringList HWMap::getArguments()
{
    Q_ASSERT(m_conn);

    int ipc_port = flib_mapconn_getport(m_conn);

    QStringList arguments;
    arguments << "--internal";
    arguments << "--port";
    arguments << QString("%1").arg(ipc_port);
    arguments << "--user-prefix";
    arguments << cfgdir->absolutePath();
    arguments << "--landpreview";
    return arguments;
}

void HWMap::onSuccess(void *context, const uint8_t *bitmap, int numHedgehogs)
{
    HWMap * hwMap = (HWMap *)context;

    QImage im(bitmap, MAPIMAGE_WIDTH, MAPIMAGE_HEIGHT, QImage::Format_Mono);
    im.setNumColors(2);
    emit hwMap->HHLimitReceived(numHedgehogs);
    emit hwMap->ImageReceived(im);

    hwMap->clientDisconnected();
}

void HWMap::onFailure(void *context, const char *errormessage)
{
    HWMap * hwMap = (HWMap *)context;

    qWarning(errormessage);

    hwMap->clientDisconnected();
}

void HWMap::onEngineStart()
{
    m_conn = flib_mapconn_create(m_map);
    flib_mapconn_onSuccess(m_conn, onSuccess, this);
    flib_mapconn_onFailure(m_conn, onFailure, this);

    new FrontLibPoller((void (*)(void *))flib_mapconn_tick, m_conn, this);
}
