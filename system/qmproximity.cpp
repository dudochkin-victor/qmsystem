/*!
 * @file qmproximity.cpp
 * @brief QmProximity

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#include "qmproximity.h"
#include "qmproximity_p.h"

#include <QDebug>

namespace MeeGo {


    QmProximity::QmProximity(QObject *parent) : QmSensor(parent)
    {
        QmProximityPrivate *priv = new QmProximityPrivate(this);
        connect(priv, SIGNAL(ProximityChanged(MeeGo::QmProximityReading)), this, SIGNAL(ProximityChanged(MeeGo::QmProximityReading)));
        priv_ptr = priv;
    }

    QmProximity::~QmProximity()
    {
    }

    QmProximityReading QmProximity::get()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return QmProximityReading();
        }
        
        QmProximityPrivate *priv = reinterpret_cast<QmProximityPrivate*>(priv_ptr);
        Unsigned value = priv->sensorIfc->proximity();
        QmProximityReading output;
        output.timestamp = value.UnsignedData().timestamp_;
        output.value = value.UnsignedData().value_;
        return output;
    }

}
