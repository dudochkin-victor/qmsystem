/*!
 * @file qmtap.cpp
 * @brief QmTap

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

   This file is part of SystemSW QtAPI.

   SystemSW QtAPI is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   SystemSW QtAPI is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with SystemSW QtAPI.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */
#include "qmtap.h"
#include "qmtap_p.h"

#include <QDebug>

namespace MeeGo {


    QmTap::QmTap(QObject *parent) : QmSensor(parent)
    {
        QmTapPrivate *priv = new QmTapPrivate(this);
        connect(priv, SIGNAL(tapped(MeeGo::QmTapReading)), this, SIGNAL(tapped(MeeGo::QmTapReading)));
        priv_ptr = priv;

    }
    
    QmTap::~QmTap()
    {
    }

}
