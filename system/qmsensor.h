/*!
 * @file qmsensor.h
 * @brief Contains QmSensor

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>

   @scope Internal

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

#ifndef QMSENSOR_H
#define QMSENSOR_H
#include <QtCore/qobject.h>
#include "system_global.h"

QT_BEGIN_HEADER;

namespace MeeGo {
    class QmSensorPrivate;

    /**
     * Basic sensor reading.
     */
    class QmSensorReading
    {
    public:
        quint64 timestamp;
    };

    /**
     * Sensor reading for plain integer value.
     */
    class QmIntReading : public QmSensorReading
    {
    public:
        int value;
    };

    /**
     * @scope Internal
     *
     * @brief Abstract base class for Sensord APIs.
     *
     * QmSensor provides common functions for all sensors served by sensord.
     * To use sensors, client must initiate communication to a server
     * by requesting a session. Then, the client can use start() and stop()
     * to control the dataflow.
     *
     * Data is provided through signals, but the previous measurement is
     * usually accessible through accessor function and/or property. It is
     * worth to note that measurements in server side are done when any
     * client is requesting data from the sensor. The previous measurement
     * will thus always match the last made measurement, regardless of
     * current client's running state. Likewise, if no client is in running state,
     * no measurement is done and the previous measured value may be
     * undefined or outdated.
     *
     * Sample use of sensor class ALSSensor:
     * @code
     * #include <qmals.h>
     * @endcode
     *
     * @code
     * QmALS* als;
     * @endcode

     * @code
     * als = new QmALS();
     * connect(als, SIGNAL(ALSChange(int)), this, SLOT(ALSChanged(int)));
     *
     * if (als->requestSession(QmSensor::SessionTypeListen) == QmSensor::SessionTypeNone) {
     *     qDebug() << "Failed to get listen session:" << als->lastError();
     * }
     * als->start();
     * @endcode
     *
     * @code
     * als->stop();
     * delete als;
     * @endcode
     */
    class MEEGO_SYSTEM_EXPORT QmSensor : public QObject
    {
        Q_OBJECT;
        Q_PROPERTY(QString lastError READ lastError);
        Q_PROPERTY(int interval READ interval WRITE setInterval);
        Q_PROPERTY(bool standbyOverride READ standbyOverride WRITE setStandbyOverride);

    public:

        /** Possible session types for sensor */
        enum SessionType {
            SessionTypeNone,    /**< No open session */
            SessionTypeListen,  /**< Listening session */
            SessionTypeControl  /**< Control session */
        };

        virtual ~QmSensor();

        /**
         * Gets the type of current session.
         *
         * @return Type of current session
         */
        SessionType sessionType();

        /**
         * Verifies that current session is at least of level \c type.
         *
         * @return \c True if same or higher, \c false if lower
         */
        bool verifySessionLevel(QmSensor::SessionType type);

        /**
         * Starts the measurement process.
         *
         * @return \c True on successfull start or already running,
         *        \c false on error
         */
        bool start();

        /**
         * Stops the measurement process.
         *
         * @return \c True on successfull stop or already stopped,
         *        \c false on error
         */
        bool stop();

        /**
         * Returns whether the sensor is in running state.
         *
         * @return \c True for running state, \c false for stopped state
         */
        bool isRunning();

        /**
         * Requests a session for the sensor from sensor daemon. If the
         * requested type of session is not granted, the function will set
         * error and automatically fall back to attempting to gain a session
         * of next type. Order is <code>SessionTypeControl -> SessionTypeListen -> SessionTypeNone</code>.
         *
         * @param type The type of session to request
         * @return Type of the session that was received. If differs from
         *              requested type, an error has been set.
         */
        SessionType requestSession(SessionType type = SessionTypeControl);

        /**
         * Closes an open session by calling stop().
         * @deprecated Deprecated, use stop() instead
         */
        void closeSession();

        /**
         * Gets an explanatory message for previous error. The returned string
         * always corresponds to the previous error that has occurred. Thus,
         * it should be checked immediately after a function has returned
         * a value indicating an error.
         *
         * @return QString containing human readable error description
         */
        QString lastError() const;


        /**
         * Returns the current poll interval request made by this client.
         * @return Current poll interval request
         */
        int interval();

        /**
         * Sets a polling interval request for sensord.
         *
         * @param value Interval value to set in milliseconds
         *
         *
         */
        void setInterval(int value);

        /**
         * Returns the current request of this client for standby override.
         * See #setStandbyOverride for details.
         * @return Current standby override request
         */
        bool standbyOverride();

        /**
         * Sets a request to override sensor standby mode on screen blank event.
         * During normal operation, screen blanking will cause all sensors
         * to stop. Client can override this behavior by setting this property
         * to true.
         * @param value Activate or deactive standby override
         */
        void setStandbyOverride(bool value);

    Q_SIGNALS:
        /**
         * Emitted when an error occurs. See #lastError().
         * @param error Human readable string describing the error
         */
        void errorSignal(QString error);

    protected:
        /**
         * Constructor. This class should not be instantiated.
         */
        QmSensor(QObject *parent);
        MEEGO_DECLARE_PROTECTED(QmSensor);

    };
} // MeeGo namespace

QT_END_HEADER

#endif
