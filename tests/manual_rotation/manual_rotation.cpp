/**
 * @file manual_rotation.cpp
 * @brief QmRotation manual tests

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
#include <QVariant>
#include <qmrotation.h>
#include <QTest>
#include <QDebug>

using namespace MeeGo;

#define TOLERANCE 10
#define TOLERANCE_Z 15

class TestClass : public QObject
{
    Q_OBJECT


    QTime lastPrint;

public slots:
    void dataAvailable(const MeeGo::QmRotationReading& data) {
        if (receiveData) {
            valueList.push_back(data);
        }
        if (receiveData) {
            printf("\tx: %d, y: %d, z: %d\n", data.x, data.y, data.z);
        } else if (printData && lastPrint.elapsed() >= 500) {
            printf("\tx: %d, y: %d, z: %d\n", data.x, data.y, data.z);
            lastPrint.restart();
        }
        xLatest = data.x;
        yLatest = data.y;
        zLatest = data.z;
    }

private:
    void testFunc(const QString &msg, int x, int y) {

        printf("%s", msg.toAscii().data());
        printf("You have 10 seconds...\n");
        valueList.clear();
        QTest::qWait(10000);
        printf("Measuring...\n");
        receiveData = true;
        QTest::qWait(1000);
        receiveData = false;

        int xAvgDst;
        int yAvgDst;

        if (valueList.size() == 0) {
            xAvgDst = abs(x - xLatest);

            yAvgDst = abs(y+179 - yLatest+179);
            if (yAvgDst > 180) {
                yAvgDst -= 180;
            }
        } else {
            QmRotationReading reading;
            int xSum = 0;
            int ySum = 0;
            int yTmp = 0;
            foreach(reading, valueList) {
                xSum += abs(x - reading.x);

                yTmp += abs(y+179 - reading.y+179);
                if (yTmp > 180) {
                    yTmp -= 180;
                }
                ySum += yTmp;
            }
            xAvgDst = xSum / valueList.size();
            yAvgDst = ySum / valueList.size();
        }

        printf("Avg Distcance for x: %d, y: %d\n", xAvgDst, yAvgDst);

        QVERIFY(abs(xAvgDst) <= TOLERANCE);

        if (abs(x) < 80 && abs(x) > 100) {
            QVERIFY(abs(yAvgDst) <= TOLERANCE);
        } else {
            /// Special case when y-rotation flips over 0<->180
            /// Let them pass... :)
        }
    }

    void initZ() {
        printf("Please put the device on a table display up, and turn it so that Z rotation is 0 (+- 5).\n");
        printData = true;
        zLatest = -99;
        while (true) {
            QTest::qWait(500);
            if (zLatest > -5 && zLatest < 5) {
                printData = false;
                printf("Z is set to: %d\n", zLatest);
                break;
            }
        }
    }

    void testFuncForZ(const QString &msg, int z) {
        printf("\n\n%s\n", msg.toAscii().data());
        printf("Your goal is to get z to: %d\n", z);
        printf("You got 10 seconds...\n");
        printData = true;
        valueList.clear();
        QTest::qWait(10000);
        printf("Measuring...\n");
        receiveData = true;
        QTest::qWait(1000);
        receiveData = false;
        valueList.push_back(sensor->rotation());

        int sum = 0;
        QmRotationReading reading;
        foreach (reading, valueList) {
            sum += reading.z;
        }
        int avg = sum / valueList.count();

        QVERIFY((avg <= z+TOLERANCE_Z) && (avg >= z-TOLERANCE_Z));
    }


private slots:
    void initTestCase() {
        receiveData = false;
        printData = false;
        sensor = new QmRotation();
        QVERIFY(sensor);
        zPrevious = 0;
        zFirstRun = true;

        QVERIFY(connect(sensor, SIGNAL(dataAvailable(MeeGo::QmRotationReading)),
                        this, SLOT(dataAvailable(MeeGo::QmRotationReading))));
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                 sensor->lastError().toLocal8Bit());
        sensor->setInterval(100);
        sensor->setStandbyOverride(true);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
    }

   void testManual() {
        printf("\n");
        printf("This test will instruct you to turn the device into various positions.\n");
        printf("You will have ten seconds to turn the device into each position, and you need to hold it for one second.\n");
        printf("Get ready! The test starts in 10 seconds...\n");
        QTest::qSleep(10000);


        testFunc(tr("\n\nPlease turn and hold the device so, that the display points up.\n"), 0, 0);
        testFunc(tr("\n\nPlease rotate the top edge of the device 45 degrees upwards.\n"), 45, 0);
        testFunc(tr("\n\nPlease reverse the previous rotation, and rotate the bottom edge of the device 45 degrees upwards.\n"), -45, 0);
        testFunc(tr("\n\nPlease reverse the previous rotation, and rotate the right edge of the device 45 degrees upwards.\n"), 0, -45);
        testFunc(tr("\n\nPlease reverse the previous rotation, and rotate the left edge of the device 45 degrees upwards.\n"), 0, 45);

        testFunc(tr("\n\nPlease turn and hold the device so, that the display points down and bottom edge points towards you.\n"), 0, 180);
        testFunc(tr("\n\nPlease rotate the top edge of the device 45 degrees upwards.\n"), 45, 0);
        testFunc(tr("\n\nPlease reverse the previous rotation, and rotate the bottom edge of the device 45 degrees upwards.\n"), -45, 0);
        testFunc(tr("\n\nPlease reverse the previous rotation, and rotate the perceived right side edge of the device 45 degrees upwards.\n"), 0, 135);
        testFunc(tr("\n\nPlease reverse the previous rotation, and rotate the perceived left side edge of the device 45 degrees upwards.\n"), 0, -135);

        testFunc(tr("\n\nPlease turn and hold the device so, that the top points up.\n"), 90, 0);
        testFunc(tr("\n\nPlease turn and hold the device so, that the top points down.\n"), -90, 0);
        testFunc(tr("\n\nPlease turn and hold the device so, that the right hand side points up.\n"), 0, -90);
        testFunc(tr("\n\nPlease turn and hold the device so, that the left hand side points up.\n"), 0, 90);

        if (sensor->hasZ()) {

            lastPrint.start();
            printf("\n\nSensor reports it can calculate z-axis values, running tests\n");
            initZ();
            testFuncForZ(tr("Please rotate the device clockwise 90 degrees\n"), -90);
            testFuncForZ(tr("Please rotate the device 180 degrees counter-clockwise\n"), 90);
        } else {
            printf("\n\nSensor reports it does not calculate z-axis values, skipping tests...\n");
        }
    }

    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        delete sensor;
    }


private:
    QmRotation *sensor;
    bool receiveData;
    QList<MeeGo::QmRotationReading> valueList;
    int xLatest;
    int yLatest;
    int zLatest;
    int zPrevious;
    bool zFirstRun;
    bool printData;
};

QTEST_MAIN(TestClass)
#include "manual_rotation.moc"

