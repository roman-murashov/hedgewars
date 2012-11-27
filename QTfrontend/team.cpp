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
    QList<QByteArray> baList;

    flib_team team;
    memset(&team, 0, sizeof(team));
    baList << teamname.toUtf8();
    team.name = baList.last().data();
    team.grave = const_cast<char *>("Statue");
    team.fort = const_cast<char *>("Plane");
    team.voicepack = const_cast<char *>("Default");
    team.flag = const_cast<char *>("hedgewars");

    for (int i = 0; i < HEDGEHOGS_PER_TEAM; i++)
    {
        baList << QLineEdit::tr("hedgehog %1").arg(i+1).toUtf8();
        team.hogs[i].name = baList.last().data();
        team.hogs[i].hat = const_cast<char *>("NoHat");
    }

    m_oldTeamName = teamname;

    QVector<flib_binding> binds(BINDS_NUMBER);
    for(int i = 0; i < BINDS_NUMBER; i++)
    {
        baList << cbinds[i].action.toUtf8();
        binds[i].action = baList.last().data();
        baList << cbinds[i].strbind.toUtf8();
        binds[i].binding = baList.last().data();
    }
    team.bindings = binds.data();
    team.bindingCount = binds.size();

    m_team = flib_team_copy(&team);
}

HWTeam::HWTeam(const QStringList& strLst, QObject *parent) :
    QObject(parent)
{
    QList<QByteArray> baList;

    // net teams are configured from QStringList
    if(strLst.size() != 23) throw HWTeamConstructException();
    flib_team team;
    memset(&team, 0, sizeof(team));

    for(int i = 0; i < 6; ++i)
        baList << strLst[i].toUtf8();
    team.name = baList[0].data();
    m_oldTeamName = strLst[0];
    team.grave = baList[1].data();
    team.fort = baList[2].data();
    team.voicepack = baList[3].data();
    team.flag = baList[4].data();
    team.ownerName = baList[5].data();
    int difficulty = strLst[6].toUInt();

    for (int i = 0; i < HEDGEHOGS_PER_TEAM; i++)
    {
        baList << strLst[i * 2 + 7].toUtf8();
        team.hogs[i].name = baList.last().data();

        QString hat = strLst[i * 2 + 8];
        if (hat.isEmpty())
            team.hogs[i].hat = const_cast<char *>("NoHat");
        else
        {
            baList << hat.toUtf8();
            team.hogs[i].hat = baList.last().data();
        }

        team.hogs[i].difficulty = difficulty;
    }

    m_oldTeamName = strLst[0];

    QVector<flib_binding> binds(BINDS_NUMBER);
    for(int i = 0; i < BINDS_NUMBER; i++)
    {
        baList << cbinds[i].action.toUtf8();
        binds[i].action = baList.last().data();
        baList << cbinds[i].strbind.toUtf8();
        binds[i].binding = baList.last().data();
    }
    team.bindings = binds.data();
    team.bindingCount = binds.size();

    m_team = flib_team_copy(&team);
}


HWTeam::HWTeam(const HWTeam & other) :
    QObject(other.parent())
    , m_oldTeamName(other.m_oldTeamName)
    , m_team(flib_team_copy(other.m_team))
{
    m_team->hogsInGame = other.m_team->hogsInGame;
    m_team->remoteDriven = other.m_team->remoteDriven;
}

HWTeam & HWTeam::operator = (const HWTeam & other)
{
    if(this != &other)
    {
        m_oldTeamName = other.m_oldTeamName;
        m_team = flib_team_copy(other.m_team);

        m_team->hogsInGame = other.m_team->hogsInGame;
        m_team->remoteDriven = other.m_team->remoteDriven;
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

    m_team = flib_team_from_ini(QString("/Teams/%1.hwt").arg(name).toUtf8().data());

    return m_team != NULL;
}

bool HWTeam::fileExists()
{
    QFile f(QString("physfs://Teams/%1.hwt").arg(name()));
    return f.exists();
}

bool HWTeam::deleteFile()
{
    if(m_team->remoteDriven)
        return false;

    QFile cfgfile(QString("physfs://Teams/%1.hwt").arg(name()));
    cfgfile.remove();
    return true;
}

bool HWTeam::saveToFile()
{
    if (m_oldTeamName != name())
    {
        QFile cfgfile(QString("physfs://Teams/%1.hwt").arg(m_oldTeamName));
        cfgfile.remove();
        m_oldTeamName = name();
    }

    return flib_team_to_ini(QString("physfs://Teams/%1.hwt").arg(name()).toUtf8(), m_team) == 0;
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

    m_team->name = strdup(name.toUtf8().constData());
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

    m_team->hogs[index].name = strdup(name.toUtf8().constData());
}

void HWTeam::setHedgehogHat(int index, const QString & hat)
{
    free(m_team->hogs[index].hat);

    m_team->hogs[index].hat = strdup(hat.toUtf8().constData());
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

    m_team->bindings[idx].binding = strdup(key.toUtf8().constData());
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

flib_team * HWTeam::toFlibTeam()
{
    return flib_team_copy(m_team);
}
