/*!
 * @file qmrotation.h
 * @brief Contains QmRotation, which provides device rotation around axes.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

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
#ifndef QMROTATION_H
#define QMROTATION_H

#include <QtCore/qobject.h>
#include <qmsensor.h>

QT_BEGIN_HEADER

namespace MeeGo {

    /**
     * Rotation measurement
     */
    class QmRotationReading : public QmSensorReading
    {
    public:
        int x;
        int y;
        int z;
    };

    /**
     * @scope Internal
     *
     * @brief Provides device rotation measurements.
     *
     * Device rotation is given in degrees of rotation around each
     * device axis.
     *
     * <ul>
     * <li><b>X-rotation:</b>: [-90, 90]</li>
     * <li><b>Y-rotation:</b>: [-179, 180]</li>
     * <li><b>Z-rotation:</b>: [-179, 180]</li>
     * </ul>
     *
     * X-rotation is limited to half-sphere in order to avoid ambiguous
     * situations. Z-Rotation might not be available on all devices. If
     * not available, the value will be \c 0. Availability can be
     * checked with hasZ() function.
     *
     * Axis directions are best described with image. For further details
     * on axis alignment, see #QmAccelerometer.
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
     * session and call start(). Details can be found from documentation
     * of #QmSensor.
     *
     */
    class MEEGO_SYSTEM_EXPORT QmRotation : public QmSensor
    {
        Q_OBJECT;

    public:
        /**
         * Constructor
         * @param parent Parent QObject.
         */
        QmRotation(QObject *parent = 0);

        /**
         * Destructor
         */
        ~QmRotation();

        /**
         * Gets the previous measured rotation.
         * @return Previous measured rotation, or XYZ(0,0,0,0) if no
         * session is open
         */
        QmRotationReading rotation();

        /**
         * Tells whether the z-axis rotation is calculated. The z-axis
         * calculations rely on having magnetometer/gyroscope on the device,
         * and might not thus be available on all devices.
         * @return Bool \c true if z-axis is calculated, \c false otherwise
         */
        bool hasZ();

    Q_SIGNALS:
        /**
         * Signals the availability of new measurement data from the sensor.
         * @param data Available measurement data
         */
        void dataAvailable(const MeeGo::QmRotationReading& data);

    };

} // MeeGo namespace

QT_END_HEADER

#endif
