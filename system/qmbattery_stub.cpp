/*!
 * @file qmbattery_stub.cpp
 * @brief Used as a stub implementation for QmBattery, if bmeipc is not available.

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

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
#include "qmbattery.h"

#include <QDebug>

namespace MeeGo
{
    class QmBatteryPrivate {
    public:
        QmBatteryPrivate() {}
        ~QmBatteryPrivate() {}
    };

    QmBattery::QmBattery(QObject *parent)
    {
        qWarning() << "QmBattery is not functional because of a missing compile-time dependency. Please compile QmSystem with bmeipc.";
    }

    QmBattery::~QmBattery()
    {
    }

    int QmBattery::getNominalCapacity() const
    {
        return 0;
    }

    QmBattery::BatteryState QmBattery::getBatteryState() const
    {
        return QmBattery::StateError;
    }

    int QmBattery::getRemainingCapacitymAh() const
    {
        return 0;
    }

    int QmBattery::getRemainingCapacityPct() const
    {
        return 0;
    }

    int QmBattery::getRemainingCapacityBars() const
    {
        return 0;
    }

    int QmBattery::getMaxBars() const
    {
        return 0;
    }

    int QmBattery::getVoltage() const
    {
        return 0;
    }

    int QmBattery::getBatteryCurrent() const
    {
        return 0;
    }

    int QmBattery::getCumulativeBatteryCurrent() const
    {
        return 0;
    }

    QmBattery::ChargerType QmBattery::getChargerType() const
    {
        return QmBattery::Unknown;
    }

    QmBattery::ChargingState QmBattery::getChargingState() const
    {
        return QmBattery::StateChargingFailed;
    }

    int QmBattery::getRemainingChargingTime() const
    {
        return 0;
    }

    bool QmBattery::startCurrentMeasurement(Period)
    {
        return false;
    }

    bool QmBattery::stopCurrentMeasurement()
    {
        return false;
    }

    int QmBattery::getRemainingTalkTime(RemainingTimeMode) const
    {
        return 0;
    }

    int QmBattery::getRemainingIdleTime(RemainingTimeMode mode) const
    {
        return 0;
    }

    QmBattery::BatteryCondition QmBattery::getBatteryCondition() const
    {
        return QmBattery::ConditionUnknown;
    }

    int QmBattery::getBatteryEnergyLevel() const
    {
        return 0;
    }

    QmBattery::Level QmBattery::getLevel() const
    {
        return QmBattery::LevelFull;
    }

    QmBattery::State QmBattery::getState() const
    {
        return QmBattery::StateChargingFailed;
    }
}
