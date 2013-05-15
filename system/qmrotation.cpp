/*!
 * @file qmrotation.cpp
 * @brief QmRotation

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

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
#include "qmrotation.h"
#include "qmrotation_p.h"

#include <QDebug>

namespace MeeGo {

    QmRotation::QmRotation(QObject *parent) : QmSensor(parent)
    {
        QmRotationPrivate *priv = new QmRotationPrivate(this);
        connect(priv, SIGNAL(dataAvailable(const MeeGo::QmRotationReading&)), this, SIGNAL(dataAvailable(const MeeGo::QmRotationReading&)));
        priv_ptr = priv;
    }

    QmRotation::~QmRotation()
    {
    }

    QmRotationReading QmRotation::rotation()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return QmRotationReading();
        }

        QmRotationPrivate *priv = reinterpret_cast<QmRotationPrivate*>(priv_ptr);

        QmRotationReading output;
        XYZ data = priv->sensorIfc->rotation();
        output.timestamp = data.XYZData().timestamp_;
        output.x = data.x();
        output.y = data.y();
        output.z = data.z();
        return output;
    }

    bool QmRotation::hasZ()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return false;
        }
        QmRotationPrivate *priv = reinterpret_cast<QmRotationPrivate*>(priv_ptr);
        return priv->sensorIfc->hasZ();
    }
}
