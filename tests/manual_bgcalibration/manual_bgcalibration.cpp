/**
 * @file manual_bgcalibration.cpp
 * @brief QmMagnetometer background calibartion tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Üstün Ergenoglu <ustun.ergenoglu@digia.com>

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
#include <qmmagnetometer.h>
#include <QTest>
#include <QList>
#include <QTime>
#include <cstdio>

#define SECONDSTOTEST 30

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

public slots:

    void magnetometerChanged(const MeeGo::QmMagnetometerReading newMagneticField) {
        printf("calibration level is: %d\n", newMagneticField.level);
    }

private slots:
    void initTestCase() {
        sensor = new MeeGo::QmMagnetometer();
        QVERIFY(sensor);
        QVERIFY(connect(sensor, SIGNAL(dataAvailable(const MeeGo::QmMagnetometerReading)),
                this, SLOT(magnetometerChanged(const MeeGo::QmMagnetometerReading))));
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                sensor->lastError().toLocal8Bit());
        sensor->setStandbyOverride(true);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
        QmMagnetometerReading r = sensor->magneticField();
        printf("initial calibration level before test: %d\n", r.level);
    }

    void testManual() {
        printf("background calibration test begins. waiting 5 seconds.\n");
        QTest::qWait(5000);
        sensor->reset();
        QTest::qWait(1000);
        QmMagnetometerReading r = sensor->magneticField();
        QVERIFY(r.level == 0);

        delete sensor;
        sensor = NULL;

        while (true) {
            printf("\n\n");
            printf("Now, move the device, do some phone calls, put the phone in your pocket and walk around, for %d seconds.\n",
                    SECONDSTOTEST);
            QTest::qWait(SECONDSTOTEST*1000);

            sensor = new MeeGo::QmMagnetometer();
            QVERIFY(connect(sensor, SIGNAL(dataAvailable(const MeeGo::QmMagnetometerReading)),
                        this, SLOT(magnetometerChanged(const MeeGo::QmMagnetometerReading))));
            QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                    sensor->lastError().toLocal8Bit());
            sensor->setStandbyOverride(true);
            QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());

            r = sensor->magneticField();
            QTest::qWait(5000);
            r = sensor->magneticField();
            printf("the calibration level is: %d\n", r.level);
            QVERIFY(sensor);
            QVERIFY(r.level > 0);

            if (r.level == 3)
                break;

            delete sensor;
            sensor = NULL;
        }
    }


    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        delete sensor;
    }

private:
    MeeGo::QmMagnetometer *sensor;
};

QTEST_MAIN(TestClass)
#include "manual_bgcalibration.moc"
