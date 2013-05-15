/**
 * @file activity.cpp
 * @brief QmActivity tests

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
#include <qmactivity.h>
#include <QTest>

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

public slots:
    void activityChanged(MeeGo::QmActivity::Activity){}
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmActivity *activity;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        activity = new MeeGo::QmActivity();
        QVERIFY(activity);
    }

    void testConnectSignals() {
        QVERIFY(connect(activity, SIGNAL(activityChanged(MeeGo::QmActivity::Activity)),
                        &signalDump, SLOT(activityChanged(MeeGo::QmActivity::Activity))));
    }

    void testGet() {
        MeeGo::QmActivity::Activity result = activity->get();
        (void)result;
    }

    void cleanupTestCase() {
        delete activity;
    }
};

QTEST_MAIN(TestClass)
#include "activity.moc"
