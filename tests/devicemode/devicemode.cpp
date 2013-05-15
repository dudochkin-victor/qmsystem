/**
 * @file devicemode.cpp
 * @brief QmDeviceMode tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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
#include <qmdevicemode.h>
#include <QTest>
#include <QDebug>
#include <QtDBus/qdbusinterface.h>

/*
 * The device must be in the "user" state for this test.
 */

class SignalDump : public QObject
{
    Q_OBJECT
public:

    SignalDump() : QObject(NULL), mode(MeeGo::QmDeviceMode::Error), state(MeeGo::QmDeviceMode::PSMError) { }

    MeeGo::QmDeviceMode::DeviceMode mode;
    MeeGo::QmDeviceMode::PSMState state;

public slots:
    void deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode newMode) {
        mode = newMode;
    }

    void devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState newState) {
        state = newState;
    }
};

class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmDeviceMode *dm;
    SignalDump signalDump;

private slots:
    void initTestCase() {
        QDBusConnection::systemBus().call(QDBusMessage::createSignal("/com/nokia/bme/signal", "com.nokia.bme.signal", "charger_charging_off"));

        dm = new MeeGo::QmDeviceMode();
        QVERIFY(dm);
        QVERIFY(connect(dm, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)),
                        &signalDump, SLOT(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode))));
        QVERIFY(connect(dm, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)),
                        &signalDump, SLOT(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState))));
    }

    void testGetMode() {
        MeeGo::QmDeviceMode::DeviceMode result = dm->getMode();
        QVERIFY(result != MeeGo::QmDeviceMode::Error);
    }

    void testSetMode() {
        MeeGo::QmDeviceMode::DeviceMode mode = dm->getMode();
        if (mode != MeeGo::QmDeviceMode::Flight) {
            QVERIFY2(dm->setMode(MeeGo::QmDeviceMode::Flight), "Note that the system must be in the user state");
            QCOMPARE(dm->getMode(), MeeGo::QmDeviceMode::Flight);
            QTest::qWait(5000);
            QCOMPARE(signalDump.mode, MeeGo::QmDeviceMode::Flight);

            QVERIFY2(dm->setMode(MeeGo::QmDeviceMode::Normal), "Note that the system must be in the user state");
            QCOMPARE(dm->getMode(), MeeGo::QmDeviceMode::Normal);
            QTest::qWait(5000);
            qDebug()<<signalDump.mode;
            QCOMPARE(signalDump.mode, MeeGo::QmDeviceMode::Normal);
        } else {
            QVERIFY2(dm->setMode(MeeGo::QmDeviceMode::Normal), "Note that the system must be in the user state");
            QCOMPARE(dm->getMode(), MeeGo::QmDeviceMode::Normal);
            QTest::qWait(5000);
            QCOMPARE(signalDump.mode, MeeGo::QmDeviceMode::Normal);
            
            QVERIFY2(dm->setMode(MeeGo::QmDeviceMode::Flight), "Note that the system must be in the user state");
            QCOMPARE(dm->getMode(), MeeGo::QmDeviceMode::Flight);
            QTest::qWait(5000);
            QCOMPARE(signalDump.mode, MeeGo::QmDeviceMode::Flight);
        }
    }

    void testSetGetPSMState() {
        for (int state=0; state < 2; state++) {
            bool result = dm->setPSMState(((MeeGo::QmDeviceMode::PSMState)state));
            QVERIFY(result);
            QCOMPARE((int)dm->getPSMState(), state);
        }
    }

    void testDevicePSMStateChanged() {
        QVERIFY(dm->setPSMState(MeeGo::QmDeviceMode::PSMStateOn));
        for (int state=0; state < 2; state++) {
            bool result = dm->setPSMState((MeeGo::QmDeviceMode::PSMState)state);
            QVERIFY(result == true);
            QTest::qWait(5000);
            QCOMPARE((int)signalDump.state, state);
        }
    }


    void testSetGetPSMBatteryMode() {
        QVERIFY(dm->setPSMBatteryMode(0));
        QCOMPARE(dm->getPSMBatteryMode(), 0);

        QVERIFY(!dm->setPSMBatteryMode(-1));
        QVERIFY(!dm->setPSMBatteryMode(101));

        QVERIFY(dm->setPSMBatteryMode(10));
        QCOMPARE(dm->getPSMBatteryMode(), 10);
        QVERIFY(dm->setPSMBatteryMode(15));
        QCOMPARE(dm->getPSMBatteryMode(), 20);
        QVERIFY(dm->setPSMBatteryMode(20));
        QCOMPARE(dm->getPSMBatteryMode(), 20);
        QVERIFY(dm->setPSMBatteryMode(25));
        QCOMPARE(dm->getPSMBatteryMode(), 30);
        QVERIFY(dm->setPSMBatteryMode(30));
        QCOMPARE(dm->getPSMBatteryMode(), 30);
        QVERIFY(dm->setPSMBatteryMode(35));
        QCOMPARE(dm->getPSMBatteryMode(), 40);
        QVERIFY(dm->setPSMBatteryMode(40));
        QCOMPARE(dm->getPSMBatteryMode(), 40);
        QVERIFY(dm->setPSMBatteryMode(45));
        QCOMPARE(dm->getPSMBatteryMode(), 50);
        QVERIFY(dm->setPSMBatteryMode(50));
        QCOMPARE(dm->getPSMBatteryMode(), 50);
        QVERIFY(dm->setPSMBatteryMode(55));
        QCOMPARE(dm->getPSMBatteryMode(), 50);

    }

    void cleanupTestCase() {
        delete dm;
    }
};

QTEST_MAIN(TestClass)
#include "devicemode.moc"
