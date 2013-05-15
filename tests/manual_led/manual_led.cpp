/**
 * @file manual_led.cpp
 * @brief QmLED manual tests

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
#include <qmled.h>
#include <QTest>
#include <iostream>
#include <cstdio>

class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmLED *led;


private slots:
    void initTestCase() {
        led = new MeeGo::QmLED();
        QVERIFY(led);
    }

    void testPattern_data() {
        QTest::addColumn<QString>("pattern");
        QTest::newRow("PatternPowerOn") << "PatternPowerOn";
        QTest::newRow("PatternPowerOff") << "PatternPowerOff";
        QTest::newRow("PatternCommunication") << "PatternCommunication";
        QTest::newRow("PatternCommunicationAndBatteryFull") << "PatternCommunicationAndBatteryFull";
        QTest::newRow("PatternBatteryCharging") << "PatternBatteryCharging";
        QTest::newRow("PatternBatteryChargingFlat") << "PatternBatteryChargingFlat";
        QTest::newRow("PatternBatteryFull") << "PatternBatteryFull";
    }

    void testPattern() {
        QFETCH(QString, pattern);
        printf("\n\nThe next pattern is called %s.\n", pattern.toAscii().data());
        printf("Please look at the LED and see if the pattern is correct.\n");
        printf("The pattern will be activated for 20 seconds.\n");
        printf("Press enter to activate the pattern.\n");
        std::cin.get();

        QVERIFY(led->enable());
        QVERIFY(led->activate(pattern));
        printf("Pattern %s activated...\n", pattern.toAscii().data());
        QTest::qWait(20*1000);
        QVERIFY(led->deactivate(pattern));
        printf("Pattern %s deactivated...\n", pattern.toAscii().data());
        QVERIFY(led->disable());
    }

    void testDisable() {

        printf("This test will first disable the LED for 30 seconds and then enable it again.\n");
        printf("You need to check that it does not flash when disabled.\n");
        printf("Get ready! The test starts in 10 seconds...\n");

        QString pattern = "PatternCommunication";
        QVERIFY(led->disable());
        printf("\n\nThe led is now disabled.\n");
        printf("Please check that it does not flash for the next 30 seconds...\n");
        QVERIFY(led->activate(pattern));
        QTest::qWait(30*1000);

        QVERIFY(led->enable());
        printf("\n\nThe LED is now enabled again.\n");
        printf("Please check that if flashes again for the next 30 seconds...\n");
        QTest::qWait(30*1000);
        QVERIFY(led->deactivate(pattern));
        QVERIFY(led->disable());
    }

    void cleanupTestCase() {
        delete led;
    }
};

QTEST_MAIN(TestClass)
#include "manual_led.moc"
