/*!
 * @file qmsensor.cpp
 * @brief QmSensor

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>ngas@nokia.com>

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
#include "qmsensor.h"
#include "qmsensor_p.h"
#include "system_global.h"
#include "sensord/sensormanagerinterface.h"
#include <QDebug>

#define GET_SENSOR_PTR_PTR(name) AbstractSensorChannelInterface** name = getSensorIfcPtr();
#define GET_SENSOR_PTR(name) AbstractSensorChannelInterface* name = *getSensorIfcPtr();
#define GET_PRIVATE_PTR(name) QmSensorPrivate* name = (QmSensorPrivate*)getPrivatePtr();
#define GET_PUBLIC_PTR(name) QmSensor* name = getPublicPtr();

namespace MeeGo {

    // ----------------- BEGIN PRIVATE CLASS DEFINITION ----------------- //

    QmSensorPrivate::QmSensorPrivate(QmSensor *sensor) : QObject(sensor), sessionType_(QmSensor::SessionTypeNone), initDone_(false), running_(false)
    {
        connect(this, SIGNAL(errorSignal(QString)), sensor, SIGNAL(errorSignal(QString)));
    }

    QmSensorPrivate::~QmSensorPrivate() {}

    QmSensor::SessionType QmSensorPrivate::sessionType()
    {
        return sessionType_;
    }

    QmSensor::SessionType QmSensorPrivate::requestSession(QmSensor::SessionType type) {

        if (!initDone_) {
            if (!init()) return QmSensor::SessionTypeNone;
        }

        GET_SENSOR_PTR_PTR(sensorIfcPtr);

        // XXX: Dies on assert if fails - can we get the error here in proper way?
        SensorManagerInterface& remoteSensorManager = SensorManagerInterface::instance();
        if (!(remoteSensorManager.isValid())) {
            setError("Unable to connect to SensorManager");
            return QmSensor::SessionTypeNone;
        }

        // Close any previous open session
        if (sessionType_ != QmSensor::SessionTypeNone) {
            closeSession();
        }

        if (type == QmSensor::SessionTypeNone) {
            return QmSensor::SessionTypeNone;
        }

        do {
            switch (type) {
                case QmSensor::SessionTypeControl:
                {
                    *sensorIfcPtr = controlSession();
                    if (*sensorIfcPtr != NULL) {
                        sessionType_ = QmSensor::SessionTypeControl;
                    } else {
                        setError(remoteSensorManager.errorString());
                        type = QmSensor::SessionTypeListen;
                    }
                    break;
                }
                case QmSensor::SessionTypeListen:
                {
                    *sensorIfcPtr = const_cast<AbstractSensorChannelInterface*>(listenSession());
                    if (*sensorIfcPtr) {
                        sessionType_ = QmSensor::SessionTypeListen;
                    } else {
                        setError(remoteSensorManager.errorString());
                        type = QmSensor::SessionTypeNone;
                    }
                    break;
                }
                case QmSensor::SessionTypeNone:
                default:
                {
                    // Error is set in the situation where is has occurred.
                    break;
                }
            }
        } while (type != QmSensor::SessionTypeNone && *sensorIfcPtr == NULL);

        return type;
    }

    void QmSensorPrivate::closeSession()
    {
        GET_SENSOR_PTR_PTR(sensorIfc);
        if (*sensorIfc) {
            stop();
            delete *sensorIfc;
            *sensorIfc = NULL;
        }
        sessionType_ = QmSensor::SessionTypeNone;
    }

    bool QmSensorPrivate::start()
    {
        GET_SENSOR_PTR(sensorIfc);
        if (sensorIfc) {
            // XXX: Check for valid D-Bus reply, set error.
            sensorIfc->start();
        } else {
            setError("Unable to start, no open session");
            return false;
        }
        return true;
    }

    bool QmSensorPrivate::stop()
    {
        GET_SENSOR_PTR(sensorIfc);
        if (sensorIfc) {
            // XXX: Check for valid D-Bus reply, set error.
            sensorIfc->stop();
        } else {
            setError("Unable to stop, no open session");
            return false;
        }
        return true;
    }

    int QmSensorPrivate::interval()
    {
        GET_SENSOR_PTR(sensorIfc);
        if (sensorIfc) {
            return sensorIfc->interval();
        }
        return 0;
    }

    void QmSensorPrivate::setInterval(int value)
    {
        GET_SENSOR_PTR(sensorIfc);
        if (sensorIfc) {
            sensorIfc->setInterval(value);
        }
    }

    bool QmSensorPrivate::standbyOverride()
    {
        GET_SENSOR_PTR(sensorIfc);
        if (sensorIfc) {
            return sensorIfc->standbyOverride();
        }
        return false;
    }

    void QmSensorPrivate::setStandbyOverride(bool value)
    {
        GET_SENSOR_PTR(sensorIfc);
        if (sensorIfc) {
            sensorIfc->setStandbyOverride(value);
        }
    }

    void QmSensorPrivate::setError(QString error)
    {
        errorString_ = error;
        emit errorSignal(errorString_);
    }

    // ------------------ END PRIVATE CLASS DEFINITION ------------------ //

    QmSensor::QmSensor(QObject *parent = 0) : QObject(parent)
    {
    }

    QmSensor::~QmSensor()
    {
        MEEGO_UNINITIALIZE(QmSensor);
    }

    QmSensor::SessionType QmSensor::sessionType()
    {
        MEEGO_PRIVATE(QmSensor);
        return priv->sessionType();
    }

    bool QmSensor::start() {
        MEEGO_PRIVATE(QmSensor);
        if (priv->running_) return true;
        if (priv->start()) {
            priv->running_ = true;
            priv->setupSignals(true);
            return true;
        }
        return false;
    }

    bool QmSensor::stop()
    {
        MEEGO_PRIVATE(QmSensor);
        if (!priv->running_) return true;

        if (priv->stop()) {
            priv->running_ = false;

            // Unbind signals, in case another listener keeps session open
            priv->setupSignals(false);
            return true;
        }
        return false;
    }

    bool QmSensor::isRunning()
    {
        MEEGO_PRIVATE(QmSensor);
        return priv->running_;
    }

    QmSensor::SessionType QmSensor::requestSession(SessionType type)
    {
        MEEGO_PRIVATE(QmSensor);

        stop();
        priv->requestSession(type);

        return sessionType();
    }

    void QmSensor::closeSession()
    {
        stop();
    }

    bool QmSensor::verifySessionLevel(QmSensor::SessionType type)
    {
        return ((bool)(type <= sessionType()));
    }

    QString QmSensor::lastError() const
    {
        MEEGO_PRIVATE_CONST(QmSensor);
        return priv->errorString_;
    }

    int QmSensor::interval()
    {
        MEEGO_PRIVATE(QmSensor);
        return priv->interval();
    }

    void QmSensor::setInterval(int value)
    {
        MEEGO_PRIVATE(QmSensor);
        priv->setInterval(value);
    }

    bool QmSensor::standbyOverride()
    {
        MEEGO_PRIVATE(QmSensor);
        return priv->standbyOverride();
    }

    void QmSensor::setStandbyOverride(bool value)
    {
        MEEGO_PRIVATE(QmSensor);
        priv->setStandbyOverride(value);
    }
}
