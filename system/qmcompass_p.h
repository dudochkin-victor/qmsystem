/*!
 * @file qmcompass_p.h
 * @brief Contains QmCompassPrivate

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
#ifndef QMCOMPASS_P_H
#define QMCOMPASS_P_H

#include "qmcompass.h"
#include "qmsensor.h"
#include "qmsensor_p.h"
#include "sensord/compasssensor_i.h"
#include "sensord/sensormanagerinterface.h"

namespace MeeGo
{

    // ----------------- BEGIN PRIVATE CLASS DEFINITION ----------------- //

    class QmCompassPrivate : public QmSensorPrivate
    {
        Q_OBJECT;

        MEEGO_DECLARE_PUBLIC(QmCompass);
        DEFINE_GENERIC_FUNCTIONS(QmCompass);
    public:
        CompassSensorChannelInterface* sensorIfc;

        QmCompassPrivate(QmCompass *compass) : QmSensorPrivate(compass), sensorIfc(NULL)
        {
        }

        ~QmCompassPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<Compass>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            remoteSensorManager.loadPlugin("compasssensor");
            remoteSensorManager.registerSensorInterface<CompassSensorChannelInterface>("compasssensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return CompassSensorChannelInterface::controlInterface("compasssensor");
        }

        const AbstractSensorChannelInterface* listenSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return CompassSensorChannelInterface::listenInterface("compasssensor");
        }


        bool setupSignals(bool setOn)
        {
            if (sensorIfc == NULL) {
                setError("No session open, unable to (dis)connect signals.");
                return false;
            }

            if (setOn) {
                bool result = connect(sensorIfc, SIGNAL(dataAvailable(const Compass&)),
                                      this, SLOT(slotDataAvailable(const Compass&)));

                if (!result) {
                    setError("Signal connect error");
                    return false;
                }

            } else {
                bool result = disconnect(sensorIfc, SIGNAL(dataAvailable(const Compass&)),
                                  this, SLOT(slotDataAvailable(const Compass&)));

                if (!result) {
                    setError("Signal disconnect error");
                    return false;
                }
            }
            return true;
        }

    Q_SIGNALS:
        void dataAvailable(const MeeGo::QmCompassReading value);

    public Q_SLOTS:

        void slotDataAvailable(const Compass& value)
        {
            QmCompassReading output;
            output.timestamp = value.data().timestamp_;
            output.degrees = (value.data().degrees_ + 90) % 360;
            output.level = value.data().level_;
            emit dataAvailable(output);
        }
    };

    // ------------------ END PRIVATE CLASS DEFINITION ------------------ //

}
#endif // QMCOMPASS_P_H
