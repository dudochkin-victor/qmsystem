/*!
 * @file qmals_p.h
 * @brief Contains QmALSPrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

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
#ifndef QMALS_P_H
#define QMALS_P_H

#include "qmals.h"
#include "qmsensor_p.h"
#include "sensord/alssensor_i.h"
#include "sensord/sensormanagerinterface.h"

namespace MeeGo
{

    class QmALSPrivate : public QmSensorPrivate
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmALS);
        DEFINE_GENERIC_FUNCTIONS(QmALS);
    public:
        ALSSensorChannelInterface* sensorIfc;

        QmALSPrivate(QmALS *parent) : QmSensorPrivate(parent), sensorIfc(NULL) {
            pub_ptr = parent;
        }

        ~QmALSPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<Unsigned>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            remoteSensorManager.loadPlugin("alssensor");
            remoteSensorManager.registerSensorInterface<ALSSensorChannelInterface>("alssensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return ALSSensorChannelInterface::controlInterface("alssensor");
        }

        const AbstractSensorChannelInterface* listenSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return ALSSensorChannelInterface::listenInterface("alssensor");
        }

        bool setupSignals(bool setOn)
        {
            if (sensorIfc == NULL) {
                setError("No session open, unable to (dis)connect signals.");
                return false;
            }

            if (setOn) {

                if (!connect(sensorIfc, SIGNAL(ALSChanged(const Unsigned&)),
                             this, SLOT(slotALSChanged(const Unsigned&))))
                {
                    setError("Unable to connect signals");
                    return false;
                }

            } else {
                if (!disconnect(sensorIfc, SIGNAL(ALSChanged(const Unsigned&)),
                                this, SLOT(slotALSChanged(const Unsigned&))))
                {
                    setError("Unable to disconnect signals");
                    return false;
                }
            }

            return true;
        }

    Q_SIGNALS:
        void ALSChanged(const MeeGo::QmAlsReading data);

    public Q_SLOTS:
        void slotALSChanged(const Unsigned& value)
        {
            QmAlsReading output;
            output.timestamp = value.UnsignedData().timestamp_;
            output.value = value.UnsignedData().value_;
            emit ALSChanged(output);
        }
    };

}

#endif // QMALS_P_H
