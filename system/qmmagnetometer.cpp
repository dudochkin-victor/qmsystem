/*!
 * @file qmmagnetometer.cpp
 * @brief QmMagnetometer

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Üstün Ergenoglu <ustun.ergenoglu@digia.com>

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
#include "qmmagnetometer.h"
#include "qmmagnetometer_p.h"

#include <QDebug>

namespace MeeGo {

    QmMagnetometer::QmMagnetometer(QObject *parent) : QmSensor(parent)
    {
        QmMagnetometerPrivate *priv = new QmMagnetometerPrivate(this);
        connect(priv, SIGNAL(dataAvailable(MeeGo::QmMagnetometerReading)), this, SIGNAL(dataAvailable(MeeGo::QmMagnetometerReading)));
        priv_ptr = priv;
    }

    QmMagnetometer::~QmMagnetometer()
    {

    }

    QmMagnetometerReading QmMagnetometer::magneticField()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return QmMagnetometerReading();
        }

        QmMagnetometerPrivate *priv = reinterpret_cast<QmMagnetometerPrivate*>(priv_ptr);
        MagneticField value = priv->sensorIfc->magneticField();
        QmMagnetometerReading output;
        output.x = value.data().x_;
        output.y = value.data().y_;
        output.z = value.data().z_;
        output.rx = value.data().rx_;
        output.ry = value.data().ry_;
        output.rz = value.data().rz_;
        output.timestamp = value.data().timestamp_;
        output.level = value.data().level_;
        return output;
    }

    void QmMagnetometer::reset() {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return;
        }

        QmMagnetometerPrivate *priv = reinterpret_cast<QmMagnetometerPrivate*>(priv_ptr);
        priv->sensorIfc->reset();

    }

}
