/*!
 * @file qmcompass.cpp
 * @brief QmCompass

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com>
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
#include "qmcompass.h"
#include "qmcompass_p.h"

#include <QDebug>

namespace MeeGo {

    QmCompass::QmCompass(QObject *parent) : QmSensor(parent)
    {
        QmCompassPrivate *priv = new QmCompassPrivate(this);
        priv_ptr = priv;
        connect(priv, SIGNAL(dataAvailable(MeeGo::QmCompassReading)), this, SIGNAL(dataAvailable(MeeGo::QmCompassReading)));
    }

    QmCompass::~QmCompass()
    {

    }


    QmCompassReading QmCompass::get()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return QmCompassReading();
        }
        QmCompassPrivate *priv = reinterpret_cast<QmCompassPrivate*>(priv_ptr);
        Compass value = priv->sensorIfc->get();
        QmCompassReading output;
        output.timestamp = value.data().timestamp_;
        output.degrees = value.data().degrees_;
        output.level = value.data().level_;
        return output;
    }

    int QmCompass::declinationValue()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return -1;
        }
        QmCompassPrivate *priv = reinterpret_cast<QmCompassPrivate*>(priv_ptr);
        return priv->sensorIfc->declinationValue();
    }

    bool QmCompass::useDeclination()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return false;
        }
        QmCompassPrivate *priv = reinterpret_cast<QmCompassPrivate*>(priv_ptr);
        return priv->sensorIfc->useDeclination();
    }

    void QmCompass::setUseDeclination(bool enable)
    {
        if(!verifySessionLevel(QmSensor::SessionTypeControl)) {
            return;
        }
        QmCompassPrivate *priv = reinterpret_cast<QmCompassPrivate*>(priv_ptr);
        priv->sensorIfc->setUseDeclination(enable);
    }

}
