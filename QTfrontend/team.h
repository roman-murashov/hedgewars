/*
 * Hedgewars, a free turn based strategy game
 * Copyright (c) 2004-2012 Andrey Korotaev <unC0Rr@gmail.com>
 * Copyright (c) 2007 Igor Ulyanov <iulyanov@gmail.com>
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

#ifndef TEAM_H
#define TEAM_H

#include <QColor>
#include <QString>
#include "binds.h"
#include "achievements.h"
#include "hwconsts.h"
#include "frontlib.h"

class HWForm;
class GameUIConfig;

class HWTeamConstructException
{
};

// class representing a team
class HWTeam : public QObject
{
        Q_OBJECT

    public:

        // constructors
        HWTeam(const QString & teamname = QString(), QObject * parent = 0);
        HWTeam(const QStringList& strLst, QObject * parent = 0);
        HWTeam(const HWTeam & other);
        ~HWTeam();

        // file operations
        static HWTeam loadFromFile(const QString & teamName);
        bool loadFromFile();
        bool deleteFile();
        bool saveToFile();
        bool fileExists();

        // attribute getters
        unsigned int campaignProgress() const;
        int color() const;
        QColor qcolor() const;
        unsigned int difficulty() const;
        QString flag() const;
        QString fort() const;
        QString grave() const;
        //const HWHog & hedgehog(unsigned int idx) const;
        bool isNetTeam() const;
        QString keyBind(unsigned int idx) const;
        QString name() const;
        unsigned char numHedgehogs() const;
        QString owner() const;
        QString voicepack() const;

        // attribute setters
        void bindKey(unsigned int idx, const QString & key);
        void setDifficulty(unsigned int level);
        void setFlag(const QString & flag);
        void setFort(const QString & fort);
        void setGrave(const QString & grave);
        void setName(const QString & name);
        void setNumHedgehogs(unsigned char num);
        void setVoicepack(const QString & voicepack);

        QString hedgehogName(int index) const;
        QString hedgehogHat(int index) const;
        void setHedgehogName(int index, const QString & name);
        void setHedgehogHat(int index, const QString & hat);

        // increments for statistical info
        void incRounds();
        void incWins();

        // comparison operators
        bool operator == (const HWTeam& t1) const;
        bool operator < (const HWTeam& t1) const;
        HWTeam & operator = (const HWTeam & other);

public slots:
        void setColor(int color);

    private:
        QString m_oldTeamName;

        // class members that contain the general team info and settings
        flib_team * m_team;
};

#endif
