/**
 * @file systeminformation.cpp
 * @brief System Information tests.

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
#include <QFile>

#include <qmsysteminformation.h>

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

private slots:
    void testSystemInformation() {
        MeeGo::QmSystemInformation systemInformation;

        QString v1 = systemInformation.valueForKey("/device/sw-release-ver");
        QString v2 = systemInformation.valueForKey("/device/sw-release-ver");
        QString v3 = systemInformation.valueForKey("/device/sw-release-ver");

        QVERIFY(v1.length() > 5);

        /* SysInfo data is static, so we must get the same value every time we query a key */
        QVERIFY(v1.compare(v2) == 0);
        QVERIFY(v1.compare(v3) == 0);
    }
};

QTEST_MAIN(TestClass)
#include "systeminformation.moc"
