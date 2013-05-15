/*!
 * @file qmorientation.h
 * @brief Contains QmOrientation, which provides device orientation.

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
#ifndef QMORIENTATION_H
#define QMORIENTATION_H

#include <QtCore/qobject.h>
#include <qmsensor.h>

QT_BEGIN_HEADER

namespace MeeGo {

    class QmOrientationReading; // Defined below

    /**
     * @scope Internal
     *
     * @brief Provides information on device orientation.
     *
     * Orientation is given as the side of the device with the strongest
     * negative acceleration. Threshold value can be used to control the
     * sensitivity of state change. \ref #setThreshold()
     *
     * Device side naming:
     * @verbatim

                          Top


                      ----------
                     /  NOKIA  /|
                    /-------- / |
                   //       //  /
                  //       //  /
          Left   //  Face //  /    Right
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
     * To get measurements and change sensor settings, the client must
     * open a session (and call start() for data). Details can be found
     * from documentation of #QmSensor.
     */
    class MEEGO_SYSTEM_EXPORT QmOrientation : public QmSensor
    {
        Q_OBJECT;
        Q_PROPERTY(QmOrientationReading orientation READ orientation);
        Q_PROPERTY(int threshold READ threshold WRITE setThreshold);

    public:

        /**
         * Enumerated states for possible orientations.
         */
        enum Orientation
        {
            Undefined = 0,   /**< Orientation is unknown. */
            BottomUp,     /**< Device bottom is up */
            BottomDown, /**< Device bottom is down */
            LeftUp,         /**< Device left side is up */
            RightUp,       /**< Device right side is up */
            FaceDown,     /**< Device face is down */
            FaceUp          /**< Device face is up */
        };

        /**
         * Constructor
         * @param parent Parent QObject.
         */
        QmOrientation(QObject *parent = 0);

        /**
         * Destructor
         */
        ~QmOrientation();

        /**
         * Gets the current device orientation.
         * @return Current device orientation
         */
        QmOrientationReading orientation();

        /**
         * Gets the currently used threshold value. Value is in mG. See #setThreshold()
         * @return Current threshold value
         */
        int threshold();

        /**
         * Sets the threshold value for the algorithm. Threshold value controls the
         * sensitivity of orientation change. Value is given in mG. Orientation is
         * considered to have changed whenever measured acceleration towards new
         * orientation is stronger than acceleration towards previous orientation
         * by the amount of threshold.
         *
         * Low threshold provides more sensitivity, high threshold value provides
         * more stability.
         *
         * Current default is 50 mG.
         * @param value Threshold value to set
         */
        void setThreshold(int value);

    Q_SIGNALS:
        /**
         * Sent when the device orientation has changed.
         * @param orientation Current device orientation
         */
        void orientationChanged(const MeeGo::QmOrientationReading orientation);

    };

    /**
     * Orientation measurement
     */
    class QmOrientationReading : public QmSensorReading
    {
    public:
        QmOrientation::Orientation value;
    };

} // MeeGo namespace

QT_END_HEADER

#endif
