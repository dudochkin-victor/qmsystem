/**
 * @file callstate.cpp
 * @brief QmCallState tests

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
#include <qmcallstate.h>
#include <QTest>
#include <QDebug>

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent), state(MeeGo::QmCallState::Error), type(MeeGo::QmCallState::Unknown), signalReceived(false) {}
    MeeGo::QmCallState::State state;
    MeeGo::QmCallState::Type type;
    bool signalReceived;
public slots:
    void stateChanged(MeeGo::QmCallState::State newState, MeeGo::QmCallState::Type newType) {
        signalReceived = true;
        state = newState;
        type = newType;
    }
};

class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmCallState *callstate;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        callstate = new MeeGo::QmCallState();
        QVERIFY(callstate);
    }

    void testConnectSignals() {
        QVERIFY(connect(callstate, SIGNAL(stateChanged(MeeGo::QmCallState::State, MeeGo::QmCallState::Type)),
                &signalDump, SLOT(stateChanged(MeeGo::QmCallState::State, MeeGo::QmCallState::Type))));
    }

    void testGetState(){
        MeeGo::QmCallState::State result = callstate->getState();
        QVERIFY(result != MeeGo::QmCallState::Error);
    }

    void testGetType() {
        MeeGo::QmCallState::Type result = callstate->getType();
        (void)result;
    }

    void testSetStates() {
        for (int state = 1; state < 3; state++) {
            for (int type = 0; type < 2; type++) {
                signalDump.signalReceived = false;
                bool result = callstate->setState((MeeGo::QmCallState::State)state, (MeeGo::QmCallState::Type)type);
                QVERIFY(result);
                QTest::qWait(5000);
                QCOMPARE((int)signalDump.state, state);
                QCOMPARE((int)signalDump.type, type);

                /* MCE allows only changes to/from None, so we reset it every time. :)
                 */
                signalDump.signalReceived = false;
                result = callstate->setState(MeeGo::QmCallState::None, (MeeGo::QmCallState::Type)type);
                QVERIFY(result);
                QTest::qWait(5000);
                QCOMPARE(signalDump.state, MeeGo::QmCallState::None);
                QCOMPARE((int)signalDump.type, type);
            }
        }
    }

    void cleanupTestCase() {
        delete callstate;
    }
};

QTEST_MAIN(TestClass)
#include "callstate.moc"
