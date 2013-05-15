/*!
 * @file qmwatchdog_p.h
 * @brief Contains QmWatchdogPrivate

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
#ifndef QMWATCHDOG_P_H
#define QMWATCHDOG_P_H

#include "qmwatchdog.h"
#include <QSocketNotifier>
#include <dsme/processwd.h>
#include <dsme/protocol.h>
#include <cstdlib>
#include <QDebug>

namespace MeeGo
{

    class QmProcessWatchdogPrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmProcessWatchdog);

    public:
        QmProcessWatchdogPrivate() : conn(NULL), notifier(NULL) { }

        bool start() {
            conn = dsmesock_connect();
            if (!conn) {
                qWarning() << "Could not connect to dsme daemon.";
                return false;
            }

            DSM_MSGTYPE_PROCESSWD_CREATE *msg = DSME_MSG_NEW(DSM_MSGTYPE_PROCESSWD_CREATE);
            msg->pid = getpid();
            int ret = dsmesock_send(conn, msg);
            free(msg);
            if (ret < 0) {
                dsmesock_close(conn);
                conn = NULL;
                qWarning("Could not register to the process watchdog service.");
                return false;
            }
            notifier = new QSocketNotifier(conn->fd, QSocketNotifier::Read, this);
            connect(notifier, SIGNAL(activated(int)), this, SLOT(readable(int)));
            notifier->setEnabled(true);

            return true;
        }

        bool stop() {
            if (notifier) {
                delete notifier;
                notifier = NULL;
            }
            if (conn) {
                DSM_MSGTYPE_PROCESSWD_DELETE *msg = DSME_MSG_NEW(DSM_MSGTYPE_PROCESSWD_DELETE);
                msg->pid = getpid();
                int ret = dsmesock_send(conn, msg);
                if (ret < 0) {
                    qWarning() << "Could not unregister from the process watchdog service.";
                }
                free (msg);
                dsmesock_close(conn);
                conn = NULL;
            }

            return true;

        }


        dsmesock_connection_t *conn;
        QSocketNotifier *notifier;

    Q_SIGNALS:
        void ping();

    public Q_SLOTS:
        bool pong() {
            if (conn) {
                DSM_MSGTYPE_PROCESSWD_PONG *msg = DSME_MSG_NEW(DSM_MSGTYPE_PROCESSWD_PONG);
                msg->pid = getpid();
                int ret = dsmesock_send(conn, msg);
                free (msg);
                if (ret < 0) {
                    qWarning() << "Could not send a PONG message.";
                    return false;
                } else {
                    return true;
                }
            }
            return false;

        }

        void readable(int fd) {
            if (conn && conn->fd == fd) {
                dsmemsg_generic_t *msg = (dsmemsg_generic_t*)dsmesock_receive(conn);
                if (msg && dsmemsg_id(msg) == DSME_MSG_ID_(DSM_MSGTYPE_PROCESSWD_PING)) {
                    emit ping();
                }
                free (msg);
            }
        }

    };
}
#endif // QMWATCHDOG_P_H
