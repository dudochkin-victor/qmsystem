/*!
 * @file qmorientation.cpp
 * @brief QmOrientation

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
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
#include "qmorientation.h"
#include "qmorientation_p.h"

#include <QDebug>

namespace MeeGo {

    QmOrientation::QmOrientation(QObject *parent) : QmSensor(parent)
    {
        QmOrientationPrivate *priv = new QmOrientationPrivate(this);
        connect(priv, SIGNAL(orientationChanged(MeeGo::QmOrientationReading)), this, SIGNAL(orientationChanged(MeeGo::QmOrientationReading)));
        priv_ptr = priv;
    }

    QmOrientation::~QmOrientation()
    {
    }

    QmOrientationReading QmOrientation::orientation()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return QmOrientationReading();
        }

        QmOrientationPrivate *priv = reinterpret_cast<QmOrientationPrivate*>(priv_ptr);
        return priv->orientation();
    }

    int QmOrientation::threshold()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return -1;
        }
        QmOrientationPrivate *priv = reinterpret_cast<QmOrientationPrivate*>(priv_ptr);
        return priv->sensorIfc->threshold();
    }

    void QmOrientation::setThreshold(int value)
    {
        if(!verifySessionLevel(QmSensor::SessionTypeControl)) {
            return;
        }
        QmOrientationPrivate *priv = reinterpret_cast<QmOrientationPrivate*>(priv_ptr);
        priv->sensorIfc->setThreshold(value);
    }

}
