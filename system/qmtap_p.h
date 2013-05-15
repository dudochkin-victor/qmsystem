/*!
 * @file qmtap_p.h
 * @brief Contains QmTapPrivate

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
#ifndef QMTAP_P_H
#define QMTAP_P_H

#include "qmtap.h"
#include "qmsensor_p.h"
#include "sensord/tapsensor_i.h"
#include "sensord/sensormanagerinterface.h"

namespace MeeGo
{

    class QmTapPrivate : public QmSensorPrivate
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmTap);
        DEFINE_GENERIC_FUNCTIONS(QmTap);
    public:
        TapSensorChannelInterface* sensorIfc;

        QmTapPrivate(QmTap *parent) : QmSensorPrivate(parent) {
        }

        ~QmTapPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<Tap>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            remoteSensorManager.loadPlugin("tapsensor");
            remoteSensorManager.registerSensorInterface<TapSensorChannelInterface>("tapsensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return TapSensorChannelInterface::controlInterface("tapsensor");
        }

        const AbstractSensorChannelInterface* listenSession()
        {
            if (!initDone_) { if (!init()) return NULL; }
            return TapSensorChannelInterface::listenInterface("tapsensor");
        }

        bool setupSignals(bool setOn)
        {

            if (sensorIfc == NULL) {
                setError("No session open, unable to (dis)connect signals.");
                return false;
            }

            if (setOn) {

                if (!connect(sensorIfc, SIGNAL(dataAvailable(const Tap&)),
                        this, SLOT(slotTapped(const Tap&)))) {
                    setError("Unable to connect signals");
                    return false;
                }

            } else {
                if (!disconnect(sensorIfc, SIGNAL(dataAvailable(const Tap&)),
                        this, SLOT(slotTapped(const Tap&)))) {
                    setError("Unable to disconnect signals");
                    return false;
                }
            }
            return true;
        }

    Q_SIGNALS:
        void tapped(const MeeGo::QmTapReading);

    public Q_SLOTS:

        void slotTapped(const Tap& tap)
        {
            QmTapReading output;
            output.timestamp = tap.tapData().timestamp_;
            output.direction = (QmTap::Direction)(tap.tapData().direction_);
            output.type = (QmTap::Type)(tap.tapData().type_);

            emit tapped(output);
        }
    };
}
#endif // QMTAP_P_H
