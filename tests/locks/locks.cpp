/**
 * @file locks.cpp
 * @brief QmLocks tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#include <qmlocks.h>
#include <QTest>

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
    }

    void testConnectSignals() {
        QVERIFY(connect(locks, SIGNAL(stateChanged(MeeGo::QmLocks::Lock, MeeGo::QmLocks::State)),
                        &signalDump, SLOT(stateChanged(MeeGo::QmLocks::Lock, MeeGo::QmLocks::State))));
    }

    void testGetTouchAndKeyboardState() {
        MeeGo::QmLocks::State result = locks->getState(MeeGo::QmLocks::TouchAndKeyboard);
        (void)result;
    }

    void testSetTouchAndKeyboardStateLocked() {
        signalDump.signal = false;
        bool result = locks->setState(MeeGo::QmLocks::TouchAndKeyboard, MeeGo::QmLocks::Locked);
        QVERIFY(result == true);
        QCOMPARE(locks->getState(MeeGo::QmLocks::TouchAndKeyboard), MeeGo::QmLocks::Locked);
        QTest::qWait(2000);
        QVERIFY(signalDump.signal);
        QVERIFY(signalDump.lock == MeeGo::QmLocks::TouchAndKeyboard);
        QVERIFY(signalDump.state == MeeGo::QmLocks::Locked);
    }

    void testSetTouchAndKeyboardStateUnlocked() {
        signalDump.signal = false;
        bool result = locks->setState(MeeGo::QmLocks::TouchAndKeyboard, MeeGo::QmLocks::Unlocked);
        QVERIFY(result == true);
        QCOMPARE(locks->getState(MeeGo::QmLocks::TouchAndKeyboard), MeeGo::QmLocks::Unlocked);
        QTest::qWait(2000);
        QVERIFY(signalDump.signal);
        QVERIFY(signalDump.lock == MeeGo::QmLocks::TouchAndKeyboard);
        QVERIFY(signalDump.state == MeeGo::QmLocks::Unlocked);
    }

    void cleanupTestCase() {
        delete locks;
    }
};

QTEST_MAIN(TestClass)
#include "locks.moc"
