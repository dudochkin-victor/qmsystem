/*!
 * @file qmaccelerometer_.h
 * @brief Contains QmAccelerometerPrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas.nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>

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
#ifndef QMACCELEROMETER_P_H
#define QMACCELEROMETER_P_H

#include "qmaccelerometer.h"
#include "qmsensor_p.h"
#include "sensord/accelerometersensor_i.h"
#include "sensord/sensormanagerinterface.h"

namespace MeeGo
{
    class QmAccelerometerPrivate : public QmSensorPrivate
    {
        Q_OBJECT;

        MEEGO_DECLARE_PUBLIC(QmAccelerometer);
        DEFINE_GENERIC_FUNCTIONS(QmAccelerometer);

    public:
        AccelerometerSensorChannelInterface* sensorIfc;

        QmAccelerometerPrivate(QmAccelerometer *parent) : QmSensorPrivate(parent), sensorIfc(NULL) {
            pub_ptr = parent;
        }

        ~QmAccelerometerPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<XYZ>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            remoteSensorManager.loadPlugin("accelerometersensor");
            remoteSensorManager.registerSensorInterface<AccelerometerSensorChannelInterface>("accelerometersensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return AccelerometerSensorChannelInterface::controlInterface("accelerometersensor");
        }

        const AbstractSensorChannelInterface* listenSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return AccelerometerSensorChannelInterface::listenInterface("accelerometersensor");
        }

        bool setupSignals(bool setOn)
        {
            MEEGO_PUBLIC(QmAccelerometer);
            if(!pub->verifySessionLevel(QmSensor::SessionTypeListen)) {
                return false;
            }
            if (setOn) {
                if (!connect(sensorIfc, SIGNAL(dataAvailable(const XYZ)), this, SLOT(slotDataAvailable(XYZ))))

                {
                    setError("Unable to connect signals");
                    return false;
                }
            } else {
                if (!disconnect(sensorIfc, SIGNAL(dataAvailable(const XYZ)), this, SLOT(slotDataAvailable(XYZ))))
                {
                    setError("Unable to disconnect signals");
                    return false;
                }
            }
            return true;
        }

    Q_SIGNALS:

        void dataAvailable(const MeeGo::QmAccelerometerReading& data);

    public Q_SLOTS:

        void slotDataAvailable(const XYZ& data)
        {
            QmAccelerometerReading output;
            output.timestamp = data.XYZData().timestamp_;
            output.x = -data.y();
            output.y = data.x();
            output.z = data.z();

            emit dataAvailable(output);
        }
    };
}
#endif // QMACCELEROMETER_P_H
