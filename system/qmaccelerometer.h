/*!
 * @file qmaccelerometer.h
 * @brief Contains QmAccelerometer, which provides raw accelerometer measurements.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>

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
#ifndef QMACCELEROMETER_H
#define QMACCELEROMETER_H

#include "system_global.h"
#include <QtCore/qobject.h>
#include <qmsensor.h>

QT_BEGIN_HEADER

namespace MeeGo {

    class QmAccelerometerPrivate;

    /**
     * Accelerometer measurement
     */
    class QmAccelerometerReading : public QmSensorReading
    {
    public:
        int x;
        int y;
        int z;
    };


    /**
     *
     * @scope Internal
     *
     * @brief Provides raw accelerometer measurements.
     *
     * Measured values are in Nokia Coordinate System (NCS). NCS is defined as
     * right hand coordinate system, with positive z-axis pointing up from the
     * screen. Positive axis directions are shown in the image below. Note that
     * regardless of device dimensions, y-axis should always be considered to
     * be pointing front from the viewer, and x-axis to the side. Devices with
     * rotating displays may have different behavior with regards to this rule.
     *
     * The unit of the values is mG, for example 1000 means 1 G.
     *
     * @verbatim


                     +z
                      |
                      |      +y
                      |     /
                      |----/----
                     /| NOKIA  /|
                    //|--/--- / |
                   // | /   //  /
                  //  |/   //  /
                 //   '--------------- +x
                //       //  /
               //       //  /
              /---------/  /
             /    O    /  /
            /         /  /
            ----------  /
            |_________!/


              Bottom

       @endverbatim
     *
     * To get measurements from the daemon, the client must open a
     * session and call start (). Details can be found from documentation
     * of #QmSensor.
     */
    class MEEGO_SYSTEM_EXPORT QmAccelerometer : public QmSensor
    {
        Q_OBJECT;

    public:
        /**
         * Constructor
         * @param parent Parent QObject.
         */
        QmAccelerometer(QObject *parent = 0);

        /**
         * Destructor
         */
        ~QmAccelerometer();

    Q_SIGNALS:
        /**
         * Signals the availability of new measurement data from the sensor.
         * @param data Available measurement data (mG)
         */
        void dataAvailable(const MeeGo::QmAccelerometerReading& data);

    };

} // MeeGo namespace

QT_END_HEADER

#endif
