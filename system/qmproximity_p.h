/*!
 * @file qmproximity_p.h
 * @brief Contains QmProximityPrivate

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
#ifndef QMPROXIMITY_P_H
#define QMPROXIMITY_P_H

#include "qmproximity.h"
#include "qmsensor_p.h"
#include "sensord/proximitysensor_i.h"
#include "sensord/sensormanagerinterface.h"

namespace MeeGo
{

    class QmProximityPrivate : public QmSensorPrivate
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmProximity);
        DEFINE_GENERIC_FUNCTIONS(QmProximity);
    public:
        ProximitySensorChannelInterface* sensorIfc;

        QmProximityPrivate(QmProximity *parent) : QmSensorPrivate(parent), sensorIfc(NULL) {
        }

        ~QmProximityPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<Unsigned>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            qDebug() << "Loading plugin: " << remoteSensorManager.loadPlugin("proximitysensor");
            remoteSensorManager.registerSensorInterface<ProximitySensorChannelInterface>("proximitysensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return ProximitySensorChannelInterface::controlInterface("proximitysensor");
        }

        const AbstractSensorChannelInterface* listenSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return ProximitySensorChannelInterface::listenInterface("proximitysensor");
        }

        bool setupSignals(bool setOn)
        {

            if (sensorIfc == NULL) {
                setError("No session open, unable to (dis)connect signals.");
                return false;
            }

            if (setOn) {
                if (!connect(sensorIfc, SIGNAL(dataAvailable(const Unsigned&)),
                        this, SLOT(slotProximityChanged(const Unsigned&)))) {
                    setError("Unable to connect signals");
                    return false;
                }

            } else {
                if (!disconnect(sensorIfc, SIGNAL(dataAvailable(const Unsigned&)),
                        this, SLOT(slotProximityChanged(const Unsigned&)))) {
                    setError("Unable to disconnect signals");
                    return false;
                }
            }
            return true;
        }

    Q_SIGNALS:
        void ProximityChanged(const MeeGo::QmProximityReading value);

    public Q_SLOTS:
        void slotProximityChanged(const Unsigned& value)
        {
            QmProximityReading output;
            output.timestamp = value.UnsignedData().timestamp_;
            output.value = value.UnsignedData().value_;
            emit ProximityChanged(output);
        }
    };

    // ------------------ END PRIVATE CLASS DEFINITION ------------------ //
}
#endif // QMPROXIMITY_P_H
