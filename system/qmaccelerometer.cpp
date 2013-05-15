/*!
 * @file qmaccelerometer.cpp
 * @brief QmAccelerometer

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
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
#include "qmaccelerometer.h"
#include "qmaccelerometer_p.h"


namespace MeeGo {
    QmAccelerometer::QmAccelerometer(QObject *parent) : QmSensor(parent)
    {
        QmAccelerometerPrivate *priv = new QmAccelerometerPrivate(this);
        connect(priv, SIGNAL(dataAvailable(MeeGo::QmAccelerometerReading)), this, SIGNAL(dataAvailable(MeeGo::QmAccelerometerReading)));
        priv_ptr = priv;
    }

    QmAccelerometer::~QmAccelerometer()
    {

    }
}
