/*!
 * @file qmmagnetometer_p.h
 * @brief Contains QmMagnetometerPrivate

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
#ifndef QMMAGNETOMETER_P_H
#define QMMAGNETOMETER_P_H

#include "qmmagnetometer.h"
#include "qmsensor_p.h"
#include "sensord/magnetometersensor_i.h"
#include "sensord/sensormanagerinterface.h"

namespace MeeGo
{

    class QmMagnetometerPrivate  :public QmSensorPrivate
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmMagnetometer);
        DEFINE_GENERIC_FUNCTIONS(QmMagnetometer);

    public:
        MagnetometerSensorChannelInterface* sensorIfc;

        QmMagnetometerPrivate(QmMagnetometer* parent) : QmSensorPrivate(parent), sensorIfc(NULL) {
            pub_ptr = parent;
        }

        ~QmMagnetometerPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<MagneticField>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            remoteSensorManager.loadPlugin("magnetometersensor");
            remoteSensorManager.registerSensorInterface<MagnetometerSensorChannelInterface>("magnetometersensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return MagnetometerSensorChannelInterface::controlInterface("magnetometersensor");
        }

        const AbstractSensorChannelInterface* listenSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return MagnetometerSensorChannelInterface::listenInterface("magnetometersensor");
        }
        bool setupSignals(bool setOn)
        {
            MEEGO_PUBLIC(QmMagnetometer)
            if(!pub->verifySessionLevel(QmSensor::SessionTypeListen)) {
                return false;
            }

            if (setOn) {
                if (!connect(sensorIfc, SIGNAL(dataAvailable(const MagneticField&)),
                             this, SLOT(slotDataAvailable(const MagneticField&)))) {
                    setError("Unable to connect signals");
                    return false;
                }

            } else {
                if (!disconnect(sensorIfc, SIGNAL(dataAvailable(const MagneticField&)),
                             this, SLOT(slotDataAvailable(const MagneticField&)))) {
                    setError("Unable to disconnect signals");
                    return false;
                }
            }
            return true;
        }

    Q_SIGNALS:
        void dataAvailable(const MeeGo::QmMagnetometerReading &data);

        public Q_SLOTS:

        void slotDataAvailable(const MagneticField& data)
        {
            QmMagnetometerReading output;
            output.x = data.data().x_;
            output.y = data.data().y_;
            output.z = data.data().z_;
            output.rx = data.data().rx_;
            output.ry = data.data().ry_;
            output.rz = data.data().rz_;
            output.timestamp = data.data().timestamp_;
            output.level = data.data().level_;

            emit dataAvailable(output);
        }
    };


}
#endif // QMMAGNETOMETER_P_H
