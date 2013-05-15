/*!
 * @file qmheartbeat_p.h
 * @brief Contains QmHeartbeat which provides system heartbeat services.

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

   @scope Private

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
#ifndef QMHEARTBEAT_P_H
#define QMHEARTBEAT_P_H

#include "qmheartbeat.h"
extern "C" {
#include <iphbd/libiphb.h>
}
#include <QSocketNotifier>
#include <QTime>

namespace MeeGo
{

    class QmHeartbeatPrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmHeartbeat)

    public:
        QmHeartbeatPrivate();
        ~QmHeartbeatPrivate();
        bool open(QmHeartbeat::SignalNeed signalNeed);
        void close();

        iphb_t iphbdHandler;
        QmHeartbeat::SignalNeed signalNeed;
        QSocketNotifier *notifier;
        QTime wait_time;

    Q_SIGNALS:
        void wakeUp(QTime);

    private Q_SLOTS:
        void socketReady(int sock);
    };

}
#endif // QMHEARTBEAT_P_H
