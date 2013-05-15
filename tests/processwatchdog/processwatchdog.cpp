/**
 * @file processwatchdog.cpp
 * @brief QmProcessWatchdog tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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

#include <QTest>
#include <QDebug>
#include <QTime>

using namespace MeeGo;
class TestClass : public QObject
{
    Q_OBJECT;
public slots:
    void ping() {
        qDebug() << "ping: " << QTime::currentTime();
        pingCount++;
        if (pongOnGivenPing == 0) {
            QVERIFY(watchdog->pong());
            qDebug() << "pong: " << QTime::currentTime();
        } else if (pingCount == pongOnGivenPing) {
            qDebug() << "pong: " << QTime::currentTime();
            QVERIFY(watchdog->pong());
        }
    }

private:
    QmProcessWatchdog *watchdog;
    unsigned pingCount;
    unsigned pongOnGivenPing;

    bool waitForPings(unsigned pings, unsigned timeout_ms)
    {
        for (unsigned int i = 0; i < timeout_ms; i += 1000) {
            QTest::qWait(1000);
            if (pingCount >= pings) {
                return true;
            }
        }
        return false;
    }

private slots:

    void initTestCase() {
        watchdog = new QmProcessWatchdog();
        pingCount = 0;
        pongOnGivenPing = 0;
        QVERIFY(connect(watchdog, SIGNAL(ping()), this, SLOT(ping())));
    }

    void testWatchdogBasicOperation() {
        pingCount = 0;
        QVERIFY(watchdog->start());
        qDebug() << "start: " << QTime::currentTime();
        bool pingsOk = waitForPings(4, 5*24*1000);
        qDebug() << "now: " << QTime::currentTime();
        QVERIFY(pingsOk);
        QVERIFY(watchdog->stop());
        pingCount = 0;
        QTest::qWait(60*1000);
        QVERIFY(pingCount == 0);
        QVERIFY(watchdog->start());
        qDebug() << "start again: " << QTime::currentTime();
        pingsOk = waitForPings(4, 5*24*1000);
        qDebug() << "now: " << QTime::currentTime();
        QVERIFY(pingsOk);
        QVERIFY(watchdog->stop());
    }

    void testWatchdogCloseShave() {
        pingCount = 0;
        pongOnGivenPing = 2;
        QVERIFY(watchdog->start());
        qDebug() << "start: " << QTime::currentTime();
        bool pingsOk = waitForPings(3, 4*24*1000);
        qDebug() << "now: " << QTime::currentTime();
        QVERIFY(pingsOk);
        QVERIFY(watchdog->stop());
    }

    void testWatchdogAbort() {
        pingCount = 0;
        pongOnGivenPing = 3;
        QVERIFY(watchdog->start());
        qDebug() << "start: " << QTime::currentTime();
        waitForPings(3, 4*24*1000);
        watchdog->stop();
    }

    void cleanupTestCase() {
        delete watchdog;
    }
};

QTEST_MAIN(TestClass)
#include "processwatchdog.moc"
