/**
 * @file manual_locks.cpp
 * @brief QmLocks manual tests

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

#include <QObject>
#include <qmlocks.h>
#include <QTest>
#include <iostream>
#include <cstdio>

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent), signal(false) {}

    MeeGo::QmLocks::Lock lock;
    MeeGo::QmLocks::State state;
    bool signal;

public slots:
    void stateChanged(MeeGo::QmLocks::Lock newLock, MeeGo::QmLocks::State newState) {
        lock = newLock;
        state = newState;
        signal = true;
    }
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmLocks *locks;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        locks = new MeeGo::QmLocks();
        QVERIFY(locks);
        QVERIFY(connect(locks, SIGNAL(stateChanged(MeeGo::QmLocks::Lock, MeeGo::QmLocks::State)),
                        &signalDump, SLOT(stateChanged(MeeGo::QmLocks::Lock, MeeGo::QmLocks::State))));
    }

    void testUnlock() {
        locks->setState(MeeGo::QmLocks::Device, MeeGo::QmLocks::Unlocked);
    }

    void test() {
        printf("This test will try to set device lock on.\n");
        printf("First you need to set a device lock pin code from the UI:\n");
        printf("System Settings -> Security -> Device lock -> Security code\n");
        printf("Do it now, and press enter to continue the test...\n");

        std::cin.get();

        printf("Setting the device lock on...\n");
        signalDump.signal = false;
        bool result = locks->setState(MeeGo::QmLocks::Device, MeeGo::QmLocks::Locked);
        QVERIFY(result == true);
        QTest::qWait(2000);
        QVERIFY(signalDump.signal);
        QVERIFY(signalDump.lock == MeeGo::QmLocks::Device);
        QVERIFY(signalDump.state == MeeGo::QmLocks::Locked);
        QCOMPARE(MeeGo::QmLocks::Locked, locks->getState(MeeGo::QmLocks::Device));

        std::string res = "";
        do {
            printf("Is the device locked? (y/n)\n");
            std::cin >> res;
        } while (res != "y" && res != "Y" && res != "n" && res != "N" && res != "\n");

        if (res == "n" || res == "N") {
            QFAIL("The device was not locked!");
        }

        printf("Settings the device lock off...\n");
        signalDump.signal = false;
        result = locks->setState(MeeGo::QmLocks::Device, MeeGo::QmLocks::Unlocked);
        QVERIFY(result == true);
        QTest::qWait(2000);
        QVERIFY(signalDump.signal);
        QVERIFY(signalDump.lock == MeeGo::QmLocks::Device);
        QVERIFY(signalDump.state == MeeGo::QmLocks::Unlocked);
        QCOMPARE(MeeGo::QmLocks::Unlocked, locks->getState(MeeGo::QmLocks::Device));

        res = "";
        do {
            printf("Is the device unlocked? (y/n)\n");
            std::cin >> res;
        } while (res != "y" && res != "Y" && res != "n" && res != "N" && res != "\n");

        if (res == "n" || res == "N") {
            QFAIL("The device was not unlocked!");
        }
    }

    void cleanupTestCase() {
        delete locks;
    }
};

QTEST_MAIN(TestClass)
#include "manual_locks.moc"
