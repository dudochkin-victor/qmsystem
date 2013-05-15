/**
 * @file manual_orientation.cpp
 * @brief QmOrientation manual tests

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
#include <qmorientation.h>
#include <QTest>
#include <QDebug>

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void orientationChanged(const MeeGo::QmOrientationReading newOrientation) {
        if (receiveOrientations) {
            orientationChangedFlag = true;
            orientation = newOrientation;
            printf("\treceived orientation: %s\n", orientationToString(orientation.value).toAscii().data());
        }
    }

private:
    QString orientationToString(QmOrientation::Orientation orientation) {
        QString orientationStr;

        switch (orientation) {
        case QmOrientation::BottomUp:
            orientationStr = "the bottom faces up";
            break;
        case QmOrientation::BottomDown:
            orientationStr = "the bottom faces down";
            break;
        case QmOrientation::LeftUp:
            orientationStr = "the left hand side faces up";
            break;
        case QmOrientation::RightUp:
            orientationStr = "the right hand side faces up";
            break;
        case QmOrientation::FaceDown:
            orientationStr = "the display faces down";
            break;
        case QmOrientation::FaceUp:
            orientationStr = "the display faces up";
            break;
        default:
            orientationStr = "the orientation is undefined";
        }

        return orientationStr;
    }

    void testFunc(QmOrientation::Orientation orientationToTest) {

        printf("\n\nPlease turn and hold the device so, that %s. You have 10 seconds...\n", orientationToString(orientationToTest).toAscii().data());
        receiveOrientations = true;
        QTest::qWait(10000);
        QCOMPARE(orientation.value, orientationToTest);
        QCOMPARE(orientation.value, sensor->orientation().value);
        receiveOrientations = false;
    }

private slots:
    void initTestCase() {
        sensor = new QmOrientation();
        QVERIFY(sensor);
        orientation.value = QmOrientation::Undefined;
        orientationChangedFlag = false;
        receiveOrientations = false;

        QVERIFY(connect(sensor, SIGNAL(orientationChanged(const MeeGo::QmOrientationReading)),
                this, SLOT(orientationChanged(const MeeGo::QmOrientationReading))));
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                 sensor->lastError().toLocal8Bit());
        sensor->setStandbyOverride(true);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());
    }

    void testManual() {
        printf("\n");
        printf("This test will instruct you to turn the device into various positions.\n");
        printf("You will have ten seconds to turn the device into each position.\n");
        printf("Get ready! The test starts in 10 seconds...\n");
        QTest::qWait(10000);
        testFunc(QmOrientation::BottomUp);
        testFunc(QmOrientation::BottomDown);
        testFunc(QmOrientation::LeftUp);
        testFunc(QmOrientation::RightUp);
        testFunc(QmOrientation::FaceDown);
        testFunc(QmOrientation::FaceUp);
        printf("\nPlease rotate the device untill the orientation changed, and then rotate back slightly,\n if the orientation changes when you rotate back, FAIL the test!!\n You have 20 seconds to do this\n\n");
        QTest::qWait(10000);
    }
    
    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        delete sensor;
    }

private:
    QmOrientation *sensor;
    QmOrientationReading orientation;
    bool orientationChangedFlag;
    bool receiveOrientations;
};

QTEST_MAIN(TestClass)
#include "manual_orientation.moc"

