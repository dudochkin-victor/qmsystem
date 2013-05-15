/**
 * @file magnetometer.cpp
 * @brief QmMagnetometer tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#include <qmmagnetometer.h>
#include <QTest>

using namespace MeeGo;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

public slots:
    void receive(const MeeGo::QmMagnetometerReading&) {}
};

class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmMagnetometer *sensor;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        sensor = new MeeGo::QmMagnetometer();
        QVERIFY(sensor);
    }

    void testConnectSignals() {
        QVERIFY(connect(sensor, SIGNAL(dataAvailable(const MeeGo::QmMagnetometerReading&)),
                &signalDump, SLOT(receive(const MeeGo::QmMagnetometerReading&))));
    }

    void testRequestSession() {
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                 sensor->lastError().toLocal8Bit());
    }

    void testStartStop() {
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
    }

    void testGetFunction() {
        QmMagnetometerReading result = sensor->magneticField();
        Q_UNUSED(result);
    }

    void cleanupTestCase() {
        delete sensor;
    }
};

QTEST_MAIN(TestClass)
#include "magnetometer.moc"
