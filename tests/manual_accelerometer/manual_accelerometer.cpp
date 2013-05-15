/**
 * @file manual_accelerometer.cpp
 * @brief QmAccelerometer manual tests

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
#include <qmaccelerometer.h>
#include <QTest>
#include <cstdio>

#define TOLERANCE 150

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void dataAvailable(const MeeGo::QmAccelerometerReading& data) {
        if (receiveData) {
            count++;
            xSum += data.x;
            ySum += data.y;
            zSum += data.z;
            printf("\tx: %d, y: %d, z: %d\n", data.x, data.y, data.z);
        }
        xLatest = data.x;
        yLatest = data.y;
        zLatest = data.z;
    }
    void errorSignal(QString error) {
        printf("error: %s\n", error.toAscii().data());
    }

    
private slots:
    void initTestCase() {
        count = -1;
        receiveData = false;
        sensor = new MeeGo::QmAccelerometer();
        QVERIFY(sensor);
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                 sensor->lastError().toLocal8Bit());
        sensor->setStandbyOverride(true);
        QVERIFY(connect(sensor, SIGNAL(dataAvailable(const MeeGo::QmAccelerometerReading&)),
                this, SLOT(dataAvailable(const MeeGo::QmAccelerometerReading&))));
        connect(sensor, SIGNAL(errorSignal(QString)), this, SLOT(errorSignal(QString)));
        sensor->setInterval(100);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
    }

    void testFunc(const QString &msg, int x, int y, int z) {

        printf("%s", msg.toAscii().data());
        printf("You have 10 seconds...\n");
        count = 0;
        xSum = 0;
        ySum = 0;
        zSum = 0;
        QTest::qWait(10000);
        printf("Measuring...\n");
        receiveData = true;
        QTest::qWait(1000);
        receiveData = false;

        int xAvg;
        int yAvg;
        int zAvg;
        if (count > 0) {
            xAvg = xSum/count;
            yAvg = ySum/count;
            zAvg = zSum/count;
        } else {
            xAvg = xLatest;
            yAvg = yLatest;
            zAvg = zLatest;
        }
        printf("Avg x: %d, y: %d, z: %d\n", xAvg, yAvg, zAvg);

        QVERIFY(xAvg <= x+TOLERANCE && xAvg >= x-TOLERANCE);
        QVERIFY(yAvg <= y+TOLERANCE && yAvg >= y-TOLERANCE);
        QVERIFY(zAvg <= z+TOLERANCE && zAvg >= z-TOLERANCE);
    }

    void testManual() {
        printf("\n");
        printf("This test will instruct you to turn the device into various positions.\n");
        printf("You will have ten seconds to turn the device into each position, and you need to hold it for one second.\n");
        printf("Get ready! The test starts in 10 seconds...\n");
        QTest::qSleep(10000);

        testFunc(tr("\n\nPlease turn and hold the device so, that the display points down.\n"), 0, 0, 1000);
        testFunc(tr("\n\nPlease turn and hold the device so, that the display points up.\n"), 0, 0, -1000);
        testFunc(tr("\n\nPlease turn and hold the device so, that the top points up.\n"), 0, -1000, 0);
        testFunc(tr("\n\nPlease turn and hold the device so, that the top points down.\n"), 0, 1000, 0);
        testFunc(tr("\n\nPlease turn and hold the device so, that the right hand side points up.\n"), -1000, 0, 0);
        testFunc(tr("\n\nPlease turn and hold the device so, that the left hand side points up.\n"), 1000, 0, 0);

    }

    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        delete sensor;
    }

private:
    MeeGo::QmAccelerometer *sensor;
    bool receiveData;
    int count;
    int xSum;
    int ySum;
    int zSum;
    int xLatest;
    int yLatest;
    int zLatest;
};

QTEST_MAIN(TestClass)
#include "manual_accelerometer.moc"
