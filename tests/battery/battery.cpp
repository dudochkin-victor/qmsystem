/**
 * @file battery.cpp
 * @brief QmBattery tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Matti Halme <matthalm@esdhcp04350.research.nokia.com>
   @author Na Fan <ext-na.2.fan@nokia.com>
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
#include <qmbattery.h>
#include <QTest>

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent), batteryCurrentSignal(false) {}

    bool batteryCurrentSignal;

public slots:
    void slotChargingStateChanged(MeeGo::QmBattery::ChargingState){}
    void slotChargerEvent(MeeGo::QmBattery::ChargerType){}
    void slotBatteryStateChanged(MeeGo::QmBattery::BatteryState){}
    void slotBatteryRemainingCapacityChanged(int, int){}
    void slotBatteryCurrent(int) { batteryCurrentSignal = true; }
    
    /* Depreciated */
    void slotBatteryEnergyLevelChanged(int){}

};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmBattery *battery;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        battery = new MeeGo::QmBattery();
        QVERIFY(battery);
    }

    void testConnectSignals() {
        QVERIFY(connect(battery, SIGNAL(chargingStateChanged(MeeGo::QmBattery::ChargingState)),
              &signalDump, SLOT(slotChargingStateChanged(MeeGo::QmBattery::ChargingState))));

        QVERIFY(connect(battery, SIGNAL(chargerEvent(MeeGo::QmBattery::ChargerType)),
                &signalDump, SLOT(slotChargerEvent(MeeGo::QmBattery::ChargerType))));

        QVERIFY(connect(battery, SIGNAL(batteryStateChanged(MeeGo::QmBattery::BatteryState)),
                &signalDump, SLOT(slotBatteryStateChanged(MeeGo::QmBattery::BatteryState))));

	/* Deprceated, remove this check when not used any more */
        QVERIFY(connect(battery, SIGNAL(batteryEnergyLevelChanged(int)),
                &signalDump, SLOT(slotBatteryEnergyLevelChanged(int))));

	QVERIFY(connect(battery,
			SIGNAL(batteryRemainingCapacityChanged(int, int)),
			&signalDump,
			SLOT(slotBatteryRemainingCapacityChanged(int, int))));

        QVERIFY(connect(battery, SIGNAL(batteryCurrent(int)),
                &signalDump, SLOT(slotBatteryCurrent(int))));
        QTest::qWait(10*1000);

    }
    
    void testBatteryEnergyPercentage() {
        int result = battery->getBatteryEnergyLevel();
        (void)result;
    }

    void testGetChargerType() {
        MeeGo::QmBattery::ChargerType result = battery->getChargerType();
        (void)result;
    }

    void testGetNominalCapacity() {
        int result = battery->getNominalCapacity();
        (void)result;
    }

    void testGetVoltage() {
        int result = battery->getVoltage();
        (void)result;
    }

    void testGetLevel() {
        MeeGo::QmBattery::Level result = battery->getLevel();
        (void)result;
    }

    void testGetChargingState() {
        MeeGo::QmBattery::ChargingState result = battery->getChargingState();
        (void)result;
    }

    void testGetBatteryCondition() {
        MeeGo::QmBattery::BatteryCondition result = battery->getBatteryCondition();
        QVERIFY(result == MeeGo::QmBattery::ConditionGood || result == MeeGo::QmBattery::ConditionPoor || result == MeeGo::QmBattery::ConditionUnknown);
        (void)result;
    }

    void testAverageTalkCurrentNormal() {
        int result = battery->getAverageTalkCurrent(MeeGo::QmBattery::NormalMode);
        (void)result;
    }

    void testRemainingTalkTimeNormal() {
        int result = battery->getRemainingTalkTime(MeeGo::QmBattery::NormalMode);
        (void)result;
    }

    void testAverageTalkCurrentPowersave() {
        int result = battery->getAverageTalkCurrent(MeeGo::QmBattery::PowersaveMode);
        (void)result;
    }

    void testRemainingTalkTimePowersave() {
        int result = battery->getRemainingTalkTime(MeeGo::QmBattery::PowersaveMode);
        (void)result;
    }

    void testAverageActiveCurrentNormal() {
        int result = battery->getAverageActiveCurrent(MeeGo::QmBattery::NormalMode);
        (void)result;
    }

    void testRemainingActiveTimeNormal() {
        int result = battery->getRemainingActiveTime(MeeGo::QmBattery::NormalMode);
        (void)result;
    }

    void testAverageActiveCurrentPowersave() {
        int result = battery->getAverageActiveCurrent(MeeGo::QmBattery::PowersaveMode);
        (void)result;
    }

    void testRemainingActiveTimePowersave() {
        int result = battery->getRemainingActiveTime(MeeGo::QmBattery::PowersaveMode);
        (void)result;
    }

    void testAverageIdleCurrentNormal() {
        int result = battery->getAverageIdleCurrent(MeeGo::QmBattery::NormalMode);
        (void)result;
    }

    void testRemainingIdleTimeNormal() {
        int result = battery->getRemainingIdleTime(MeeGo::QmBattery::NormalMode);
        (void)result;
    }

    void testAverageIdleCurrentPowersave() {
        int result = battery->getAverageIdleCurrent(MeeGo::QmBattery::PowersaveMode);
        (void)result;
    }

    void testRemainingIdleTimePowersave() {
        int result = battery->getRemainingIdleTime(MeeGo::QmBattery::PowersaveMode);
        (void)result;
    }

    void testRemainingChargingTime() {
        int result = battery->getRemainingChargingTime();
        (void)result;
    }

    void testStartCurrentMeasurementMs250() {
        signalDump.batteryCurrentSignal = false;
        bool result = battery->startCurrentMeasurement(MeeGo::QmBattery::RATE_250ms);
        QVERIFY(result == true );
        QTest::qWait(1000);
        QVERIFY(signalDump.batteryCurrentSignal);
    }

    void testStopCurrentMeasurementMs250() {
        bool result = battery->stopCurrentMeasurement();
        QVERIFY(result == true);
        QTest::qWait(1000);
        signalDump.batteryCurrentSignal = false;
        QTest::qWait(2000);
        QVERIFY(!signalDump.batteryCurrentSignal);
    }

    void testStartCurrentMeasurementMs1000() {
        signalDump.batteryCurrentSignal = false;
        bool result = battery->startCurrentMeasurement(MeeGo::QmBattery::RATE_1000ms);
        QVERIFY(result == true);
        QTest::qWait(2000);
        QVERIFY(signalDump.batteryCurrentSignal);
    }

    void testStopCurrentMeasurementMs1000() {
        bool result = battery->stopCurrentMeasurement();
        QVERIFY(result == true);
        QTest::qWait(1000);
        signalDump.batteryCurrentSignal = false;
        QTest::qWait(2000);
        QVERIFY(!signalDump.batteryCurrentSignal);
    }

    void testStartCurrentMeasurementMs5000() {
        signalDump.batteryCurrentSignal = false;
        bool result = battery->startCurrentMeasurement(MeeGo::QmBattery::RATE_5000ms);
        QVERIFY(result == true);
        QTest::qWait(6000);
        QVERIFY(signalDump.batteryCurrentSignal);
    }

    void testStopCurrentMeasurementMs5000() {
        bool result = battery->stopCurrentMeasurement();
        QVERIFY(result == true);
        QTest::qWait(1000);
        signalDump.batteryCurrentSignal = false;
        QTest::qWait(2000);
        QVERIFY(!signalDump.batteryCurrentSignal);
    }

    void cleanupTestCase() {
        delete battery;
    }
};

QTEST_MAIN(TestClass)
#include "battery.moc"
