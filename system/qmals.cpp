/*!
 * @file qmals.cpp
 * @brief QmAls

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

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
#include "qmals.h"
#include "qmals_p.h"
#include <QDebug>

namespace MeeGo {


    QmALS::QmALS(QObject *parent) : QmSensor(parent)
    {
        QmALSPrivate* priv = new QmALSPrivate(this);
        connect(priv, SIGNAL(ALSChanged(MeeGo::QmAlsReading)), this, SIGNAL(ALSChanged(MeeGo::QmAlsReading)));
        priv_ptr = priv;
    }

    QmALS::~QmALS()
    {
    }


    QmAlsReading QmALS::get()
    {
        if(!verifySessionLevel(QmSensor::SessionTypeListen)) {
            return QmAlsReading();
        }
        QmALSPrivate *priv = reinterpret_cast<QmALSPrivate*>(priv_ptr);
        Unsigned value = priv->sensorIfc->lux();
        QmAlsReading output;
        output.value = value.UnsignedData().value_;
        output.timestamp = value.UnsignedData().timestamp_;
        return output;
    }

}
