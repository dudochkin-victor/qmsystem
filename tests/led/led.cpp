/**
 * @file led.cpp
 * @brief QmLED tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Sagar Shinde <ext-sagar.shinde@nokia.com>

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

    void testEnable() {
        bool result = led->enable();
        QVERIFY(result == true);
    }

    void testDisable() {
        bool result = led->disable();
        QVERIFY(result == true);
    }

    void testActivate() {
        bool result = led->activate("PatternCommunicationCall");
        QVERIFY(result == true);
    }

    void testDeactivate() {
        bool result = led->deactivate("PatternCommunicationCall");
        QVERIFY(result == true);
    }

    void cleanupTestCase() {
        delete led;
    }
};

QTEST_MAIN(TestClass)
#include "led.moc"
