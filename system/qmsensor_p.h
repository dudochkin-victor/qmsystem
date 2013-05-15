/*!
 * @file qmsensor_p.h
 * @brief Contains QmSensorPrivate

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
#ifndef QMSENSOR_P_H
#define QMSENSOR_P_H

#include "sensord/abstractsensor_i.h"
#include "qmsensor.h"

#define DEFINE_GENERIC_FUNCTIONS(Class) \
        private: \
        AbstractSensorChannelInterface** getSensorIfcPtr() \
        { \
            return (AbstractSensorChannelInterface**)&sensorIfc; \
        } \
        \
        QmSensor* getPublicPtr() \
        { \
            MEEGO_PUBLIC(Class); \
            return pub; \
        }

namespace MeeGo 
{
    class QmSensorPrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmSensor)

    public:

        QmSensorPrivate(QmSensor *parent);
        ~QmSensorPrivate();

        QmSensor::SessionType sessionType();
        QmSensor::SessionType requestSession(QmSensor::SessionType type);
        void closeSession();

        virtual bool start();
        virtual bool stop();

        int interval();
        void setInterval(int value);

        bool standbyOverride();
        void setStandbyOverride(bool value);

    Q_SIGNALS:
        void errorSignal(QString error);

    protected:

        /**
         * Initaliases the plugins and datatypes required for the sensor.
         *
         * @return \c true on success, \c false on failure.
         */
        virtual bool init() = 0;
        /**
        * Returns a base class pointer to the SensorChannelInterface held by the
        * child class.
        *
        * Do not implement this directly, but add macro
        * \c DEFINE_GENERIC_FUNCTIONS to the child class definition.
        */
        virtual AbstractSensorChannelInterface** getSensorIfcPtr() = 0;

        /**
         * Returns a pointer to the public class that has instantiated the child
         * of this class.
         *
         * Do not implement this directly, but add macro
         * \c DEFINE_GENERIC_FUNCTIONS to the child class definition.
         */
         virtual QmSensor* getPublicPtr() = 0;

        /**
         * Runs any actions required to gain controlSession. It is safe to assume
         * #init() has been called before this function executes (visible API
         * does not allow direct calls).
         *
         * @return Pointer to AbstractSensorChannelInterface child,
         *         or NULL on any possible error (error handling is done
         *         based on the return value).
         */
        virtual AbstractSensorChannelInterface* controlSession() = 0;

        /**
         * See #controlSession().
         * @return See #controlSession(). The only difference is the const attribute
         *         on return value. Not necessary, will be cast off by the receiving
         *         function.
         */
        virtual const AbstractSensorChannelInterface* listenSession() = 0;

        /**
         * Setup signals connections for sensor. Bind sensor interface to
         * QmSensor subclass.
         *
         * <b>Internal use only</b>
         *
         * @param setOn Connect signals if true, disconnect signals if false.
         * @return true on success, false on failure.
         */
        virtual bool setupSignals(bool setOn) = 0;

        QmSensor::SessionType sessionType_;
        bool initDone_;

        void setError(QString error);
        QString errorString_;
        bool running_;
    };
    
} // MeeGo namespace

#endif 
