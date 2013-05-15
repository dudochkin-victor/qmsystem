/**
 * @file manual_tap.cpp
 * @brief QmTap manual tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Yang Yang <ext-yang.25.yang@nokia.com>

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

#include <qmtap.h>
#include <QTest>
#include <QDebug>

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

private:
    QmTap *sensor;
    QmTapReading read;
public slots:
    void receive(const MeeGo::QmTapReading read) {
        qDebug()<<"Direction:"<<read.direction<<" Type:"<<read.type;
        this->read.direction = read.direction;
        this->read.type = read.type;
    }

private slots:
    void initTestCase() {
        sensor = new QmTap();
        QVERIFY(sensor);
        QVERIFY(connect(sensor, SIGNAL(tapped(const MeeGo::QmTapReading)),
                this, SLOT(receive(const MeeGo::QmTapReading))));
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                sensor->lastError().toLocal8Bit());
        sensor->setStandbyOverride(true);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
    }

    void testTap() {
        qDebug()<<"This test will ask you to tap and double tap the screen\nGet ready! Test will start in 5 seconds\n\n";
        QTest::qWait(5000);
        qDebug()<<"Please tap the device";
        QTest::qWait(10000);
        QCOMPARE(read.type, QmTap::SingleTap);

        qDebug()<<"Please double tap the device";
        QTest::qWait(10000);
        QCOMPARE(read.type, QmTap::DoubleTap);
    }

    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        delete sensor;
    }
};

QTEST_MAIN(TestClass)
#include "manual_tap.moc"
