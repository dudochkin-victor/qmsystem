/*!
 * @file qmmagnetometer.h
 * @brief Contains QmMagnetometer, which provides raw magnetometer measurements.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Üstün Ergenoglu <ustun.ergenoglu@digia.com>

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
#ifndef QMMAGNETOMETER_H
#define QMMAGNETOMETER_H

#include <QtCore/qobject.h>
#include <qmsensor.h>

QT_BEGIN_HEADER

namespace MeeGo {

    /**
     * Magnetometer measurement
     */
    class QmMagnetometerReading : public QmSensorReading
    {
    public:
        int x;
        int y;
        int z;
        int rx;
        int ry;
        int rz;
        int level;
    };

    /**
     * @scope Internal
     *
     * @brief Provides raw magnetometer measurements.
     *
     * To get measurements from the daemon, the client must open a
     * session and call start (). Details can be found from documentation
     * of #QmSensor.
     */
    class MEEGO_SYSTEM_EXPORT QmMagnetometer : public QmSensor
    {
        Q_OBJECT;
        Q_PROPERTY(QmMagnetometerReading magneticField READ magneticField);

    public:
        /**
         * Constructor
         * @param parent Parent QObject
         */
        QmMagnetometer(QObject *parent = 0);

        /**
         * Destructor
         */
        ~QmMagnetometer();

        /**
         * Gets the latest measured value for magnetic field in nT (nanotesla).
         * @return Previous measurement
         */
        QmMagnetometerReading magneticField();

        /**
         * Resets the magnetometer calibration back to 0.
         */
        void reset();

    Q_SIGNALS:
        /**
         * Signals the availability of new measurement data from the sensor.
         * @param data Available measurement data in nT (nanotesla)
         */
        void dataAvailable(const MeeGo::QmMagnetometerReading& data);

    };

} // MeeGo namespace

QT_END_HEADER

#endif
