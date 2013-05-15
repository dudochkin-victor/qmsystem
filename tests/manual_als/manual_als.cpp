/**
 * @file manual_als.cpp
 * @brief QmALS manual tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
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
#include <qmals.h>
#include <QTest>
#include <QDebug>

using namespace MeeGo;

#define TOLERANCE 20

class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void ALSChanged(const MeeGo::QmAlsReading newValue) {
        als = newValue.value;
    }
    
private slots:
    void initTestCase() {
        sensor = new MeeGo::QmALS();
        QVERIFY(sensor);
        QVERIFY(connect(sensor, SIGNAL(ALSChanged(const MeeGo::QmAlsReading)),
                this, SLOT(ALSChanged(const MeeGo::QmAlsReading))));
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                 sensor->lastError().toLocal8Bit());
        sensor->setStandbyOverride(true);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
        als = -1;
    }

    void testManual() {
       
        printf("\n");
        printf("This test should be run in a normally lighted room or better\n");
        printf("This test will ask you to block the light sensor with your hand.\n");
        printf("Get ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10000);

        /* First, get the normal light level. */
        printf("Measuring the normal light level. Please do not cover the light sensor...\n");
        QTest::qWait(5000);
        if (als == -1) {
            als = sensor->get().value;
        }
        int normalAls = als;
        QVERIFY(normalAls > 0);
        printf("The normal light level is: %d\n\n", normalAls);

        /* Then get the zero light level. */
        printf("Please cover the light sensor fully. You have 10 seconds...\n");
        QTest::qWait(10000);
        printf("Fully covered als: %d\n", als);
        //QCOMPARE(als, 0);
        QVERIFY(als >= 0 && als < 5);

        /* Then get the "dimmed" light level. Basically we just want this to be smaller than the "normal" light, but above zero. */
        printf("\nPlease cover the light sensor so, that some light gets through. For example, cast a shadow over it.\n");
        printf("You have 10 seconds...\n");
        QTest::qWait(10000);
        printf("The dimmed light level is: %d\n\n", als);
        QVERIFY(als < normalAls && als > 0);

        /* Again, go back to normal. */
        printf("Please do not cover the sensor at all. You have 10 seconds...\n");
        QTest::qWait(10000);
        printf("The new normal light level is: %d\n", als);
        QVERIFY(als >= normalAls - TOLERANCE && als <= normalAls + TOLERANCE);
       

        /* Test the decreases when move the device from stronger light source to darker place. */
        printf("\nPlease do not cover the light sensor, move the device sensor near the strongest light source in your test place.\n");
        printf("You have 10 seconds...\n");
        QTest::qWait(10000);
        printf("The light level in bright place is: %d\n", als);

        printf("\nPlease move device to the dark place smoothly\n");
        printf("You have 10 seconds...\n");
        QTest::qWait(2000);

        for (int i = 0; i < 20; i++) {
            QTest::qWait(500);
            printf("The light level: %d\n", als);
        }

        printf("The light level in dark place is: %d\n", als);

        /* Test the decreases when move the device from stronger light source to darker place. */
        printf("\nPlease do not cover the light sensor, move back the device from dark to bright smoothly.\n");
        printf("You have 10 seconds...\n");
        printf("The light level in dark place is: %d\n", als);
        QTest::qWait(2000);

        for (int i = 0; i < 20; i++) {
            QTest::qWait(500);
            printf("The light level: %d\n", als);
        }
        printf("The light level in bright place is: %d\n", als);
    }

    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        delete sensor;
    }


private:
    MeeGo::QmALS *sensor;
    int als;
};

QTEST_MAIN(TestClass)
#include "manual_als.moc"
