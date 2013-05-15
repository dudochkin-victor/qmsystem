/*!
 * @file qmcompass.h
 * @brief Contains QmCompass, which provides compass measurement.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

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
#ifndef QMCOMPASS_H
#define QMCOMPASS_H
#include <QtCore/qobject.h>
#include "qmsensor.h"

QT_BEGIN_HEADER

namespace MeeGo {
    /**
     * Compass measurement. Provides compass azimuth in relation
     * to device Y-axis (See QmAccelerometer for axis directions).
     * Applications must rotate the measurements themselves
     * if they need data aligned to UI orientation.
     */
    class QmCompassReading : public QmSensorReading
    {
    public:
        int degrees; /**< Compass Azimuth in degrees */
        int level;   /**< Calibration level */
    };

    /**
     * @scope Internal
     *
     * @brief Provides logical compass sensor.
     *
     * Logical compass sensor uses magnetometer and accelerometer to measure
     * current north angle.
     *
     * To get meaningful direction from the compass, it must be calibrated.
     * Calibration level varies from 0 to 3. Only measurements at level 3
     * are accurate. Calibration is done by rotating the device around
     * each axis simultaneously. Rotating one's wrist for 2-3 rounds with
     * the device in hand usually is enough. Fast changes in the magnetic
     * environment may cause the calibration level to drop.
     * 
     * To get measurements and change sensor settings, the client must
     * open a session (and call start() for data). Details can be found
     * from documentation of #QmSensor.
     */
    class MEEGO_SYSTEM_EXPORT QmCompass : public QmSensor
    {
        Q_OBJECT;
        Q_PROPERTY(QmCompassReading value READ get);
        Q_PROPERTY(bool usedeclination READ useDeclination WRITE setUseDeclination);
        Q_PROPERTY(int declinationvalue READ declinationValue);

    public:
        /**
         * Constructor
         * @param parent Parent object
         */
        QmCompass(QObject *parent = 0);

        /**
         * Destructor
         */
        ~QmCompass();

        /**
         * Returns the previous measured compass value.
         * @return Most recently measured compass value
         */
        QmCompassReading get();

        /**
         * Returns the currently used declination correction value.
         * @return Current declination value
         */
        int declinationValue();

        /**
         * Returns whether the sensor is applying declination correction to the 
         * output value and returning <i>true north</i>, or not applying it and 
         * returning <i>magnetic north</i>.
         *
         * @return True if decliation correction is applied, false otherwise
         */
        bool useDeclination();

        /**
         * Sets whether the declination correction should be applied or not.
         * @param enable If true, declination correction will be applied,
         *               if false, it will not be applied
         */
        void setUseDeclination(bool enable);

    Q_SIGNALS:
        /**
         * Signal to notify the listener about change of compass direction,
         * or calibration level.
         * @param value Current compass measurement
         */
        void dataAvailable(const MeeGo::QmCompassReading value);

    };

} // MeeGo namespace

QT_END_HEADER

#endif
