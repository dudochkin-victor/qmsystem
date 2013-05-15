/*!
 * @file qmheartbeat.cpp
 * @brief QmHeartbeat

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

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
#include "qmheartbeat.h"
#include "qmheartbeat_p.h"
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <QDebug>
#include <QProcess>

namespace MeeGo {

#define LOG_ERROR { qWarning() << Q_FUNC_INFO << " PID=" << (unsigned long)getpid() << " " << strerror(errno) <<  " errno=" << errno; }

QmHeartbeatPrivate::QmHeartbeatPrivate() {
    //Init
    iphbdHandler = 0;
    signalNeed = QmHeartbeat::NoSignalNeeded;
    notifier = 0;
}

QmHeartbeatPrivate::~QmHeartbeatPrivate() {
    //Call iphb_close if the developer forgot to call it
    close();
}

bool QmHeartbeatPrivate::open(QmHeartbeat::SignalNeed signalNeed) {
    bool status =  false;

    iphbdHandler = iphb_open(0);
    this->signalNeed = signalNeed;

    if (iphbdHandler) {
        if (signalNeed == QmHeartbeat::SignalNeeded) {
            int sockfd  = iphb_get_fd(iphbdHandler);

            if (sockfd == -1) {
                LOG_ERROR;
            }
            else {
                notifier = new QSocketNotifier( sockfd, QSocketNotifier::Read, this);
                if (!notifier) {
                    qFatal("QSocketNotifier failed");
                }
                else {
                    QObject::connect( notifier, SIGNAL(activated(int)),
                                  this, SLOT(socketReady(int)));

                    notifier->setEnabled(false);
                    status =  true;
                }
            }
        } else {
            status = true;
        }
    }
    else
        LOG_ERROR;
    return status;

}

void QmHeartbeatPrivate::close() {
    //Avoiding ~QmHeartbeat to call iphb_close again
    if (iphbdHandler) {
        //Actually sets priv->iphbdHandler to 0
        iphbdHandler = iphb_close(iphbdHandler);
    }

    if (notifier) {
        delete notifier;
        notifier = 0;
    }

    signalNeed = QmHeartbeat::NoSignalNeeded;

}

void QmHeartbeatPrivate::socketReady(int sock) {
    Q_UNUSED(sock);

    int st = iphb_discard_wakeups(iphbdHandler);

    if (st == -1)
        LOG_ERROR;

    wait_time.elapsed();
    emit wakeUp(wait_time);
}

QmHeartbeat::QmHeartbeat(QObject *parent)
           : QObject(parent) {
    MEEGO_INITIALIZE(QmHeartbeat)
}

QmHeartbeat::~QmHeartbeat() {
    MEEGO_UNINITIALIZE(QmHeartbeat);
}

bool QmHeartbeat::open(QmHeartbeat::SignalNeed signalNeed) {
    MEEGO_PRIVATE(QmHeartbeat);
    bool retVal = priv->open(signalNeed);
    if (retVal) {
        connect(priv, SIGNAL(wakeUp(QTime)), this, SIGNAL(wakeUp(QTime)));
    }
    return retVal;
}

void QmHeartbeat::close(void) {
    MEEGO_PRIVATE(QmHeartbeat);

    priv->close();
}

int QmHeartbeat::getFD() {
    MEEGO_PRIVATE(QmHeartbeat);
    return iphb_get_fd(priv->iphbdHandler);
}

bool QmHeartbeat::IWokeUp(void) {
    MEEGO_PRIVATE(QmHeartbeat);

    if (priv->notifier)
        priv->notifier->setEnabled(false);

    int st = iphb_I_woke_up(priv->iphbdHandler);
    if (st >= 0) {
        // qDebug() << Q_FUNC_INFO << "Woke up, discared" << st << " bytes";
        return true;
    }
    else {
        LOG_ERROR;
        return false;
    }
}

QTime QmHeartbeat::wait(unsigned short mintime, unsigned short maxtime, QmHeartbeat::WaitMode wait) {
    MEEGO_PRIVATE(QmHeartbeat);

    // qDebug()) << Q_FUNC_INFO << "Wait, mintime=" << mintime << "maxtime=" << maxtime << "wait mode=" << wait;

    if (priv->notifier) {

        // qDebug()) << Q_FUNC_INFO << "setting socket notifier to" << (wait == DoNotWaitHeartbeat ? "enabled" : "disabled");

        if (wait == DoNotWaitHeartbeat)
            priv->notifier->setEnabled(true);
        else
            priv->notifier->setEnabled(false);
    }

    if (wait == DoNotWaitHeartbeat) {
        priv->wait_time.start();
    }

    //NOTE: This function could freeze the GUI if wait is not DoNotWaitHeartbeat
    time_t unixTime = iphb_wait(priv->iphbdHandler, mintime, maxtime, (int) wait);

    if (unixTime == (time_t) - 1) {
        LOG_ERROR;
    }

    QTime time = QDateTime::fromTime_t(unixTime).toUTC().time();

    // qDebug() ) << Q_FUNC_INFO << "Waited, time=" << time;

    return time;
}

}
