/**
 * @file heartbeat.cpp
 * @brief QmHeartbeat tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
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
#include <QObject>
#include <qmheartbeat.h>
#include <QTest>
#include <QTimer>
#include <QDebug>


static int got_signal = 0;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {  }

public slots:
    void wakeUp(QTime time){ qDebug() << "Woke up, consumed" << time; got_signal = 1; }
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmHeartbeat *heartbeat, *heartbeat2;
    SignalDump signalDump;

private slots:
    void initTestCase() {
        heartbeat = new MeeGo::QmHeartbeat();
        QVERIFY(heartbeat);

        bool result = heartbeat->open(MeeGo::QmHeartbeat::SignalNeeded);
        QVERIFY(result);

        QVERIFY(connect(heartbeat, SIGNAL(wakeUp(QTime)),
                &signalDump, SLOT(wakeUp(QTime))));

        heartbeat2 = new MeeGo::QmHeartbeat();
        QVERIFY(heartbeat2);

        bool result2 = heartbeat2->open(MeeGo::QmHeartbeat::NoSignalNeeded);
        QVERIFY(result2);
    }

    void testGetFD(){
        int result = heartbeat->getFD();
        QVERIFY(result != -1);

        int result2 = heartbeat2->getFD();
        QVERIFY(result2 != -1);
    }

    void testSyncronousWait() {
        qDebug() << "Test syncronous wait...";
        int sleeptime = 4;
        qDebug() << "Wait " <<  sleeptime << " seconds" ;
        // Sleep 4-8 seconds, so probably the wakeup time is on average ~6 seconds
        QTime result = heartbeat2->wait(sleeptime, sleeptime*2, MeeGo::QmHeartbeat::WaitHeartbeat);

        QVERIFY(result.second() >= sleeptime);
        QVERIFY(result.second() <= sleeptime * 2);
        qDebug() << "Slept " << result.second() << "seconds" ;
        QVERIFY(got_signal == 0);
    }

    void testAsyncWaitAndCancel() {
        qDebug() << "Test async wait, cancel immediately";
        int sleeptime = 7;
        QTime result = heartbeat->wait(0, sleeptime, MeeGo::QmHeartbeat::DoNotWaitHeartbeat);
        QVERIFY(result.second() >= 0);
        QVERIFY(result.second() <= 1);
        heartbeat->IWokeUp();
        QTest::qWait(1000);
        QVERIFY(got_signal == 0);
    }

    void testAsyncWait() {
        qDebug() << "Test async wait, assume that signal was got";
        int sleeptime = 7;
        QTime result = heartbeat->wait(0, sleeptime, MeeGo::QmHeartbeat::DoNotWaitHeartbeat);
        QVERIFY(result.second() >= 0);
        QVERIFY(result.second() <= 1);
        QVERIFY(got_signal == 0);
        qDebug() << "Slept " << result.second() << "seconds" ;
        sleeptime = 8;
        qDebug() << "Sleep " <<  sleeptime << " seconds" ;
        while (sleeptime > 0 && !got_signal) {
           QTest::qWait(1000);
           sleeptime--;
           qDebug("zzz");
        }
        heartbeat->IWokeUp();

        QVERIFY(got_signal == 1);
    }

    void testNoSignalNeeded() {
        got_signal=0;
        int sleep_s = 4;

        QTime result = heartbeat2->wait(0, sleep_s, MeeGo::QmHeartbeat::WaitHeartbeat);
        QVERIFY(result.second() >= sleep_s && result.second() <= sleep_s+1);
        QVERIFY(got_signal == 0);
        heartbeat2->IWokeUp();

        sleep_s=7;
        result = heartbeat2->wait(0, 7, MeeGo::QmHeartbeat::DoNotWaitHeartbeat);
        QVERIFY(result.second() >= 0 &&  result.second() <= 1);
        QTest::qWait(8000);
        heartbeat2->IWokeUp();
        QVERIFY(got_signal == 0);
    }

   void cleanupTestCase() {
        heartbeat->close();
        heartbeat2->close();

        delete heartbeat, heartbeat = 0;
        delete heartbeat2, heartbeat2 = 0;
    }
};

QTEST_MAIN(TestClass)
#include "heartbeat.moc"
