/*
 * Hedgewars, a free turn based strategy game
 * Copyright (c) 2008 Andrey Korotaev <unC0Rr@gmail.com>
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

#ifndef _HATS_INCLUDED
#define _HATS_INCLUDED

#include <QAbstractTableModel>
#include <QStringList>

class HatsModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	HatsModel(QObject *parent = 0);

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int rowCount(const QModelIndex & parent) const;
	int columnCount(const QModelIndex & parent) const;

//protected:
//	QList<QStringList> games;
};

#endif // _HATS_INCLUDED
