/*!
 * @file qmorientation.h
 * @brief Contains QmOrientationPrivate

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
#ifndef QMORIENTATION_P_H
#define QMORIENTATION_P_H

#include "qmorientation.h"

#include "sensord/orientationsensor_i.h"
#include "sensord/sensormanagerinterface.h"
#include "datatypes/posedata.h"
#include "qmsensor_p.h"

namespace MeeGo
{


    class QmOrientationPrivate : public QmSensorPrivate
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmOrientation);
        DEFINE_GENERIC_FUNCTIONS(QmOrientation);
    public:
        OrientationSensorChannelInterface* sensorIfc;

        QmOrientationPrivate(QmOrientation *parent) : QmSensorPrivate(parent), sensorIfc(NULL) {
        }

        ~QmOrientationPrivate() {
            closeSession();
        }

        bool init()
        {
            qDBusRegisterMetaType<Unsigned>();
            SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
            remoteSensorManager.loadPlugin("orientationsensor");
            remoteSensorManager.registerSensorInterface<OrientationSensorChannelInterface>("orientationsensor");
            initDone_ = true;
            return true;
        }

        AbstractSensorChannelInterface* controlSession() {
            if (!initDone_) { if (!init()) return NULL; }
            return OrientationSensorChannelInterface::controlInterface("orientationsensor");
        }

        const AbstractSensorChannelInterface* listenSession() {
            if (!initDone_) { if (!init()) return NULL; }
            return OrientationSensorChannelInterface::listenInterface("orientationsensor");
        }

        bool setupSignals(bool setOn)
        {
            if (sensorIfc == NULL) {
                setError("No session open, unable to (dis)connect signals.");
                return false;
            }

            if (setOn) {
                if( !connect(sensorIfc, SIGNAL(orientationChanged(const Unsigned&)),
                             this, SLOT(slotOrientationChanged(const Unsigned&)))) {
                    setError("Signal connect error");
                    return false;
                }

            } else {
                if( !disconnect(sensorIfc, SIGNAL(orientationChanged(const Unsigned&)),
                             this, SLOT(slotOrientationChanged(const Unsigned&)))) {
                    setError("Signal disconnect error");
                    return false;
                }
            }
            return true;
        }

        QmOrientation::Orientation poseDataToOrientation(PoseData::Orientation input)
        {
            switch (input) {
                case PoseData::LeftUp:
                    return QmOrientation::BottomUp;
                case PoseData::RightUp:
                    return QmOrientation::BottomDown;
                case PoseData::BottomUp:
                    return QmOrientation::RightUp;
                case PoseData::BottomDown:
                    return QmOrientation::LeftUp;
                case PoseData::FaceDown:
                    return QmOrientation::FaceDown;
                case PoseData::FaceUp:
                    return QmOrientation::FaceUp;
                default:
                    return QmOrientation::Undefined;
            }
        }

        QmOrientationReading orientation()
        {
            Unsigned value = sensorIfc->orientation();
            QmOrientationReading output;
            output.value = poseDataToOrientation((PoseData::Orientation)(value.UnsignedData().value_));
            output.timestamp = value.UnsignedData().timestamp_;
            return output;
        }

    Q_SIGNALS:
        void orientationChanged(const MeeGo::QmOrientationReading orientation);

    public Q_SLOTS:
        void slotOrientationChanged(const Unsigned& orientation)
        {
            QmOrientationReading output;
            output.value = poseDataToOrientation((PoseData::Orientation)orientation.UnsignedData().value_);
            output.timestamp = orientation.UnsignedData().timestamp_;
            emit orientationChanged(output);
        }
    };

    // ------------------ END PRIVATE CLASS DEFINITION ------------------ //

}
#endif // QMORIENTATION_P_H
