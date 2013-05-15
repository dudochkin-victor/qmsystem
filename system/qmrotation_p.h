/*!
 * @file qmrotation_p.h
 * @brief Contains QmRotationPrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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
#ifndef QMROTATION_P_H
#define QMROTATION_P_H

#include "qmrotation.h"
#include "qmsensor_p.h"
#include "sensord/rotationsensor_i.h"
#include "sensord/sensormanagerinterface.h"
#include <math.h>

namespace MeeGo
{

    class QmRotationPrivate : public QmSensorPrivate
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmRotation);
        DEFINE_GENERIC_FUNCTIONS(QmRotation);

    public:
        RotationSensorChannelInterface* sensorIfc;

        QmRotationPrivate(QmRotation *parent) : QmSensorPrivate(parent), sensorIfc(NULL) {
            pub_ptr = parent;
        }

        ~QmRotationPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<XYZ>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            remoteSensorManager.loadPlugin("rotationsensor");
            remoteSensorManager.registerSensorInterface<RotationSensorChannelInterface>("rotationsensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return RotationSensorChannelInterface::controlInterface("rotationsensor");
        }

        const AbstractSensorChannelInterface* listenSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return RotationSensorChannelInterface::listenInterface("rotationsensor");
        }

        bool setupSignals(bool setOn)
        {
            MEEGO_PUBLIC(QmRotation);
            if(!pub->verifySessionLevel(QmSensor::SessionTypeListen)) {
                return false;
            }

            if (setOn) {
                if (!connect(sensorIfc, SIGNAL(dataAvailable(const XYZ&)),
                             this, SLOT(slotDataAvailable(const XYZ&)))) {
                    setError("Unable to connect signals");
                    return false;
                }

            } else {
                if (!disconnect(sensorIfc, SIGNAL(dataAvailable(const XYZ&)),
                             this, SLOT(slotDataAvailable(const XYZ&)))) {
                    setError("Unable to disconnect signals");
                    return false;
                }
            }
            return true;
        }

    Q_SIGNALS:
        void dataAvailable(const MeeGo::QmRotationReading& data);

    public Q_SLOTS:

        void slotDataAvailable(const XYZ& data)
        {
            QmRotationReading output;
            output.timestamp = data.XYZData().timestamp_;

            // Mangle X to definition...
            if (abs(data.y()) <= 90)
            {
                output.x = -data.y();
            }
            else
            {
                output.x = (data.y()<0 ? 1 : -1) * 180 + data.y();
            }

            // Mangle Y to definition
            if (abs(data.y()) <= 90)
            {
                output.y = data.x();
            }
            else
            {
                output.y = (data.x()>0 ? 1 : -1) * (180-abs(data.x()));
            }

            // ..and finally match z=0 to north.
            output.z = (((data.z() + 180) + 90) % 360) - 180;


            emit dataAvailable(output);
        }
    };


}
#endif // QMROTATION_P_H
