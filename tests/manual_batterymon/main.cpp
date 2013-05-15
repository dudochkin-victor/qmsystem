/**
 * @file main.cpp
 * @brief Battery monitor manual test

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

#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <qmbattery.h>

static QString ts()
{
    uint ts = QDateTime::currentDateTime().toTime_t();
    return QString("[%1]").arg(ts);
}

class BatteryMonitor : public QObject {
    Q_OBJECT

public slots:
    void slotChargingStateChanged(MeeGo::QmBattery::ChargingState chargingState) {
        if (chargingState == MeeGo::QmBattery::StateNotCharging) {
            qDebug() << ts() << "Charger disconnected";
        } else if (chargingState == MeeGo::QmBattery::StateCharging) {
            qDebug() << ts() << "Charger connected";
        } else if (chargingState == MeeGo::QmBattery::StateChargingFailed) {
            qDebug() << ts() << "Charger connected but failed to charge";
        } else {
            qDebug() << ts() << "Error: unknown charging state";
        }
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    BatteryMonitor batteryMonitor;

    qDebug() << ts() << "Initializing QmBattery";
    MeeGo::QmBattery battery;
    qDebug() << ts() << "QmBattery initialized";

    qDebug() << ts() << "Trying to connect the chargingStateChanged signal";
    if (!QObject::connect(&battery, SIGNAL(chargingStateChanged(MeeGo::QmBattery::ChargingState)), &batteryMonitor,
                                    SLOT(slotChargingStateChanged(MeeGo::QmBattery::ChargingState)))) {
        qDebug() << ts() << "Signal connecting failed. Exit.";
        QCoreApplication::exit(EXIT_FAILURE);
    }

    qDebug() << ts() << "Signal connected, entering main loop. Use Ctrl+C to quit.";
    return app.exec();
}

#include "main.moc"
