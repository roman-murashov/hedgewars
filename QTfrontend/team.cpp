/*
 * Hedgewars, a free turn based strategy game
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

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QSettings>
#include <QStandardItemModel>
#include <QDebug>

#include "team.h"
#include "hwform.h"
#include "DataManager.h"

HWTeam::HWTeam(const QString & teamname, QObject *parent) :
    QObject(parent)
{
    flib_team team;
    bzero(&team, sizeof(team));
    team.name = teamname.toUtf8().data();
    team.grave = "Statue";
    team.fort = "Plane";
    team.voicepack = "Default";
    team.flag = "hedgewars";

    for (int i = 0; i < HEDGEHOGS_PER_TEAM; i++)
    {
        team.hogs[i].name = QLineEdit::tr("hedgehog %1").arg(i+1).toUtf8().data();
        team.hogs[i].hat = "NoHat";
    }

    m_oldTeamName = teamname;

    QVector<flib_binding> binds(BINDS_NUMBER);
    for(int i = 0; i < BINDS_NUMBER; i++)
    {
        binds[i].action = cbinds[i].action.toUtf8().data();
        binds[i].binding = cbinds[i].strbind.toUtf8().data();
    }
    team.bindings = binds.data();
    team.bindingCount = binds.size();

    team.remoteDriven = false;
    team.hogsInGame = 4;

    m_team = flib_team_copy(&team);
}

HWTeam::HWTeam(const QStringList& strLst, QObject *parent) :
    QObject(parent)
{
    // net teams are configured from QStringList
    if(strLst.size() != 23) throw HWTeamConstructException();
    flib_team team;
    bzero(&team, sizeof(team));
    team.name = strLst[0].toUtf8().data();
    m_oldTeamName = strLst[0];
    team.grave = strLst[1].toUtf8().data();
    team.fort = strLst[2].toUtf8().data();
    team.voicepack = strLst[3].toUtf8().data();
    team.flag = strLst[4].toUtf8().data();
    team.ownerName = strLst[5].toUtf8().data();
    int difficulty = strLst[6].toUInt();

    for (int i = 0; i < HEDGEHOGS_PER_TEAM; i++)
    {
        team.hogs[i].name = strLst[i * 2 + 7].toUtf8().data();

        QString hat = strLst[i * 2 + 8];
        if (hat.isEmpty())
            team.hogs[i].hat = "NoHat";
        else
            team.hogs[i].hat = hat.toUtf8().data();

        team.hogs[i].difficulty = difficulty;
    }

    m_oldTeamName = strLst[0];

    QVector<flib_binding> binds(BINDS_NUMBER);
    for(int i = 0; i < BINDS_NUMBER; i++)
    {
        binds[i].action = cbinds[i].action.toUtf8().data();
        binds[i].binding = cbinds[i].strbind.toUtf8().data();
    }
    team.bindings = binds.data();
    team.bindingCount = binds.size();

    team.remoteDriven = true;
    team.hogsInGame = 4;

    m_team = flib_team_copy(&team);
}


HWTeam::HWTeam(const HWTeam & other) :
    QObject(other.parent())
    , m_oldTeamName(other.m_oldTeamName)
    , m_team(flib_team_copy(other.m_team))
{

}

HWTeam & HWTeam::operator = (const HWTeam & other)
{
    if(this != &other)
    {
        m_oldTeamName = other.m_oldTeamName;
        m_team = flib_team_copy(other.m_team);
    }

    return *this;
}

HWTeam::~HWTeam()
{
    if(m_team)
        flib_team_destroy(m_team);
}

bool HWTeam::loadFromFile()
{
    QString name = QString::fromUtf8(m_team->name);

    if(m_team)
        flib_team_destroy(m_team);

    m_team = flib_team_from_ini(QString("/config/Teams/%1.hwt").arg(name).toUtf8().data());

    return m_team != NULL;
}

bool HWTeam::fileExists()
{
    QFile f(QString("physfs://config/Teams/%1.hwt").arg(name()));
    return f.exists();
}

bool HWTeam::deleteFile()
{
    if(m_team->remoteDriven)
        return false;

    QFile cfgfile(QString("physfs://config/Teams/%1.hwt").arg(name()));
    cfgfile.remove();
    return true;
}

bool HWTeam::saveToFile()
{
    if (m_oldTeamName != name())
    {
        QFile cfgfile(QString("physfs://config/Teams/%1.hwt").arg(m_oldTeamName));
        cfgfile.remove();
        m_oldTeamName = name();
    }

    return flib_team_to_ini(QString("physfs://config/Teams/%1.hwt").arg(name()).toUtf8(), m_team) == 0;
}


bool HWTeam::isNetTeam() const
{
    return m_team->remoteDriven;
}


bool HWTeam::operator==(const HWTeam& t1) const
{
    return qstrcmp(m_team->name, t1.m_team->name) == 0;
}

bool HWTeam::operator<(const HWTeam& t1) const
{
    return qstrcmp(m_team->name, t1.m_team->name) < 0; // if names are equal - test if it is net team
}


//// Methods for member inspection+modification ////


// name
QString HWTeam::name() const
{
    return QString::fromUtf8(m_team->name);
}

void HWTeam::setName(const QString & name)
{
    free(m_team->name);

    m_team->name = qstrdup(name.toUtf8().constData());
}

QString HWTeam::hedgehogName(int index) const
{
    return QString::fromUtf8(m_team->hogs[index].name);
}

QString HWTeam::hedgehogHat(int index) const
{
    return QString::fromUtf8(m_team->hogs[index].hat);
}

void HWTeam::setHedgehogName(int index, const QString & name)
{
    free(m_team->hogs[index].name);

    m_team->hogs[index].name = qstrdup(name.toUtf8().constData());
}

void HWTeam::setHedgehogHat(int index, const QString & hat)
{
    free(m_team->hogs[index].hat);

    m_team->hogs[index].hat = qstrdup(hat.toUtf8().constData());
}


// owner
QString HWTeam::owner() const
{
    return QString::fromUtf8(m_team->ownerName);
}



// difficulty
unsigned int HWTeam::difficulty() const
{
    return m_team->hogs[0].difficulty;
}

void HWTeam::setDifficulty(unsigned int level)
{
    for(int i = 0; i < HEDGEHOGS_PER_TEAM; ++i)
        m_team->hogs[i].difficulty = level;
}

// color
int HWTeam::color() const
{
    return m_team->colorIndex;
}

QColor HWTeam::qcolor() const
{
    return DataManager::instance().colorsModel()->item(m_team->colorIndex)->data().value<QColor>();
}

void HWTeam::setColor(int color)
{
    m_team->colorIndex = color % DataManager::instance().colorsModel()->rowCount();
}


// binds
QString HWTeam::keyBind(unsigned int idx) const
{
    return QString::fromUtf8(m_team->bindings[idx].binding);
}

void HWTeam::bindKey(unsigned int idx, const QString & key)
{
    free(m_team->bindings[idx].binding);

    m_team->bindings[idx].binding = qstrdup(key.toUtf8().constData());
}

// flag
void HWTeam::setFlag(const QString & flag)
{
    free(m_team->flag);

    m_team->flag = strdup(flag.toUtf8().constData());
}

QString HWTeam::flag() const
{
    return QString::fromUtf8(m_team->flag);
}

// fort
void HWTeam::setFort(const QString & fort)
{
    free(m_team->fort);

    m_team->fort = strdup(fort.toUtf8().constData());
}

QString HWTeam::fort() const
{
    return QString::fromUtf8(m_team->fort);
}

// grave
void HWTeam::setGrave(const QString & grave)
{
    free(m_team->grave);

    m_team->grave = strdup(grave.toUtf8().constData());
}

QString HWTeam::grave() const
{
    return QString::fromUtf8(m_team->grave);
}

// voicepack - getter/setter
void HWTeam::setVoicepack(const QString & voicepack)
{
    free(m_team->voicepack);

    m_team->voicepack = strdup(voicepack.toUtf8().constData());
}

QString HWTeam::voicepack() const
{
    return QString::fromUtf8(m_team->voicepack);
}


// campaignProgress - getter
unsigned int HWTeam::campaignProgress() const
{
    return m_team->campaignProgress;
}

// amount of hedgehogs
unsigned char HWTeam::numHedgehogs() const
{
    return m_team->hogsInGame;
}

void HWTeam::setNumHedgehogs(unsigned char num)
{
    m_team->hogsInGame = num;
}

// rounds+wins - incrementors
void HWTeam::incRounds()
{
    m_team->rounds++;
}
void HWTeam::incWins()
{
    m_team->wins++;
}
