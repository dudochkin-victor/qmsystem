/*!
 * @file qmwatchdog.cpp
 * @brief QmProcessWatchdog

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

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
#include "qmwatchdog.h"
#include "qmwatchdog_p.h"

namespace MeeGo {


QmProcessWatchdog::QmProcessWatchdog(QObject *parent) : QObject(parent)
{
    MEEGO_INITIALIZE(QmProcessWatchdog);
    connect(priv, SIGNAL(ping()), this, SIGNAL(ping()));
}

QmProcessWatchdog::~QmProcessWatchdog()
{
    stop();
    MEEGO_UNINITIALIZE(QmProcessWatchdog);
}

bool QmProcessWatchdog::start()
{
    MEEGO_PRIVATE(QmProcessWatchdog);
    return priv->start();

}

bool QmProcessWatchdog::stop()
{
    MEEGO_PRIVATE(QmProcessWatchdog);
    return priv->stop();
}

bool QmProcessWatchdog::pong()
{
    MEEGO_PRIVATE(QmProcessWatchdog);
    return priv->pong();
}

} // Namespace MeeGo
