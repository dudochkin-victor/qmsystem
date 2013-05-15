/*!
 * @file qmbattery_p.h
 * @brief Contains Polling thread used to poll on libbme-ipc sockets

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Aliaksei Katovich <aliaksei.katovich@nokia.com>
   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Matti Halme <matthalm@esdhcp04350.research.nokia.com>
   @author Na Fan <ext-na.2.fan@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

   @scope Private

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

#ifndef QMBATTERY_P_H
#define QMBATTERY_P_H

#include <QtCore/qobject.h>
#include <QThread>
#include <QDateTime>
#include <QScopedPointer>
#include <QTimer>

#include <mqueue.h>
#include <time.h>
#include <sys/time.h>
#include <sys/poll.h>
#include "qmbattery.h"

extern "C" {
#include "bme/bmeipc.h"
}

namespace MeeGo {

class EmIpc;
class EmEvents;
class EmCurrentMeasurement;

class QmBatteryPrivate : public QObject
{
    Q_OBJECT
    MEEGO_DECLARE_PUBLIC(QmBattery)

public:
    QmBatteryPrivate();
    ~QmBatteryPrivate();

    bool init(QmBattery*);

    bool startCurrentMeasurement(QmBattery::Period);
    bool stopCurrentMeasurement();

    int getStat(int) const;
    int getCumulativeBatteryCurrent();
    int getAverageCurrent(int usageMode, QmBattery::RemainingTimeMode psMode,
			  int defaultCurrent) const;
    int getRemainingTime(int usageMode, QmBattery::RemainingTimeMode psMode,
			 int defaultCurrent) const;

private Q_SLOTS:
    void onEmEvent(int);
    void onMeasurement(int);
    void waitForUSB500mA();

private:
    void queryStat_() const;
    void emitEventBatmon_();
    void saveStat_();

    int makeUsetimeQuery(const QString& method, int usageMode,
			 QmBattery::RemainingTimeMode psMode) const;

    QmBattery *parent_;

    mutable bmestat_t stat_;
    mutable bool is_data_actual_;
    mutable QDateTime cache_expire_;

    mutable int cc_offset_;
    mutable int prev_cc_restart_count_;
    bmestat_t saved_stat_;

    QScopedPointer<EmIpc> ipc_;
    QScopedPointer<EmEvents> events_;
    QScopedPointer<EmCurrentMeasurement> measurements_;
    QTimer *timer;
    int usb100ma_emit_delayed;
};

} /* MeeGo */
#endif /* QMBATTERY_P_H */
