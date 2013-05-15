/**
 * @file powerontime.cpp
 * @brief Power on timer tests.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#include <QTest>
#include <QDebug>
#include <QFile>

#include <qmsystemstate.h>

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

private slots:
    void testPowerOnTime() {
        MeeGo::QmSystemState state;
        int tries = 3;
        unsigned int pot = 0;

        while (tries-- > 0) {
            pot = state.getPowerOnTimeInSeconds();
            qDebug() << pot;

            // Wait for the power on time to accumulate
	    QTest::qWait(5000);

	    // cur - prev >= 0 if the timer accumulates as it should
	    int diff = state.getPowerOnTimeInSeconds() - pot;
            QVERIFY(diff >= 0);
        }
    }
};

QTEST_MAIN(TestClass)
#include "powerontime.moc"
