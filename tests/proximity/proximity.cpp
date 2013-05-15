/**
 * @file proximity.cpp
 * @brief QmProximity tests

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
#include <QVariant>
#include <qmproximity.h>
#include <QTest>
#include <QDebug>

using namespace MeeGo;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

public slots:
    void receive(MeeGo::QmProximityReading) {}
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    QmProximity *sensor;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        sensor = new QmProximity();
        QVERIFY(sensor);
    }

    void testConnectProximitySensorSignals() {
        QVERIFY(connect(sensor, SIGNAL(ProximityChanged(MeeGo::QmProximityReading)),
                &signalDump, SLOT(receive(MeeGo::QmProximityReading))));
    }

    void testRequestProximitySensorSession() {
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                 sensor->lastError().toLocal8Bit());
    }
    
    void testProximitySensorStartStop() {
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
    }

    void testGetProximityDataFunction() {
        QmProximityReading result = sensor->get();
        (void)result;
    }

    void cleanupTestCase() {
        delete sensor;
    }
};

QTEST_MAIN(TestClass)
#include "proximity.moc"
