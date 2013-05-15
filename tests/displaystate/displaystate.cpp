/**
 * @file displaystate.cpp
 * @brief QmDisplayState tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
#include <qmdisplaystate.h>
#include <QTest>
#include <QDebug>

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent), state(MeeGo::QmDisplayState::On) {}

    MeeGo::QmDisplayState::DisplayState state;

public slots:
    void displayStateChanged(MeeGo::QmDisplayState::DisplayState newState ) {
        state = newState;
    }
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmDisplayState *displaystate;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        displaystate = new MeeGo::QmDisplayState();
        QVERIFY(displaystate);
    }

    void testConnectSignals() {
        QVERIFY(connect(displaystate, SIGNAL(displayStateChanged(MeeGo::QmDisplayState::DisplayState)),
                &signalDump, SLOT(displayStateChanged(MeeGo::QmDisplayState::DisplayState))));
    }

    void testGet() {
        MeeGo::QmDisplayState::DisplayState result = displaystate->get();
        (void)result;
    }

    void testSetStateOff() {
        displaystate->set(MeeGo::QmDisplayState::On);
        bool result = displaystate->set(MeeGo::QmDisplayState::Off);
        QVERIFY(result == true);
        QTest::qWait(2000);
        QVERIFY(signalDump.state == MeeGo::QmDisplayState::Off);
    }

    void testSetStateDimmed() {
        displaystate->set(MeeGo::QmDisplayState::On);
        bool result = displaystate->set(MeeGo::QmDisplayState::Dimmed);
        QVERIFY(result == true);
        QTest::qWait(2000);
        QVERIFY(signalDump.state == MeeGo::QmDisplayState::Dimmed);
    }

    void testSetStateOn() {
        displaystate->set(MeeGo::QmDisplayState::Off);
        bool result = displaystate->set(MeeGo::QmDisplayState::On);
        QVERIFY(result == true);
        QTest::qWait(2000);
        QVERIFY(signalDump.state == MeeGo::QmDisplayState::On);
    }

    void testSetBlankingPause() {
        bool result = displaystate->setBlankingPause();
        QVERIFY(result == true);
    }

    void testGetMaxDisplayBrightnessValue() {
        displaystate->getMaxDisplayBrightnessValue();
    }

    void testDisplayBrightnessValue() {
        int originalDisplayBrightnessValue = displaystate->getDisplayBrightnessValue();

        displaystate->setDisplayBrightnessValue(3);
        int result = displaystate->getDisplayBrightnessValue();
        QVERIFY(result == 3);

        displaystate->setDisplayBrightnessValue(originalDisplayBrightnessValue);
    }

    void testDisplayBlankTimeout() {
        int originalDisplayBlankTimeout = displaystate->getDisplayBlankTimeout();

        displaystate->setDisplayBlankTimeout(10);
        int result = displaystate->getDisplayBlankTimeout();
        QVERIFY(result == 10);

        displaystate->setDisplayBlankTimeout(originalDisplayBlankTimeout);
    }

    void testDisplayDimTimeout() {
        int originalDisplayDimTimeout = displaystate->getDisplayDimTimeout();

        displaystate->setDisplayDimTimeout(15);
        int result = displaystate->getDisplayDimTimeout();
        QVERIFY(result == 15);

        displaystate->setDisplayDimTimeout(originalDisplayDimTimeout);
    }

    void testBlankingCharging() {
        bool originalBlankingWhenCharging = displaystate->getBlankingWhenCharging();

        displaystate->setBlankingWhenCharging(true);
        bool result = displaystate->getBlankingWhenCharging();
        QVERIFY(result == true);

        displaystate->setBlankingWhenCharging(false);
        result = displaystate->getBlankingWhenCharging();
        QVERIFY(result == false);

        displaystate->setBlankingWhenCharging(originalBlankingWhenCharging);
    }

    void cleanupTestCase() {
        delete displaystate;
    }
};

QTEST_MAIN(TestClass)
#include "displaystate.moc"
