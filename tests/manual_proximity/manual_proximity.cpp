/**
 * @file manual_proximity.cpp
 * @brief QmProximity manual tests

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
#include <qmproximity.h>
#include <QTest>
#include <cstdio>

using namespace MeeGo;

#define TOLERANCE 20

class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void ProximityChanged(MeeGo::QmProximityReading newValue) {
        printf("proximity: %d\n", newValue.value);
        proximity = newValue.value;
    }
    
private slots:
    void initTestCase() {
        proximity = -1;
        sensor = new QmProximity();
        QVERIFY(sensor);
        QVERIFY(connect(sensor, SIGNAL(ProximityChanged(MeeGo::QmProximityReading)),
                this, SLOT(ProximityChanged(MeeGo::QmProximityReading))));
        QVERIFY2(sensor->requestSession(QmSensor::SessionTypeControl) != QmSensor::SessionTypeNone,
                 sensor->lastError().toLocal8Bit());
        sensor->setStandbyOverride(true);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());

    }

    void testManual() {

        printf("\n");
        printf("This test will ask you to block the proximity sensor with your hand.\n");
        printf("Get ready! The test starts in 10 seconds...\n\n");
        QTest::qWait(10000);

        printf("Measuring the \"no proximity\" state. Please do not cover the proximity sensor...\n");
        QTest::qWait(5000);
        if (proximity == -1) {
            proximity = sensor->get().value;
        }
        QCOMPARE(proximity, 0);

        printf("Please cover the proximity sensor. You have 10 seconds...\n");
        QTest::qWait(10000);
        QCOMPARE(proximity, 1);

        /* Again, go back to normal. */
        printf("Please do not cover the sensor at all. You have 10 seconds...\n");
        QTest::qWait(10000);
        QCOMPARE(proximity, 0);

    }

    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        delete sensor;
    }


private:
    MeeGo::QmProximity *sensor;
    int proximity;
};

QTEST_MAIN(TestClass)
#include "manual_proximity.moc"
